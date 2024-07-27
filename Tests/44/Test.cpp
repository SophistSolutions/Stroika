/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::IO::Network::HTTP
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Foundation/IO/Network/HTTP/CacheControl.h"
#include "Stroika/Foundation/IO/Network/HTTP/Cookie.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;

using namespace Stroika::Frameworks;

#if qHasFeature_GoogleTest
namespace {
    namespace Cookies_Test01_ {
        void RunAll ()
        {
            // Examples from https://tools.ietf.org/html/rfc6265#section-3.1
            {
                Cookie c = Cookie::Parse (L"SID=31d4d96e407aad42");
                EXPECT_TRUE (c.fKey == L"SID" and c.fValue == L"31d4d96e407aad42");
                EXPECT_TRUE (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Parse (L"SID=31d4d96e407aad42; Path=/; Secure; HttpOnly");
                EXPECT_TRUE (c.fKey == L"SID" and c.fValue == L"31d4d96e407aad42");
                EXPECT_TRUE (c.fPath == L"/");
                EXPECT_TRUE (c.fSecure);
                EXPECT_TRUE (c.fHttpOnly);
                c.fPath.reset ();
                c.fSecure   = false;
                c.fHttpOnly = false;
                EXPECT_TRUE (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Parse (L"lang=en-US; Path=/; Domain=example.com");
                EXPECT_TRUE (c.fKey == L"lang" and c.fValue == L"en-US");
                EXPECT_TRUE (c.fPath == L"/");
                EXPECT_TRUE (c.fDomain == L"example.com");
                c.fPath.reset ();
                c.fDomain.reset ();
                EXPECT_TRUE (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Parse (L"lang=en-US; Expires=Wed, 09 Jun 2021 10:18:14 GMT");
                EXPECT_TRUE (c.fKey == L"lang" and c.fValue == L"en-US");
                using namespace Time;
                EXPECT_TRUE (c.fExpires == (DateTime{Date{Time::Year{2021}, June, DayOfMonth{9}}, TimeOfDay{10, 18, 14}, Timezone::kUTC}));
                c.fExpires.reset ();
                EXPECT_TRUE (c.GetAttributes ().empty ());
            }
        }
    }
    namespace CacheControl_Test02_ {
        void RunAll ()
        {
            {
                CacheControl c{};
                EXPECT_TRUE (c.As<String> () == L"");
                EXPECT_TRUE (c == CacheControl::Parse (L""));
            }
            {
                CacheControl c{CacheControl::eNoStore};
                EXPECT_TRUE (c.As<String> () == L"no-store");
                EXPECT_TRUE (c == CacheControl::Parse (L"no-store"));
            }
            {
                CacheControl c{.fCacheability = CacheControl::eNoStore, .fMaxAge = 0};
                EXPECT_TRUE (c.As<String> () == L"no-store, max-age=0");
                EXPECT_TRUE (c == CacheControl::Parse (L"no-store, max-age=0"));
            }
        }
    }
    namespace HTTPHeaders_Test03_ {
        void RunAll ()
        {
            {
                IO::Network::HTTP::Headers h;
                h.contentLength        = 3;
                const auto kReference_ = Mapping<String, String>{{L"Content-Length", L"3"}};
                EXPECT_TRUE ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                IO::Network::HTTP::Headers h;
                h.ETag                 = ETag{L"1-2-3-4"};
                const auto kReference_ = Mapping<String, String>{{L"ETag", L"\"1-2-3-4\""}};
                EXPECT_TRUE ((h.As<Mapping<String, String>> () == kReference_));
                h = IO::Network::HTTP::Headers{kReference_};
                EXPECT_TRUE ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                IO::Network::HTTP::Headers h;
                h.contentLength        = 3;
                h.cacheControl         = CacheControl{CacheControl::eNoCache};
                const auto kReference_ = Mapping<String, String>{{L"Cache-Control", L"no-cache"}, {L"Content-Length", L"3"}};
                EXPECT_TRUE ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                const auto kReference_ =
                    Mapping<String, String>{{L"Cache-Control", L"no-cache"}, {L"blah-blah", L"unknown-header"}, {L"Content-Length", L"3"}};
                IO::Network::HTTP::Headers h{kReference_};
                EXPECT_TRUE (h.contentLength () == 3);
                EXPECT_TRUE (h.cacheControl () == CacheControl{CacheControl::eNoCache});
                EXPECT_TRUE ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                using namespace Time;
                IO::Network::HTTP::Headers h1;
                // Test cases from https://tools.ietf.org/html/rfc2616#section-3.3.1
                const DateTime kReferenceTest_{Date{Year{1994}, November, DayOfMonth{6}}, TimeOfDay{8, 49, 37}, Timezone::kUTC};
                const String   kTest1_ = L"Sun, 06 Nov 1994 08:49:37 GMT";
                const String   kTest2_ = L"Sunday, 06-Nov-94 08:49:37 GMT";
                const String   kTest3_ = L"Sun Nov  6 08:49:37 1994";
                EXPECT_TRUE (h1.date == nullopt);
                EXPECT_TRUE (h1.LookupOne (HTTP::HeaderName::kDate) == nullopt);
                h1.date = kReferenceTest_;
                EXPECT_TRUE (h1.date == kReferenceTest_);
                EXPECT_TRUE (h1.LookupOne (HTTP::HeaderName::kDate) == kTest1_);
                IO::Network::HTTP::Headers h2 = h1;
                EXPECT_TRUE (h2.date == kReferenceTest_);
                EXPECT_TRUE (h2.LookupOne (HTTP::HeaderName::kDate) == kTest1_);
                // now check older formats still work
                h2.date = nullopt;
                h2.Set (HTTP::HeaderName::kDate, kTest2_);
#if 0
                // see http://stroika-bugs.sophists.com/browse/STK-731 - should support parsing (not writing) older formats too
                EXPECT_TRUE (h2.date == kReferenceTest_);
                EXPECT_TRUE (h2.LookupOne (HTTP::HeaderName::kDate) == kTest1_);
#endif
                h2.date = nullopt;
                h2.Set (HTTP::HeaderName::kDate, kTest3_);
#if 0
                // see http://stroika-bugs.sophists.com/browse/STK-731 - should support parsing (not writing) older formats too
                EXPECT_TRUE (h2.date == kReferenceTest_);
                EXPECT_TRUE (h2.LookupOne (HTTP::HeaderName::kDate) == kTest1_);
#endif
            }
            {
                IO::Network::HTTP::Headers h1;
                ETag                       etag = ETag{L"1-2-3-4"};
                h1.ETag                         = etag;
                IO::Network::HTTP::Headers h2;
                h2.ifNoneMatch = IfNoneMatch{{etag}};
                EXPECT_TRUE (h1 != h2);
                EXPECT_TRUE (h2.ifNoneMatch ().value ().fETags.Contains (etag));
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        Cookies_Test01_::RunAll ();
        CacheControl_Test02_::RunAll ();
        HTTPHeaders_Test03_::RunAll ();
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
