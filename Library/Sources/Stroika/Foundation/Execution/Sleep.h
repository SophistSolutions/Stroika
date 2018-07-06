/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

namespace Stroika::Foundation::Time {
    class Duration;
}

namespace Stroika::Foundation {
    namespace Execution {

        /**
         * The portable Sleep() function - will wait the given amount of time - blocking the running thread.
         * It CAN be interrupted (not talking about thread interrupt). If interrupted, one overload will return the amount of time remaining, allowing
         * easy re-sleeping. The other overload (/1) - will check for aborting, but otherwise keep sleeping
         * through interrupts until the time has elapsed.
         *
         *  @see SleepUntil
         *
         *  Sleep\1:    will restart sleeping to use up the full given sleep time, if interrupted (unless thread interruption causes throw)
         *  Sleep\2:    may not use up the entire time given as argument, and if not, will place in remainingInSleep the time remaining.
         *
         *  \note   Sleep (0) will still yield the processor (so like std::thread::yield ())
         *
         *  \req    seconds2Wait >= 0
         *
         *  \note   \em Thread-Safety   <a href="thread_safety.html#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
         *
         *  \note   ***Cancelation Point***
         *
         *  \note   Very similar to std::this_thread::sleep_for () - except for the overload returning remaining amount, and cancelation support
         */
        void Sleep (Time::DurationSecondsType seconds2Wait);
        void Sleep (Time::DurationSecondsType seconds2Wait, Time::DurationSecondsType* remainingInSleep);
        void Sleep (const Time::Duration& wait);
        void Sleep (const Time::Duration& wait, Time::Duration* remainingInSleep);

        /**
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
         *  \note   ***Cancelation Point***
         *
         *  \note   Unlike Sleep () - this may or may not yield.
         *
         *  \note   SleepUntil restarts if interruped, so if it returns, it will return after untilTickCount
         *
         *  \note   This may or may not end up calling Sleep(). It is not an error to call with a tickCount which has already passed: it just returns quickly, and may not yield.
         *
         *  \note   Very similar to std::this_thread::sleep_until () - except for the cancelation support
         */
        void SleepUntil (Time::DurationSecondsType untilTickCount);
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Sleep.inl"

#endif /*_Stroika_Foundation_Execution_Sleep_h_*/
