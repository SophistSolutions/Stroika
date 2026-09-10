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

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimeOutException.inl"

#endif /*_Stroika_Foundation_Execution_TimeOutException_h_*/
