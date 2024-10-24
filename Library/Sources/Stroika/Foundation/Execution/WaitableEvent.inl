/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Common.h"

#if qExecution_WaitableEvent_SupportWaitForMultipleObjects
#include "Finally.h"
#include "Thread.h"
#endif

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ********************** Execution::WaitableEvent::WE_ ***************************
     ********************************************************************************
     */
    inline void WaitableEvent::WE_::Reset ()
    {
        /*
         *  NOTE: this code does NOT do a fConditionVariable.MutateDataNotifyAll () - because we dont want anyone to WAKE to discover new data. Resetting the waitable event
         *  is meant to be like destroying it and starting over.
         */
        typename ConditionVariable<>::QuickLockType critSection{fConditionVariable.fMutex};
        fTriggered = false;
    }
    inline bool WaitableEvent::WE_::GetIsSet () const noexcept
    {
        typename ConditionVariable<>::QuickLockType critSection{fConditionVariable.fMutex};
        return fTriggered;
    }
    inline bool WaitableEvent::WE_::PeekIsSet () const noexcept
    {
        return fTriggered;
    }
    inline void WaitableEvent::WE_::Set ()
    {
        fConditionVariable.MutateDataNotifyAll ([this] () { fTriggered = true; });
    }

    /*
     ********************************************************************************
     *************************** Execution::WaitableEvent ***************************
     ********************************************************************************
     */
    inline WaitableEvent::WaitableEvent ()
        : fWE_{}
    {
    }
    inline void WaitableEvent::Reset ()
    {
        //Debug::TraceContextBumper ctx{"WaitableEvent::Reset"};
        fWE_.Reset ();
    }
    inline bool WaitableEvent::GetIsSet () const noexcept
    {
        return fWE_.GetIsSet ();
    }
    inline bool WaitableEvent::PeekIsSet () const noexcept
    {
        return fWE_.PeekIsSet ();
    }
    inline void WaitableEvent::Wait (Time::DurationSeconds timeout)
    {
        fWE_.WaitUntil (timeout + Time::GetTickCount ());
    }
    inline auto WaitableEvent::WaitQuietly (Time::DurationSeconds timeout) -> WaitStatus
    {
        return fWE_.WaitUntilQuietly (timeout + Time::GetTickCount ());
    }
    inline void WaitableEvent::WaitUntil (Time::TimePointSeconds timeoutAt)
    {
        fWE_.WaitUntil (timeoutAt);
    }
    inline auto WaitableEvent::WaitUntilQuietly (Time::TimePointSeconds timeoutAt) -> WaitStatus
    {
        return fWE_.WaitUntilQuietly (timeoutAt);
    }
    inline void WaitableEvent::WaitAndReset (Time::Duration timeout)
    {
        Wait (timeout);
        //Require (fWE_.fTriggered); // This wait-and-reset approach only really works with a single waiter (else this is a race) - so caller must use that way
        Reset ();
    }
    inline void WaitableEvent::WaitUntilAndReset (Time::TimePointSeconds timeoutAt)
    {
        WaitUntil (timeoutAt);
        //Require (fWE_.fTriggered); // This wait-and-reset approach only really works with a single waiter (else this is a race) - so caller must use that way
        Reset ();
    }
    inline auto WaitableEvent::WaitQuietlyAndReset (const Time::Duration& timeout) -> WaitStatus
    {
        auto r = WaitQuietly (timeout);
        // Require (fWE_.fTriggered); // This wait-and-reset approach only really works with a single waiter (else this is a race) - so caller must use that way
        Reset ();
        return r;
    }
    inline auto WaitableEvent::WaitUntilQuietlyAndReset (Time::TimePointSeconds timeoutAt) -> WaitStatus
    {
        auto r = WaitUntilQuietly (timeoutAt);
        // Require (fWE_.fTriggered); // This wait-and-reset approach only really works with a single waiter (else this is a race) - so caller must use that way
        Reset ();
        return r;
    }
#if qExecution_WaitableEvent_SupportWaitForMultipleObjects
    template <typename CONTAINER_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT>
    inline SET_OF_WAITABLE_EVENTS_RESULT WaitableEvent::WaitForAny (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSeconds timeout)
    {
        return WaitForAnyUntil (waitableEvents, timeout + Time::GetTickCount ());
    }
    template <typename ITERATOR_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT>
    inline SET_OF_WAITABLE_EVENTS_RESULT WaitableEvent::WaitForAny (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart,
                                                                    ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSeconds timeout)
    {
        return WaitForAnyUntil (waitableEventsStart, waitableEventsEnd, timeout + Time::GetTickCount ());
    }
    template <typename CONTAINER_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT>
    inline SET_OF_WAITABLE_EVENTS_RESULT WaitableEvent::WaitForAnyUntil (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::TimePointSeconds timeoutAt)
    {
        return WaitForAnyUntil (begin (waitableEvents), end (waitableEvents), timeoutAt);
    }
    template <typename ITERATOR_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT>
    SET_OF_WAITABLE_EVENTS_RESULT WaitableEvent::WaitForAnyUntil (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart,
                                                                  ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::TimePointSeconds timeoutAt)
    {
        SET_OF_WAITABLE_EVENTS_RESULT result;
        /*
         *  Create another WE as shared.
         *  Stick it onto the list for each waitablevent
         *  Wait on 'new private fake' enevt (and if it succeeeds, then check orig events and return right now)
         *  Either way - undo additions
         *
         *  <<< @todo DOCUMENT AND EXPLAIN MUTEX >>>
         */
        shared_ptr<WE_>         we      = make_shared<WE_> (eAutoReset);
        [[maybe_unused]] auto&& cleanup = Finally ([we, waitableEventsStart, waitableEventsEnd] () noexcept {
            Thread::SuppressInterruptionInContext suppressThreadInterrupts;
            [[maybe_unused]] auto&&               critSec = lock_guard{sExtraWaitableEventsMutex_};
            for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                (*i)->fExtraWaitableEvents_.remove (we);
            }
        });
        {
            [[maybe_unused]] lock_guard critSec{sExtraWaitableEventsMutex_};
            for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                (*i)->fExtraWaitableEvents_.push_front (we);
            }
        }
        while (result.empty ()) {
            we->WaitUntil (timeoutAt);
            for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                WaitableEvent* we2Test = *i;
                if (we2Test->fWE_.PeekIsSet ()) {
                    if (we2Test->fWE_.fResetType == eAutoReset) {
                        we2Test->fWE_.Reset ();
                    }
                    result.insert (we2Test);
                }
            }
        }
        return result;
    }
    template <typename CONTAINER_OF_WAITABLE_EVENTS>
    inline void WaitableEvent::WaitForAll (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSeconds timeout)
    {
        WaitForAllUntil (waitableEvents, timeout + Time::GetTickCount ());
    }
    template <typename ITERATOR_OF_WAITABLE_EVENTS>
    inline void WaitableEvent::WaitForAll (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd,
                                           Time::DurationSeconds timeout)
    {
        WaitForAllUntil (waitableEventsStart, waitableEventsEnd, timeout + Time::GetTickCount ());
    }
    template <typename CONTAINER_OF_WAITABLE_EVENTS>
    inline void WaitableEvent::WaitForAllUntil (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::TimePointSeconds timeoutAt)
    {
        WaitForAllUntil (begin (waitableEvents), end (waitableEvents), timeoutAt);
    }
    template <typename ITERATOR_OF_WAITABLE_EVENTS>
    void WaitableEvent::WaitForAllUntil (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd,
                                         Time::TimePointSeconds timeoutAt)
    {
        /*
         *  Create another WE as shared.
         *  Stick it onto the list for each waitablevent
         *  Wait on 'new private fake' enevt (and if it succeeeds, then check orig events and return right now)
         *  Either way - undo additions
         *
         *  <<< @todo DOCUMENT AND EXPLAIN MUTEX >>>
         */
        shared_ptr<WE_>         we      = make_shared<WE_> (eAutoReset);
        [[maybe_unused]] auto&& cleanup = Finally ([we, waitableEventsStart, waitableEventsEnd] () noexcept {
            Thread::SuppressInterruptionInContext suppressThreadInterrupts;
            [[maybe_unused]] auto&&               critSec = lock_guard{sExtraWaitableEventsMutex_};
            for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                (*i)->fExtraWaitableEvents_.remove (we);
            }
        });
        {
            [[maybe_unused]] lock_guard critSec{sExtraWaitableEventsMutex_};
            for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                (*i)->fExtraWaitableEvents_.push_front (we);
            }
        }
        while (true) {
            we->WaitUntil (timeoutAt);
            bool anyStillWaiting = false;
            for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                WaitableEvent* we2Test = *i;
                RequireNotNull (we2Test);
                if (not we2Test->fWE_.PeekIsSet ()) {
                    anyStillWaiting = true;
                }
            }
            if (anyStillWaiting) {
                continue;
            }
            else {
                for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                    WaitableEvent* we2Test = *i;
                    RequireNotNull (we2Test);
                    if (we2Test->fWE_.fResetType == eAutoReset) {
                        Assert (we2Test->fWE_.PeekIsSet ()); // we probably need some mechanism to assure this is true but we currently have no enforcement of this!
                        we2Test->fWE_.Reset ();
                    }
                }
                return; // success
            }
        }
    }
#endif
}

namespace Stroika::Foundation::Configuration {
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    template <>
    constexpr EnumNames<Execution::WaitableEvent::ResetType> DefaultNames<Execution::WaitableEvent::ResetType>::k{{{
        {Execution::WaitableEvent::ResetType::eAutoReset, L"AutoReset"},
        {Execution::WaitableEvent::ResetType::eManualReset, L"ManualReset"},
    }}};
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    template <>
    constexpr EnumNames<Execution::WaitableEvent::WaitStatus> DefaultNames<Execution::WaitableEvent::WaitStatus>::k{{{
        {Execution::WaitableEvent::WaitStatus::eTimeout, L"Timeout"},
        {Execution::WaitableEvent::WaitStatus::eTriggered, L"Triggered"},
    }}};
}
