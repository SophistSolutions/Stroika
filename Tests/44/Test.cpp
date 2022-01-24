/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
//  TEST    Foundation::IO::Network::HTTP
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Stroika/Foundation/IO/Network/HTTP/CacheControl.h"
#include "Stroika/Foundation/IO/Network/HTTP/Cookie.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;

namespace {
    namespace Cookies_Test01_ {
        void RunAll ()
        {
            // Examples from https://tools.ietf.org/html/rfc6265#section-3.1
            {
                Cookie c = Cookie::Parse (L"SID=31d4d96e407aad42");
                VerifyTestResult (c.fKey == L"SID" and c.fValue == L"31d4d96e407aad42");
                VerifyTestResult (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Parse (L"SID=31d4d96e407aad42; Path=/; Secure; HttpOnly");
                VerifyTestResult (c.fKey == L"SID" and c.fValue == L"31d4d96e407aad42");
                VerifyTestResult (c.fPath == L"/");
                VerifyTestResult (c.fSecure);
                VerifyTestResult (c.fHttpOnly);
                c.fPath.reset ();
                c.fSecure   = false;
                c.fHttpOnly = false;
                VerifyTestResult (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Parse (L"lang=en-US; Path=/; Domain=example.com");
                VerifyTestResult (c.fKey == L"lang" and c.fValue == L"en-US");
                VerifyTestResult (c.fPath == L"/");
                VerifyTestResult (c.fDomain == L"example.com");
                c.fPath.reset ();
                c.fDomain.reset ();
                VerifyTestResult (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Parse (L"lang=en-US; Expires=Wed, 09 Jun 2021 10:18:14 GMT");
                VerifyTestResult (c.fKey == L"lang" and c.fValue == L"en-US");
                using namespace Time;
                VerifyTestResult (c.fExpires == (DateTime{Date{Time::Year{2021}, MonthOfYear::eJune, DayOfMonth{9}}, TimeOfDay{10, 18, 14}, Timezone::kUTC}));
                c.fExpires.reset ();
                VerifyTestResult (c.GetAttributes ().empty ());
            }
        }
    }
    namespace CacheControl_Test02_ {
        void RunAll ()
        {
            {
                CacheControl c{};
                VerifyTestResult (c.As<String> () == L"");
                VerifyTestResult (c == CacheControl::Parse (L""));
            }
            {
                CacheControl c{CacheControl::eNoStore};
                VerifyTestResult (c.As<String> () == L"no-store");
                VerifyTestResult (c == CacheControl::Parse (L"no-store"));
            }
            {
#if __cpp_designated_initializers
                CacheControl c{.fCacheability = CacheControl::eNoStore, .fMaxAge = 0};
#else
                CacheControl c{CacheControl::eNoStore, 0};
#endif
                VerifyTestResult (c.As<String> () == L"no-store, max-age=0");
                VerifyTestResult (c == CacheControl::Parse (L"no-store, max-age=0"));
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
                VerifyTestResult ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                IO::Network::HTTP::Headers h;
                h.ETag                 = ETag{L"1-2-3-4"};
                const auto kReference_ = Mapping<String, String>{{L"ETag", L"\"1-2-3-4\""}};
                VerifyTestResult ((h.As<Mapping<String, String>> () == kReference_));
                h = IO::Network::HTTP::Headers{kReference_};
                VerifyTestResult ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                IO::Network::HTTP::Headers h;
                h.contentLength        = 3;
                h.cacheControl         = CacheControl{CacheControl::eNoCache};
                const auto kReference_ = Mapping<String, String>{{L"Cache-Control", L"no-cache"},
                                                                 {L"Content-Length", L"3"}};
                VerifyTestResult ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                const auto                 kReference_ = Mapping<String, String>{{L"Cache-Control", L"no-cache"},
                                                                 {L"blah-blah", L"unknown-header"},
                                                                 {L"Content-Length", L"3"}};
                IO::Network::HTTP::Headers h{kReference_};
                VerifyTestResult (h.contentLength () == 3);
                VerifyTestResult (h.cacheControl () == CacheControl{CacheControl::eNoCache});
                VerifyTestResult ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                using namespace Time;
                IO::Network::HTTP::Headers h1;
                // Test cases from https://tools.ietf.org/html/rfc2616#section-3.3.1
                const DateTime kReferenceTest_{Date{Year (1994), MonthOfYear::eNovember, DayOfMonth (6)}, TimeOfDay{8, 49, 37}, Timezone::kUTC};
                const String   kTest1_ = L"Sun, 06 Nov 1994 08:49:37 GMT";
                const String   kTest2_ = L"Sunday, 06-Nov-94 08:49:37 GMT";
                const String   kTest3_ = L"Sun Nov  6 08:49:37 1994";
                VerifyTestResult (h1.date == nullopt);
                VerifyTestResult (h1.LookupOne (HTTP::HeaderName::kDate) == nullopt);
                h1.date = kReferenceTest_;
                VerifyTestResult (h1.date == kReferenceTest_);
                VerifyTestResult (h1.LookupOne (HTTP::HeaderName::kDate) == kTest1_);
                IO::Network::HTTP::Headers h2 = h1;
                VerifyTestResult (h2.date == kReferenceTest_);
                VerifyTestResult (h2.LookupOne (HTTP::HeaderName::kDate) == kTest1_);
                // now check older formats still work
                h2.date = nullopt;
                h2.Set (HTTP::HeaderName::kDate, kTest2_);
#if 0
                // see https://stroika.atlassian.net/browse/STK-731 - should support parsing (not writing) older formats too
                VerifyTestResult (h2.date == kReferenceTest_);
                VerifyTestResult (h2.LookupOne (HTTP::HeaderName::kDate) == kTest1_);
#endif
                h2.date = nullopt;
                h2.Set (HTTP::HeaderName::kDate, kTest3_);
#if 0
                // see https://stroika.atlassian.net/browse/STK-731 - should support parsing (not writing) older formats too
                VerifyTestResult (h2.date == kReferenceTest_);
                VerifyTestResult (h2.LookupOne (HTTP::HeaderName::kDate) == kTest1_);
#endif
            }
            {
                IO::Network::HTTP::Headers h1;
                ETag                       etag = ETag{L"1-2-3-4"};
                h1.ETag                         = etag;
                IO::Network::HTTP::Headers h2;
                h2.ifNoneMatch = IfNoneMatch{{etag}};
                VerifyTestResult (h1 != h2);
                VerifyTestResult (h2.ifNoneMatch ().value ().fETags.Contains (etag));
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Cookies_Test01_::RunAll ();
        CacheControl_Test02_::RunAll ();
        HTTPHeaders_Test03_::RunAll ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
