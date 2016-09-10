/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <iostream>

#include    "Stroika/Foundation/Characters/ToString.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/DataExchange/Archive/Reader.h"
#if     qHasFeature_LZMA
#include    "Stroika/Foundation/DataExchange/Archive/7z/Reader.h"
#endif
#if     qHasFeature_ZLib
#include    "Stroika/Foundation/DataExchange/Archive/Zip/Reader.h"
#endif
#include    "Stroika/Foundation/IO/FileSystem/Directory.h"
#include    "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include    "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#include    "Stroika/Foundation/IO/FileSystem/PathName.h"
#include    "Stroika/Foundation/Streams/MemoryStream.h"
#include    "Stroika/Foundation/Streams/TextReader.h"

using   namespace std;

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Streams;


using   Characters::String;
using   Containers::Sequence;
using   Memory::BLOB;
using   Memory::Byte;
using   Memory::Optional;




namespace {
    struct  Options_ {
        enum class Operation { eList, eExtract, eCreate, eUpdate };
        Operation                   fOperation;
        String                      fArchiveFileName;
        Optional<String>            fOutputDirectory;   // applies only if extract
        Optional<Sequence<String>>  fFiles2Add;
    };
    void    Usage_ ()
    {
        cerr << "Usage: ArchiveUtility (--help | -h) | ((--list | --create | --extract |--update) ARCHIVENAME [--outputDirectory D] [FILES])" << endl;
        cerr << "    --help prints this help" << endl;
        cerr << "    -h prints this help" << endl;
        cerr << "    --list prints all the files in the argument archive" << endl;
        cerr << "    --create creates the argument ARHCIVE and adds the argument FILES to it" << endl;
        cerr << "    --extract extracts all the files from the argument ARHCIVE and to the output directory specified by --ouptutDirectory (defaulting to .)" << endl;
        cerr << "    --update adds to the argument ARHCIVE and adds the argument FILES to it" << endl;
        cerr << "    ARCHIVENAME can be the single character - to designate stdin" << endl; // NYI
    }
    // Emits errors to stderr, and Usage, etc, if needed, and Optional<> IsMissing()
    Optional<Options_>  ParseOptions_ (int argc, const char* argv[])
    {
        Options_    result {};
        Sequence<String>  args    =   Execution::ParseCommandLine (argc, argv);
        Optional<Options_::Operation>   operation;
        Optional<String>    archiveName;
        for (auto argi = args.begin (); argi != args.end(); ++argi) {
            if (argi == args.begin ()) {
                continue;   // skip argv[0] - command name
            }
            if (Execution::MatchesCommandLineArgument (*argi, L"h") or Execution::MatchesCommandLineArgument (*argi, L"help")) {
                Usage_ ();
                return Optional<Options_> {};
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
            else if (archiveName.IsMissing ()) {
                archiveName = *argi;
            }
            // else more cases todo
        }
        if (not archiveName) {
            cerr << "Missing name of archive" << endl;
            Usage_ ();
            return Optional<Options_> {};
        }
        if (not operation) {
            cerr << "Missing operation" << endl;
            Usage_ ();
            return Optional<Options_> {};
        }
        result.fOperation = *operation;
        result.fArchiveFileName = *archiveName;
        // @todo add more.. - files etc
        return result;
    }
}


namespace {
    DataExchange::Archive::Reader OpenArchive_ (const String& archiveName)
    {
        // @todo - must support other formats, have a registry, and autodetect
#if     qHasFeature_LZMA
        if (archiveName.EndsWith (L".7z", Characters::CompareOptions::eCaseInsensitive)) {
            return move (Archive::_7z::Reader { IO::FileSystem::FileInputStream { archiveName } });
        }
#endif
#if   qHasFeature_ZLib
        if (archiveName.EndsWith (L".zip", Characters::CompareOptions::eCaseInsensitive)) {
            return move (Archive::Zip::Reader { IO::FileSystem::FileInputStream { archiveName } });
        }
#endif
        Execution::Throw (Execution::StringException (L"Unrecognized format"));
    }

}


namespace {
    void    ListArchive_ (const String& archiveName)
    {
        for (String i : OpenArchive_ (archiveName).GetContainedFiles ()) {
            cout << i.AsNarrowSDKString () << endl;
        }
    }
    void    ExtractArchive_ (const String& archiveName, const String& toDirectory)
    {
        Debug::TraceContextBumper ctx { L"ExtractArchive_" };
        DbgTrace (L"(archiveName=%s, toDir=%s)", archiveName.c_str (), toDirectory.c_str ());
        DataExchange::Archive::Reader archive { OpenArchive_ (archiveName) };
        for (String i : archive.GetContainedFiles ()) {
            String  srcFileName =   i;
            String  trgFileName =   toDirectory + L"/" + srcFileName;

            //tmphac
#if     qPlatform_Windows
            trgFileName = trgFileName.ReplaceAll (L"/", L"\\");
#endif

            //DbgTrace (L"(srcFileName=%s, trgFileName=%s)", srcFileName.c_str (), trgFileName.c_str ());
            BLOB    b           =   archive.GetData (srcFileName);
            //DbgTrace (L"IO::FileSystem::GetFileDirectory (trgFileName)=%s", IO::FileSystem::GetFileDirectory (trgFileName).c_str ());
            IO::FileSystem::Directory { IO::FileSystem::GetFileDirectory (trgFileName) } .AssureExists ();
            IO::FileSystem::FileOutputStream ostream  { trgFileName };
            ostream.Write (b);
        }
    }
}







int     main (int argc, const char* argv[])
{
    if (Optional<Options_> o = ParseOptions_ (argc, argv)) {
        try {
            switch (o->fOperation) {
                case Options_::Operation::eList:
                    ListArchive_ (o->fArchiveFileName);
                    break;
                case Options_::Operation::eExtract:
                    ExtractArchive_ (o->fArchiveFileName, o->fOutputDirectory.Value (L"."));
                    break;
            }
            // rest NYI
        }
        catch (...) {
            String  exceptMsg = Characters::ToString (current_exception ());
            cerr << "Exception: " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
            return EXIT_FAILURE;
        }
    }
    else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
