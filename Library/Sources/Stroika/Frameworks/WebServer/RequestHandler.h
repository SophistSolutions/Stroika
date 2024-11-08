/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_RequestHandler_h_
#define _Stroika_Framework_WebServer_RequestHandler_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Sequence.h"

#include "Message.h"

/*
 */

/*
 * TODO:
 *      @todo   RequestHandler overloads taking STRING arguments should use variadic templates (but tricky)
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Containers::Sequence;

    /**
     * A request handler should be understood to be stateless - as far as the connection is concerned.
     * 
     *  Each handler is assumed to take an array of strings as arguments (or none). These string arguments come from
     *  the regular expression MATCH of the URL (not from the body of the request, nor from the query string - unless that's part of the regexp matching).
     *
     *  \note bool* handled - \req handle != nullptr, and *handled defaults to false, and is typically set to true.
     *        For constructor overloads with no 'handled' flag argument, 'handle' is automatically set to true 
     *        (this is typically what you want to do - ignore param).
     *        
     *        This allows for multiple routes to match a given url, and dynamically choosing which one to apply.
     *
     *  \notes
     *      ??? Maybe - or maybe have add/remove or notification so associated?? For now - assume stateless - and just called
     *      with HandleRequest ...
     *      
     *      Also - a RequestHandler should be careful about threads, as it could be called first on one thread, and
     *      then - possibly at the same time - on another thread. The same handler can be used multiple times (multiple sessions).
     *      (meaning handler/lambda function itself required to be const or at least internally synchronized).
     */
    class RequestHandler : public function<void (Message* message, const Sequence<String>& matchedArgs, bool* handled)> {
    public:
        /**
         *  \note _Fx _Func, void* arg for one overload is just to differentiate the overload cases so
         *        compiler doesn't complain its a redefinition.
         */
        RequestHandler (const function<void (Message* message, const Sequence<String>& matchedArgs, bool* handled)>& f);
        RequestHandler (const function<void (Message* message, const Sequence<String>& matchedArgs)>& f);
        RequestHandler (const function<void (Message* message)>& f);
        RequestHandler (const function<void (Request* request, Response* response)>& f);
        template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*)>>>* = nullptr>
        RequestHandler (_Fx _Func);
        template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const Sequence<String>&)>>>* = nullptr>
        RequestHandler (_Fx _Func, int* = nullptr);
        template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const String& arg0)>>>* = nullptr>
        RequestHandler (_Fx _Func, short* = nullptr);
        template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Message*, const String& arg0, const String& arg1)>>>* = nullptr>
        RequestHandler (_Fx _Func, char* = nullptr);
        template <typename _Fx, enable_if_t<is_convertible_v<_Fx, function<void (Request*, Response*)>>>* = nullptr>
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
