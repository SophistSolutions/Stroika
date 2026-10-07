/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <atomic>
#include <iostream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Common/SystemConfiguration.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/XML/DOM.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/BasicServer.h"
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

using Containers::Sequence;
using DataExchange::InternetMediaType;
using Server::BasicServer;

/*
 *  A light, switched on and off over the network: the UPnP Forum's standard BinaryLight device, whose one service - SwitchPower -
 *  does the switching (https://upnp.org/specs/ha/UPnP-ha-BinaryLight-v1-Device.pdf, UPnP-ha-SwitchPower-v1-Service.pdf).
 *  BasicServer advertises it (SSDP), so control points find it; a web server then serves what they ask for: its description,
 *  its service's description, and its service's actions (SOAP: UPnP Device Architecture 1.1, section 3).
 *
 *  Not shown: eventing (GENA - telling subscribers each change of the light's Status: section 4), so a control point asks
 *  instead, with GetStatus. For a web service of your own design, rather than a standard UPnP one, see Samples/WebService.
 */
namespace {
    const String kSwitchPowerServiceType_{"urn:schemas-upnp-org:service:SwitchPower:1"sv};

    // what UPnP's XML - its descriptions, and SOAP - is sent as (UPnP Device Architecture 1.1, sections 2.11 and 3.2.2)
    const InternetMediaType kUPnPXML_{"text/xml"sv};

    // SwitchPower's description (its SCPD): its actions, and the state they act on - as its standard gives it
    constexpr string_view kSwitchPowerDescription_ = R"(<?xml version="1.0"?>
<scpd xmlns="urn:schemas-upnp-org:service-1-0">
    <specVersion>
        <major>1</major>
        <minor>0</minor>
    </specVersion>
    <actionList>
        <action>
            <name>SetTarget</name>
            <argumentList>
                <argument>
                    <name>newTargetValue</name>
                    <relatedStateVariable>Target</relatedStateVariable>
                    <direction>in</direction>
                </argument>
            </argumentList>
        </action>
        <action>
            <name>GetTarget</name>
            <argumentList>
                <argument>
                    <name>RetTargetValue</name>
                    <relatedStateVariable>Target</relatedStateVariable>
                    <direction>out</direction>
                </argument>
            </argumentList>
        </action>
        <action>
            <name>GetStatus</name>
            <argumentList>
                <argument>
                    <name>ResultStatus</name>
                    <relatedStateVariable>Status</relatedStateVariable>
                    <direction>out</direction>
                </argument>
            </argumentList>
        </action>
    </actionList>
    <serviceStateTable>
        <stateVariable sendEvents="no">
            <name>Target</name>
            <dataType>boolean</dataType>
            <defaultValue>0</defaultValue>
        </stateVariable>
        <stateVariable sendEvents="yes">
            <name>Status</name>
            <dataType>boolean</dataType>
            <defaultValue>0</defaultValue>
        </stateVariable>
    </serviceStateTable>
</scpd>
)";

    // a SOAP envelope: each control request, and each answer, is one - round its body (UPnP Device Architecture 1.1, section 3.2)
    String SOAPEnvelope_ (const String& body)
    {
        return "<?xml version=\"1.0\"?>\r\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
               "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>{}</s:Body></s:Envelope>\r\n"_f(body);
    }

    // the value of the argument called name, in a SOAP request: an unqualified element in the action's (UPnP Device Architecture
    // 1.1, section 3.2.1). The only XML this device reads - all the rest it writes, as text - so an XML parser (Xerces or libxml2)
    // is optional: where the build has one, it parses the request; where not, it finds the argument as text, which is enough for
    // the usual <name>value</name>, though not for an argument with attributes (which UPnP allows)
    optional<String> Argument_ (const Memory::BLOB& soapRequest, const String& name)
    {
#if qStroika_Foundation_DataExchange_XML_SupportDOM
        using namespace DataExchange::XML::DOM;
        return Document::New (soapRequest.As<Streams::InputStream::Ptr<std::byte>> ()).GetRootElement ().GetValue (XPath::Expression{"//{}"_f(name)});
#else
        const String text     = String::FromUTF8 (soapRequest.As<string> ());
        const String startTag = "<{}>"_f(name);
        if (optional<size_t> start = text.Find (startTag)) {
            if (optional<size_t> end = text.Find ("</{}>"_f(name), *start)) {
                return text.SubString (*start + startTag.size (), *end);
            }
        }
        return nullopt;
#endif
    }

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

    // a SwitchPower action, POSTed to its control URL as SOAP - which one, its SOAPACTION header says: the service type, # and the
    // action's name, in quotes (UPnP Device Architecture 1.1, section 3.2.1) - answered with its out arguments, or a fault
    void SwitchPowerAction_ (Message& m, atomic<bool>& lightOn)
    {
        Response& response        = m.rwResponse ();
        response.contentType      = kUPnPXML_;
        const String soapAction   = m.request ().headers ().LookupOne ("SOAPACTION"sv).value_or (String{}).Trim ([] (Character c) {
            return c == '"' or c.IsWhitespace ();
        });
        const String actionPrefix = kSwitchPowerServiceType_ + "#"sv;
        const String action       = soapAction.StartsWith (actionPrefix) ? soapAction.SubString (actionPrefix.size ()) : String{};
        auto         answer       = [&] (const String& outArguments) {
            response.write (SOAPEnvelope_ ("<u:{0}Response xmlns:u=\"{1}\">{2}</u:{0}Response>"_f(action, kSwitchPowerServiceType_, outArguments)));
        };
        auto fault = [&] (unsigned int errorCode, const String& errorDescription) {
            response.status = HTTP::StatusCodes::kInternalError; // how SOAP says it failed - with a UPnPError saying how
            response.write (SOAPEnvelope_ ("<s:Fault><faultcode>s:Client</faultcode><faultstring>UPnPError</faultstring><detail><UPnPError "
                                           "xmlns=\"urn:schemas-upnp-org:control-1-0\"><errorCode>{}</errorCode><errorDescription>{}"
                                           "</errorDescription></UPnPError></detail></s:Fault>"_f(errorCode, errorDescription)));
        };
        // the light is as it was last told: its Status is always its Target, as the standard allows a simple one to be
        if (action == "GetStatus"sv) {
            answer ("<ResultStatus>{}</ResultStatus>"_f(lightOn ? 1 : 0));
        }
        else if (action == "GetTarget"sv) {
            answer ("<RetTargetValue>{}</RetTargetValue>"_f(lightOn ? 1 : 0));
        }
        else if (action == "SetTarget"sv) {
            optional<String> newTargetValue = Argument_ (m.rwRequest ().GetBody (), "newTargetValue"sv);
            optional<bool>   on             = newTargetValue ? ParseBoolean_ (*newTargetValue) : nullopt;
            if (not on) {
                fault (402, "Invalid Args"sv);
                return;
            }
            lightOn = *on;
            cout << "The light is now " << (*on ? "on" : "off") << endl;
            answer (String{});
        }
        else {
            fault (401, "Invalid Action"sv);
        }
    }

    struct DeviceWebServer_ : WebServer::ConnectionManager {
        static inline const HTTP::Headers kDefaultResponseHeaders_{[] () {
            HTTP::Headers h;
            h.server = "stroika-ssdp-server-demo"sv;
            return h;
        }()};
        // the device description dd - at /, the LOCATION SSDP advertises - and its service's description and actions, at the URLs dd
        // gives them; the light, lightOn, must outlive this
        DeviceWebServer_ (uint16_t webServerPortNumber, const DeviceDescription& dd, atomic<bool>* lightOn)
            : ConnectionManager{SocketAddresses (InternetAddresses_Any (), webServerPortNumber),
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
                                              response.write (kSwitchPowerDescription_);
                                          }},
                                    Route{HTTP::MethodsRegEx::kPost, "SwitchPower/control"_RegEx,
                                          [lightOn] (Message& m) { SwitchPowerAction_ (m, *lightOn); }},
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

    const CommandLine::Option           kQuitAfterO_{.fLongName = "quit-after"sv, .fSupportsArgument = true};
    const Sequence<CommandLine::Option> kAllOptions_{StandardCommandLineOptions::kHelp, kQuitAfterO_};

    if (auto o = cmdLine.GetArgument (kQuitAfterO_)) {
        quitAfter = Time::DurationSeconds{Characters::FloatConversion::ToFloat<Time::DurationSeconds::rep> (*o)};
    }

    if (cmdLine.Has (StandardCommandLineOptions::kHelp)) {
        cerr << cmdLine.GenerateUsage (kAllOptions_) << endl;
        return EXIT_SUCCESS;
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
        deviceInfo.fPresentationURL  = URI{"http://www.sophists.com/"sv};
        deviceInfo.fDeviceType       = "urn:schemas-upnp-org:device:BinaryLight:1"sv;
        deviceInfo.fManufactureName  = "Sophist Solutions, Inc."sv;
        deviceInfo.fFriendlyName     = "Stroika sample light"sv;
        deviceInfo.fManufacturingURL = URI{"http://www.sophists.com/"sv};
        deviceInfo.fModelDescription = "a light, switched over the network - the Stroika SSDPServer sample"sv;
        deviceInfo.fModelName        = "Stroika sample light"sv;
        deviceInfo.fModelNumber      = "model number"sv;
        deviceInfo.fModelURL         = URI{"http://www.sophists.com/"sv};
        deviceInfo.fSerialNumber     = "manufacturer's serial number"sv;
        deviceInfo.fUDN              = "uuid:" + d.fDeviceID.As<String> ();
        // its one service: SwitchPower - its description and actions where deviceWS serves them (each URL relative to the device
        // description's), and no eventing, so no URL to subscribe at
        deviceInfo.fServices = Containers::Collection<DeviceDescription::Service>{DeviceDescription::Service{
            .fServiceType = kSwitchPowerServiceType_,
            .fServiceID   = "urn:upnp-org:serviceId:SwitchPower"sv,
            .fSCPDURL     = URI{"/SwitchPower/description.xml"sv},
            .fControlURL  = URI{"/SwitchPower/control"sv},
        }};

        atomic<bool>     lightOn{false}; // the light: off, until a control point switches it on
        DeviceWebServer_ deviceWS{portForOurWS, deviceInfo, &lightOn};
        BasicServer      b{d, deviceInfo, Server::LocationFromBindings (deviceWS.bindings ())}; // on each network, where deviceWS listens
        cout << "A UPnP light, off - to switch on, e.g.: SSDPClient -s " << kSwitchPowerServiceType_.AsUTF8<string> () << " --switch on" << endl;
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
