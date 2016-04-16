/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <iostream>
#if		qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
#include	<cstdio>
#endif

#include    "Stroika/Foundation/Characters/ToString.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/DataExchange/ArchiveReader.h"
#if     qHasFeature_LZMA
#include    "Stroika/Foundation/DataExchange/7z/Reader.h"
#endif
#if     qHasFeature_ZLib
#include    "Stroika/Foundation/DataExchange/Zip/Reader.h"
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
#if		qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
        (void)::fprintf (stderr, "Usage: ArchiveUtility (--help | -h) | ((--list | --create | --extract |--update) ARCHIVENAME [--outputDirectory D] [FILES])\n");
        (void)::fprintf (stderr, "    --help prints this help\n");
        (void)::fprintf (stderr, "    -h prints this help\n");
        (void)::fprintf (stderr, "    --list prints all the files in the argument archive\n");
        (void)::fprintf (stderr, "    --create creates the argument ARHCIVE and adds the argument FILES to it\n");
        (void)::fprintf (stderr, "    --extract extracts all the files from the argument ARHCIVE and to the output directory specified by --ouptutDirectory (defaulting to .)\n");
        (void)::fprintf (stderr, "    --update adds to the argument ARHCIVE and adds the argument FILES to it\n");
        (void)::fprintf (stderr, "    ARCHIVENAME can be the single character - to designate stdin\n"); // NYI
#else
        cerr << "Usage: ArchiveUtility (--help | -h) | ((--list | --create | --extract |--update) ARCHIVENAME [--outputDirectory D] [FILES])" << endl;
        cerr << "    --help prints this help" << endl;
        cerr << "    -h prints this help" << endl;
        cerr << "    --list prints all the files in the argument archive" << endl;
        cerr << "    --create creates the argument ARHCIVE and adds the argument FILES to it" << endl;
        cerr << "    --extract extracts all the files from the argument ARHCIVE and to the output directory specified by --ouptutDirectory (defaulting to .)" << endl;
        cerr << "    --update adds to the argument ARHCIVE and adds the argument FILES to it" << endl;
        cerr << "    ARCHIVENAME can be the single character - to designate stdin" << endl; // NYI
#endif
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
#if		qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
			(void)::fprintf (stderr, "Missing name of archive\n");
#else
			cerr << "Missing name of archive" << endl;
#endif
            Usage_ ();
            return Optional<Options_> {};
        }
        if (not operation) {
#if		qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
            (void)::fprintf (stderr, "Missing operation\n");
#else
            cerr << "Missing operation" << endl;
#endif
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
    DataExchange::ArchiveReader OpenArchive_ (const String& archiveName)
    {
        // @todo - must support other formats, have a registry, and autodetect
#if     qHasFeature_LZMA
        if (archiveName.EndsWith (L".7z", Characters::CompareOptions::eCaseInsensitive)) {
            return move (_7z::ArchiveReader { IO::FileSystem::FileInputStream { archiveName } });
        }
#endif
#if   qHasFeature_ZLib
        if (archiveName.EndsWith (L".zip", Characters::CompareOptions::eCaseInsensitive)) {
            return move (Zip::ArchiveReader { IO::FileSystem::FileInputStream { archiveName } });
        }
#endif
        Execution::Throw (Execution::StringException (L"Unrecognized format"));
    }

}


namespace {
    void    ListArchive_ (const String& archiveName)
    {
        for (String i : OpenArchive_ (archiveName).GetContainedFiles ()) {
#if		qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
            printf ("%s\n", i.AsNarrowSDKString ().c_str ());
#else
            cout << i.AsNarrowSDKString () << endl;
#endif
        }
    }
    void    ExtractArchive_ (const String& archiveName, const String& toDirectory)
    {
        Debug::TraceContextBumper ctx { L"ExtractArchive_" };
        DbgTrace (L"(archiveName=%s, toDir=%s)", archiveName.c_str (), toDirectory.c_str ());
        DataExchange::ArchiveReader archive { OpenArchive_ (archiveName) };
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
			String	exceptMsg = Characters::ToString (current_exception ());
#if		qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
			(void)::fprintf (stderr, "Exception: %s - terminating...\n", exceptMsg.AsNarrowSDKString ().c_str ());
#else
			cerr << "Exception: " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
#endif
            return EXIT_FAILURE;
        }
    }
    else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
