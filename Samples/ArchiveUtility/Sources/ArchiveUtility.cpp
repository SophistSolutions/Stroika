/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/Archive/Reader.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#if qStroika_HasComponent_LZMA
#include "Stroika/Foundation/DataExchange/Archive/7z/Reader.h"
#endif
#if qStroika_HasComponent_zlib
#include "Stroika/Foundation/DataExchange/Archive/Zip/Reader.h"
#include "Stroika/Foundation/DataExchange/Archive/Zip/Writer.h"
#endif
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
#if qStroika_HasComponent_LZMA || qStroika_HasComponent_zlib
using namespace Stroika::Foundation::DataExchange;
#endif
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Streams;

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

    using StandardCommandLineOptions::kHelp;

    const CommandLine::Option kNoFailOnMissingO_{.fLongName       = "no-fail-on-missing-library"sv,
                                                 .fHelpOptionText = "just warns when we fail because of missing library"sv};
    const CommandLine::Option kListO_{.fLongName = "list"sv, .fHelpOptionText = "prints all the files in the argument archive"sv};
    const CommandLine::Option kCreateO_{.fLongName = "create"sv, .fHelpOptionText = "creates the argument ARCHIVE-FILE and adds the argument FILE(s) to it"sv};
    const CommandLine::Option kExtractO_{
        .fLongName = "extract"sv,
        .fHelpOptionText = "extracts all the files from the argument ARCHIVE-FILE and to the output directory specified by --ouptutDirectory "sv};
    const CommandLine::Option kUpdateO_{.fLongName       = "update"sv,
                                        .fHelpOptionText = "adds to the argument ARCHIVE-FILE and adds the argument FILE(s) to it "sv};
    const CommandLine::Option kArchiveFileO_{
        .fRequired       = true,
        .fHelpArgName    = "ARCHIVE-FILE"sv,
        .fHelpOptionText = "ARCHIVE-FILE can be the single character - to designate stdin"sv // NYI stdin part...
    };
    const CommandLine::Option kOtherFilenamesO_{
        .fRepeatable          = true,
        .fSkipFirstNArguments = 1,
        .fHelpArgName         = "FILE"sv,
    };
    const CommandLine::Option kOutputDirO_{.fLongName = "outputDirectory"sv, .fSupportsArgument = true, .fHelpOptionText = "(defaulting to .)"sv};

    const initializer_list<CommandLine::Option> kAllOptions_{
        kHelp, kNoFailOnMissingO_, kListO_, kCreateO_, kExtractO_, kUpdateO_, kOutputDirO_, kArchiveFileO_, kOtherFilenamesO_,
    };

    // Emits errors to stderr, and Usage, etc, if needed, and not Optional<> has_value()
    optional<Options_> ParseOptions_ (int argc, const char* argv[])
    {
        Options_    result{};
        CommandLine cmdLine{argc, argv};

        try {
            cmdLine.Validate (kAllOptions_);
        }
        catch (...) {
            cerr << ToString (current_exception ()) << endl;
            cerr << cmdLine.GenerateUsage (kAllOptions_);
            return optional<Options_>{};
        }
        if (cmdLine.Has (kHelp)) {
            cerr << cmdLine.GenerateUsage (kAllOptions_);
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
            cerr << cmdLine.GenerateUsage (kAllOptions_);
            return optional<Options_>{};
        }
        result.fArchiveFileName = Memory::ValueOf (cmdLine.GetArgument (kArchiveFileO_)).As<filesystem::path> ();
        result.fFiles2Add       = cmdLine.GetArguments (kOtherFilenamesO_);
        if (auto o = cmdLine.GetArgument (kOutputDirO_)) {
            result.fOutputDirectory = o->As<filesystem::path> ();
        }
        // @todo add more.. - update support etc...
        result.fNoFailOnMissingLibrary = cmdLine.Has (kNoFailOnMissingO_);
        return result;
    }
}

namespace {
    DataExchange::Archive::Reader::Ptr OpenArchive_ (const filesystem::path& archiveName)
    {
// @todo - must support other formats, have a registry, and autodetect
#if qStroika_HasComponent_LZMA
        if (String{archiveName}.EndsWith (".7z"sv, Characters::eCaseInsensitive)) {
            return Archive::_7z::Reader::New (IO::FileSystem::FileInputStream::New (archiveName));
        }
#endif
#if qStroika_HasComponent_zlib
        if (String{archiveName}.EndsWith (".zip"sv, Characters::eCaseInsensitive)) {
            return Archive::Zip::Reader::New (IO::FileSystem::FileInputStream::New (archiveName));
        }
#endif
        Throw (Exception{"Unrecognized format"sv});
    }
}

namespace {
    DataExchange::Archive::Writer::Ptr CreateWritingArchive_ (const filesystem::path& archiveName)
    {
#if qStroika_HasComponent_zlib
        // for now require just zip
        if (String{archiveName}.EndsWith (".zip"sv, Characters::eCaseInsensitive)) {
            return DataExchange::Archive::Zip::Writer::New (IO::FileSystem::FileOutputStream::New (archiveName));
        }
#endif
        Throw (Exception{"Unrecognized format"sv});
    }
}

namespace {
    void ListArchive_ (const filesystem::path& archiveName)
    {
        for (String i : OpenArchive_ (archiveName).GetContainedFiles ()) {
            cout << i << endl;
        }
    }
    void ExtractArchive_ (const filesystem::path& archiveName, const filesystem::path& toDirectory)
    {
        Debug::TraceContextBumper ctx{"ExtractArchive_", Stroika_Foundation_Debug_OptionalizeTraceArgs("(archiveName={}, toDir={})"_f, archiveName, toDirectory)};
        DataExchange::Archive::Reader::Ptr archive{OpenArchive_ (archiveName)};
        for (String i : archive.GetContainedFiles ()) {
            String           srcFileName = i;
            filesystem::path trgFileName = toDirectory / srcFileName.As<filesystem::path> ();
            //DbgTrace ("(srcFileName={}, trgFileName={})"_f, srcFileName, trgFileName);
            BLOB b = archive.GetData (srcFileName);
            //DbgTrace (L"IO::FileSystem::GetFileDirectory (trgFileName)=%s", IO::FileSystem::GetFileDirectory (trgFileName).c_str ());
            create_directories (trgFileName.parent_path ());
            IO::FileSystem::FileOutputStream::Ptr ostream = IO::FileSystem::FileOutputStream::New (trgFileName);
            ostream.Write (b);
        }
    }
    void CreateArchive_ (const filesystem::path& archiveName, const Sequence<String>& files2Add)
    {
        Debug::TraceContextBumper          ctx{"CreateArchive_",
                                      Stroika_Foundation_Debug_OptionalizeTraceArgs ("(archiveName={}, files2Add={})"_f, archiveName, files2Add)};
        DataExchange::Archive::Writer::Ptr archive = CreateWritingArchive_ (archiveName);
        for (String f2a : files2Add) {
            archive.Add (f2a, IO::FileSystem::FileInputStream::New (f2a.As<filesystem::path> ()).ReadAll ());
        }
    }
}

int main (int argc, const char* argv[])
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("main", "argv={}"_f, CommandLine{argc, argv})};
    if (optional<Options_> o = ParseOptions_ (argc, argv)) {
        try {
            switch (o->fOperation) {
                case Options_::Operation::eList:
                    ListArchive_ (o->fArchiveFileName);
                    break;
                case Options_::Operation::eExtract:
                    ExtractArchive_ (o->fArchiveFileName, o->fOutputDirectory.value_or ("."sv));
                    break;
                case Options_::Operation::eCreate:
                    CreateArchive_ (o->fArchiveFileName, o->fFiles2Add.value_or (Sequence<String>{}));
                    break;
                default:
                    cerr << "that option NYI" << endl;
                    break;
            }
        }
        catch (const InvalidCommandLineArgument&) {
            cerr << "Exception: " << Characters::ToString (current_exception ()) << endl;
            return EXIT_FAILURE;
        }
        catch (...) {
            String exceptMsg = Characters::ToString (current_exception ());
            cerr << "Exception: " << exceptMsg << " - terminating..." << endl;
            if (o->fNoFailOnMissingLibrary.value_or (false)) {
#if !qStroika_HasComponent_LZMA || !qStroika_HasComponent_zlib
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
