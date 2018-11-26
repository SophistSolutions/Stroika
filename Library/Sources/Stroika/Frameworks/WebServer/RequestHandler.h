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

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    /**
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
         *  \note _Fx _Func, void* arg for one overload is just to differentiate the overload cases so
         *        compiler doesn't complain its a redefinition.
         */
        RequestHandler (const function<void(Message* message)>& f);
        RequestHandler (const function<void(Request* request, Response* response)>& f);
        template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void(Message*)>>>* = nullptr>
        RequestHandler (_Fx _Func);
        template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void(Request*, Response*)>>>* = nullptr>
        RequestHandler (_Fx _Func, void* = nullptr);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "RequestHandler.inl"

#endif /*_Stroika_Framework_WebServer_RequestHandler_h_*/
