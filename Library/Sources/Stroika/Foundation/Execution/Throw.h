/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Throw_h_
#define _Stroia_Foundation_Execution_Throw_h_ 1

#include "../StroikaPreComp.h"

#include <exception>
#include <memory>
#include <stdexcept>
#include <system_error>

#include "../Configuration/Common.h"
#include "../Debug/Trace.h"

/**
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  \note \em Design Note
 *      (essentially) All exceptions thrown by Stroika (except where needed by quirks of underlying library
 *      integrated with) inherit from std::exception, or Stroika::Foundation::Execution::SilentException.
 *
 *      This means that any code which wishes to report an exception can catch these two types, and use
 *      the 'what()' method to report the text of the exception message.
 *
 *      Sadly, there is no documentation (I'm aware of) or specification of the characterset/code page reported
 *      back by the what () on an exception. It tends to be ascii. Stroika guarantees that all exceptions it throws
 *      will use the current SDK characters (@see SDKString). But - its best to check for inheriting from
 *      Exception<>, since the SDK characterset might not allow representing some unicode characters.
 */

namespace Stroika::Foundation::Execution {

    using Characters::SDKString;

    /**
     *  qStroika_Foundation_Exection_Throw_TraceThrowpoint controls whether or not Stroika will DbgTrace () on
     *  (essentially) all exception throws.
     *
     *  This is nearly always desirable, but in applications that do lots of exceptions (probably not a good idea), this can produce
     *  alot of tracelog noise, and some people object to it.
     *
     *  Since this only affects calls to DbgTrace () - it only has effect if qDefaultTracingOn is on.
     *
     *  \note - to turn this on, you can add the flag
     *          --c-define '\#define qStroika_Foundation_Exection_Throw_TraceThrowpoint 0'
     *          to your configure line
     */
#ifndef qStroika_Foundation_Exection_Throw_TraceThrowpoint
#define qStroika_Foundation_Exection_Throw_TraceThrowpoint qDefaultTracingOn
#endif

    /**
     *  qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace is only meant for debugging. If true, then the
     *  overloads to Throw will also include a call to BackTrace, so its easier to track in TraceLogs where an exception 
     *  is thrown from (helpful when you don't have a debugger).
     *
     *  This has no effect unless qStroika_Foundation_Exection_Throw_TraceThrowpoint is also true.
     */
#ifndef qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace
#define qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace qDefaultTracingOn
#endif

    /**
     *  \brief  identical to builtin C++ 'throw' except that it does helpful, type dependent DbgTrace() messages first
     *
     *  Utility to call a Trace message (hopefully an appropriate one) for an exception being
     *  thrown... But this function is also specialized to do call D::Throw() for several types -
     *  which CAN translate the kind of exception throw. For example, for Platoform:Windows::Execption -
     *  ERROR_OUTOFMEMORY is translated to std::bad_alloc ().
     *
     *  ONLY the first variation (with no traceMessage) is template specialized. The overloads
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
     *  If the first argument is null, throw the second argument exception (or if none, throw bad_alloc)
     */
    template <typename E>
    void ThrowIfNull (const void* p, const E& e = E ());
    void ThrowIfNull (const void* p);
    template <typename T>
    void ThrowIfNull (const unique_ptr<T>& p);
    template <typename T>
    void ThrowIfNull (const shared_ptr<T>& p);

    /** 
     *  \def IgnoreExceptionsForCall - ignore all exceptions for the given argument call (evaluate arg)
     *
     *      @see IgnoreExceptionsExceptThreadAbortForCall
     *      @see IgnoreExceptionsExceptThreadInterruptForCall
     */
#define IgnoreExceptionsForCall(theCode) \
    try {                                \
        theCode;                         \
    }                                    \
    catch (...) {                        \
    }

    /**
     *  \def IgnoreExceptionsExceptThreadAbortForCall - ignore all exceptions (except thread abort) for the given argument call (evaluate arg)
     *
     *      @see IgnoreExceptionsForCall
     *      @see IgnoreExceptionsExceptThreadInterruptForCall
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

    /**
     *  \def IgnoreExceptionsExceptThreadInterruptForCall - ignore all exceptions (except thread interrupt or thread abort) for the given argument call (evaluate arg)
     *
     *      @see IgnoreExceptionsForCall
     *      @see IgnoreExceptionsExceptThreadAbortForCall
     */
#define IgnoreExceptionsExceptThreadInterruptForCall(theCode)                   \
    try {                                                                       \
        theCode;                                                                \
    }                                                                           \
    catch (const Stroika::Foundation::Execution::Thread::InterruptException&) { \
        Execution::ReThrow ();                                                  \
    }                                                                           \
    catch (...) {                                                               \
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Throw.inl"

#endif /*_Stroia_Foundation_Execution_Throw_h_*/
