/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
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
        : inherited{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, [[maybe_unused]] bool& handled) {
            if constexpr (invocable<HANDLER_FUNCTION, Message&, const Sequence<String>&, bool&>) {
                messageHandler (m, matchedArgs, handled);
            }
            else if constexpr (invocable<HANDLER_FUNCTION, Message&, const Sequence<String>&>) {
                messageHandler (m, matchedArgs);
                handled = true;
            }
            else if constexpr (invocable<HANDLER_FUNCTION, Message&>) {
                messageHandler (m);
                handled = true;
            }
            else if constexpr (invocable<HANDLER_FUNCTION, Request&, Response&>) {
                messageHandler (m.rwRequest (), m.rwResponse ());
                handled = true;
            }
            else if constexpr (invocable<HANDLER_FUNCTION, Request&, Response&, const Sequence<String>&>) {
                messageHandler (m.rwRequest (), m.rwResponse (), matchedArgs);
                handled = true;
            }
            else if constexpr (invocable<HANDLER_FUNCTION, Request&, Response&, const Sequence<String>&, bool&>) {
                messageHandler (m.rwRequest (), m.rwResponse (), matchedArgs);
                handled = true;
            }
            else if constexpr (invocable<HANDLER_FUNCTION, Message&, const String&>) {
                Require (matchedArgs.size () == 1);
                messageHandler (m, matchedArgs[0]);
                handled = true;
            }
            else if constexpr (invocable<HANDLER_FUNCTION, Message&, const String&, const String&>) {
                Require (matchedArgs.size () == 2);
                messageHandler (m, matchedArgs[0], matchedArgs[1]);
                handled = true;
            }
            else if constexpr (invocable<HANDLER_FUNCTION, Message&, const String&, const String&, const String&>) {
                Require (matchedArgs.size () == 3);
                messageHandler (m, matchedArgs[0], matchedArgs[1], matchedArgs[2]);
                handled = true;
            }
        }}
    {
    }
#else
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const Sequence<String>&, bool&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : inherited{messageHandler}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const Sequence<String>&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, const Sequence<String>& matchedArgs, bool& handled) {
            messageHandler (m, matchedArgs);
            handled = true;
        }}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, bool& handled) {
            messageHandler (m);
            handled = true;
        }}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Request&, Response&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, bool& handled) {
            messageHandler (m.rwRequest (), m.rwResponse ());
            handled = true;
        }}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Request&, Response&, const Sequence<String>&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, bool& handled) {
            messageHandler (m.rwRequest (), m.rwResponse (), matchedArgs);
            handled = true;
        }}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Request&, Response&, const Sequence<String>&, bool&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, bool& handled) {
            messageHandler (m);
            handled = true;
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
            handled = true;
        }}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const String&, const String&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, bool& handled) {
            Require (matchedArgs.size () == 2);
            messageHandler (m, matchedArgs[0], matchedArgs[1]);
            handled = true;
        }}
    {
    }
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const String&, const String&, const String&>) HANDLER_FUNCTION>
    inline RequestHandler::RequestHandler (HANDLER_FUNCTION&& messageHandler)
        : RequestHandler{[=] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs, bool& handled) {
            Require (matchedArgs.size () == 3);
            messageHandler (m, matchedArgs[0], matchedArgs[1], matchedArgs[2]);
            handled = true;
        }}
    {
    }
#endif

    // DEPRECATED....
    inline RequestHandler::RequestHandler (const function<void (Message*, const Sequence<String>&, bool*)>& f)
        : RequestHandler{[f] (Message& m, const Sequence<String>& a, bool& h) { f (&m, a, &h); }}
    {
    }
    inline RequestHandler::RequestHandler (const function<void (Message*, const Sequence<String>&)>& f)
        : RequestHandler{[f] (Message& m, const Sequence<String>& matches, bool& completed) {
            f (&m, matches);
            completed = true;
        }}
    {
    }
    inline RequestHandler::RequestHandler (const function<void (Message*)>& f)
        : RequestHandler{[f] (Message& m, const Sequence<String>&, bool& completed) {
            f (&m);
            completed = true;
        }}
    {
    }
    template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*)>>>*>
    RequestHandler::RequestHandler (_Fx _Func)
        : RequestHandler{[_Func] (Message& m, const Sequence<String>&, bool& completed) {
            _Func (&m);
            completed = true;
        }}
    {
    }
    template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const Sequence<String>&)>>>*>
    RequestHandler::RequestHandler (_Fx _Func, int*)
        : RequestHandler{[_Func] (Message& m, const Sequence<String>& args, bool& completed) {
            _Func (&m, args);
            completed = true;
        }}
    {
    }
    template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const String& arg0)>>>*>
    RequestHandler::RequestHandler (_Fx _Func, short*)
        : RequestHandler{[_Func] (Message& msg, const Sequence<String>& matches) {
            Require (matches.length () >= 1);
            _Func (&msg, matches[0]);
        }}
    {
    }
    template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const String& arg0, const String& arg1)>>>*>
    RequestHandler::RequestHandler (_Fx _Func, char*)
        : RequestHandler{[_Func] (Message& msg, const Sequence<String>& matches) {
            Require (matches.length () >= 2);
            _Func (&msg, matches[0], matches[1]);
        }}
    {
    }
    template <class _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Request*, Response*)>>>*>
    RequestHandler::RequestHandler (_Fx _Func, void*)
        : RequestHandler{[_Func] (Message& message) { _Func (&message.rwRequest (), &message.rwResponse ()); }}
    {
    }

}
