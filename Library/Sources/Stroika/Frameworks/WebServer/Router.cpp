/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/Format.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Execution/Exceptions.h"
#include "../../Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/Methods.h"

#include "Router.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

using HTTP::ClientErrorException;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace {
    String ExtractHostRelPath_ (const URI& url)
    {
        try {
            return url.GetAbsPath<String> ().SubString (1); // According to https://tools.ietf.org/html/rfc2616#section-5.1.2 - the URI must be abs_path
        }
        catch (...) {
            Execution::Throw (ClientErrorException{HTTP::StatusCodes::kBadRequest, L"request URI requires an absolute path"sv});
        }
    }
}

/*
 ********************************************************************************
 ******************************* Route::Matches *********************************
 ********************************************************************************
 */
bool Route::Matches (const Request& request, Sequence<String>* pathRegExpMatches) const
{
    return Matches (request.GetHTTPMethod (), ExtractHostRelPath_ (request.GetURL ()), request, pathRegExpMatches);
}
bool Route::Matches (const String& method, const String& hostRelPath, const Request& request, Sequence<String>* pathRegExpMatches) const
{
    if (fVerbAndPathMatch_) {
        if (not method.Match (fVerbAndPathMatch_->first)) {
            return false;
        }
        return (pathRegExpMatches == nullptr)
                   ? hostRelPath.Match (fVerbAndPathMatch_->second)
                   : hostRelPath.Match (fVerbAndPathMatch_->second, pathRegExpMatches);
    }
    else if (fRequestMatch_) {
        return (*fRequestMatch_) (method, hostRelPath, request);
    }
    else {
        AssertNotReached ();
        return false;
    }
}

/*
 ********************************************************************************
 ************************* WebServer::Router::Rep_ ******************************
 ********************************************************************************
 */

struct Router::Rep_ : Interceptor::_IRep {
    static const optional<Set<String>> MapStartToNullOpt_ (const optional<Set<String>>& o)
    {
        // internally we treat missing as wildcard but caller may not, so map
        optional<Set<String>> m = o;
        if (m and m->Contains (CORSOptions::kAccessControlAllowOriginWildcard)) {
            m = nullopt;
        }
        return m;
    }
    // OLD LISTS: fAccessControlAllowHeadersValue_{L"Accept, Access-Control-Allow-Origin, Authorization, Cache-Control, Content-Type, Connection, Pragma, X-Requested-With"sv}
    //  OLD LISTS: static const inline Set<String> kBasicHeadersAlwaysAllowed_{L"Allow", L"Authorization", IO::Network::HTTP::HeaderName::kContentType}; // fogire out
    static const inline Set<String> kBasicHeadersAlwaysAllowed_{IO::Network::HTTP::HeaderName::kContentType}; // @todo figure out what logically goes here?
    // requires all optional values filled in on corsOptions
    Rep_ (const Sequence<Route>& routes, const CORSOptions& filledInCORSOptions)
        : fRoutes_{routes}
        , fAllowedOrigins_{MapStartToNullOpt_ (filledInCORSOptions.fAllowedOrigins)}
        , fAccessControlAllowCredentialsValue_{*filledInCORSOptions.fAllowCredentials ? L"true"sv : L"false"sv}
        , fAccessControlAllowHeadersValue_{String::Join (kBasicHeadersAlwaysAllowed_ + *filledInCORSOptions.fAllowedExtraHTTPHeaders)}
        , fAccessControlMaxAgeValue_{Characters::Format (L"%d", *filledInCORSOptions.fAccessControlMaxAge)}
    {
    }
    virtual void HandleMessage (Message* m) override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Router::Rep_::HandleMessage", L"(...method=%s,url=%s)", m->GetRequestHTTPMethod ().c_str (), Characters::ToString (m->GetRequestURL ()).c_str ()};
#endif
        Sequence<String>         matches;
        optional<RequestHandler> handler = Lookup_ (*m->PeekRequest (), &matches);
        if (handler) {
            (*handler) (m, matches);
            HandleCORSInNormallyHandledMessage_ (*m->PeekRequest (), *m->PeekResponse ());
        }
        else if (m->PeekRequest ()->GetHTTPMethod () == HTTP::Methods::kOptions) {
            Handle_OPTIONS_ (m);
        }
        else {
            if (optional<Set<String>> o = GetAllowedMethodsForRequest_ (*m->PeekRequest ())) {
                // From 10.4.6 405 Method Not Allowed
                //      The method specified in the Request-Line is not allowed for the resource identified by the Request-URI.
                //      The response MUST include an Allow header containing a list of valid methods for the requested resource.
                Assert (not o->empty ());
                m->PeekResponse ()->UpdateHeader ([&] (auto* header) { RequireNotNull (header); header->pAllow = o; });
                Execution::Throw (ClientErrorException{HTTP::StatusCodes::kMethodNotAllowed});
            }
            else {
                DbgTrace (L"Router 404: (...url=%s)", Characters::ToString (m->GetRequestURL ()).c_str ());
                Execution::Throw (ClientErrorException{HTTP::StatusCodes::kNotFound});
            }
        }
    }
    nonvirtual void HandleCORSInNormallyHandledMessage_ (const Request& request, Response& response)
    {
        /*
         *  Origin and Access-Control-Allow-Origin
         *      documented here:    http://www.w3.org/TR/cors/#http-responses
         * 
         *  IF CORS is being used, the request will contain an Origin header, and will expect a
         *  corresponding Access-Control-Allow-Origin response header. If the response header is missing
         *  that implies a CORS failure (but if we have no header in the request - presumably no matter)
         */
        optional<String> allowedOrigin;
        if (auto origin = request.GetHeaders ().pOrigin ()) {
            if (fAllowedOrigins_.has_value ()) {
                // see https://fetch.spec.whatwg.org/#http-origin for hints about how to compare - not sure
                // may need to be more flexible about how we compare, but for now a good approximation... &&& @todo docs above link say how to compar
                String originStr = origin->ToString ();
                if (fAllowedOrigins_->Contains (originStr)) {
                    allowedOrigin = originStr;
                }
            }
            else {
                allowedOrigin = CORSOptions::kAccessControlAllowOriginWildcard;
            }
        }
        if (allowedOrigin) {
            response.UpdateHeader ([&] (auto* header) {
                RequireNotNull (header);
                header->pAccessControlAllowOrigin = allowedOrigin;
                // @todo see https://fetch.spec.whatwg.org/#cors-protocol-and-http-caches to see if we need to add Vary response
            });
        }
    }
    nonvirtual optional<RequestHandler> Lookup_ (const String& method, const String& hostRelPath, const Request& request, Sequence<String>* matches) const
    {
        for (Route r : fRoutes_) {
            if (r.Matches (method, hostRelPath, request, matches)) {
                return r.fHandler_;
            }
        }
        return nullopt;
    }
    nonvirtual optional<RequestHandler> Lookup_ (const Request& request, Sequence<String>* matches) const
    {
        return Lookup_ (request.GetHTTPMethod (), ExtractHostRelPath_ (request.GetURL ()), request, matches);
    }
    nonvirtual optional<Set<String>> GetAllowedMethodsForRequest_ (const Request& request) const
    {
        String                   hostRelPath = ExtractHostRelPath_ (request.GetURL ());
        static const Set<String> kMethods2Try_{HTTP::Methods::kGet, HTTP::Methods::kPut, HTTP::Methods::kOptions, HTTP::Methods::kDelete, HTTP::Methods::kPost};
        Set<String>              methods;
        for (String method : kMethods2Try_) {
            for (Route r : fRoutes_) {
                if (r.Matches (method, hostRelPath, request)) {
                    methods.Add (method);
                }
            }
        }
        return methods.empty () ? nullopt : optional<Set<String>>{methods};
    }
    nonvirtual void Handle_OPTIONS_ (Message* message)
    {
        Request&  request  = *message->PeekRequest ();
        Response& response = *message->PeekResponse ();
        auto      o        = GetAllowedMethodsForRequest_ (request);
        if (o) {
            response.UpdateHeader ([this, &o] (auto* header) {
                RequireNotNull (header);
                header->Set (HeaderName::kAccessControlAllowCredentials, fAccessControlAllowCredentialsValue_);
                header->Set (HeaderName::kAccessControlAllowHeaders, fAccessControlAllowHeadersValue_);
                header->Set (HeaderName::kAccessControlAllowMethods, String::Join (*o));
                header->Set (HeaderName::kAccessControlMaxAge, fAccessControlMaxAgeValue_);
            });
            HandleCORSInNormallyHandledMessage_ (request, response);    // include access-origin-header
            response.SetStatus (IO::Network::HTTP::StatusCodes::kNoContent);
        }
        else {
            DbgTrace (L"Router 404: (...url=%s)", Characters::ToString (message->GetRequestURL ()).c_str ());
            Execution::Throw (ClientErrorException{HTTP::StatusCodes::kNotFound});
        }
    }

    const optional<Set<String>> fAllowedOrigins_; // missing <==> '*'
    const String                fAccessControlAllowCredentialsValue_;
    const String                fAccessControlAllowHeadersValue_;
    const String                fAccessControlMaxAgeValue_;
    const Sequence<Route>       fRoutes_; // no need for synchronization cuz constant - just set on construction
};

/*
 ********************************************************************************
 *************************** WebServer::Router **********************************
 ********************************************************************************
 */
namespace {
    CORSOptions FillIn_ (CORSOptions corsOptions)
    {
        corsOptions.fAllowCredentials        = Memory::NullCoalesce (corsOptions.fAllowCredentials, kDefault_CORSOptions.fAllowCredentials);
        corsOptions.fAccessControlMaxAge     = Memory::NullCoalesce (corsOptions.fAccessControlMaxAge, kDefault_CORSOptions.fAccessControlMaxAge);
        corsOptions.fAllowedExtraHTTPHeaders = Memory::NullCoalesce (corsOptions.fAllowedExtraHTTPHeaders, kDefault_CORSOptions.fAllowedExtraHTTPHeaders);
        corsOptions.fAllowedOrigins          = Memory::NullCoalesce (corsOptions.fAllowedOrigins, kDefault_CORSOptions.fAllowedOrigins);
        return corsOptions;
    }
}
Router::Router (const Sequence<Route>& routes, const CORSOptions& corsOptions)
    : inherited{make_shared<Rep_> (routes, FillIn_ (corsOptions))}
{
}

optional<RequestHandler> Router::Lookup (const Request& request) const
{
    return _GetRep<Rep_> ().Lookup_ (request, nullptr);
}
