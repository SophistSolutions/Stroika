/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <atomic>
#include <iostream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Common/SystemConfiguration.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Frameworks/UPnP/GENA/Publisher.h"
#include "Stroika/Frameworks/UPnP/SOAP/Action.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/BasicServer.h"
#include "Stroika/Frameworks/UPnP/ServiceDescription.h"
#include "Stroika/Frameworks/WebServer/ConnectionManager.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;
using namespace Stroika::Frameworks::WebServer;

using Containers::Mapping;
using Containers::Sequence;
using DataExchange::InternetMediaType;
using Server::BasicServer;

/*
 *  A light, switched on and off over the network: the UPnP Forum's standard BinaryLight device, whose one service - SwitchPower -
 *  does the switching (https://upnp.org/specs/ha/UPnP-ha-BinaryLight-v1-Device.pdf, UPnP-ha-SwitchPower-v1-Service.pdf).
 *  BasicServer advertises it (SSDP), so control points find it; a web server then serves what they ask for: its description,
 *  its service's description, and its service's actions (SOAP: UPnP Device Architecture 1.1, section 3) - and, for a person, a
 *  page (its presentationURL) saying whether it is on, with buttons to switch it. And its eventing (GENA: section 4): a control
 *  point that subscribes is told each change of the light's Status.
 *
 *  For a web service of your own design, rather than a standard UPnP one, see Samples/WebService.
 */
namespace {
    const String kSwitchPowerServiceType_{"urn:schemas-upnp-org:service:SwitchPower:1"sv};

    // what UPnP's descriptions are sent as (UPnP Device Architecture 1.1, section 2.11) - and what a control request must be
    const InternetMediaType kUPnPXML_{"text/xml"sv};

    // SwitchPower's description (its SCPD): its actions, and the state they act on - as its standard gives it
    using SD = ServiceDescription;
    const SD kSwitchPowerDescription_{
        .fActions = {SD::Action{.fName = "SetTarget"sv, .fArguments = {{.fName = "newTargetValue"sv, .fRelatedStateVariable = "Target"sv}}},
                     SD::Action{.fName = "GetTarget"sv,
                                .fArguments = {{.fName = "RetTargetValue"sv, .fDirection = SD::Argument::Direction::eOut, .fRelatedStateVariable = "Target"sv}}},
                     SD::Action{.fName = "GetStatus"sv,
                                .fArguments = {{.fName = "ResultStatus"sv, .fDirection = SD::Argument::Direction::eOut, .fRelatedStateVariable = "Status"sv}}}},
        .fStateVariables = {SD::StateVariable{.fName = "Target"sv, .fDataType = "boolean"sv, .fDefaultValue = "0"sv, .fSendEvents = false},
                            SD::StateVariable{.fName = "Status"sv, .fDataType = "boolean"sv, .fDefaultValue = "0"sv}}};

    // a UPnP boolean: 0, false or no; 1, true or yes (UPnP Device Architecture 1.1, section 2.5)
    optional<bool> ParseBoolean_ (const String& s)
    {
        String v = s.Trim ().ToLowerCase ();
        if (v == "1"sv or v == "true"sv or v == "yes"sv) {
            return true;
        }
        if (v == "0"sv or v == "false"sv or v == "no"sv) {
            return false;
        }
        return nullopt;
    }

    // the light: on or off - its SwitchPower service's Status (and its Target: a simple light, it is as it was last told) - and
    // the eventing of that Status, which SwitchPower's description says is evented
    struct Light_ {
        atomic<bool>    fOn{false}; // off, until switched on
        GENA::Publisher fEvents{[this] () { return Mapping<String, String>{{"Status"sv, fOn ? "1"sv : "0"sv}}; }};

        // switched - by a control point, or on the light's page: each subscriber told
        void Set (bool on)
        {
            fOn = on;
            cout << "The light is now " << (on ? "on" : "off") << endl;
            fEvents.Notify ({{"Status"sv, on ? "1"sv : "0"sv}});
        }
    };

    // a SwitchPower action, POSTed to its control URL as SOAP - which one, the request says (as does its SOAPACTION header) -
    // answered with its out arguments, or with an error (UPnP Device Architecture 1.1, section 3.2)
    void SwitchPowerAction_ (Message& m, Light_& light)
    {
        Response& response = m.rwResponse ();
        // its body must be text/xml (with any charset), or it is refused: 415 (UPnP Device Architecture 1.1, section 3.2.1)
        if (optional<InternetMediaType> ct = m.request ().headers ().contentType ();
            not ct or not DataExchange::InternetMediaTypeRegistry::sThe->IsA (kUPnPXML_, *ct)) {
            response.status = HTTP::StatusCodes::kUnsupportedMediaType;
            return;
        }
        response.contentType = SOAP::kContentType;
        auto fail            = [&] (unsigned int errorCode, const String& errorDescription) {
            response.status = HTTP::StatusCodes::kInternalError; // how SOAP says it failed - the error saying how
            response.write (SOAP::Serialize (SOAP::ActionError{.fErrorCode = errorCode, .fErrorDescription = errorDescription}));
        };
        SOAP::ActionRequest request;
        try {
            SOAP::DeSerialize (m.rwRequest ().GetBody (), &request);
        }
        catch (...) {
            fail (SOAP::ActionError::kInvalidAction, "Invalid Action"sv); // not a SOAP request: not one of its actions
            return;
        }
        auto answer = [&] (const SOAP::Arguments& outArguments) {
            response.write (SOAP::Serialize (
                SOAP::ActionResponse{.fServiceType = kSwitchPowerServiceType_, .fAction = request.fAction, .fArguments = outArguments}));
        };
        const String status = light.fOn ? "1"sv : "0"sv;
        if (request.fServiceType != kSwitchPowerServiceType_) {
            fail (SOAP::ActionError::kInvalidAction, "Invalid Action"sv); // another service's
        }
        else if (request.fAction == "GetStatus"sv) {
            answer ({{"ResultStatus"sv, status}});
        }
        else if (request.fAction == "GetTarget"sv) {
            answer ({{"RetTargetValue"sv, status}});
        }
        else if (request.fAction == "SetTarget"sv) {
            optional<String> newTargetValue = request.LookupArgument ("newTargetValue"sv);
            optional<bool>   on             = newTargetValue ? ParseBoolean_ (*newTargetValue) : nullopt;
            if (not on) {
                fail (SOAP::ActionError::kInvalidArgs, "Invalid Args"sv);
                return;
            }
            light.Set (*on);
            answer ({});
        }
        else {
            fail (SOAP::ActionError::kInvalidAction, "Invalid Action"sv);
        }
    }

    // the light's page - its presentationURL - for a person: whether it is on, and buttons that POST switch=on or switch=off back
    // to it (an HTML form's own encoding: application/x-www-form-urlencoded)
    void LightPage_ (Message& m, Light_& light)
    {
        if (m.request ().httpMethod () == HTTP::Methods::kPost) {
            const String form = String::FromUTF8 (m.rwRequest ().GetBody ().As<string> ());
            if (form == "switch=on"sv or form == "switch=off"sv) {
                light.Set (form == "switch=on"sv);
            }
        }
        Response& response   = m.rwResponse ();
        response.contentType = DataExchange::InternetMediaTypes::kHTML;
        response.write ("<!DOCTYPE html>\r\n<html><head><title>Stroika sample light</title></head><body><h1>Stroika sample light</h1>"
                        "<p>The light is <b>{}</b>.</p><form method=\"post\"><button name=\"switch\" value=\"on\">On</button> "
                        "<button name=\"switch\" value=\"off\">Off</button></form></body></html>\r\n"_f(String{light.fOn ? "on"sv : "off"sv}));
    }

    struct DeviceWebServer_ : WebServer::ConnectionManager {
        static inline const HTTP::Headers kDefaultResponseHeaders_{[] () {
            HTTP::Headers h;
            h.server = "stroika-ssdp-server-demo"sv;
            return h;
        }()};
        // the device description dd - at /, the LOCATION SSDP advertises - its service's description, actions and eventing, and its
        // page, at the URLs dd gives them; light must outlive this
        DeviceWebServer_ (uint16_t webServerPortNumber, const DeviceDescription& dd, Light_* light)
            : ConnectionManager{
                  SocketAddresses (InternetAddresses_Any (), webServerPortNumber),
                  Sequence<Route>{
                      Route{""_RegEx,
                            [dd] (Message& m) {
                                Response& response   = m.rwResponse ();
                                response.contentType = kUPnPXML_;
                                response.write (Stroika::Frameworks::UPnP::Serialize (dd));
                            }},
                      Route{"SwitchPower/description.xml"_RegEx,
                            [] (Message& m) {
                                Response& response   = m.rwResponse ();
                                response.contentType = kUPnPXML_;
                                response.write (Stroika::Frameworks::UPnP::Serialize (kSwitchPowerDescription_));
                            }},
                      Route{HTTP::MethodsRegEx::kPost, "SwitchPower/control"_RegEx, [light] (Message& m) { SwitchPowerAction_ (m, *light); }},
                      Route{"SUBSCRIBE|UNSUBSCRIBE"_RegEx, "SwitchPower/event"_RegEx,
                            [light] (Message& m) { light->fEvents.HandleRequest (m); }},
                      Route{"light"_RegEx, [light] (Message& m) { LightPage_ (m, *light); }},
                      Route{HTTP::MethodsRegEx::kPost, "light"_RegEx, [light] (Message& m) { LightPage_ (m, *light); }},
                  },
                  Options{.fMaxConnections = 3, .fDefaultResponseHeaders = kDefaultResponseHeaders_}}
        {
        }
    };
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    CommandLine cmdLine{argc, argv};

    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("main", "argv={}"_f, cmdLine)};

#if qStroika_Platform_POSIX
    SignalHandlerRegistry::sThe.SetSignalHandlers (SIGPIPE, SignalHandlerRegistry::kIGNORED);
#endif

    Time::DurationSeconds quitAfter    = Time::kInfinity;
    uint16_t              portForOurWS = 8080;

    const CommandLine::Option kPortO_{.fLongName         = "port"sv,
                                      .fSupportsArgument = true,
                                      .fHelpArgName      = "PORT"sv,
                                      .fHelpOptionText = "The port its web server - its description, control and page - listens on (default 8080)"sv};
    const CommandLine::Option           kQuitAfterO_{.fLongName = "quit-after"sv, .fSupportsArgument = true, .fHelpArgName = "NSECONDS"sv};
    const Sequence<CommandLine::Option> kAllOptions_{StandardCommandLineOptions::kHelp, kPortO_, kQuitAfterO_};

    if (cmdLine.Has (StandardCommandLineOptions::kHelp)) {
        cerr << cmdLine.GenerateUsage (kAllOptions_) << endl;
        return EXIT_SUCCESS;
    }
    if (auto o = cmdLine.GetArgument (kPortO_)) {
        portForOurWS = String2Int<uint16_t> (*o); // 0 if not a number
        if (portForOurWS == 0) {
            cerr << "--port takes a port number, 1 to 65535" << endl;
            return EXIT_FAILURE;
        }
    }
    if (auto o = cmdLine.GetArgument (kQuitAfterO_)) {
        quitAfter = Time::DurationSeconds{Characters::FloatConversion::ToFloat<Time::DurationSeconds::rep> (*o)};
    }

    IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by UPnP::BasicServer

    try {
        Device d;
        d.fServer = UPnP::SSDP::MakeServerHeaderValue ("MyStroikaBasedSampleProduct/1.0"sv);
        // the device's ID: the same each time it runs here, and different on another machine (as UPnP requires) - this machine's
        // own ID, derived for this product (so not revealing it). Where the OS has none (some containers), a new one each run.
        static const Common::GUID kThisProduct_{"315CAAE0-1335-57BF-A178-24C9EE756627"sv};
        d.fDeviceID = Common::GetSystemConfiguration_MachineID (kThisProduct_).value_or (Common::GUID::GenerateNew ());

        DeviceDescription deviceInfo;
        deviceInfo.fPresentationURL  = URI{"/light"sv}; // its page, where deviceWS serves it (relative to the description's URL)
        deviceInfo.fDeviceType       = "urn:schemas-upnp-org:device:BinaryLight:1"sv;
        deviceInfo.fManufactureName  = "Sophist Solutions, Inc."sv;
        deviceInfo.fFriendlyName     = "Stroika sample light"sv;
        deviceInfo.fManufacturingURL = URI{"https://www.sophists.com/"sv};
        deviceInfo.fModelDescription = "a light, switched over the network - the Stroika SSDPServer sample"sv;
        deviceInfo.fModelName        = "Stroika sample light"sv;
        deviceInfo.fModelNumber      = "1"sv;
        deviceInfo.fModelURL         = URI{"https://github.com/SophistSolutions/Stroika/tree/v3-Release/Samples/SSDPServer"sv};
        // no fSerialNumber: optional, and a sample has none to give
        deviceInfo.fUDN = "uuid:" + d.fDeviceID.As<String> ();
        // its one service: SwitchPower - its description, actions and eventing where deviceWS serves them (each URL relative to the
        // device description's)
        deviceInfo.fServices = Containers::Collection<DeviceDescription::Service>{DeviceDescription::Service{
            .fServiceType = kSwitchPowerServiceType_,
            .fServiceID   = "urn:upnp-org:serviceId:SwitchPower"sv,
            .fSCPDURL     = URI{"/SwitchPower/description.xml"sv},
            .fControlURL  = URI{"/SwitchPower/control"sv},
            .fEventSubURL = URI{"/SwitchPower/event"sv},
        }};

        Light_           light;
        DeviceWebServer_ deviceWS{portForOurWS, deviceInfo, &light};
        BasicServer      b{d, deviceInfo, Server::LocationFromBindings (deviceWS.bindings ())}; // on each network, where deviceWS listens
        cout << "A UPnP light, off - to switch on, e.g.: SSDPClient -s " << kSwitchPowerServiceType_.AsUTF8<string> () << " --switch on" << endl;
        cout << "or with its page: http://localhost:" << portForOurWS << "/light" << endl;
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
