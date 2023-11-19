/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Realtime_h_
#define _Stroika_Foundation_Time_Realtime_h_ 1

#include "../StroikaPreComp.h"

#include <chrono>

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */
namespace Stroika::Foundation {
    using namespace std;
};

namespace Stroika::Foundation::Time {

    using chrono::time_point;

    /**
     *  \brief chrono::duration<double> - a time span (length of time) measured in seconds.
     *
     *  DurationSeconds is just a choice of what chrono::duration template parameters to use to make use much simpler.
     *  Converting to a common sensible base format greatly simplifies a number of Stroika APIs, so rather than having to
     *  template all your 'duration' arguments, just use this DurationSeconds for simplicity, clarity, and at only
     * a small cost.
     * 
     *  \note this is one of two types which replaces the Stroika v2.1 DurationSecondsType (DurationSeconds and TimePointSeconds)
     * 
     *  \note Use double instead of long double (as the rep) because we don't have time to test performance impact, and only some (gcc/unix)
     *  systems make a difference anyhow (not on ppc). Everything else in Stroika should key off this choice, so this is the place to change
     *  the basic rep used throughout Stroika if I need to experiment (float/long double).
     */
    using DurationSeconds = chrono::duration<double>;
    static_assert (sizeof (DurationSeconds::rep) == sizeof (DurationSeconds));

    /**
     *  \brief TimePointSeconds is a simpler approach to chrono::time_point, which doesn't require using templates everywhere.
     * 
     *  But - TimePointSeconds - since it uses chrono::time_point - is fully interoperable with the other time_point etc objects.
     * 
     *  The clock it uses IS guaraneed to be a 'steady' clock, though not necessarily THE 'steady_clock' class.
     */
    using TimePointSeconds = time_point<chrono::steady_clock, DurationSeconds>;
    static_assert (sizeof (DurationSeconds::rep) == sizeof (TimePointSeconds));
    static_assert (TimePointSeconds::clock::is_steady);

    /**
     *  \note no longer true, but in Stroika v2.1:
     *      this always started at offset zero for start of app (instead see FromAppStartRelative).
     *      this always used steady_clock (now see TimePointSeconds).
     *      it used to return a 'float' type and now returns a chrono::time_point<> type (for better type safety).
     */
    TimePointSeconds GetTickCount () noexcept;

    /**
     *  @See https://stroika.atlassian.net/browse/STK-619    CONSIDER LOSING THIS - AND USE special TYPE and overloading, and handle kInfinity differently - no arithmatic, just no timeout
     */
    constexpr DurationSeconds kInfinity = DurationSeconds{numeric_limits<DurationSeconds::rep>::infinity ()};

    /**
     */
    TimePointSeconds FromAppStartRelative (const TimePointSeconds& tp);

    /**
     */
    TimePointSeconds ToAppStartRelative (const TimePointSeconds& tp);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Realtime.inl"

#endif /*_Stroika_Foundation_Time_Realtime_h_*/
