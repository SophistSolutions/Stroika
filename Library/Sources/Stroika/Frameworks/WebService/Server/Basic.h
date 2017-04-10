/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_Basic_h_
#define _Stroika_Framework_WebService_Server_Basic_h_ 1

#include "../../StroikaPreComp.h"

#include "../../../Foundation/Containers/Sequence.h"
#include "../../../Foundation/Containers/Set.h"
#include "../../../Foundation/DataExchange/InternetMediaType.h"
#include "../../../Foundation/DataExchange/VariantValue.h"

#include "../../WebServer/Request.h"
#include "../../WebServer/RequestHandler.h"
#include "../../WebServer/Response.h"

#include "../../../Foundation/DataExchange/ObjectVariantMapper.h"

#include "../Basic.h"

/*
 */

/*
 * TODO:
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
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
                using DataExchange::InternetMediaType;
                using DataExchange::VariantValue;
                using Memory::Optional;

                using WebServer::Request;
                using WebServer::Response;

                ////// SUPER DUPER ROUGH DRAFT

                VariantValue GetWebServiceArgsAsVariantValue (Request* request, const Optional<String>& fromInMessage);

                void ExpectedMethod (const Request* request, const Set<String>& methods, const Optional<String>& fromInMessage);
                void ExpectedMethod (const Request* request, const WebServiceMethodDescription& wsMethodDescription);

                void WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const VariantValue& responseValue);

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
