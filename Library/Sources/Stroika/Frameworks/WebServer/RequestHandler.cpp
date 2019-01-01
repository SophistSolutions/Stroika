/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "RequestHandler.h"

using namespace Stroika::Foundation;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

/*
 ********************************************************************************
 ************************* WebServer::RequestHandler ****************************
 ********************************************************************************
 */
RequestHandler::RequestHandler (const function<void(Request* request, Response* response)>& f)
    : RequestHandler{[f](Message* message) { f (message->PeekRequest (), message->PeekResponse ()); }}
{
}
