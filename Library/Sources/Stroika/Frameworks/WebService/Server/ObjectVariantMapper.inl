/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_ObjectVariantMapper_inl_
#define _Stroika_Framework_WebService_Server_ObjectVariantMapper_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Frameworks {
        namespace WebService {
            namespace Server {
                namespace ObjectVariantMapper {

                    /*
                     ********************************************************************************
                     ************************ ObjectVariantMapper::mkRequestHandler *****************
                     ********************************************************************************
                     */
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
            }
        }
    }
}
#endif /*_Stroika_Framework_WebService_Server_ObjectVariantMapper_inl_*/
