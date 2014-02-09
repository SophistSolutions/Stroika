/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitableEvent_h_
#define _Stroika_Foundation_Execution_WaitableEvent_h_  1

#include    "../StroikaPreComp.h"

#include    <condition_variable>
#include    <forward_list>
#include    <mutex>

#include    "../Configuration/Common.h"

#include    "Exceptions.h"



/**
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Maybe change WaitForAny/WaitForAnyUntil to return a SET! That addresses one of the main issues reported in
 *              the below referenced compaints about starvation.
 *
 *              The challenge is that I dont want to use STL set, but also dont want to create interdependncy with Containers
 *              and something this low level.
 *
 *              Maybe can use SET_TYPE template arg, and have it default to set<>?
 *
 *      @todo   Consider if there is a need for timed mutexes.
 *
 *      @todo   Add docs on why no WaitForMultipleObjects, and instead use
 *              http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/BlockingQueue.html (which is already in README file for
 *              Foundation::Execution::ReadMe.md
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             *  UNSURE IF/HOW LONG WE WANT TO SUPPORT THIS API. EXPERIMENTAL!
             */
#ifndef qExecution_WaitableEvent_SupportWaitForMultipleObjects
#define qExecution_WaitableEvent_SupportWaitForMultipleObjects      1
#endif


            /**
             *  AutoReset Waitable Event (like Windows' CreateEvent (resetType==eManualReset, false)).
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *  \note   \em Design Note     Considered making this copyable, or at least movable, but mutex and
             *              other similar classes are not.
             *              and you can easily use shared_ptr<> on an WaitableEvent to make it copyable.
             *
             *  \note   \em Design Note     WaitForAny/WaitForAnyUntil and WaitForMultipleEvents
             *
             *      See also qExecution_WaitableEvent_SupportWaitForMultipleObjects
             *
             *      This appears to be an issue with strong arguments on both sides. I'm very uncertain.
             *
             *      I've used the Windows WaitForMutlipleObjects API for years, and feel pretty comfortable with it.
             *      I find it handy.
             *
             *      However, there are compelling arguments (for example):
             *          https://groups.google.com/forum/#!msg/comp.unix.programmer/q2x0yQR5txk/34v1qQZN_u0J
             *          https://groups.google.com/forum/#!msg/comp.unix.programmer/WsgZZmu4ESA/Rv1MCun1CmUJ
             *
             *      which argue that its a bad idea, and that it leads to bad programming (bugs).
             *
             *      References:
             *          o   Notes on implementing Windows WaitForMultipleEvents API using POSIX (which are similar to stdc++) APIs:
             *              https://www.hackerzvoice.net/madchat/windoz/coding/winapi/waitfor_api.pdf
             *
             *          o   Interesting notes on how to implement WaitForMultipleEvents
             *              http://lists.boost.org/Archives/boost/2004/12/77175.php
             *
             */
            class   WaitableEvent {
            public:
                /**
                 *  eAutoReset is ALMOST worth being the default, and is very frequently the simplest thing to do.
                 *  eManualReset is the simplest to understand. Wait only returns when the event is set, and the only thing that can set it
                 *  is a call to Set().
                 *
                 *  The only difference between eManualReset and eAutoReset is that when a Wait() succeeds, as the very last step in returning
                 *  a successful wait, the event is automatically 'Reset'.
                 */
                enum    ResetType {
                    eAutoReset,
                    eManualreset,
                };

            public:
                /**
                 *
                 */
                WaitableEvent (ResetType resetType);
                WaitableEvent (const WaitableEvent&) = delete;

            public:
                nonvirtual  const WaitableEvent& operator= (const WaitableEvent&) = delete;

            public:
                /**
                 *  Set the event to the non-signaled state
                 */
                nonvirtual  void    Reset ();

            public:
                /**
                 *  Set the event to the signaled state.
                 *
                 *  \note   This COULD have been called 'Signal', or 'SetSignaled'.
                 */
                nonvirtual  void    Set ();

            public:
                /**
                 *  Simple wait. Can use operator HANDLE() to do fancier waits.
                 *  timeout can be negative (which triggers an immediate exception).
                 *
                 *  WaitTimedOutException throws if the timeout is exceeeded.
                 */
                nonvirtual  void    Wait (Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 *  WaitTimedOutException throws if the event is not signaled before timeoutAt is exceeeded.
                 */
                nonvirtual  void    WaitUntil (Time::DurationSecondsType timeoutAt);

#if     qExecution_WaitableEvent_SupportWaitForMultipleObjects
            public:
                /**
                 *  Note - CONTAINER_OF_WAITABLE_EVENTS - must iterate over WaitableEvent*!
                 *
                 *  \note   WaitForAny IS EXPERIMENTAL
                 */
                template    <typename CONTAINER_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT = set<WaitableEvent*>>
                static  SET_OF_WAITABLE_EVENTS_RESULT  WaitForAny (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeout = Time::kInfinite);
                template    <typename ITERATOR_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT = set<WaitableEvent*>>
                static  SET_OF_WAITABLE_EVENTS_RESULT  WaitForAny (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 *  Note - CONTAINER_OF_WAITABLE_EVENTS - must iterate over WaitableEvent*!
                 *
                 *  \note   WaitForAny IS EXPERIMENTAL
                 */
                template    <typename CONTAINER_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT = set<WaitableEvent*>>
                static  SET_OF_WAITABLE_EVENTS_RESULT  WaitForAnyUntil (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeoutAt);
                template    <typename ITERATOR_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT = set<WaitableEvent*>>
                static  SET_OF_WAITABLE_EVENTS_RESULT  WaitForAnyUntil (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeoutAt);

            public:
                /**
                 *  Note - CONTAINER_OF_WAITABLE_EVENTS - must iterate over WaitableEvent*!
                 *
                 *  \note   WaitForAll IS EXPERIMENTAL
                 */
                template    <typename CONTAINER_OF_WAITABLE_EVENTS>
                static  void  WaitForAll (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeout = Time::kInfinite);
                template    <typename ITERATOR_OF_WAITABLE_EVENTS>
                static  void  WaitForAll (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 *  Note - CONTAINER_OF_WAITABLE_EVENTS - must iterate over WaitableEvent*!
                 *
                 *  \note   WaitForAll IS EXPERIMENTAL
                 */
                template    <typename CONTAINER_OF_WAITABLE_EVENTS>
                static  void  WaitForAllUntil (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeoutAt);
                template    <typename ITERATOR_OF_WAITABLE_EVENTS>
                static  void  WaitForAllUntil (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeoutAt);
#endif

            public:
                /**
                 *  This API shouldnt be needed - if we had a better underlying implementation, and beware, the API could go away
                 *  if we find a better way. But callers may find it advisible to control this timeout to tune performance.
                 *
                 *  The WaitableEvent class internally uses condition_variable::wait_for () - and this doesnt advertise support for
                 *  EINTR or using Windows SDK 'alertable states' - so its not clear how often it returns to allow checking
                 *  for aborts. This 'feature' allows us to periodically check. You dont want to check too often, or you
                 *  effecitvely busy wait, and this checking is ONLY needed for the specail, rare case of thread abort.
                 */
                nonvirtual  void    SetThreadAbortCheckFrequency (Time::DurationSecondsType frequency);

            private:
                struct WE_ {
                    ResetType                   fResetType;
                    mutex                       fMutex;
                    condition_variable          fConditionVariable;
                    bool                        fTriggered = false;
                    Time::DurationSecondsType   fThreadAbortCheckFrequency = 0.5;

                    WE_ (ResetType resetType);
                    nonvirtual  void    Reset ();
                    nonvirtual  void    Set ();
                    nonvirtual  void    WaitUntil (Time::DurationSecondsType timeoutAt);
                };
                WE_                             fWE_;
#if     qExecution_WaitableEvent_SupportWaitForMultipleObjects
                static  mutex                   sExtraWaitableEventsMutex_;
                forward_list<shared_ptr<WE_>>   fExtraWaitableEvents_;
#endif
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "WaitableEvent.inl"

#endif  /*_Stroika_Foundation_Execution_WaitableEvent_h_*/
