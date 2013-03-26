/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Event_h_
#define _Stroika_Foundation_Execution_Event_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>

#if     qUseThreads_StdCPlusPlus
#include    <condition_variable>
#elif   qUseThreads_WindowsNative
#include    <windows.h>
#endif

#include    "../Configuration/Common.h"

#include    "Exceptions.h"


//
// Workaround bug that the SIGNAL KILL doesn't appear to interupt the GCC implemntation of condition_variable::wait_for ()
// -- LGP 2012-05-27
//
//      MAYBE use http://www.mkssoftware.com/docs/man3/pthread_cancel.3.asp
//
//
#ifndef qEVENT_GCCTHREADS_LINUX_WAITBUG
#define qEVENT_GCCTHREADS_LINUX_WAITBUG     qUseThreads_StdCPlusPlus
#endif






/*
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

#ifndef qTrack_ThreadUtils_HandleCounts
#define qTrack_ThreadUtils_HandleCounts     qDebug
#endif


            /*
             * AutoReset Event (like Windwow CreateEvent (false, false).
             * Easy to fix to NOT be auto-reset, but right now - I thinking this maybe a better paradigm, and simpler to assume always in unset
             * state by default.
             */
            class   Event {
#if     qTrack_ThreadUtils_HandleCounts
            public:
                static  uint32_t    sCurAllocatedHandleCount;
#endif
            public:
                NO_COPY_CONSTRUCTOR(Event);
                NO_ASSIGNMENT_OPERATOR(Event);

            public:
                Event ();
                ~Event ();

            public:
                // Set the event to the non-signaled state
                nonvirtual  void    Reset ();

                // Set the event to the signaled state
                nonvirtual  void    Set ();

                // Simple wait. Can use operator HANDLE() to do fancier waits
                nonvirtual  void    Wait (Time::DurationSecondsType timeout = Time::kInfinite);

#if         qUseThreads_WindowsNative
            public:
                operator HANDLE () const;
            private:
                HANDLE  fEventHandle;
#endif
#if     qUseThreads_StdCPlusPlus
                std::mutex              fMutex_;
                std::condition_variable fConditionVariable_;
                bool                    fTriggered_;
#endif
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Event_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Event.inl"
