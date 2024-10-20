/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>
#include <mutex>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"

#include "Stroika/Frameworks/UPnP/DeviceDescription.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/Listener.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/Search.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;

using Client::Listener;
using Client::Search;
using Containers::Sequence;

namespace {
    mutex kStdOutMutex_; // If the listener impl uses multiple listen threads, prevent display from getting messed up
}

namespace {
    // Ignore if fails
    void DoPrintDeviceDescription_ (const URI& deviceDescriptionURL)
    {
        try {
            using namespace IO::Network::Transfer;
            Connection::Ptr c = Connection::New ();
            Response        r = c.GET (deviceDescriptionURL);
            if (r.GetSucceeded ()) {
                DeviceDescription deviceInfo = DeSerialize (r.GetData ());
                cout << "\t\tDevice-Decsciption: " << Characters::ToString (deviceInfo).AsNarrowSDKString () << endl;
            }
        }
        catch (...) {
            DbgTrace ("failed to fetch description: {}"_f, Characters::ToString (current_exception ()));
        }
    }
}

namespace {
    void DoListening_ (Listener* l)
    {
        cout << "Listening..." << endl;
        l->AddOnFoundCallback ([] (const SSDP::Advertisement& d) {
            lock_guard<mutex> critSection{kStdOutMutex_};
            cout << "\tFound device (NOTIFY):" << endl;
            cout << "\t\tUSN:      " << d.fUSN.AsUTF8<string> () << endl;
            if (d.fAlive.has_value ()) {
                cout << "\t\tAlive:    " << Characters::ToString (d.fAlive).AsUTF8<string> () << endl;
            }
            cout << "\t\tST:       " << d.fTarget.AsUTF8<string> () << endl;
            cout << "\t\tLocation: " << Characters::ToString (d.fLocation).AsUTF8<string> () << endl;
            if (not d.fServer.empty ()) {
                cout << "\t\tServer:   " << d.fServer.AsUTF8<string> () << endl;
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
        cout << "Searching for '" << searchFor.AsUTF8<string> () << "'..." << endl;
        searcher->AddOnFoundCallback ([] (const SSDP::Advertisement& d) {
            lock_guard<mutex> critSection{kStdOutMutex_};
            cout << "\tFound device (MATCHED SEARCH):" << endl;
            cout << "\t\tUSN:      " << d.fUSN.AsUTF8<string> () << endl;
            cout << "\t\tLocation: " << Characters::ToString (d.fLocation).AsUTF8<string> () << endl;
            cout << "\t\tST:       " << d.fTarget.AsUTF8<string> () << endl;
            if (not d.fServer.empty ()) {
                cout << "\t\tServer:   " << d.fServer.AsUTF8<string> () << endl;
            }
            DoPrintDeviceDescription_ (d.fLocation);
            cout << endl;
        });
        searcher->Start (searchFor);
    }
}

int main (int argc, const char* argv[])
{
    Debug::TraceContextBumper ctx{
        Stroika_Foundation_Debug_OptionalizeTraceArgs ("main", "argv={}"_f, Characters::ToString (vector<const char*>{argv, argv + argc}))};
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    bool                  listen = false;
    optional<String>      searchFor;
    Time::DurationSeconds quitAfter = Time::kInfinity;

    const Execution::CommandLine::Option kListenO_{
        .fSingleCharName = 'l',
    };
    const Execution::CommandLine::Option kSearchO_{
        .fSingleCharName = 's', .fSupportsArgument = true, .fHelpArgName = "SEARCHFOR"sv, .fHelpOptionText = "Search for the argument UPNP name"sv};
    const Execution::CommandLine::Option kQuitAfterO_{.fLongName = "quit-after"sv, .fSupportsArgument = true, .fHelpArgName = "NSECONDS"sv};

    Execution::CommandLine cmdLine{argc, argv};
    listen    = cmdLine.Has (kListenO_);
    searchFor = cmdLine.GetArgument (kSearchO_);
    if (auto o = cmdLine.GetArgument (kQuitAfterO_)) {
        quitAfter = Time::DurationSeconds{Characters::FloatConversion::ToFloat<Time::DurationSeconds::rep> (*o)};
    }

    if (not listen and not searchFor.has_value ()) {
        cerr << "Usage: SSDPClient [-l] [-s SEARCHFOR] [--quit-after N]" << endl;
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
            Execution::WaitableEvent{}.Wait (quitAfter); // wait quitAfter seconds, or til user hits ctrl-c
        }
        else {
            cerr << "Specify -l to listen or -s STRING to search" << endl;
            return EXIT_FAILURE;
        }
    }
    catch (const Execution::TimeOutException&) {
        cerr << "Timed out - so - exiting..." << endl;
        return EXIT_SUCCESS;
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
        cerr << "Exception - " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
