/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ConditionVariable_inl_
#define _Stroika_Foundation_Execution_ConditionVariable_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Time/Duration.h"

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            void CheckForThreadInterruption ();

            /*
             ********************************************************************************
             **************** ConditionVariable<MUTEX, CONDITION_VARIABLE> ******************
             ********************************************************************************
             */
            template <typename MUTEX, typename CONDITION_VARIABLE>
            Time::DurationSecondsType ConditionVariable<MUTEX, CONDITION_VARIABLE>::sThreadAbortCheckFrequency_Default{1};

            template <typename MUTEX, typename CONDITION_VARIABLE>
            inline void ConditionVariable<MUTEX, CONDITION_VARIABLE>::release_and_notify_one (LockType& lock)
            {
                lock.unlock ();
                notify_one ();
            }
            template <typename MUTEX, typename CONDITION_VARIABLE>
            inline void ConditionVariable<MUTEX, CONDITION_VARIABLE>::release_and_notify_all (LockType& lock)
            {
                lock.unlock ();
                notify_all ();
            }
            template <typename MUTEX, typename CONDITION_VARIABLE>
            inline void ConditionVariable<MUTEX, CONDITION_VARIABLE>::notify_one () noexcept
            {
                fConditionVariable.notify_one ();
            }
            template <typename MUTEX, typename CONDITION_VARIABLE>
            inline void ConditionVariable<MUTEX, CONDITION_VARIABLE>::notify_all () noexcept
            {
                fConditionVariable.notify_all ();
            }
            template <typename MUTEX, typename CONDITION_VARIABLE>
            cv_status ConditionVariable<MUTEX, CONDITION_VARIABLE>::wait_until (LockType& lock, Time::DurationSecondsType timeoutAt)
            {
                Require (lock.owns_lock ());
                // @todo WRONG - must redo the loop below - not using predicate to handle timeout right
                unsigned int cntCalled = 0;
                return wait_until (lock, timeoutAt, [&cntCalled]() { return ++cntCalled > 1; }) ? cv_status::no_timeout : cv_status::timeout;
            }
            template <typename MUTEX, typename CONDITION_VARIABLE>
            template <typename PREDICATE>
            bool ConditionVariable<MUTEX, CONDITION_VARIABLE>::wait_until (LockType& lock, Time::DurationSecondsType timeoutAt, PREDICATE readyToWake)
            {
                Require (lock.owns_lock ());
                while (not readyToWake ()) {
                    CheckForThreadInterruption ();
                    Time::DurationSecondsType remaining = timeoutAt - Time::GetTickCount ();
                    if (remaining < 0) {
                        return false; // maybe should recheck readyToWake () but wait_until/2 assumes NO for now
                    }
                    remaining = min (remaining, fThreadAbortCheckFrequency);

                    Assert (lock.owns_lock ());
                    if (fConditionVariable.wait_for (lock, Time::Duration (remaining).As<std::chrono::milliseconds> ()) == std::cv_status::timeout) {
                        /*
                         *  Cannot quit here because we trim time to wait so we can re-check for thread aborting. No need to pay attention to
                         *  this timeout value (or any return code) - cuz we re-examine pred () and tickcount.
                         */
                    }
                    else {
                        // https://stroika.atlassian.net/browse/STK-623
                        // @todo DOCUMENT WHY - when can get spurrious wakeups
                        ////no break - recheck pred();;;; spurrious??? break; // if not a timeout - condition variable really signaled, we really return
                    }
                    Assert (lock.owns_lock ());
                }
                return true;
            }
            template <typename MUTEX, typename CONDITION_VARIABLE>
            inline cv_status ConditionVariable<MUTEX, CONDITION_VARIABLE>::wait_for (LockType& lock, Time::DurationSecondsType timeout)
            {
                Require (lock.owns_lock ());
                return wait_until (lock, timeout + Time::GetTickCount ());
            }
            template <typename MUTEX, typename CONDITION_VARIABLE>
            template <typename PREDICATE>
            bool ConditionVariable<MUTEX, CONDITION_VARIABLE>::wait_for (LockType& lock, Time::DurationSecondsType timeout, PREDICATE readyToWake)
            {
                Require (lock.owns_lock ());
                return wait_until (lock, timeout + Time::GetTickCount (), std::move (readyToWake));
            }
            template <typename MUTEX, typename CONDITION_VARIABLE>
            template <typename FUNCTION>
            void ConditionVariable<MUTEX, CONDITION_VARIABLE>::MutateDataNotifyAll (FUNCTION mutatorFunction)
            {
                {
                    QuickLockType quickLock{fMutex};
                    mutatorFunction ();
                }
                fConditionVariable.notify_all ();
            }
            template <typename MUTEX, typename CONDITION_VARIABLE>
            template <typename FUNCTION>
            void ConditionVariable<MUTEX, CONDITION_VARIABLE>::MutateDataNotifyOne (FUNCTION mutatorFunction)
            {
                {
                    QuickLockType quickLock{fMutex};
                    mutatorFunction ();
                }
                fConditionVariable.notify_one ();
            }
        }
    }
}
#endif /*_Stroika_Foundation_Execution_ConditionVariable_inl_*/
