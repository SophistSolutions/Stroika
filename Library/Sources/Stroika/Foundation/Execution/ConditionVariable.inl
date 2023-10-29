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
        bool IsCurrentThreadInterruptible ();
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
    inline void ConditionVariable<MUTEX, CONDITION_VARIABLE>::release_and_notify_one (LockType& lock) noexcept
    {
        lock.unlock ();
        notify_one ();
    }
    template <typename MUTEX, typename CONDITION_VARIABLE>
    inline void ConditionVariable<MUTEX, CONDITION_VARIABLE>::release_and_notify_all (LockType& lock) noexcept
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
        /*
         *  NOTE: this overload CAN return spurrious wakeups, and just checks timeoutAt to see which cv_status to return.
         */
        Require (lock.owns_lock ());

        Thread::CheckForInterruption ();
        if (Time::GetTickCount () > timeoutAt) [[unlikely]] {
            Ensure (lock.owns_lock ());
            return cv_status::timeout;
        }

        // convert DurationSecondsType to time_point for stdc++ calls, but ping to more modest maximum...
        auto timeoutAtStopPoint = Time::DurationSeconds2time_point (timeoutAt);

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

        // Not all threads are interuptible. For example, the 'main' thread cannot be interrupted or aborted
        // @todo NOTE - this is a defect compared to Stroika v2.1 interupption - where you could interupted but not usefully abort the main thread)
        // But if  kSupportsStopToken, and the current thread supports interruption, we don't get here. So just check the other case
        bool currentThreadIsInterruptible = (not kSupportsStopToken) and Thread::IsCurrentThreadInterruptible ();
        Assert (not kSupportsStopToken or not currentThreadIsInterruptible); // just cuz of tests above

        if (currentThreadIsInterruptible) {
            timeoutAtStopPoint = Time::DurationSeconds2time_point (min (timeoutAt, Time::GetTickCount () + sConditionVariableWaitChunkTime));
        }

        Assert (lock.owns_lock ());

        // If for some reason, we cannot use the stop token (old c++, on main thread or not Stroika thread, or not using condition_variable_any)
        // fall back on basic condition variable code
        (void)fConditionVariable.wait_until (lock, timeoutAtStopPoint);
        Ensure (lock.owns_lock ());

        // cannot use fConditionVariable.wait_until result because we may have fiddled its timeoutAtStopPoint
        // can be spurious wakeup, or real, no way to know
        return (Time::GetTickCount () > timeoutAt) ? cv_status::timeout : cv_status::no_timeout;
    }
    template <typename MUTEX, typename CONDITION_VARIABLE>
    template <invocable PREDICATE>
    bool ConditionVariable<MUTEX, CONDITION_VARIABLE>::wait_until (LockType& lock, Time::DurationSecondsType timeoutAt, PREDICATE&& readyToWake)
    {
        Require (lock.owns_lock ());
        Thread::CheckForInterruption ();

        // Support interruption using the c++20 stop token API, if possible (supported and we are called from a thread with a Thread object
        // whose stop_token we can access)
        if constexpr (kSupportsStopToken) {
#if __cpp_lib_jthread >= 201911
            if (optional<stop_token> ost = Thread::GetCurrentThreadStopToken ()) {
                bool ready = fConditionVariable.wait_until (lock, *ost, Time::DurationSeconds2time_point (timeoutAt), forward<PREDICATE> (readyToWake));
                while (ost->stop_requested () and not ready) {
                    // tricky case.
                    //
                    // We are blocking, waiting for a signaled condition. This thread has been asked to stop. But the only reason why we wouldn't throw in the CheckForInterruption is that it
                    // was suppressed by (SuppressInterruptionInContext).
                    //
                    // This function is NOT permitted to return spurrious interrupts. Just readyToWake return, or timeout.
                    //
                    // If you find yourself looping here - consider if you really wanted to SuppressInterruptionInContext around this!
                    //
                    Thread::CheckForInterruption ();
                    if (Time::GetTickCount () > timeoutAt) {
                        return ready;   // don't throw here - this API doesn't throw timeout...
                    }
                    // must recheck / re-wait ONLY on the condition var itself - no stop token (cuz then this instantly returns and doesn't unlock argument lock so the signaler can progress)
                    ready = fConditionVariable.wait_until (lock, Time::DurationSeconds2time_point (min (timeoutAt, Time::GetTickCount () + sConditionVariableWaitChunkTime)),
                                                           forward<PREDICATE> (readyToWake));
                }
                return ready;
            }
#endif
        }

        // if kSupportsStopToken not in use (or not in a Stroika thread so this thread not stoppable)
        while (not readyToWake ()) {
            Assert (lock.owns_lock ()); // lock owned during readyToWake call and before wait_until call
            // NB: further checks for interruption happen inside wait_until() called here...
            //
            // Another SUBTLE point. we could get here with kSupportsStopToken==false, which might be because of
            // the kind of condition_variable used, etc (many reasons). Point is - we need to be interrupted
            // in 3 cases:
            //      o   timeout
            //      o   interrupted
            //      o   readyToWake() (variable it looks at) changes, which happens spontaneously (other thread wakes us toggling lock).
            //
            //  We DONT need to tweak timeoutAt with sConditionVariableWaitChunkTime (as is done in called wait_until) because
            //  if its possible to handle the interuption case, thats done in called wait_until (possibly using sConditionVariableWaitChunkTime).
            //  if we are woken because of a toggle of lock, we'll get (apparently from point of view of called wait_until) spurrious wakeup and can check
            //  again.
            //
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
        Assert (isinf (timeout) == isinf (timeout + Time::GetTickCount ())); // make sure arithmatic works right with inf
        return wait_until (lock, timeout + Time::GetTickCount ());
    }
    template <typename MUTEX, typename CONDITION_VARIABLE>
    template <invocable PREDICATE>
    inline bool ConditionVariable<MUTEX, CONDITION_VARIABLE>::wait_for (LockType& lock, Time::DurationSecondsType timeout, PREDICATE&& readyToWake)
    {
        Require (lock.owns_lock ());
        Assert (isinf (timeout) == isinf (timeout + Time::GetTickCount ())); // make sure arithmatic works right with inf
        return wait_until (lock, timeout + Time::GetTickCount (), forward<PREDICATE> (readyToWake));
    }
    template <typename MUTEX, typename CONDITION_VARIABLE>
    template <invocable FUNCTION>
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
    template <invocable FUNCTION>
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
