/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Exceptions_h_
#define _Stroia_Foundation_Execution_Exceptions_h_  1

#include    "../StroikaPreComp.h"

// Unclear if/why we should include <exception>/string/stdexcept here? -- LGP 2011-11-29
#include    <exception>
#include    <memory>
#include    <stdexcept>
#include    <system_error>

#include    "../Configuration/Common.h"
#include    "../Debug/Trace.h"



/**
 *  TODO:
 *      @todo   Review new <system_error> stuff
 *              like http://en.cppreference.com/w/cpp/error/error_condition
 *
 *              And see how that impacts ExceptionPropagate.h, ErrNoException etc...
 *
 *      @todo   See about std::make_exception_ptr for copyable exceptions
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            using   Characters::SDKString;


            /**
             *  Utility to call a Trace message (hopefully an appropriate one) for an exception being
             *  thrown... But this function is also specailized to do call D::DoThrow() for several types -
             *  which CAN translate the kind of exception throw. For example, for Platoform:Windows::Execption -
             *  ERROR_OUTOFMEMORY is translated to std::bad_alloc ().
             *
             *  ONLY the first variation (with no traceMessage) is template specailized. The overloads
             *  which take an extra message are JUST for convenience, and vector through the 1-arg overload -
             *  so as to get is specialization.
             */
            template    <typename T>
            void     _NoReturn_ DoThrow (const T& e2Throw);
            template    <typename T>
            void    _NoReturn_  DoThrow (const T& e2Throw, const char* traceMsg);
            template    <typename T>
            void    _NoReturn_  DoThrow (const T& e2Throw, const wchar_t* traceMsg);


            /**
             *  Just a regular C++ rethrow, but with a DbgTrace message...
             */
            void    _NoReturn_  DoReThrow ();
            void    _NoReturn_  DoReThrow (const exception_ptr& e);
            void    _NoReturn_  DoReThrow (const char* traceMsg);
            void    _NoReturn_  DoReThrow (const exception_ptr& e, const char* traceMsg);
            void    _NoReturn_  DoReThrow (const wchar_t* traceMsg);
            void    _NoReturn_  DoReThrow (const exception_ptr& e, const wchar_t* traceMsg);


            /**
             */
            template    <typename E>
            void    ThrowIfNull (const void* p, const E& e = E ());
            void    ThrowIfNull (const void* p);
            template    <typename   T>
            void    ThrowIfNull (const shared_ptr<T>& p);


            /**
             */
#define IgnoreExceptionsForCall(theCode)        try {theCode;} catch (...) {}


            /**
             */
#define IgnoreExceptionsExceptThreadAbortForCall(theCode)        try {theCode;} catch (const Stroika::Foundation::Execution::Thread::AbortException&) { Execution::DoReThrow (); } catch (...) {}


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Exceptions.inl"

#endif  /*_Stroia_Foundation_Execution_Exceptions_h_*/
