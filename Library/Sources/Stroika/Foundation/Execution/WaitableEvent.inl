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
            inline  void    WaitableEvent::Reset ()
            {
                //Debug::TraceContextBumper ctx (SDKSTR ("WaitableEvent::Reset"));
                fWE_.Reset ();
            }
            inline  void    WaitableEvent::Set ()
            {
                //Debug::TraceContextBumper ctx (SDKSTR ("WaitableEvent::Set"));
                fWE_.Set ();
                lock_guard<mutex> critSec (sExtraWaitableEventsMutex_);
                for (auto i : fExtraWaitableEvents_) {
                    i->Set ();
                }
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
            inline  unsigned int    WaitableEvent::WaitForAnyUntil (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeoutAt)
            {
                return WaitForAnyUntil (begin (waitableEvents), end (waitableEvents), timeoutAt);
            }
            template    <typename ITERATOR_OF_WAITABLE_EVENTS>
            unsigned int    WaitableEvent::WaitForAnyUntil (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeoutAt)
            {
                /////VERY ROUGH DRAFT
                //AssertNotImplemented ();

                shared_ptr<WE_> we  =   shared_ptr<WE_> (new WE_ ());
                {
                    lock_guard<mutex> critSec (sExtraWaitableEventsMutex_);
                    for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                        i->fExtraWaitableEvents_.push_front (we);
                    }
                }
                try {
                    we->WaitUntil (timeoutAt);
                    unsigned int cnt = 0;
                    for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                        if (i->fWE_.fTriggered) {
                            return cnt;
                        }
                        ++cnt;
                    }

                }
                catch (...) {
                    /// NEED TO FIX AND REMOVE FROM LSIT AND RETHROW
                }

                {
                    lock_guard<mutex> critSec (sExtraWaitableEventsMutex_);
                    for (ITERATOR_OF_WAITABLE_EVENTS i = waitableEventsStart; i != waitableEventsEnd; ++i) {
                        i->fExtraWaitableEvents_->remove (we);
                    }
                }

                // create another WE as shared.
                // stick it onto the list ofr each waitablevent
                // wait on invated evnet (and if it successeds, then check orig events and return right now)
                // either way - undo additions
                return 0;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_WaitableEvent_inl_*/
