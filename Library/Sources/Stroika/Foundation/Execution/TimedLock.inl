/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Time/Common.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ************************* Execution::TryLockUntil ******************************
     ********************************************************************************
     */
    template <Common::StdCompat::TimedMutex TIMED_MUTEX, typename EXCEPTION>
    inline void TryLockUntil (TIMED_MUTEX& m, Time::TimePointSeconds afterTickCount, EXCEPTION&& exception2Throw)
    {
        if (not m.try_lock_until (Time::Pin2SafeSeconds (afterTickCount))) {
            Throw (forward<EXCEPTION> (exception2Throw));
        }
    }
    template <Common::StdCompat::TimedMutex TIMED_MUTEX>
    inline void TryLockUntil (TIMED_MUTEX& m, Time::TimePointSeconds afterTickCount)
    {
        if (not m.try_lock_until (Time::Pin2SafeSeconds (afterTickCount))) {
            ThrowError (errc::timed_out);
        }
    }

    /*
     ********************************************************************************
     ***************************** Execution::UniqueLock ****************************
     ********************************************************************************
     */
    template <Common::StdCompat::TimedMutex TIMED_MUTEX, typename EXCEPTION>
    inline unique_lock<TIMED_MUTEX> UniqueLock (TIMED_MUTEX& m, const chrono::duration<double>& d, EXCEPTION&& exception2Throw)
    {
        unique_lock<TIMED_MUTEX> lock{m, d};
        if (not lock.owns_lock ()) {
            Throw (forward<EXCEPTION> (exception2Throw));
        }
        return lock;
    }
    template <Common::StdCompat::TimedMutex TIMED_MUTEX>
    inline unique_lock<TIMED_MUTEX> UniqueLock (TIMED_MUTEX& m, const chrono::duration<double>& d)
    {
        unique_lock<TIMED_MUTEX> lock{m, d};
        if (not lock.owns_lock ()) {
            ThrowError (errc::timed_out);
        }
        return lock;
    }

}
