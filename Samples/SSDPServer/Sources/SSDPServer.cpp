/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <mutex>
#include    <iostream>

#include    "Stroika/Foundation/Characters/Format.h"
#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/WaitableEvent.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include    "Stroika/Foundation/IO/Network/LinkMonitor.h"
#include    "Stroika/Foundation/IO/Network/Listener.h"
#include    "Stroika/Foundation/Streams/iostream/BinaryInputStreamFromIStreamAdapter.h"
#include    "Stroika/Frameworks/UPnP/SSDP/Server/BasicServer.h"
#include    "Stroika/Frameworks/WebServer/ConnectionManager.h"


using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Foundation::IO;
using   namespace Stroika::Foundation::IO::Network;
using   namespace Stroika::Frameworks;
using   namespace Stroika::Frameworks::UPnP;
using   namespace Stroika::Frameworks::UPnP::SSDP;
using   namespace Stroika::Frameworks::WebServer;


using   Characters::String;
using   Containers::Sequence;
using   Memory::Optional;
using   Server::BasicServer;



namespace {
    struct WebServerForDeviceDescription_ {

        WebServerForDeviceDescription_ (uint16_t webServerPortNumber, const Device& d, const DeviceDescription& dd)
            : fListener ()
        {
            auto onConnect = [d, dd](Socket s) {
                Execution::Thread runConnectionOnAnotherThread ([s, d, dd]() {
                    // If the URLs are served locally, you may want to update the URL based on
                    // IO::Network::GetPrimaryInternetAddress ()
                    Memory::BLOB deviceDescription  =   Stroika::Frameworks::UPnP::Serialize (d, dd);
                    // now read
                    Connection conn (s);
                    conn.ReadHeaders ();    // bad API. Must rethink...
                    conn.GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kServer, L"stroika-ssdp-server-demo");
                    conn.GetResponse ().write (deviceDescription.begin (), deviceDescription.end ());
                    conn.GetResponse ().SetContentType (DataExchange::PredefinedInternetMediaType::XML_CT ());
                    conn.GetResponse ().End ();
                });
                runConnectionOnAnotherThread.SetThreadName (L"Connection Thread");  // SHOULD use a fancier name (connection#)
                runConnectionOnAnotherThread.Start ();
                //runConnectionOnAnotherThread.WaitForDone ();    // maybe save these in connection mgr so we can force them all shut down...
            };
            fListener = Optional<Listener> (Listener (SocketAddress (Network::V4::kAddrAny, webServerPortNumber), onConnect));
        }

        Optional<Listener>    fListener;
    };
}





int main (int argc, const char* argv[])
{
    try {
        uint16_t    portForOurWS = 8080;

        Device  d;
        d.fLocation.SetPortNumber (portForOurWS);
        d.fServer = L"unix/5.1 UPnP/1.0 MyProduct/1.0";     // @todo wrong - to be fixed
        d.fDeviceID = UPnP::MungePrimaryMacAddrIntoBaseDeviceID (L"315CAAE0-1335-57BF-A178-24C9EE756627");

        DeviceDescription   deviceInfo;
        deviceInfo.fPresentationURL = URL (L"http://www.sophists.com/");
        deviceInfo.fDeviceType = L"urn:sophists.com:device:deviceType:1.0";
        deviceInfo.fManufactureName = L"Sophist Solutions, Inc.";
        deviceInfo.fFriendlyName = L"Sophist Solutions fake device";
        deviceInfo.fManufacturingURL = URL (L"http://www.sophists.com/");
        deviceInfo.fModelDescription = L"long user-friendly title";
        deviceInfo.fModelName = L"model name";
        deviceInfo.fModelNumber = L"model number";
        deviceInfo.fModelURL = URL (L"http://www.sophists.com/");
        deviceInfo.fSerialNumber = L"manufacturer's serial number";

        WebServerForDeviceDescription_  deviceWS (portForOurWS, d, deviceInfo);
        BasicServer b (d, BasicServer::FrequencyInfo ());
        Execution::WaitableEvent (Execution::WaitableEvent::eAutoReset).Wait ();    // wait forever - til user hits ctrl-c
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
