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

/*
 *  IMPLEMENTATION HINTS:
 *      A good place to look for a mature implementation of similar logic would be
 *          https://github.com/apache/cxf/blob/master/rt/rs/security/cors/src/main/java/org/apache/cxf/rs/security/cors/CrossOriginResourceSharingFilter.java
 */

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
    return Matches (request.httpMethod, ExtractHostRelPath_ (request.url ()), request, pathRegExpMatches);
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
        if (m and m->Contains (CORSOptions::kAccessControlWildcard)) {
            m = nullopt;
        }
        if (m) {
            m = Set<String>{HTTP::kHeaderNameEqualsComparer, *m};
        }
        return m;
    }
    // requires all optional values filled in on corsOptions
    Rep_ (const Sequence<Route>& routes, const CORSOptions& filledInCORSOptions)
        : fAllowedOrigins_{MapStartToNullOpt_ (filledInCORSOptions.fAllowedOrigins)}
        , fAllowedHeaders_{MapStartToNullOpt_ (filledInCORSOptions.fAllowedHeaders)}
        , fAccessControlAllowCredentialsValue_{*filledInCORSOptions.fAllowCredentials ? L"true"sv : L"false"sv}
        , fAccessControlMaxAgeValue_{Characters::Format (L"%d", *filledInCORSOptions.fAccessControlMaxAge)}
        , fRoutes_{routes}
    {
    }
    virtual void HandleMessage (Message* m) const override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Router::Rep_::HandleMessage", L"(...method=%s,url=%s)", m->GetRequestHTTPMethod ().c_str (), Characters::ToString (m->GetRequestURL ()).c_str ()};
#endif
        Sequence<String>         matches;
        optional<RequestHandler> handler = Lookup_ (m->request (), &matches);
        if (handler) {
            Handle_GET_ (m, matches, *handler);
        }
        else if (m->request ().httpMethod () == HTTP::Methods::kHead and Handle_HEAD_ (m)) {
            // handled
        }
        else if (m->request ().httpMethod () == HTTP::Methods::kOptions) {
            Handle_OPTIONS_ (m);
        }
        else {
            if (optional<Set<String>> o = GetAllowedMethodsForRequest_ (m->request ())) {
                // From 10.4.6 405 Method Not Allowed
                //      The method specified in the Request-Line is not allowed for the resource identified by the Request-URI.
                //      The response MUST include an Allow header containing a list of valid methods for the requested resource.
                Assert (not o->empty ());
                m->rwResponse ().rwHeaders ().allow = o;
                Execution::Throw (ClientErrorException{HTTP::StatusCodes::kMethodNotAllowed});
            }
            else {
                DbgTrace (L"Router 404: (...url=%s)", Characters::ToString (m->request ().url ()).c_str ());
                Execution::Throw (ClientErrorException{HTTP::StatusCodes::kNotFound});
            }
        }
    }
    nonvirtual void HandleCORSInNormallyHandledMessage_ (const Request& request, Response& response) const
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
        if (auto origin = request.headers ().origin ()) {
            if (fAllowedOrigins_.has_value ()) {
                // see https://fetch.spec.whatwg.org/#http-origin for hints about how to compare - not sure
                // may need to be more flexible about how we compare, but for now a good approximation... &&& @todo docs above link say how to compar
                String originStr = origin->ToString ();
                if (fAllowedOrigins_->Contains (originStr)) {
                    allowedOrigin = originStr;
                }
            }
            else {
                allowedOrigin = CORSOptions::kAccessControlWildcard;
            }
        }
        if (allowedOrigin) {
            response.rwHeaders ().accessControlAllowOrigin = allowedOrigin;
            if (fAllowedOrigins_) {
                // see https://fetch.spec.whatwg.org/#cors-protocol-and-http-caches to see why we need to add Vary response
                // if response depends on origin (so not '*')
                response.rwHeaders ().vary = Memory::NullCoalesce (response.headers ().vary ()) + String{HTTP::HeaderName::kOrigin};
            }
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
        return Lookup_ (request.httpMethod, ExtractHostRelPath_ (request.url), request, matches);
    }
    nonvirtual optional<Set<String>> GetAllowedMethodsForRequest_ (const Request& request) const
    {
        String                   hostRelPath = ExtractHostRelPath_ (request.url);
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
    nonvirtual void Handle_GET_ (Message* message, const Sequence<String>& matches, const RequestHandler& handler) const
    {
        const Request& request  = message->request ();
        Response&      response = message->rwResponse ();
        HandleCORSInNormallyHandledMessage_ (request, response);
        (handler) (message, matches);
    }
    nonvirtual bool Handle_HEAD_ (Message* message) const
    {
        const Request&   request  = message->request ();
        Response&        response = message->rwResponse ();
        Sequence<String> matches;
        if (optional<RequestHandler> handler = Lookup_ (HTTP::Methods::kGet, ExtractHostRelPath_ (request.url ()), request, &matches)) {
            // do someting to response so 'in HEAD mode' and won't write
            response.EnterHeadMode ();
            HandleCORSInNormallyHandledMessage_ (request, response);
            (*handler) (message, matches);
            return true;
        }
        return false;
    }
    nonvirtual void Handle_OPTIONS_ (Message* message) const
    {
        const Request& request  = message->request ();
        Response&      response = message->rwResponse ();
        // @todo note - This ignores - Access-Control-Request-Method - not sure how we are expected to use it?
        auto o = GetAllowedMethodsForRequest_ (request);
        if (o) {
            {
                auto& responseHeaders = response.rwHeaders ();
                responseHeaders.Set (HTTP::HeaderName::kAccessControlAllowCredentials, fAccessControlAllowCredentialsValue_);
                if (auto accessControlRequestHeaders = request.headers ().LookupOne (HTTP::HeaderName::kAccessControlRequestHeaders)) {
                    if (fAllowedHeaders_) {
                        // intersect requested headers with those configured to permit
                        Iterable<String> requestAccessHeaders = accessControlRequestHeaders->Tokenize ({','});
                        auto             r                    = fAllowedHeaders_->Intersection (requestAccessHeaders);
                        if (r.empty ()) {
                            responseHeaders.Set (HTTP::HeaderName::kAccessControlAllowHeaders, String::Join (r));
                        }
                    }
                    else {
                        responseHeaders.Set (HTTP::HeaderName::kAccessControlAllowHeaders, *accessControlRequestHeaders);
                    }
                }
                responseHeaders.Set (HTTP::HeaderName::kAccessControlAllowMethods, String::Join (*o));
                responseHeaders.Set (HTTP::HeaderName::kAccessControlMaxAge, fAccessControlMaxAgeValue_);
            }
            HandleCORSInNormallyHandledMessage_ (request, response); // include access-origin-header
            response.status = HTTP::StatusCodes::kNoContent;
        }
        else {
            DbgTrace (L"Router 404: (...url=%s)", Characters::ToString (message->request ().url ()).c_str ());
            Execution::Throw (ClientErrorException{HTTP::StatusCodes::kNotFound});
        }
    }

    const optional<Set<String>> fAllowedOrigins_; // missing <==> '*'
    const optional<Set<String>> fAllowedHeaders_; // missing <==> '*'
    const String                fAccessControlAllowCredentialsValue_;
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
        corsOptions.fAllowCredentials    = Memory::NullCoalesce (corsOptions.fAllowCredentials, kDefault_CORSOptions.fAllowCredentials);
        corsOptions.fAccessControlMaxAge = Memory::NullCoalesce (corsOptions.fAccessControlMaxAge, kDefault_CORSOptions.fAccessControlMaxAge);
        corsOptions.fAllowedHeaders      = Memory::NullCoalesce (corsOptions.fAllowedHeaders, kDefault_CORSOptions.fAllowedHeaders);
        corsOptions.fAllowedOrigins      = Memory::NullCoalesce (corsOptions.fAllowedOrigins, kDefault_CORSOptions.fAllowedOrigins);
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
