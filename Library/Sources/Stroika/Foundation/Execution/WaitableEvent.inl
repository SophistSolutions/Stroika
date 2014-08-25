/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitableEvent_inl_
#define _Stroika_Foundation_Execution_WaitableEvent_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Finally.h"
#include    "ModuleInit.h"


#if     qExecution_WaitableEvent_SupportWaitForMultipleObjects
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace Private_ {
                struct  WaitableEvent_ModuleInit_ {
                    mutex     fExtraWaitableEventsMutex_;
                };
            }
        }
    }
}
namespace   {
    extern  Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Execution::Private_::WaitableEvent_ModuleInit_>   _Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            //redeclare to avoid having to #include Thread.h
            void    CheckForThreadAborting ();


            /*
             ********************************************************************************
             ********************** Execution::WaitableEvent::WE_ ***************************
             ********************************************************************************
             */
            inline  WaitableEvent::WE_::WE_ (ResetType resetType)
                : fResetType (resetType)
            {
            }
            inline  void    WaitableEvent::WE_::Reset ()
            {
                auto    critSec { make_unique_lock (fMutex) };
                fTriggered = false;
            }
            inline  void    WaitableEvent::WE_::Set ()
            {
                auto    critSec { make_unique_lock (fMutex) };
                fTriggered = true;
                fConditionVariable.notify_all ();
            }


            /*
             ********************************************************************************
             *************************** Execution::WaitableEvent ***************************
             ********************************************************************************
             */
            inline  WaitableEvent::WaitableEvent (ResetType resetType)
                : fWE_ (resetType)
            {
            }
            inline  void    WaitableEvent::Reset ()
            {
                //Debug::TraceContextBumper ctx (SDKSTR ("WaitableEvent::Reset"));
                fWE_.Reset ();
            }
            inline  void    WaitableEvent::Wait (Time::DurationSecondsType timeout)
            {
                fWE_.WaitUntil (timeout + Time::GetTickCount ());
            }
            inline  void    WaitableEvent::WaitUntil (Time::DurationSecondsType timeoutAt)
            {
                fWE_.WaitUntil (timeoutAt);
            }
#if     qExecution_WaitableEvent_SupportWaitForMultipleObjects
            template    <typename CONTAINER_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT>
            inline  SET_OF_WAITABLE_EVENTS_RESULT  WaitableEvent::WaitForAny (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeout)
            {
                return WaitForAnyUntil (waitableEvents, timeout + Time::GetTickCount ());
            }
            template    <typename ITERATOR_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT>
            inline  SET_OF_WAITABLE_EVENTS_RESULT  WaitableEvent::WaitForAny (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeout)
            {
                return WaitForAnyUntil (waitableEventsStart, waitableEventsEnd, timeout + Time::GetTickCount ());
            }
            template    <typename CONTAINER_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT>
            inline  SET_OF_WAITABLE_EVENTS_RESULT  WaitableEvent::WaitForAnyUntil (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeoutAt)
            {
                return WaitForAnyUntil (begin (waitableEvents), end (waitableEvents), timeoutAt);
            }
            template    <typename ITERATOR_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT>
            SET_OF_WAITABLE_EVENTS_RESULT  WaitableEvent::WaitForAnyUntil (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeoutAt)
            {
                SET_OF_WAITABLE_EVENTS_RESULT   result;
                /*
                 *  Create another WE as shared.
                 *  Stick it onto the list for each waitablevent
                 *  Wait on 'new private fake' enevt (and if it succeeeds, then check orig events and return right now)
                 *  Either way - undo additions
                 *
                 *  <<< @todo DOCUMENT AND EXPLAIN MUTEX >>>
                 */
                shared_ptr<WE_> we  =   shared_ptr<WE_> (new WE_ (eAutoReset));
                Execution::Finally cleanup ([we, waitableEventsStart, waitableEventsEnd] () {
                    auto    critSec { make_unique_lock (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_) };
                    for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                        (*i)->fExtraWaitableEvents_.remove (we);
                    }
                });
                {
                    auto    critSec { make_unique_lock (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_) };
                    for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                        (*i)->fExtraWaitableEvents_.push_front (we);
                    }
                }
                while (result.empty ()) {
                    we->WaitUntil (timeoutAt);
                    for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                        WaitableEvent*  we2Test =   *i;
                        if (we2Test->fWE_.fTriggered) {
                            if (we2Test->fWE_.fResetType == eAutoReset) {
                                we2Test->fWE_.Reset ();
                            }
                            result.insert (we2Test);
                        }
                    }
                }
                return result;
            }
            template    <typename CONTAINER_OF_WAITABLE_EVENTS>
            inline  void    WaitableEvent::WaitForAll (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeout)
            {
                WaitForAllUntil (waitableEvents, timeout + Time::GetTickCount ());
            }
            template    <typename ITERATOR_OF_WAITABLE_EVENTS>
            inline  void    WaitableEvent::WaitForAll (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeout)
            {
                WaitForAllUntil (waitableEventsStart, waitableEventsEnd, timeout + Time::GetTickCount ());
            }
            template    <typename CONTAINER_OF_WAITABLE_EVENTS>
            inline  void    WaitableEvent::WaitForAllUntil (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeoutAt)
            {
                WaitForAllUntil (begin (waitableEvents), end (waitableEvents), timeoutAt);
            }
            template    <typename ITERATOR_OF_WAITABLE_EVENTS>
            void    WaitableEvent::WaitForAllUntil (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeoutAt)
            {
                /*
                 *  Create another WE as shared.
                 *  Stick it onto the list for each waitablevent
                 *  Wait on 'new private fake' enevt (and if it succeeeds, then check orig events and return right now)
                 *  Either way - undo additions
                 *
                 *  <<< @todo DOCUMENT AND EXPLAIN MUTEX >>>
                 */
                shared_ptr<WE_> we  =   shared_ptr<WE_> (new WE_ (eAutoReset));
                Execution::Finally cleanup ([we, waitableEventsStart, waitableEventsEnd] () {
                    auto    critSec { make_unique_lock (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_) };
                    for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                        (*i)->fExtraWaitableEvents_.remove (we);
                    }
                });
                {
                    auto    critSec { make_unique_lock (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_) };
                    for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                        (*i)->fExtraWaitableEvents_.push_front (we);
                    }
                }
                while (true) {
                    we->WaitUntil (timeoutAt);
                    bool anyStillWaiting = false;
                    for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                        WaitableEvent*  we2Test =   *i;
                        RequireNotNull (we2Test);
                        if (not we2Test->fWE_.fTriggered) {
                            anyStillWaiting = true;
                        }
                    }
                    if (anyStillWaiting) {
                        continue;
                    }
                    else {
                        for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                            WaitableEvent*  we2Test =   *i;
                            RequireNotNull (we2Test);
                            if (we2Test->fWE_.fResetType == eAutoReset) {
                                Assert (we2Test->fWE_.fTriggered);  // we probably need some mechanism to assure this is true but we currently have no enforcement of this!
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


#if     qExecution_WaitableEvent_SupportWaitForMultipleObjects
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Execution::Private_::WaitableEvent_ModuleInit_>   _Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif

#endif  /*_Stroika_Foundation_Execution_WaitableEvent_inl_*/
