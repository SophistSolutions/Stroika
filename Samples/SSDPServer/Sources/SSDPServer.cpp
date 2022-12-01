/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String_Constant.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
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
    struct WebServerForDeviceDescription_ {

        WebServerForDeviceDescription_ (uint16_t webServerPortNumber, const DeviceDescription& dd)
        {
            // @todo Consider simplifying this using WebServer Framework more fully - Router or Interceptor
            auto onConnect = [dd] (const ConnectionOrientedStreamSocket::Ptr& acceptedSocketConnection) {
                Execution::Thread::Ptr runConnectionOnAnotherThread = Execution::Thread::New ([acceptedSocketConnection, dd] () {
                    // If the URLs are served locally, you may want to update the URL based on
                    // IO::Network::GetPrimaryInternetAddress ()
                    Connection conn{acceptedSocketConnection,
                                    Sequence<Interceptor>{
                                        Interceptor{
                                            [=] (Message* m) {
                                                RequireNotNull (m);
                                                Response& response           = m->rwResponse ();
                                                response.rwHeaders ().server = L"stroika-ssdp-server-demo"sv;
                                                response.contentType         = DataExchange::InternetMediaTypes::kXML;
                                                response.write (Stroika::Frameworks::UPnP::Serialize (dd));
                                            }}}};
                    conn.remainingConnectionLimits = HTTP::KeepAlive{0, 0}; // disable keep-alives
                    conn.ReadAndProcessMessage ();
                });
                runConnectionOnAnotherThread.SetThreadName (L"SSDP Service Connection Thread"sv);
                runConnectionOnAnotherThread.Start ();
            };
            fListener = Listener{SocketAddresses (InternetAddresses_Any (), webServerPortNumber), onConnect};
        }

        optional<Listener> fListener;
    };
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"main", L"argv=%s", Characters::ToString (vector<const char*>{argv, argv + argc}).c_str ())};
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif

    Time::DurationSecondsType quitAfter    = numeric_limits<Time::DurationSecondsType>::max ();
    uint16_t                  portForOurWS = 8080;

    Sequence<String> args = Execution::ParseCommandLine (argc, argv);
    for (auto argi = args.begin (); argi != args.end (); ++argi) {
        if (Execution::MatchesCommandLineArgument (*argi, L"quit-after")) {
            ++argi;
            if (argi != args.end ()) {
                quitAfter = Characters::FloatConversion::ToFloat<Time::DurationSecondsType> (*argi);
            }
            else {
                cerr << "Expected arg to -quit-after" << endl;
                return EXIT_FAILURE;
            }
        }
    }

    try {
        Device d;
        d.fLocation.SetScheme (URI::SchemeType{L"http"sv});
        d.fLocation.SetAuthority (URI::Authority{nullopt, portForOurWS});
        d.fServer   = UPnP::SSDP::MakeServerHeaderValue (L"MyStroikaBasedSampleProduct/1.0"sv);
        d.fDeviceID = UPnP::MungePrimaryMacAddrIntoBaseDeviceID (L"315CAAE0-1335-57BF-A178-24C9EE756627"sv);

        DeviceDescription deviceInfo;
        deviceInfo.fPresentationURL  = URI{L"http://www.sophists.com/"sv};
        deviceInfo.fDeviceType       = L"urn:sophists.com:device:deviceType:1.0"sv;
        deviceInfo.fManufactureName  = L"Sophist Solutions, Inc."sv;
        deviceInfo.fFriendlyName     = L"Sophist Solutions fake device"sv;
        deviceInfo.fManufacturingURL = URI{L"http://www.sophists.com/"sv};
        deviceInfo.fModelDescription = L"long user-friendly title"sv;
        deviceInfo.fModelName        = L"model name"sv;
        deviceInfo.fModelNumber      = L"model number"sv;
        deviceInfo.fModelURL         = URI{L"http://www.sophists.com/"sv};
        deviceInfo.fSerialNumber     = L"manufacturer's serial number"sv;
        deviceInfo.fUDN              = L"uuid:" + d.fDeviceID;

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
