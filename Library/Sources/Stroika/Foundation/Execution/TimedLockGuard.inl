/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_TimedLockGuard_inl_
#define _Stroika_Foundation_Execution_TimedLockGuard_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */
            template <typename MUTEX>
            template <typename FAILURE_EXCEPTION>
            inline TimedLockGuard<MUTEX>::TimedLockGuard (MUTEX& m, const Time::Duration& waitUpTo, const FAILURE_EXCEPTION& timeoutException)
                : fMutex_ (m)
            {
                // std::timed_mutex::try_lock_for: If timeout_duration is less or equal timeout_duration.zero(), the function behaves like try_lock().
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
    }
}
#endif /*_Stroika_Foundation_Execution_TimedLockGuard_inl_*/
