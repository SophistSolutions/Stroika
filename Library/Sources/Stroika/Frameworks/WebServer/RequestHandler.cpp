/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "RequestHandler.h"

using namespace Stroika::Foundation;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

/*
 ********************************************************************************
 ************************* WebServer::RequestHandler ****************************
 ********************************************************************************
 */
RequestHandler::RequestHandler (const function<void (Request* request, Response* response)>& f)
    : RequestHandler{[f] (Message* message) { f (&message->rwRequest (), &message->rwResponse ()); }}
{
}
