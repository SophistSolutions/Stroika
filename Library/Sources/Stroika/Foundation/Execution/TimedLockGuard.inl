/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_TimedLockGuard_inl_
#define _Stroika_Foundation_Execution_TimedLockGuard_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */
            template    <typename MUTEX>
            template    <typename FAILURE_EXCEPTION>
            inline  TimedLockGuard<MUTEX>::TimedLockGuard (MUTEX& m, const Time::Duration& waitUpTo, const FAILURE_EXCEPTION& timeoutException)
                : fMutex_ (m)
            {
                if (not m.try_lock_for (waitUpTo.As<chrono::duration<double>> ())) {
                    Exeuction::DoThrow (timeoutException);
                }
            }
            template    <typename MUTEX>
            inline  TimedLockGuard<MUTEX>::~TimedLockGuard ()
            {
                fMutex_.unlock ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_TimedLockGuard_inl_*/
