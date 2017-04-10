/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "Basic.h"

#include "ObjectVariantMapper.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::Server;

using Characters::StringBuilder;
using Characters::String_Constant;

/*
 ********************************************************************************
 ************************ ObjectVariantMapper::mkRequestHandler *****************
 ********************************************************************************
 */
WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<Memory::BLOB (WebServer::Message* m)>& f)
{
    return [=](WebServer::Message* m) {
        ExpectedMethod (m->PeekRequest (), webServiceDescription);
        if (webServiceDescription.fResponseType) {
            m->PeekResponse ()->SetContentType (*webServiceDescription.fResponseType);
        }
        m->PeekResponse ()->write (f (m));
    };
}
