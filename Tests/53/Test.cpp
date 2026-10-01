/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Frameworks::UPnP
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h" // for qStroika_Foundation_DataExchange_XML_SupportParsing
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/IntervalTimer.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/Interface.h"

#include "Stroika/Frameworks/Test/TestHarness.h"
#include "Stroika/Frameworks/UPnP/Device.h"
#include "Stroika/Frameworks/UPnP/DeviceDescription.h"
#include "Stroika/Frameworks/UPnP/SSDP/Advertisement.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/Search.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/BasicServer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Frameworks_UPnP, SSDP_Notify_RoundTrip_)
    {
        Debug::TraceContextBumper ctx{"SSDP_Notify_RoundTrip_"};
        for (bool alive : {true, false}) {
            SSDP::Advertisement a;
            a.fAlive    = alive;
            a.fUSN      = "uuid:315caae0-1335-57bf-a178-24c9ee756627::upnp:rootdevice"sv;
            a.fLocation = URI{"http://192.168.1.2:8080/device.xml"sv};
            a.fServer   = "Linux/6.8 UPnP/1.0 StroikaTest/1.0"sv;
            a.fTarget   = SSDP::kTarget_UPNPRootDevice;

            Memory::BLOB data = SSDP::Serialize ("NOTIFY * HTTP/1.1"sv, SSDP::SearchOrNotify::Notify, a);

            String              headLine;
            SSDP::Advertisement b;
            SSDP::DeSerialize (data, &headLine, &b);
            EXPECT_EQ (headLine, "NOTIFY * HTTP/1.1"sv);
            EXPECT_EQ (b.fAlive, a.fAlive);
            EXPECT_EQ (b.fUSN, a.fUSN);
            EXPECT_EQ (b.fLocation, a.fLocation);
            EXPECT_EQ (b.fServer, a.fServer);
            EXPECT_EQ (b.fTarget, a.fTarget);
            // and every header the packet carried is kept, raw
            EXPECT_EQ (b.fRawHeaders.LookupValue ("Host"sv), "239.255.255.250:1900"sv);
            EXPECT_EQ (b.fRawHeaders.LookupValue ("NTS"sv), alive ? "ssdp:alive"sv : "ssdp:byebye"sv);
        }
    }

    GTEST_TEST (Frameworks_UPnP, DeviceDescription_RoundTrip_)
    {
        Debug::TraceContextBumper ctx{"DeviceDescription_RoundTrip_"};
        DeviceDescription         dd;
        dd.fPresentationURL  = URI{"http://www.sophists.com/"sv};
        dd.fDeviceType       = "urn:sophists.com:device:deviceType:1.0"sv;
        dd.fManufactureName  = "Sophist Solutions, Inc."sv;
        dd.fFriendlyName     = "Stroika regression test device"sv;
        dd.fManufacturingURL = URI{"http://www.sophists.com/"sv};
        dd.fModelDescription = "long user-friendly title"sv;
        dd.fModelName        = "model name"sv;
        dd.fModelNumber      = "model number"sv;
        dd.fModelURL         = URI{"http://www.sophists.com/"sv};
        dd.fSerialNumber     = "manufacturer's serial number"sv;
        dd.fUDN              = "uuid:315caae0-1335-57bf-a178-24c9ee756627"sv;

        Memory::BLOB xml = UPnP::Serialize (dd);
        DbgTrace ("xml: {}"_f, String::FromUTF8 (xml.As<string> ()));
#if qStroika_Foundation_DataExchange_XML_SupportParsing
        DeviceDescription back = UPnP::DeSerialize (xml);
        EXPECT_EQ (back.fPresentationURL, dd.fPresentationURL);
        EXPECT_EQ (back.fDeviceType, dd.fDeviceType);
        EXPECT_EQ (back.fManufactureName, dd.fManufactureName);
        EXPECT_EQ (back.fFriendlyName, dd.fFriendlyName);
        EXPECT_EQ (back.fManufacturingURL, dd.fManufacturingURL);
        EXPECT_EQ (back.fModelDescription, dd.fModelDescription);
        EXPECT_EQ (back.fModelName, dd.fModelName);
        EXPECT_EQ (back.fModelNumber, dd.fModelNumber);
        EXPECT_EQ (back.fModelURL, dd.fModelURL);
        EXPECT_EQ (back.fSerialNumber, dd.fSerialNumber);
        EXPECT_EQ (back.fUDN, dd.fUDN);
#else
        Stroika::Frameworks::Test::WarnTestIssue (
            "DeviceDescription_RoundTrip_ only checks Serialize: this configuration has no XML parser");
#endif
    }

    GTEST_TEST (Frameworks_UPnP, SSDP_SearchResponse_And_MSearch_Parse_)
    {
        Debug::TraceContextBumper ctx{"SSDP_SearchResponse_And_MSearch_Parse_"};
        {
            // a search response carries its target as ST, not NT
            SSDP::Advertisement a;
            a.fUSN                   = "uuid:315caae0-1335-57bf-a178-24c9ee756627::upnp:rootdevice"sv;
            a.fLocation              = URI{"http://192.168.1.2:8080/device.xml"sv};
            a.fServer                = "Linux/6.8 UPnP/1.0 StroikaTest/1.0"sv;
            a.fTarget                = SSDP::kTarget_UPNPRootDevice;
            Memory::BLOB        data = SSDP::Serialize ("HTTP/1.1 200 OK"sv, SSDP::SearchOrNotify::SearchResponse, a);
            String              headLine;
            SSDP::Advertisement b;
            SSDP::DeSerialize (data, &headLine, &b);
            EXPECT_EQ (headLine, "HTTP/1.1 200 OK"sv);
            EXPECT_EQ (b.fTarget, a.fTarget);
            EXPECT_EQ (b.fUSN, a.fUSN);
            EXPECT_EQ (b.fLocation, a.fLocation);
            EXPECT_EQ (b.fServer, a.fServer);
            EXPECT_EQ (b.fAlive, nullopt); // only a NOTIFY says alive or byebye
        }
        {
            // an M-SEARCH, as SearchResponder parses it
            constexpr string_view kMSearch_ =
                "M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 2\r\nST: ssdp:all\r\n\r\n";
            String              headLine;
            SSDP::Advertisement b;
            SSDP::DeSerialize (Memory::BLOB{as_bytes (span{kMSearch_})}, &headLine, &b);
            EXPECT_EQ (headLine, "M-SEARCH * HTTP/1.1"sv);
            EXPECT_EQ (b.fTarget, SSDP::kTarget_SSDPAll);
            EXPECT_EQ (b.fRawHeaders.LookupValue ("MAN"sv), "\"ssdp:discover\""sv);
        }
        {
            // a Location that is not a URI is treated as missing - the rest of the packet still counts
            constexpr string_view kBadLocation_ =
                "HTTP/1.1 200 OK\r\nLOCATION: http://[not a uri\r\nST: upnp:rootdevice\r\nUSN: uuid:x::upnp:rootdevice\r\n\r\n";
            String              headLine;
            SSDP::Advertisement b;
            SSDP::DeSerialize (Memory::BLOB{as_bytes (span{kBadLocation_})}, &headLine, &b);
            EXPECT_EQ (b.fLocation, URI{});
            EXPECT_EQ (b.fTarget, SSDP::kTarget_UPNPRootDevice);
            EXPECT_EQ (b.fUSN, "uuid:x::upnp:rootdevice"sv);
        }
    }

    /*
     *  Packets real devices sent (captured on a LAN 2026-10-01, then anonymized: addresses moved to 192.0.2.x, and every
     *  UUID, MAC address and household id replaced) - every byte else as sent, quirks included: empty values ("EXT:"),
     *  odd spacing ("max-age = 1800"), header names starting with a digit or containing dots, quoted values with
     *  semicolons, and each vendor's own header order.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_RealDevicePackets_)
    {
        Debug::TraceContextBumper ctx{"SSDP_RealDevicePackets_"};
        struct Fixture_ {
            const char*    fName;
            const char*    fPacket;
            const char*    fHeadLine;
            const char*    fTarget;
            const char*    fUSN;
            const char*    fLocation;
            const char*    fServer;
            optional<bool> fAlive;
            size_t         fHeaderCount;
        };
        static const Fixture_ kFixtures_[] = {
            {"Sonos speaker",
             "HTTP/1.1 200 OK\r\n"
             "CACHE-CONTROL: max-age = 1800\r\n"
             "EXT:\r\n"
             "LOCATION: http://192.0.2.163:1400/xml/device_description.xml\r\n"
             "SERVER: Linux UPnP/1.0 Sonos/86.10-80260 (ZPS6)\r\n"
             "ST: upnp:rootdevice\r\n"
             "USN: uuid:RINCON_00000000000101400::upnp:rootdevice\r\n"
             "X-RINCON-HOUSEHOLD: Sonos_TestHousehold\r\n"
             "X-RINCON-BOOTSEQ: 4878\r\n"
             "BOOTID.UPNP.ORG: 4878\r\n"
             "X-RINCON-VARIANT: 2\r\n"
             "HOUSEHOLD.SMARTSPEAKER.AUDIO: Sonos_TestHousehold.TestHouseholdKey\r\n"
             "LOCATION.SMARTSPEAKER.AUDIO: lc_00000000000000000000000000000000\r\n"
             "SECURELOCATION.UPNP.ORG: https://192.0.2.163:1443/xml/device_description.xml\r\n"
             "X-SONOS-HHSECURELOCATION: https://192.0.2.163:1843/xml/device_description.xml\r\n"
             "\r\n",
             "HTTP/1.1 200 OK", "upnp:rootdevice", "uuid:RINCON_00000000000101400::upnp:rootdevice",
             "http://192.0.2.163:1400/xml/device_description.xml", "Linux UPnP/1.0 Sonos/86.10-80260 (ZPS6)", nullopt, 14},
            {"Samsung TV",
             "HTTP/1.1 200 OK\r\n"
             "CACHE-CONTROL: max-age=1800\r\n"
             "DATE: Thu, 01 Oct 2026 18:45:28 GMT\r\n"
             "EXT: \r\n"
             "LOCATION: http://192.0.2.25:7678/nservice/\r\n"
             "SERVER: Samsung-Linux/4.1, UPnP/1.0, Samsung_UPnP_SDK/1.0\r\n"
             "ST: upnp:rootdevice\r\n"
             "USN: uuid:00000000-0000-4000-8000-000000000001::upnp:rootdevice\r\n"
             "Content-Length: 0\r\n"
             "BOOTID.UPNP.ORG: 4\r\n"
             "\r\n",
             "HTTP/1.1 200 OK", "upnp:rootdevice", "uuid:00000000-0000-4000-8000-000000000001::upnp:rootdevice",
             "http://192.0.2.25:7678/nservice/", "Samsung-Linux/4.1, UPnP/1.0, Samsung_UPnP_SDK/1.0", nullopt, 9},
            {"Netgear switch",
             "HTTP/1.1 200 OK\r\n"
             "CACHE-CONTROL: max-age=120\r\n"
             "ST: upnp:rootdevice\r\n"
             "USN: uuid:00000000-0000-4000-8000-000000000002::upnp:rootdevice\r\n"
             "EXT:\r\n"
             "SERVER: Netgear_Switch UPnP/1.1 FANTASTIC4_V3/2.2.2\r\n"
             "LOCATION: http://192.0.2.9:43691/rootDesc.xml\r\n"
             "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
             "01-NLS: 1767225678\r\n"
             "BOOTID.UPNP.ORG: 1767225678\r\n"
             "CONFIGID.UPNP.ORG: 1337\r\n"
             "\r\n",
             "HTTP/1.1 200 OK", "upnp:rootdevice", "uuid:00000000-0000-4000-8000-000000000002::upnp:rootdevice",
             "http://192.0.2.9:43691/rootDesc.xml", "Netgear_Switch UPnP/1.1 FANTASTIC4_V3/2.2.2", nullopt, 10},
            {"Chromecast",
             "HTTP/1.1 200 OK\r\n"
             "CACHE-CONTROL: max-age=1800\r\n"
             "DATE: Thu, 01 Oct 2026 18:45:37 GMT\r\n"
             "EXT:\r\n"
             "LOCATION: http://192.0.2.127:8008/ssdp/device-desc.xml\r\n"
             "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
             "01-NLS: 00000000-0000-4000-8000-000000000003\r\n"
             "SERVER: Linux/5.15.170-android14-11-gf4a1f03072af, UPnP/1.0, Chromecast/1.6.18\r\n"
             "X-User-Agent: redsonic\r\n"
             "ST: upnp:rootdevice\r\n"
             "USN: uuid:00000000-0000-4000-8000-000000000004::upnp:rootdevice\r\n"
             "BOOTID.UPNP.ORG: 0\r\n"
             "CONFIGID.UPNP.ORG: 1\r\n"
             "\r\n",
             "HTTP/1.1 200 OK", "upnp:rootdevice", "uuid:00000000-0000-4000-8000-000000000004::upnp:rootdevice",
             "http://192.0.2.127:8008/ssdp/device-desc.xml", "Linux/5.15.170-android14-11-gf4a1f03072af, UPnP/1.0, Chromecast/1.6.18", nullopt, 12},
            {"Portable SDK for UPnP (libupnp)",
             "HTTP/1.1 200 OK\r\n"
             "CACHE-CONTROL: max-age=1900\r\n"
             "DATE: Thu, 01 Oct 2026 18:45:37 GMT\r\n"
             "EXT:\r\n"
             "LOCATION: http://192.0.2.39:50001/desc/device.xml\r\n"
             "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
             "01-NLS: 00000000-0000-4000-8000-000000000005\r\n"
             "SERVER: Linux/4.4.302+, UPnP/1.0, Portable SDK for UPnP devices/1.12.1\r\n"
             "X-User-Agent: redsonic\r\n"
             "ST: upnp:rootdevice\r\n"
             "USN: uuid:00000000-0000-4000-8000-000000000006::upnp:rootdevice\r\n"
             "\r\n",
             "HTTP/1.1 200 OK", "upnp:rootdevice", "uuid:00000000-0000-4000-8000-000000000006::upnp:rootdevice",
             "http://192.0.2.39:50001/desc/device.xml", "Linux/4.4.302+, UPnP/1.0, Portable SDK for UPnP devices/1.12.1", nullopt, 10},
            {"Private Upnp SDK",
             "HTTP/1.1 200 OK\r\n"
             "CACHE-CONTROL: max-age=1800\r\n"
             "DATE: Thu, 01 Oct 2026 14:45:37 GMT\r\n"
             "EXT:\r\n"
             "X-User-Agent: redsonic\r\n"
             "LOCATION: http://192.0.2.19:80/upnp_device_desc.xml\r\n"
             "SERVER: Linux, UPnP/1.0, Private Upnp SDK\r\n"
             "ST: upnp:rootdevice\r\n"
             "USN: uuid:device_3_0-AMC000000000000000::upnp:rootdevice\r\n"
             "\r\n",
             "HTTP/1.1 200 OK", "upnp:rootdevice", "uuid:device_3_0-AMC000000000000000::upnp:rootdevice",
             "http://192.0.2.19:80/upnp_device_desc.xml", "Linux, UPnP/1.0, Private Upnp SDK", nullopt, 8},
            {"Windows UPnP Device Host NOTIFY",
             "NOTIFY * HTTP/1.1\r\n"
             "Host: 239.255.255.250:1900\r\n"
             "NT: urn:schemas-upnp-org:device:MediaRenderer:1\r\n"
             "NTS: ssdp:alive\r\n"
             "Location: http://192.0.2.48:2869/upnphost/udhisapi.dll?content=uuid:00000000-0000-4000-8000-000000000007\r\n"
             "USN: uuid:00000000-0000-4000-8000-000000000007::urn:schemas-upnp-org:device:MediaRenderer:1\r\n"
             "Cache-Control: max-age=1800\r\n"
             "Server: Microsoft-Windows/10.0 UPnP/1.0 UPnP-Device-Host/1.0\r\n"
             "OPT:\"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
             "01-NLS: 00000000000000000000000000000000\r\n"
             "\r\n",
             "NOTIFY * HTTP/1.1", "urn:schemas-upnp-org:device:MediaRenderer:1", "uuid:00000000-0000-4000-8000-000000000007::urn:schemas-upnp-org:device:MediaRenderer:1",
             "http://192.0.2.48:2869/upnphost/udhisapi.dll?content=uuid:00000000-0000-4000-8000-000000000007",
             "Microsoft-Windows/10.0 UPnP/1.0 UPnP-Device-Host/1.0", true, 9},
        };
        for (const Fixture_& f : kFixtures_) {
            String              headLine;
            SSDP::Advertisement a;
            SSDP::DeSerialize (Memory::BLOB{as_bytes (span{string_view{f.fPacket}})}, &headLine, &a);
            EXPECT_EQ (headLine, String{f.fHeadLine}) << f.fName;
            EXPECT_EQ (a.fTarget, String{f.fTarget}) << f.fName;
            EXPECT_EQ (a.fUSN, String{f.fUSN}) << f.fName;
            EXPECT_EQ (a.fLocation, URI{String{f.fLocation}}) << f.fName;
            EXPECT_EQ (a.fServer, String{f.fServer}) << f.fName;
            EXPECT_EQ (a.fAlive, f.fAlive) << f.fName;
            EXPECT_EQ (a.fRawHeaders.size (), f.fHeaderCount) << f.fName;
        }
    }

    /*
     *  A real SSDP exchange: our own BasicServer answering our own Search, both in this process (the server's responder
     *  turns multicast loopback on).
     *
     *  Whether that can happen at all depends on the environment, not on Stroika: binding UDP 1900 may be refused, or the
     *  port shared with another SSDP service; multicast may be filtered (a firewall, a container network, macOS's Local
     *  Network privacy). So anything that keeps the exchange from happening is reported as a test issue, never a failure.
     *  What IS a failure: an answer that comes back wrong.
     *
     *  The device type is unique to this run, so no other device on the network answers - and only our device's
     *  announcements (a few seconds of them, TTL 4) reach the network.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Loopback_Search_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_Loopback_Search_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by BasicServer
        const String                                 deviceID   = Common::GUID::GenerateNew ().As<String> ();
        const String                                 deviceType = "urn:stroika-regression-test:device:SSDPLoopback-{}:1"_f(deviceID);
        constexpr uint16_t                           kPort_     = 49152; // only advertised - nothing listens on it

        Device d;
        d.fDeviceID = deviceID;
        d.fLocation.SetScheme (URI::SchemeType{"http"sv});
        d.fLocation.SetAuthority (URI::Authority{nullopt, kPort_}); // no host - BasicServer fills in one of ours
        d.fLocation.SetPath ("/device.xml"sv);
        d.fServer = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
        DeviceDescription dd;
        dd.fDeviceType   = deviceType;
        dd.fFriendlyName = "Stroika regression test device"sv;
        dd.fUDN          = "uuid:" + deviceID;

        Execution::Synchronized<optional<SSDP::Advertisement>> found;
        Execution::WaitableEvent                               foundEvent;
        try {
            using IO::Network::InternetProtocol::IP::IPVersionSupport;
            SSDP::Server::BasicServer server{d, dd, SSDP::Server::BasicServer::FrequencyInfo{}, IPVersionSupport::eIPV4Only};
            SSDP::Client::Search      search{[&] (const SSDP::Advertisement& a) {
                                            if (a.fTarget == deviceType) {
                                                found.store (a);
                                                foundEvent.Set ();
                                            }
                                             },
                                             deviceType, nullopt, IPVersionSupport::eIPV4Only};
            (void)foundEvent.WaitQuietly (10s);
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue ("SSDP_Loopback_Search_ skipped - could not run an SSDP server and search here: {}"_f(current_exception ())
                                                          .AsNarrowSDKString ()
                                                          .c_str ());
            return;
        }
        optional<SSDP::Advertisement> a = found.load ();
        if (not a) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_Search_ skipped - our own device was not found within 10 seconds (this "
                "environment probably blocks multicast, or UDP 1900)");
            return;
        }
        DbgTrace ("found: {}"_f, a);
        EXPECT_EQ (a->fUSN, "uuid:{}::{}"_f(deviceID, deviceType));
        EXPECT_EQ (a->fLocation.GetScheme (), URI::SchemeType{"http"sv});
        EXPECT_EQ (a->fLocation.GetPath (), "/device.xml"sv);
        optional<URI::Authority> authority = a->fLocation.GetAuthority ();
        EXPECT_TRUE (authority and authority->GetPort () == kPort_);
        // the host filled in must be one of this machine's own addresses
        optional<InternetAddress> host = authority and authority->GetHost () ? authority->GetHost ()->AsInternetAddress () : nullopt;
        EXPECT_TRUE (host.has_value ()) << Characters::ToString (a->fLocation).AsNarrowSDKString ();
        if (host) {
            bool mine = host->IsLocalhostAddress () or
                        SystemInterfacesMgr{}.GetAll ().Any ([&] (const Interface& i) { return i.fBindings.fAddresses.Contains (*host); });
            EXPECT_TRUE (mine) << Characters::ToString (*host).AsNarrowSDKString () << " is not one of this machine's addresses";
        }
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "[  SKIPPED ] every test - Stroika regression tests require building with google test feature" << endl;
#endif
}
