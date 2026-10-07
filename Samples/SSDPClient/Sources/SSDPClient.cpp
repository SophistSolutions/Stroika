/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>
#include <mutex>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/DataExchange/TypedBLOB.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"

#include "Stroika/Frameworks/UPnP/DeviceDescription.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/Listener.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/Search.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;

using Client::Listener;
using Client::Search;
using Containers::Sequence;

namespace {
    mutex kStdOutMutex_; // If the listener impl uses multiple listen threads, prevent display from getting messed up
}

namespace {
    // a light that can be switched on and off: the UPnP Forum's standard SwitchPower service (as the SSDPServer sample has)
    const String kSwitchPowerServiceType_{"urn:schemas-upnp-org:service:SwitchPower:1"sv};

    // switch the light controlled at controlURL on or off: its SwitchPower service's SetTarget action - a SOAP request, saying which
    // action in its SOAPACTION header (UPnP Device Architecture 1.1, section 3.2)
    void SwitchLight_ (const URI& controlURL, bool on)
    {
        try {
            using namespace IO::Network::Transfer;
            const string request =
                "<?xml version=\"1.0\"?>\r\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetTarget xmlns:u=\"{}\">"
                "<newTargetValue>{}</newTargetValue></u:SetTarget></s:Body></s:Envelope>\r\n"_f(kSwitchPowerServiceType_, on ? 1 : 0)
                    .AsUTF8<string> ();
            Connection::New ().POST (controlURL,
                                     DataExchange::TypedBLOB{.fData = as_bytes (span{request}),
                                                             .fType = DataExchange::InternetMediaType{"text/xml; charset=\"utf-8\""sv}},
                                     Containers::Mapping<String, String>{{"SOAPACTION"sv, "\"{}#SetTarget\""_f(kSwitchPowerServiceType_)}});
            cout << "\t\tSwitched it " << (on ? "on" : "off") << endl;
        }
        catch (...) {
            cout << "\t\tCould not switch it " << (on ? "on" : "off") << ": " << Characters::ToString (current_exception ()).AsUTF8<string> () << endl;
        }
    }

    // the device's description, fetched from its LOCATION, and its services - switching it on or off if it is a light and
    // switchLightsTo says. Ignore if fails
    void DoPrintDeviceDescription_ (const URI& deviceDescriptionURL, optional<bool> switchLightsTo)
    {
        try {
            using namespace IO::Network::Transfer;
            Connection::Ptr c = Connection::New ();
            Response        r = c.GET (deviceDescriptionURL);
            if (r.GetSucceeded ()) {
                DeviceDescription deviceInfo = DeSerialize (r.GetData ());
                cout << "\t\tDevice-Description: " << Characters::ToString (deviceInfo) << endl;
                for (const DeviceDescription::Service& s : deviceInfo.fServices.value_or (Containers::Collection<DeviceDescription::Service>{})) {
                    URI controlURL = deviceDescriptionURL.Combine (s.fControlURL); // relative to the description's URL
                    cout << "\t\tService:  " << s.fServiceType.AsUTF8<string> () << ", controlled at "
                         << Characters::ToString (controlURL).AsUTF8<string> () << endl;
                    if (switchLightsTo and s.fServiceType == kSwitchPowerServiceType_) {
                        SwitchLight_ (controlURL, *switchLightsTo);
                    }
                }
            }
        }
        catch (...) {
            DbgTrace ("failed to fetch description: {}"_f, Characters::ToString (current_exception ()));
        }
    }
}

namespace {
    void DoListening_ (Listener* l, optional<bool> switchLightsTo)
    {
        cout << "Listening..." << endl;
        l->AddOnFoundCallback ([switchLightsTo] (const SSDP::Advertisement& d) {
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
            if (d.fAlive != false) { // (a byebye says no LOCATION)
                DoPrintDeviceDescription_ (d.fLocation, switchLightsTo);
            }
            cout << endl;
        });
        l->Start ();
    }
}

namespace {
    void DoSearching_ (Search* searcher, const String& searchFor, optional<bool> switchLightsTo)
    {
        cout << "Searching for '" << searchFor.AsUTF8<string> () << "'..." << endl;
        searcher->AddOnFoundCallback ([switchLightsTo] (const SSDP::Advertisement& d) {
            lock_guard<mutex> critSection{kStdOutMutex_};
            cout << "\tFound device (MATCHED SEARCH):" << endl;
            cout << "\t\tUSN:      " << d.fUSN.AsUTF8<string> () << endl;
            cout << "\t\tLocation: " << Characters::ToString (d.fLocation).AsUTF8<string> () << endl;
            cout << "\t\tST:       " << d.fTarget.AsUTF8<string> () << endl;
            if (not d.fServer.empty ()) {
                cout << "\t\tServer:   " << d.fServer.AsUTF8<string> () << endl;
            }
            DoPrintDeviceDescription_ (d.fLocation, switchLightsTo);
            cout << endl;
        });
        searcher->Start (searchFor);
    }
}

int main (int argc, const char* argv[])
{
    Debug::TraceContextBumper ctx{
        Stroika_Foundation_Debug_OptionalizeTraceArgs ("main", "argv={}"_f, Characters::ToString (vector<const char*>{argv, argv + argc}))};
#if qStroika_Platform_POSIX
    SignalHandlerRegistry::sThe.SetSignalHandlers (SIGPIPE, SignalHandlerRegistry::kIGNORED);
#endif
    bool                  listen = false;
    optional<String>      searchFor;
    optional<bool>        switchLightsTo;
    Time::DurationSeconds quitAfter = Time::kInfinity;

    const CommandLine::Option kListenO_{
        .fSingleCharName = 'l',
    };
    const CommandLine::Option kSearchO_{
        .fSingleCharName = 's', .fSupportsArgument = true, .fHelpArgName = "SEARCHFOR"sv, .fHelpOptionText = "Search for the argument UPNP name"sv};
    const CommandLine::Option kSwitchO_{.fLongName         = "switch"sv,
                                        .fSupportsArgument = true,
                                        .fHelpArgName      = "on|off"sv,
                                        .fHelpOptionText   = "Switch each UPnP light found (each SwitchPower service) on or off"sv};
    const CommandLine::Option kQuitAfterO_{.fLongName = "quit-after"sv, .fSupportsArgument = true, .fHelpArgName = "NSECONDS"sv};

    CommandLine cmdLine{argc, argv};
    listen    = cmdLine.Has (kListenO_);
    searchFor = cmdLine.GetArgument (kSearchO_);
    if (auto o = cmdLine.GetArgument (kSwitchO_)) {
        if (*o == "on"sv or *o == "off"sv) {
            switchLightsTo = *o == "on"sv;
        }
        else {
            cerr << "--switch takes on or off" << endl;
            return EXIT_FAILURE;
        }
    }
    if (auto o = cmdLine.GetArgument (kQuitAfterO_)) {
        quitAfter = Time::DurationSeconds{Characters::FloatConversion::ToFloat<Time::DurationSeconds::rep> (*o)};
    }

    if (not listen and not searchFor.has_value ()) {
        cerr << "Usage: SSDPClient [-l] [-s SEARCHFOR] [--switch on|off] [--quit-after N]" << endl;
        cerr << "   e.g. SSDPClient -l" << endl;
        cerr << "   e.g. SSDPClient -s \"upnp:rootdevice\"" << endl;
        cerr << "   e.g. SSDPClient -s " << kSwitchPowerServiceType_.AsUTF8<string> ()
             << " --switch on      (switches on each UPnP light found)" << endl;
        return EXIT_FAILURE;
    }

    try {
        Listener l;
        if (listen) {
            DoListening_ (&l, switchLightsTo);
        }
        Search s;
        if (searchFor.has_value ()) {
            DoSearching_ (&s, *searchFor, switchLightsTo);
        }
        if (listen or searchFor.has_value ()) {
            WaitableEvent{}.Wait (quitAfter); // wait quitAfter seconds, or til user hits ctrl-c
        }
        else {
            cerr << "Specify -l to listen or -s STRING to search" << endl;
            return EXIT_FAILURE;
        }
    }
    catch (const system_error& e) {
        if (Execution::IsA (e, errc::timed_out)) {
            cerr << "Timed out - so - exiting..." << endl;
            return EXIT_SUCCESS;
        }
        cerr << "Exception - " << Characters::ToString (e) << " - terminating..." << endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        cerr << "Exception - " << Characters::ToString (current_exception ()) << " - terminating..." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
