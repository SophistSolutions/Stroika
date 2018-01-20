/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_RequestHandler_h_
#define _Stroika_Framework_WebServer_RequestHandler_h_ 1

#include "../StroikaPreComp.h"

#include "Message.h"

/*
 */

/*
 * TODO:
 *      (o)
 */

namespace Stroika {
    namespace Frameworks {
        namespace WebServer {

            using namespace Stroika::Foundation;

            /*
             * A request handler should be understood to be stateless - as far as the connection is concerned.
             * ??? Maybe - or maybe have add/remove or notication so assocaited?? For now - assume stateless - and just called
             * with HandleRequest ...
             *
             * Also - a RequestHandler should be careful about threads, as it could be called first on one thread, and
             * then - possibly at the same time - on another thread. The same handler can be used multiple times (multiple sessions).
             */
            class RequestHandler : public function<void(Message* message)> {
            public:
                /**
                 *  @todo - figure out how to get rid of crazy int j = 6 in overloads...
                 */
                RequestHandler (const function<void(Message* message)>& f);
                RequestHandler (const function<void(Request* request, Response* response)>& f);
                template <typename _Fx, typename COMPILE_IF_IS_CONVERTIBLE_FUNC_MESSAGE = typename enable_if<is_convertible<_Fx, function<void(Message*)>>::value>::type>
                RequestHandler (_Fx _Func, COMPILE_IF_IS_CONVERTIBLE_FUNC_MESSAGE* = 0);
                template <typename _Fx, typename COMPILE_IF_IS_CONVERTIBLE_FUNC_REQ_RESP = typename enable_if<is_convertible<_Fx, function<void(Request*, Response*)>>::value>::type>
                RequestHandler (_Fx _Func, COMPILE_IF_IS_CONVERTIBLE_FUNC_REQ_RESP* = 0, int j = 6);
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "RequestHandler.inl"

#endif /*_Stroika_Framework_WebServer_RequestHandler_h_*/
