/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#if qStroika_Foundation_Common_Platform_Windows
#include <windows.h>
#elif qStroika_Foundation_Common_Platform_POSIX
#include <time.h>
#include <unistd.h>
#endif
#include <cerrno>

#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Execution {

    //redeclare to avoid having to include Thread code
    namespace Thread {
        void CheckForInterruption ();
    }

    /*
     ********************************************************************************
     ******************************** Execution::Sleep ******************************
     ********************************************************************************
     */
    inline void Sleep (Time::Duration seconds2Wait, Time::DurationSeconds* remainingInSleep)
    {
        // NB: even though this is complicated, its inlined because most of it is if constexpr, and eliminated and inlining
        // makes it easier for optimizer to eliminate unused portions
        Require (seconds2Wait >= 0.0s);
        RequireNotNull (remainingInSleep); // else call the one-argument overload
        Thread::CheckForInterruption ();
        // @todo lose if the #if stuff and use just if constexpr (but not working on msvc - complains about nanosleep undefined)
#if qStroika_Foundation_Common_Platform_POSIX
        if constexpr (qStroika_Foundation_Common_Platform_POSIX) {
            constexpr long kNanoSecondsPerSecond = 1000L * 1000L * 1000L;
            timespec       ts;
            ts.tv_sec  = seconds2Wait.As<time_t> ();
            ts.tv_nsec = static_cast<long> (kNanoSecondsPerSecond * (seconds2Wait.As<double> () - ts.tv_sec));
            Assert (0 <= ts.tv_sec);
            Assert (0 <= ts.tv_nsec and ts.tv_nsec < kNanoSecondsPerSecond);
            timespec nextTS;
            int      nanoSleepResult = ::nanosleep (&ts, &nextTS);

            // See https://github.com/microsoft/WSL/issues/4898 - workaround nanosleep EINVAL on Windows/WSL 1 with newer libc (like with ubuntu 20.04)
#if _POSIX_C_SOURCE >= 200809L
            if (nanoSleepResult < 0 and errno == EINVAL) {
                if ((errno = ::clock_nanosleep (CLOCK_MONOTONIC, 0, &ts, &nextTS)) == 0) {
                    nanoSleepResult = 0;
                }
            }
#endif
            if (nanoSleepResult == 0) {
                *remainingInSleep = 0s;
            }
            else {
                Assert (errno == EINTR); // only in this case do they guarantee nextTS set properly
                // https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/time.h.html doesn't clearly document allowed range for timespec
                // https://pubs.opengroup.org/onlinepubs/9699919799/functions/nanosleep.html doesn't clearly document allowed range for output timespec (can results go negative)
                WeakAssert (0 <= nextTS.tv_nsec and nextTS.tv_nsec < kNanoSecondsPerSecond); // docs not clear but I think this should always be true (on EINTR)... -- LGP 2020-05-29
                WeakAssert (nextTS.tv_sec >= 0);                                             // ""
                *remainingInSleep =
                    Time::DurationSeconds{nextTS.tv_sec + static_cast<Time::DurationSeconds::rep> (nextTS.tv_nsec) / kNanoSecondsPerSecond};
            }
        }
#elif qStroika_Foundation_Common_Platform_Windows
        if constexpr (qStroika_Foundation_Common_Platform_Windows) {
            Time::TimePointSeconds tc = Time::GetTickCount ();
            if (::SleepEx (static_cast<int> (seconds2Wait.count () * 1000), true) == 0) {
                *remainingInSleep = 0s;
            }
            else {
                Time::DurationSeconds remaining = (tc + seconds2Wait) - Time::GetTickCount ();
                if (remaining < 0s) {
                    remaining = 0s;
                }
                *remainingInSleep = remaining;
            }
        }
#else
        AssertNotImplemented ();
#endif
        Ensure (*remainingInSleep <= seconds2Wait);
        Ensure (*remainingInSleep >= 0s);
        // Consider if THIS is truly needed - doing BOTH at start and end appears excessive!
        // But we don't want to wait at all if interrupted. And if we've waited a while and had low level sleep return because
        // of a thread interruption, we want to translate that to an exception.
        // So - maybe both really needed.
        Thread::CheckForInterruption ();
    }

    /*
     ********************************************************************************
     ***************************** Execution::Sleep *********************************
     ********************************************************************************
     */
    inline void Sleep (Time::Duration seconds2Wait)
    {
        // to avoid accumulating error on total time waited, compute the UNTIL value and keep waiting UNTIL that point
        Require (seconds2Wait >= 0s);
        SleepUntil (Time::GetTickCount () + seconds2Wait);
    }

}
