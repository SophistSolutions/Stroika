/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Router.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::WebServer;





/*
 ********************************************************************************
 ************************* WebServer::Router ************************************
 ********************************************************************************
 */
Router::Router (const Sequence<Route>& routes)
    : fRoutes_ (routes)
{
}

Optional<RequestHandler>   Router::Lookup (const Request& request) const
{
    String  method  =   request.fMethod;
    URL     url     =   request.fURL;
    String  hostRelPath =   url.GetHostRelativePath ();
    for (Route r : fRoutes_.load ()) {
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
