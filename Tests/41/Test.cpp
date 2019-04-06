/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
//  TEST    Foundation::IO::Network
#include "Stroika/Foundation/StroikaPreComp.h"

#if qPlatform_Windows
#if qHasFeature_ATLMFC
#include <atlbase.h>
#endif

#include <Windows.h>

#include <URLMon.h>
#endif

#include "Stroika/Foundation/Characters/String_Constant.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include "Stroika/Foundation/IO/Network/DNS.h"
#include "Stroika/Foundation/IO/Network/Interface.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/IO/Network/URL.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using Characters::String_Constant;

#if qPlatform_Windows && qHasFeature_ATLMFC
namespace {
    DISABLE_COMPILER_MSC_WARNING_START (6262)
    void OLD_Cracker_ (const String& w, String* protocol, String* host, [[maybe_unused]] String* port, String* relPath, String* query)
    {
        using Stroika::Foundation::Execution::Platform::Windows::ThrowIfErrorHRESULT;
        RequireNotNull (protocol);
        RequireNotNull (host);
        RequireNotNull (relPath);
        RequireNotNull (query);

        DWORD   ingored = 0;
        wchar_t outBuf[10 * 1024];

        String canonical;
        ThrowIfErrorHRESULT (::CoInternetParseUrl (CComBSTR (w.c_str ()), PARSE_CANONICALIZE, 0, outBuf, static_cast<DWORD> (NEltsOf (outBuf)), &ingored, 0));
        canonical = outBuf;

        {
            size_t e = canonical.find (':');
            if (e != String::npos) {
                *protocol = canonical.SubString (0, e);
            }
        }

        if (SUCCEEDED (::CoInternetParseUrl (CComBSTR (canonical.c_str ()), PARSE_DOMAIN, 0, outBuf, static_cast<DWORD> (NEltsOf (outBuf)), &ingored, 0))) {
            *host = outBuf;
        }

        // I cannot see how to get other fields using CoInternetParseURL??? - LGP 2004-04-13...
        {
            String matchStr    = *protocol + String_Constant (L"://") + *host;
            size_t startOfPath = canonical.Find (matchStr).value_or (String::npos);
            if (startOfPath == String::npos) {
                matchStr    = *protocol + String_Constant (L":");
                startOfPath = canonical.Find (matchStr).value_or (String::npos);
            }
            if (startOfPath == String::npos) {
                startOfPath = canonical.length ();
            }
            else {
                startOfPath += matchStr.length ();
            }
            *relPath = canonical.SubString (startOfPath);

            size_t startOfQuery = relPath->find ('?');
            if (startOfQuery != String::npos) {
                *query = relPath->SubString (startOfQuery + 1);
                relPath->erase (startOfQuery);
            }
        }
    }
    DISABLE_COMPILER_MSC_WARNING_END (6262)
}
#endif

namespace {
    namespace Test1_URL_Parsing_ {
        namespace Private_ {
            void TestOldWinCracker_ ([[maybe_unused]] const String& w, [[maybe_unused]] const URL& url)
            {
#if qPlatform_Windows && qHasFeature_ATLMFC
                {
                    String testProtocol;
                    String testHost;
                    String testPort;
                    String testRelPath;
                    String testQuery;
                    OLD_Cracker_ (w, &testProtocol, &testHost, &testPort, &testRelPath, &testQuery);
                    VerifyTestResult (testProtocol == url.GetScheme ());
                    if (testProtocol == L"http") {
                        VerifyTestResult (testHost == url.GetHost ().ToLowerCase ());
                        {
                            //Assert (testPort == fPort);
                            if (url.GetPortValue () == 80) {
                                VerifyTestResult (testPort == L"" or testPort == L"80");
                            }
                            else {
                                // apparently never really implemented in old cracker...
                                //Assert (fPort == ::_wtoi (testPort.c_str ()));
                            }
                        }
                        VerifyTestResult (testRelPath == url.GetHostRelativePath () or testRelPath.find (':') != String::npos or ((String_Constant (L"/") + url.GetHostRelativePath ()) == testRelPath)); //old code didnt handle port#   --LGP 2007-09-20
                        VerifyTestResult (testQuery == url.GetQueryString () or not url.GetFragment ().empty ());                                                                                         // old code didn't check fragment
                    }
                }
#endif
            }
            void TestOldWinCracker_ (const String& w)
            {
                TestOldWinCracker_ (w, URL (w, URL::eStroikaPre20a50BackCompatMode));
            }
            void TestBackCompatURL_ ()
            {
                TestOldWinCracker_ (L"dyn:/Reminders/Home.htm");
                TestOldWinCracker_ (L"dyn:/Startup.htm");
                TestOldWinCracker_ (L"home:Home.htm");
                {
                    URL url = URL (L"dyn:/StyleSheet.css?ThemeName=Cupertino", URL::eStroikaPre20a50BackCompatMode);
                    VerifyTestResult (url.GetScheme () == L"dyn");
                    VerifyTestResult (url.GetHost ().empty ());
                    VerifyTestResult (url.GetHostRelativePath () == L"StyleSheet.css");
                    VerifyTestResult (url.GetQueryString () == L"ThemeName=Cupertino");
                }
            }
            void BasicTests_AsOf21d22_ ()
            {
                {
                    URL url = URL::Parse (L"http:/StyleSheet.css?ThemeName=Cupertino", URL::eFlexiblyAsUI);
                    VerifyTestResult (url.GetPortValue () == 80);
                    VerifyTestResult (url.GetQueryString () == L"ThemeName=Cupertino");
                    VerifyTestResult (url.GetHost ().empty ());
                    VerifyTestResult (url.GetHostRelativePath () == L"StyleSheet.css");
                    VerifyTestResult (url.GetFragment ().empty ());
                    VerifyTestResult (url.GetScheme () == L"http");
                }
                {
                    URL url = URL::Parse (L"http://www.recordsforliving.com/");
                    VerifyTestResult (url.GetPortValue () == 80);
                    VerifyTestResult (url.GetQueryString ().empty ());
                    VerifyTestResult (url.GetFragment ().empty ());
                    VerifyTestResult (url.GetHostRelativePath ().empty ());
                    VerifyTestResult (url.GetHost () == L"www.recordsforliving.com");
                    VerifyTestResult (url.GetScheme () == L"http");
                    VerifyTestResult (not url.IsSecure ());
                }
                {
                    URL url = URL::Parse (L"https://xxx.recordsforliving.com/");
                    VerifyTestResult (url.GetPortValue () == 443);
                    VerifyTestResult (url.GetQueryString ().empty ());
                    VerifyTestResult (url.GetFragment ().empty ());
                    VerifyTestResult (url.GetHostRelativePath ().empty ());
                    VerifyTestResult (url.GetHost () == L"xxx.recordsforliving.com");
                    VerifyTestResult (url.GetScheme () == L"https");
                    VerifyTestResult (url.IsSecure ());
                }
                for (auto po : {URL::eAsFullURL, URL::eFlexiblyAsUI}) {
                    const wchar_t kTestURL_[] = L"http://www.x.com/foo?bar=3";
                    VerifyTestResult (URL::Parse (kTestURL_, po).GetFullURL () == kTestURL_);
                    VerifyTestResult (URL::Parse (kTestURL_, po).GetHost () == L"www.x.com");
                    VerifyTestResult (URL::Parse (kTestURL_, po).GetHostRelativePath () == L"foo");
                    VerifyTestResult (URL::Parse (kTestURL_, po).GetQueryString () == L"bar=3");
                    VerifyTestResult (URL::Parse (kTestURL_, po) == URL (L"http", L"www.x.com", L"foo", L"bar=3"));
                }
                {
                    URL url{URL::Parse (L"localhost", URL::eFlexiblyAsUI)};
                    //VerifyTestResult (not url.GetScheme ().has_value ()); // UNCLEAR WHAT THIS SHOULD DO - MUST RETHINK - NOT WELL DEFINED
                    VerifyTestResult (url.GetScheme () == L"http"); // DITTO
                    VerifyTestResult (url.GetHost () == L"localhost");
                    VerifyTestResult (url.GetPortValue () == 80);
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
                        URL url = URL::Parse (L"http://www.cwi.nl:80/%7Eguido/Python.html", URL::eFlexiblyAsUI);
                        VerifyTestResult (url.GetHost () == L"www.cwi.nl");
                        VerifyTestResult (url.GetPortValue () == 80);
                        VerifyTestResult (url.GetHostRelativePath () == L"%7Eguido/Python.html"); // python includes leading / - we don't
                        VerifyTestResult (url.GetQueryString () == L"");
                        VerifyTestResult (url.GetFragment () == L"");
                        VerifyTestResult (url.GetFullURL () == L"http://www.cwi.nl/%7Eguido/Python.html");
                    }
#if 0
                    // Unclear why this  (#if section/comment) is needed, but without it, clang-format screws up the indent level
#endif
                    {
                        URL url{URL::Parse (L"//www.cwi.nl:80/%7Eguido/Python.html", URL::eFlexiblyAsUI)};
                        VerifyTestResult (url.GetHost () == L"www.cwi.nl");
                        VerifyTestResult (url.GetPortValue () == 80);
                        VerifyTestResult (url.GetHostRelativePath () == L"%7Eguido/Python.html"); // python includes leading / - we don't
                        VerifyTestResult (url.GetQueryString () == L"");
                        VerifyTestResult (url.GetFragment () == L"");
                        VerifyTestResult (url.GetFullURL () == L"http://www.cwi.nl/%7Eguido/Python.html");
                    }
                }
                {
                    URL url{URL::Parse (L"//www.cwi.nl:8080/%7Eguido/Python.html", URL::eFlexiblyAsUI)};
                    VerifyTestResult (url.GetHost () == L"www.cwi.nl");
                    VerifyTestResult (url.GetPortValue () == 8080);
                    VerifyTestResult (url.GetHostRelativePath () == L"%7Eguido/Python.html");
                    VerifyTestResult (url.GetQueryString () == L"");
                    VerifyTestResult (url.GetFragment () == L"");
                    VerifyTestResult (url.GetFullURL () == L"http://www.cwi.nl:8080/%7Eguido/Python.html");
                }
                {
                    URL url{URL::Parse (L"https://www.cwi.nl/%7Eguido/Python.html", URL::eFlexiblyAsUI)};
                    VerifyTestResult (url.GetScheme () == L"https");
                    VerifyTestResult (url.GetHost () == L"www.cwi.nl");
                    VerifyTestResult (url.GetPortValue () == 443);
                    VerifyTestResult (url.GetHostRelativePath () == L"%7Eguido/Python.html");
                    VerifyTestResult (url.GetQueryString () == L"");
                    VerifyTestResult (url.GetFragment () == L"");
                    VerifyTestResult (url.GetFullURL () == L"https://www.cwi.nl/%7Eguido/Python.html");
                }
                {
                    URL url = URL::Parse (L"Start.htm", URL::eAsRelativeURL);
                    VerifyTestResult (url.GetHost ().empty ());
                    VerifyTestResult (url.GetHostRelativePath () == L"Start.htm");
                    VerifyTestResult (url.GetFragment ().empty ());
                }
                {
                    URL url = URL::Parse (L"fred/Start.htm", URL::eAsRelativeURL);
                    VerifyTestResult (url.GetHost ().empty ());
                    VerifyTestResult (url.GetHostRelativePath () == L"fred/Start.htm");
                    VerifyTestResult (url.GetFragment ().empty ());
                }
                if (false) {
                    // https://stroika.atlassian.net/browse/STK-502
                    URL url = URL::Parse (L"123.1.2.3:8080", URL::eFlexiblyAsUI);
                    VerifyTestResult (url.GetHost () == L"123.1.2.3");
                    VerifyTestResult (url.GetPortValue () == 8080);
                    VerifyTestResult (url.GetHostRelativePath ().empty ());
                    VerifyTestResult (url.GetFragment ().empty ());
                }
            }
            void TestHostParsing_ ()
            {
                using UniformResourceIdentification::Host;
                VerifyTestResult ((Host{Network::V4::kLocalhost}.AsEncoded () == L"127.0.0.1"sv));
                VerifyTestResult ((Host{InternetAddress{169, 254, 0, 1}}.AsEncoded () == L"169.254.0.1"sv));
                VerifyTestResult ((Host{InternetAddress{L"fe80::44de:4247:5b76:ddc9"}}.AsEncoded () == L"[fe80::44de:4247:5b76:ddc9]"sv));
                VerifyTestResult ((Host::Parse (L"[fe80::44de:4247:5b76:ddc9]").AsInternetAddress () == InternetAddress{L"fe80::44de:4247:5b76:ddc9"}));
                VerifyTestResult ((Host{L"www.sophists.com"}.AsEncoded () == L"www.sophists.com"sv));
                VerifyTestResult ((Host{L"hello mom"}.AsEncoded () == L"hello%20mom"sv));
                VerifyTestResult ((Host::Parse (L"hello%20mom") == Host{L"hello mom"}));
                {
                    // negative tests - must throw
                    try {
                        Host::Parse (L"%%%");
                        VerifyTestResult (false); // must throw
                    }
                    catch (const runtime_error&) {
                    }
                    try {
                        Host::Parse (L"%a");
                        VerifyTestResult (false); // must throw
                    }
                    catch (const runtime_error&) {
                    }
                    try {
                        Host::Parse (L"%ag");
                        VerifyTestResult (false); // must throw
                    }
                    catch (const runtime_error&) {
                    }
                }
            }
            void Test_NewURI_Class_ ()
            {
                {
                    IO::Network::URI uri = IO::Network::URI::Parse (L"http://localhost:1234");
                    VerifyTestResult (uri.GetAuthority ()->GetHost ()->AsRegisteredName () == L"localhost");
                    VerifyTestResult (uri.GetAuthority ()->GetPort () == 1234);
                    VerifyTestResult (uri.As<String> () == L"http://localhost:1234");
                }
                {
                    IO::Network::URI uri = IO::Network::URI::Parse (L"localhost:1234");
                    VerifyTestResult (not uri.GetAuthority ().has_value ()); // treated as a scheme
                    VerifyTestResult (uri.As<String> () == L"localhost:1234");
                }
                {
                    IO::Network::URI uri = IO::Network::URI::Parse (L"http://www.ics.uci.edu/pub/ietf/uri/#Related");
                    VerifyTestResult (uri.GetAuthority ()->GetHost ()->AsRegisteredName () == L"www.ics.uci.edu");
                    VerifyTestResult (uri.As<String> () == L"http://www.ics.uci.edu/pub/ietf/uri/#Related");
                }
                {
                    IO::Network::URI uri = IO::Network::URI::Parse (L"/uri/#Related");
                    VerifyTestResult (not uri.GetAuthority ().has_value ());
                    VerifyTestResult (uri.As<String> () == L"/uri/#Related");
                }
                {
                    // This behavior appears to meet the needs of my URL::eStroikaPre20a50BackCompatMode tests - so may work for Stroika - just replace its use with URI -- LGP 2019-04-04
                    // AT LEAST CLOSE - I THINK I CAN COME UP WITH CHEATSHEET TO MAP NAMES (like GetRElPath to GetPath () - but does it handle leading slash same?)
                    IO::Network::URI uri = IO::Network::URI::Parse (L"dyn:/StyleSheet.css?ThemeName=Cupertino");
                    VerifyTestResult (*uri.GetScheme () == L"dyn");
                    VerifyTestResult (not uri.GetAuthority ().has_value ());
                    VerifyTestResult (uri.GetPath () == L"/StyleSheet.css");
                    VerifyTestResult (uri.GetQuery<String> () == L"ThemeName=Cupertino");
                    VerifyTestResult (uri.GetQuery ()->operator() (L"ThemeName") == L"Cupertino");
                }
                {
                    IO::Network::URI uri = IO::Network::URI::Parse (L"HTTPS://www.MICROSOFT.com/Path");
                    VerifyTestResult (uri.Normalize ().As<String> () == L"https://www.microsoft.com/Path");
                }
            }
        }
        void DoTests_ ()
        {
            Private_::TestBackCompatURL_ ();
            Private_::BasicTests_AsOf21d22_ ();
            Private_::TestHostParsing_ ();
            Private_::Test_NewURI_Class_ ();
        }
    }
}

namespace {
    void Test2_InternetAddress_ ()
    {
        Debug::TraceContextBumper trcCtx ("Test2_InternetAddress_");
        {
            VerifyTestResult ((InternetAddress{169, 254, 0, 1}).As<String> () == L"169.254.0.1");
            VerifyTestResult ((InternetAddress{1, 2, 3, 4}).As<String> () == L"1.2.3.4");
            VerifyTestResult ((InternetAddress{L"1.2.3.4"}).As<String> () == L"1.2.3.4");
            VerifyTestResult ((InternetAddress{"1.2.3.4"}).As<String> () == L"1.2.3.4");
        }
        {
            VerifyTestResult ((InternetAddress{1, 2, 3, 4}.As<array<uint8_t, 4>> ()[0] == 1));
            VerifyTestResult ((InternetAddress{1, 2, 3, 4}.As<array<uint8_t, 4>> ()[2] == 3));
        }
        {
            auto testRoundtrip = [] (const String& s) {
                InternetAddress iaddr1{s};
                InternetAddress iaddr2{s.As<wstring> ()};
                InternetAddress iaddr3{s.AsASCII ()};
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
            struct Tester {
                InternetAddress addr;
                bool            isLocalHost;
                bool            isLinkLocal;
                bool            isMulticast;
                bool            isPrivate;
            };
            const InternetAddress kSamplePrivateAddr_{"192.168.244.121"};
            const InternetAddress kSSDPAddr_{239, 255, 255, 250};
            const InternetAddress kSomeIPV4LinkLocalAddr_{"169.254.0.1"};
            const InternetAddress kSomeIPV6LinkLocalAddr_{"fe80::44de:4247:5b76:ddc9"};
            const Tester          kTests_[] = {
                //  ADDR                                    localhost   linklocal   multicast   privateaddr
                {V4::kAddrAny, false, false, false, false},
                {V6::kAddrAny, false, false, false, false},
                {V4::kLocalhost, true, false, false, false},
                {V6::kLocalhost, true, false, false, false},
                {kSSDPAddr_, false, false, true, false},
                {kSomeIPV4LinkLocalAddr_, false, true, false, false},
                {kSomeIPV6LinkLocalAddr_, false, true, false, false},
                {kSamplePrivateAddr_, false, false, false, true},
                {InternetAddress{10, 0, 0, 0}, false, false, false, true},
                {InternetAddress{10, 255, 255, 255}, false, false, false, true},
                {InternetAddress{172, 16, 0, 1}, false, false, false, true},
                {InternetAddress{172, 16, 0, 3}, false, false, false, true},
                {InternetAddress{172, 31, 255, 255}, false, false, false, true},
                {InternetAddress{192, 168, 0, 0}, false, false, false, true},
                {InternetAddress{192, 168, 255, 255}, false, false, false, true},
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
            VerifyTestResult (InternetAddress (V4::kLocalhost.As<in_addr> (InternetAddress::ByteOrder::Host)) != V4::kLocalhost or ntohl (0x01020304) == 0x01020304); // if big-endian machine, net byte order equals host byte order
        }
        {
            VerifyTestResult (InternetAddress{L"192.168.99.1"}.AsAddressFamily (InternetAddress::AddressFamily::V6) == InternetAddress{L"2002:C0A8:6301::"});
            VerifyTestResult (InternetAddress{L"2002:C0A8:6301::"}.AsAddressFamily (InternetAddress::AddressFamily::V4) == InternetAddress{L"192.168.99.1"});
        }
    }
}

namespace {
    void Test3_NetworkInterfaceList_ ()
    {
        Debug::TraceContextBumper trcCtx ("Test3_NetworkInterfaceList_");
        for (Interface iFace : Network::GetInterfaces ()) {
            Debug::TraceContextBumper trcCtx1 ("iface");
            DbgTrace (L"fInternalInterfaceID: %s", iFace.fInternalInterfaceID.c_str ());
#if qPlatform_POSIX
            DbgTrace (L"InterfaceName: %s", iFace.GetInterfaceName ().c_str ());
#endif
            DbgTrace (L"Friendly-name: %s", iFace.fFriendlyName.c_str ());
            if (iFace.fDescription.has_value ()) {
                DbgTrace (L"description: %s", iFace.fDescription->c_str ());
            }
            if (iFace.fType.has_value ()) {
                DbgTrace (L"type: %s", Configuration::DefaultNames<Interface::Type>::k.GetName (*iFace.fType));
            }
            for (auto binding : iFace.fBindings) {
                DbgTrace (L"binding-addr: %s", Characters::ToString (binding).c_str ());
            }
            if (iFace.fStatus.has_value ()) {
                for (Interface::Status s : *iFace.fStatus) {
                    DbgTrace (L"status: %s", Configuration::DefaultNames<Interface::Status>::k.GetName (s));
                }
            }
        }
    }
}

namespace {
    namespace Test4_DNS_ {
        void DoTests_ ()
        {
            {
                DNS::HostEntry e = DNS::Default ().GetHostEntry (L"www.sophists.com");
                VerifyTestResult (e.fCanonicalName.Contains (L".sophists.com"));
                VerifyTestResult (e.fAddressList.size () >= 1);
            }
            {
                DNS::HostEntry e = DNS::Default ().GetHostEntry (L"www.google.com");
                VerifyTestResult (e.fAddressList.size () >= 1);
            }
            {
                DNS::HostEntry e = DNS::Default ().GetHostEntry (L"www.cnn.com");
                VerifyTestResult (e.fAddressList.size () >= 1);
            }
            {
                Memory::Optional<String> aaa = DNS::Default ().ReverseLookup (InternetAddress (23, 56, 90, 167));
                DbgTrace (L"reverselookup %s", ValueOrDefault (aaa).c_str ());
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Test1_URL_Parsing_::DoTests_ ();
        Test2_InternetAddress_ ();
        Test3_NetworkInterfaceList_ ();
        Test4_DNS_::DoTests_ ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
