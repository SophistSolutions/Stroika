/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_RequestHandler_inl_
#define _Stroika_Frameworks_WebServer_RequestHandler_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Foundation/Debug/Assertions.h"

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ************************** WebServer::RequestHandler ***************************
     ********************************************************************************
     */
    inline RequestHandler::RequestHandler (const function<void (Message*, const Containers::Sequence<Characters::String>&)>& f)
        : function<void (Message*, const Containers::Sequence<Characters::String>&)>{f}
    {
    }
    inline RequestHandler::RequestHandler (const function<void (Message*)>& f)
        : function<void (Message*, const Containers::Sequence<Characters::String>&)>{[=] (Message* m, const Containers::Sequence<Characters::String>&) { f (m); }}
    {
    }
    template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*)>>>*>
    RequestHandler::RequestHandler (_Fx _Func)
        : RequestHandler (function<void (Message*)>{_Func})
    {
    }
    template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const Containers::Sequence<Characters::String>&)>>>*>
    RequestHandler::RequestHandler (_Fx _Func, int*)
        : RequestHandler (function<void (Message*, const Containers::Sequence<Characters::String>&)>{_Func})
    {
    }
    template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const Characters::String& arg0)>>>*>
    RequestHandler::RequestHandler (_Fx _Func, short*)
        : RequestHandler (function<void (Message*, const Containers::Sequence<Characters::String>&)>{
              [_Func] (Message* msg, const Containers::Sequence<Characters::String>& matches) {
                  Require (matches.length () >= 1);
                  _Func (msg, matches[0]);
              }})
    {
    }
    template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const Characters::String& arg0, const Characters::String& arg1)>>>*>
    RequestHandler::RequestHandler (_Fx _Func, char*)
        : RequestHandler (function<void (Message*, const Containers::Sequence<Characters::String>&)>{
              [_Func] (Message* msg, const Containers::Sequence<Characters::String>& matches) {
                  Require (matches.length () >= 2);
                  _Func (msg, matches[0], matches[1]);
              }})
    {
    }
    template <class _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Request*, Response*)>>>*>
    RequestHandler::RequestHandler (_Fx _Func, void*)
        : RequestHandler ([_Func] (Message* message) { RequireNotNull (message); _Func (message->PeekRequest (), message->PeekResponse ()); })
    {
    }

}

#endif /*_Stroika_Frameworks_WebServer_RequestHandler_inl_*/
