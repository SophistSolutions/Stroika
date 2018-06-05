/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
            Time::DurationSecondsType ConditionVariable<MUTEX, CONDITION_VARIABLE>::sThreadAbortCheckFrequency_Default{2.5};

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
                while (true) {
                    CheckForThreadInterruption ();
                    Time::DurationSecondsType remaining = timeoutAt - Time::GetTickCount ();
                    if (remaining < 0) {
                        return cv_status::timeout;
                    }
                    remaining = min (remaining, fThreadAbortCheckFrequency);

                    Assert (lock.owns_lock ());
                    std::cv_status tmp = fConditionVariable.wait_for (lock, Time::Duration (remaining).As<std::chrono::milliseconds> ());
                    Assert (lock.owns_lock ());
                    if (tmp == std::cv_status::timeout) {
                        /*
                         *  Cannot quit here because we trim time to wait so we can re-check for thread aborting. No need to pay attention to
                         *  this timeout value (or any return code) - cuz we re-examine tickcount at the top of the loop.
                         */
                        {
                            // TEST POSSIBLE FIX/WORKAROUND FOR https://stroika.atlassian.net/browse/STK-629
                            Time::DurationSecondsType stillRemaining = timeoutAt - Time::GetTickCount ();
                            if (stillRemaining < 0) {
                                return cv_status::timeout;
                            }
                            return std::cv_status::no_timeout; // can be spurious wakeup, or real, no way to know
                        }
                    }
                    else {
                        Assert (tmp == std::cv_status::no_timeout);
                        return std::cv_status::no_timeout; // can be spurious wakeup, or real, no way to know
                    }
                }
            }
            template <typename MUTEX, typename CONDITION_VARIABLE>
            template <typename PREDICATE>
            bool ConditionVariable<MUTEX, CONDITION_VARIABLE>::wait_until (LockType& lock, Time::DurationSecondsType timeoutAt, PREDICATE readyToWake)
            {
                Require (lock.owns_lock ());
                CheckForThreadInterruption ();
                while (not readyToWake ()) {
                    // NB: further checks for interruption happen inside wait_until() called here...
                    if (wait_until (lock, timeoutAt) == cv_status::timeout) {
                        /*
                         *  Somewhat ambiguous if this should check readyToWake just return false. Probably best to check, since the condition is met, and thats
                         *  probably more important than the timeout.
                         */
                        return readyToWake ();
                    }
                    // Maybe a real wakeup, or a spurious one, so just check the readyToWake() predicate again, and keep looping!
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
            inline bool ConditionVariable<MUTEX, CONDITION_VARIABLE>::wait_for (LockType& lock, Time::DurationSecondsType timeout, PREDICATE readyToWake)
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
