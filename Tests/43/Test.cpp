/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::IO::Network::HTTP
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Stroika/Foundation/IO/Network/HTTP/CacheControl.h"
#include "Stroika/Foundation/IO/Network/HTTP/Cookies.h"
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
                Cookie c = Cookie::Decode (L"SID=31d4d96e407aad42");
                VerifyTestResult (c.fKey == L"SID" and c.fValue == L"31d4d96e407aad42");
                VerifyTestResult (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Decode (L"SID=31d4d96e407aad42; Path=/; Secure; HttpOnly");
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
                Cookie c = Cookie::Decode (L"lang=en-US; Path=/; Domain=example.com");
                VerifyTestResult (c.fKey == L"lang" and c.fValue == L"en-US");
                VerifyTestResult (c.fPath == L"/");
                VerifyTestResult (c.fDomain == L"example.com");
                c.fPath.reset ();
                c.fDomain.reset ();
                VerifyTestResult (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Decode (L"lang=en-US; Expires=Wed, 09 Jun 2021 10:18:14 GMT");
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
                CacheControl c{.fStoreRestriction = CacheControl::eNoStore, .fMaxAge = 0};
#else
                CacheControl c{CacheControl::eNoStore, nullopt, false, nullopt, 0};
#endif
                VerifyTestResult (c.As<String> () == L"no-store, max-age=0");
                VerifyTestResult (c == CacheControl::Parse (L"no-store, max-age=0"));
            }
        }
    }
    namespace HTTPHeaders_Test03_ {
        void RunAll ()
        {
            using KVP = KeyValuePair<String, String>;
            {
                IO::Network::HTTP::Headers h;
                h.SetContentLength (3);
                const auto kReference_ = initializer_list{KVP{L"Content-Length", L"3"}};
                VerifyTestResult ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                IO::Network::HTTP::Headers h;
                h.SetETag (ETag{L"1-2-3-4"});
                const auto kReference_ = initializer_list{KVP{L"ETag", L"\"1-2-3-4\""}};
                VerifyTestResult ((h.As<Mapping<String, String>> () == kReference_));
                h = IO::Network::HTTP::Headers{kReference_};
                VerifyTestResult ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                IO::Network::HTTP::Headers h;
                h.SetContentLength (3);
                h.SetCacheControl (CacheControl{CacheControl::eNoCache});
                const auto kReference_ = initializer_list{
                    KVP{L"Cache-Control", L"no-cache"}, 
                    KVP{L"Content-Length", L"3"}};
                VerifyTestResult ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                const auto kReference_ = initializer_list{
                    KVP{L"Cache-Control", L"no-cache"},
                    KVP{L"blah-blah", L"unknown-header"},
                    KVP{L"Content-Length", L"3"}};
                IO::Network::HTTP::Headers h{kReference_};
                VerifyTestResult (h.GetContentLength () == 3);
                VerifyTestResult (h.GetCacheControl () == CacheControl{CacheControl::eNoCache});
                VerifyTestResult ((h.As<Mapping<String, String>> () == kReference_));
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
