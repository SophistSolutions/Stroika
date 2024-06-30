/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/Listener.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/BasicServer.h"
#include "Stroika/Frameworks/WebServer/ConnectionManager.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;
using namespace Stroika::Frameworks::WebServer;

using Containers::Sequence;
using Server::BasicServer;

namespace {
    struct WebServerForDeviceDescription_ : WebServer::ConnectionManager {
        static inline const HTTP::Headers kDefaultResponseHeaders_{[] () {
            HTTP::Headers h;
            h.server = "stroika-ssdp-server-demo"sv;
            return h;
        }()};
        WebServerForDeviceDescription_ (uint16_t webServerPortNumber, const DeviceDescription& dd)
            : ConnectionManager{SocketAddresses (InternetAddresses_Any (), webServerPortNumber),
                                Sequence<Route>{
                                    Route{""_RegEx,
                                          [dd] (Message* m) {
                                              RequireNotNull (m);
                                              Response& response   = m->rwResponse ();
                                              response.contentType = DataExchange::InternetMediaTypes::kXML;
                                              response.write (Stroika::Frameworks::UPnP::Serialize (dd));
                                          }},
                                },
                                Options{.fMaxConnections = 3, .fDefaultResponseHeaders = kDefaultResponseHeaders_}}
        {
        }
    };
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Debug::TraceContextBumper ctx{
        Stroika_Foundation_Debug_OptionalizeTraceArgs ("main", "argv={}"_f, Characters::ToString (vector<const char*>{argv, argv + argc}))};
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif

    Time::DurationSeconds quitAfter    = Time::kInfinity;
    uint16_t              portForOurWS = 8080;

    const Execution::CommandLine::Option kQuitAfterO_{.fLongName = "quit-after"sv, .fSupportsArgument = true};

    Execution::CommandLine cmdLine{argc, argv};
    if (auto o = cmdLine.GetArgument (kQuitAfterO_)) {
        quitAfter = Time::DurationSeconds{Characters::FloatConversion::ToFloat<Time::DurationSeconds::rep> (*o)};
    }

    Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by UPnP::BasicServer

    try {
        Device d;
        d.fLocation.SetScheme (URI::SchemeType{"http"sv});
        d.fLocation.SetAuthority (URI::Authority{nullopt, portForOurWS});
        d.fServer   = UPnP::SSDP::MakeServerHeaderValue ("MyStroikaBasedSampleProduct/1.0"sv);
        d.fDeviceID = UPnP::MungePrimaryMacAddrIntoBaseDeviceID ("315CAAE0-1335-57BF-A178-24C9EE756627"sv);

        DeviceDescription deviceInfo;
        deviceInfo.fPresentationURL  = URI{"http://www.sophists.com/"sv};
        deviceInfo.fDeviceType       = "urn:sophists.com:device:deviceType:1.0"sv;
        deviceInfo.fManufactureName  = "Sophist Solutions, Inc."sv;
        deviceInfo.fFriendlyName     = "Sophist Solutions fake device"sv;
        deviceInfo.fManufacturingURL = URI{"http://www.sophists.com/"sv};
        deviceInfo.fModelDescription = "long user-friendly title"sv;
        deviceInfo.fModelName        = "model name"sv;
        deviceInfo.fModelNumber      = "model number"sv;
        deviceInfo.fModelURL         = URI{"http://www.sophists.com/"sv};
        deviceInfo.fSerialNumber     = "manufacturer's serial number"sv;
        deviceInfo.fUDN              = "uuid:" + d.fDeviceID;

        WebServerForDeviceDescription_ deviceWS{portForOurWS, deviceInfo};
        BasicServer                    b{d, deviceInfo};
        Execution::WaitableEvent{}.Wait (quitAfter); // wait quitAfter seconds, or til user hits ctrl-c
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
