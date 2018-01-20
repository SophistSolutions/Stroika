/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Exceptions_h_
#define _Stroia_Foundation_Execution_Exceptions_h_ 1

#include "../StroikaPreComp.h"

// Unclear if/why we should include <exception>/string/stdexcept here? -- LGP 2011-11-29
#include <exception>
#include <memory>
#include <stdexcept>
#include <system_error>

#include "../Configuration/Common.h"
#include "../Debug/Trace.h"

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

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            using Characters::SDKString;

/**
             *  qStroika_Foundation_Exection_Exceptions_TraceThrowpoint controls whether or not Stroika will DbgTrace () on
             *  (essentially) all exception throws.
             *
             *  This is nearly always desirable, but in applications that do lots of exceptions (probably not a good idea), this can produce
             *  alot of tracelog noise, and some people object to it.
             *
             *  Since this only affects calls to DbgTrace () - it only has effect if qDefaultTracingOn is on.
             *
             *  \note - to turn this on, you can add the flag
             *          --c-define '\#define qStroika_Foundation_Exection_Exceptions_TraceThrowpoint 0'
             *          to your configure line
             */
#ifndef qStroika_Foundation_Exection_Exceptions_TraceThrowpoint
#define qStroika_Foundation_Exection_Exceptions_TraceThrowpoint qDefaultTracingOn
#endif

/**
             *  qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace is only meant for debugging. If true, then the
             *  overloads to Throw will also include a call to BackTrace, so its easier to track in TraceLogs where an exception 
             *  is thrown from (helpful when you dont have a debugger).
             *
             *  This has no effect unless qStroika_Foundation_Exection_Exceptions_TraceThrowpoint is also true.
             */
#ifndef qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
#define qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace qDefaultTracingOn
#endif

            /**
             *  \brief  identical to builtin C++ 'throw' except that it does helpful, type dependent DbgTrace() messages first
             *
             *  Utility to call a Trace message (hopefully an appropriate one) for an exception being
             *  thrown... But this function is also specailized to do call D::Throw() for several types -
             *  which CAN translate the kind of exception throw. For example, for Platoform:Windows::Execption -
             *  ERROR_OUTOFMEMORY is translated to std::bad_alloc ().
             *
             *  ONLY the first variation (with no traceMessage) is template specailized. The overloads
             *  which take an extra message are JUST for convenience, and vector through the 1-arg overload -
             *  so as to get is specialization.
             */
            template <typename T>
            [[noreturn]] void Throw (const T& e2Throw);
            template <typename T>
            [[noreturn]] void Throw (const T& e2Throw, const char* traceMsg);
            template <typename T>
            [[noreturn]] void Throw (const T& e2Throw, const wchar_t* traceMsg);

            /**
             *  Just a regular C++ rethrow, but with a DbgTrace message...
             */
            [[noreturn]] void ReThrow ();
            [[noreturn]] void ReThrow (const exception_ptr& e);
            [[noreturn]] void ReThrow (const char* traceMsg);
            [[noreturn]] void ReThrow (const exception_ptr& e, const char* traceMsg);
            [[noreturn]] void ReThrow (const wchar_t* traceMsg);
            [[noreturn]] void ReThrow (const exception_ptr& e, const wchar_t* traceMsg);

            /**
             */
            template <typename E>
            void ThrowIfNull (const void* p, const E& e = E ());
            void ThrowIfNull (const void* p);
            template <typename T>
            void ThrowIfNull (const shared_ptr<T>& p);

/**
             */
#define IgnoreExceptionsForCall(theCode) \
    try {                                \
        theCode;                         \
    }                                    \
    catch (...) {                        \
    }

/**
             */
#define IgnoreExceptionsExceptThreadAbortForCall(theCode)                   \
    try {                                                                   \
        theCode;                                                            \
    }                                                                       \
    catch (const Stroika::Foundation::Execution::Thread::AbortException&) { \
        Execution::ReThrow ();                                              \
    }                                                                       \
    catch (...) {                                                           \
    }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Exceptions.inl"

#endif /*_Stroia_Foundation_Execution_Exceptions_h_*/
