/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_VariantValue_h_
#define _Stroika_Framework_WebService_Server_VariantValue_h_ 1

#include "../../StroikaPreComp.h"

#include "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include "../../../Foundation/DataExchange/VariantValue.h"

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
                namespace VariantValue {

                    using namespace Stroika::Foundation;

                    using DataExchange::VariantValue;
                    using Memory::BLOB;
                    using Memory::Optional;

                    using WebServer::Request;
                    using WebServer::Response;

                    ////// SUPER DUPER ROUGH DRAFT

                    VariantValue GetWebServiceArgsAsVariantValue (Request* request, const Optional<String>& fromInMessage);

                    void WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const VariantValue& responseValue);

                    /**
                     *  The variants of mkRequestHandler () which take paramNames will eventually be replaced with variadic templates. So the calls should remain
                     *  unchanged, but the exact parameters passed to the template instantiation will.
                     *
                     *  Each named parameter (in paramNames) must correspond to the JSON param arg to the funtion and must correspond exactly in type and
                     *  order to the parameters of the function.
                     *
                     *  \todo - NEEED EXAMPLE
                     *
                     *  The overload with f (void) as argument, takes no arguments (and so omits paramNames), and just returns the given result.
                     *
                     *  The overload with no 'f' just takes a simple message handler (and wraps a bit of WebServiceMethodDescription checking around its call).
                     */
                    template <typename OUT_ARGS, typename ARG_TYPE_0>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0)>& f);
                    template <typename OUT_ARGS, typename ARG_TYPE_0, typename ARG_TYPE_1>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0, ARG_TYPE_1)>& f);

#if 1
                    template <typename OUT_ARGS, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2)>& f);
#endif

#if 0
                    // @todo eventually find a way to make this owrk with JSON or XML in/ out and in can be GET query args (depending on WebServiceMethodDescription properties)
                    template <typename OUT_ARG, typename... IN_ARGS>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<OUT_ARG (IN_ARGS...)>& f);
                    template <typename OUT_ARG, typename... IN_ARGS>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARG (IN_ARGS...)>& f);
#endif
                    template <typename OUT_ARG>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<OUT_ARG (void)>& f);

                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<BLOB (WebServer::Message* m)>& f);
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
#include "VariantValue.inl"

#endif /*_Stroika_Framework_WebService_Server_VariantValue_h_*/
