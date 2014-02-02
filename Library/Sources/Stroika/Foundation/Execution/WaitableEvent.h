/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitableEvent_h_
#define _Stroika_Foundation_Execution_WaitableEvent_h_  1

#include    "../StroikaPreComp.h"

#include    <condition_variable>
#include    <mutex>

#include    "../Configuration/Common.h"

#include    "Exceptions.h"



/**
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Consider if there is a need for timed mutexes.
 *
 *      @todo   Add docs on why no WaitForMultipleObjects, and instead use
 *              http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/BlockingQueue.html (which is already in README file for
 *              Foundation::Execution::ReadMe.md
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  AutoReset Waitable Event (like Windows' CreateEvent (false, false)).
             *
             *  Easy to fix to NOT be auto-reset, but right now - I thinking this maybe a better paradigm,
             *  and simpler to assume always in unset state by default.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *  \note   \em Design Note     Considered making this copyable, or at least movable, but mutex and other similar classes are not.
                            and you can easily use shared_ptr<> on an WaitableEvent to make it copyable.
             */
            class   WaitableEvent {
            public:
                WaitableEvent ();
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
                 *  Set the event to the signaled state
                 */
                nonvirtual  void    Set ();

            public:
                /**
                 *  Simple wait. Can use operator HANDLE() to do fancier waits.
                 *  timeout can be negative (which triggers an immediate exception).
                 *
                 *  WaitTimedOutException throw when the timeout is exceeeded.
                 */
                nonvirtual  void    Wait (Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 *  WaitTimedOutException throw when the timeout is exceeeded.
                 */
                nonvirtual  void    WaitUntil (Time::DurationSecondsType timeoutAt);

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
                mutex                       fMutex_;
                condition_variable          fConditionVariable_;
                bool                        fTriggered_;
                Time::DurationSecondsType   fThreadAbortCheckFrequency_;
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
