/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_ObjectVariantMapper_h_
#define _Stroika_Framework_WebService_Server_ObjectVariantMapperh_ 1

#include "../../StroikaPreComp.h"

#include "../../../Foundation/Containers/Sequence.h"
#include "../../../Foundation/Containers/Set.h"
#include "../../../Foundation/DataExchange/InternetMediaType.h"
#include "../../../Foundation/DataExchange/VariantValue.h"
#include "../../../Foundation/DataExchange/ObjectVariantMapper.h"

#include "../../WebServer/Request.h"
#include "../../WebServer/RequestHandler.h"
#include "../../WebServer/Response.h"

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

            namespace cvt2Obj {
                // @todo eventually find a way to make this owrk with JSON or XML in/ out and in can be GET query args (depending on WebServiceMethodDescription properties)
                template <typename OUT_ARGS, typename IN_ARGS>
                WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<OUT_ARGS (IN_ARGS)>& f)
                {
                    return [=](WebServer::Message* m) {
                        ExpectedMethod (m->PeekRequest (), webServiceDescription);
                        WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (f (objVarMapper.ToObject<IN_ARGS> (GetWebServiceArgsAsVariantValue (m->PeekRequest (), {})))));
                    };
                }
                template <typename OUT_ARGS>
                WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<OUT_ARGS (void)>& f)
                {
                    return [=](WebServer::Message* m) {
                        ExpectedMethod (m->PeekRequest (), webServiceDescription);
                        WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (f ()));
                    };
                }
            }

            namespace Basic {
                inline WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<Memory::BLOB (WebServer::Message* m)>& f)
                {
                    return [=](WebServer::Message* m) {
                        ExpectedMethod (m->PeekRequest (), webServiceDescription);
                        if (webServiceDescription.fResponseType) {
                            m->PeekResponse ()->SetContentType (*webServiceDescription.fResponseType);
                        }
                        m->PeekResponse ()->write (f (m));
                    };
                }
			}
			}
		}
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectVariantMapper.inl"

#endif /*_Stroika_Framework_WebService_Server_ObjectVariantMapper_h_*/
