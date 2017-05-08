/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Sleep_h_
#define _Stroika_Foundation_Execution_Sleep_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "../Time/Realtime.h"

/**
 * TODO
 *
 *  \file
 *
 *
 *      @todo   Should probably AMMEND (though dangerous at this stage) behavior of Sleep and esp SleepUntil - so if interutped (not throwing)
 *              with no 'sleep more seconds' arg- we re-sleep the balance. So interupots dont mess up how long we sleep.
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Time {
            class Duration;
        }
    }
}

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /**
             * The portable Sleep() function - will wait the given amount of time - blocking the running thread.
             * It CAN be interrupted. If interrupted, one overload will return the amount of time remaining, allowing
             * easy re-sleeping. The other overload (/1) - will check for aborting, but otherwise keep sleeping
             * through interrupts until the time has elapsed.
             *
             * Causes for interruption are platform specific. Some platform specific notes:
             *
             *  qPlatform_Windows:
             *      MAIN reason to use this - is it sets the 'alertable' flag on the sleep, so the
             *      QueueUserAPC () stuff works!  which allows Thread::Abort () to work properly...
             *
             *  qPlatform_POSIX:
             *      Sleep can be interrupted by receipt of a signal (see POSIX nanosleep()). But the second form can
             *      be used to easily continue the sleep.
             *
             *  @see SleepUntil
             *
             *  \req    seconds2Wait >= 0
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
             *
             *  \note   This calls CheckForThreadInterruption ();
             *
             */
            void Sleep (Time::DurationSecondsType seconds2Wait);
            void Sleep (Time::DurationSecondsType seconds2Wait, Time::DurationSecondsType* remainingInSleep);
            void Sleep (const Time::Duration& wait);
            void Sleep (const Time::Duration& wait, Time::Duration* remainingInSleep);

            /*
             *  Wait until the tickCount is >= the given value.
             *
             *  \par Example Usage
             *      \code
             *      Time::DurationSeconds startedAt =   Time::GetTickCount ();
             *      do_someting_dont_know_how_long_it_will_take();
             *      Execution::SleepUntil (1.0 + startedAt);        // make sure do_someting_dont_know_how_long_it_will_take () took at least one second
             *      \endcode
             *
             *  @see Sleep ();
             *
             *  \note   This always calls CheckForThreadInterruption () at least once, but may not call Sleep() if not needed.
             *  \note   This may or may not end up calling Sleep(). It is not an error to call with a tickCount which has already passed
             */
            void SleepUntil (Time::DurationSecondsType untilTickCount);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Sleep.inl"

#endif /*_Stroika_Foundation_Execution_Sleep_h_*/
