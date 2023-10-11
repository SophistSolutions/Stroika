/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include "Stroika/Foundation/IO/Network/CIDR.h"
#include "Stroika/Foundation/IO/Network/DNS.h"
#include "Stroika/Foundation/IO/Network/Interface.h"
#include "Stroika/Foundation/IO/Network/Neighbors.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

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

        DWORD   ignr = 0;
        wchar_t outBuf[10 * 1024];

        String canonical;
        ThrowIfErrorHRESULT (::CoInternetParseUrl (CComBSTR{w.As<wstring> ().c_str ()}, PARSE_CANONICALIZE, 0, outBuf,
                                                   static_cast<DWORD> (Memory::NEltsOf (outBuf)), &ignr, 0));
        canonical = outBuf;

        {
            size_t e = canonical.find (':');
            if (e != String::npos) {
                *protocol = canonical.SubString (0, e);
            }
        }

        if (SUCCEEDED (::CoInternetParseUrl (CComBSTR{canonical.As<wstring> ().c_str ()}, PARSE_DOMAIN, 0, outBuf,
                                             static_cast<DWORD> (Memory::NEltsOf (outBuf)), &ignr, 0))) {
            *host = outBuf;
        }

        // I cannot see how to get other fields using CoInternetParseURL??? - LGP 2004-04-13...
        {
            String matchStr    = *protocol + L"://"sv + *host;
            size_t startOfPath = canonical.Find (matchStr).value_or (String::npos);
            if (startOfPath == String::npos) {
                matchStr    = *protocol + L":"sv;
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
    namespace Test1_URI_ {
        namespace Private_ {
            void TestHostParsing_ ()
            {
                Debug::TraceContextBumper ctx{"Test1_URI_::Private_::TestHostParsing_"};
                using UniformResourceIdentification::Host;
                VerifyTestResult ((Host{Network::V4::kLocalhost}.AsEncoded () == L"127.0.0.1"sv));
                VerifyTestResult ((Host{InternetAddress{169, 254, 0, 1}}.AsEncoded () == L"169.254.0.1"sv));
                VerifyTestResult ((Host{InternetAddress{L"fe80::44de:4247:5b76:ddc9"}}.AsEncoded () == L"[fe80::44de:4247:5b76:ddc9]"sv));
                VerifyTestResult (
                    (Host::Parse (L"[fe80::44de:4247:5b76:ddc9]").AsInternetAddress () == InternetAddress{L"fe80::44de:4247:5b76:ddc9"}));
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
            void SimpleURITests_ ()
            {
                using IO::Network::UniformResourceIdentification::SchemeType;
                Debug::TraceContextBumper ctx{"Test1_URI_::Private_::SimpleURITests_"};
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
                    DbgTrace (L"X=%s", uri.As<String> ().c_str ());
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
                    IO::Network::URI uri = IO::Network::URI::Parse ("dyn:/StyleSheet.css?ThemeName=Cupertino");
                    VerifyTestResult (*uri.GetScheme () == SchemeType{"dyn"sv});
                    VerifyTestResult (not uri.GetAuthority ().has_value ());
                    VerifyTestResult (uri.GetPath () == "/StyleSheet.css");
                    VerifyTestResult (uri.GetQuery<String> () == "ThemeName=Cupertino");
                    VerifyTestResult (uri.GetQuery ()->operator() (L"ThemeName") == "Cupertino");
                }
                {
                    IO::Network::URI uri = IO::Network::URI::Parse (L"HTTPS://www.MICROSOFT.com/Path");
                    VerifyTestResult (uri.Normalize ().As<String> () == L"https://www.microsoft.com/Path");
                }
                {
                    URI base{"http://www.sophists.com"};
                    VerifyTestResult (base.Combine (URI{"/blag/foo.icon"}).As<String> () == "http://www.sophists.com/blag/foo.icon");
                }
                {
                    URI base{"http://www.sophists.com/"};
                    VerifyTestResult (base.Combine (URI{"/blag/foo.icon"}).As<String> () == "http://www.sophists.com/blag/foo.icon");
                }
            }
            void Test_Reference_Resolution_Examples_From_RFC_3986_ ()
            {
                Debug::TraceContextBumper ctx{"Test1_URI_::Private_::Test_Reference_Resolution_Examples_From_RFC_3986_"};
                // tests from https://tools.ietf.org/html/rfc3986#section-5.4
                URI base = URI::Parse (L"http://a/b/c/d;p?q");

                // https://tools.ietf.org/html/rfc3986#section-5.4.1
                VerifyTestResult (base.Combine (URI::Parse (L"g:h")).As<String> () == L"g:h");
                VerifyTestResult (base.Combine (URI::Parse (L"g")).As<String> () == L"http://a/b/c/g");
                VerifyTestResult (base.Combine (URI::Parse (L"./g")).As<String> () == L"http://a/b/c/g");
                VerifyTestResult (base.Combine (URI::Parse (L"/g")).As<String> () == L"http://a/g");
                VerifyTestResult (base.Combine (URI::Parse (L"//g")).As<String> () == L"http://g");
                VerifyTestResult (base.Combine (URI::Parse (L"?y")).As<String> () == L"http://a/b/c/d;p?y");
                VerifyTestResult (base.Combine (URI::Parse (L"g?y")).As<String> () == L"http://a/b/c/g?y");
                VerifyTestResult (base.Combine (URI::Parse (L"#s")).As<String> () == L"http://a/b/c/d;p?q#s");
                VerifyTestResult (base.Combine (URI::Parse (L"g#s")).As<String> () == L"http://a/b/c/g#s");
                VerifyTestResult (base.Combine (URI::Parse (L"g?y#s")).As<String> () == L"http://a/b/c/g?y#s");
                VerifyTestResult (base.Combine (URI::Parse (L";x")).As<String> () == L"http://a/b/c/;x");
                VerifyTestResult (base.Combine (URI::Parse (L"g;x")).As<String> () == L"http://a/b/c/g;x");
                VerifyTestResult (base.Combine (URI::Parse (L"")).As<String> () == L"http://a/b/c/d;p?q");
                VerifyTestResult (base.Combine (URI::Parse (L".")).As<String> () == L"http://a/b/c/");
                VerifyTestResult (base.Combine (URI::Parse (L"./")).As<String> () == L"http://a/b/c/");
                VerifyTestResult (base.Combine (URI::Parse (L"..")).As<String> () == L"http://a/b/");
                VerifyTestResult (base.Combine (URI::Parse (L"../")).As<String> () == L"http://a/b/");
                VerifyTestResult (base.Combine (URI::Parse (L"../g")).As<String> () == L"http://a/b/g");
                VerifyTestResult (base.Combine (URI::Parse (L"../..")).As<String> () == L"http://a/");
                VerifyTestResult (base.Combine (URI::Parse (L"../../")).As<String> () == L"http://a/");
                VerifyTestResult (base.Combine (URI::Parse (L"../../g")).As<String> () == L"http://a/g");

                // https://tools.ietf.org/html/rfc3986#section-5.4.2 Abnormal Examples
                VerifyTestResult (base.Combine (URI::Parse (L"../../../g")).As<String> () == L"http://a/g");
                VerifyTestResult (base.Combine (URI::Parse (L"../../../../g")).As<String> () == L"http://a/g");
                VerifyTestResult (base.Combine (URI::Parse (L"/./g")).As<String> () == L"http://a/g");
                VerifyTestResult (base.Combine (URI::Parse (L"/../g")).As<String> () == L"http://a/g");
                VerifyTestResult (base.Combine (URI::Parse (L"g.")).As<String> () == L"http://a/b/c/g.");
                VerifyTestResult (base.Combine (URI::Parse (L".g")).As<String> () == L"http://a/b/c/.g");
                VerifyTestResult (base.Combine (URI::Parse (L"g..")).As<String> () == L"http://a/b/c/g..");
                VerifyTestResult (base.Combine (URI::Parse (L"..g")).As<String> () == L"http://a/b/c/..g");
                VerifyTestResult (base.Combine (URI::Parse (L"./../g")).As<String> () == L"http://a/b/g");
                VerifyTestResult (base.Combine (URI::Parse (L"./g/.")).As<String> () == L"http://a/b/c/g/");
                VerifyTestResult (base.Combine (URI::Parse (L"g/./h")).As<String> () == L"http://a/b/c/g/h");
                VerifyTestResult (base.Combine (URI::Parse (L"g/../h")).As<String> () == L"http://a/b/c/h");
                VerifyTestResult (base.Combine (URI::Parse (L"g;x=1/./y")).As<String> () == L"http://a/b/c/g;x=1/y");
                VerifyTestResult (base.Combine (URI::Parse (L"g;x=1/../y")).As<String> () == L"http://a/b/c/y");
                VerifyTestResult (base.Combine (URI::Parse (L"g?y/./x")).As<String> () == L"http://a/b/c/g?y/./x");
                VerifyTestResult (base.Combine (URI::Parse (L"g?y/../x")).As<String> () == L"http://a/b/c/g?y/../x");
                VerifyTestResult (base.Combine (URI::Parse (L"g#s/./x")).As<String> () == L"http://a/b/c/g#s/./x");
                VerifyTestResult (base.Combine (URI::Parse (L"g#s/../x")).As<String> () == L"http://a/b/c/g#s/../x");
                VerifyTestResult (base.Combine (URI::Parse (L"http:g")).As<String> () == L"http:g"); // strict interpretation "for strict parsers"
            }
            void TestEmptyURI_ ()
            {
                Debug::TraceContextBumper ctx{"TestEmptyURI_"};
                {
                    URI u{};
                    VerifyTestResult (u.As<String> () == L"");
                }
                {
                    URI u{L""};
                    VerifyTestResult (u.As<String> () == L"");
                }
                {
                    URI u{L" "};
                    VerifyTestResult (u.As<String> () == L"%20"); // @todo REVIEW SPEC- urlparse(' ').geturl () produces space, but I think this makes more sense
                    VerifyTestResult (u.GetPath () == L" ");
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
                    VerifyTestResult (o.GetScheme () == SchemeType{"http"});
                    VerifyTestResult (o.GetAuthority ()->GetPort () == 80);
                    // NOTE - here python emits '%7Eguido' instead of '~guido'. However, according to
                    // https://tools.ietf.org/html/rfc3986#section-2.3, '~' - %7E - is an unreserved character, and
                    // so Stroika does NOT encode it.
                    VerifyTestResult (o.As<string> () == "http://www.cwi.nl:80/~guido/Python.html");
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
                    VerifyTestResult (o.GetScheme () == nullopt);
                    VerifyTestResult (o.GetAuthority () == Authority::Parse ("www.cwi.nl:80"));
                    VerifyTestResult (o.GetPath () == L"/~guido/Python.html");
                    VerifyTestResult (o.GetQuery () == nullopt);
                    VerifyTestResult (o.GetFragment () == nullopt);
                    VerifyTestResult (o.As<string> () == "//www.cwi.nl:80/~guido/Python.html");
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
                    VerifyTestResult (o.GetScheme () == nullopt);
                    VerifyTestResult (o.GetAuthority () == nullopt);
                    VerifyTestResult (o.GetPath () == L"www.cwi.nl/~guido/Python.html"); // again, differ from python because our getPath returns decoded string
                    VerifyTestResult (o.GetQuery () == nullopt);
                    VerifyTestResult (o.GetFragment () == nullopt);
                    VerifyTestResult (o.As<string> () == "www.cwi.nl/~guido/Python.html"); // again, differ from python because %7E (~) is unreserved character
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
                    VerifyTestResult (o.GetScheme () == nullopt);
                    VerifyTestResult (o.GetAuthority () == nullopt);
                    VerifyTestResult (o.GetPath () == L"help/Python.html");
                    VerifyTestResult (o.GetQuery () == nullopt);
                    VerifyTestResult (o.GetFragment () == nullopt);
                    VerifyTestResult (o.As<string> () == "help/Python.html");
                }
                {
                    /*
                     *  From https://docs.python.org/2/library/urlparse.html
                     *      from urlparse import urljoin
                     *      urljoin('http://www.cwi.nl/%7Eguido/Python.html', 'FAQ.html')
                     */
                    auto o = URI{"http://www.cwi.nl/%7Eguido/Python.html"}.Combine (URI{"FAQ.html"});
                    VerifyTestResult (o.As<string> () == "http://www.cwi.nl/~guido/FAQ.html"); // again, differ from python because %7E (~) is unreserved character
                }
                {
                    /*
                     *  From https://docs.python.org/2/library/urlparse.html
                     *      from urlparse import urljoin
                     *      urljoin('http://www.cwi.nl/%7Eguido/Python.html', '//www.python.org/%7Eguido')
                     */
                    auto o = URI{"http://www.cwi.nl/%7Eguido/Python.html"}.Combine (URI{"//www.python.org/%7Eguido"});
                    VerifyTestResult (o.As<string> () == "http://www.python.org/~guido"); // again, differ from python because %7E (~) is unreserved character
                }
            }
            void Test_PatternUsedInHealthFrame_ ()
            {
                Debug::TraceContextBumper ctx{"Test_PatternUsedInHealthFrame_"};
                using namespace IO::Network::UniformResourceIdentification;
                {
                    auto o = URI{"dyn:/Reminders/Home.htm"};
                    VerifyTestResult (o.GetScheme () == SchemeType{L"dyn"});
                    VerifyTestResult (o.GetPath () == L"/Reminders/Home.htm");
                }
                {
                    auto o = URI{"dyn:/StyleSheet.css?ThemeName=Cupertino"};
                    VerifyTestResult (o.GetScheme () == SchemeType{L"dyn"});
                    VerifyTestResult (o.GetPath () == L"/StyleSheet.css");
                    VerifyTestResult ((*o.GetQuery<Query> ()) (L"ThemeName") == L"Cupertino");
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
                    VerifyTestResult (false);
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
                     initializer_list<URI>{URI{"http://httpbin.org/get"}, URI{"http://www.google.com"}, fred, URI{"http://www.cnn.com"}}
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
                    VerifyTestResult (b1 != b4);
                    VerifyTestResult (b2 != b3 or b1);
                }
            }
            void Test_UPNPBadURIIPv6_ ()
            {
                // Test case for fix: https://stroika.atlassian.net/browse/STK-957
                {
                    using namespace IO::Network::UniformResourceIdentification;
                    // Sub-problem where original bug lies
                    try {
                        optional<Authority> authority = Authority::Parse (L"[fe80::354f:9016:fed2:8b9b]:2869");
                        VerifyTestResult (authority->GetPort () == 2869);
                        VerifyTestResult (authority->GetHost () == InternetAddress{L"fe80::354f:9016:fed2:8b9b"});
                    }
                    catch (...) {
                        VerifyTestResult (false); // not reached - valid parse
                    }
                }
                try {
                    auto uri = URI::Parse (
                        L"http://[fe80::354f:9016:fed2:8b9b]:2869/upnphost/udhisapi.dll?content=uuid:4becec11-428e-46e0-801b-9b293cf1d2c7");
                    VerifyTestResult (uri.GetAuthority ()->GetPort () == 2869);
                    VerifyTestResult (uri.GetAuthority ()->GetHost () == InternetAddress{L"fe80::354f:9016:fed2:8b9b"});
                    VerifyTestResult (uri.GetAbsPath () == L"/upnphost/udhisapi.dll");
                }
                catch (...) {
                    VerifyTestResult (false); // not reached - valid parse
                }
                try {
                    auto uri = URI::Parse (L"http://[fe80::354f:9016:fed2:8b9b");
                    VerifyTestResult (false); // not reached - invalid URL
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
            VerifyTestResult (InternetAddress (V4::kLocalhost.As<in_addr> (InternetAddress::ByteOrder::Host)) != V4::kLocalhost or
                              ntohl (0x01020304) == 0x01020304); // if big-endian machine, net byte order equals host byte order
        }
        {
            VerifyTestResult (InternetAddress{L"192.168.99.1"}.AsAddressFamily (InternetAddress::AddressFamily::V6) ==
                              InternetAddress{L"2002:C0A8:6301::"});
            VerifyTestResult (InternetAddress{L"2002:C0A8:6301::"}.AsAddressFamily (InternetAddress::AddressFamily::V4) == InternetAddress{L"192.168.99.1"});
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
                DNS::HostEntry e = DNS::kThe.GetHostEntry (L"www.sophists.com");
                VerifyTestResult (e.fCanonicalName.Contains (L".sophists.com"));
                VerifyTestResult (e.fAddressList.size () >= 1);
            }
            {
                DNS::HostEntry e = DNS::kThe.GetHostEntry (L"www.google.com");
                VerifyTestResult (e.fAddressList.size () >= 1);
            }
            {
                DNS::HostEntry e = DNS::kThe.GetHostEntry (L"www.cnn.com");
                VerifyTestResult (e.fAddressList.size () >= 1);
            }
            {
                optional<String> aaa = DNS::kThe.ReverseLookup (InternetAddress (23, 56, 90, 167));
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
                CIDR cidr{L"10.70.0.0/15"};
                auto cidr2 = CIDR{InternetAddress{L"10.70.0.0"}, 15};
                VerifyTestResult (cidr == cidr2);
                VerifyTestResult (Characters::ToString (cidr) == L"10.70.0.0/15");
                VerifyTestResult (cidr.GetNumberOfSignificantBits () == 15);
                VerifyTestResult ((cidr.GetRange () == Traversal::DiscreteRange<InternetAddress>{InternetAddress{10, 70, 0, 0},
                                                                                                 InternetAddress{10, 71, 255, 255}}));
            }
            {
                auto cidr = CIDR{InternetAddress{"192.168.56.1"}, 24};
                VerifyTestResult (Characters::ToString (cidr) == L"192.168.56.0/24");
            }
            {
                auto cidr = CIDR{InternetAddress{"172.28.240.1"}, 20};
                VerifyTestResult (Characters::ToString (cidr) == L"172.28.240.0/20");
            }
            {
                auto cidr = CIDR{InternetAddress{"172.17.185.1"}, 28};
                VerifyTestResult (Characters::ToString (cidr) == L"172.17.185.0/28");
            }
            {
                // fix for https://stroika.atlassian.net/browse/STK-909
                auto cidr = CIDR{V6::kAddrAny, 64};
                VerifyTestResult (Characters::ToString (cidr) == L"in6addr_any/64");
                VerifyTestResult (CIDR{cidr.As<String> ()} == cidr); // can roundtrip numeric form
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
                Time::DurationSecondsType     timeoutAt = (Time::GetTickCount () + Time::Duration{3s}).As<Time::DurationSecondsType> ();
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
                            TestHarness::WarnTestIssue ((L"Ignoring NeighborsMonitor exeption on linux cuz probably WSL failure: " +
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
    void DoRegressionTests_ ()
    {
        Test1_URI_::DoTests_ ();
        Test2_InternetAddress_ ();
        Test3_NetworkInterfaceList_ ();
        Test4_DNS_::DoTests_ ();
        Test5_CIDR_::DoTests_ ();
        Test6_Neighbors_::DoTests_ ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
