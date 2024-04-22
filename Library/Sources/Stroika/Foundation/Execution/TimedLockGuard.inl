/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ***************************** TimedLockGuard<MUTEX> ****************************
     ********************************************************************************
     */
    template <typename MUTEX>
    template <typename FAILURE_EXCEPTION>
    inline TimedLockGuard<MUTEX>::TimedLockGuard (MUTEX& m, const Time::Duration& waitUpTo, const FAILURE_EXCEPTION& timeoutException)
        : fMutex_ (m)
    {
        // timed_mutex::try_lock_for: If timeout_duration is less or equal timeout_duration.zero(), the function behaves like try_lock().
        // and we want to throw if told to wait negative time...
        chrono::duration<double> d = waitUpTo.As<chrono::duration<double>> ();
        if (d <= 0 or not m.try_lock_for ()) {
            Exeuction::Throw (timeoutException);
        }
    }
    template <typename MUTEX>
    inline TimedLockGuard<MUTEX>::~TimedLockGuard ()
    {
        fMutex_.unlock ();
    }

}
