/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::IO::Network::HTTP
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Foundation/IO/Network/HTTP/CacheControl.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "Stroika/Foundation/IO/Network/HTTP/Cookie.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/MessageStartTextInputStreamBinaryAdapter.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/SharedMemoryStream.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;

using namespace Stroika::Frameworks;

#if qStroika_HasComponent_googletest
namespace {
    namespace Cookies_Test01_ {
        void RunAll ()
        {
            // Examples from https://tools.ietf.org/html/rfc6265#section-3.1
            {
                Cookie c = Cookie::Parse ("SID=31d4d96e407aad42");
                EXPECT_TRUE (c.fKey == "SID" and c.fValue == "31d4d96e407aad42");
                EXPECT_TRUE (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Parse ("SID=31d4d96e407aad42; Path=/; Secure; HttpOnly");
                EXPECT_TRUE (c.fKey == "SID" and c.fValue == "31d4d96e407aad42");
                EXPECT_TRUE (c.fPath == "/");
                EXPECT_TRUE (c.fSecure);
                EXPECT_TRUE (c.fHttpOnly);
                c.fPath.reset ();
                c.fSecure   = false;
                c.fHttpOnly = false;
                EXPECT_TRUE (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Parse ("lang=en-US; Path=/; Domain=example.com");
                EXPECT_TRUE (c.fKey == "lang" and c.fValue == "en-US");
                EXPECT_TRUE (c.fPath == "/");
                EXPECT_TRUE (c.fDomain == "example.com");
                c.fPath.reset ();
                c.fDomain.reset ();
                EXPECT_TRUE (c.GetAttributes ().empty ());
            }
            {
                // As<String> writes it as a Set-Cookie header's value - name=value, then its attributes - which Parse reads back
                Cookie c{"lang", "en-US"};
                c.fPath   = "/docs";
                c.fSecure = true;
                String s  = c.As<String> ();
                EXPECT_TRUE (s.StartsWith ("lang=en-US"sv)) << s;
                Cookie d = Cookie::Parse (s);
                EXPECT_EQ (d.fKey, c.fKey);
                EXPECT_EQ (d.fValue, c.fValue);
                EXPECT_EQ (d.fPath, c.fPath);
                EXPECT_EQ (d.fSecure, c.fSecure);
            }
            {
                Cookie c = Cookie::Parse ("lang=en-US; Expires=Wed, 09 Jun 2021 10:18:14 GMT");
                EXPECT_TRUE (c.fKey == "lang" and c.fValue == "en-US");
                using namespace Time;
                EXPECT_TRUE (c.fExpires == (DateTime{Date{Time::Year{2021}, June, DayOfMonth{9}}, TimeOfDay{10, 18, 14}, Timezone::kUTC}));
                c.fExpires.reset ();
                EXPECT_TRUE (c.GetAttributes ().empty ());
            }
            {
                // and written as HTTP writes dates, in GMT - so a browser reads it, as does Parse
                using namespace Time;
                Cookie c{"lang", "en-US"};
                c.fExpires = DateTime{Date{Time::Year{2021}, June, DayOfMonth{9}}, TimeOfDay{6, 18, 14}, Timezone{-4 * 60}};
                EXPECT_EQ (c.GetAttributes ().LookupValue (Cookie::kExpiresAttributeLabel), "Wed, 09 Jun 2021 10:18:14 GMT"sv);
                EXPECT_EQ (Cookie::Parse (c.As<String> ()).fExpires, c.fExpires);
                // and read in HTTP's other date forms - such as this, the most common in cookies (from Netscape's original spec)
                EXPECT_EQ (Cookie::Parse ("lang=en-US; Expires=Wed, 09-Jun-2021 10:18:14 GMT"sv).fExpires, c.fExpires);
            }
            {
                // AddAttribute of one string, as a Set-Cookie header writes an attribute: name=value, or a name alone
                Cookie c{"lang", "en-US"};
                c.AddAttribute ("Path=/docs"sv);
                EXPECT_TRUE (c.fPath == "/docs");
                c.AddAttribute ("Priority=High"sv);
                EXPECT_EQ (c.GetAttributes ().LookupValue ("Priority"sv), "High"sv);
                c.AddAttribute ("Secure"sv);
                EXPECT_TRUE (c.fSecure);
                c.AddAttribute ("Partitioned"sv);
                EXPECT_TRUE (c.GetAttributes ().ContainsKey ("Partitioned"sv));
            }
        }
    }
    namespace CacheControl_Test02_ {
        void RunAll ()
        {
            {
                CacheControl c{};
                EXPECT_TRUE (c.As<String> () == "");
                EXPECT_TRUE (c == CacheControl::Parse (""));
            }
            {
                CacheControl c{CacheControl::eNoStore};
                EXPECT_TRUE (c.As<String> () == "no-store");
                EXPECT_TRUE (c == CacheControl::Parse ("no-store"));
            }
            {
                CacheControl c{.fCacheability = CacheControl::eNoStore, .fMaxAge = 0};
                EXPECT_TRUE (c.As<String> () == "no-store, max-age=0");
                EXPECT_TRUE (c == CacheControl::Parse ("no-store, max-age=0"));
            }
        }
    }
    namespace HTTPHeaders_Test03_ {
        void RunAll ()
        {
            {
                IO::Network::HTTP::Headers h;
                h.contentLength        = 3;
                const auto kReference_ = Mapping<String, String>{{"Content-Length", "3"}};
                EXPECT_TRUE ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                IO::Network::HTTP::Headers h;
                h.ETag                 = ETag{"1-2-3-4"};
                const auto kReference_ = Mapping<String, String>{{"ETag", "\"1-2-3-4\""}};
                EXPECT_TRUE ((h.As<Mapping<String, String>> () == kReference_));
                h = IO::Network::HTTP::Headers{kReference_};
                EXPECT_TRUE ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                IO::Network::HTTP::Headers h;
                h.contentLength        = 3;
                h.cacheControl         = CacheControl{CacheControl::eNoCache};
                const auto kReference_ = Mapping<String, String>{{"Cache-Control", "no-cache"}, {"Content-Length", "3"}};
                EXPECT_TRUE ((h.As<Mapping<String, String>> () == kReference_));
            }
            {
                const auto kReference_ =
                    Mapping<String, String>{{"Cache-Control", "no-cache"}, {"blah-blah", "unknown-header"}, {"Content-Length", "3"}};
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
                const String   kTest1_ = "Sun, 06 Nov 1994 08:49:37 GMT";
                const String   kTest2_ = "Sunday, 06-Nov-94 08:49:37 GMT";
                const String   kTest3_ = "Sun Nov  6 08:49:37 1994";
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
                EXPECT_TRUE (h2.date == kReferenceTest_);
                EXPECT_TRUE (h2.LookupOne (HTTP::HeaderName::kDate) == kTest1_);
                h2.date = nullopt;
                h2.Set (HTTP::HeaderName::kDate, kTest3_);
                EXPECT_TRUE (h2.date == kReferenceTest_);
                EXPECT_TRUE (h2.LookupOne (HTTP::HeaderName::kDate) == kTest1_);
            }
            {
                IO::Network::HTTP::Headers h1;
                ETag                       etag = ETag{"1-2-3-4"};
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
    GTEST_TEST (Foundation_IO_Network_HTTP, MessageStartTextInputStreamBinaryAdapter_ToString)
    {
        Debug::TraceContextBumper ctx{"MessageStartTextInputStreamBinaryAdapter_ToString"};
        const std::byte           kMessageStart_[] = {std::byte{'G'},  std::byte{'E'},  std::byte{'T'},  std::byte{0xab},
                                                      std::byte{'\r'}, std::byte{'\n'}, std::byte{'\r'}, std::byte{'\n'}};
        MessageStartTextInputStreamBinaryAdapter::Ptr adapter =
            MessageStartTextInputStreamBinaryAdapter::New (Streams::MemoryStream::New<std::byte> (span{kMessageStart_}));
        EXPECT_TRUE (adapter.AssureHeaderSectionAvailable ());
        String asBytes = adapter.ToString (MessageStartTextInputStreamBinaryAdapter::eAsBytes); // hex - it threw format_error
        EXPECT_TRUE (asBytes.Contains ("x47, x45, x54, xab, "sv)) << asBytes.AsNarrowSDKString ();
    }

    /*
     *  A header that has arrived only in part is not available yet - AssureHeaderSectionAvailable returns false, keeping what
     *  it read - and is, once the rest arrives. Before Stroika v3.0d26 it threw EWouldBlock instead.
     */
    GTEST_TEST (Foundation_IO_Network_HTTP, MessageStartTextInputStreamBinaryAdapter_HeaderInPieces_)
    {
        Debug::TraceContextBumper                     ctx{"MessageStartTextInputStreamBinaryAdapter_HeaderInPieces_"};
        Streams::InputOutputStream::Ptr<std::byte>    source  = Streams::SharedMemoryStream::New<std::byte> ();
        MessageStartTextInputStreamBinaryAdapter::Ptr adapter = MessageStartTextInputStreamBinaryAdapter::New (source);
        auto arrives = [&] (string_view s) { source.Write (as_bytes (span{s.data (), s.size ()})); };
        arrives ("GET /x HTTP/1.1\r\nHo"sv);
        EXPECT_FALSE (adapter.AssureHeaderSectionAvailable ());
        arrives ("st: h\r\n\r\n"sv);
        ASSERT_TRUE (adapter.AssureHeaderSectionAvailable ());
        EXPECT_EQ (adapter.ReadLine (), "GET /x HTTP/1.1\r\n"sv);
        EXPECT_EQ (adapter.ReadLine (), "Host: h\r\n"sv);
    }

    /*
     *  A weak entity-tag is W/"..." (RFC 9110, 8.8.3): read, and written back, as that - in an ETag header, and among an
     *  If-None-Match header's tags
     */
    GTEST_TEST (Foundation_IO_Network_HTTP, WeakETag_)
    {
        optional<ETag> weak = ETag::Parse ("W/\"v1\""sv);
        ASSERT_TRUE (weak.has_value ());
        EXPECT_TRUE (weak->fWeak);
        EXPECT_EQ (weak->fValue, "v1"sv);
        EXPECT_EQ (weak->As<String> (), "W/\"v1\""sv);
        optional<ETag> strong = ETag::Parse ("\"v2\""sv);
        ASSERT_TRUE (strong.has_value ());
        EXPECT_FALSE (strong->fWeak);
        EXPECT_EQ (strong->As<String> (), "\"v2\""sv);

        Headers h;
        h.Add (String{HeaderName::kETag}, "W/\"v1\""sv);
        EXPECT_EQ (h.LookupOne (String{HeaderName::kETag}), "W/\"v1\""sv);
        h.Add (String{HeaderName::kIfNoneMatch}, "W/\"v1\", \"v2\""sv);
        EXPECT_EQ (h.LookupOne (String{HeaderName::kIfNoneMatch}), "W/\"v1\", \"v2\""sv);
    }

    /*
     *  Cache-Control as RFC 9111 has it: s-maxage (not "s-max-age") both ways; immutable read back; a delta-seconds argument
     *  read quoted or not, one too big for 31 bits as kMaximumAgeValue, one not digits as 0
     */
    GTEST_TEST (Foundation_IO_Network_HTTP, CacheControl_RFC9111_)
    {
        CacheControl shared{.fSharedMaxAge = 600};
        EXPECT_EQ (shared.As<String> (), "s-maxage=600"sv);
        EXPECT_TRUE (CacheControl::Parse ("s-maxage=600") == shared);
        EXPECT_TRUE (CacheControl::Parse (CacheControl::kImmutable.As<String> ()) == CacheControl::kImmutable);
        EXPECT_TRUE (CacheControl::Parse ("max-age=\"5\"").fMaxAge == 5u);
        EXPECT_TRUE (CacheControl::Parse ("max-age=99999999999").fMaxAge == CacheControl::kMaximumAgeValue);
        EXPECT_TRUE (CacheControl::Parse ("max-age=-5").fMaxAge == 0u);
        EXPECT_TRUE (CacheControl::Parse ("max-age=abc").fMaxAge == 0u);
    }

    /*
     *  A Keep-Alive header's parameters (https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Keep-Alive) are all read,
     *  in either order, one that is not a number skipped - and written as read.
     */
    GTEST_TEST (Foundation_IO_Network_HTTP, KeepAlive_)
    {
        for (const String& value : {"timeout=5, max=100"_k, "max=100, timeout=5"_k}) {
            KeepAlive k = KeepAlive::Parse (value);
            EXPECT_TRUE (k.fTimeout == 5s);
            EXPECT_TRUE (k.fMessages == 100u);
        }
        KeepAlive bad = KeepAlive::Parse ("timeout=soon, max=many"sv);
        EXPECT_FALSE (bad.fTimeout.has_value ());
        EXPECT_FALSE (bad.fMessages.has_value ());

        KeepAlive written{.fMessages = 100, .fTimeout = 5s};
        EXPECT_EQ (written.AsValue (), "timeout=5, max=100"sv);
        Headers h;
        h.keepAlive = written;
        EXPECT_EQ (h.LookupOne (String{HeaderName::kKeepAlive}), "timeout=5, max=100"sv);
    }

    /*
     *  Merging two Keep-Alive limits keeps the stricter of each - the fewer messages, the shorter timeout; a limit only one sets,
     *  that one's
     */
    GTEST_TEST (Foundation_IO_Network_HTTP, KeepAlive_Merge_)
    {
        optional<KeepAlive> m = KeepAlive::Merge (KeepAlive{.fMessages = 100, .fTimeout = 5s}, KeepAlive{.fMessages = 5, .fTimeout = 60s});
        ASSERT_TRUE (m.has_value ());
        EXPECT_TRUE (m->fMessages == 5u);
        EXPECT_TRUE (m->fTimeout == 5s);
        m = KeepAlive::Merge (KeepAlive{.fMessages = 3}, KeepAlive{.fTimeout = 10s});
        ASSERT_TRUE (m.has_value ());
        EXPECT_TRUE (m->fMessages == 3u);
        EXPECT_TRUE (m->fTimeout == 10s);
        m = KeepAlive::Merge (nullopt, KeepAlive{.fMessages = 7});
        ASSERT_TRUE (m.has_value ());
        EXPECT_TRUE (m->fMessages == 7u);
        EXPECT_FALSE (KeepAlive::Merge (nullopt, nullopt).has_value ());
    }

    /*
     *  Content-Length read as RFC 9112 (6.3) has it: digits, or a list of the same value; anything else - not digits, a sign, a
     *  list of different values - throws ClientErrorException
     */
    GTEST_TEST (Foundation_IO_Network_HTTP, ContentLength_RFC9112_)
    {
        Headers one;
        one.Add ("Content-Length"sv, "5"sv);
        EXPECT_TRUE (one.contentLength () == 5u);
        Headers repeated;
        repeated.Add ("Content-Length"sv, "5, 5"sv);
        EXPECT_TRUE (repeated.contentLength () == 5u);
        for (string_view bad : {"abc"sv, "-1"sv, "+5"sv, "5x"sv, "5, 6"sv, ""sv}) {
            Headers h;
            EXPECT_THROW (h.Add ("Content-Length"sv, String{bad}), ClientErrorException) << bad;
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
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "[  SKIPPED ] every test - Stroika regression tests require building with google test feature" << endl;
#endif
}
