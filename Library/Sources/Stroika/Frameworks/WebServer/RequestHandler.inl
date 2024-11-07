/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ************************** WebServer::RequestHandler ***************************
     ********************************************************************************
     */
    inline RequestHandler::RequestHandler (const function<void (Message*, const Sequence<String>&, bool*)>& f)
        : function<void (Message*, const Sequence<String>&, bool*)>{f}
    {
    }
    inline RequestHandler::RequestHandler (const function<void (Message*, const Sequence<String>&)>& f)
        : RequestHandler{[f] (Message* m, const Sequence<String>& matches, bool* completed) {
            f (m, matches);
            *completed = true;
        }}
    {
    }
    inline RequestHandler::RequestHandler (const function<void (Message*)>& f)
        : RequestHandler{[f] (Message* m, const Sequence<String>&, bool* completed) {
            f (m);
            *completed = true;
        }}
    {
    }
    template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*)>>>*>
    RequestHandler::RequestHandler (_Fx _Func)
        : RequestHandler (function<void (Message*)>{_Func})
    {
    }
    template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const Sequence<String>&)>>>*>
    RequestHandler::RequestHandler (_Fx _Func, int*)
        : RequestHandler (function<void (Message*, const Sequence<String>&)>{_Func})
    {
    }
    template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const String& arg0)>>>*>
    RequestHandler::RequestHandler (_Fx _Func, short*)
        : RequestHandler (function<void (Message*, const Sequence<String>&)>{[_Func] (Message* msg, const Sequence<String>& matches) {
            Require (matches.length () >= 1);
            _Func (msg, matches[0]);
        }})
    {
    }
    template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const String& arg0, const String& arg1)>>>*>
    RequestHandler::RequestHandler (_Fx _Func, char*)
        : RequestHandler (function<void (Message*, const Sequence<String>&)>{[_Func] (Message* msg, const Sequence<String>& matches) {
            Require (matches.length () >= 2);
            _Func (msg, matches[0], matches[1]);
        }})
    {
    }
    template <class _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Request*, Response*)>>>*>
    RequestHandler::RequestHandler (_Fx _Func, void*)
        : RequestHandler ([_Func] (Message* message) {
            RequireNotNull (message);
            _Func (&message->rwRequest (), &message->rwResponse ());
        })
    {
    }

}
