/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Event_h_
#define _Stroika_Foundation_Execution_Event_h_  1

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
 *      @todo   Perhaps rename to WaitableEvent
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
             *  AutoReset Event (like Windows' CreateEvent (false, false)).
             *
             *  Easy to fix to NOT be auto-reset, but right now - I thinking this maybe a better paradigm,
             *  and simpler to assume always in unset state by default.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *  \note   \em Design Note     Considered making this copyable, or at least movable, but mutex and other similar classes are not.
                            and you can easily use shared_ptr<> on an Event to make it copyable.
             */
            class   Event {
            public:
                Event ();
                Event (const Event&) = delete;

            public:
                nonvirtual  const Event& operator= (const Event&) = delete;

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

            private:
                mutex               fMutex_;
                condition_variable  fConditionVariable_;
                bool                fTriggered_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Event.inl"

#endif  /*_Stroika_Foundation_Execution_Event_h_*/
