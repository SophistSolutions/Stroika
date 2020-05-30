/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Sleep_inl_
#define _Stroika_Foundation_Execution_Sleep_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#if qPlatform_Windows
#include <windows.h>
#elif qPlatform_POSIX
#include <time.h>
#include <unistd.h>
#endif

#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Execution {

    //redeclare to avoid having to include Thread code
    void CheckForThreadInterruption ();

    /*
     ********************************************************************************
     ******************************** Execution::Sleep ******************************
     ********************************************************************************
     */
    inline void Sleep (Time::DurationSecondsType seconds2Wait, Time::DurationSecondsType* remainingInSleep)
    {
        Require (seconds2Wait >= 0.0);
        RequireNotNull (remainingInSleep); // else call the over overload
        CheckForThreadInterruption ();
        // @todo lose if the #if stuff and use just if constexpr (but not working on msvc)
#if qPlatform_POSIX
        if constexpr (qPlatform_POSIX) {
            const long kNanoSecondsPerSecond = 1000L * 1000L * 1000L;
            timespec   ts;
            ts.tv_sec  = static_cast<time_t> (seconds2Wait);
            ts.tv_nsec = static_cast<long> (kNanoSecondsPerSecond * (seconds2Wait - ts.tv_sec));
            Assert (0 <= ts.tv_sec);
            Assert (0 <= ts.tv_nsec and ts.tv_nsec < kNanoSecondsPerSecond);
            timespec nextTS;
            if (::nanosleep (&ts, &nextTS) == 0) {
                *remainingInSleep = 0;
            }
            else {
                // https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/time.h.html doesn't clearly document allowed range for timespec
                // https://pubs.opengroup.org/onlinepubs/9699919799/functions/nanosleep.html doesn't clearly document allowed range for output timespec
                // value (can it go negative)
                // On WSL 1 (WinVer 1909) nextTS sometimes goes negative (or blows up to be very large?), so check)
                WeakAssert (0 <= ts.tv_nsec and ts.tv_nsec < kNanoSecondsPerSecond); // not sure this should happen? - log for now... -- LGP 2020-05-29
                WeakAssert (nextTS.tv_sec >= 0);                                     // ""
                if ((nextTS.tv_sec < 0) or not(0 <= ts.tv_nsec and ts.tv_nsec < kNanoSecondsPerSecond)) {
                    *remainingInSleep = 0;
                }
                else {
                    *remainingInSleep = nextTS.tv_sec + static_cast<Time::DurationSecondsType> (ts.tv_nsec) / kNanoSecondsPerSecond;
                }
            }
        }
#elif qPlatform_Windows
        if constexpr (qPlatform_Windows) {
            Time::DurationSecondsType tc = Time::GetTickCount ();
            if (::SleepEx (static_cast<int> (seconds2Wait * 1000), true) == 0) {
                *remainingInSleep = 0;
            }
            else {
                Time::DurationSecondsType remaining = (tc + seconds2Wait) - Time::GetTickCount ();
                if (remaining < 0) {
                    remaining = 0;
                }
                *remainingInSleep = remaining;
            }
        }
#else
        AssertNotImplemented ();
#endif
        Ensure (*remainingInSleep <= seconds2Wait);
        Ensure (*remainingInSleep >= 0);
        CheckForThreadInterruption ();
    }

    /*
     ********************************************************************************
     *************************** Execution::SleepUntil ******************************
     ********************************************************************************
     */
    inline void SleepUntil (Time::DurationSecondsType untilTickCount)
    {
        Time::DurationSecondsType waitMoreSeconds = untilTickCount - Time::GetTickCount ();
        if (waitMoreSeconds <= 0) {
            CheckForThreadInterruption ();
        }
        else {
            Sleep (waitMoreSeconds);
        }
    }

}

#endif /*_Stroika_Foundation_Execution_Sleep_inl_*/
