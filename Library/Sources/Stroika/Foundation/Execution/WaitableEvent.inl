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
                std::lock_guard<mutex> lockGaurd (fMutex);
                fTriggered = false;
            }
            inline  void    WaitableEvent::WE_::Set ()
            {
                std::lock_guard<mutex> lockGaurd (fMutex);
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
            template    <typename CONTAINER_OF_WAITABLE_EVENTS>
            inline  unsigned int    WaitableEvent::WaitForAny (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeout)
            {
                return WaitForAnyUntil (waitableEvents, timeout + Time::GetTickCount ());
            }
            template    <typename ITERATOR_OF_WAITABLE_EVENTS>
            inline  unsigned int    WaitableEvent::WaitForAny (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeout)
            {
                return WaitForAnyUntil (waitableEventsStart, waitableEventsEnd, timeout + Time::GetTickCount ());
            }
            template    <typename CONTAINER_OF_WAITABLE_EVENTS>
            inline  unsigned int    WaitableEvent::WaitForAnyUntil (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeoutAt)
            {
                return WaitForAnyUntil (begin (waitableEvents), end (waitableEvents), timeoutAt);
            }
            template    <typename ITERATOR_OF_WAITABLE_EVENTS>
            unsigned int    WaitableEvent::WaitForAnyUntil (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeoutAt)
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
                    lock_guard<mutex> critSec (sExtraWaitableEventsMutex_);
                    for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                        (*i)->fExtraWaitableEvents_.remove (we);
                    }
                });
                {
                    lock_guard<mutex> critSec (sExtraWaitableEventsMutex_);
                    for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                        (*i)->fExtraWaitableEvents_.push_front (we);
                    }
                }
                while (true) {
                    we->WaitUntil (timeoutAt);
                    unsigned int cnt = 0;
                    for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                        if ((*i)->fWE_.fTriggered) {
                            if ((*i)->fWE_.fResetType == eAutoReset) {
                                (*i)->fWE_->Reset ();
                            }
                            return cnt;
                        }
                        ++cnt;
                    }
                }
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_WaitableEvent_inl_*/
