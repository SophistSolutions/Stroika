/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

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
    Rep_ (const Sequence<Route>& routes, const CORSOptions& corsOptions)
        : fRoutes_{routes}
        , fAccessControlAllowCredentials_str_{true ? L"true"sv : L"false"sv} // get from options
    {
    }
    virtual void HandleFault ([[maybe_unused]] Message* m, [[maybe_unused]] const exception_ptr& e) noexcept override
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
                // @todo working on enhacing this based on configured data
                using namespace IO::Network::HTTP::HeaderName;
                header->Set (kAccessControlAllowCredentials, fAccessControlAllowCredentials_str_);
                header->Set (kAccessControlAllowHeaders, fAccessControlAllowHeaders_str_);
                header->Set (kAccessControlAllowMethods, String::Join (*o));
                header->Set (kAccessControlMaxAge, L"86400"sv);
            });
            response.SetStatus (IO::Network::HTTP::StatusCodes::kNoContent);
        }
        else {
            DbgTrace (L"Router 404: (...url=%s)", Characters::ToString (message->GetRequestURL ()).c_str ());
            Execution::Throw (ClientErrorException{HTTP::StatusCodes::kNotFound});
        }
    }

    const String          fAccessControlAllowCredentials_str_;
    const String          fAccessControlAllowHeaders_str_ = L"Accept, Access-Control-Allow-Origin, Authorization, Cache-Control, Content-Type, Connection, Pragma, X-Requested-With"sv;
    const Sequence<Route> fRoutes_; // no need for synchronization cuz constant - just set on construction
};

/*
 ********************************************************************************
 *************************** WebServer::Router **********************************
 ********************************************************************************
 */
Router::Router (const Sequence<Route>& routes, const CORSOptions& corsOptions)
    : inherited{make_shared<Rep_> (routes, corsOptions)}
{
}

optional<RequestHandler> Router::Lookup (const Request& request) const
{
    return _GetRep<Rep_> ().Lookup_ (request, nullptr);
}
