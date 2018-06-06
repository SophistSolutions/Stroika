/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_RequestHandler_inl_
#define _Stroika_Frameworks_WebServer_RequestHandler_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Foundation/Debug/Assertions.h"

namespace Stroika {
    namespace Frameworks {
        namespace WebServer {

            /*
             ********************************************************************************
             ***************************** WebServer::RequestHandler ************************
             ********************************************************************************
             */
            inline RequestHandler::RequestHandler (const function<void(Message*)>& f)
                : function<void(Message*)>{f}
            {
            }
            template <typename _Fx, typename COMPILE_IF_IS_CONVERTIBLE_FUNC_MESSAGE>
            RequestHandler::RequestHandler (_Fx _Func, COMPILE_IF_IS_CONVERTIBLE_FUNC_MESSAGE*)
                : RequestHandler (function<void(Message*)>{_Func})
            {
            }
            template <class _Fx, typename COMPILE_IF_IS_CONVERTIBLE_FUNC_REQ_RESP>
            RequestHandler::RequestHandler (_Fx _Func, COMPILE_IF_IS_CONVERTIBLE_FUNC_REQ_RESP*, [[maybe_unused]] int j)
                : RequestHandler ([_Func](Message* message) { RequireNotNull (message); _Func (message->PeekRequest (), message->PeekResponse ()); })
            {
            }
        }
    }
}
#endif /*_Stroika_Frameworks_WebServer_RequestHandler_inl_*/
