/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Frameworks::UPnP
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>
#include <map>
#include <set>

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
#include "Stroika/Foundation/Execution/WaitForIOReady.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/ConnectionlessSocket.h"
#include "Stroika/Foundation/IO/Network/Interface.h"
#include "Stroika/Foundation/Time/DateTime.h"

#include "Stroika/Frameworks/Test/TestHarness.h"
#include "Stroika/Frameworks/UPnP/Device.h"
#include "Stroika/Frameworks/UPnP/DeviceDescription.h"
#include "Stroika/Frameworks/UPnP/SSDP/Advertisement.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/CachingListener.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/Listener.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/Search.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/BasicServer.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/LocationProvider.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/PeriodicNotifier.h"

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
            EXPECT_EQ (b.fLocation, alive ? a.fLocation : URI{}); // an ssdp:byebye says neither where the device was, nor what it ran
            EXPECT_EQ (b.fServer, alive ? a.fServer : String{});
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

    /*
     *  Each SSDP packet has the headers the UPnP Device Architecture gives it: a NOTIFY's HOST is the multicast group it goes
     *  to; an ssdp:byebye has no CACHE-CONTROL, LOCATION or SERVER; a search answer has EXT and DATE, and no HOST or NTS (it goes
     *  to the asker, not to a group).
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Serialize_Headers_)
    {
        Debug::TraceContextBumper ctx{"SSDP_Serialize_Headers_"};
        SSDP::Advertisement       a;
        a.fAlive    = true;
        a.fUSN      = "uuid:315caae0-1335-57bf-a178-24c9ee756627::upnp:rootdevice"sv;
        a.fLocation = URI{"http://192.168.1.2:8080/device.xml"sv};
        a.fServer   = "Linux/6.8 UPnP/1.0 StroikaTest/1.0"sv;
        a.fTarget   = SSDP::kTarget_UPNPRootDevice;
        // a header of the packet - SSDP's, like HTTP's, are named in any case
        auto header = [] (const Memory::BLOB& packet, const String& name) -> optional<String> {
            String              headLine;
            SSDP::Advertisement d;
            SSDP::DeSerialize (packet, &headLine, &d);
            for (const auto& kv : d.fRawHeaders) {
                if (String::ThreeWayComparer{Characters::eCaseInsensitive}(kv.fKey, name) == 0) {
                    return kv.fValue;
                }
            }
            return nullopt;
        };

        Memory::BLOB alive = SSDP::Serialize ("NOTIFY * HTTP/1.1"sv, SSDP::SearchOrNotify::Notify, a, SSDP::V4::kSocketAddress);
        for (string_view h : {"Host"sv, "Cache-Control"sv, "Location"sv, "NT"sv, "NTS"sv, "Server"sv, "USN"sv}) {
            EXPECT_TRUE (header (alive, h)) << "ssdp:alive has " << h;
        }
        EXPECT_EQ (header (alive, "Host"sv), String{"239.255.255.250:1900"sv});
        // and IPv6's group, for IPv6
        optional<String> host6 = header (SSDP::Serialize ("NOTIFY * HTTP/1.1"sv, SSDP::SearchOrNotify::Notify, a, SSDP::V6::kSocketAddress), "Host"sv);
        EXPECT_EQ (host6 ? host6->ToLowerCase () : String{}, String{"[ff02::c]:1900"sv});

        a.fAlive            = false;
        Memory::BLOB byebye = SSDP::Serialize ("NOTIFY * HTTP/1.1"sv, SSDP::SearchOrNotify::Notify, a);
        for (string_view h : {"Host"sv, "NT"sv, "NTS"sv, "USN"sv}) {
            EXPECT_TRUE (header (byebye, h)) << "ssdp:byebye has " << h;
        }
        for (string_view h : {"Cache-Control"sv, "Location"sv, "Server"sv}) {
            EXPECT_FALSE (header (byebye, h)) << "ssdp:byebye has no " << h;
        }

        a.fAlive            = true; // (and ignored there)
        Memory::BLOB answer = SSDP::Serialize ("HTTP/1.1 200 OK"sv, SSDP::SearchOrNotify::SearchResponse, a);
        EXPECT_EQ (header (answer, "EXT"sv), String{}) << "a search answer has EXT, with no value";
        EXPECT_FALSE (header (answer, "Host"sv)) << "a search answer has no HOST";
        EXPECT_FALSE (header (answer, "NTS"sv)) << "a search answer has no NTS";
        for (string_view h : {"Cache-Control"sv, "Location"sv, "Server"sv, "ST"sv, "USN"sv}) {
            EXPECT_TRUE (header (answer, h)) << "a search answer has " << h;
        }
        // and DATE: when it was sent, as an HTTP date
        optional<String> date = header (answer, "Date"sv);
        ASSERT_TRUE (date) << "a search answer has DATE";
        optional<Time::DateTime> sent = Time::DateTime::ParseQuietly (*date, Time::DateTime::kHTTPDateFormat);
        ASSERT_TRUE (sent) << "a search answer's DATE is an HTTP date";
        EXPECT_EQ (sent->Format (Time::DateTime::kHTTPDateFormat), *date); // in the one form an HTTP date is written in
        EXPECT_LT (std::abs ((Time::DateTime::Now () - *sent).count ()), 60) << "a search answer's DATE is when it was sent";
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
        dd.fServices =
            Containers::Collection<DeviceDescription::Service>{DeviceDescription::Service{.fServiceType = "urn:schemas-upnp-org:service:SwitchPower:1"sv,
                                                                                          .fServiceID = "urn:upnp-org:serviceId:SwitchPower"sv,
                                                                                          .fSCPDURL = URI{"/SwitchPower/description.xml"sv},
                                                                                          .fControlURL  = URI{"/SwitchPower/control"sv},
                                                                                          .fEventSubURL = URI{"/SwitchPower/events"sv}}};

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
        // and its service, with each of its URLs
        optional<DeviceDescription::Service> service = back.fServices ? back.fServices->First () : nullopt;
        EXPECT_TRUE (service.has_value () and back.fServices->size () == 1);
        if (service) {
            const DeviceDescription::Service was = *dd.fServices->First ();
            EXPECT_EQ (service->fServiceType, was.fServiceType);
            EXPECT_EQ (service->fServiceID, was.fServiceID);
            EXPECT_EQ (service->fSCPDURL, was.fSCPDURL);
            EXPECT_EQ (service->fControlURL, was.fControlURL);
            EXPECT_EQ (service->fEventSubURL, was.fEventSubURL);
        }
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
     *  turns multicast loopback on) - found by its device type, and by its service type: a device is advertised by each
     *  service type it has, too (UPnP Device Architecture 1.1, section 1.1.2).
     *
     *  Whether that can happen at all depends on the environment, not on Stroika: binding UDP 1900 may be refused, or the
     *  port shared with another SSDP service; multicast may be filtered (a firewall, a container network, macOS's Local
     *  Network privacy). So anything that keeps the exchange from happening is reported as a test issue, never a failure.
     *  What IS a failure: an answer that comes back wrong.
     *
     *  Its device and service types are unique to this run, so no other device on the network answers - and only our device's
     *  announcements (a few seconds of them, multicast with SSDP::kDefaultMulticastTTL) reach the network.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Loopback_Search_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_Loopback_Search_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by BasicServer
        const String                                 deviceID    = Common::GUID::GenerateNew ().As<String> ();
        const String                                 deviceType  = "urn:stroika-regression-test:device:SSDPLoopback-{}:1"_f(deviceID);
        const String                                 serviceType = "urn:stroika-regression-test:service:SSDPLoopback-{}:1"_f(deviceID);
        constexpr uint16_t                           kPort_      = 49152; // only advertised - nothing listens on it

        Device d;
        d.fDeviceID = deviceID;
        d.fServer   = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
        const URI location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, kPort_}, "/device.xml"sv}; // no host: each network's own
        DeviceDescription dd;
        dd.fDeviceType   = deviceType;
        dd.fFriendlyName = "Stroika regression test device"sv;
        dd.fUDN          = "uuid:" + deviceID;
        dd.fServices     = Containers::Collection<DeviceDescription::Service>{
            DeviceDescription::Service{.fServiceType = serviceType, .fServiceID = "urn:stroika-regression-test:serviceId:SSDPLoopback"sv}};

        // Synchronized: the search's callback, and the server's location provider, run on their own threads, not this one
        Execution::Synchronized<optional<SSDP::Advertisement>> found;
        Execution::WaitableEvent                               foundEvent;
        Execution::Synchronized<optional<SSDP::Advertisement>> foundByService; // by a search for its service type
        Execution::WaitableEvent                               foundByServiceEvent;
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
            SSDP::Client::Search serviceSearch{[&] (const SSDP::Advertisement& a) {
                                                   if (a.fTarget == serviceType) {
                                                       foundByService.store (a);
                                                       foundByServiceEvent.Set ();
                                                   }
                                               },
                                               serviceType, nullopt, SSDP::Client::Search::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
            if (foundEvent.WaitQuietly (10s) == Execution::WaitableEvent::WaitStatus::eTriggered) {
                // it searched somewhere - and only where the default filter lets it
                EXPECT_FALSE (search.GetNetworkInterfaces ().empty ());
                EXPECT_TRUE (search.GetNetworkInterfaces ().All ([] (const Interface& i) { return SSDP::DefaultInterfaceFilter (i); }));
                (void)foundByServiceEvent.WaitQuietly (10s); // the exchange works here, so its service type must find it too
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
        optional<SSDP::Advertisement> byService = foundByService.load ();
        EXPECT_TRUE (byService.has_value ()) << "found by its device type, but not by its service type";
        if (byService) {
            EXPECT_EQ (byService->fUSN, "uuid:{}::{}"_f(deviceID, serviceType));
            EXPECT_EQ (byService->fLocation.GetPath (), "/device.xml"sv);
        }
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
                                                    // its ssdp:alive NOTIFYs - not the byebye it says as it goes (SSDP_Loopback_Byebye_'s)
                                                    if (a.fTarget == deviceType and a.fAlive == true) {
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
     *  A BasicServer that stops says so: an ssdp:byebye for each of its advertisements, out of each interface it announced on -
     *  so control points drop the device at once, rather than when its max-age (30 minutes) runs out - and no ssdp:alive after
     *  it. Its advertisements include one for each of its service types: once, however many of its services are of it (UPnP
     *  Device Architecture 1.1, section 1.1.2). As in SSDP_Loopback_Notify_, anything that keeps the exchange from happening
     *  is a test issue.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Loopback_Byebye_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_Loopback_Byebye_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by BasicServer
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        const URI    location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, uint16_t{49152}}, "/device.xml"sv}; // only advertised
        const String deviceID     = Common::GUID::GenerateNew ().As<String> ();
        const String deviceType   = "urn:stroika-regression-test:device:SSDPLoopbackByebye-{}:1"_f(deviceID);
        const String serviceType1 = "urn:stroika-regression-test:service:SSDPLoopbackByebye1-{}:1"_f(deviceID);
        const String serviceType2 = "urn:stroika-regression-test:service:SSDPLoopbackByebye2-{}:1"_f(deviceID);
        Device       d;
        d.fDeviceID = deviceID;
        d.fServer   = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
        DeviceDescription dd;
        dd.fDeviceType   = deviceType;
        dd.fFriendlyName = "Stroika regression test device"sv;
        dd.fUDN          = "uuid:" + deviceID;
        dd.fServices     = Containers::Collection<DeviceDescription::Service>{
            // two services of one type, one of another
            DeviceDescription::Service{.fServiceType = serviceType1, .fServiceID = "urn:stroika-regression-test:serviceId:A"sv},
            DeviceDescription::Service{.fServiceType = serviceType1, .fServiceID = "urn:stroika-regression-test:serviceId:B"sv},
            DeviceDescription::Service{.fServiceType = serviceType2, .fServiceID = "urn:stroika-regression-test:serviceId:C"sv}};
        Execution::Synchronized<Containers::Sequence<SSDP::Advertisement>> heard; // Synchronized: the listener calls back on its own thread
        Execution::WaitableEvent                                           aliveHeard;
        Execution::WaitableEvent                                           byebyeHeard;
        try {
            SSDP::Client::Listener listener{[&] (const SSDP::Advertisement& a) {
                                                if (a.fUSN.Contains (deviceID)) { // any of its advertisements
                                                    heard.rwget ()->Append (a);
                                                    (a.fAlive == false ? byebyeHeard : aliveHeard).Set ();
                                                }
                                            },
                                            SSDP::Client::Listener::Options{.fIPVersion = IPVersionSupport::eIPV4Only},
                                            SSDP::Client::Listener::eAutoStart};
            {
                SSDP::Server::BasicServer server{d, dd, SSDP::Server::LocationFillingInHost (location),
                                                 SSDP::Server::BasicServer::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
                if (aliveHeard.WaitQuietly (10s) != Execution::WaitableEvent::WaitStatus::eTriggered) {
                    Stroika::Frameworks::Test::WarnTestIssue ("SSDP_Loopback_Byebye_ skipped - our own NOTIFY was not heard within 10 "
                                                              "seconds (this environment probably blocks multicast, or UDP 1900)");
                    return;
                }
                Execution::Sleep (1s); // and the ones out of the other interfaces: each its own socket, read in no set order
            } // the server stops
            EXPECT_TRUE (byebyeHeard.WaitQuietly (10s) == Execution::WaitableEvent::WaitStatus::eTriggered)
                << "no ssdp:byebye heard after the server stopped";
            Execution::Sleep (1s); // for anything after it
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_Byebye_ skipped - could not run an SSDP server and listener here: {}"_f(current_exception ())
                    .AsNarrowSDKString ()
                    .c_str ());
            return;
        }
        bool                      byebyeYet = false;
        Containers::Set<String>   aliveUSNs;
        map<String, unsigned int> byebyes; // by USN
        for (const SSDP::Advertisement& a : heard.load ()) {
            DbgTrace ("heard: {}"_f, a);
            if (a.fAlive == false) {
                byebyeYet = true;
                ++byebyes[a.fUSN];
                EXPECT_EQ (a.fLocation, URI{}); // a byebye says only what is going away
            }
            else {
                aliveUSNs += a.fUSN;
                EXPECT_FALSE (byebyeYet) << "an ssdp:alive after the ssdp:byebye: " << a.fUSN.AsNarrowSDKString ();
            }
        }
        // one for each of its advertisements: the root device, the device itself, its device type, and each of its service types -
        // each in as many ssdp:byebye as the root device, so a service type once however many of its services are of it
        const String rootDevice = "uuid:{}::upnp:rootdevice"_f(deviceID);
        for (const String& usn : {rootDevice, "uuid:{}"_f(deviceID), "uuid:{}::{}"_f(deviceID, deviceType),
                                  "uuid:{}::{}"_f(deviceID, serviceType1), "uuid:{}::{}"_f(deviceID, serviceType2)}) {
            EXPECT_TRUE (aliveUSNs.Contains (usn)) << "no ssdp:alive for " << usn.AsNarrowSDKString ();
            EXPECT_TRUE (byebyes.contains (usn)) << "no ssdp:byebye for " << usn.AsNarrowSDKString ();
            EXPECT_EQ (byebyes[usn], byebyes[rootDevice]) << "not as many ssdp:byebye for " << usn.AsNarrowSDKString () << " as for the root device";
        }
    }

    /*
     *  Each set of NOTIFYs goes out more than once, a short pause apart, as UDP loses packets (UPnP Device Architecture 1.1
     *  section 1.2.2) - the ssdp:alive set as the server starts, and the ssdp:byebye set as it stops: one
     *  byebye for each alive (section 1.2.3). Each interface's alive carries that interface's own LOCATION, so each
     *  advertisement's is heard more than once from each; its byebyes, with no LOCATION, more than once per interface. As in
     *  SSDP_Loopback_Notify_, anything that keeps the exchange from happening is a test issue.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Loopback_RepeatedSets_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_Loopback_RepeatedSets_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by BasicServer
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        const URI    location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, uint16_t{49152}}, "/device.xml"sv}; // only advertised
        const String deviceID   = Common::GUID::GenerateNew ().As<String> ();
        const String deviceType = "urn:stroika-regression-test:device:SSDPLoopbackRepeatedSets-{}:1"_f(deviceID);
        Device       d;
        d.fDeviceID = deviceID;
        d.fServer   = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
        DeviceDescription dd;
        dd.fDeviceType   = deviceType;
        dd.fFriendlyName = "Stroika regression test device"sv;
        dd.fUDN          = "uuid:" + deviceID;
        Execution::Synchronized<Containers::Sequence<SSDP::Advertisement>> heard; // Synchronized: the listener calls back on its own thread
        Execution::WaitableEvent                                           aliveHeard;
        try {
            SSDP::Client::Listener listener{[&] (const SSDP::Advertisement& a) {
                                                if (a.fUSN.Contains (deviceID)) { // any of its advertisements
                                                    heard.rwget ()->Append (a);
                                                    if (a.fAlive == true) {
                                                        aliveHeard.Set ();
                                                    }
                                                }
                                            },
                                            SSDP::Client::Listener::Options{.fIPVersion = IPVersionSupport::eIPV4Only},
                                            SSDP::Client::Listener::eAutoStart};
            {
                SSDP::Server::BasicServer server{d, dd, SSDP::Server::LocationFillingInHost (location),
                                                 SSDP::Server::BasicServer::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
                if (aliveHeard.WaitQuietly (10s) != Execution::WaitableEvent::WaitStatus::eTriggered) {
                    Stroika::Frameworks::Test::WarnTestIssue ("SSDP_Loopback_RepeatedSets_ skipped - our own NOTIFY was not heard within "
                                                              "10 seconds (this environment probably blocks multicast, or UDP 1900)");
                    return;
                }
                Execution::Sleep (1s); // the rest of its alive sets
            } // the server stops
            Execution::Sleep (1s); // its byebye sets
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_RepeatedSets_ skipped - could not run an SSDP server and listener here: {}"_f(current_exception ())
                    .AsNarrowSDKString ()
                    .c_str ());
            return;
        }
        map<String, unsigned int> alives;     // by USN and LOCATION - one advertisement, from one interface
        map<String, set<String>>  interfaces; // by USN: the LOCATIONs it was heard with - an interface each
        map<String, unsigned int> byebyes;    // by USN
        for (const SSDP::Advertisement& a : heard.load ()) {
            if (a.fAlive == true) {
                String where = Characters::ToString (a.fLocation);
                ++alives[a.fUSN + " " + where];
                interfaces[a.fUSN].insert (where);
            }
            else {
                ++byebyes[a.fUSN];
            }
        }
        for (const auto& [usnWhere, n] : alives) {
            EXPECT_GE (n, 2u) << "ssdp:alive heard once only: " << usnWhere.AsNarrowSDKString ();
        }
        for (const String& usn : {"uuid:{}::upnp:rootdevice"_f(deviceID), "uuid:{}"_f(deviceID), "uuid:{}::{}"_f(deviceID, deviceType)}) {
            EXPECT_FALSE (interfaces[usn].empty ()) << "no ssdp:alive for " << usn.AsNarrowSDKString ();
            EXPECT_GE (byebyes[usn], 2 * interfaces[usn].size ()) << "ssdp:byebye heard once only, per interface: " << usn.AsNarrowSDKString ();
        }
    }

    /*
     *  Options::fMulticastTTL: our own search finds our own device, and our own listener hears it announce itself, with the
     *  M-SEARCH and the NOTIFYs multicast with a TTL other than the default. How far they would travel is not seen here (all in
     *  this process); that each goes out with its TTL was checked by capturing them. As in SSDP_Loopback_Search_, anything that
     *  keeps the exchange from happening is a test issue - seen as its failing with the default TTL too.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Loopback_MulticastTTL_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_Loopback_MulticastTTL_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by BasicServer
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        const URI location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, uint16_t{49152}}, "/device.xml"sv}; // only advertised

        // with this TTL on the M-SEARCH and the NOTIFYs: was our device found by our search, and heard by our listener
        auto foundAndHeard = [&] (uint8_t ttl) -> pair<bool, bool> {
            const String deviceID   = Common::GUID::GenerateNew ().As<String> ();
            const String deviceType = "urn:stroika-regression-test:device:SSDPLoopbackMulticastTTL-{}:1"_f(deviceID);
            Device       d;
            d.fDeviceID = deviceID;
            d.fServer   = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
            DeviceDescription dd;
            dd.fDeviceType   = deviceType;
            dd.fFriendlyName = "Stroika regression test device"sv;
            dd.fUDN          = "uuid:" + deviceID;
            Execution::WaitableEvent found;
            Execution::WaitableEvent heard;
            // the listener first, so it hears the server's very first NOTIFYs (sent as it starts)
            SSDP::Client::Listener listener{[&] (const SSDP::Advertisement& a) {
                                                if (a.fTarget == deviceType and a.fAlive == true) {
                                                    heard.Set ();
                                                }
                                            },
                                            SSDP::Client::Listener::Options{.fIPVersion = IPVersionSupport::eIPV4Only},
                                            SSDP::Client::Listener::eAutoStart};
            SSDP::Server::BasicServer server{d, dd, SSDP::Server::LocationFillingInHost (location),
                                             SSDP::Server::BasicServer::Options{.fIPVersion = IPVersionSupport::eIPV4Only, .fMulticastTTL = ttl}};
            SSDP::Client::Search search{[&] (const SSDP::Advertisement& a) {
                                            if (a.fTarget == deviceType) {
                                                found.Set ();
                                            }
                                        },
                                        deviceType, nullopt,
                                        SSDP::Client::Search::Options{.fIPVersion = IPVersionSupport::eIPV4Only, .fMulticastTTL = ttl}};

            const Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s;
            bool                         wasFound = found.WaitUntilQuietly (giveUpAt) == Execution::WaitableEvent::WaitStatus::eTriggered;
            bool                         wasHeard = heard.WaitUntilQuietly (giveUpAt) == Execution::WaitableEvent::WaitStatus::eTriggered;
            return {wasFound, wasHeard};
        };

        try {
            if (foundAndHeard (SSDP::kDefaultMulticastTTL) != pair{true, true}) {
                Stroika::Frameworks::Test::WarnTestIssue (
                    "SSDP_Loopback_MulticastTTL_ skipped - even with the default TTL, our own device was not found and heard within "
                    "10 seconds (this environment probably blocks multicast, or UDP 1900)");
                return;
            }
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_MulticastTTL_ skipped - could not run an SSDP server, listener and search here: {}"_f(current_exception ())
                    .AsNarrowSDKString ()
                    .c_str ());
            return;
        }
        pair<bool, bool> withTTL3{false, false};
        EXPECT_NO_THROW (withTTL3 = foundAndHeard (3));
        EXPECT_TRUE (withTTL3.first) << "not found by our own search, its M-SEARCH sent with TTL 3";
        EXPECT_TRUE (withTTL3.second) << "not heard by our own listener, its NOTIFYs sent with TTL 3";
    }

    /*
     *  One callOnFinds throwing does not keep the advertisement from the others, of a Listener or of a Search: the exception
     *  ended the loop calling them, so the callbacks after a thrower missed every advertisement it threw on. As in
     *  SSDP_Loopback_Notify_, anything that keeps the exchange from happening - the callback BEFORE the thrower getting
     *  nothing either - is a test issue.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Loopback_CallbackThrows_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_Loopback_CallbackThrows_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by BasicServer
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        const URI    location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, uint16_t{49152}}, "/device.xml"sv}; // only advertised
        const String deviceID   = Common::GUID::GenerateNew ().As<String> ();
        const String deviceType = "urn:stroika-regression-test:device:SSDPLoopbackCallbackThrows-{}:1"_f(deviceID);
        Device       d;
        d.fDeviceID = deviceID;
        d.fServer   = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
        DeviceDescription dd;
        dd.fDeviceType   = deviceType;
        dd.fFriendlyName = "Stroika regression test device"sv;
        dd.fUDN          = "uuid:" + deviceID;
        // an ssdp:alive (the Listener's) or a search answer (the Search's) for our device
        auto ours = [&] (const SSDP::Advertisement& a) { return a.fTarget == deviceType and a.fAlive != false; };

        // a callback before a throwing one, and one after it: each says when it gets one of ours
        struct BeforeAndAfter {
            Execution::WaitableEvent fBefore;
            Execution::WaitableEvent fAfter;
        };
        auto addCallbacks = [&] (auto& listenerOrSearch, BeforeAndAfter& got) {
            listenerOrSearch.AddOnFoundCallback ([&] (const SSDP::Advertisement& a) {
                if (ours (a)) {
                    got.fBefore.Set ();
                }
            });
            listenerOrSearch.AddOnFoundCallback ([&] (const SSDP::Advertisement& a) {
                if (ours (a)) {
                    Execution::Throw (Execution::Exception<>{"a regression test's callOnFinds, throwing"sv});
                }
            });
            listenerOrSearch.AddOnFoundCallback ([&] (const SSDP::Advertisement& a) {
                if (ours (a)) {
                    got.fAfter.Set ();
                }
            });
        };
        BeforeAndAfter byListener;
        BeforeAndAfter bySearch;
        try {
            SSDP::Client::Listener listener{SSDP::Client::Listener::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
            addCallbacks (listener, byListener);
            listener.Start (); // before the server, so it hears its very first NOTIFYs
            SSDP::Server::BasicServer server{d, dd, SSDP::Server::LocationFillingInHost (location),
                                             SSDP::Server::BasicServer::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
            SSDP::Client::Search      search{SSDP::Client::Search::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
            addCallbacks (search, bySearch);
            search.Start (deviceType);
            const Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s;
            if (byListener.fBefore.WaitUntilQuietly (giveUpAt) != Execution::WaitableEvent::WaitStatus::eTriggered or
                bySearch.fBefore.WaitUntilQuietly (giveUpAt) != Execution::WaitableEvent::WaitStatus::eTriggered) {
                Stroika::Frameworks::Test::WarnTestIssue ("SSDP_Loopback_CallbackThrows_ skipped - our own device was not heard and found "
                                                          "within 10 seconds (this environment probably blocks multicast, or UDP 1900)");
                return;
            }
            // the callback after the thrower gets the very advertisement the one before it did (a second allows for a loaded machine)
            EXPECT_TRUE (byListener.fAfter.WaitQuietly (1s) == Execution::WaitableEvent::WaitStatus::eTriggered)
                << "the Listener's callback after a throwing one got nothing";
            EXPECT_TRUE (bySearch.fAfter.WaitQuietly (1s) == Execution::WaitableEvent::WaitStatus::eTriggered)
                << "the Search's callback after a throwing one got nothing";
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_CallbackThrows_ skipped - could not run an SSDP server, listener and search here: {}"_f(current_exception ())
                    .AsNarrowSDKString ()
                    .c_str ());
        }
    }

    /*
     *  Adding a callback does not wait for a callOnFinds running on the Listener's, or the Search's, own thread. It did: the
     *  callbacks ran holding the lock AddOnFoundCallback takes - so a callOnFinds waiting for the thread adding one waited
     *  forever. As in SSDP_Loopback_Notify_, anything that keeps the exchange from happening is a test issue.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Loopback_AddCallbackWhileOneRuns_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_Loopback_AddCallbackWhileOneRuns_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by BasicServer
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        const URI    location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, uint16_t{49152}}, "/device.xml"sv}; // only advertised
        const String deviceID   = Common::GUID::GenerateNew ().As<String> ();
        const String deviceType = "urn:stroika-regression-test:device:SSDPLoopbackAddCallbackWhileOneRuns-{}:1"_f(deviceID);
        Device       d;
        d.fDeviceID = deviceID;
        d.fServer   = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
        DeviceDescription dd;
        dd.fDeviceType   = deviceType;
        dd.fFriendlyName = "Stroika regression test device"sv;
        dd.fUDN          = "uuid:" + deviceID;
        // an ssdp:alive (the Listener's) or a search answer (the Search's) for our device
        auto ours = [&] (const SSDP::Advertisement& a) { return a.fTarget == deviceType and a.fAlive != false; };

        // a callback that, given its first advertisement of ours, says so - then waits, up to 5 seconds, to be let go
        struct Waiting {
            atomic<bool>             fStarted{false};
            Execution::WaitableEvent fRunning;
            Execution::WaitableEvent fLetGo;
        };
        auto addWaitingCallback = [&] (auto& listenerOrSearch, Waiting& w) {
            listenerOrSearch.AddOnFoundCallback ([&] (const SSDP::Advertisement& a) {
                if (ours (a) and not w.fStarted.exchange (true)) {
                    w.fRunning.Set ();
                    w.fLetGo.WaitQuietly (5s);
                }
            });
        };
        // how long adding a callback takes, with w's running - then lets w's go
        auto secondsToAdd = [] (auto& listenerOrSearch, Waiting& w) {
            Time::TimePointSeconds start = Time::GetTickCount ();
            listenerOrSearch.AddOnFoundCallback ([] ([[maybe_unused]] const SSDP::Advertisement& a) {});
            Time::DurationSeconds took = Time::GetTickCount () - start;
            w.fLetGo.Set ();
            return took.count ();
        };
        auto notRunning = [] () {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_AddCallbackWhileOneRuns_ skipped - our own device was not heard "
                "or found within 10 seconds (this environment probably blocks multicast, or UDP 1900)");
        };
        // one at a time - each callback's 5 seconds run from its own start
        Waiting inListener;
        Waiting inSearch;
        try {
            SSDP::Client::Listener listener{SSDP::Client::Listener::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
            addWaitingCallback (listener, inListener);
            listener.Start (); // before the server, so it hears its very first NOTIFYs
            SSDP::Server::BasicServer server{d, dd, SSDP::Server::LocationFillingInHost (location),
                                             SSDP::Server::BasicServer::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
            if (inListener.fRunning.WaitQuietly (10s) != Execution::WaitableEvent::WaitStatus::eTriggered) {
                notRunning ();
                return;
            }
            EXPECT_LT (secondsToAdd (listener, inListener), 1.0) << "adding a callback to a Listener waited for a running one";

            SSDP::Client::Search search{SSDP::Client::Search::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
            addWaitingCallback (search, inSearch);
            search.Start (deviceType);
            if (inSearch.fRunning.WaitQuietly (10s) != Execution::WaitableEvent::WaitStatus::eTriggered) {
                notRunning ();
                return;
            }
            EXPECT_LT (secondsToAdd (search, inSearch), 1.0) << "adding a callback to a Search waited for a running one";
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_AddCallbackWhileOneRuns_ skipped - could not run an SSDP server, listener and search here: {}"_f(current_exception ())
                    .AsNarrowSDKString ()
                    .c_str ());
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
     *  Joining the SSDP group again, on interfaces a socket has joined it on already, counts as joined - so when a network
     *  appears, the sockets already listening join it there too, keeping all else (before Stroika v3.0d25 a re-join failed - "already
     *  a member" - so new sockets were made instead, and what waited in the old ones was lost).
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_JoinAgain_)
    {
        Debug::TraceContextBumper                                              ctx{"SSDP_JoinAgain_"};
        Containers::Sequence<pair<ConnectionlessSocket::Ptr, InternetAddress>> toJoin; // a socket for each SSDP group this host can have
        for (const SocketAddress& group : {SSDP::V4::kSocketAddress, SSDP::V6::kSocketAddress}) {
            try {
                bool                      v4 = group.GetAddressFamily () == SocketAddress::INET;
                ConnectionlessSocket::Ptr s  = ConnectionlessSocket::New (group.GetAddressFamily (), Socket::DGRAM);
                s.Bind (SocketAddress{v4 ? V4::kAddrAny : V6::kAddrAny, group.GetPort ()}, Socket::BindFlags{.fSO_REUSEADDR = true});
                toJoin += make_pair (s, group.GetInternetAddress ());
            }
            catch (...) {
                DbgTrace ("SSDP_JoinAgain_: no socket for {} here: {}"_f, group, current_exception ());
            }
        }
        if (toJoin.empty ()) {
            Stroika::Frameworks::Test::WarnTestIssue ("SSDP_JoinAgain_ skipped - could not bind SSDP's sockets here");
            return;
        }
        auto ids = [] (const InterfacesByID& interfaces) {
            Containers::Set<String> result;
            for (const Interface& i : interfaces) {
                result += i.fInterfaceID;
            }
            return result;
        };
        Containers::Set<String> joined = ids (SSDP::Private_::JoinOnEveryInterface (toJoin, SSDP::DefaultInterfaceFilter));
        if (joined.empty ()) {
            Stroika::Frameworks::Test::WarnTestIssue ("SSDP_JoinAgain_ skipped - no network interface to join SSDP's group on");
            return;
        }
        EXPECT_EQ (ids (SSDP::Private_::JoinOnEveryInterface (toJoin, SSDP::DefaultInterfaceFilter)), joined)
            << "joined again, not counted as joined";
    }

    /*
     *  A Listener or Search moved from can still be destroyed: before Stroika v3.0d25 its destructor stopped the rep it had given
     *  up - a null one - and crashed.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Client_MovedFrom_)
    {
        Debug::TraceContextBumper ctx{"SSDP_Client_MovedFrom_"};
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        try {
            {
                SSDP::Client::Listener movedFrom{SSDP::Client::Listener::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
                SSDP::Client::Listener movedTo{move (movedFrom)};
            }
            {
                SSDP::Client::Search movedFrom{SSDP::Client::Search::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
                SSDP::Client::Search movedTo{move (movedFrom)};
            }
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Client_MovedFrom_ skipped - could not make an SSDP listener and search here: {}"_f(current_exception ())
                    .AsNarrowSDKString ()
                    .c_str ());
        }
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

    /*
     *  A CachingListener keeps each advertisement - a USN at a LOCATION - from its ssdp:alive until an ssdp:byebye withdraws it,
     *  or its max-age runs out; and tells its callbacks of each change, once. Told by NOTIFYs this test sends itself, out of
     *  each interface - so each arrives more than once, as a device's do. As in SSDP_Loopback_Notify_, anything that keeps the
     *  exchange from happening is a test issue.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_CachingListener_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_CachingListener_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by CachingListener
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        using SSDP::Client::CachingListener;
        const String deviceID  = Common::GUID::GenerateNew ().As<String> ();
        const String rootUSN   = "uuid:{}::upnp:rootdevice"_f(deviceID);
        const String deviceUSN = "uuid:{}"_f(deviceID);
        const URI    location1 = URI::Parse ("http://127.0.0.1:49152/a.xml"sv); // only advertised
        const URI    location2 = URI::Parse ("http://127.0.0.1:49153/b.xml"sv);
        auto         alive     = [] (const String& target, const String& usn, const URI& location, Time::Duration maxAge) {
            SSDP::Advertisement a;
            a.fAlive    = true;
            a.fUSN      = usn;
            a.fLocation = location;
            a.fServer   = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
            a.fTarget   = target;
            a.fMaxAge   = maxAge;
            return a;
        };
        auto byebye = [] (const String& target, const String& usn) {
            SSDP::Advertisement a;
            a.fAlive  = false;
            a.fUSN    = usn;
            a.fTarget = target;
            return a;
        };
        Execution::Synchronized<Containers::Sequence<SSDP::Advertisement>> told; // ours - Synchronized: the callback runs on its own thread
        // BWA: told read under its lock - cget () - not through a copy (load ()) while the callback may still append: ThreadSanitizer
        // reports a copy's reads as racing copy-on-write's later write in place (Memory::SharedByValue's use_count () check - TODO.md,
        // #1205). Restore load () - the reproducer - when that is fixed
        auto waitForChanges = [&] (size_t n) {
            for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; told.cget ()->size () < n and Time::GetTickCount () < giveUpAt;) {
                Execution::Sleep (50ms);
            }
            return told.cget ()->size () >= n;
        };
        try {
            CachingListener cache{[&] (const SSDP::Advertisement& a) {
                                      if (a.fUSN.Contains (deviceID)) {
                                          told.rwget ()->Append (a);
                                      }
                                  },
                                  CachingListener::Options{.fListener = {.fIPVersion = IPVersionSupport::eIPV4Only}, .fSearchFor = nullopt},
                                  CachingListener::eAutoStart};
            auto            cached = [&] () {
                return Containers::Sequence<SSDP::Advertisement>{
                    cache.GetAdvertisements ().Where ([&] (const SSDP::Advertisement& a) { return a.fUSN.Contains (deviceID); })};
            };
            // out of each interface it listens on, as a device's NOTIFYs come
            ConnectionlessSocket::Ptr sender = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            sender.SetMulticastLoopMode (true); // so this process hears it
            auto notify = [&] (const SSDP::Advertisement& a) {
                Memory::BLOB packet = SSDP::Serialize ("NOTIFY * HTTP/1.1"sv, SSDP::SearchOrNotify::Notify, a, SSDP::V4::kSocketAddress);
                for (const Interface& i : SystemInterfacesMgr{}.GetAll ()) {
                    if (SSDP::DefaultInterfaceFilter (i) and i.fBindings.fAddresses.Any ([] (const InternetAddress& ia) {
                            return ia.GetAddressFamily () == InternetAddress::AddressFamily::V4;
                        })) {
                        try {
                            sender.SetMulticastInterface (i);
                            sender.SendTo (packet, SSDP::V4::kSocketAddress);
                        }
                        catch (...) {
                            DbgTrace ("could not send on {}: {}"_f, i.fInterfaceID, current_exception ());
                        }
                    }
                }
            };
            notify (alive ("upnp:rootdevice"sv, rootUSN, location1, 60s));
            if (not waitForChanges (1)) {
                Stroika::Frameworks::Test::WarnTestIssue ("SSDP_CachingListener_ skipped - our own NOTIFY was not heard within 10 seconds "
                                                          "(this environment probably blocks multicast, or UDP 1900)");
                return;
            }
            // the same USN at another LOCATION: an advertisement of its own
            notify (alive ("upnp:rootdevice"sv, rootUSN, location2, 60s));
            EXPECT_TRUE (waitForChanges (2));
            // heard again: kept longer - not added again
            notify (alive ("upnp:rootdevice"sv, rootUSN, location1, 60s));
            Execution::Sleep (500ms);
            EXPECT_EQ (cached ().size (), 2u);
            EXPECT_TRUE (cached ().All ([] (const SSDP::Advertisement& a) { return a.fAlive == true; }));
            // withdrawn: at every LOCATION
            notify (byebye ("upnp:rootdevice"sv, rootUSN));
            EXPECT_TRUE (waitForChanges (4));
            EXPECT_EQ (cached ().size (), 0u);
            // expired: removed when its max-age runs out, with no ssdp:byebye
            const Time::TimePointSeconds sentAt = Time::GetTickCount ();
            notify (alive (deviceUSN, deviceUSN, location1, 1s));
            EXPECT_TRUE (waitForChanges (6));
            EXPECT_GE ((Time::GetTickCount () - sentAt).count (), 1.0);
            EXPECT_EQ (cached ().size (), 0u);
            Execution::Sleep (500ms); // for anything after it
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_CachingListener_ skipped - could not run an SSDP listener here: {}"_f(current_exception ()).AsNarrowSDKString ().c_str ());
            return;
        }
        Containers::Sequence<SSDP::Advertisement> changes = told.load ();
        for (const SSDP::Advertisement& a : changes) {
            DbgTrace ("told: {}"_f, a);
        }
        // each change told once, in order - though each NOTIFY came once per interface; fAlive says which: added or removed
        ASSERT_EQ (changes.size (), 6u);
        auto is = [] (const SSDP::Advertisement& a, bool alive, const String& usn, const URI& location) {
            return a.fAlive == alive and a.fUSN == usn and a.fLocation == location;
        };
        EXPECT_TRUE (is (changes[0], true, rootUSN, location1));
        EXPECT_TRUE (is (changes[1], true, rootUSN, location2));
        // the byebye removes the USN at both LOCATIONs, in either order - each told as last heard (a byebye has no LOCATION)
        EXPECT_TRUE ((is (changes[2], false, rootUSN, location1) and is (changes[3], false, rootUSN, location2)) or
                     (is (changes[2], false, rootUSN, location2) and is (changes[3], false, rootUSN, location1)));
        EXPECT_TRUE (is (changes[4], true, deviceUSN, location1));
        EXPECT_TRUE (is (changes[5], false, deviceUSN, location1));
    }

    /*
     *  A CachingListener's search finds what is there already - our own BasicServer, which announced itself before the
     *  CachingListener existed - and the server's ssdp:byebyes, as it stops, remove all that was found. As in
     *  SSDP_Loopback_Search_, anything that keeps the exchange from happening is a test issue.
     *
     *  It searches for the server's device type, unique to this run, so only our server answers: with ssdp:all, every device on
     *  a busy network answers too, and the search's thread can fall far enough behind that an answer from our server is read
     *  after its byebye, adding back what that removed (@see CachingListener).
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_CachingListener_Search_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_CachingListener_Search_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by BasicServer and CachingListener
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        using SSDP::Client::CachingListener;
        const URI    location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, uint16_t{49152}}, "/device.xml"sv}; // only advertised
        const String deviceID   = Common::GUID::GenerateNew ().As<String> ();
        const String deviceType = "urn:stroika-regression-test:device:SSDPCachingListener-{}:1"_f(deviceID);
        Device       d;
        d.fDeviceID = deviceID;
        d.fServer   = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
        DeviceDescription dd;
        dd.fDeviceType   = deviceType;
        dd.fFriendlyName = "Stroika regression test device"sv;
        dd.fUDN          = "uuid:" + deviceID;
        // a search for its device type finds that one of its advertisements (at each LOCATION)
        const Containers::Set<String>                                      usns{"uuid:{}::{}"_f(deviceID, deviceType)};
        Execution::Synchronized<Containers::Sequence<SSDP::Advertisement>> told; // ours - Synchronized: the callback runs on its own thread
        auto                                                               usnsTold = [&] (bool alive) {
            Containers::Set<String> result;
            auto lockedTold = told.cget (); // BWA: under its lock, not through a copy (load ()) - as in SSDP_CachingListener_
            for (const SSDP::Advertisement& a : lockedTold.cref ()) {
                if (a.fAlive == alive) {
                    result += a.fUSN;
                }
            }
            return result;
        };
        auto waitUntil = [] (const function<bool ()>& done) {
            for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; not done () and Time::GetTickCount () < giveUpAt;) {
                Execution::Sleep (50ms);
            }
            return done ();
        };
        try {
            optional<SSDP::Server::BasicServer> server;
            server.emplace (d, dd, SSDP::Server::LocationFillingInHost (location),
                            SSDP::Server::BasicServer::Options{.fIPVersion = IPVersionSupport::eIPV4Only});
            Execution::Sleep (1s); // the NOTIFYs it starts with are done: what the CachingListener has, its search found
            CachingListener cache{[&] (const SSDP::Advertisement& a) {
                                      if (a.fUSN.Contains (deviceID)) {
                                          told.rwget ()->Append (a);
                                      }
                                  },
                                  CachingListener::Options{.fListener = {.fIPVersion = IPVersionSupport::eIPV4Only}, .fSearchFor = deviceType},
                                  CachingListener::eAutoStart};
            if (not waitUntil ([&] () { return usnsTold (true).ContainsAll (usns); })) {
                Stroika::Frameworks::Test::WarnTestIssue ("SSDP_CachingListener_Search_ skipped - our own device was not found within 10 "
                                                          "seconds (this environment probably blocks multicast, or UDP 1900)");
                return;
            }
            Execution::Sleep (3s); // more answers (to its second M-SEARCH, 2 seconds in): those still waiting as the server stops are dropped
                                   // - its responder stops before its ssdp:byebyes - so no answer crosses them
            server.reset ();       // it stops: an ssdp:byebye for each advertisement
            EXPECT_TRUE (waitUntil ([&] () { return usnsTold (false).ContainsAll (usns); }));
            Execution::Sleep (500ms); // for anything after it
            EXPECT_FALSE (cache.GetAdvertisements ().Any ([&] (const SSDP::Advertisement& a) { return a.fUSN.Contains (deviceID); }));
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_CachingListener_Search_ skipped - could not run an SSDP server and CachingListener here: {}"_f(current_exception ())
                    .AsNarrowSDKString ()
                    .c_str ());
            return;
        }
        // each advertisement found - at each LOCATION - added once, then removed once
        Containers::Sequence<SSDP::Advertisement> changes = told.load ();
        for (const SSDP::Advertisement& a : changes) {
            DbgTrace ("told: {}"_f, a);
        }
        std::map<pair<String, URI>, Containers::Sequence<bool>> alives;
        for (const SSDP::Advertisement& a : changes) {
            alives[make_pair (a.fUSN, a.fLocation)].Append (a.fAlive == true);
        }
        for (const auto& [key, alive] : alives) {
            EXPECT_EQ (alive, (Containers::Sequence<bool>{true, false}))
                << key.first.AsNarrowSDKString () << " at " << Characters::ToString (key.second).AsNarrowSDKString ();
        }
    }

    // the first network interface SSDP talks on by default that has an IPv4 address - for a test sending out of just one
    optional<Interface> AnSSDPInterface_ ()
    {
        return SystemInterfacesMgr{}.GetAll ().First ([] (const Interface& i) {
            return SSDP::DefaultInterfaceFilter (i) and i.fBindings.fAddresses.Any ([] (const InternetAddress& a) {
                return a.GetAddressFamily () == InternetAddress::AddressFamily::V4;
            });
        });
    }

    /*
     *  A device answers a multicast M-SEARCH after a random wait of up to its MX seconds - each answer a wait of its own, so
     *  devices, and a device's several answers, do not all come at once - and ignores one with no MX (UPnP Device Architecture
     *  1.1, section 1.3.3). Told by M-SEARCHes this test sends our own BasicServer itself, out of one interface. As in
     *  SSDP_Loopback_Search_, anything that keeps the exchange from happening is a test issue.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Loopback_SearchAnswerWaits_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_Loopback_SearchAnswerWaits_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by BasicServer
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        const URI    location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, uint16_t{49152}}, "/device.xml"sv}; // only advertised
        const String deviceID   = Common::GUID::GenerateNew ().As<String> ();
        const String deviceType = "urn:stroika-regression-test:device:SSDPAnswerWaits-{}:1"_f(deviceID);
        Device       d;
        d.fDeviceID = deviceID;
        d.fServer   = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
        DeviceDescription dd;
        dd.fDeviceType          = deviceType;
        dd.fFriendlyName        = "Stroika regression test device"sv;
        dd.fUDN                 = "uuid:" + deviceID;
        optional<Interface> via = AnSSDPInterface_ ();
        if (not via) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_SearchAnswerWaits_ skipped - no network interface with an IPv4 address");
            return;
        }
        try {
            SSDP::Server::BasicServer server{d, dd, SSDP::Server::LocationFillingInHost (location),
                                             SSDP::Server::BasicServer::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
            ConnectionlessSocket::Ptr asker = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            asker.SetMulticastLoopMode (true); // so our own server hears it
            asker.SetMulticastInterface (*via);
            auto search = [&] (const String& searchTarget, optional<unsigned int> mx) {
                String request = "M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nST: {}\r\n{}\r\n"_f(
                    searchTarget, mx ? "MX: {}\r\n"_f(*mx) : String{});
                string utf8 = request.AsUTF8<string> ();
                asker.SendTo (as_bytes (span{utf8}), SSDP::V4::kSocketAddress);
            };
            // when each answer from our device came, until until - or until enough have
            auto answersUntil = [&] (Time::TimePointSeconds until, size_t enough = numeric_limits<size_t>::max ()) {
                Containers::Sequence<Time::TimePointSeconds> result;
                while (result.size () < enough and not Execution::WaitForIOReady<ConnectionlessSocket::Ptr>{asker}.WaitQuietlyUntil (until).empty ()) {
                    std::byte           buf[8 * 1024];
                    SocketAddress       from;
                    String              headLine;
                    SSDP::Advertisement a;
                    SSDP::DeSerialize (Memory::BLOB{asker.ReceiveFrom (span{buf}, 0, &from)}, &headLine, &a);
                    if (headLine.StartsWith ("HTTP/1.1 200"sv) and a.fUSN.Contains (deviceID)) {
                        result += Time::GetTickCount ();
                    }
                }
                return result;
            };
            // until the server hears us (it joins the multicast group on its own thread): search till it answers - then let the
            // rest of those answers come
            bool answered = false;
            for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; not answered and Time::GetTickCount () < giveUpAt;) {
                search (deviceType, 1);
                answered = not answersUntil (Time::GetTickCount () + 1.5s, 1).empty ();
            }
            if (not answered) {
                Stroika::Frameworks::Test::WarnTestIssue (
                    "SSDP_Loopback_SearchAnswerWaits_ skipped - our own device did not answer within 10 "
                    "seconds (this environment probably blocks multicast, or UDP 1900)");
                return;
            }
            (void)answersUntil (Time::GetTickCount () + 1.5s);
            // MX 1: each answer - one for each of its advertisements (3), searched for ssdp:all - after a random wait of up to a second
            const Time::TimePointSeconds sentAt = Time::GetTickCount ();
            for (int i = 0; i < 3; ++i) {
                search (SSDP::kTarget_SSDPAll, 1);
            }
            Containers::Sequence<Time::TimePointSeconds> came = answersUntil (sentAt + 3s); // all of them, before asking anything else
            EXPECT_GE (came.size (), 3u * 3u); // (more if the OS hands the server a search more than once)
            if (came.empty ()) {
                return;
            }
            DbgTrace ("{} answers, {} to {} seconds after"_f, came.size (), (came.MinValue () - sentAt).count (),
                      (came.MaxValue () - sentAt).count ());
            EXPECT_GE ((came.MaxValue () - came.MinValue ()).count (), 0.1) << "all at once";
            EXPECT_LE ((came.MaxValue () - sentAt).count (), 2.5)
                << "longer than the MX (1 second) after, even allowing for slow processing";
            // no MX: ignored - asked once those answers are all in, so the server is known to hear us
            search (deviceType, nullopt);
            EXPECT_TRUE (answersUntil (Time::GetTickCount () + 1.5s).empty ()) << "answered a multicast M-SEARCH with no MX";
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue ("SSDP_Loopback_SearchAnswerWaits_ skipped - could not run an SSDP server here: {}"_f(current_exception ())
                                                          .AsNarrowSDKString ()
                                                          .c_str ());
        }
    }

    /*
     *  A device answers a search for an older version of its device or service type too, as that version - a :2 device answers a
     *  search for :1, with :1 in its ST and USN (UPnP Device Architecture 1.1, sections 1.3.2 and 1.3.3) - but not a search for a
     *  newer version than it has. Told by M-SEARCHes this test sends our own BasicServer itself, out of one interface. As in
     *  SSDP_Loopback_Search_, anything that keeps the exchange from happening is a test issue.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Loopback_SearchOlderVersion_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_Loopback_SearchOlderVersion_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by BasicServer
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        const URI    location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, uint16_t{49152}}, "/device.xml"sv}; // only advertised
        const String deviceID = Common::GUID::GenerateNew ().As<String> ();
        // its device and service types, at the given version: the device's is at 2, its service's at 3
        auto deviceType = [&] (int version) -> String {
            return "urn:stroika-regression-test:device:SSDPVersions-{}:{}"_f(deviceID, version);
        };
        auto serviceType = [&] (int version) -> String {
            return "urn:stroika-regression-test:service:SSDPVersions-{}:{}"_f(deviceID, version);
        };
        Device d;
        d.fDeviceID = deviceID;
        d.fServer   = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
        DeviceDescription dd;
        dd.fDeviceType   = deviceType (2);
        dd.fFriendlyName = "Stroika regression test device"sv;
        dd.fUDN          = "uuid:" + deviceID;
        dd.fServices     = Containers::Collection<DeviceDescription::Service>{
            DeviceDescription::Service{.fServiceType = serviceType (3), .fServiceID = "urn:stroika-regression-test:serviceId:SSDPVersions"sv}};
        optional<Interface> via = AnSSDPInterface_ ();
        if (not via) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_SearchOlderVersion_ skipped - no network interface with an IPv4 address");
            return;
        }
        try {
            SSDP::Server::BasicServer server{d, dd, SSDP::Server::LocationFillingInHost (location),
                                             SSDP::Server::BasicServer::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
            ConnectionlessSocket::Ptr asker = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            asker.SetMulticastLoopMode (true); // so our own server hears it
            asker.SetMulticastInterface (*via);
            auto search = [&] (const String& searchTarget) {
                string request =
                    "M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nST: {}\r\nMX: 1\r\n\r\n"_f(searchTarget).AsUTF8<string> ();
                asker.SendTo (as_bytes (span{request}), SSDP::V4::kSocketAddress);
            };
            // the answers from our device until until - or until enough have come
            auto answersUntil = [&] (Time::TimePointSeconds until, size_t enough = numeric_limits<size_t>::max ()) {
                Containers::Sequence<SSDP::Advertisement> result;
                while (result.size () < enough and not Execution::WaitForIOReady<ConnectionlessSocket::Ptr>{asker}.WaitQuietlyUntil (until).empty ()) {
                    std::byte           buf[8 * 1024];
                    SocketAddress       from;
                    String              headLine;
                    SSDP::Advertisement a;
                    SSDP::DeSerialize (Memory::BLOB{asker.ReceiveFrom (span{buf}, 0, &from)}, &headLine, &a);
                    if (headLine.StartsWith ("HTTP/1.1 200"sv) and a.fUSN.Contains (deviceID)) {
                        result += a;
                    }
                }
                return result;
            };
            // until the server hears us (it joins the multicast group on its own thread): search for its own device type till it answers
            bool answered = false;
            for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; not answered and Time::GetTickCount () < giveUpAt;) {
                search (deviceType (2));
                answered = not answersUntil (Time::GetTickCount () + 1.5s, 1).empty ();
            }
            if (not answered) {
                Stroika::Frameworks::Test::WarnTestIssue (
                    "SSDP_Loopback_SearchOlderVersion_ skipped - our own device did not answer within "
                    "10 seconds (this environment probably blocks multicast, or UDP 1900)");
                return;
            }
            // an older version of each type, and a newer one of the device's than it has - MX 1, so all answered within a second
            for (const String& t : {deviceType (1), serviceType (2), deviceType (3)}) {
                search (t);
            }
            Containers::Sequence<SSDP::Advertisement> came = answersUntil (Time::GetTickCount () + 2.5s); // (allowing for slow processing)
            for (const SSDP::Advertisement& a : came) {
                DbgTrace ("answer: {}"_f, a);
                EXPECT_EQ (a.fUSN, "uuid:{}::{}"_f(deviceID, a.fTarget)) << "its USN names the type at the version its ST does";
            }
            auto answeredAs = [&] (const String& t) { return came.Any ([&] (const SSDP::Advertisement& a) { return a.fTarget == t; }); };
            EXPECT_TRUE (answeredAs (deviceType (1))) << "no answer to a search for an older version of its device type";
            EXPECT_TRUE (answeredAs (serviceType (2))) << "no answer to a search for an older version of its service type";
            EXPECT_FALSE (answeredAs (deviceType (3))) << "answered a search for a newer version of its device type than it has";
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_SearchOlderVersion_ skipped - could not run an SSDP server here: {}"_f(current_exception ())
                    .AsNarrowSDKString ()
                    .c_str ());
        }
    }

    /*
     *  A PeriodicNotifier waits a random 0 to 100 ms before its first NOTIFYs, so devices starting together - after a power cut,
     *  say - do not all announce at once (UPnP Device Architecture 1.1, section 1.2.2). Seen as how long each of several is heard
     *  after its construction began: not all the same. As in SSDP_Loopback_Notify_, anything that keeps the exchange from
     *  happening is a test issue.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Loopback_NotifyInitialWait_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_Loopback_NotifyInitialWait_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by PeriodicNotifier
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        const URI    location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, uint16_t{49152}}, "/device.xml"sv}; // only advertised
        const String runID      = Common::GUID::GenerateNew ().As<String> ();
        optional<Interface> via = AnSSDPInterface_ ();
        if (not via) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_NotifyInitialWait_ skipped - no network interface with an IPv4 address");
            return;
        }
        const SSDP::InterfaceFilter onlyVia = [id = via->fInterfaceID] (const Interface& i) { return i.fInterfaceID == id; };
        Execution::Synchronized<Containers::Mapping<String, Time::TimePointSeconds>> firstHeard; // by USN - Synchronized: the listener calls back on its own thread
        Containers::Sequence<Time::DurationSeconds> waits;
        try {
            SSDP::Client::Listener listener{[&] (const SSDP::Advertisement& a) {
                                                if (a.fUSN.Contains (runID) and a.fAlive == true) {
                                                    auto heard = firstHeard.rwget ();
                                                    if (not heard->ContainsKey (a.fUSN)) {
                                                        heard->Add (a.fUSN, Time::GetTickCount ());
                                                    }
                                                }
                                            },
                                            SSDP::Client::Listener::Options{.fIPVersion = IPVersionSupport::eIPV4Only},
                                            SSDP::Client::Listener::eAutoStart};
            for (int i = 0; i < 8; ++i) {
                SSDP::Advertisement a;
                a.fTarget                                = SSDP::kTarget_UPNPRootDevice;
                a.fUSN                                   = "uuid:{}-{}::upnp:rootdevice"_f(runID, i);
                a.fServer                                = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
                const Time::TimePointSeconds   startedAt = Time::GetTickCount ();
                SSDP::Server::PeriodicNotifier notifier{
                    Containers::Sequence<SSDP::Advertisement>{a}, SSDP::Server::LocationFillingInHost (location),
                    SSDP::Server::PeriodicNotifier::Options{.fIPVersion = IPVersionSupport::eIPV4Only, .fInterfaces = onlyVia}};
                for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 5s;
                     not firstHeard.cget ()->ContainsKey (a.fUSN) and Time::GetTickCount () < giveUpAt;) {
                    Execution::Sleep (10ms);
                }
                optional<Time::TimePointSeconds> heardAt = firstHeard.cget ()->Lookup (a.fUSN);
                if (not heardAt) {
                    Stroika::Frameworks::Test::WarnTestIssue (
                        "SSDP_Loopback_NotifyInitialWait_ skipped - our own NOTIFY was not heard within 5 "
                        "seconds (this environment probably blocks multicast, or UDP 1900)");
                    return;
                }
                waits += *heardAt - startedAt;
            } // each notifier says ssdp:byebye as it goes
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_NotifyInitialWait_ skipped - could not run an SSDP listener and notifier here: {}"_f(current_exception ())
                    .AsNarrowSDKString ()
                    .c_str ());
            return;
        }
        DbgTrace ("heard after: {}"_f, waits);
        EXPECT_GE ((waits.MaxValue () - waits.MinValue ()).count (), 0.015) << "all heard as soon after construction: no random wait";
        EXPECT_LE (waits.MaxValue ().count (), 1.0);
    }

    /*
     *  A PeriodicNotifier's NOTIFYs repeat after a random pause - between half its fRepeatInterval and all of it - so devices
     *  do not keep announcing together (UPnP Device Architecture 1.1, section 1.2.2: "a randomly-distributed interval"). Seen
     *  over a few seconds, with a short fRepeatInterval: the pauses between its rounds of NOTIFYs are not all the same. As in
     *  SSDP_Loopback_Notify_, anything that keeps the exchange from happening is a test issue.
     */
    GTEST_TEST (Frameworks_UPnP, SSDP_Loopback_NotifyRepeatSpread_)
    {
        Debug::TraceContextBumper                    ctx{"SSDP_Loopback_NotifyRepeatSpread_"};
        Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // required by PeriodicNotifier
        using IO::Network::InternetProtocol::IP::IPVersionSupport;
        constexpr Time::DurationSeconds kRepeatInterval_{0.5};
        const URI    location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, uint16_t{49152}}, "/device.xml"sv}; // only advertised
        const String usn        = "uuid:{}::upnp:rootdevice"_f(Common::GUID::GenerateNew ().As<String> ());
        optional<Interface> via = AnSSDPInterface_ ();
        if (not via) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_NotifyRepeatSpread_ skipped - no network interface with an IPv4 address");
            return;
        }
        const SSDP::InterfaceFilter onlyVia = [id = via->fInterfaceID] (const Interface& i) { return i.fInterfaceID == id; };
        Execution::Synchronized<Containers::Sequence<Time::TimePointSeconds>> heard; // when each of its NOTIFYs came - Synchronized: the listener calls back on its own thread
        try {
            SSDP::Client::Listener listener{[&] (const SSDP::Advertisement& a) {
                                                if (a.fUSN == usn and a.fAlive == true) {
                                                    heard.rwget ()->Append (Time::GetTickCount ());
                                                }
                                            },
                                            SSDP::Client::Listener::Options{.fIPVersion = IPVersionSupport::eIPV4Only},
                                            SSDP::Client::Listener::eAutoStart};
            SSDP::Advertisement    a;
            a.fTarget = SSDP::kTarget_UPNPRootDevice;
            a.fUSN    = usn;
            a.fServer = SSDP::MakeServerHeaderValue ("StroikaRegressionTest/1.0"sv);
            SSDP::Server::PeriodicNotifier notifier{Containers::Sequence<SSDP::Advertisement>{a}, SSDP::Server::LocationFillingInHost (location),
                                                    SSDP::Server::PeriodicNotifier::Options{.fFrequencyInfo = {.fRepeatInterval = kRepeatInterval_},
                                                                                            .fIPVersion  = IPVersionSupport::eIPV4Only,
                                                                                            .fInterfaces = onlyVia}};
            Execution::Sleep (5s);
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SSDP_Loopback_NotifyRepeatSpread_ skipped - could not run an SSDP listener and notifier here: {}"_f(current_exception ())
                    .AsNarrowSDKString ()
                    .c_str ());
            return;
        }
        // each round of NOTIFYs is two sets, 100 ms apart - here a NOTIFY each, out of one interface - so a longer gap is a pause.
        // Leaving out the first: it follows the round sent as the notifier starts, which the timer does not time from
        Containers::Sequence<Time::DurationSeconds> pauses;
        optional<Time::TimePointSeconds>            last;
        bool                                        first = true;
        for (Time::TimePointSeconds t : heard.load ()) {
            if (last and t - *last > 0.2s) {
                if (not first) {
                    pauses += t - *last;
                }
                first = false;
            }
            last = t;
        }
        if (pauses.size () < 5) {
            Stroika::Frameworks::Test::WarnTestIssue ("SSDP_Loopback_NotifyRepeatSpread_ skipped - too few of our own NOTIFYs heard (this "
                                                      "environment probably blocks multicast, or UDP 1900)");
            return;
        }
        DbgTrace ("pauses: {}"_f, pauses);
        EXPECT_GE (pauses.MinValue ().count (), kRepeatInterval_.count () / 2 - 0.05) << Characters::ToString (pauses).AsNarrowSDKString ();
        EXPECT_LE (pauses.MaxValue ().count (), kRepeatInterval_.count () + 0.15) << Characters::ToString (pauses).AsNarrowSDKString ();
        EXPECT_GE ((pauses.MaxValue () - pauses.MinValue ()).count (), 0.03)
            << "every pause the same: not random - " << Characters::ToString (pauses).AsNarrowSDKString ();
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
