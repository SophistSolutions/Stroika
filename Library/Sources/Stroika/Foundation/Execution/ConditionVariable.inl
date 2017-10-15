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
             *************************** ConditionVariable<MUTEX> ***************************
             ********************************************************************************
             */
            template <typename MUTEX>
            Time::DurationSecondsType ConditionVariable<MUTEX>::sThreadAbortCheckFrequency_Default{1};

            template <typename MUTEX>
            inline void ConditionVariable<MUTEX>::release_and_notify_one (LockType& lock)
            {
                lock.unlock ();
                fConditionVariable.notify_one ();
            }
            template <typename MUTEX>
            inline void ConditionVariable<MUTEX>::release_and_notify_all (LockType& lock)
            {
                lock.unlock ();
                fConditionVariable.notify_all ();
            }
            template <typename MUTEX>
            cv_status ConditionVariable<MUTEX>::wait_until (LockType& lock, Time::DurationSecondsType timeoutAt)
            {
                while (true) {
                    CheckForThreadInterruption ();
                    Time::DurationSecondsType remaining = timeoutAt - Time::GetTickCount ();
                    if (remaining < 0) {
                        return cv_status::timeout;
                    }
                    remaining = min (remaining, fThreadAbortCheckFrequency);

                    if (fConditionVariable.wait_for (lock, Time::Duration (remaining).As<std::chrono::milliseconds> ()) == std::cv_status::timeout) {
                        /*
                         *  Cannot throw here because we trim time to wait so we can re-check for thread aborting. No need to pay attention to
                         *  this timeout value (or any return code) - cuz we re-examine fTriggered and tickcount.
                         */
                    }
                    else {
                        // if not a timeout, we really return
                        return cv_status::no_timeout;
                    }
                }
            }
            template <typename MUTEX>
            template <typename _Predicate>
            bool ConditionVariable<MUTEX>::wait_until (LockType& lock, Time::DurationSecondsType timeoutAt, PREDICATE pred)
            {
                while (not pred ()) {
                    if (wait_until (lock, timeoutAt) == cv_status::timeout) {
                        return pred ();
                    }
                }
                return true;
            }
            template <typename MUTEX>
            inline cv_status ConditionVariable<MUTEX>::wait_for (LockType& lock, Time::DurationSecondsType timeout)
            {
                return wait_until (lock, timeout + Time::GetTickCount ());
            }
            template <typename MUTEX>
            template <typename _Predicate>
            bool ConditionVariable<MUTEX>::wait_for (LockType& lock, Time::DurationSecondsType timeout, PREDICATE pred)
            {
                return wait_until (lock, timeout + Time::GetTickCount (), std::move (pred));
            }
            template <typename MUTEX>
            template <typename FUNCTION>
            void ConditionVariable<MUTEX>::MutateDataNotifyAll (FUNCTION mutatorFunction)
            {
                {
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                    MACRO_LOCK_GUARD_CONTEXT (fMutex);
#else
                    auto critSec{make_unique_lock (fMutex)};
#endif
                    doIt ();
                }
                fConditionVariable.notify_all ();
            }
            template <typename MUTEX>
            template <typename FUNCTION>
            void ConditionVariable<MUTEX>::MutateDataNotifyOne (FUNCTION mutatorFunction)
            {
                {
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                    MACRO_LOCK_GUARD_CONTEXT (fMutex);
#else
                    auto critSec{make_unique_lock (fMutex)};
#endif
                    doIt ();
                }
                fConditionVariable.notify_one ();
            }
        }
    }
}
#endif /*_Stroika_Foundation_Execution_ConditionVariable_inl_*/
