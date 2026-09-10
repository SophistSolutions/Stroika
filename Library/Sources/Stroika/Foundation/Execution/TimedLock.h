/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_TimedLock_h_
#define _Stroika_Foundation_Execution_TimedLock_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <chrono>
#include <mutex>

#include "Stroika/Foundation/Common/StdCompat.h"
#include "Stroika/Foundation/Execution/Timeout.h"
#include "Stroika/Foundation/Time/Realtime.h"

/**
 *  \file
 *
 *  Acquiring a lock with a DEADLINE, where failing to get it in time is an error rather than
 *  something the caller polls for. Both helpers here report that failure the one way Stroika
 *  reports any timeout - @see Execution/Timeout.h - so the caller writes straight-line code and
 *  handles the timeout with a catch rather than a branch.
 *
 *  For a lock with no deadline, just use std::lock_guard / std::unique_lock directly.
 */

namespace Stroika::Foundation::Execution {

    /**
     *  Translate timed_mutex, or recursive_timed_mutex try_lock_until () calls which fail into a timeout
     *  exception (@see ThrowError (errc::timed_out)).
     */
    template <Common::StdCompat::TimedMutex TIMED_MUTEX, typename EXCEPTION>
    void TryLockUntil (TIMED_MUTEX& m, Time::TimePointSeconds afterTickCount, EXCEPTION&& exception2Throw);
    template <Common::StdCompat::TimedMutex TIMED_MUTEX>
    void TryLockUntil (TIMED_MUTEX& m, Time::TimePointSeconds afterTickCount);

    /**
     *  Simple wrapper on construction of unique_lock<TIMED_MUTEX> - which translates the timeout into a
     *  timeout exception (@see ThrowError (errc::timed_out)).
     *
     *  \note if this function returns (doesn't throw) - the required unique_lock<> OWNS the mutex.
     *
     *  \note   ALIAS - TimedLockGuard. If you are hunting for a "timed lock guard" - a scope-bound guard over
     *          a timed_mutex that throws rather than quietly failing to lock - UniqueLock is that thing under
     *          a less obvious name. Execution::TimedLockGuard existed until Stroika v3.0d25, and was removed
     *          because unique_lock<> dominates any lock_guard analogue here: it is movable, returnable, can
     *          be released early, and is the only form condition_variable accepts.
     *
     *  \see also TryLockUntil
     */
    template <Common::StdCompat::TimedMutex TIMED_MUTEX, typename EXCEPTION>
    unique_lock<TIMED_MUTEX> UniqueLock (TIMED_MUTEX& m, const chrono::duration<double>& d, EXCEPTION&& exception2Throw);
    template <Common::StdCompat::TimedMutex TIMED_MUTEX>
    unique_lock<TIMED_MUTEX> UniqueLock (TIMED_MUTEX& m, const chrono::duration<double>& d);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimedLock.inl"

#endif /*_Stroika_Foundation_Execution_TimedLock_h_*/
