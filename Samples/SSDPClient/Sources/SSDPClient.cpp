/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <mutex>
#include    <iostream>
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
#include    <cstdio>
#endif

#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/WaitableEvent.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Frameworks/UPnP/SSDP/Client/Listener.h"
#include    "Stroika/Frameworks/UPnP/SSDP/Client/Search.h"

using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Frameworks::UPnP;
using   namespace Stroika::Frameworks::UPnP::SSDP;

using   Characters::String;
using   Containers::Sequence;
using   Memory::Optional;
using   Client::Listener;
using   Client::Search;


namespace {
    mutex       kStdOutMutex_;      // If the listener impl uses multiple listen threads, prevent display from getting messed up
}

namespace {
    void    DoListening_ (Listener* l)
    {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
        (void)::fprintf (stderr, "Listening...\n");
#else
        cout << "Listening..." << endl;
#endif
        l->AddOnFoundCallback ([](const SSDP::Advertisement & d) {
            lock_guard<mutex> critSection (kStdOutMutex_);
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
            (void)::printf ("\tFound device (NOTIFY):\n");
            (void)::printf ("\t\tUSN:      %s\n", d.fUSN.AsUTF8 ().c_str ());
            if (d.fAlive.IsPresent ()) {
                (void)::printf ("\t\tAlive:    %s\n", (*d.fAlive ? "true" : "false"));
            }
            (void)::printf ("\t\tST:       %s\n", d.fTarget.AsUTF8 ().c_str ());
            (void)::printf ("\t\tLocation: %s\n", d.fLocation.AsUTF8 ().c_str ());
            if (not d.fServer.empty ()) {
                (void)::printf ("\t\tServer:   %s\n", d.fServer.AsUTF8 ().c_str ());
            }
            (void)::printf ("\n");
#else
            cout << "\tFound device (NOTIFY):" << endl;
            cout << "\t\tUSN:      " << d.fUSN.AsUTF8 () << endl;
            if (d.fAlive.IsPresent ()) {
                cout << "\t\tAlive:    " << (*d.fAlive ? "true" : "false") << endl;
            }
            cout << "\t\tST:       " << d.fTarget.AsUTF8 () << endl;
            cout << "\t\tLocation: " << d.fLocation.AsUTF8 () << endl;
            if (not d.fServer.empty ()) {
                cout << "\t\tServer:   " << d.fServer.AsUTF8 () << endl;
            }
            cout << endl;
#endif
        });
        l->Start ();
    }
}

namespace {
    void    DoSearching_ (Search* searcher, const String& searchFor)
    {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
        (void)::printf ("Searching for '%s...\n", searchFor.AsUTF8 ().c_str ());
#else
        cout << "Searching for '" << searchFor.AsUTF8 () << "'..." << endl;
#endif
        searcher->AddOnFoundCallback ([](const SSDP::Advertisement & d) {
            lock_guard<mutex> critSection (kStdOutMutex_);
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
            (void)::printf ("\tFound device (MATCHED SEARCH):\n");
            (void)::printf ("\t\tUSN:      %s\n", d.fUSN.AsUTF8 ().c_str ());
            (void)::printf ("\t\tLocation: %s\n", d.fLocation.AsUTF8 ().c_str ());
            (void)::printf ("\t\tST:       %s\n", d.fTarget.AsUTF8 ().c_str ());
            if (not d.fServer.empty ()) {
                (void)::printf ("\t\tServer:   %s\n", d.fServer.AsUTF8 ().c_str ());
            }
            (void)::printf ("\n");
#else
            cout << "\tFound device (MATCHED SEARCH):" << endl;
            cout << "\t\tUSN:      " << d.fUSN.AsUTF8 () << endl;
            cout << "\t\tLocation: " << d.fLocation.AsUTF8 () << endl;
            cout << "\t\tST:       " << d.fTarget.AsUTF8 () << endl;
            if (not d.fServer.empty ()) {
                cout << "\t\tServer:   " << d.fServer.AsUTF8 () << endl;
            }
            cout << endl;
#endif
        });
        searcher->Start (searchFor);
    }
}




int main (int argc, const char* argv[])
{
    bool    listen  =   false;
    Optional<String>    searchFor;

    Sequence<String>  args    =   Execution::ParseCommandLine (argc, argv);
    for (auto argi = args.begin (); argi != args.end(); ++argi) {
        if (Execution::MatchesCommandLineArgument (*argi, L"l")) {
            listen = true;
        }
        if (Execution::MatchesCommandLineArgument (*argi, L"s")) {
            ++argi;
            if (argi != args.end ()) {
                searchFor = *argi;
            }
            else {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
                (void)::fprintf (stderr, "Expected arg to -s\n");
#else
                cerr << "Expected arg to -s" << endl;
#endif
                return EXIT_FAILURE;
            }
        }
    }
    if (not listen and searchFor.IsMissing ()) {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
        (void)::fprintf (stderr, "Usage: SSDPClient [-l] [-s SEARCHFOR]\n");
#else
        cerr << "Usage: SSDPClient [-l] [-s SEARCHFOR]" << endl;
#endif
        return EXIT_FAILURE;
    }

    try {
        Listener l;
        if (listen) {
            DoListening_ (&l);
        }
        Search  s;
        if (searchFor.IsPresent ()) {
            DoSearching_ (&s, *searchFor);
        }

        if (listen or searchFor.IsPresent ()) {
            using   Execution::WaitableEvent;
            WaitableEvent (WaitableEvent::eAutoReset).Wait ();    // wait forever - til user hits ctrl-c
        }
        else {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
            (void)::fprintf (stderr, "Specify -l to listen or -s STRING to search\n");
#else
            cerr << "Specify -l to listen or -s STRING to search" << endl;
#endif
            return EXIT_FAILURE;
        }
    }
    catch (...) {
        String  exceptMsg = Characters::ToString (current_exception ());
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
        (void)::fprintf (stderr, "Exception - %s - terminating...\n", exceptMsg.AsNarrowSDKString ().c_str());
#else
        cerr << "Exception - " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
#endif
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
