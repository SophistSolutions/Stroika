/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Cache/SynchronizedLRUCache.h"
#include "../../../Characters/ToString.h"
#include "../../../Debug/Trace.h"
#include "../../../Time/Duration.h"

#include "../HTTP/Headers.h"
#include "../HTTP/Methods.h"

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
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {

    struct DefaultCacheRep_ : Transfer::Cache::Rep {

        using Element        = Transfer::Cache::Element;
        using EvalContext    = Transfer::Cache::EvalContext;
        using DefaultOptions = Transfer::Cache::DefaultOptions;

        struct MyElement_ : Element {
            MyElement_ () = default;
            MyElement_ (const Response& response)
                : Element (response)
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
                sb += L", fExpiresDefault: " + Characters::ToString (fExpiresDefault);
                sb += L"}";
                return sb.str ();
            }
            optional<Time::DateTime> fExpiresDefault;
        };

        DefaultCacheRep_ (const DefaultOptions& options)
            : fOptions_{options}
            , fCache_{options.fCacheSize.value_or (101)}
        {
        }

        virtual optional<Response> OnBeforeFetch (EvalContext* context, const URI& schemeAndAuthority, Request* request) noexcept override
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"DefaultCacheRep_::OnBeforeFetch", L"schemeAndAuthority=%s", Characters::ToString (schemeAndAuthority).c_str ())};
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
                        bool canCheckCacheETAG = false;
                        if (canCheckCacheETAG) {
                            // @todo - modify request to check etag
                            context->fCachedElement = *o;
                            return nullopt; // keep processing with request changes
                        }
                        bool canCheckModifiedSince = false;
                        if (canCheckModifiedSince) {
                            //  @todo - modify request to check last modfieid for maxage
                            context->fCachedElement = *o;
                            return nullopt; // keep processing with request changes
                        }
                    }
                }
                catch (...) {
                    DbgTrace (L"Cache::OnBeforeFetch::oops: %s", Characters::ToString (current_exception ()).c_str ()); // ignore...
                }
            }
            // In this case, no caching is possible - nothing todo
            return nullopt;
        }

        virtual void OnAfterFetch (const EvalContext& context, Response* response) noexcept override
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"DefaultCacheRep_::OnAfterFetch", L"context.fFullURI=%s", Characters::ToString (context.fFullURI).c_str ())};
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
                                //DbgTrace (L"Add2Cache: uri=%s, cacheElement=%s", Characters::ToString (*context.fFullURI).c_str (), Characters::ToString (cacheElement).c_str ());
                                fCache_.Add (*context.fFullURI, cacheElement);
                            }
                        }
                        catch (...) {
                            DbgTrace (L"Cache::OnAfterFetch::oops(ok): %s", Characters::ToString (current_exception ()).c_str ()); // ignore...
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
                            DbgTrace (L"Cache::OnAfterFetch::oops: unexpected NOT-MODIFIED result when nothing was in the cache"); // ignore...
                        }
                    }
                    catch (...) {
                        DbgTrace (L"Cache::OnAfterFetch::oops(ok): %s", Characters::ToString (current_exception ()).c_str ()); // ignore...
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

        // we want to use hash stuff but then need hash<URI>
        ///pair<string, string>{}, 3, 10, hash<string>{}
        SynchronizedLRUCache<URI, MyElement_> fCache_;
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
    for (auto i = headers.begin (); i != headers.end (); ++i) {
        // HTTP Date formats:
        //
        // According to https://tools.ietf.org/html/rfc7234#section-5.3
        //      The Expires value is an HTTP-date timestamp, as defined in Section 7.1.1.1 of[RFC7231].
        // From https://tools.ietf.org/html/rfc7231#section-7.1.1.1
        //      The preferred format is
        //      a fixed - length and single - zone subset of the date and time specification used by the Internet Message Format[RFC5322].
        //
        if (i->fKey == HTTP::HeaderName::kETag) {
            if (i->fValue.size () < 2 or not i->fValue.StartsWith (L"\"") or not i->fValue.EndsWith (L"\"")) {
                Execution::Throw (Execution::Exception (L"malformed etag"sv));
            }
            fETag = i->fValue.SubString (1, -1);
            headers.erase (i);
        }
        else if (i->fKey == HTTP::HeaderName::kExpires) {
            try {
                fExpires = DateTime::Parse (i->fValue, DateTime::ParseFormat::eRFC1123);
            }
            catch (...) {
                // treat invalid dates as if the resource has already exipred
                //fExpires = DateTime::min ();  // better but cannot convert back to date - fix stk date stuff so this works
                fExpires = DateTime::Now ();
                DbgTrace (L"Malformed expires (%s) treated as expires immediately", Characters::ToString (i->fValue).c_str ());
            }
            headers.erase (i);
        }
        else if (i->fKey == HTTP::HeaderName::kLastModified) {
            try {
                fLastModified = DateTime::Parse (i->fValue, DateTime::ParseFormat::eRFC1123);
            }
            catch (...) {
                DbgTrace (L"Malformed last-modfied (%s) treated as ignored", Characters::ToString (i->fValue).c_str ());
            }
            headers.erase (i);
        }
        else if (i->fKey == HTTP::HeaderName::kCacheControl) {
            fCacheControl = Set<String>{i->fValue.Tokenize (Set<Character>{','})};
            headers.erase (i);
            static const String kMaxAgeEquals_{L"max-age="sv};
            for (String cci : *fCacheControl) {
                if (cci.StartsWith (kMaxAgeEquals_)) {
                    fExpiresDueToMaxAge = DateTime::Now () + Duration{Characters::String2Float (cci.SubString (kMaxAgeEquals_.size ()))};
                }
            }
        }
        else if (i->fKey == HTTP::HeaderName::kContentType) {
            fContentType = DataExchange::InternetMediaType{i->fValue};
            headers.erase (i);
        }
    }
    fOtherHeaders = headers;
}

Mapping<String, String> Transfer::Cache::Element::GetCombinedHeaders () const
{
    Mapping<String, String> result = fOtherHeaders;
    if (fETag) {
        result.Add (HTTP::HeaderName::kETag, L"\"" + *fETag + L"\"");
    }
    if (fExpires) {
        result.Add (HTTP::HeaderName::kExpires, fExpires->Format (DateTime::PrintFormat::eRFC1123));
    }
    if (fLastModified) {
        result.Add (HTTP::HeaderName::kLastModified, fLastModified->Format (DateTime::PrintFormat::eRFC1123));
    }
    if (fCacheControl) {
        function<String (const String& lhs, const String& rhs)> a = [] (const String& lhs, const String& rhs) -> String { return lhs.empty () ? rhs : (lhs + L"," + rhs); };
        result.Add (HTTP::HeaderName::kCacheControl, fCacheControl->Accumulate (a).value_or (String{}));
    }
    if (fContentType) {
        result.Add (HTTP::HeaderName::kContentType, fContentType->As<String> ());
    }
    return result;
}

bool Transfer::Cache::Element::IsCachable () const
{
    static const String kNoStore_{L"no-store"sv};
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
    static const String kNoCache_{L"no-cache"sv};
    if (fCacheControl and fCacheControl->Contains (kNoCache_)) {
        return DateTime::Now ().AddSeconds (-1);
    }
    return nullopt;
}

String Transfer::Cache::Element::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L", ETag: " + Characters::ToString (fETag);
    sb += L", Expires: " + Characters::ToString (fExpires);
    sb += L", ExpiresDueToMaxAge: " + Characters::ToString (fExpiresDueToMaxAge);
    sb += L", LastModified: " + Characters::ToString (fLastModified);
    sb += L", CacheControl: " + Characters::ToString (fCacheControl);
    sb += L", ContentType: " + Characters::ToString (fContentType);
    sb += L", OtherHeaders: " + Characters::ToString (fOtherHeaders);
    sb += L", Body: " + Characters::ToString (fBody);
    sb += L"}";
    return sb.str ();
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
