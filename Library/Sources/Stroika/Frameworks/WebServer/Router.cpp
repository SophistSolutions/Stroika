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





/*
 ********************************************************************************
 ************************* WebServer::Router ************************************
 ********************************************************************************
 */
Router::Router (const Collection<Route>& routes)
    : fRoutes_ (routes)
{
}

Optional<Handler>   Router::Lookup (const String& method, const URL& url) const
{
    String  hostRelPath =   url.GetHostRelativePath ();
    for (Route r : fRoutes_.load ()) {
        if (method.Match (r.fVerbMatch) and hostRelPath.Match (r.fPathMatch)) {
            return r.fHandler;
        }
    }
    return Optional<Handler> {};
}

