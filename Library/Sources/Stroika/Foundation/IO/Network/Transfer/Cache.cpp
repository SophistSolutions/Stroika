/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Cache/SynchronizedLRUCache.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Cache.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;
using namespace Stroika::Foundation::Time;

using Stroika::Foundation::Cache::SynchronizedLRUCache;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace {

    struct DefaultCacheRep_ : Transfer::Cache::IRep {

        using Element        = Transfer::Cache::Element;
        using EvalContext    = Transfer::Cache::EvalContext;
        using DefaultOptions = Transfer::Cache::DefaultOptions;

        struct MyElement_ : Element {
            MyElement_ () = default;
            MyElement_ (const Response& response)
                : Element{response}
            {
            }

            virtual optional<DateTime> IsValidUntil () const override
            {
                if (auto tmp = Element::IsValidUntil ()) {
                    return *tmp;
                }
                if (fExpiresDefault) {
                    return *fExpiresDefault;
                }
                return nullopt;
            }

            nonvirtual String ToString () const
            {
                StringBuilder sb = Element::ToString ().SubString (0, -1);
                sb << ", ExpiresDefault: "sv << fExpiresDefault;
                sb << "}"sv;
                return sb;
            }
            optional<Time::DateTime> fExpiresDefault;
        };

        DefaultCacheRep_ (const DefaultOptions& options)
            : fOptions_{options}
            , fCache_{options.fCacheSize.value_or (101), 11u}
        {
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
                    if (optional<MyElement_> o = fCache_.Lookup (fullURI)) {
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
                        // @todo - UNCLEAR if we should always include both etag/iflastmodiifeid if both available? Also may want to consider MAXAGE etc.
                        // AND maybe fOptions control over how we do this?
                        // -- LGP 2019-07-10
                        bool canCheckCacheETAG = o->fETag.has_value ();
                        if (canCheckCacheETAG) {
                            context->fCachedElement = *o;
                            request->fOverrideHeaders.Add (HTTP::HeaderName::kIfNoneMatch, "\""sv + *o->fETag + "\""sv);
                            // keep going as we can combine If-None-Match/If-Modified-Since
                        }
                        bool canCheckModifiedSince = o->fLastModified.has_value ();
                        if (canCheckModifiedSince) {
                            context->fCachedElement = *o;
                            request->fOverrideHeaders.Add (HTTP::HeaderName::kIfModifiedSince,
                                                           "\""sv + o->fLastModified->Format (DateTime::kRFC1123Format) + "\""sv);
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
                            MyElement_ cacheElement{*response};
                            if (fOptions_.fDefaultResourceTTL) {
                                cacheElement.fExpiresDefault = DateTime::Now () + *fOptions_.fDefaultResourceTTL;
                            }
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
                            Mapping<String, String> headers = context.fCachedElement->GetCombinedHeaders ();
                            if (fOptions_.fCachedResultHeader) {
                                headers.Add (*fOptions_.fCachedResultHeader, String{});
                            }
                            *response = Response{context.fCachedElement->fBody, HTTP::StatusCodes::kOK, headers, response->GetSSLResultInfo ()};
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

        DefaultOptions fOptions_;

        SynchronizedLRUCache<URI, MyElement_, equal_to<URI>, hash<URI>> fCache_;
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
    Mapping<String, String> headers = response.GetHeaders ();
    for (auto hi = headers.begin (); hi != headers.end ();) {
        // HTTP Date formats:
        //
        // According to https://tools.ietf.org/html/rfc7234#section-5.3
        //      The Expires value is an HTTP-date timestamp, as defined in Section 7.1.1.1 of[RFC7231].
        // From https://tools.ietf.org/html/rfc7231#section-7.1.1.1
        //      The preferred format is
        //      a fixed - length and single - zone subset of the date and time specification used by the Internet Message Format[RFC5322].
        //
        if (hi->fKey == HTTP::HeaderName::kETag) {
            if (hi->fValue.size () < 2 or not hi->fValue.StartsWith ("\""sv) or not hi->fValue.EndsWith ("\""sv)) {
                Execution::Throw (Execution::Exception{"malformed etag"sv});
            }
            fETag = hi->fValue.SubString (1, -1);
            hi    = headers.erase (hi);
        }
        else if (hi->fKey == HTTP::HeaderName::kExpires) {
            try {
                fExpires = DateTime::Parse (hi->fValue, DateTime::kRFC1123Format);
            }
            catch (...) {
                // treat invalid dates as if the resource has already expired
                //fExpires = DateTime::min ();  // better but cannot convert back to date - fix stk date stuff so this works
                fExpires = DateTime::Now ();
                DbgTrace ("Malformed expires ({}) treated as expires immediately"_f, hi->fValue);
            }
            hi = headers.erase (hi);
        }
        else if (hi->fKey == HTTP::HeaderName::kLastModified) {
            try {
                fLastModified = DateTime::Parse (hi->fValue, DateTime::kRFC1123Format);
            }
            catch (...) {
                DbgTrace ("Malformed last-modified ({}) treated as ignored"_f, hi->fValue);
            }
            hi = headers.erase (hi);
        }
        else if (hi->fKey == HTTP::HeaderName::kCacheControl) {
            fCacheControl = Set<String>{hi->fValue.Tokenize ({','})};
            hi            = headers.erase (hi);
            static const String kMaxAgeEquals_{"max-age="sv};
            for (const String& cci : *fCacheControl) {
                if (cci.StartsWith (kMaxAgeEquals_)) {
                    fExpiresDueToMaxAge =
                        DateTime::Now () + Duration{Characters::FloatConversion::ToFloat (cci.SubString (kMaxAgeEquals_.size ()))};
                }
            }
        }
        else if (hi->fKey == HTTP::HeaderName::kContentType) {
            fContentType = DataExchange::InternetMediaType{hi->fValue};
            hi           = headers.erase (hi);
        }
        else {
            ++hi;
        }
    }
    fOtherHeaders = headers;
}

Mapping<String, String> Transfer::Cache::Element::GetCombinedHeaders () const
{
    Mapping<String, String> result = fOtherHeaders;
    if (fETag) {
        result.Add (HTTP::HeaderName::kETag, "\""sv + *fETag + "\""sv);
    }
    if (fExpires) {
        result.Add (HTTP::HeaderName::kExpires, fExpires->Format (DateTime::kRFC1123Format));
    }
    if (fLastModified) {
        result.Add (HTTP::HeaderName::kLastModified, fLastModified->Format (DateTime::kRFC1123Format));
    }
    if (fCacheControl) {
        function<String (const String& lhs, const String& rhs)> a = [] (const String& lhs, const String& rhs) -> String {
            return lhs.empty () ? rhs : (lhs + ","sv + rhs);
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
    if (fExpires) {
        return *fExpires;
    }
    if (fExpiresDueToMaxAge) {
        return *fExpiresDueToMaxAge;
    }
    static const String kNoCache_{"no-cache"sv};
    if (fCacheControl and fCacheControl->Contains (kNoCache_)) {
        return DateTime::Now ().AddSeconds (-1);
    }
    return nullopt;
}

String Transfer::Cache::Element::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << ", ETag: "sv << fETag;
    sb << ", Expires: "sv << fExpires;
    sb << ", ExpiresDueToMaxAge: "sv << fExpiresDueToMaxAge;
    sb << ", LastModified: "sv << fLastModified;
    sb << ", CacheControl: "sv << fCacheControl;
    sb << ", ContentType: "sv << fContentType;
    sb << ", OtherHeaders: "sv << fOtherHeaders;
    sb << ", Body: "sv << fBody;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 **************************** Transfer::Cache ***********************************
 ********************************************************************************
 */
Transfer::Cache::Ptr Transfer::Cache::CreateDefault ()
{
    return CreateDefault (DefaultOptions{});
}
Transfer::Cache::Ptr Transfer::Cache::CreateDefault (const DefaultOptions& options)
{
    return Ptr{make_shared<DefaultCacheRep_> (options)};
}
