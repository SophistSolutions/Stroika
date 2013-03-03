/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <mutex>
#include    <iostream>

#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/Event.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Frameworks/UPnP/SSDP/Client/Listener.h"
#include    "Stroika/Frameworks/UPnP/SSDP/Client/Search.h"

using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Frameworks::UPnP;
using   namespace Stroika::Frameworks::UPnP::SSDP;

using   Characters::String;
using   Memory::Optional;
using   Client::Listener;
using   Client::Search;


namespace {
    mutex       kStdOutMutex_;      // If the listener impl uses multiple listen threads, prevent display from getting messed up
}

namespace {
    void    DoListening_ (Listener* l)
    {
        cout << "Listening..." << endl;
        l->AddOnFoundCallback ([] (const Listener::Result & d) {
            lock_guard<mutex> critSection (kStdOutMutex_);
            cout << "\tFound device (NOTIFY):" << endl;
            cout << "\t\tUSN:      " << d.fUSN.AsUTF8 () << endl;
            if (not d.fAlive.empty ()) {
                cout << "\t\tAlive:    " << (*d.fAlive ? "true" : "false") << endl;
            }
            cout << "\t\tST:       " << d.fST.AsUTF8 () << endl;
            cout << "\t\tLocation: " << d.fLocation.AsUTF8 () << endl;
            if (not d.fServer.empty ()) {
                cout << "\t\tServer:   " << d.fServer.AsUTF8 () << endl;
            }
            cout << endl;
        });
        l->Start ();
    }
}

namespace {
    void    DoSearching_ (Search* searcher, const String& searchFor)
    {
        cout << "Searching for '" << searchFor.AsUTF8 () << "'..." << endl;
        searcher->AddOnFoundCallback ([] (const Search::Result & d) {
            lock_guard<mutex> critSection (kStdOutMutex_);
            cout << "\tFound device (MATCHED SEARCH):" << endl;
            cout << "\t\tUSN:      " << d.fUSN.AsUTF8 () << endl;
            cout << "\t\tLocation: " << d.fLocation.AsUTF8 () << endl;
            cout << "\t\tST:       " << d.fST.AsUTF8 () << endl;
            if (not d.fServer.empty ()) {
                cout << "\t\tServer:   " << d.fServer.AsUTF8 () << endl;
            }
            cout << endl;
        });
        searcher->Start (searchFor);
    }
}




int main (int argc, const char* argv[])
{
    bool    listen  =   false;
    Optional<String>    searchFor;

    vector<String>  args    =   Execution::ParseCommandLine (argc, argv);
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
                cerr << "Expected arg to -s" << endl;
                return EXIT_FAILURE;
            }
        }
    }
    if (not listen and searchFor.empty ()) {
        cerr << "Usage: SSDPClient [-l] [-s SEARCHFOR]" << endl;
        return EXIT_FAILURE;
    }

    try {
        Listener l;
        if (listen) {
            DoListening_ (&l);
        }
        Search  s;
        if (not searchFor.empty ()) {
            DoSearching_ (&s, *searchFor);
        }

        if (listen or not searchFor.empty ()) {
            Execution::Event ().Wait ();    // wait forever - til user hits ctrl-c
        }
        else {
            cerr << "Specify -l to listen or -s STRING to search" << endl;
            return EXIT_FAILURE;
        }
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
