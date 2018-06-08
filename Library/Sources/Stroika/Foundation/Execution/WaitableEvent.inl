/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitableEvent_inl_
#define _Stroika_Foundation_Execution_WaitableEvent_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.h"
#include "Finally.h"
#include "ModuleInit.h"
#include "SpinLock.h"
#include "Thread.h"

#if qExecution_WaitableEvent_SupportWaitForMultipleObjects
namespace Stroika {
    namespace Foundation {
        namespace Execution {
            namespace Private_ {
                struct WaitableEvent_ModuleInit_ {
                    SpinLock fExtraWaitableEventsMutex_;
                };
            }
        }
    }
}
namespace {
    extern Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Execution::Private_::WaitableEvent_ModuleInit_> _Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_; // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /*
             ********************************************************************************
             ********************** Execution::WaitableEvent::WE_ ***************************
             ********************************************************************************
             */
            inline WaitableEvent::WE_::WE_ (ResetType resetType)
                : fResetType (resetType)
            {
            }
            inline void WaitableEvent::WE_::Reset ()
            {
                typename ConditionVariable<>::QuickLockType critSection{fConditionVariable.fMutex};
                fTriggered = false;
            }
            inline bool WaitableEvent::WE_::PeekIsSet () const noexcept
            {
                typename ConditionVariable<>::QuickLockType critSection{fConditionVariable.fMutex};
                return fTriggered;
            }
            inline void WaitableEvent::WE_::Set ()
            {
                /**
                 * NOTIFY the condition variable (notify_all), but unlock first due to:                  *
                 *      http://en.cppreference.com/w/cpp/thread/condition_variable/notify_all
                 *
                 *          The notifying thread does not need to hold the lock on the same mutex as the
                 *          one held by the waiting thread(s); in fact doing so is a pessimization, since
                 *          the notified thread would immediately block again, waiting for the notifying
                 *          thread to release the lock.
                 *
                 *  \note https://stroika.atlassian.net/browse/STK-620 - helgrind workaround needed because of this unlock, but the unlock is still correct
                 *
                 */
                {
                    typename ConditionVariable<>::QuickLockType critSection{fConditionVariable.fMutex};
                    fTriggered = true;
                }
                fConditionVariable.notify_all ();
            }

            /*
             ********************************************************************************
             *************************** Execution::WaitableEvent ***************************
             ********************************************************************************
             */
            inline WaitableEvent::WaitableEvent (ResetType resetType)
                : fWE_ (resetType)
            {
            }
            inline void WaitableEvent::Reset ()
            {
                //Debug::TraceContextBumper ctx ("WaitableEvent::Reset");
                fWE_.Reset ();
            }
            inline bool WaitableEvent::PeekIsSet () const noexcept
            {
                return fWE_.PeekIsSet ();
            }
            inline void WaitableEvent::Wait (Time::DurationSecondsType timeout)
            {
                fWE_.WaitUntil (timeout + Time::GetTickCount ());
            }
            inline bool WaitableEvent::WaitQuietly (Time::DurationSecondsType timeout)
            {
                return fWE_.WaitUntilQuietly (timeout + Time::GetTickCount ());
            }
            inline void WaitableEvent::WaitUntil (Time::DurationSecondsType timeoutAt)
            {
                fWE_.WaitUntil (timeoutAt);
            }
            inline bool WaitableEvent::WaitUntilQuietly (Time::DurationSecondsType timeoutAt)
            {
                return fWE_.WaitUntilQuietly (timeoutAt);
            }
#if qExecution_WaitableEvent_SupportWaitForMultipleObjects
            template <typename CONTAINER_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT>
            inline SET_OF_WAITABLE_EVENTS_RESULT WaitableEvent::WaitForAny (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeout)
            {
                return WaitForAnyUntil (waitableEvents, timeout + Time::GetTickCount ());
            }
            template <typename ITERATOR_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT>
            inline SET_OF_WAITABLE_EVENTS_RESULT WaitableEvent::WaitForAny (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeout)
            {
                return WaitForAnyUntil (waitableEventsStart, waitableEventsEnd, timeout + Time::GetTickCount ());
            }
            template <typename CONTAINER_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT>
            inline SET_OF_WAITABLE_EVENTS_RESULT WaitableEvent::WaitForAnyUntil (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeoutAt)
            {
                return WaitForAnyUntil (begin (waitableEvents), end (waitableEvents), timeoutAt);
            }
            template <typename ITERATOR_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT>
            SET_OF_WAITABLE_EVENTS_RESULT WaitableEvent::WaitForAnyUntil (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeoutAt)
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
                shared_ptr<WE_> we      = make_shared<WE_> (eAutoReset);
                [[maybe_unused]] auto&& cleanup = Finally (
                    [ we, waitableEventsStart, waitableEventsEnd ]() noexcept {
                        Thread::SuppressInterruptionInContext suppressThreadInterrupts;
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_);
#else
                        auto critSec{make_unique_lock (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_)};
#endif
                        for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                            (*i)->fExtraWaitableEvents_.remove (we);
                        }
                    });
                {
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                    MACRO_LOCK_GUARD_CONTEXT (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_);
#else
                    auto     critSec{make_unique_lock (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_)};
#endif
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
            inline void WaitableEvent::WaitForAll (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeout)
            {
                WaitForAllUntil (waitableEvents, timeout + Time::GetTickCount ());
            }
            template <typename ITERATOR_OF_WAITABLE_EVENTS>
            inline void WaitableEvent::WaitForAll (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeout)
            {
                WaitForAllUntil (waitableEventsStart, waitableEventsEnd, timeout + Time::GetTickCount ());
            }
            template <typename CONTAINER_OF_WAITABLE_EVENTS>
            inline void WaitableEvent::WaitForAllUntil (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeoutAt)
            {
                WaitForAllUntil (begin (waitableEvents), end (waitableEvents), timeoutAt);
            }
            template <typename ITERATOR_OF_WAITABLE_EVENTS>
            void WaitableEvent::WaitForAllUntil (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeoutAt)
            {
                /*
                 *  Create another WE as shared.
                 *  Stick it onto the list for each waitablevent
                 *  Wait on 'new private fake' enevt (and if it succeeeds, then check orig events and return right now)
                 *  Either way - undo additions
                 *
                 *  <<< @todo DOCUMENT AND EXPLAIN MUTEX >>>
                 */
                shared_ptr<WE_> we      = make_shared<WE_> (eAutoReset);
                auto&&          cleanup = Finally (
                    [ we, waitableEventsStart, waitableEventsEnd ]() noexcept {
                        Thread::SuppressInterruptionInContext suppressThreadInterrupts;
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_);
#else
                        auto critSec{make_unique_lock (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_)};
#endif
                        for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                            (*i)->fExtraWaitableEvents_.remove (we);
                        }
                    });
                {
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                    MACRO_LOCK_GUARD_CONTEXT (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_);
#else
                    auto     critSec{make_unique_lock (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_)};
#endif
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
    }
}

#if qExecution_WaitableEvent_SupportWaitForMultipleObjects
namespace {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Execution::Private_::WaitableEvent_ModuleInit_> _Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_; // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif

#endif /*_Stroika_Foundation_Execution_WaitableEvent_inl_*/
