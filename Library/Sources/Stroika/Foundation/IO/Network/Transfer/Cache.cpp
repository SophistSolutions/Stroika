/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Cache/LRUCache.h"
#include "Stroika/Foundation/Characters/Number.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/IO/Network/HTTP/CacheControl.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Cache.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cache;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;
using namespace Stroika::Foundation::Time;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace {

    struct DefaultCacheRep_ : Transfer::Cache::IRep {

        using Element     = Transfer::Cache::Element;
        using EvalContext = Transfer::Cache::EvalContext;
        using Options     = Transfer::Cache::Options;

        DefaultCacheRep_ (const Options& options)
            : fOptions_{options}
            , fCache_{options.fCacheSize, 11u}
        {
        }

        // what the cache stores for a response: it, with the default TTL where fOptions_ has one
        Element NewElement_ (const Response& response) const
        {
            Element e{response};
            if (fOptions_.fDefaultResourceTTL) {
                e.fExpiresDefault = DateTime::Now () + *fOptions_.fDefaultResourceTTL;
            }
            return e;
        }

        virtual optional<Response> OnBeforeFetch (EvalContext* context, const URI& schemeAndAuthority, Request* request) noexcept override
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
                "IO::Network::Transfer ... {}::DefaultCacheRep_::OnBeforeFetch", "schemeAndAuthority={}"_f, schemeAndAuthority)};
#endif
            if (request->fMethod == HTTP::Methods::kGet) {
                try {
                    URI fullURI       = schemeAndAuthority.Combine (request->fAuthorityRelativeURL);
                    context->fFullURI = fullURI; // only try to cache GETs (for now)
                    if (optional<Element> o = fCache_.Lookup (fullURI)) {
                        // check if cacheable - and either return directly or
                        DateTime now               = DateTime::Now ();
                        bool     canReturnDirectly = o->IsValidUntil ().value_or (now) > now;
                        if (canReturnDirectly) {
                            // fill in response and return short-circuiting normal full web fetch
                            Mapping<String, String> headers = o->GetCombinedHeaders ();
                            if (fOptions_.fCachedResultHeader) {
                                headers.Add (*fOptions_.fCachedResultHeader, String{});
                            }
                            return Response{o->fBody, HTTP::StatusCodes::kOK, headers};
                        }
                        // Revalidate with what the stored response has - its ETag (If-None-Match) and its Last-Modified
                        // (If-Modified-Since), both where it has both, as RFC 9111 (4.3.1) says: a server that knows both uses
                        // If-None-Match and ignores If-Modified-Since (RFC 9110, 13.1.3)
                        bool canCheckCacheETAG = o->fETag.has_value ();
                        if (canCheckCacheETAG) {
                            context->fCachedElement = *o;
                            request->fOverrideHeaders.Add (HTTP::HeaderName::kIfNoneMatch, HTTP::ETag{*o->fETag, o->fETagIsWeak}.As<String> ());
                            // keep going as we can combine If-None-Match/If-Modified-Since
                        }
                        bool canCheckModifiedSince = o->fLastModified.has_value ();
                        if (canCheckModifiedSince) {
                            context->fCachedElement = *o;
                            // an HTTP date - not quoted, as the ETag is
                            request->fOverrideHeaders.Add (HTTP::HeaderName::kIfModifiedSince, o->fLastModified->Format (DateTime::kHTTPDateFormat));
                        }
                    }
                }
                catch (...) {
                    DbgTrace ("Cache::OnBeforeFetch::oops: {}"_f, current_exception ()); // ignore...
                }
            }
            // In this case, no caching is possible - nothing todo
            return nullopt;
        }

        virtual void OnAfterFetch (const EvalContext& context, Response* response) noexcept override
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"DefaultCacheRep_::OnAfterFetch", "context.fFullURI={}"_f, context.fFullURI};
#endif
            RequireNotNull (response);
            switch (response->GetStatus ()) {
                case HTTP::StatusCodes::kOK: {
                    if (context.fFullURI) {
                        try {
                            Element cacheElement = NewElement_ (*response);
                            if (cacheElement.IsCachable ()) {
                                //DbgTrace ("Add2Cache: uri={}, cacheElement={}"_f, *context.fFullURI, cacheElement);
                                fCache_.Add (*context.fFullURI, cacheElement);
                            }
                        }
                        catch (...) {
                            DbgTrace ("Cache::OnAfterFetch::oops(ok): {}"_f, current_exception ()); // ignore...
                        }
                    }
                } break;
                case HTTP::StatusCodes::kNotModified: {
                    try {
                        /*
                         *  Not 100% clear what to return here as status. Mostly want 200. But also want to know - sometimes - that the result
                         *  was from cache. For now - return 200, but mark it with header fOptions_.fCachedResultHeader
                         *      -- LGP 2019-06-27
                         */
                        if (context.fCachedElement) {
                            // the 304's header fields update the stored response's (RFC 9111, 4.3.4), so it is fresh again by
                            // them - or by the default TTL - and stored again
                            Mapping<String, String> headers = context.fCachedElement->GetCombinedHeaders ();
                            // the stored response's Age is not its age now - the 304 brings its own, if any
                            headers.RemoveAll (
                                [] (const auto& h) { return HTTP::kHeaderNameEqualsComparer (h.fKey, HTTP::HeaderName::kAge); });
                            for (const auto& [name, value] : response->GetHeaders ().As<Mapping<String, String>> ()) {
                                if (not HTTP::kHeaderNameEqualsComparer (name, HTTP::HeaderName::kContentLength)) {
                                    headers.Add (name, value);
                                }
                            }
                            Element renewed = NewElement_ (Response{context.fCachedElement->fBody, HTTP::StatusCodes::kOK, headers});
                            if (context.fFullURI and renewed.IsCachable ()) {
                                fCache_.Add (*context.fFullURI, renewed);
                            }
                            Mapping<String, String> answerHeaders = renewed.GetCombinedHeaders ();
                            if (fOptions_.fCachedResultHeader) {
                                answerHeaders.Add (*fOptions_.fCachedResultHeader, String{});
                            }
                            *response = Response{renewed.fBody, HTTP::StatusCodes::kOK, answerHeaders, response->GetSSLResultInfo ()};
                        }
                        else {
                            DbgTrace ("Cache::OnAfterFetch::oops: unexpected NOT-MODIFIED result when nothing was in the cache"_f); // ignore...
                        }
                    }
                    catch (...) {
                        DbgTrace ("Cache::OnAfterFetch::oops(ok): {}"_f, current_exception ()); // ignore...
                    }
                } break;
                default: {
                    // ignored
                } break;
            }
        }

        virtual void ClearCache () override
        {
            fCache_.clear ();
        }

        virtual optional<Element> Lookup (const URI& url) const override
        {
            return fCache_.Lookup (url);
        }

        Options fOptions_;

        using TRAITS_ =
            LRUCacheSupport::InternallySynchronizedTraits<LRUCacheSupport::WithKeyHashTraits<LRUCacheSupport::DefaultTraits<URI, Element>>>;
        mutable LRUCache<URI, Element, TRAITS_> fCache_; // lookups DO mutate the cache so mark mutable, but safe threadwise cuz of InternallySynchronizedTraits
    };

}

/*
 ********************************************************************************
 ******************* Transfer::Cache::Element ***********************************
 ********************************************************************************
 */
Transfer::Cache::Element::Element (const Response& response)
    : fBody{response.GetData ()}
{
    const DateTime          received = DateTime::Now (); // RFC 9111's response_time - it is just in
    optional<uint32_t>      maxAge;
    bool                    expiresMalformed{false};
    optional<DateTime>      date;  // the server's clock, when it sent this
    uint32_t                age{}; // seconds caches upstream held it
    Mapping<String, String> headers = response.GetHeaders ().As<Mapping<String, String>> ();
    for (auto hi = headers.begin (); hi != headers.end ();) {
        // HTTP Date formats:
        //
        // According to https://tools.ietf.org/html/rfc7234#section-5.3
        //      The Expires value is an HTTP-date timestamp, as defined in Section 7.1.1.1 of[RFC7231].
        // From https://tools.ietf.org/html/rfc7231#section-7.1.1.1
        //      The preferred format is
        //      a fixed - length and single - zone subset of the date and time specification used by the Internet Message Format[RFC5322].
        //
        if (HTTP::kHeaderNameEqualsComparer (hi->fKey, HTTP::HeaderName::kETag)) {
            optional<HTTP::ETag> etag = HTTP::ETag::Parse (hi->fValue); // "xyz", or W/"xyz" - a weak one
            if (not etag) {
                Execution::Throw (Execution::Exception{"malformed etag"sv});
            }
            fETag       = etag->fValue;
            fETagIsWeak = etag->fWeak;
            hi          = headers.erase (hi);
        }
        else if (HTTP::kHeaderNameEqualsComparer (hi->fKey, HTTP::HeaderName::kExpires)) {
            try {
                fExpires = DateTime::Parse (hi->fValue, DateTime::kHTTPDateFormat);
            }
            catch (...) {
                // treat invalid dates as if the resource has already expired
                //fExpires = DateTime::min ();  // better but cannot convert back to date - fix stk date stuff so this works
                fExpires         = DateTime::Now ();
                expiresMalformed = true;
                DbgTrace ("Malformed expires ({}) treated as expires immediately"_f, hi->fValue);
            }
            hi = headers.erase (hi);
        }
        // Date and Age are only read here - they stay among fOtherHeaders, so an answer from the cache carries them
        else if (HTTP::kHeaderNameEqualsComparer (hi->fKey, HTTP::HeaderName::kDate)) {
            date = DateTime::ParseQuietly (hi->fValue, DateTime::kHTTPDateFormat); // one that does not parse: as if none (RFC 9110, 6.6.1)
            ++hi;
        }
        else if (HTTP::kHeaderNameEqualsComparer (hi->fKey, HTTP::HeaderName::kAge)) {
            // delta-seconds; of a list, the first; one that is not a number is ignored (RFC 9111, 5.1)
            if (auto a = Number::Parse<uint32_t> (hi->fValue.Tokenize ({','}).FirstValue (String{}).Trim (), {.fAcceptSign = false, .fSaturate = true})) {
                age = *a;
            }
            ++hi;
        }
        else if (HTTP::kHeaderNameEqualsComparer (hi->fKey, HTTP::HeaderName::kLastModified)) {
            try {
                fLastModified = DateTime::Parse (hi->fValue, DateTime::kHTTPDateFormat);
            }
            catch (...) {
                DbgTrace ("Malformed last-modified ({}) treated as ignored"_f, hi->fValue);
            }
            hi = headers.erase (hi);
        }
        else if (HTTP::kHeaderNameEqualsComparer (hi->fKey, HTTP::HeaderName::kCacheControl)) {
            fCacheControl = hi->fValue.Tokenize ({','}).Map<Set<String>> ([] (const String& d) { return d.Trim (); }); // "private, no-store"
            maxAge = HTTP::CacheControl::Parse (hi->fValue).fMaxAge; // one that is not a number reads as 0: stale, as RFC 9111 (4.2.1) encourages
            hi = headers.erase (hi);
        }
        else if (HTTP::kHeaderNameEqualsComparer (hi->fKey, HTTP::HeaderName::kContentType)) {
            fContentType = DataExchange::InternetMediaType{hi->fValue};
            hi           = headers.erase (hi);
        }
        else {
            ++hi;
        }
    }
    fOtherHeaders = headers;

    // freshness (RFC 9111, 4.2), by this machine's clock; the request's round trip (response_delay, 4.2.3) is not known here.
    // max-age, where there is one, overrides Expires (RFC 9111, 5.3)
    optional<Duration> lifetime;
    if (maxAge) {
        lifetime = Duration{chrono::seconds{*maxAge}};
    }
    else if (fExpires) {
        lifetime = expiresMalformed ? Duration{0} : *fExpires - date.value_or (received);
    }
    if (lifetime) {
        Duration sinceDate = date ? max (received - *date, Duration{0}) : Duration{0};
        fFreshUntil        = received + *lifetime - max (sinceDate, Duration{chrono::seconds{age}});
    }
}

Mapping<String, String> Transfer::Cache::Element::GetCombinedHeaders () const
{
    Mapping<String, String> result = fOtherHeaders;
    if (fETag) {
        result.Add (HTTP::HeaderName::kETag, HTTP::ETag{*fETag, fETagIsWeak}.As<String> ());
    }
    if (fExpires) {
        result.Add (HTTP::HeaderName::kExpires, fExpires->Format (DateTime::kHTTPDateFormat));
    }
    if (fLastModified) {
        result.Add (HTTP::HeaderName::kLastModified, fLastModified->Format (DateTime::kHTTPDateFormat));
    }
    if (fCacheControl) {
        function<String (const String& lhs, const String& rhs)> a = [] (const String& lhs, const String& rhs) -> String {
            if (lhs.empty ()) {
                return rhs;
            }
            StringBuilder sb;
            sb << lhs;
            sb << ","sv;
            sb << rhs;
            return sb;
        };
        result.Add (HTTP::HeaderName::kCacheControl, fCacheControl->Reduce (a).value_or (String{}));
    }
    if (fContentType) {
        result.Add (HTTP::HeaderName::kContentType, fContentType->As<String> ());
    }
    return result;
}

bool Transfer::Cache::Element::IsCachable () const
{
    static const String kNoStore_{"no-store"sv};
    if (fCacheControl) {
        return not fCacheControl->Contains (kNoStore_);
    }
    return true;
}

optional<DateTime> Transfer::Cache::Element::IsValidUntil () const
{
    // no-cache: never fresh - each use is revalidated first (RFC 9111, 5.2.2.4), whatever max-age or Expires say
    static const String kNoCache_{"no-cache"sv};
    if (fCacheControl and fCacheControl->Contains (kNoCache_)) {
        return DateTime::Now ().AddSeconds (-1);
    }
    if (fFreshUntil) {
        return *fFreshUntil;
    }
    return fExpiresDefault;
}

String Transfer::Cache::Element::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << ", ETag: "sv << fETag;
    if (fETagIsWeak) {
        sb << " (weak)"sv;
    }
    sb << ", expires: "sv << fExpires;
    sb << ", freshUntil: "sv << fFreshUntil;
    sb << ", lastModified: "sv << fLastModified;
    sb << ", cacheControl: "sv << fCacheControl;
    sb << ", contentType: "sv << fContentType;
    sb << ", otherHeaders: "sv << fOtherHeaders;
    sb << ", body: "sv << fBody;
    sb << ", expiresDefault: "sv << fExpiresDefault;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 **************************** Transfer::Cache ***********************************
 ********************************************************************************
 */
Transfer::Cache::Ptr Transfer::Cache::New (const Options& options)
{
    return Ptr{Memory::MakeSharedPtr<DefaultCacheRep_> (options)};
}
