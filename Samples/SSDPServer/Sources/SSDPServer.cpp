/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <mutex>
#include    <iostream>
#include    <sstream>

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
using   namespace Stroika::Frameworks::UPnP;
using   namespace Stroika::Frameworks::UPnP::SSDP;
using   namespace Stroika::Frameworks::WebServer;


using   Characters::String;
using   Containers::Sequence;
using   Memory::Optional;
using   Server::BasicServer;


/*
 *Example based on
   http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf

 */

Memory::BLOB mkDoc_ (const string& presentationURL)
{
    stringstream tmp;
    tmp << "<?xml version=\"1.0\"?>" << endl;
    tmp << " <root xmlns=\"urn:schemas-upnp-org:device-1-0\"" << endl;
    tmp << " configId=\"configuration number\"> " << endl;
    tmp << " <specVersion> " << endl;
    tmp << " <major>1</major> " << endl;
    tmp << " <minor>1</minor> " << endl;
    tmp << " </specVersion> " << endl;
    tmp << " <device> " << endl;
    tmp << " <deviceType>urn:schemas-upnp-org:device:deviceType:v</deviceType> " << endl;
    tmp << " <friendlyName>short user-friendly title</friendlyName> " << endl;
    tmp << " <manufacturer>manufacturer name</manufacturer> " << endl;
    tmp << " <manufacturerURL>URL to manufacturer site</manufacturerURL> " << endl;
    tmp << " <modelDescription>long user-friendly title</modelDescription> " << endl;
    tmp << " <modelName>model name</modelName> " << endl;
    tmp << " <modelNumber>model number</modelNumber> " << endl;
    tmp << " <modelURL>URL to model site</modelURL> " << endl;
    tmp << " <serialNumber>manufacturer's serial number</serialNumber> " << endl;
    tmp << " <UDN>uuid:UUID</UDN> " << endl;
    tmp << " <UPC>Universal Product Code</UPC> " << endl;
    tmp << " <iconList> " << endl;
    tmp << " <icon> " << endl;
    tmp << " <mimetype>image/format</mimetype> " << endl;
    tmp << " <width>horizontal pixels</width> " << endl;
    tmp << " <height>vertical pixels</height> " << endl;
    tmp << " <depth>color depth</depth> " << endl;
    tmp << " <url>URL to icon</url> " << endl;
    tmp << " </icon> " << endl;
    // <!-- XML to declare other icons, if any, go here -->
    tmp << " </iconList> " << endl;
    tmp << " <serviceList> " << endl;
    tmp << " <service> " << endl;
    tmp << " <serviceType>urn:schemas-upnp-org:service:serviceType:v</serviceType> " << endl;
    tmp << " <serviceId>urn:upnp-org:serviceId:serviceID</serviceId> " << endl;
    tmp << " <SCPDURL>URL to service description</SCPDURL> " << endl;
    tmp << " <controlURL>URL for control</controlURL> " << endl;
    tmp << " <eventSubURL>URL for eventing</eventSubURL> " << endl;
    tmp << " </service> " << endl;
    // <!-- Declarations for other services defined by a UPnP Forum working committee
    // (if any) go here -->
    // <!-- Declarations for other services added by UPnP vendor (if any) go here -->
    tmp << " </serviceList> " << endl;
    tmp << " <deviceList> " << endl;
    // <!-- Description of embedded devices defined by a UPnP Forum working committee
    // (if any) go here -->
    // <!-- Description of embedded devices added by UPnP vendor (if any) go here -->
    tmp << " </deviceList> " << endl;
    tmp << " <presentationURL>" << presentationURL << "</presentationURL>" << endl;
    tmp << " </device> " << endl;
    tmp << " </root> " << endl;
    Streams::iostream::BinaryInputStreamFromIStreamAdapter tmp2 (tmp);
    return tmp2.ReadAll ();
}

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
        WebServerForDeviceDescription_  deviceWS (portForOurWS, mkDoc_ ("http://www.google.com"));
        Device  d;
        d.fLocation = Characters::Format (L"http://%s:%d", IO::Network::GetPrimaryInternetAddress ().As<String> ().c_str (), portForOurWS);
        d.fServer = L"sample-stroika-ssdp-server";
        d.fUSN = L"uuid:TESTUID";
        d.fST = L"upnp:rootdevice";
        BasicServer b (d, BasicServer::FrequencyInfo ());
        Execution::Event ().Wait ();    // wait forever - til user hits ctrl-c
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
