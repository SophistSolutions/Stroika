/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/String_Constant.h"
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
 ********************************************************************************
 ************************* WebServer::Router::Rep_ ******************************
 ********************************************************************************
 */

struct Router::Rep_ : Interceptor::_IRep {
    Rep_ (const Sequence<Route>& routes)
        : fRoutes_ (routes)
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
        else {
            if (optional<Set<String>> o = GetAllowedMethodsForRequest_ (*m->PeekRequest ())) {
                // From 10.4.6 405 Method Not Allowed
                //      The method specified in the Request-Line is not allowed for the resource identified by the Request-URI.
                //      The response MUST include an Allow header containing a list of valid methods for the requested resource.
                Assert (not o->empty ());
                StringBuilder res;
                o->Apply ([&res] (const String& i) { if (not res.empty ()) { res += L", "; } res += i; });
                m->PeekResponse ()->AddHeader (HTTP::HeaderName::kAllow, res.str ());
                Execution::Throw (ClientErrorException (HTTP::StatusCodes::kMethodNotAllowed));
            }
            else {
                DbgTrace (L"Router 404: (...url=%s)", Characters::ToString (m->GetRequestURL ()).c_str ());
                Execution::Throw (ClientErrorException (HTTP::StatusCodes::kNotFound));
            }
        }
    }
    optional<RequestHandler> Lookup_ (const Request& request, Sequence<String>* matches) const
    {
        String method = request.GetHTTPMethod ();
        URI    url    = request.GetURL ();

        String hostRelPath;
        try {
            hostRelPath = url.GetAbsPath<String> ().SubString (1); // According to https://tools.ietf.org/html/rfc2616#section-5.1.2 - the URI must be abs_path
        }
        catch (...) {
            Execution::Throw (ClientErrorException (HTTP::StatusCodes::kBadRequest, L"request URI requires an absolute path"sv));
        }

        // We interpret routes as matching against a relative path from the root
        for (Route r : fRoutes_) {
            if (r.fVerbMatch_ and not method.Match (*r.fVerbMatch_)) {
                continue;
            }
            if (matches == nullptr) {
                if (r.fPathMatch_ and not hostRelPath.Match (*r.fPathMatch_)) {
                    continue;
                }
            }
            else {
                if (r.fPathMatch_ and not hostRelPath.Match (*r.fPathMatch_, matches)) {
                    continue;
                }
            }
            if (r.fRequestMatch_ and not(*r.fRequestMatch_) (request)) {
                continue;
            }
            return r.fHandler_;
        }
        return nullopt;
    }
    optional<Set<String>> GetAllowedMethodsForRequest_ (const Request& request) const
    {
        URI    url = request.GetURL ();
        String hostRelPath;
        try {
            hostRelPath = url.GetAbsPath<String> ().SubString (1); // According to https://tools.ietf.org/html/rfc2616#section-5.1.2 - the URI must be abs_path
        }
        catch (...) {
            return nullopt;
        }
        static const Set<String> kMethods2Try_{HTTP::Methods::kGet, HTTP::Methods::kPut, HTTP::Methods::kOptions, HTTP::Methods::kDelete, HTTP::Methods::kPost};
        Set<String>              methods;
        for (String method : kMethods2Try_) {
            for (Route r : fRoutes_) {
                if (r.fVerbMatch_ and not method.Match (*r.fVerbMatch_)) {
                    continue;
                }
                if (r.fPathMatch_ and not hostRelPath.Match (*r.fPathMatch_)) {
                    continue;
                }
                if (r.fRequestMatch_ and not(*r.fRequestMatch_) (request)) {
                    continue;
                }
                methods.Add (method);
            }
        }
        return methods.empty () ? nullopt : optional<Set<String>>{methods};
    }

    const Sequence<Route> fRoutes_; // no need for synchronization cuz constant - just set on construction
};

/*
 ********************************************************************************
 *************************** WebServer::Router **********************************
 ********************************************************************************
 */
Router::Router (const Sequence<Route>& routes)
    : inherited (make_shared<Rep_> (routes))
{
}

optional<RequestHandler> Router::Lookup (const Request& request) const
{
    return _GetRep<Rep_> ().Lookup_ (request, nullptr);
}
