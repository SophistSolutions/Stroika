/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <mutex>
#include    <iostream>

#include    "Stroika/Foundation/Characters/Format.h"
#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/Event.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include    "Stroika/Foundation/IO/Network/NetworkInterfaces.h"
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

        WebServerForDeviceDescription_ (uint16_t webServerPortNumber, Memory::BLOB deviceDescription)
            : fListener ()
        {
            auto onConnect = [deviceDescription](Socket s) {
                Execution::Thread runConnectionOnAnotherThread ([s, deviceDescription]() {
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
            //fListener = std::move (Listener (SocketAddress (Network::V4::kAddrAny, webServerPortNumber), onConnect));
            fListener = shared_ptr<Listener> (new Listener (SocketAddress (Network::V4::kAddrAny, webServerPortNumber), onConnect));
        }

        // @todo - FIX OPTIONAL (and related code) -so Optional<> works here - rvalue -references
        //Memory::Optional<Listener>    fListener;
        shared_ptr<Listener>    fListener;
    };
}





int main (int argc, const char* argv[])
{
    try {
        uint16_t    portForOurWS = 8080;

        Device  d;
        d.fLocation = Characters::Format (L"http://%s:%d", IO::Network::GetPrimaryInternetAddress ().As<String> ().c_str (), portForOurWS);
        d.fServer = L"unix/5.1 UPnP/1.1 MyProduct/1.0";     // @todo wrong - to be fixed
        d.fDeviceID = UPnP::MungePrimaryMacAddrIntoBaseDeviceID (L"315CAAE0-1335-57BF-A178-24C9EE756627");

        DeviceDescription   deviceInfo;
        deviceInfo.fPresentationURL = L"http://www.sophists.com/";
        deviceInfo.fDeviceType = L"urn:sophists.com:device:deviceType:1.0";
        deviceInfo.fManufactureName = L"Sophist Solutions, Inc.";
        deviceInfo.fFriendlyName = L"Sophist Solutions fake device";
        deviceInfo.fManufacturingURL = L"http://www.sophists.com/";
        deviceInfo.fModelDescription = L"long user-friendly title";
        deviceInfo.fModelName = L"model name";
        deviceInfo.fModelNumber = L"model number";
        deviceInfo.fModelURL = L"http://www.sophists.com/";
        deviceInfo.fSerialNumber = L"manufacturer's serial number";

        WebServerForDeviceDescription_  deviceWS (portForOurWS, Stroika::Frameworks::UPnP::Serialize (d, deviceInfo));
        BasicServer b (d, BasicServer::FrequencyInfo ());
        Execution::Event ().Wait ();    // wait forever - til user hits ctrl-c
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
