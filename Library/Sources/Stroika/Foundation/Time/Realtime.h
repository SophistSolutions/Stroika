/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Realtime_h_
#define _Stroika_Foundation_Time_Realtime_h_ 1

#include "../StroikaPreComp.h"

#include <chrono>
#include <limits>

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *      @todo   Consider making DurationSecondsType = long double. Tried and worked, but didnt have time
 *              to test impact/performance, and possibly pointless without changing internal rep of Duration at the same time.
 */

namespace Stroika {
    namespace Foundation {
        namespace Time {

            using std::chrono::time_point;

            /**
             *  Use double instead of long double because we dont have time to test performance impact, and only some (gcc/unix)
             *  systems make a difference anyhow (not on ppc).
             *
             *  <<<OBSOLETE COMMENT>>>>Use long double because sometimes the basis of tickcount can get large (if we run for a year or so).
             */
            using DurationSecondsType = double;

            /**
             *  Computes the offset of the time_point from the beginning time for the given clock.
             *
             *  \note Starts At Zero
             *      To avoid issues with roundoff, we start this at zero when the software first boots.
             *
             *  \todo - never implement assure done asap or at startup time - when not = chrono::steady_clock. Basline is from first call. Should add static init call as well.
             */
            template <class Clock = chrono::steady_clock, class Duration = typename Clock::duration>
            DurationSecondsType time_point2DurationSeconds (const time_point<Clock, Duration>& tp);

            /**
             */
            template <class Clock = chrono::steady_clock, class Duration = typename Clock::duration>
            time_point<Clock, Duration> DurationSeconds2time_point (DurationSecondsType t);

            /**
             *  Get the number of seconds since time_point2DurationSeconds, etc APIs first called (typically since this process started).
             *
             *  This uses chrono::steady_clock.
             *
             *  \note Starts At Zero
             *      To avoid issues with roundoff, we start this at zero when the software first boots.
             *
             *  \note - this value is convertible to time_point<chrono::steady_clock> via DurationSeconds2time_point, and so can be used with wait_until etc APIs
             */
            DurationSecondsType GetTickCount () noexcept;

            /**
             */
            constexpr DurationSecondsType kInfinite = std::numeric_limits<DurationSecondsType>::max ();
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Realtime.inl"

#endif /*_Stroika_Foundation_Time_Realtime_h_*/
