#include    "Stroika/Frameworks/StroikaPreComp.h"

#if     qPlatform_Windows
#include    <winsock2.h>
#endif

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
    void    DoListening_ (Listener* l)
    {
        cout << "Listening..." << endl;
        l->AddOnFoundCallback ([] (const Listener::Result & d) {
            static  mutex       m;      // If the listener impl uses multiple listen threads, prevent display from getting messed up
            lock_guard<mutex> critSection (m);
            cout << "\tFound device (NOTIFY):" << endl;
            cout << "\t\tUSN:      " << d.fUSN.AsUTF8 () << endl;
            cout << "\t\tLocation: " << d.fLocation.AsUTF8 () << endl;

            // INCLUDE FLAG ABOUT ALLIVE

            cout << "\t\tST:       " << d.fST.AsUTF8 () << endl;
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
            static  mutex       m;      // If the listener impl uses multiple listen threads, prevent display from getting messed up
            lock_guard<mutex> critSection (m);
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
            }
        }
    }

#if     qPlatform_Windows
    {
        // Initialize Winsock
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            cerr << "WSAStartup failed " << iResult << endl;
            return 1;
        }
    }
#endif

    // for now - given current API (and not using threads) - do just one or the other
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
        return 1;
    }

    return 0;
}
