/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>
#include <mutex>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/TypedBLOB.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/IntervalTimer.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/ThreadPool.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/HTTP/Status.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"
#include "Stroika/Foundation/Memory/SharedPtr.h"

#include "Stroika/Frameworks/UPnP/DeviceDescription.h"
#include "Stroika/Frameworks/UPnP/GENA/Subscriber.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/CachingListener.h"
#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/Router.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;

using Client::CachingListener;
using Containers::Mapping;
using Containers::Set;

/*
 *  A UPnP control point: finds the devices around it - listening for their announcements, and searching - and says when
 *  each is found and when it goes, then fetches each one's description, once, listing its services. Told --switch on|off,
 *  it switches each light it finds (each SwitchPower service), once, and asks it its status. Told --watch, it subscribes to
 *  each service's events (GENA: UPnP Device Architecture 1.1, section 4), and prints each one - a light's each change of
 *  Status, say. The SSDPServer sample is such a light.
 */
namespace {
    mutex kStdOutMutex_; // what is found is told on the CachingListener's threads, and described on the fetcher's

    void Print_ (const String& line)
    {
        lock_guard critSection{kStdOutMutex_};
        cout << line.AsUTF8<string> () << endl;
    }

    // a light that can be switched on and off: the UPnP Forum's standard SwitchPower service (as the SSDPServer sample has)
    const String kSwitchPowerServiceType_{"urn:schemas-upnp-org:service:SwitchPower:1"sv};

    // an action of the SwitchPower service controlled at controlURL: a SOAP request, saying which action in its SOAPACTION header
    // (UPnP Device Architecture 1.1, section 3.2) - and its answer, the action's out arguments, as SOAP
    String SwitchPowerAction_ (const URI& controlURL, const String& action, const String& inArguments)
    {
        using namespace IO::Network::Transfer;
        const string request = "<?xml version=\"1.0\"?>\r\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                               "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:{0} xmlns:u=\"{1}\">{2}</u:{0}>"
                               "</s:Body></s:Envelope>\r\n"_f(action, kSwitchPowerServiceType_, inArguments)
                                   .AsUTF8<string> ();
        Response r = Connection::New ().POST (controlURL,
                                              DataExchange::TypedBLOB{.fData = as_bytes (span{request}),
                                                                      .fType = DataExchange::InternetMediaType{"text/xml; charset=\"utf-8\""sv}},
                                              Mapping<String, String>{{"SOAPACTION"sv, "\"{}#{}\""_f(kSwitchPowerServiceType_, action)}});
        return String::FromUTF8 (r.GetData ().As<string> ());
    }

    // an out argument, in an action's answer: an element in its <u:actionResponse> (UPnP Device Architecture 1.1, section 3.2.2).
    // Found as text, which is enough for the usual <name>value</name>
    optional<String> OutArgument_ (const String& answer, const String& name)
    {
        const String startTag = "<{}>"_f(name);
        if (optional<size_t> start = answer.Find (startTag)) {
            if (optional<size_t> end = answer.Find ("</{}>"_f(name), *start)) {
                return answer.SubString (*start + startTag.size (), *end);
            }
        }
        return nullopt;
    }

    // where --watch is told the services' events: this port, each service at a path of its own
    constexpr IO::Network::PortType kWatchPort_ = 8091;

    // what the fetcher does for each device it describes
    struct Fetching_ {
        optional<bool> fSwitchLightsTo; // --switch
        Set<String>    fSwitched;       // (only on the fetcher's thread) UDNs: each light switched once - not again, should it come back
        bool           fWatch{false};   // --watch
        unsigned int   fWatched{0};     // (only on the fetcher's thread) how many: so each its own callback path
        Mapping<String, Set<String>> fWatchedPaths; // (only on the fetcher's thread) each device's services watched: UDN -> callback paths
        // the services watched, by their callback path - by which their NOTIFYs are routed to them, on the web server's threads
        Execution::Synchronized<Mapping<String, shared_ptr<GENA::Subscriber>>> fWatching;
    };

    // subscribe to the events of the service at eventSubURL, and print each: with its device's UDN, and service's ID
    void Watch_ (const URI& eventSubURL, const String& udn, const String& serviceID, Fetching_* fetching)
    {
        const String path = "/gena/{}"_f(++fetching->fWatched);
        try {
            shared_ptr<GENA::Subscriber> subscriber =
                Memory::MakeSharedPtr<GENA::Subscriber> (eventSubURL, GENA::Subscriber::MakeCallbackURL (eventSubURL, kWatchPort_, path),
                                                         [udn, serviceID] (const GENA::Subscriber::Event& e) {
                                                             Print_ ("{} {} event {}: {}"_f(udn, serviceID, e.fSEQ, e.fVariables));
                                                         });
            fetching->fWatching.rwget ()->Add (path, subscriber); // routed to before subscribing: its first event can come first
            subscriber->Start ();
            fetching->fWatchedPaths.Add (udn, fetching->fWatchedPaths.LookupValue (udn) + path);
            Print_ ("\t\twatching it"sv);
        }
        catch (...) {
            fetching->fWatching.rwget ()->RemoveIf (path);
            Print_ ("\t\tcould not watch it: {}"_f(current_exception ()));
        }
    }

    // stop watching the services of the device udn - gone, so watched anew should it come back, not twice
    void Unwatch_ (const String& udn, Fetching_* fetching)
    {
        for (const String& path : fetching->fWatchedPaths.LookupValue (udn)) {
            shared_ptr<GENA::Subscriber> subscriber;
            {
                auto watching = fetching->fWatching.rwget ();
                subscriber    = watching->LookupValue (path);
                watching->RemoveIf (path);
            }
            // subscriber goes here, out of the lock - unsubscribing, over the network - or when a NOTIFY it is taking is done
        }
        fetching->fWatchedPaths.RemoveIf (udn);
    }

    // the device found at location: its description, and its services - switching each light on or off, and asking it its
    // status, and watching each service, as fetching says. On the fetcher's thread: fetching takes time, and the
    // CachingListener's threads are for what is heard - the next device found waits while a callback runs
    void Describe_ (const URI& location, Fetching_* fetching)
    {
        try {
            using namespace IO::Network::Transfer;
            DeviceDescription dd = DeSerialize (Connection::New ().GET (location).GetData ());
            Print_ ("\t{}: {} ({})"_f(dd.fUDN, dd.fFriendlyName, dd.fDeviceType));
            for (const DeviceDescription::Service& s : dd.fServices.value_or (Containers::Collection<DeviceDescription::Service>{})) {
                Print_ ("\t\tservice: {}"_f(s.fServiceType));
                if (s.fServiceType == kSwitchPowerServiceType_) {
                    URI controlURL = location.Combine (s.fControlURL); // relative to the description's URL
                    if (fetching->fSwitchLightsTo and not fetching->fSwitched.Contains (dd.fUDN)) {
                        fetching->fSwitched.Add (dd.fUDN);
                        SwitchPowerAction_ (controlURL, "SetTarget"sv, "<newTargetValue>{}</newTargetValue>"_f(*fetching->fSwitchLightsTo ? 1 : 0));
                        Print_ ("\t\tswitched it {}"_f(String{*fetching->fSwitchLightsTo ? "on"sv : "off"sv}));
                    }
                    optional<String> status = OutArgument_ (SwitchPowerAction_ (controlURL, "GetStatus"sv, String{}), "ResultStatus"sv);
                    Print_ ("\t\tit is {}"_f(String{status == "1"sv ? "on"sv : status == "0"sv ? "off"sv : "neither on nor off?"sv}));
                }
                if (fetching->fWatch and not s.fEventSubURL.GetPath ().empty ()) { // empty: it has no events
                    Watch_ (location.Combine (s.fEventSubURL), dd.fUDN, s.fServiceID, fetching);
                }
            }
        }
        catch (...) {
            Print_ ("\tcould not describe the device at {}: {}"_f(location, current_exception ()));
        }
    }

    // the device a USN names: its "uuid:device-UUID", before any "::" (UPnP Device Architecture 1.1, section 1.2.2)
    String DeviceOf_ (const String& usn)
    {
        optional<size_t> i = usn.Find ("::"sv);
        return i ? usn.SubString (0, *i) : usn;
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
    bool                  watch     = false;
    Time::DurationSeconds quitAfter = Time::kInfinity;

    const CommandLine::Option kListenO_{.fSingleCharName = 'l', .fHelpOptionText = "Listen: find devices as they announce themselves"sv};
    const CommandLine::Option kSearchO_{.fSingleCharName   = 's',
                                        .fSupportsArgument = true,
                                        .fHelpArgName      = "SEARCHFOR"sv,
                                        .fHelpOptionText   = "Search for it (a UPnP type, or ssdp:all) - and listen"sv};
    const CommandLine::Option kSwitchO_{.fLongName         = "switch"sv,
                                        .fSupportsArgument = true,
                                        .fHelpArgName      = "on|off"sv,
                                        .fHelpOptionText   = "Switch each UPnP light found (each SwitchPower service) on or off"sv};
    const CommandLine::Option kWatchO_{.fLongName = "watch"sv, .fHelpOptionText = "Watch each service found: subscribe to its events, and print each"sv};
    const CommandLine::Option kQuitAfterO_{.fLongName = "quit-after"sv, .fSupportsArgument = true, .fHelpArgName = "NSECONDS"sv};

    CommandLine cmdLine{argc, argv};
    listen    = cmdLine.Has (kListenO_);
    searchFor = cmdLine.GetArgument (kSearchO_);
    watch     = cmdLine.Has (kWatchO_);
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
        cerr << "Usage: SSDPClient [-l] [-s SEARCHFOR] [--switch on|off] [--watch] [--quit-after N]" << endl;
        cerr << "   e.g. SSDPClient -l" << endl;
        cerr << "   e.g. SSDPClient -s \"upnp:rootdevice\"" << endl;
        cerr << "   e.g. SSDPClient -s " << kSwitchPowerServiceType_.AsUTF8<string> ()
             << " --switch on      (switches on each UPnP light found)" << endl;
        cerr << "   e.g. SSDPClient -s " << kSwitchPowerServiceType_.AsUTF8<string> () << " --watch      (prints each UPnP light's each change)" << endl;
        return EXIT_FAILURE;
    }

    IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by CachingListener

    try {
        // what was found: only on the CachingListener's callbacks, which it calls one at a time
        Mapping<String, Set<String>> devices; // the USNs in force of each device found
        // what was described (or is being): each description once - a device on several networks has a LOCATION on each, and an
        // embedded device has its root's - so a device or a LOCATION described already is not described again
        Set<String> described;
        Set<URI>    describedAt;
        // what the fetcher does with each device
        Fetching_ fetching;
        fetching.fSwitchLightsTo = switchLightsTo;
        fetching.fWatch          = watch;
        // --watch: where the services' events are told - each NOTIFY routed to its subscriber by its path. Before the fetcher, so
        // destroyed after it; after fetching, so destroyed before it - its subscribers unsubscribe after the last NOTIFY is taken
        optional<Stroika::Frameworks::WebServer::ConnectionManager> notifies;
        if (watch) {
            using namespace Stroika::Frameworks::WebServer;
            notifies.emplace (
                IO::Network::SocketAddresses (IO::Network::InternetAddresses_Any (), kWatchPort_),
                Containers::Sequence<Route>{Route{"NOTIFY"_RegEx, "gena/.+"_RegEx, [&fetching] (Message& m) {
                                                      if (shared_ptr<GENA::Subscriber> s =
                                                              fetching.fWatching.cget ()->LookupValue (m.request ().url ().GetPath ())) {
                                                          s->HandleNotify (m);
                                                      }
                                                      else {
                                                          m.rwResponse ().status = IO::Network::HTTP::StatusCodes::kPreconditionFailed; // no such subscription
                                                      }
                                                  }}});
        }
        ThreadPool fetcher{ThreadPool::Options{.fThreadCount = 1}};
        // what it searches for, or - just listening - everything: of what it hears, the advertisements of that
        const bool everything = not searchFor or *searchFor == kTarget_SSDPAll;
        auto       onChange   = [&] (const SSDP::Advertisement& a) {
            if (not everything and a.fTarget != *searchFor) {
                return;
            }
            const String device = DeviceOf_ (a.fUSN);
            Set<String>  usns   = devices.LookupValue (device);
            if (a.fAlive == true) {
                if (usns.empty ()) {
                    Print_ ("found {}, at {}"_f(device, a.fLocation));
                }
                usns.Add (a.fUSN);
                devices.Add (device, usns);
                if (not described.Contains (device) and not describedAt.Contains (a.fLocation)) {
                    fetcher.AddTask ([location = a.fLocation, &fetching] () { Describe_ (location, &fetching); });
                }
                described.Add (device);
                describedAt.Add (a.fLocation);
            }
            else if (usns.RemoveIf (a.fUSN)) {
                if (usns.empty ()) {
                    devices.Remove (device);
                    described.Remove (device); // so described again if it comes back
                    describedAt.RemoveIf (a.fLocation);
                    Print_ ("gone: {}"_f(device));
                    if (watch) {
                        fetcher.AddTask ([device, &fetching] () { Unwatch_ (device, &fetching); });
                    }
                }
                else {
                    devices.Add (device, usns);
                }
            }
        };
        // last: so destroyed first, its callbacks done before what they use is
        CachingListener cache{onChange, CachingListener::Options{.fSearchFor = searchFor}, CachingListener::eAutoStart};
        Print_ (searchFor ? "Searching for {}, and listening..."_f(*searchFor) : String{"Listening..."sv});
        WaitableEvent{}.Wait (quitAfter); // wait quitAfter seconds, or til user hits ctrl-c
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
