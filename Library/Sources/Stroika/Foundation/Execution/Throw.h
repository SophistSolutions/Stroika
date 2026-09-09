/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Throw_h_
#define _Stroia_Foundation_Execution_Throw_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <exception>
#include <memory>
#include <stdexcept>
#include <system_error>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Debug/Trace.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  \note \em Design Note
 *      (essentially) All exceptions thrown by Stroika (except where needed by quirks of underlying library
 *      integrated with) inherit from std::exception, or Stroika::Foundation::Execution::SilentException.
 *
 *      This means that any code which wishes to report an exception can catch these two types, and use
 *      the 'what()' method to report the text of the exception message.
 *
 *      Sadly, there is no documentation (I'm aware of) or specification of the character set/code page reported
 *      back by the what () on an exception. It tends to be ascii. Stroika guarantees that all exceptions it throws
 *      will use the current SDK characters (@see SDKString). But - its best to check for inheriting from
 *      Exception<>, since the SDK character set might not allow representing some unicode characters.
 */

namespace Stroika::Foundation::Execution {

    /**
     *  qStroika_Foundation_Execution_Throw_TraceThrowpoint controls whether or not Stroika will DbgTrace () on
     *  (essentially) all exception throws.
     *
     *  This is nearly always desirable, but in applications that do lots of exceptions (probably not a good idea), this can produce
     *  a lot of trace log noise, and some people object to it.
     *
     *  Since this only affects calls to DbgTrace () - it only has effect if qStroika_Foundation_Debug_DefaultTracingOn is on.
     *
     *  \note - to turn this on, you can add the flag
     *          --c-define '\#define qStroika_Foundation_Execution_Throw_TraceThrowpoint 0'
     *          to your configure line
     */
#ifndef qStroika_Foundation_Execution_Throw_TraceThrowpoint
#define qStroika_Foundation_Execution_Throw_TraceThrowpoint qStroika_Foundation_Debug_DefaultTracingOn
#endif

    /**
     *  qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace is only meant for debugging. If true, then the
     *  overloads to Throw will also include a call to BackTrace, so its easier to track in TraceLogs where an exception 
     *  is thrown from (helpful when you don't have a debugger).
     *
     *  This has no effect unless qStroika_Foundation_Execution_Throw_TraceThrowpoint is also true.
     */
#ifndef qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
#define qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace qStroika_Foundation_Debug_DefaultTracingOn
#endif

    /**
     *  \brief  like the builtin C++ 'throw', but stamps in the current Activity context and does helpful,
     *          type dependent DbgTrace () messages first
     *
     *  Utility to call a Trace message (hopefully an appropriate one) for an exception being thrown.
     *
     *  \note   **Throw () and ThrowError () are a deliberate pair, and choosing between them is a real
     *          decision - not a matter of which argument type you happen to have:**
     *
     *              o   `ThrowError (ec)` - *"report this error in Stroika's NORMALIZED form."* It may change
     *                  the type thrown, and may give up part of how the error was represented, so that what
     *                  arrives can be tested portably. @see ThrowError for exactly what is preserved (the
     *                  meaning) and what can be given up (the representation).
     *
     *              o   `Throw (x)` - *"throw exactly THIS."* No translation, no promotion, nothing lossy: the
     *                  catcher gets the object you constructed, with its TYPE and its MESSAGE unchanged. That
     *                  is a deliberate property, not a shortcoming: Throw (X) throwing something other than X
     *                  would be a far worse surprise. (It does fill in the Activity context when the exception
     *                  did not specify one - see below - which adds to what the exception reports without
     *                  changing what it is.)
     *
     *          The consequence is that constructing a SystemErrorException yourself and throwing it here gets
     *          you NONE of the promotions documented at @see Execution::ThrowError - so a code whose condition
     *          is errc::not_enough_memory arrives as a SystemErrorException rather than std::bad_alloc, and a
     *          caller who wrote catch (const bad_alloc&) does not see it.
     *
     *          Doing that is perfectly legal, and occasionally what you want. But it is almost never what you
     *          want when the error_code came from the OS or from a library with its own error_category: there,
     *          you want ThrowError (ec). This is not a hypothetical - Connection_libcurl did exactly the wrong
     *          one of these, it read correctly, and the resulting missed timeouts were not diagnosed for years.
     *          **If you have an error_code, reach for ThrowError () first, and use Throw () only if you have a
     *          reason to want the promotions suppressed.**
     *
     *  \note   ***Changed in Stroika v3.0d25*** Throw () stamps the CURRENT Activity stack into the exception -
     *          but only when the exception did not specify one (GetActivities () == nullopt). It imbues a COPY,
     *          so the object you passed is not modified. That is what lets a `static const` exception - built
     *          once and thrown many times - report each throw's own context instead of freezing whichever
     *          context happened to be live the first time. Activities specified deliberately (passed to the
     *          constructor, or set with ImbueActivities ()) are never overwritten, and an exception thrown with
     *          a bare `throw X{}` gets no Activity context at all.
     *          @see ExceptionStringHelper::ImbueActivities.
     *
     *  \note   What you hand Throw () also decides what the CATCHER gets. Stroika's Exception<> types - and so
     *          SystemErrorException and everything derived from it - preserve UNICODE message text, and can
     *          carry the Activity context described above (@see Exception<>). Throw (system_error{ec})
     *          carries neither, and neither can be recovered later by whoever catches it. Since
     *          SystemErrorException IS a std::system_error, throwing the Stroika type costs nothing in
     *          interoperability - so prefer Throw (SystemErrorException{ec}), or better still ThrowError (ec).
     *
     *  \note   The overloads which take an extra trace message are JUST for convenience, and vector through the
     *          1-arg overload.
     */
    template <typename T>
    [[noreturn]] void Throw (T&& e2Throw);
    template <typename T>
    [[noreturn]] void Throw (T&& e2Throw, const char* traceMsg);
    template <typename T>
    [[noreturn]] void Throw (T&& e2Throw, const wchar_t* traceMsg);

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
     * @brief Throws an exception if the argument is null (nullptr, or nullopt, or similar). Optionally takes specific exception to throw (defaults to bad_alloc).
     * 
     *  If the first argument is null, throw the second argument exception (which defaults to bad_alloc)
     */
    template <equality_comparable_with<nullptr_t> T, typename E>
    void ThrowIfNull (const T& p, const E& e);
    template <equality_comparable_with<nullptr_t> T>
    void ThrowIfNull (const T& p);
    template <Common::Weak_Equality_Comparable_With<nullopt_t> T, typename E>
    void ThrowIfNull (const T& p, const E& e)
        requires (not equality_comparable_with<nullptr_t, T>);
    template <Common::Weak_Equality_Comparable_With<nullopt_t> T>
    void ThrowIfNull (const T& p)
        requires (not equality_comparable_with<nullptr_t, T>);

    /**
     *  If the first argument is failed throw the exception, and else return the value. Works with std::expected, to Common::StdCompat::expected
     * 
     *  For some reason, this doesn't appear to be part of the std::expected interface, so we define it here. Maybe there is a way with
     *  transform, etc, but it doesn't appear to be so (without using a two step process where you create the expected, and then can
     *  reference it in a second step, binding it into the lambda with transform_error or some such).
     */
    template <typename EXPECTED>
    typename EXPECTED::value_type ThrowIfFailed (const EXPECTED& e);

    /** 
     *  \def IgnoreExceptionsForCall - ignore all exceptions for the given argument call (evaluate arg)
     *
     *      @see IgnoreExceptionsExceptThreadAbortForCall
     */
#define IgnoreExceptionsForCall(theCode)                                                                                                   \
    try {                                                                                                                                  \
        theCode;                                                                                                                           \
    }                                                                                                                                      \
    catch (...) {                                                                                                                          \
    }

    /**
     *  \def IgnoreExceptionsExceptThreadAbortForCall - ignore all exceptions (except thread abort) for the given argument call (evaluate arg)
     *
     *      @see IgnoreExceptionsForCall
     */
#define IgnoreExceptionsExceptThreadAbortForCall(theCode)                                                                                  \
    try {                                                                                                                                  \
        theCode;                                                                                                                           \
    }                                                                                                                                      \
    catch (const Stroika::Foundation::Execution::Thread::AbortException&) {                                                                \
        Execution::ReThrow ();                                                                                                             \
    }                                                                                                                                      \
    catch (...) {                                                                                                                          \
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Throw.inl"

#endif /*_Stroia_Foundation_Execution_Throw_h_*/
