/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/Archive/Reader.h"
#include "Stroika/Foundation/Debug/Trace.h"
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
    void Usage_ ()
    {
        cerr << "Usage: ArchiveUtility (--help | -h) | (--no-fail-on-missing-library) | ((--list | --create | --extract |--update) ARCHIVENAME [--outputDirectory D] [FILES])" << endl;
        cerr << "    --help prints this help" << endl;
        cerr << "    -h prints this help" << endl;
        cerr << "    --no-fail-on-missing-library just warns when we fail because of missing library" << endl;
        cerr << "    --list prints all the files in the argument archive" << endl;
        cerr << "    --create creates the argument ARHCIVE and adds the argument FILES to it" << endl;
        cerr << "    --extract extracts all the files from the argument ARHCIVE and to the output directory specified by --ouptutDirectory (defaulting to .)" << endl;
        cerr << "    --update adds to the argument ARHCIVE and adds the argument FILES to it" << endl;
        cerr << "    ARCHIVENAME can be the single character - to designate stdin" << endl; // NYI
    }
    // Emits errors to stderr, and Usage, etc, if needed, and not Optional<> has_value()
    optional<Options_> ParseOptions_ (int argc, const char* argv[])
    {
        Options_                      result{};
        Sequence<String>              args = Execution::ParseCommandLine (argc, argv);
        optional<Options_::Operation> operation;
        optional<String>              archiveName;
        optional<bool>                noFailOnMissingLibrary;
        for (auto argi = args.begin (); argi != args.end (); ++argi) {
            if (argi == args.begin ()) {
                continue; // skip argv[0] - command name
            }
            if (Execution::MatchesCommandLineArgument (*argi, L"h") or Execution::MatchesCommandLineArgument (*argi, L"help")) {
                Usage_ ();
                return optional<Options_>{};
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"no-fail-on-missing-library")) {
                noFailOnMissingLibrary = true;
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"list")) {
                operation = Options_::Operation::eList;
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"create")) {
                operation = Options_::Operation::eCreate;
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"extract")) {
                operation = Options_::Operation::eExtract;
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"update")) {
                operation = Options_::Operation::eUpdate;
            }
            else if (not archiveName.has_value ()) {
                archiveName = *argi;
            }
            // else more cases todo
        }
        if (not archiveName) {
            cerr << "Missing name of archive" << endl;
            Usage_ ();
            return optional<Options_>{};
        }
        if (not operation) {
            cerr << "Missing operation" << endl;
            Usage_ ();
            return optional<Options_>{};
        }
        result.fOperation       = *operation;
        result.fArchiveFileName = IO::FileSystem::ToPath (*archiveName);
        // @todo add more.. - files etc
        result.fNoFailOnMissingLibrary = noFailOnMissingLibrary;
        return result;
    }
}

namespace {
    DataExchange::Archive::Reader OpenArchive_ (const filesystem::path& archiveName)
    {
// @todo - must support other formats, have a registry, and autodetect
#if qHasFeature_LZMA
        if (IO::FileSystem::FromPath (archiveName).EndsWith (L".7z", Characters::CompareOptions::eCaseInsensitive)) {
            return move (Archive::_7z::Reader{IO::FileSystem::FileInputStream::New (archiveName)});
        }
#endif
#if qHasFeature_ZLib
        if (IO::FileSystem::FromPath (archiveName).EndsWith (L".zip", Characters::CompareOptions::eCaseInsensitive)) {
            return move (Archive::Zip::Reader{IO::FileSystem::FileInputStream::New (archiveName)});
        }
#endif
        Execution::Throw (Execution::Exception{L"Unrecognized format"sv});
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
        Debug::TraceContextBumper ctx{L"ExtractArchive_"};
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
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"main", L"argv=%s", Characters::ToString (vector<const char*>{argv, argv + argc}).c_str ())};
    if (optional<Options_> o = ParseOptions_ (argc, argv)) {
        try {
            switch (o->fOperation) {
                case Options_::Operation::eList:
                    ListArchive_ (o->fArchiveFileName);
                    break;
                case Options_::Operation::eExtract:
                    ExtractArchive_ (o->fArchiveFileName, o->fOutputDirectory.value_or (L"."));
                    break;
                default:
                    cerr << "that option NYI" << endl;
                    break;
            }
        }
        catch (...) {
            String exceptMsg = Characters::ToString (current_exception ());
            cerr << "Exception: " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
            if (o->fNoFailOnMissingLibrary.value_or (false)) {
#if !qHasFeature_LZMA || !qHasFeature_ZLib
                if (exceptMsg.Contains (L"Unrecognized format"sv)) {
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
