/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/Archive/Reader.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#if qHasFeature_LZMA
#include "Stroika/Foundation/DataExchange/Archive/7z/Reader.h"
#endif
#if qHasFeature_ZLib
#include "Stroika/Foundation/DataExchange/Archive/Zip/Reader.h"
#endif
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/TextReader.h"

using namespace std;

using namespace Stroika::Foundation;
#if qHasFeature_LZMA || qHasFeature_ZLib
using namespace Stroika::Foundation::DataExchange;
#endif
using namespace Stroika::Foundation::Streams;

using Characters::String;
using Containers::Sequence;
using Memory::BLOB;

namespace {
    struct Options_ {
        enum class Operation {
            eList,
            eExtract,
            eCreate,
            eUpdate
        };
        Operation                  fOperation;
        filesystem::path           fArchiveFileName;
        optional<filesystem::path> fOutputDirectory; // applies only if extract
        optional<Sequence<String>> fFiles2Add;
        optional<bool>             fNoFailOnMissingLibrary; // for regression tests
    };

    using Execution::StandardCommandLineOptions::kHelp;

    const Execution::CommandLine::Option kNoFailOnMissingO_{.fLongName       = "no-fail-on-missing-library"sv,
                                                            .fHelpOptionText = "just warns when we fail because of missing library"sv};
    const Execution::CommandLine::Option kListO_{.fLongName = "list"sv, .fHelpOptionText = "prints all the files in the argument archive"sv};
    const Execution::CommandLine::Option kCreateO_{.fLongName = "create"sv,
                                                   .fHelpOptionText = "creates the argument ARCHIVE-FILE and adds the argument FILE(s) to it"sv};
    const Execution::CommandLine::Option kExtractO_{
        .fLongName = "extract"sv,
        .fHelpOptionText = "extracts all the files from the argument ARCHIVE-FILE and to the output directory specified by --ouptutDirectory "sv};
    const Execution::CommandLine::Option kUpdateO_{.fLongName = "update"sv,
                                                   .fHelpOptionText = "adds to the argument ARCHIVE-FILE and adds the argument FILE(s) to it "sv};
    const Execution::CommandLine::Option kArchiveFileO_{
        .fSupportsArgument = true,
        .fRequired         = true,
        .fHelpArgName      = "ARCHIVE-FILE",
        .fHelpOptionText   = "ARCHIVE-FILE can be the single character - to designate stdin"sv // NYI stdin part...
    };
    const Execution::CommandLine::Option kOtherFilenamesO_{
        .fSupportsArgument = true,
        .fRepeatable       = true,
        .fHelpArgName      = "FILE",
    };
    const Execution::CommandLine::Option kOutputDirO_{.fLongName = "outputDirectory"sv, .fSupportsArgument = true, .fHelpOptionText = "(defaulting to .)"sv};

    const initializer_list<Execution::CommandLine::Option> kAllOptions_{
        kHelp, kNoFailOnMissingO_, kListO_, kCreateO_, kExtractO_, kUpdateO_, kOutputDirO_, kArchiveFileO_, kOtherFilenamesO_,
    };

    void Usage_ ()
    {
        cerr << Execution::CommandLine::GenerateUsage ("ArchiveUtility", kAllOptions_).AsNarrowSDKString ();
    }
    // Emits errors to stderr, and Usage, etc, if needed, and not Optional<> has_value()
    optional<Options_> ParseOptions_ (int argc, const char* argv[])
    {
        Options_               result{};
        Execution::CommandLine cmdLine{argc, argv};

        try {
            cmdLine.Validate (kAllOptions_);
        }
        catch (...) {
            Usage_ ();
            return optional<Options_>{};
        }
        if (cmdLine.Has (kHelp)) {
            Usage_ ();
            return optional<Options_>{};
        }

        if (cmdLine.Has (kListO_)) {
            result.fOperation = Options_::Operation::eList;
        }
        else if (cmdLine.Has (kCreateO_)) {
            result.fOperation = Options_::Operation::eCreate;
        }
        else if (cmdLine.Has (kExtractO_)) {
            result.fOperation = Options_::Operation::eExtract;
        }
        else if (cmdLine.Has (kUpdateO_)) {
            result.fOperation = Options_::Operation::eUpdate;
        }
        else {
            cerr << "Missing operation" << endl;
            Usage_ ();
            return optional<Options_>{};
        }
        result.fArchiveFileName = IO::FileSystem::ToPath (Memory::ValueOf (cmdLine.GetArgument (kArchiveFileO_)));
        if (auto o = cmdLine.GetArgument (kOutputDirO_)) {
            result.fOutputDirectory = IO::FileSystem::ToPath (*o);
        }
        // @todo add more.. - files etc
        result.fNoFailOnMissingLibrary = cmdLine.Has (kNoFailOnMissingO_);
        return result;
    }
}

namespace {
    DataExchange::Archive::Reader OpenArchive_ (const filesystem::path& archiveName)
    {
// @todo - must support other formats, have a registry, and autodetect
#if qHasFeature_LZMA
        if (IO::FileSystem::FromPath (archiveName).EndsWith (".7z"sv, Characters::CompareOptions::eCaseInsensitive)) {
            return move (Archive::_7z::Reader{IO::FileSystem::FileInputStream::New (archiveName)});
        }
#endif
#if qHasFeature_ZLib
        if (IO::FileSystem::FromPath (archiveName).EndsWith (".zip"sv, Characters::CompareOptions::eCaseInsensitive)) {
            return move (Archive::Zip::Reader{IO::FileSystem::FileInputStream::New (archiveName)});
        }
#endif
        Execution::Throw (Execution::Exception{"Unrecognized format"sv});
    }
}

namespace {
    void ListArchive_ (const filesystem::path& archiveName)
    {
        for (String i : OpenArchive_ (archiveName).GetContainedFiles ()) {
            cout << i.AsNarrowSDKString () << endl;
        }
    }
    void ExtractArchive_ (const filesystem::path& archiveName, const filesystem::path& toDirectory)
    {
        Debug::TraceContextBumper ctx{"ExtractArchive_"};
        DbgTrace (L"(archiveName=%s, toDir=%s)", archiveName.c_str (), toDirectory.c_str ());
        DataExchange::Archive::Reader archive{OpenArchive_ (archiveName)};
        for (String i : archive.GetContainedFiles ()) {
            String           srcFileName = i;
            filesystem::path trgFileName = toDirectory / IO::FileSystem::ToPath (srcFileName);
            //DbgTrace (L"(srcFileName=%s, trgFileName=%s)", Characters::ToString (srcFileName).c_str (), Characters::ToString (trgFileName).c_str ());
            BLOB b = archive.GetData (srcFileName);
            //DbgTrace (L"IO::FileSystem::GetFileDirectory (trgFileName)=%s", IO::FileSystem::GetFileDirectory (trgFileName).c_str ());
            create_directories (trgFileName.parent_path ());
            IO::FileSystem::FileOutputStream::Ptr ostream = IO::FileSystem::FileOutputStream::New (trgFileName);
            ostream.Write (b);
        }
    }
}

int main (int argc, const char* argv[])
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
        L"main", L"argv=%s", Characters::ToString (vector<const char*>{argv, argv + argc}).c_str ())};
    if (optional<Options_> o = ParseOptions_ (argc, argv)) {
        try {
            switch (o->fOperation) {
                case Options_::Operation::eList:
                    ListArchive_ (o->fArchiveFileName);
                    break;
                case Options_::Operation::eExtract:
                    ExtractArchive_ (o->fArchiveFileName, o->fOutputDirectory.value_or ("."sv));
                    break;
                default:
                    cerr << "that option NYI" << endl;
                    break;
            }
        }
        catch (Execution::InvalidCommandLineArgument) {
            String exceptMsg = Characters::ToString (current_exception ());
            cerr << "Exception: " << exceptMsg.AsNarrowSDKString () << endl;
            Usage_ ();
            return EXIT_FAILURE;
        }
        catch (...) {
            String exceptMsg = Characters::ToString (current_exception ());
            cerr << "Exception: " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
            if (o->fNoFailOnMissingLibrary.value_or (false)) {
#if !qHasFeature_LZMA || !qHasFeature_ZLib
                if (exceptMsg.Contains ("Unrecognized format"sv)) {
                    return EXIT_SUCCESS;
                }
#endif
            }
            return EXIT_FAILURE;
        }
    }
    else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
