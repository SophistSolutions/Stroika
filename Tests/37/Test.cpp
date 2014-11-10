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
        {
            URL url { L"localhost" };
            VerifyTestResult (url.GetProtocol () == L"http");
            VerifyTestResult (url.GetHost () == L"localhost");
            VerifyTestResult (url.GetEffectivePortNumber () == 80);
            VerifyTestResult (url.GetHostRelativePath () == L"");
            VerifyTestResult (url.GetQueryString () == L"");
            VerifyTestResult (url.GetFragment () == L"");
            VerifyTestResult (url.GetFullURL () == L"http://localhost/");
        }
        {
            // Test case/examples from:
            //      https://docs.python.org/2/library/urlparse.html
            //
            //  Though the names of our attributes differ, and our results, somewhat differ...
            {
                URL url { L"http://www.cwi.nl:80/%7Eguido/Python.html" };
                VerifyTestResult (url.GetProtocol () == L"http");
                VerifyTestResult (url.GetHost () == L"www.cwi.nl");
                VerifyTestResult (url.GetEffectivePortNumber () == 80);
                VerifyTestResult (url.GetHostRelativePath () == L"%7Eguido/Python.html");   // python includes leading / - we dont
                VerifyTestResult (url.GetQueryString () == L"");
                VerifyTestResult (url.GetFragment () == L"");
                VerifyTestResult (url.GetFullURL () == L"http://www.cwi.nl/%7Eguido/Python.html");
            }
            {
                URL url { L"//www.cwi.nl:80/%7Eguido/Python.html" };
                VerifyTestResult (url.GetProtocol () == L"http");
                VerifyTestResult (url.GetHost () == L"www.cwi.nl");
                VerifyTestResult (url.GetEffectivePortNumber () == 80);
                VerifyTestResult (url.GetHostRelativePath () == L"%7Eguido/Python.html");   // python includes leading / - we dont
                VerifyTestResult (url.GetQueryString () == L"");
                VerifyTestResult (url.GetFragment () == L"");
                VerifyTestResult (url.GetFullURL () == L"http://www.cwi.nl/%7Eguido/Python.html");
            }
        }
        {
            URL url { L"//www.cwi.nl:8080/%7Eguido/Python.html" };
            VerifyTestResult (url.GetProtocol () == L"http");
            VerifyTestResult (url.GetHost () == L"www.cwi.nl");
            VerifyTestResult (url.GetEffectivePortNumber () == 8080);
            VerifyTestResult (url.GetHostRelativePath () == L"%7Eguido/Python.html");
            VerifyTestResult (url.GetQueryString () == L"");
            VerifyTestResult (url.GetFragment () == L"");
            VerifyTestResult (url.GetFullURL () == L"http://www.cwi.nl:8080/%7Eguido/Python.html");
        }
        {
            URL url { L"https://www.cwi.nl/%7Eguido/Python.html" };
            VerifyTestResult (url.GetProtocol () == L"https");
            VerifyTestResult (url.GetHost () == L"www.cwi.nl");
            VerifyTestResult (url.GetEffectivePortNumber () == 443);
            VerifyTestResult (url.GetHostRelativePath () == L"%7Eguido/Python.html");
            VerifyTestResult (url.GetQueryString () == L"");
            VerifyTestResult (url.GetFragment () == L"");
            VerifyTestResult (url.GetFullURL () == L"https://www.cwi.nl/%7Eguido/Python.html");
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



namespace {
    void    Test3_NetworkInterfaceList_ ()
    {
        Debug::TraceContextBumper trcCtx (SDKSTR ("Test3_NetworkInterfaceList_"));
        for (Interface iFace : Network::GetInterfaces ()) {
            Debug::TraceContextBumper trcCtx (SDKSTR ("iface"));
            if (iFace.fInterfaceName.IsPresent ()) {
                DbgTrace (L"interface-name: %s", iFace.fInterfaceName->c_str ());
            }
            if (iFace.fFriendlyName.IsPresent ()) {
                DbgTrace (L"friendly-name: %s", iFace.fFriendlyName->c_str ());
            }
            if (iFace.fDescription.IsPresent ()) {
                DbgTrace (L"description: %s", iFace.fDescription->c_str ());
            }
            if (iFace.fType.IsPresent ()) {
                DbgTrace (L"type: %d",  (int)*iFace.fType);
            }
            for (InternetAddress ipAddr : iFace.fBindings) {
                DbgTrace (L"addr: %s",  ipAddr.As<String> ().c_str ());
            }
            if (iFace.fStatus.IsPresent ()) {
                for (Interface::Status s : *iFace.fStatus) {
                    DbgTrace (L"status: %d",  (int)s);
                }
            }
        }
    }
}



namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_URL_ ();
        Test2_InternetAddress_ ();
//#if     qPlatform_POSIX
        // WINDOZE NYI
        Test3_NetworkInterfaceList_ ();
//#endif
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



