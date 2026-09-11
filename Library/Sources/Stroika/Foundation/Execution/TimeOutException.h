/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_TimeOutException_h_
#define _Stroika_Foundation_Execution_TimeOutException_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

_DeprecatedFile_ ("DEPRECATED TimeOutException.h - since v3.0d25 - catch (const system_error&) and test "
                  "e.code () == errc::timed_out; for the non-deprecated helpers that were here, use "
                  "Execution/Timeout.h and Execution/TimedLock.h");

#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/TimedLock.h"
#include "Stroika/Foundation/Execution/Timeout.h"
#include "Stroika/Foundation/Time/Realtime.h"

namespace Stroika::Foundation::Execution {

    /**
     *  \note   This entire header is DEPRECATED (Stroika v3.0d25) - the TimeOutException type and
     *          ThrowTimeOutException (), both removed at the next release-stage transition.
     *
     *          Everything that is NOT deprecated has moved out: @see Execution/Timeout.h for how
     *          Stroika reports a timeout and for ThrowTimeoutExceptionAfter () / ThrowIfTimeout (),
     *          and @see Execution/TimedLock.h for TryLockUntil () / UniqueLock (). This file includes
     *          both, so code that included this one keeps compiling unchanged.
     */
    /*
     ********************************************************************************
     ***************************** DEPRECATED (v3.0d25) *****************************
     ********************************************************************************
     */
    inline [[deprecated ("Since Stroika v3.0d25 - use ThrowError (errc::timed_out)")]] [[noreturn]] void ThrowTimeOutException ()
    {
        ThrowError (errc::timed_out);
    }

    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    /**
     *  \deprecated Since Stroika v3.0d25 - catch the CONDITION instead of this type:
     *      \code
     *          catch (const system_error& e) {
     *              if (e.code () == errc::timed_out) { ... }
     *          }
     *      \endcode
     *      That also matches timeouts raised outside Stroika, which catching by type never did.
     */
    class [[deprecated ("Since Stroika v3.0d25 - catch (const system_error&) and test e.code () == errc::timed_out")]] TimeOutException
        : public Execution::SystemErrorException {
    public:
        TimeOutException ()
            : TimeOutException{"Timeout Expired"sv}
        {
        }
        TimeOutException (error_code ec)
            : TimeOutException{ec, "Timeout Expired"sv}
        {
        }
        TimeOutException (const Characters::String& message)
            : TimeOutException{make_error_code (errc::timed_out), message}
        {
        }
        TimeOutException (error_code ec, const Characters::String& message)
            : SystemErrorException{ec, message}
        {
        }

    public:
        static const TimeOutException kThe;
    };
    inline const TimeOutException TimeOutException::kThe;
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

}

#endif /*_Stroika_Foundation_Execution_TimeOutException_h_*/
