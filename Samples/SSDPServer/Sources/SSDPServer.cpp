/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String_Constant.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
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
            : fListener ()
        {
            // @todo Consider simplifying this using WebServer Framework more fully - Router or Interceptor
            auto onConnect = [dd] (const ConnectionOrientedStreamSocket::Ptr& acceptedSocketConnection) {
                Execution::Thread::Ptr runConnectionOnAnotherThread = Execution::Thread::New ([acceptedSocketConnection, dd] () {
                    // If the URLs are served locally, you may want to update the URL based on
                    // IO::Network::GetPrimaryInternetAddress ()
                    Connection conn{acceptedSocketConnection,
                                    Sequence<Interceptor>{
                                        Interceptor (
                                            [=] (Message* m) {
                                                Response* response = m->PeekResponse ();
                                                response->AddHeader (IO::Network::HTTP::HeaderName::kServer, L"stroika-ssdp-server-demo");
                                                response->write (Stroika::Frameworks::UPnP::Serialize (dd));
                                                response->SetContentType (DataExchange::PredefinedInternetMediaType::kText_XML);
                                            })}};
                    conn.SetRemainingConnectionMessages (Connection::Remaining{0, 0}); // disable keep-alives
                    conn.ReadAndProcessMessage ();
                });
                runConnectionOnAnotherThread.SetThreadName (L"SSDP Servcie Connection Thread");
                runConnectionOnAnotherThread.Start ();
            };
            fListener = Listener (SocketAddresses (InternetAddresses_Any (), webServerPortNumber), onConnect);
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
    try {
        uint16_t portForOurWS = 8080;

        Device d;
        d.fLocation.SetPortNumber (portForOurWS);
        d.fServer   = UPnP::SSDP::MakeServerHeaderValue (L"MyStroikaBasedSampleProduct/1.0"_k);
        d.fDeviceID = UPnP::MungePrimaryMacAddrIntoBaseDeviceID (L"315CAAE0-1335-57BF-A178-24C9EE756627"_k);

        DeviceDescription deviceInfo;
        deviceInfo.fPresentationURL  = URL::Parse (L"http://www.sophists.com/"_k);
        deviceInfo.fDeviceType       = L"urn:sophists.com:device:deviceType:1.0"_k;
        deviceInfo.fManufactureName  = L"Sophist Solutions, Inc."_k;
        deviceInfo.fFriendlyName     = L"Sophist Solutions fake device"_k;
        deviceInfo.fManufacturingURL = URL::Parse (L"http://www.sophists.com/"_k);
        deviceInfo.fModelDescription = L"long user-friendly title"_k;
        deviceInfo.fModelName        = L"model name"_k;
        deviceInfo.fModelNumber      = L"model number"_k;
        deviceInfo.fModelURL         = URL::Parse (L"http://www.sophists.com/"_k);
        deviceInfo.fSerialNumber     = L"manufacturer's serial number"_k;
        deviceInfo.fUDN              = L"uuid:" + d.fDeviceID;

        WebServerForDeviceDescription_ deviceWS (portForOurWS, deviceInfo);
        BasicServer                    b{d, deviceInfo, BasicServer::FrequencyInfo ()};
        Execution::WaitableEvent{}.Wait (); // wait forever - til user hits ctrl-c
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
        cerr << "Exception - " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
