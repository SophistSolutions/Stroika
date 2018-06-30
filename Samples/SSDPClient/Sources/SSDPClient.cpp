/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>
#include <mutex>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/Transfer/Client.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "Stroika/Frameworks/UPnP/DeviceDescription.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/Listener.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/Search.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;

using Characters::String;
using Client::Listener;
using Client::Search;
using Containers::Sequence;
using Memory::Optional;

namespace {
    mutex kStdOutMutex_; // If the listener impl uses multiple listen threads, prevent display from getting messed up
}

namespace {
    // Ignore if fails
    void DoPrintDeviceDescription_ (const URL& deviceDescriptionURL)
    {
        try {
            IO::Network::Transfer::Connection c = IO::Network::Transfer::CreateConnection ();
            c.SetURL (deviceDescriptionURL);
            IO::Network::Transfer::Response r = c.GET ();
            if (r.GetSucceeded ()) {
                DeviceDescription deviceInfo = DeSerialize (r.GetData ());
                cout << "\t\tDevice-Decsciption: " << Characters::ToString (deviceInfo).AsNarrowSDKString () << endl;
            }
        }
        catch (...) {
            DbgTrace (L"failed to fetch description: %s", Characters::ToString (current_exception ()).c_str ());
        }
    }
}

namespace {
    void DoListening_ (Listener* l)
    {
        cout << "Listening..." << endl;
        l->AddOnFoundCallback ([](const SSDP::Advertisement& d) {
            lock_guard<mutex> critSection (kStdOutMutex_);
            cout << "\tFound device (NOTIFY):" << endl;
            cout << "\t\tUSN:      " << d.fUSN.AsUTF8 () << endl;
            if (d.fAlive.has_value ()) {
                cout << "\t\tAlive:    " << (*d.fAlive ? "true" : "false") << endl;
            }
            cout << "\t\tST:       " << d.fTarget.AsUTF8 () << endl;
            cout << "\t\tLocation: " << Characters::ToString (d.fLocation).AsUTF8 () << endl;
            if (not d.fServer.empty ()) {
                cout << "\t\tServer:   " << d.fServer.AsUTF8 () << endl;
            }
            DoPrintDeviceDescription_ (d.fLocation);
            cout << endl;
        });
        l->Start ();
    }
}

namespace {
    void DoSearching_ (Search* searcher, const String& searchFor)
    {
        cout << "Searching for '" << searchFor.AsUTF8 () << "'..." << endl;
        searcher->AddOnFoundCallback ([](const SSDP::Advertisement& d) {
            lock_guard<mutex> critSection (kStdOutMutex_);
            cout << "\tFound device (MATCHED SEARCH):" << endl;
            cout << "\t\tUSN:      " << d.fUSN.AsUTF8 () << endl;
            cout << "\t\tLocation: " << Characters::ToString (d.fLocation).AsUTF8 () << endl;
            cout << "\t\tST:       " << d.fTarget.AsUTF8 () << endl;
            if (not d.fServer.empty ()) {
                cout << "\t\tServer:   " << d.fServer.AsUTF8 () << endl;
            }
            DoPrintDeviceDescription_ (d.fLocation);
            cout << endl;
        });
        searcher->Start (searchFor);
    }
}

int main (int argc, const char* argv[])
{
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    bool             listen = false;
    Optional<String> searchFor;

    Sequence<String> args = Execution::ParseCommandLine (argc, argv);
    for (auto argi = args.begin (); argi != args.end (); ++argi) {
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
    if (not listen and not searchFor.has_value ()) {
        cerr << "Usage: SSDPClient [-l] [-s SEARCHFOR]" << endl;
        cerr << "   e.g. SSDPClient -l" << endl;
        cerr << "   e.g. SSDPClient -s \"upnp:rootdevice\"" << endl;
        return EXIT_FAILURE;
    }

    try {
        Listener l;
        if (listen) {
            DoListening_ (&l);
        }
        Search s;
        if (searchFor.has_value ()) {
            DoSearching_ (&s, *searchFor);
        }

        if (listen or searchFor.has_value ()) {
            Execution::WaitableEvent{}.Wait (); // wait forever - til user hits ctrl-c
        }
        else {
            cerr << "Specify -l to listen or -s STRING to search" << endl;
            return EXIT_FAILURE;
        }
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
        cerr << "Exception - " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
