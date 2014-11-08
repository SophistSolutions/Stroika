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
        {
            VerifyTestResult (not V4::kAddrAny.IsLocalhostAddress ());
            VerifyTestResult (V4::kLocalhost.IsLocalhostAddress ());
            VerifyTestResult (V4::kLocalhost.IsLinkLocalAddress ());
            VerifyTestResult (not V6::kAddrAny.IsLocalhostAddress ());
            VerifyTestResult (V6::kLocalhost.IsLocalhostAddress ());
            VerifyTestResult (V6::kLocalhost.IsLinkLocalAddress ());
        }
        {
            VerifyTestResult (InternetAddress (V4::kLocalhost.As<in_addr> ()) == V4::kLocalhost);
            VerifyTestResult (InternetAddress (V4::kLocalhost.As<in_addr> (InternetAddress::ByteOrder::Host)) != V4::kLocalhost or ntohl (0x01020304) == 0x01020304);   // if big-endian machine, net byte order equals host byte order
        }
        {
            const   InternetAddress kSSDPAddr_ { "239.255.255.250" };
            VerifyTestResult (not kSSDPAddr_.IsLocalhostAddress ());
            VerifyTestResult (not kSSDPAddr_.IsPrivateAddress ());
            VerifyTestResult (kSSDPAddr_.IsMulticastAddress ());
        }
        {
            const   InternetAddress kSomeIPV4LinkLocalAddr_ { "169.254.0.1" };
            VerifyTestResult (kSomeIPV4LinkLocalAddr_.IsLinkLocalAddress ());
            VerifyTestResult (not  kSomeIPV4LinkLocalAddr_.IsLocalhostAddress ());
            VerifyTestResult (not  kSomeIPV4LinkLocalAddr_.IsMulticastAddress ());
        }
#if     (qPlatform_POSIX || (qPlatformWindows && (NTDDI_VERSION >= NTDDI_VISTA)))
        {
            const   InternetAddress kSomeIPV6LinkLocalAddr_ { "fe80::44de:4247:5b76:ddc9%4" };
            VerifyTestResult (kSomeIPV6LinkLocalAddr_.IsLinkLocalAddress ());
            VerifyTestResult (not kSomeIPV6LinkLocalAddr_.IsLocalhostAddress ());
            VerifyTestResult (not kSomeIPV6LinkLocalAddr_.IsMulticastAddress ());
        }
#endif
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



