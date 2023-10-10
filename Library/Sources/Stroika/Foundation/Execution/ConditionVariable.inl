/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ConditionVariable_inl_
#define _Stroika_Foundation_Execution_ConditionVariable_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Time/Duration.h"

namespace Stroika::Foundation::Execution {

    namespace Thread {
        void CheckForInterruption ();
#if __cpp_lib_jthread >= 201911
        optional<stop_token> GetCurrentThreadStopToken ();
#endif
    }

    /*
     ********************************************************************************
     **************** ConditionVariable<MUTEX, CONDITION_VARIABLE> ******************
     ********************************************************************************
     */
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
        {
            Thread::CheckForInterruption ();
            Time::DurationSecondsType remaining = timeoutAt - Time::GetTickCount ();
            if (remaining < 0) {
                Ensure (lock.owns_lock ());
                return cv_status::timeout;
            }

            if constexpr (kSupportsStopToken) {
                // native std c++ fConditionVariable.wait_until works with stop token, but my version in overload wtih no predicate doesn't - perhaps
                // critucal to hold lock whole predicate checked? which I think I'm doing here, but maybe review lib code more carefully... cuz this case
                // works and mine doesn't...
                if (optional<stop_token> ost = Thread::GetCurrentThreadStopToken ()) {
                    if (fConditionVariable.wait_until (lock, *ost, Time::DurationSeconds2time_point (timeoutAt),
                                                       [&] () { return ost->stop_requested (); })) {
                        Thread::CheckForInterruption ();
                    }
                    return (Time::GetTickCount () < timeoutAt) ? cv_status::no_timeout : cv_status::timeout;
                }
            }

            // @todo DOC THIS BETTER
            if (kSupportsStopToken) {
                remaining = min (remaining, 60.0 * 60.0);
            }
            else {
                remaining = min (remaining, sThreadAbortCheckFrequency_Default);
            }
            Assert (not isinf (remaining));

            Thread::CheckForInterruption ();
            // @todo simplify to call wait_until() after we lose __cpp_lib_jthread < 201911 support
            Assert (lock.owns_lock ());
            (void)fConditionVariable.wait_for (lock, Time::Duration{remaining}.As<chrono::milliseconds> ());
            Assert (lock.owns_lock ());
            {
                /*
                 *  Cannot quit here because we trim time to wait so we can re-check for thread aborting. No need to pay attention to
                 *  this timeout value (or any return code) - cuz we re-examine tickcount at the top of the loop.
                 */
                {
                    Time::DurationSecondsType stillRemaining = timeoutAt - Time::GetTickCount ();
                    if (stillRemaining < 0) {
                        return cv_status::timeout;
                    }
                    Ensure (lock.owns_lock ());
                    return cv_status::no_timeout; // can be spurious wakeup, or real, no way to know
                }
            }
        }
    }
    template <typename MUTEX, typename CONDITION_VARIABLE>
    template <typename PREDICATE>
    bool ConditionVariable<MUTEX, CONDITION_VARIABLE>::wait_until (LockType& lock, Time::DurationSecondsType timeoutAt, PREDICATE&& readyToWake)
    {
        Require (lock.owns_lock ());
        Thread::CheckForInterruption ();

#if 1
        // native std c++ fConditionVariable.wait_until works with stop token, but my version in overload wtih no predicate doesn't - perhaps
        // critucal to hold lock whole predicate checked? which I think I'm doing here, but maybe review lib code more carefully... cuz this case
        // works and mine doesn't...
        if constexpr (kSupportsStopToken) {
            if (optional<stop_token> ost = Thread::GetCurrentThreadStopToken ()) {
                auto r = fConditionVariable.wait_until (lock, *ost, Time::DurationSeconds2time_point (timeoutAt), forward<PREDICATE> (readyToWake));
                Thread::CheckForInterruption ();
                return r;
            }
        }
#endif

        while (not readyToWake ()) {
            // NB: further checks for interruption happen inside wait_until() called here...
            if (wait_until (lock, timeoutAt) == cv_status::timeout) {
                /*
                 *  Somewhat ambiguous if this should check readyToWake or just return false. Probably best to check, since the condition is met, and thats
                 *  probably more important than the timeout.
                 * 
                 *  Also - docs in https://en.cppreference.com/w/cpp/thread/condition_variable/wait_until - make it clear this is the right thing todo.
                 */
                auto result = readyToWake ();
                Ensure (lock.owns_lock ());
                return result;
            }
            // Maybe a real wakeup, or a spurious one, so just check the readyToWake() predicate again, and keep looping!
        }
        Ensure (lock.owns_lock ());
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
        return wait_until (lock, timeout + Time::GetTickCount (), move (readyToWake));
    }
    template <typename MUTEX, typename CONDITION_VARIABLE>
    template <typename FUNCTION>
    inline void ConditionVariable<MUTEX, CONDITION_VARIABLE>::MutateDataNotifyAll (FUNCTION&& mutatorFunction)
    {
        // See https://en.cppreference.com/w/cpp/thread/condition_variable for why we modify the data under the lock (maybe obvious)
        // but call the notify_all() after releasing the lock - also https://stackoverflow.com/questions/35775501/c-should-condition-variable-be-notified-under-lock
        {
            QuickLockType quickLock{fMutex};
            forward<FUNCTION> (mutatorFunction) ();
        }
        fConditionVariable.notify_all ();
    }
    template <typename MUTEX, typename CONDITION_VARIABLE>
    template <typename FUNCTION>
    inline void ConditionVariable<MUTEX, CONDITION_VARIABLE>::MutateDataNotifyOne (FUNCTION&& mutatorFunction)
    {
        // See https://en.cppreference.com/w/cpp/thread/condition_variable for why we modify the data under the lock (maybe obvious)
        // but call the notify_all() after releasing the lock - also https://stackoverflow.com/questions/35775501/c-should-condition-variable-be-notified-under-lock
        {
            QuickLockType quickLock{fMutex};
            forward<FUNCTION> (mutatorFunction) ();
        }
        fConditionVariable.notify_one ();
    }

}

#endif /*_Stroika_Foundation_Execution_ConditionVariable_inl_*/
