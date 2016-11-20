/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/ToString.h"
#include    "../../Foundation/IO/Network/HTTP/Exception.h"

#include    "Router.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::WebServer;





/*
 ********************************************************************************
 ************************* WebServer::Router::Rep_ ******************************
 ********************************************************************************
 */
//
struct  Router::Rep_ : Interceptor::_IRep {
    Rep_ (const Sequence<Route>& routes)
        : fRoutes_ (routes)
    {
    }
    virtual void    HandleFault (Message* m, const exception_ptr& e) noexcept override
    {
        // @todo - For now - this is our only FAULT handler - so we do it here, but probably should ben a separate interceptor!!!--LGP 2016-09-02
        RequireNotNull (m);
        Response*   response    =   m->PeekResponse ();
        try {
            try {
                std::rethrow_exception (e);
            }
            catch (const IO::Network::HTTP::Exception& ee) {
                response->SetStatus (ee.GetStatus (), ee.GetReason ());
                response->printf (L"Exception: %s", Characters::ToString (ee).c_str ());
                response->SetContentType (DataExchange::PredefinedInternetMediaType::Text_CT ());
            }
            catch (...) {
                response->SetStatus (IO::Network::HTTP::StatusCodes::kInternalError);
                response->printf (L"Exception: %s", Characters::ToString (e).c_str ());
                response->SetContentType (DataExchange::PredefinedInternetMediaType::Text_CT ());
            }
        }
        catch (...) {
            DbgTrace (L"Oops! - not good, but nothing todo but burry it."); // not assert failure cuz could be out of memory
        }
    }
    virtual void    HandleMessage (Message* m) override
    {
        Optional<RequestHandler>    handler = Lookup_ (*m->PeekRequest ());
        if (handler) {
            (*handler) (m);
        }
        else {
            Execution::Throw (IO::Network::HTTP::Exception (IO::Network::HTTP::StatusCodes::kNotFound));
        }
    }
    Optional<RequestHandler>   Lookup_ (const Request& request) const
    {
        String  method  =   request.GetHTTPMethod ();
        URL     url     =   request.GetURL ();
        String  hostRelPath =   url.GetHostRelativePath ();
        for (Route r : fRoutes_) {
            if (r.fVerbMatch_ and not method.Match (*r.fVerbMatch_)) {
                continue;
            }
            if (r.fPathMatch_ and not hostRelPath.Match (*r.fPathMatch_)) {
                continue;
            }
            if (r.fRequestMatch_ and not (*r.fRequestMatch_) (request)) {
                continue;
            }
            return r.fHandler_;
        }
        return Optional<RequestHandler> {};
    }

    const Sequence<Route>  fRoutes_;    // no need for synchronization cuz constant - just set on construction
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

Optional<RequestHandler>   Router::Lookup (const Request& request) const
{
    return _GetRep<Rep_> ().Lookup_ (request);
}
