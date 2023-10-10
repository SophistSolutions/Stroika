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

        Thread::CheckForInterruption ();
        if (Time::GetTickCount () > timeoutAt) [[unlikely]] {
            Ensure (lock.owns_lock ());
            return cv_status::timeout;
        }

        // convert DurationSecondsType  to time_point for stdc++ calls, but ping to more modest maximum...
        auto timeoutAtStopPoint = Time::DurationSeconds2time_point (
            kSupportsStopToken ? timeoutAt : min (timeoutAt, Time::GetTickCount () + sThreadAbortCheckFrequency_Default));

        if constexpr (kSupportsStopToken) {
            // If no predicate function is provided (to say when we are done) - use stop_requested() as the predicate
#if __cpp_lib_jthread >= 201911
            if (optional<stop_token> ost = Thread::GetCurrentThreadStopToken ()) {
                if (fConditionVariable.wait_until (lock, *ost, timeoutAtStopPoint, [&] () { return ost->stop_requested (); })) [[unlikely]] {
                    Thread::CheckForInterruption ();
                }
                return (Time::GetTickCount () < timeoutAt) ? cv_status::no_timeout : cv_status::timeout;
            }
#endif
        }

        Thread::CheckForInterruption ();
        (void)fConditionVariable.wait_until (lock, timeoutAtStopPoint);

        Ensure (lock.owns_lock ());

        // cannot use fConditionVariable.wait_until result because we may have fiddled its argument
        // can be spurious wakeup, or real, no way to know
        return (Time::GetTickCount () > timeoutAt) ? cv_status::timeout : cv_status::no_timeout;
    }
    template <typename MUTEX, typename CONDITION_VARIABLE>
    template <typename PREDICATE>
    bool ConditionVariable<MUTEX, CONDITION_VARIABLE>::wait_until (LockType& lock, Time::DurationSecondsType timeoutAt, PREDICATE&& readyToWake)
    {
        Require (lock.owns_lock ());
        Thread::CheckForInterruption ();

        auto timeoutAtStopPoint = Time::DurationSeconds2time_point (timeoutAt);

        // native std c++ fConditionVariable.wait_until works with stop token, but my version in overload wtih no predicate doesn't - perhaps
        // critucal to hold lock whole predicate checked? which I think I'm doing here, but maybe review lib code more carefully... cuz this case
        // works and mine doesn't...
        if constexpr (kSupportsStopToken) {
#if __cpp_lib_jthread >= 201911
            if (optional<stop_token> ost = Thread::GetCurrentThreadStopToken ()) {
                bool ready = fConditionVariable.wait_until (lock, *ost, timeoutAtStopPoint, forward<PREDICATE> (readyToWake));
                if (ost->stop_requested ()) {
                    Thread::CheckForInterruption ();
                }
                return ready;
            }
#endif
        }

        // if kSupportsStopToken not in use (or not in a Stroika thread so this thread not stoppable)
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
    inline bool ConditionVariable<MUTEX, CONDITION_VARIABLE>::wait_for (LockType& lock, Time::DurationSecondsType timeout, PREDICATE&& readyToWake)
    {
        Require (lock.owns_lock ());
        return wait_until (lock, timeout + Time::GetTickCount (), forward<PREDICATE> (readyToWake));
    }
    template <typename MUTEX, typename CONDITION_VARIABLE>
    template <typename FUNCTION>
    inline void ConditionVariable<MUTEX, CONDITION_VARIABLE>::MutateDataNotifyAll (FUNCTION&& mutatorFunction)
    {
        // See https://en.cppreference.com/w/cpp/thread/condition_variable for why we modify the data under the lock
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
        // See https://en.cppreference.com/w/cpp/thread/condition_variable for why we modify the data under the lock
        // but call the notify_all() after releasing the lock - also https://stackoverflow.com/questions/35775501/c-should-condition-variable-be-notified-under-lock
        {
            QuickLockType quickLock{fMutex};
            forward<FUNCTION> (mutatorFunction) ();
        }
        fConditionVariable.notify_one ();
    }

}

#endif /*_Stroika_Foundation_Execution_ConditionVariable_inl_*/
