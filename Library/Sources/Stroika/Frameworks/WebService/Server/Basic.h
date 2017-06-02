/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_Basic_h_
#define _Stroika_Framework_WebService_Server_Basic_h_ 1

#include "../../StroikaPreComp.h"

#include "../../WebServer/Request.h"
#include "../../WebServer/RequestHandler.h"
#include "../../WebServer/Response.h"

#include "../Basic.h"

/*
 */

/*
 * TODO:
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *       SUPER DUPER ROUGH DRAFT
 *
 */

namespace Stroika {
    namespace Frameworks {
        namespace WebService {
            namespace Server {

                using namespace Stroika::Foundation;

                using Characters::String;
                using Containers::Sequence;
                using Containers::Set;
                using Memory::Optional;

                using WebServer::Request;
                using WebServer::Response;

                void ExpectedMethod (const Request* request, const Set<String>& methods, const Optional<String>& fromInMessage);
                void ExpectedMethod (const Request* request, const WebServiceMethodDescription& wsMethodDescription);

                void WriteDocsPage (Response* response, const Sequence<WebServiceMethodDescription>& operations, const String& h1Text = L"Operations");
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Basic.inl"

#endif /*_Stroika_Framework_WebService_Server_Basic_h_*/
