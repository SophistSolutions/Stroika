/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Cache/SynchronizedLRUCache.h"
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

namespace {

    struct DefaultCacheRep_ : Transfer::Cache::Rep {

        using Element     = Transfer::Cache::Element;
        using EvalContext = Transfer::Cache::EvalContext;

        virtual optional<Response> OnBeforeFetch (EvalContext* context, const URI& schemeAndAuthority, Request* request) override
        {
            if (request->fMethod == HTTP::Methods::kGet) {
                URI fullURI       = schemeAndAuthority.Combine (request->fAuthorityRelativeURL);
                context->fFullURI = fullURI;
                if (optional<Element> o = fCache_.Lookup (fullURI)) {
                    // check if cacheable - and either return directly or
                    bool canReturnDirectly = false;
                    if (canReturnDirectly) {
                        // fill in response and return short-circuiting
                        return Response{o->fBody, HTTP::StatusCodes::kOK, o->GetCombinedHeaders ()};
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
            // In this case, no caching is possible - nothing todo
            return nullopt;
        }

        virtual void OnAfterFetch (const EvalContext& context, Request* request, Response* response) override
        {
            RequireNotNull (request);
            RequireNotNull (response);
            switch (response->GetStatus ()) {
                case HTTP::StatusCodes::kOK: {
                    // @todo add result to cache
                    // but must look at headers closely first - LIKE PRAGMA NO-CACHE!
                    if (context.fFullURI) {
                        fCache_.Add (*context.fFullURI, Element{*response});
                    }
                } break;
                case HTTP::StatusCodes::kNotModified: {
                    // @todo replaces response value with right answer on 304
                    // lookup cache value and return it - updating any needed http headers stored in cache
                    if (context.fCachedElement) {
                        *response = Response{context.fCachedElement->fBody, HTTP::StatusCodes::kOK, context.fCachedElement->GetCombinedHeaders (), response->GetSSLResultInfo ()};
                    }
                    else {
                        Execution::Throw (Execution::Exception (L"unexpected NOT-MODIFIED result when nothing was in the cache"sv));
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

        // we want to use hash stuff but then need hash<URI>
        ///pair<string, string>{}, 3, 10, hash<string>{}
        SynchronizedLRUCache<URI, Element> fCache_{101};
    };

}

/*
 ********************************************************************************
 ******************* Transfer::Cache::Element ***********************************
 ********************************************************************************
 */
Transfer::Cache::Element::Element (const Response& response)
{
    Mapping<String, String> headers = response.GetHeaders ();
    for (auto i = headers.begin (); i != headers.end (); ++i) {
        if (i->fKey == HTTP::HeaderName::kETag) {
            if (i->fValue.size () < 2 or not i->fValue.StartsWith (L"\"") or not i->fValue.EndsWith (L"\"")) {
                Execution::Throw (Execution::Exception (L"malformed etag"sv));
            }
            fETag = i->fValue.SubString (1, -1);
            headers.erase (i);
        }
        else if (i->fKey == HTTP::HeaderName::kExpires) {
            fExpires = DateTime::Parse (i->fValue, DateTime::ParseFormat::eRFC1123);
            headers.erase (i);
        }
        else if (i->fKey == HTTP::HeaderName::kLastModified) {
            fLastModified = DateTime::Parse (i->fValue, DateTime::ParseFormat::eRFC1123);
            headers.erase (i);
        }
        else if (i->fKey == HTTP::HeaderName::kCacheControl) {
            fCacheControl = Set<String>{i->fValue.Tokenize (Set<Character>{','})};
            headers.erase (i);
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

String Transfer::Cache::Element::ToString () const
{
    return Characters::ToString (GetCombinedHeaders ());
}

/*
 ********************************************************************************
 **************************** Transfer::Cache ***********************************
 ********************************************************************************
 */
Transfer::Cache::Ptr Transfer::Cache::CreateDefault ()
{
    return Ptr{make_shared<DefaultCacheRep_> ()};
}
