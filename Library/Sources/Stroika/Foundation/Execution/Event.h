/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Event_h_
#define _Stroika_Foundation_Execution_Event_h_  1

#include    "../StroikaPreComp.h"

#if     qUseThreads_StdCPlusPlus
#include    <mutex>
#include    <condition_variable>
#elif   qUseThreads_WindowsNative
#include    <windows.h>
#endif

#include    "../Configuration/Common.h"

#include    "CriticalSection.h"
#include    "Exceptions.h"



// Workaround bug that the SIGNAL KILL doesn't appear to interupt the GCC implemntation of condition_variable::wait_for ()
// NOTE - the workaround is QUITE INCOMPLETE - and just affects SOME of threadpool code
// -- LGP 2012-05-27
#ifndef qEVENT_GCCTHREADS_LINUX_WAITBUG
#define qEVENT_GCCTHREADS_LINUX_WAITBUG     qUseThreads_StdCPlusPlus
#endif

//PLANA is to workaround in ThreadPool. This is probaly the worst appraoch but probably best for debugging, since
// runnign regression tests and seeing this break is a good way to localize the problem.
//
// Set _PLANA to 0 to use PLANB
#define qEVENT_GCCTHREADS_LINUX_WAITBUG_PLANA       0
#define qEVENT_GCCTHREADS_LINUX_WAITBUG_PLANA       qEVENT_GCCTHREADS_LINUX_WAITBUG
#define qEVENT_GCCTHREADS_LINUX_WAITBUG_PLANB       (qEVENT_GCCTHREADS_LINUX_WAITBUG && !qEVENT_GCCTHREADS_LINUX_WAITBUG_PLANA)


/*
 * TODO:
 *      o   Consider if there is a need for timed mutexes.
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

#if         qPlatform_Windows
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
