/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::IO::Network
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Foundation/IO/Network/Interface.h"
#include    "Stroika/Foundation/IO/Network/URL.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;


namespace   {
    void    Test1_URL_ ()
    {
        {
            const   wchar_t kTestURL_[] = L"http://www.x.com/foo?bar=3";
            VerifyTestResult (URL (kTestURL_).GetFullURL () == kTestURL_);
            VerifyTestResult (URL (kTestURL_).GetHost () == L"www.x.com");
            VerifyTestResult (URL (kTestURL_).GetHostRelativePath () == L"foo");
            VerifyTestResult (URL (kTestURL_).GetQueryString () == L"bar=3");
            VerifyTestResult (URL (kTestURL_) == URL (L"http", L"www.x.com", L"foo", L"bar=3"));
        }
    }
}


namespace   {
    void    Test2_InternetAddress_ ()
    {
        Debug::TraceContextBumper trcCtx (SDKSTR ("Test2_InternetAddress_"));
        {
            VerifyTestResult ((InternetAddress { 169, 254, 0, 1 }).As<String> () == L"169.254.0.1");
            VerifyTestResult ((InternetAddress { 1, 2, 3, 4 }).As<String> () == L"1.2.3.4");
            VerifyTestResult ((InternetAddress { L"1.2.3.4" }).As<String> () == L"1.2.3.4");
            VerifyTestResult ((InternetAddress { "1.2.3.4" }).As<String> () == L"1.2.3.4");
        }
        {
            VerifyTestResult (std::get<0> (InternetAddress { 1, 2, 3, 4 } .As<tuple<uint8_t, uint8_t, uint8_t, uint8_t>> ()) == 1);
            VerifyTestResult (std::get<2> (InternetAddress { 1, 2, 3, 4 } .As<tuple<uint8_t, uint8_t, uint8_t, uint8_t>> ()) == 3);
        }
        {
            auto    testRoundtrip = [] (const String & s) {
                InternetAddress iaddr1 { s };
                InternetAddress iaddr2 { s.As<wstring> () };
                InternetAddress iaddr3 { s.AsASCII () };
                VerifyTestResult (iaddr1 == iaddr2);
                VerifyTestResult (iaddr2 == iaddr3);
                VerifyTestResult (iaddr1.As<String> () == s);
                VerifyTestResult (iaddr2.As<String> () == s);
                VerifyTestResult (iaddr3.As<String> () == s);
            };
            testRoundtrip (L"192.168.131.3");
            testRoundtrip (L"::");
            testRoundtrip (L"fec0:0:0:ffff::1");
            testRoundtrip (L"fe80::44de:4247:5b76:ddc9");
        }
        {
            struct  Tester {
                InternetAddress addr;
                bool    isLocalHost;
                bool    isLinkLocal;
                bool    isMulticast;
                bool    isPrivate;
            };
            const   InternetAddress kSamplePrivateAddr_ { "192.168.244.121" };
            const   InternetAddress kSSDPAddr_ { 239, 255, 255, 250 };
            const   InternetAddress kSomeIPV4LinkLocalAddr_ { "169.254.0.1" };
            const   InternetAddress kSomeIPV6LinkLocalAddr_ { "fe80::44de:4247:5b76:ddc9" };
            const Tester  kTests_ [] = {
                //  ADDR                                    localhost   linklocal   multicast   privateaddr
                {   V4::kAddrAny,                           false,      false,      false,      false       },
                {   V6::kAddrAny,                           false,      false,      false,      false       },
                {   V4::kLocalhost,                         true,       false,      false,      false       },
                {   V6::kLocalhost,                         true,       false,      false,      false       },
                {   kSSDPAddr_,                             false,      false,      true,       false       },
                {   kSomeIPV4LinkLocalAddr_,                false,      true,       false,      false       },
                {   kSomeIPV6LinkLocalAddr_,                false,      true,       false,      false       },
                {   kSamplePrivateAddr_,                    false,      false,      false,      true        },
                {   InternetAddress{10, 0, 0, 0},           false,      false,      false,      true        },
                {   InternetAddress{10, 255, 255, 255},     false,      false,      false,      true        },
                {   InternetAddress{172, 16, 0, 1},         false,      false,      false,      true        },
                {   InternetAddress{172, 16, 0, 3},         false,      false,      false,      true        },
                {   InternetAddress{172, 31, 255, 255},     false,      false,      false,      true        },
                {   InternetAddress{192, 168, 0, 0},        false,      false,      false,      true        },
                {   InternetAddress{192, 168, 255, 255},    false,      false,      false,      true        },
            };
            for (auto i : kTests_) {
                DbgTrace (L"i.addr=%s", i.addr.As<String> ().c_str ());
                VerifyTestResult (i.addr.IsLocalhostAddress () == i.isLocalHost);
                VerifyTestResult (i.addr.IsLinkLocalAddress () == i.isLinkLocal);
                VerifyTestResult (i.addr.IsMulticastAddress () == i.isMulticast);
                VerifyTestResult (i.addr.IsPrivateAddress () == i.isPrivate);
            }
        }
        {
            VerifyTestResult (InternetAddress (V4::kLocalhost.As<in_addr> ()) == V4::kLocalhost);
            VerifyTestResult (InternetAddress (V4::kLocalhost.As<in_addr> (InternetAddress::ByteOrder::Host)) != V4::kLocalhost or ntohl (0x01020304) == 0x01020304);   // if big-endian machine, net byte order equals host byte order
        }
    }
}

#if 0
#include <iostream>
#endif

namespace {
    void    Test3_NetworkInterfaceList_ ()
    {
        for (Interface iFace : Network::GetInterfaces ()) {
#if 0
            cout << "iface: " << endl;
            cout << "  name: " << iFace.fInterfaceName.AsUTF8 () << endl;
            if (iFace.fType.IsPresent ()) {
                cout << "  type: " << (int)*iFace.fType << endl;
            }
            for (InternetAddress ipAddr : iFace.fBindings) {
                cout << "  addr: " << ipAddr.As<String> ().AsUTF8 () << endl;
            }
            if (iFace.fStatus.IsPresent ()) {
                for (Interface::Status s : *iFace.fStatus) {
                    cout << "  status: " << (int)s << endl;
                }
            }
#endif
        }
    }
}



namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_URL_ ();
        Test2_InternetAddress_ ();
#if     qPlatform_POSIX
        // WINDOZE NYI
        Test3_NetworkInterfaceList_ ();
#endif
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



