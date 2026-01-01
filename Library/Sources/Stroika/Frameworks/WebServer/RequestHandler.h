/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_RequestHandler_h_
#define _Stroika_Framework_WebServer_RequestHandler_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Sequence.h"

#include "Message.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Containers::Sequence;

    /**
     * A (WebServer Route) request handler (callback function) should be understood to be stateless - as far as the connection is concerned.
     * 
     *  Each handler is assumed to take an array of strings as arguments (or none). These string arguments come from
     *  the regular expression MATCH of the URL (not from the body of the request, nor from the query string - unless that's part of the regexp matching - not recommended).
     *
     *  \note bool& handled - handled defaults to false, and is typically set to true.
     *        For constructor overloads with no 'handled' flag argument, 'handled' is automatically set to true 
     *        (this is typically what you want to do - ignore param).
     *        
     *        This allows for multiple routes to match a given url, and dynamically choosing which one to apply.
     *
     *  \note
     *      a RequestHandler should be careful about threads, as it could be called first on one thread, and
     *      then - possibly at the same time - on another thread. The same handler can be used multiple times (multiple sessions).
     *      (meaning handler/lambda function itself required to be const or at least internally synchronized).
     * 
     *  \note Before Stroika v3.0d12 - this inherited from function<void(Message*, const Sequence<String>&)> - so not fully backward
     *        compatible change...
     */
    class RequestHandler : public function<void (Message& message, const Sequence<String>& matchedArgs, bool& handled)> {
    private:
        using inherited = function<void (Message& message, const Sequence<String>& matchedArgs, bool& handled)>;

    public:
        /**
         * HANDLER (Message& message, Sequence<String> matchedArgs, bool& handled)
         *       This is the generic, underlying full form of the message-handler, without anything defaulted. Other APIs just trivially map back to this.
         * HANDLER (Message& message, Sequence<String> matchedArgs)
         *       Just ignore handled flags
         * HANDLER (Message& message)
         *       Just ignore matchedArgs, and handled flags
         * 
         * HANDLER (Message& message, String arg1, String arg2, ...)
         *      note: for this overload, \pre count of arguments == matchedArgs.size () - cuz must match count of matches returned from Route regexp. 
         * 
         *  \par See Route constructors for examples...
         */
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const Sequence<String>&, bool&>) HANDLER_FUNCTION>
        RequestHandler (HANDLER_FUNCTION&& messageHandler);
#if !qCompilerAndStdLib_template_ConstraintDiffersInTemplateRedeclaration_Buggy
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const Sequence<String>&>) HANDLER_FUNCTION>
        RequestHandler (HANDLER_FUNCTION&& messageHandler);
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&>) HANDLER_FUNCTION>
        RequestHandler (HANDLER_FUNCTION&& messageHandler);
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Request&, Response&>) HANDLER_FUNCTION>
        RequestHandler (HANDLER_FUNCTION&& messageHandler);
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Request&, Response&, const Sequence<String>&>) HANDLER_FUNCTION>
        RequestHandler (HANDLER_FUNCTION&& messageHandler);
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Request&, Response&, const Sequence<String>&, bool&>) HANDLER_FUNCTION>
        RequestHandler (HANDLER_FUNCTION&& messageHandler);

        // explode Sequence<String> arg inline: caller bug/assertion of invoked with wrong # of arguments (since based solely on route regexp)
        // @todo RequestHandler overloads taking STRING arguments should use variadic templates (but tricky)
        // not sure (yet) how to do this
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const String&>) HANDLER_FUNCTION>
        RequestHandler (HANDLER_FUNCTION&& messageHandler);
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const String&, const String&>) HANDLER_FUNCTION>
        RequestHandler (HANDLER_FUNCTION&& messageHandler);
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<Message&, const String&, const String&, const String&>) HANDLER_FUNCTION>
        RequestHandler (HANDLER_FUNCTION&& messageHandler);
#endif

        [[deprecated ("Since Stroika v3.0d12 - use Message& overload)")]] RequestHandler (
            const function<void (Message* message, const Sequence<String>& matchedArgs, bool* handled)>& f);
        [[deprecated ("Since Stroika v3.0d12 - use Message& overload)")]] RequestHandler (
            const function<void (Message* message, const Sequence<String>& matchedArgs)>& f);
        [[deprecated ("Since Stroika v3.0d12 - use Message& overload)")]] RequestHandler (const function<void (Message* message)>& f);
        [[deprecated ("Since Stroika v3.0d12 - use Message& overload)")]] RequestHandler (const function<void (Request* request, Response* response)>& f);
        template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*)>>>* = nullptr>
        [[deprecated ("Since Stroika v3.0d12 - use Message& overload)")]] RequestHandler (_Fx _Func);
        template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const Sequence<String>&)>>>* = nullptr>
        [[deprecated ("Since Stroika v3.0d12 - use Message& overload)")]] RequestHandler (_Fx _Func, int* = nullptr);
        template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const String& arg0)>>>* = nullptr>
        [[deprecated ("Since Stroika v3.0d12 - use Message& overload)")]] RequestHandler (_Fx _Func, short* = nullptr);
        template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const String& arg0, const String& arg1)>>>* = nullptr>
        [[deprecated ("Since Stroika v3.0d12 - use Message& overload)")]] RequestHandler (_Fx _Func, char* = nullptr);
        template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Request*, Response*)>>>* = nullptr>
        [[deprecated ("Since Stroika v3.0d12 - use Message& overload)")]] RequestHandler (_Fx _Func, void* = nullptr);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "RequestHandler.inl"

#endif /*_Stroika_Framework_WebServer_RequestHandler_h_*/
