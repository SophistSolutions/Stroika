/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/String_Constant.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/IO/Network/HTTP/Exception.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"

#include "Router.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

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
        Debug::TraceContextBumper ctx{L"Router::Rep_::HandleMessage", L"(...url=%s)", Characters::ToString (m->GetRequestURL ()).c_str ()};
#endif
        Optional<RequestHandler> handler = Lookup_ (*m->PeekRequest ());
        if (handler) {
            (*handler) (m);
        }
        else {
            if (Optional<Set<String>> o = GetAllowedMethodsForRequest_ (*m->PeekRequest ())) {
                // From 10.4.6 405 Method Not Allowed
                //      The method specified in the Request-Line is not allowed for the resource identified by the Request-URI.
                //      The response MUST include an Allow header containing a list of valid methods for the requested resource.
                Assert (not o->empty ());
                StringBuilder res;
                o->Apply ([&res](const String& i) { if (not res.empty ()) { res += L", "; } res += i; });
                m->PeekResponse ()->AddHeader (IO::Network::HTTP::HeaderName::kAllow, res.str ());
                Execution::Throw (IO::Network::HTTP::Exception (IO::Network::HTTP::StatusCodes::kMethodNotAllowed));
            }
            else {
                DbgTrace (L"Router 404: (...url=%s)", Characters::ToString (m->GetRequestURL ()).c_str ());
                Execution::Throw (IO::Network::HTTP::Exception (IO::Network::HTTP::StatusCodes::kNotFound));
            }
        }
    }
    Optional<RequestHandler> Lookup_ (const Request& request) const
    {
        String method      = request.GetHTTPMethod ();
        URL    url         = request.GetURL ();
        String hostRelPath = url.GetHostRelativePath ();
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
            return r.fHandler_;
        }
        return Optional<RequestHandler>{};
    }
    Optional<Set<String>> GetAllowedMethodsForRequest_ (const Request& request) const
    {
        URL                      url         = request.GetURL ();
        String                   hostRelPath = url.GetHostRelativePath ();
        static const Set<String> kMethods2Try_{String_Constant{L"GET"}, String_Constant{L"PUT"}, String_Constant{L"OPTIONS"}, String_Constant{L"DELETE"}, String_Constant{L"POST"}};
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
        return methods.empty () ? Optional<Set<String>>{} : Optional<Set<String>>{methods};
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

Optional<RequestHandler> Router::Lookup (const Request& request) const
{
    return _GetRep<Rep_> ().Lookup_ (request);
}
