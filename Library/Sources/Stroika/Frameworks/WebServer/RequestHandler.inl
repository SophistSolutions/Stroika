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
#if qCompilerAndStdLib_template_ConstraintDiffersInTemplateRedeclaration_Buggy
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const Sequence<String>&, bool&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : function<void (Message* m, const Sequence<String>& args, bool* handled)>{
              [= messageHandler] (Message* m, [[maybe_unused]] const Sequence<String>& args, [[maybe_unused]] bool* handled) {
                  if constexpr (invocable<Message&, const Sequence<String>&, bool&>) {
                      messageHandler (*m, args, *handled);
                  }
                  else if constexpr (invocable<Message&, const Sequence<String>&>) {
                      messageHandler (m, matchedArgs);
                      *handled = true;
                  }
                  else if constexpr (invocable<Message&>) {
                      messageHandler (m);
                      *handled = true;
                  }
                  // @todo other cases... below all handled
              }}
    {
    }
#else
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const Sequence<String>&, bool&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        //: function<void (Message*, const Sequence<String>&, bool*)>{messageHandler}
        : function<void (Message* m, const Sequence<String>& args, bool* handled)>{[=] (Message* m, const Sequence<String>& args, bool* handled) {
            // backward compat with old api
            messageHandler (*m, args, *handled);
        }}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const Sequence<String>&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, const Sequence<String>& matchedArgs, bool& handled) {
            messageHandler (m, matchedArgs);
            *handled = true;
        }}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, bool& handled) {
            messageHandler (m);
            *handled = true;
        }}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Request&, Response&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, bool& handled) {
            messageHandler (m.rwRequest (), m.rwResponse ());
            *handled = true;
        }}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Request&, Response&, const Sequence<String>&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, bool& handled) {
            messageHandler (m, matchedArgs);
            *handled = true;
        }}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Request&, Response&, const Sequence<String>&, bool&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, bool& handled) {
            messageHandler (m);
            *handled = true;
        }}
    {
    }
    // not sure (yet) how to do this with variadic templates
    // explode Sequence<String> - caller bug/assertion of invoked with wrong # of arguments (since based solely on route regexp)
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const String&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, bool& handled) {
            Require (matchedArgs.size () == 1);
            messageHandler (m, matchedArgs[0]);
            *handled = true;
        }}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const String&, const String&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, bool& handled) {
            Require (matchedArgs.size () == 2);
            messageHandler (m, matchedArgs[0], matchedArgs[1]);
            *handled = true;
        }}
    {
    }
#endif

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
