/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Frameworks::UPnP
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h" // for qStroika_Foundation_DataExchange_XML_SupportParsing
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/IntervalTimer.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/Interface.h"

#include "Stroika/Frameworks/Test/TestHarness.h"
#include "Stroika/Frameworks/UPnP/Device.h"
#include "Stroika/Frameworks/UPnP/DeviceDescription.h"
#include "Stroika/Frameworks/UPnP/SSDP/Advertisement.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/Listener.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/Search.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/BasicServer.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/LocationProvider.h"

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

    GTEST_TEST (Frameworks_UPnP, SSDP_MaxAge_)
    {
        Debug::TraceContextBumper ctx{"SSDP_MaxAge_"};
        // an announcement must say it is good until at least the next one - and the UPnP Device Architecture says its
        // CACHE-CONTROL max-age SHOULD be at least 1800 seconds
        SSDP::Advertisement a;
        a.fAlive    = true;
        a.fUSN      = "uuid:315caae0-1335-57bf-a178-24c9ee756627::upnp:rootdevice"sv;
        a.fLocation = URI{"http://192.168.1.2:8080/device.xml"sv};
        a.fTarget   = SSDP::kTarget_UPNPRootDevice;
        String              headLine;
        SSDP::Advertisement b;
        SSDP::DeSerialize (SSDP::Serialize ("NOTIFY * HTTP/1.1"sv, SSDP::SearchOrNotify::Notify, a), &headLine, &b);
        EXPECT_EQ (b.fRawHeaders.LookupValue ("Cache-Control"sv), "max-age=1800"sv);
        EXPECT_EQ (b.fMaxAge, Time::Duration{1800.0});

        // its own, when it has one
        a.fMaxAge = Time::Duration{900.0};
        SSDP::DeSerialize (SSDP::Serialize ("NOTIFY * HTTP/1.1"sv, SSDP::SearchOrNotify::Notify, a), &headLine, &b);
        EXPECT_EQ (b.fMaxAge, a.fMaxAge);

        // and read however another device writes it
        static const char kOtherDevice_[] = "NOTIFY * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nCACHE-CONTROL: max-age = 120\r\nNT: "
                                            "upnp:rootdevice\r\nNTS: ssdp:alive\r\n\r\n";
        SSDP::DeSerialize (Memory::BLOB::FromRaw (kOtherDevice_, std::size (kOtherDevice_) - 1), &headLine, &b);
        EXPECT_EQ (b.fMaxAge, Time::Duration{120.0});

        // and kept by Advertisement::kMapper
        EXPECT_EQ (SSDP::Advertisement::kMapper->ToObject<SSDP::Advertisement> (SSDP::Advertisement::kMapper->FromObject (b)).fMaxAge, b.fMaxAge);
    }

    GTEST_TEST (Frameworks_UPnP, Device_Mapper_)
    {
        // each field serialized under a name saying what it holds - and back
        Debug::TraceContextBumper ctx{"Device_Mapper_"};
        Device                    d;
        d.fDeviceID = "315CAAE0-668D-47C7-A178-24C9EE756627"sv;
        d.fLocation = URI{"http://192.168.1.2:8080/device.xml"};
        d.fServer   = "Linux/6.1 UPnP/1.0 Stroika/3.0"sv;
        Containers::Mapping<String, DataExchange::VariantValue> m =
            Device::kMapper.FromObject (d).As<Containers::Mapping<String, DataExchange::VariantValue>> ();
        EXPECT_EQ (m.Keys ().As<Containers::Set<String>> (), (Containers::Set<String>{"Device-ID"sv, "Location"sv, "Server"sv}));
        EXPECT_EQ (m.LookupValue ("Device-ID"sv).As<String> (), "315caae0-668d-47c7-a178-24c9ee756627"sv); // a GUID's text: lower case
        EXPECT_EQ (m.LookupValue ("Location"sv).As<String> (), d.fLocation.As<String> ());
        Device back = Device::kMapper.ToObject<Device> (Device::kMapper.FromObject (d));
        EXPECT_EQ (back.fDeviceID, d.fDeviceID);
        EXPECT_EQ (back.fLocation, d.fLocation);
        EXPECT_EQ (back.fServer, d.fServer);
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
     *  The ready-made location providers - what each advertises where.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_LocationProviders_)
    {
        Debug::TraceContextBumper ctx{"SSDP_LocationProviders_"};
        using namespace SSDP::Server;
        const InternetAddress kLAN_{"192.168.1.5"sv};
        const InternetAddress kOtherLAN_{"10.0.0.7"sv};
        const InternetAddress kV6_{"2001:db8::5"sv};
        const InternetAddress kV6LinkLocal_{"fe80::5"sv};
        auto                  at  = [] (const InternetAddress& a) { return LocationContext{a, nullopt}; };
        auto                  str = [] (const optional<URI>& u) { return u ? optional<String>{u->As<String> ()} : nullopt; };
        // LocationFromBindings: a wildcard binding covers its own family only; a specific one only its own address
        {
            LocationProvider p = LocationFromBindings (Containers::Sequence<SocketAddress>{SocketAddress{V4::kAddrAny, 8080}}, "/d.xml"sv);
            EXPECT_EQ (str (p (at (kLAN_))), "http://192.168.1.5:8080/d.xml"sv);
            EXPECT_EQ (str (p (at (kV6_))), nullopt); // the web server listens only on IPv4
        }
        {
            LocationProvider p = LocationFromBindings (Containers::Sequence<SocketAddress>{SocketAddress{V6::kAddrAny, 8080}}, "/d.xml"sv);
            EXPECT_EQ (str (p (at (kV6_))), "http://[2001:db8::5]:8080/d.xml"sv);
            EXPECT_EQ (str (p (at (kLAN_))), nullopt);
            EXPECT_EQ (str (p (at (kV6LinkLocal_))), nullopt); // its URL would need a zone naming OUR interface
        }
        {
            LocationProvider p =
                LocationFromBindings (Containers::Sequence<SocketAddress>{SocketAddress{V4::kAddrAny, 9090}, SocketAddress{kLAN_, 8080}});
            EXPECT_EQ (str (p (at (kLAN_))), "http://192.168.1.5:8080/"sv); // its own binding, not the wildcard
            EXPECT_EQ (str (p (at (kOtherLAN_))), "http://10.0.0.7:9090/"sv);
        }
        {
            LocationProvider p = LocationFromBindings (Containers::Sequence<SocketAddress>{SocketAddress{kLAN_, 8080}});
            EXPECT_EQ (str (p (at (kOtherLAN_))), nullopt); // listens on another network's address only
        }
        // FixedLocation: always that URL
        EXPECT_EQ (str (FixedLocation (URI{"https://device.example/d.xml"sv}) (at (kLAN_))), "https://device.example/d.xml"sv);
        // LocationFillingInHost: a host given is kept; a missing one is filled in - but never with a link-local IPv6 address
        EXPECT_EQ (str (LocationFillingInHost (URI{"http://device.example:8000/d.xml"sv}) (at (kLAN_))), "http://device.example:8000/d.xml"sv);
        const URI noHost{URI::SchemeType{"http"sv}, URI::Authority{nullopt, PortType{8080}}, "/d.xml"sv};
        EXPECT_EQ (str (LocationFillingInHost (noHost) (at (kLAN_))), "http://192.168.1.5:8080/d.xml"sv);
        EXPECT_EQ (str (LocationFillingInHost (noHost) (at (kV6_))), "http://[2001:db8::5]:8080/d.xml"sv);
        EXPECT_EQ (str (LocationFillingInHost (noHost) (at (kV6LinkLocal_))), nullopt);
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
        d.fServer   = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
        const URI location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, kPort_}, "/device.xml"sv}; // no host: each network's own
        DeviceDescription dd;
        dd.fDeviceType   = deviceType;
        dd.fFriendlyName = "Stroika regression test device"sv;
        dd.fUDN          = "uuid:" + deviceID;

        // Synchronized: the search's callback, and the server's location provider, run on their own threads, not this one
        Execution::Synchronized<optional<SSDP::Advertisement>> found;
        Execution::WaitableEvent                               foundEvent;
        // BasicServer's default provider, recording what each search response asks it
        Execution::Synchronized<Containers::Sequence<SSDP::Server::LocationContext>> askedFor;
        SSDP::Server::LocationProvider fillInHost          = SSDP::Server::LocationFillingInHost (location);
        SSDP::Server::LocationProvider recordingFillInHost = [&] (const SSDP::Server::LocationContext& c) {
            if (c.fAsker) {
                askedFor.rwget ()->Append (c);
            }
            return fillInHost (c);
        };
        try {
            using IO::Network::InternetProtocol::IP::IPVersionSupport;
            SSDP::Server::BasicServer server{d, dd, recordingFillInHost, SSDP::Server::BasicServer::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
            SSDP::Client::Search search{[&] (const SSDP::Advertisement& a) {
                                            if (a.fTarget == deviceType) {
                                                found.store (a);
                                                foundEvent.Set ();
                                            }
                                        },
                                        deviceType, nullopt, SSDP::Client::Search::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
            if (foundEvent.WaitQuietly (10s) == Execution::WaitableEvent::WaitStatus::eTriggered) {
                // it searched somewhere - and only where the default filter lets it
                EXPECT_FALSE (search.GetNetworkInterfaces ().empty ());
                EXPECT_TRUE (search.GetNetworkInterfaces ().All ([] (const Interface& i) { return SSDP::DefaultInterfaceFilter (i); }));
            }
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
        Containers::Collection<InternetAddress> ourAddresses;
        for (const Interface& i : SystemInterfacesMgr{}.GetAll ()) {
            ourAddresses.AddAll (i.fBindings.fAddresses);
        }
        auto isOurs = [&] (const InternetAddress& ia) { return ia.IsLocalhostAddress () or ourAddresses.Contains (ia); };
        // the host filled in must be one of this machine's own addresses
        optional<InternetAddress> host = authority and authority->GetHost () ? authority->GetHost ()->AsInternetAddress () : nullopt;
        EXPECT_TRUE (host.has_value ()) << Characters::ToString (a->fLocation).AsNarrowSDKString ();
        if (host) {
            EXPECT_TRUE (isOurs (*host)) << Characters::ToString (*host).AsNarrowSDKString () << " is not one of this machine's addresses";
        }
        // and it is this machine's address as the asker reaches it - which, for a search from this machine (ours: others on
        // the network search too), is the very address the search came from
        Containers::Sequence<SSDP::Server::LocationContext> fromUs{
            askedFor.load ().Where ([&] (const SSDP::Server::LocationContext& c) { return isOurs (c.fAsker->GetInternetAddress ()); })};
        EXPECT_FALSE (fromUs.empty ());
        for (const SSDP::Server::LocationContext& c : fromUs) {
            EXPECT_EQ (c.fLocalAddress, c.fAsker->GetInternetAddress ()) << Characters::ToString (c).AsNarrowSDKString ();
        }
        EXPECT_TRUE (fromUs.Any ([&] (const SSDP::Server::LocationContext& c) { return c.fLocalAddress == host; }))
            << Characters::ToString (a->fLocation).AsNarrowSDKString ();
    }

    /*
     *  Our own Listener hearing our own BasicServer's NOTIFY, both in this process: the server sends one out of each network
     *  interface its Options::fInterfaces accepts, with that network's own address as its LOCATION, and the Listener listens on
     *  each. As in SSDP_Loopback_Search_, anything that keeps the exchange from happening is a test issue; a wrong NOTIFY is a
     *  failure.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Loopback_Notify_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_Loopback_Notify_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by BasicServer
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        constexpr uint16_t kPort_ = 49152; // only advertised - nothing listens on it
        const URI location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, kPort_}, "/device.xml"sv}; // no host: each network's own

        // our Listener (on every interface) hearing our server, which advertises on the interfaces serverInterfaces accepts: the
        // NOTIFYs heard - or nullopt if the exchange could not happen here
        auto exchange = [&] (const SSDP::InterfaceFilter& serverInterfaces) -> optional<Containers::Sequence<SSDP::Advertisement>> {
            const String deviceID   = Common::GUID::GenerateNew ().As<String> ();
            const String deviceType = "urn:stroika-regression-test:device:SSDPLoopbackNotify-{}:1"_f(deviceID);
            Device       d;
            d.fDeviceID = deviceID;
            d.fServer   = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
            DeviceDescription dd;
            dd.fDeviceType   = deviceType;
            dd.fFriendlyName = "Stroika regression test device"sv;
            dd.fUDN          = "uuid:" + deviceID;
            Execution::Synchronized<Containers::Sequence<SSDP::Advertisement>> heard; // Synchronized: the listener calls back on its own thread
            Execution::WaitableEvent heardEvent;
            try {
                // the listener first, so it hears the server's very first NOTIFYs (sent as it starts)
                SSDP::Client::Listener    listener{[&] (const SSDP::Advertisement& a) {
                                                    if (a.fTarget == deviceType) {
                                                        heard.rwget ()->Append (a);
                                                        heardEvent.Set ();
                                                    }
                                                   },
                                                   SSDP::Client::Listener::Options{.fIPVersion = IPVersionSupport::eIPV4Only},
                                                   SSDP::Client::Listener::eAutoStart};
                SSDP::Server::BasicServer server{
                    d, dd, SSDP::Server::LocationFillingInHost (location),
                    SSDP::Server::BasicServer::Options{.fIPVersion = IPVersionSupport::eIPV4Only, .fInterfaces = serverInterfaces}};
                if (heardEvent.WaitQuietly (10s) == Execution::WaitableEvent::WaitStatus::eTriggered) {
                    Execution::Sleep (1s); // and the ones out of the other interfaces
                    EXPECT_FALSE (listener.GetNetworkInterfaces ().empty ());
                    EXPECT_FALSE (server.GetNetworkInterfaces ().empty ());
                    EXPECT_TRUE (server.GetNetworkInterfaces ().All ([&] (const Interface& i) { return serverInterfaces (i); }));
                }
            }
            catch (...) {
                Stroika::Frameworks::Test::WarnTestIssue (
                    "SSDP_Loopback_Notify_ skipped - could not run an SSDP server and listener here: {}"_f(current_exception ())
                        .AsNarrowSDKString ()
                        .c_str ());
                return nullopt;
            }
            Containers::Sequence<SSDP::Advertisement> notifies = heard.load ();
            if (notifies.empty ()) {
                Stroika::Frameworks::Test::WarnTestIssue (
                    "SSDP_Loopback_Notify_ skipped - our own NOTIFY was not heard within 10 seconds (this environment probably blocks "
                    "multicast, or UDP 1900)");
                return nullopt;
            }
            for (const SSDP::Advertisement& a : notifies) {
                DbgTrace ("heard: {}"_f, a);
                EXPECT_EQ (a.fAlive, true);
                EXPECT_EQ (a.fUSN, "uuid:{}::{}"_f(deviceID, deviceType));
                EXPECT_EQ (a.fLocation.GetPath (), "/device.xml"sv);
                optional<URI::Authority> authority = a.fLocation.GetAuthority ();
                EXPECT_TRUE (authority and authority->GetPort () == kPort_);
            }
            return notifies;
        };
        auto hostOf = [] (const SSDP::Advertisement& a) -> optional<InternetAddress> {
            optional<URI::Authority> authority = a.fLocation.GetAuthority ();
            return authority and authority->GetHost () ? authority->GetHost ()->AsInternetAddress () : nullopt;
        };
        const Containers::Sequence<Interface> interfaces{SystemInterfacesMgr{}.GetAll ()};
        auto                                  interfaceWith = [&] (const InternetAddress& a) {
            return interfaces.First ([&] (const Interface& i) { return i.fBindings.fAddresses.Contains (a); });
        };

        // on every interface (the default): each NOTIFY carries one of this machine's own addresses
        optional<Containers::Sequence<SSDP::Advertisement>> everywhere = exchange (SSDP::DefaultInterfaceFilter);
        if (not everywhere) {
            return;
        }
        Containers::Set<InternetAddress> hosts;
        for (const SSDP::Advertisement& a : *everywhere) {
            optional<InternetAddress> host = hostOf (a);
            EXPECT_TRUE (host and interfaceWith (*host)) << Characters::ToString (a.fLocation).AsNarrowSDKString () << " - not one of ours";
            if (host) {
                hosts += *host;
            }
        }
        DbgTrace ("on every interface: heard {} NOTIFYs, with LOCATION hosts {}"_f, everywhere->size (), hosts);

        // on one interface only - one we just heard from: every NOTIFY then carries that interface's address
        optional<Interface> only = hosts.empty () ? nullopt : interfaceWith (*hosts.First ());
        if (not only) {
            return; // (already a failure, above)
        }
        const Characters::String onlyID = only->fInterfaceID;
        if (optional<Containers::Sequence<SSDP::Advertisement>> there =
                exchange ([onlyID] (const Interface& i) { return SSDP::DefaultInterfaceFilter (i) and i.fInterfaceID == onlyID; })) {
            for (const SSDP::Advertisement& a : *there) {
                optional<InternetAddress> host = hostOf (a);
                EXPECT_TRUE (host and only->fBindings.fAddresses.Contains (*host))
                    << Characters::ToString (a.fLocation).AsNarrowSDKString () << " - not on " << onlyID.AsNarrowSDKString ();
            }
        }
    }

    /*
     *  A Listener that can listen on no interface (none its Options::fInterfaces accepts, or no network yet) is still made: it
     *  just hears nothing until there is one.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Listener_NoInterface_)
    {
        Debug::TraceContextBumper ctx{"SSDP_Listener_NoInterface_"};
        SSDP::Client::Listener    listener{
            SSDP::Client::Listener::Options{.fInterfaces = [] ([[maybe_unused]] const Interface& i) { return false; }}};
        EXPECT_TRUE (listener.GetNetworkInterfaces ().empty ());
    }

    /*
     *  Following network changes costs no thread per SSDP object while the network stays as it is: they share LinkMonitor's
     *  (on POSIX - on Windows an OS registration, so not even that), and each has a thread of its own only while a burst of
     *  changes settles.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_FollowNetworkChanges_Threads_)
    {
        Debug::TraceContextBumper ctx{"SSDP_FollowNetworkChanges_Threads_"};
#if qStroika_Foundation_Execution_Thread_SupportThreadStatistics
        namespace Thread = Execution::Thread;
        const Containers::Set<Thread::IDType> before{Thread::GetStatistics ().fRunningThreads};
        auto                                  newThreads = [&] () {
            return Thread::GetStatistics ().fRunningThreads.Where ([&] (Thread::IDType id) { return not before.Contains (id); }).size ();
        };
        {
            // a Search and a Listener, as a control point has - not started, so with no threads of their own to search or listen
            SSDP::Client::Search   search{SSDP::Client::Search::Options{.fFollowNetworkChanges = true}};
            SSDP::Client::Listener listener{SSDP::Client::Listener::Options{.fFollowNetworkChanges = true}};
            EXPECT_LE (newThreads (), 1u) << "they share LinkMonitor's thread";
        }
        for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 5s; newThreads () != 0 and Time::GetTickCount () < giveUpAt;) {
            Execution::Sleep (10ms);
        }
        EXPECT_EQ (newThreads (), 0u) << "and leave none behind";
#endif
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
