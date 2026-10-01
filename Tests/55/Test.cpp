/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Frameworks::UPnP
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h" // for qStroika_Foundation_DataExchange_XML_SupportParsing
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/TestHarness.h"
#include "Stroika/Frameworks/UPnP/DeviceDescription.h"
#include "Stroika/Frameworks/UPnP/SSDP/Advertisement.h"

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
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
