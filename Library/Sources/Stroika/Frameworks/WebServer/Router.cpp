/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdlib>

#include    "../../Foundation/Characters/String2Int.h"
#include    "../../Foundation/Containers/Common.h"
#include    "../../Foundation/DataExchange/BadFormatException.h"
#include    "../../Foundation/Debug/Assertions.h"
#include    "../../Foundation/Execution/Exceptions.h"
#include    "../../Foundation/Memory/SmallStackBuffer.h"
#include    "../../Foundation/IO/Network/HTTP/Headers.h"

#include    "Router.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::WebServer;





#if     !qCompilerAndStdLib_regex_Buggy
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
        if (r.fRequestMatch_ and not (r.fRequestMatch_) (request)) {
            continue;
        }
        return r.fHandler_;
    }
    return Optional<RequestHandler> {};
}
#endif