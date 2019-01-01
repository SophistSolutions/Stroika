/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_TimedLockGuard_h_
#define _Stroika_Foundation_Execution_TimedLockGuard_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>

#include "../Time/Duration.h"

#include "TimeOutException.h"

namespace Stroika::Foundation::Execution {

    /**
     *  Like lock_guard, but only used on TIMED mutexes, and instead of unique_lock::try_lock() returning false
     *  or the CTOR just silently failing to lock, this throws a lock failure.
     */
    template <typename MUTEX = timed_mutex>
    class TimedLockGuard {
    public:
        /**
         *  \note   if argument duration is <= 0, this always throws regardless if whether or not the mutex could be acquired.
         */
        template <typename FAILURE_EXCEPTION = TimeOutException>
        TimedLockGuard (MUTEX& m, const Time::Duration& waitUpTo, const FAILURE_EXCEPTION& timeoutException = FAILURE_EXCEPTION ());
        TimedLockGuard (const TimedLockGuard&) = delete;
        TimedLockGuard& operator= (const TimedLockGuard&) = delete;
        ~TimedLockGuard ();

    private:
        MUTEX& fMutex_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimedLockGuard.inl"

#endif /*_Stroika_Foundation_Execution_TimedLockGuard_h_*/
