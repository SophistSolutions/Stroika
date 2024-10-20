/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#if qPlatform_Windows
#include <windows.h>
#elif qPlatform_POSIX
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
        Require (seconds2Wait >= 0.0s);
        RequireNotNull (remainingInSleep); // else call the one-argument overload
        Thread::CheckForInterruption ();
        // @todo lose if the #if stuff and use just if constexpr (but not working on msvc - complains about nanosleep undefined)
#if qPlatform_POSIX
        if constexpr (qPlatform_POSIX) {
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
#elif qPlatform_Windows
        if constexpr (qPlatform_Windows) {
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
        Thread::CheckForInterruption ();
    }

    /*
     ********************************************************************************
     *************************** Execution::SleepUntil ******************************
     ********************************************************************************
     */
    inline void SleepUntil (Time::TimePointSeconds untilTickCount)
    {
        Time::DurationSeconds waitMoreSeconds = untilTickCount - Time::GetTickCount ();
        if (waitMoreSeconds <= 0s) {
            Thread::CheckForInterruption ();
        }
        else {
            Sleep (waitMoreSeconds);
        }
    }

}
