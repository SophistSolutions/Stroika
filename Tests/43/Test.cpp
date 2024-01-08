/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::IO::Network
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/IO/Network/CIDR.h"
#include "Stroika/Foundation/IO/Network/DNS.h"
#include "Stroika/Foundation/IO/Network/Interface.h"
#include "Stroika/Foundation/IO/Network/Neighbors.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;

#if qHasFeature_GoogleTest
namespace {
    namespace Test1_URI_ {
        namespace Private_ {
            void TestHostParsing_ ()
            {
                Debug::TraceContextBumper ctx{"Test1_URI_::Private_::TestHostParsing_"};
                using UniformResourceIdentification::Host;
                EXPECT_TRUE ((Host{Network::V4::kLocalhost}.AsEncoded () == L"127.0.0.1"sv));
                EXPECT_TRUE ((Host{InternetAddress{169, 254, 0, 1}}.AsEncoded () == L"169.254.0.1"sv));
                EXPECT_TRUE ((Host{InternetAddress{L"fe80::44de:4247:5b76:ddc9"}}.AsEncoded () == "[fe80::44de:4247:5b76:ddc9]"sv));
                EXPECT_TRUE ((Host::Parse ("[fe80::44de:4247:5b76:ddc9]"sv).AsInternetAddress () == InternetAddress{"fe80::44de:4247:5b76:ddc9"sv}));
                EXPECT_TRUE ((Host{"www.sophists.com"}.AsEncoded () == "www.sophists.com"sv));
                EXPECT_TRUE ((Host{"hello mom"}.AsEncoded () == L"hello%20mom"sv));
                EXPECT_TRUE ((Host::Parse ("hello%20mom") == Host{"hello mom"}));
                {
                    // negative tests - must throw
                    try {
                        (void)Host::Parse ("%%%");
                        EXPECT_TRUE (false); // must throw
                    }
                    catch (const runtime_error&) {
                    }
                    try {
                        (void)Host::Parse (L"%a");
                        EXPECT_TRUE (false); // must throw
                    }
                    catch (const runtime_error&) {
                    }
                    try {
                        (void)Host::Parse (L"%ag");
                        EXPECT_TRUE (false); // must throw
                    }
                    catch (const runtime_error&) {
                    }
                }
            }
            void SimpleURITests_ ()
            {
                using IO::Network::UniformResourceIdentification::SchemeType;
                Debug::TraceContextBumper ctx{"Test1_URI_::Private_::SimpleURITests_"};
                {
                    IO::Network::URI uri = IO::Network::URI::Parse (L"http://localhost:1234");
                    EXPECT_TRUE (uri.GetAuthority ()->GetHost ()->AsRegisteredName () == "localhost");
                    EXPECT_TRUE (uri.GetAuthority ()->GetPort () == 1234);
                    EXPECT_TRUE (uri.As<String> () == "http://localhost:1234");
                }
                {
                    IO::Network::URI uri = IO::Network::URI::Parse ("localhost:1234");
                    EXPECT_TRUE (not uri.GetAuthority ().has_value ()); // treated as a scheme
                    EXPECT_TRUE (uri.As<String> () == "localhost:1234");
                }
                {
                    IO::Network::URI uri = IO::Network::URI::Parse ("http://www.ics.uci.edu/pub/ietf/uri/#Related");
                    EXPECT_TRUE (uri.GetAuthority ()->GetHost ()->AsRegisteredName () == "www.ics.uci.edu");
                    DbgTrace (L"X=%s", uri.As<String> ().c_str ());
                    EXPECT_TRUE (uri.As<String> () == "http://www.ics.uci.edu/pub/ietf/uri/#Related");
                }
                {
                    IO::Network::URI uri = IO::Network::URI::Parse ("/uri/#Related");
                    EXPECT_TRUE (not uri.GetAuthority ().has_value ());
                    EXPECT_TRUE (uri.As<String> () == "/uri/#Related");
                }
                {
                    // This behavior appears to meet the needs of my URL::eStroikaPre20a50BackCompatMode tests - so may work for Stroika - just replace its use with URI -- LGP 2019-04-04
                    // AT LEAST CLOSE - I THINK I CAN COME UP WITH CHEATSHEET TO MAP NAMES (like GetRElPath to GetPath () - but does it handle leading slash same?)
                    IO::Network::URI uri = IO::Network::URI::Parse ("dyn:/StyleSheet.css?ThemeName=Cupertino");
                    EXPECT_TRUE (*uri.GetScheme () == SchemeType{"dyn"sv});
                    EXPECT_TRUE (not uri.GetAuthority ().has_value ());
                    EXPECT_TRUE (uri.GetPath () == "/StyleSheet.css");
                    EXPECT_TRUE (uri.GetQuery<String> () == "ThemeName=Cupertino");
                    EXPECT_TRUE (uri.GetQuery ()->operator() (L"ThemeName") == "Cupertino");
                }
                {
                    IO::Network::URI uri = IO::Network::URI::Parse ("HTTPS://www.MICROSOFT.com/Path");
                    EXPECT_TRUE (uri.Normalize ().As<String> () == "https://www.microsoft.com/Path");
                }
                {
                    URI base{"http://www.sophists.com"};
                    EXPECT_TRUE (base.Combine (URI{"/blag/foo.icon"}).As<String> () == "http://www.sophists.com/blag/foo.icon");
                }
                {
                    URI base{"http://www.sophists.com/"};
                    EXPECT_TRUE (base.Combine (URI{"/blag/foo.icon"}).As<String> () == "http://www.sophists.com/blag/foo.icon");
                }
            }
            void Test_Reference_Resolution_Examples_From_RFC_3986_ ()
            {
                Debug::TraceContextBumper ctx{"Test1_URI_::Private_::Test_Reference_Resolution_Examples_From_RFC_3986_"};
                // tests from https://tools.ietf.org/html/rfc3986#section-5.4
                URI base = URI::Parse (L"http://a/b/c/d;p?q");

                // https://tools.ietf.org/html/rfc3986#section-5.4.1
                EXPECT_TRUE (base.Combine (URI::Parse ("g:h")).As<String> () == L"g:h");
                EXPECT_TRUE (base.Combine (URI::Parse ("g")).As<String> () == L"http://a/b/c/g");
                EXPECT_TRUE (base.Combine (URI::Parse ("./g")).As<String> () == L"http://a/b/c/g");
                EXPECT_TRUE (base.Combine (URI::Parse ("/g")).As<String> () == L"http://a/g");
                EXPECT_TRUE (base.Combine (URI::Parse ("//g")).As<String> () == L"http://g");
                EXPECT_TRUE (base.Combine (URI::Parse ("?y")).As<String> () == L"http://a/b/c/d;p?y");
                EXPECT_TRUE (base.Combine (URI::Parse ("g?y")).As<String> () == L"http://a/b/c/g?y");
                EXPECT_TRUE (base.Combine (URI::Parse ("#s")).As<String> () == L"http://a/b/c/d;p?q#s");
                EXPECT_TRUE (base.Combine (URI::Parse ("g#s")).As<String> () == L"http://a/b/c/g#s");
                EXPECT_TRUE (base.Combine (URI::Parse ("g?y#s")).As<String> () == L"http://a/b/c/g?y#s");
                EXPECT_TRUE (base.Combine (URI::Parse (";x")).As<String> () == L"http://a/b/c/;x");
                EXPECT_TRUE (base.Combine (URI::Parse ("g;x")).As<String> () == L"http://a/b/c/g;x");
                EXPECT_TRUE (base.Combine (URI::Parse ("")).As<String> () == L"http://a/b/c/d;p?q");
                EXPECT_TRUE (base.Combine (URI::Parse (".")).As<String> () == L"http://a/b/c/");
                EXPECT_TRUE (base.Combine (URI::Parse ("./")).As<String> () == L"http://a/b/c/");
                EXPECT_TRUE (base.Combine (URI::Parse ("..")).As<String> () == L"http://a/b/");
                EXPECT_TRUE (base.Combine (URI::Parse ("../")).As<String> () == L"http://a/b/");
                EXPECT_TRUE (base.Combine (URI::Parse ("../g")).As<String> () == L"http://a/b/g");
                EXPECT_TRUE (base.Combine (URI::Parse ("../..")).As<String> () == L"http://a/");
                EXPECT_TRUE (base.Combine (URI::Parse ("../../")).As<String> () == L"http://a/");
                EXPECT_TRUE (base.Combine (URI::Parse ("../../g")).As<String> () == L"http://a/g");

                // https://tools.ietf.org/html/rfc3986#section-5.4.2 Abnormal Examples
                EXPECT_TRUE (base.Combine (URI::Parse ("../../../g")).As<String> () == L"http://a/g");
                EXPECT_TRUE (base.Combine (URI::Parse ("../../../../g")).As<String> () == L"http://a/g");
                EXPECT_TRUE (base.Combine (URI::Parse ("/./g")).As<String> () == L"http://a/g");
                EXPECT_TRUE (base.Combine (URI::Parse ("/../g")).As<String> () == L"http://a/g");
                EXPECT_TRUE (base.Combine (URI::Parse ("g.")).As<String> () == L"http://a/b/c/g.");
                EXPECT_TRUE (base.Combine (URI::Parse (".g")).As<String> () == L"http://a/b/c/.g");
                EXPECT_TRUE (base.Combine (URI::Parse ("g..")).As<String> () == L"http://a/b/c/g..");
                EXPECT_TRUE (base.Combine (URI::Parse ("..g")).As<String> () == L"http://a/b/c/..g");
                EXPECT_TRUE (base.Combine (URI::Parse ("./../g")).As<String> () == L"http://a/b/g");
                EXPECT_TRUE (base.Combine (URI::Parse ("./g/.")).As<String> () == L"http://a/b/c/g/");
                EXPECT_TRUE (base.Combine (URI::Parse ("g/./h")).As<String> () == L"http://a/b/c/g/h");
                EXPECT_TRUE (base.Combine (URI::Parse ("g/../h")).As<String> () == L"http://a/b/c/h");
                EXPECT_TRUE (base.Combine (URI::Parse ("g;x=1/./y")).As<String> () == L"http://a/b/c/g;x=1/y");
                EXPECT_TRUE (base.Combine (URI::Parse ("g;x=1/../y")).As<String> () == L"http://a/b/c/y");
                EXPECT_TRUE (base.Combine (URI::Parse ("g?y/./x")).As<String> () == L"http://a/b/c/g?y/./x");
                EXPECT_TRUE (base.Combine (URI::Parse ("g?y/../x")).As<String> () == L"http://a/b/c/g?y/../x");
                EXPECT_TRUE (base.Combine (URI::Parse ("g#s/./x")).As<String> () == L"http://a/b/c/g#s/./x");
                EXPECT_TRUE (base.Combine (URI::Parse ("g#s/../x")).As<String> () == L"http://a/b/c/g#s/../x");
                EXPECT_TRUE (base.Combine (URI::Parse ("http:g")).As<String> () == L"http:g"); // strict interpretation "for strict parsers"
            }
            void TestEmptyURI_ ()
            {
                Debug::TraceContextBumper ctx{"TestEmptyURI_"};
                {
                    URI u{};
                    EXPECT_TRUE (u.As<String> () == "");
                }
                {
                    URI u{""};
                    EXPECT_TRUE (u.As<String> () == "");
                }
                {
                    URI u{" "};
                    EXPECT_TRUE (u.As<String> () == "%20"); // @todo REVIEW SPEC- urlparse(' ').geturl () produces space, but I think this makes more sense
                    EXPECT_TRUE (u.GetPath () == " ");
                }
            }
            void TestSamplesFromPythonURLParseDocs_ ()
            {
                Debug::TraceContextBumper ctx{"TestSamplesFromPythonURLParseDocs_"};
                using namespace IO::Network::UniformResourceIdentification;
                {
                    /*
                     *  From https://docs.python.org/2/library/urlparse.html
                     *      from urlparse import urlparse
                     *      o = urlparse('http://www.cwi.nl:80/%7Eguido/Python.html')
                     *      o.scheme
                     *      o.port
                     *      o.geturl ()
                     */
                    auto o = URI{"http://www.cwi.nl:80/%7Eguido/Python.html"};
                    EXPECT_TRUE (o.GetScheme () == SchemeType{"http"});
                    EXPECT_TRUE (o.GetAuthority ()->GetPort () == 80);
                    // NOTE - here python emits '%7Eguido' instead of '~guido'. However, according to
                    // https://tools.ietf.org/html/rfc3986#section-2.3, '~' - %7E - is an unreserved character, and
                    // so Stroika does NOT encode it.
                    EXPECT_TRUE (o.As<string> () == "http://www.cwi.nl:80/~guido/Python.html");
                }
                {
                    /*
                     *  From https://docs.python.org/2/library/urlparse.html
                     *      from urlparse import urlparse
                     *      o = urlparse('//www.cwi.nl:80/%7Eguido/Python.html')
                     *      o.scheme
                     *      o.netloc
                     *      o.path
                     *      o.query
                     *      o.fragment
                     */
                    auto o = URI{"//www.cwi.nl:80/%7Eguido/Python.html"};
                    EXPECT_TRUE (o.GetScheme () == nullopt);
                    EXPECT_TRUE (o.GetAuthority () == Authority::Parse ("www.cwi.nl:80"));
                    EXPECT_TRUE (o.GetPath () == L"/~guido/Python.html");
                    EXPECT_TRUE (o.GetQuery () == nullopt);
                    EXPECT_TRUE (o.GetFragment () == nullopt);
                    EXPECT_TRUE (o.As<string> () == "//www.cwi.nl:80/~guido/Python.html");
                }
                {
                    /*
                     *  From https://docs.python.org/2/library/urlparse.html
                     *      from urlparse import urlparse
                     *      o = urlparse('www.cwi.nl/%7Eguido/Python.html')
                     *      o.scheme
                     *      o.netloc
                     *      o.path
                     *      o.query
                     *      o.fragment
                     */
                    auto o = URI{"www.cwi.nl/%7Eguido/Python.html"};
                    EXPECT_TRUE (o.GetScheme () == nullopt);
                    EXPECT_TRUE (o.GetAuthority () == nullopt);
                    EXPECT_TRUE (o.GetPath () == L"www.cwi.nl/~guido/Python.html"); // again, differ from python because our getPath returns decoded string
                    EXPECT_TRUE (o.GetQuery () == nullopt);
                    EXPECT_TRUE (o.GetFragment () == nullopt);
                    EXPECT_TRUE (o.As<string> () == "www.cwi.nl/~guido/Python.html"); // again, differ from python because %7E (~) is unreserved character
                }
                {
                    /*
                     *  From https://docs.python.org/2/library/urlparse.html
                     *      from urlparse import urlparse
                     *      o = urlparse('help/Python.html')
                     *      o.scheme
                     *      o.netloc
                     *      o.path
                     *      o.query
                     *      o.fragment
                     */
                    auto o = URI{"help/Python.html"};
                    EXPECT_TRUE (o.GetScheme () == nullopt);
                    EXPECT_TRUE (o.GetAuthority () == nullopt);
                    EXPECT_TRUE (o.GetPath () == L"help/Python.html");
                    EXPECT_TRUE (o.GetQuery () == nullopt);
                    EXPECT_TRUE (o.GetFragment () == nullopt);
                    EXPECT_TRUE (o.As<string> () == "help/Python.html");
                }
                {
                    /*
                     *  From https://docs.python.org/2/library/urlparse.html
                     *      from urlparse import urljoin
                     *      urljoin('http://www.cwi.nl/%7Eguido/Python.html', 'FAQ.html')
                     */
                    auto o = URI{"http://www.cwi.nl/%7Eguido/Python.html"}.Combine (URI{"FAQ.html"});
                    EXPECT_TRUE (o.As<string> () == "http://www.cwi.nl/~guido/FAQ.html"); // again, differ from python because %7E (~) is unreserved character
                }
                {
                    /*
                     *  From https://docs.python.org/2/library/urlparse.html
                     *      from urlparse import urljoin
                     *      urljoin('http://www.cwi.nl/%7Eguido/Python.html', '//www.python.org/%7Eguido')
                     */
                    auto o = URI{"http://www.cwi.nl/%7Eguido/Python.html"}.Combine (URI{"//www.python.org/%7Eguido"});
                    EXPECT_TRUE (o.As<string> () == "http://www.python.org/~guido"); // again, differ from python because %7E (~) is unreserved character
                }
            }
            void Test_PatternUsedInHealthFrame_ ()
            {
                Debug::TraceContextBumper ctx{"Test_PatternUsedInHealthFrame_"};
                using namespace IO::Network::UniformResourceIdentification;
                {
                    auto o = URI{"dyn:/Reminders/Home.htm"};
                    EXPECT_TRUE (o.GetScheme () == SchemeType{L"dyn"});
                    EXPECT_TRUE (o.GetPath () == L"/Reminders/Home.htm");
                }
                {
                    auto o = URI{"dyn:/StyleSheet.css?ThemeName=Cupertino"};
                    EXPECT_TRUE (o.GetScheme () == SchemeType{L"dyn"});
                    EXPECT_TRUE (o.GetPath () == L"/StyleSheet.css");
                    EXPECT_TRUE ((*o.GetQuery<Query> ()) (L"ThemeName") == L"Cupertino");
                }
            }
            void Test_RegressionDueToBugInCompareURIsC20Spaceship_ ()
            {
                Debug::TraceContextBumper ctx{"Test_RegressionDueToBugInCompareURIsC20Spaceship_"};
                URI                       fred = URI{"http://abc.com/bar"};
                if (fred) {
                    // make sure operator bool working
                }
                else {
                    EXPECT_TRUE (false);
                }
// workaround really only needed if ASAN enabled, but more of a PITA to test that
#if qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy
                static const auto kInitList_ =
                    initializer_list<URI>{URI{"http://httpbin.org/get"}, URI{"http://www.google.com"}, fred, URI{"http://www.cnn.com"}};
#endif
                for (URI u :
#if qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy
                     kInitList_
#else
                     {URI{"http://httpbin.org/get"}, URI{"http://www.google.com"}, fred, URI{"http://www.cnn.com"}}
#endif
                ) {
                    auto schemeAndAuthority = fred.GetSchemeAndAuthority ();
                    URI  fURL_              = u;
                    URI  newURL             = u;
                    newURL.SetScheme (schemeAndAuthority.GetScheme ());
                    newURL.SetAuthority (schemeAndAuthority.GetAuthority ());
                    auto b1 = (fURL_ == newURL);
                    auto b2 = (fURL_ < newURL);
                    auto b3 = (fURL_ > newURL);
                    auto b4 = (fURL_ != newURL);
                    EXPECT_TRUE (b1 != b4);
                    EXPECT_TRUE (b2 != b3 or b1);
                }
            }
            void Test_UPNPBadURIIPv6_ ()
            {
                // Test case for fix: https://stroika.atlassian.net/browse/STK-957
                {
                    using namespace IO::Network::UniformResourceIdentification;
                    // Sub-problem where original bug lies
                    try {
                        optional<Authority> authority = Authority::Parse ("[fe80::354f:9016:fed2:8b9b]:2869"sv);
                        EXPECT_TRUE (authority->GetPort () == 2869);
                        EXPECT_TRUE (authority->GetHost () == InternetAddress{"fe80::354f:9016:fed2:8b9b"sv});
                    }
                    catch (...) {
                        EXPECT_TRUE (false); // not reached - valid parse
                    }
                }
                try {
                    auto uri = URI::Parse ("http://[fe80::354f:9016:fed2:8b9b]:2869/upnphost/udhisapi.dll?content=uuid:4becec11-428e-46e0-801b-9b293cf1d2c7"sv);
                    EXPECT_TRUE (uri.GetAuthority ()->GetPort () == 2869);
                    EXPECT_TRUE (uri.GetAuthority ()->GetHost () == InternetAddress{"fe80::354f:9016:fed2:8b9b"sv});
                    EXPECT_TRUE (uri.GetAbsPath () == "/upnphost/udhisapi.dll"sv);
                }
                catch (...) {
                    EXPECT_TRUE (false); // not reached - valid parse
                }
                try {
                    auto uri = URI::Parse ("http://[fe80::354f:9016:fed2:8b9b"sv);
                    EXPECT_TRUE (false); // not reached - invalid URL
                }
                catch (...) {
                    // GOOD - sb exception
                }
            }
            void Test_SetScheme_ ()
            {
                URI u;
                u.SetScheme (URI::SchemeType{L"http"});
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper ctx{"Test1_URI_::DoTests"};
            Private_::TestHostParsing_ ();
            Private_::SimpleURITests_ ();
            Private_::Test_Reference_Resolution_Examples_From_RFC_3986_ ();
            Private_::TestEmptyURI_ ();
            Private_::TestSamplesFromPythonURLParseDocs_ ();
            Private_::Test_PatternUsedInHealthFrame_ ();
            Private_::Test_RegressionDueToBugInCompareURIsC20Spaceship_ ();
            Private_::Test_UPNPBadURIIPv6_ ();
            Private_::Test_SetScheme_ ();
        }
    }
}

namespace {
    void Test2_InternetAddress_ ()
    {
        Debug::TraceContextBumper trcCtx{"Test2_InternetAddress_"};
        {
            EXPECT_TRUE ((InternetAddress{169, 254, 0, 1}).As<String> () == L"169.254.0.1");
            EXPECT_TRUE ((InternetAddress{1, 2, 3, 4}).As<String> () == L"1.2.3.4");
            EXPECT_TRUE ((InternetAddress{L"1.2.3.4"}).As<String> () == L"1.2.3.4");
            EXPECT_TRUE ((InternetAddress{"1.2.3.4"}).As<String> () == L"1.2.3.4");
        }
        {
            EXPECT_TRUE ((InternetAddress{1, 2, 3, 4}.As<array<uint8_t, 4>> ()[0] == 1));
            EXPECT_TRUE ((InternetAddress{1, 2, 3, 4}.As<array<uint8_t, 4>> ()[2] == 3));
        }
        {
            auto testRoundtrip = [] (const String& s) {
                InternetAddress iaddr1{s};
                InternetAddress iaddr2{s.As<wstring> ()};
                InternetAddress iaddr3{s.AsASCII ()};
                EXPECT_TRUE (iaddr1 == iaddr2);
                EXPECT_TRUE (iaddr2 == iaddr3);
                EXPECT_TRUE (iaddr1.As<String> () == s);
                EXPECT_TRUE (iaddr2.As<String> () == s);
                EXPECT_TRUE (iaddr3.As<String> () == s);
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
                EXPECT_TRUE (i.addr.IsLocalhostAddress () == i.isLocalHost);
                EXPECT_TRUE (i.addr.IsLinkLocalAddress () == i.isLinkLocal);
                EXPECT_TRUE (i.addr.IsMulticastAddress () == i.isMulticast);
                EXPECT_TRUE (i.addr.IsPrivateAddress () == i.isPrivate);
            }
        }
        {
            EXPECT_TRUE (InternetAddress (V4::kLocalhost.As<in_addr> ()) == V4::kLocalhost);
            EXPECT_TRUE (InternetAddress (V4::kLocalhost.As<in_addr> (InternetAddress::ByteOrder::Host)) != V4::kLocalhost or
                         ntohl (0x01020304) == 0x01020304); // if big-endian machine, net byte order equals host byte order
        }
        {
            EXPECT_TRUE (InternetAddress{"192.168.99.1"}.AsAddressFamily (InternetAddress::AddressFamily::V6) ==
                         InternetAddress{"2002:C0A8:6301::"});
            EXPECT_TRUE (InternetAddress{"2002:C0A8:6301::"}.AsAddressFamily (InternetAddress::AddressFamily::V4) ==
                         InternetAddress{"192.168.99.1"});
        }
    }
}

namespace {
    void Test3_NetworkInterfaceList_ ()
    {
        Debug::TraceContextBumper trcCtx{"Test3_NetworkInterfaceList_"};
        for (Interface iFace : SystemInterfacesMgr{}.GetAll ()) {
            DbgTrace (L"iFace: %s", Characters::ToString (iFace).c_str ());
        }
    }
}

namespace {
    namespace Test4_DNS_ {
        void DoTests_ ()
        {
            Debug::TraceContextBumper ctx{"Test4_DNS_::DoTests_"};
            {
                DNS::HostEntry e = DNS::kThe.GetHostEntry ("www.sophists.com");
                EXPECT_TRUE (e.fCanonicalName.Contains (".sophists.com"));
                EXPECT_TRUE (e.fAddressList.size () >= 1);
            }
            {
                DNS::HostEntry e = DNS::kThe.GetHostEntry ("www.google.com");
                EXPECT_TRUE (e.fAddressList.size () >= 1);
            }
            {
                DNS::HostEntry e = DNS::kThe.GetHostEntry ("www.cnn.com");
                EXPECT_TRUE (e.fAddressList.size () >= 1);
            }
            {
                optional<String> aaa = DNS::kThe.ReverseLookup (InternetAddress{23, 56, 90, 167});
                DbgTrace (L"reverselookup %s", Memory::NullCoalesce (aaa).c_str ());
            }
        }
    }
}

namespace {
    namespace Test5_CIDR_ {
        void DoTests_ ()
        {
            Debug::TraceContextBumper ctx{"Test5_CIDR_::DoTests_"};
            {
                CIDR cidr{"10.70.0.0/15"};
                auto cidr2 = CIDR{InternetAddress{"10.70.0.0"}, 15};
                EXPECT_TRUE (cidr == cidr2);
                EXPECT_TRUE (Characters::ToString (cidr) == "10.70.0.0/15");
                EXPECT_TRUE (cidr.GetNumberOfSignificantBits () == 15);
                EXPECT_TRUE ((cidr.GetRange () ==
                              Traversal::DiscreteRange<InternetAddress>{InternetAddress{10, 70, 0, 0}, InternetAddress{10, 71, 255, 255}}));
            }
            {
                auto cidr = CIDR{InternetAddress{"192.168.56.1"}, 24};
                EXPECT_TRUE (Characters::ToString (cidr) == "192.168.56.0/24");
            }
            {
                auto cidr = CIDR{InternetAddress{"172.28.240.1"}, 20};
                EXPECT_TRUE (Characters::ToString (cidr) == "172.28.240.0/20");
            }
            {
                auto cidr = CIDR{InternetAddress{"172.17.185.1"}, 28};
                EXPECT_TRUE (Characters::ToString (cidr) == "172.17.185.0/28");
            }
            {
                // fix for https://stroika.atlassian.net/browse/STK-909
                auto cidr = CIDR{V6::kAddrAny, 64};
                EXPECT_TRUE (Characters::ToString (cidr) == "in6addr_any/64");
                EXPECT_TRUE (CIDR{cidr.As<String> ()} == cidr); // can roundtrip numeric form
            }
        }
    }
}

namespace {
    namespace Test6_Neighbors_ {
        void DoTests_ ()
        {
            Debug::TraceContextBumper ctx{"Test6_Neighbors_::DoTests_"};
            {
                IO::Network::NeighborsMonitor monitor;
                Time::TimePointSeconds        timeoutAt = (Time::GetTickCount () + Time::Duration{3s});
                while (Time::GetTickCount () < timeoutAt) {
                    // Note - this try/catch is ONLY needed to workaround bug with MSFT WSL (windows subsystem for linux), and when thats fixed we can remove the try/catch -- LGP 2020-03-20
                    try {
                        for (NeighborsMonitor::Neighbor n : monitor.GetNeighbors ()) {
                            DbgTrace (L"discovered %s", Characters::ToString (n).c_str ());
                        }
                    }
                    catch ([[maybe_unused]] const filesystem::filesystem_error& e) {
#if qPlatform_Linux
                        if (e.code () == errc::no_such_file_or_directory) {
                            Test::WarnTestIssue ((L"Ignoring NeighborsMonitor exeption on linux cuz probably WSL failure: " +
                                                  Characters::ToString (current_exception ()))
                                                     .c_str ()); // hopefully fixed soon on WSL - arp -a --LGP 2020-03-19
                            return;
                        }
#endif
                        Execution::ReThrow ();
                    }
                }
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        Test1_URI_::DoTests_ ();
        Test2_InternetAddress_ ();
        Test3_NetworkInterfaceList_ ();
        Test4_DNS_::DoTests_ ();
        Test5_CIDR_::DoTests_ ();
        Test6_Neighbors_::DoTests_ ();
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
