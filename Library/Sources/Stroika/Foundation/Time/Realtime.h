/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Realtime_h_
#define _Stroika_Foundation_Time_Realtime_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <chrono>

#include "Stroika/Foundation/Time/Clock.h"
#include "Stroika/Foundation/Traversal/Common.h"

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
     * 
     *  \note DurationSeconds is a 'floating point version of chrono::seconds'
     * 
     *  \note - WHY is it so important Stroika uses a 'floating point' version of duration.
     *      Consider this code:
     *          TimeOutAt t = now + REALLY_BIG_TIMOUT;
     *      say we define REALLY_BIG_TIMEOUT = DURUATION::max();
     *          if we used fixed point numbers, REALLY_BIG_TIMOUT + tiny number wraps - basically back to zero.
     *          with floating point numbers, max + small number remains max.
     *      Thats a HUGE, and USEFUL simplification of wildly common code.
     */
    using DurationSeconds = chrono::duration<double>;
    static_assert (sizeof (DurationSeconds::rep) == sizeof (DurationSeconds));
    static_assert (floating_point<DurationSeconds::rep>); // perhaps allow #define control over DurationSeconds, but always promise its a floating point type

    /**
     *  \brief this is an alias for steady_clock; this is the clock used for GetTickCount () results.
     *  
     *  The clock it uses IS guaraneed to be a 'steady' clock, though not necessarily THE 'steady_clock' class.
     * 
     *  \note - could use AppStartZeroedClock to get zero-based results, or clock_cast to map from regular tick-counts to zero based.
     * 
     *  \todo consider a configuration define that could be used to switch RealtimeClock to chrono::high_resolution_clock
     */
    using RealtimeClock = chrono::steady_clock;
    static_assert (RealtimeClock::is_steady);

    /**
     *  \brief TimePointSeconds is a simpler approach to chrono::time_point, which doesn't require using templates everywhere.
     * 
     *  But - TimePointSeconds - since it uses chrono::time_point - is mostly (see Pin2SafeSeconds()) interoperable with the other time_point etc objects.
     * 
     *  \note - CARE is required passing this value to STD C++ APIs!; see and consider using Pin2SafeSeconds() in calling those APIs.
     * 
     *  \see RealtimeClock for details of how time is measured.
     */
    using TimePointSeconds = time_point<RealtimeClock, DurationSeconds>;
    static_assert (sizeof (DurationSeconds::rep) == sizeof (TimePointSeconds));

    /**
     *  \brief get the current (monontonically increasing) time - from RealtimeClock
     * 
     *  \note no longer true, but in Stroika v2.1:
     *      this always started at offset zero for start of app.
     *      this always used steady_clock (now see TimePointSeconds).
     *      it used to return a 'float' type and now returns a chrono::time_point<> type (for better type safety).
     * 
     *      Since Stroika v3.0d5 - defined to be based on RealtimeClock (which is same - steady_clock), and uses double 
     *      internally (using DurationSeconds = chrono::duration<double>). And to get it to be zero based,
     *      \code
     *          Time::clock_cast<Time::AppStartZeroedClock<Time::RealtimeClock>> (Time::GetTickCount ());
     *      \endcode
     */
    TimePointSeconds GetTickCount () noexcept;

    /**
     *  @See https://stroika.atlassian.net/browse/STK-619    CONSIDER LOSING THIS - AND USE special TYPE and overloading, and handle kInfinity differently - no arithmatic, just no timeout
     */
    constexpr DurationSeconds kInfinity = DurationSeconds{numeric_limits<DurationSeconds::rep>::infinity ()};

    /**
     *  If you want to convert tickcount times to be zero based (often helpful for display purposes), you can use:
     *      \code
     *          clock_cast<DisplayedRealtimeClock> (GetTickCount ())
     *      \endcode
     */
    using DisplayedRealtimeClock = AppStartZeroedClock<RealtimeClock, DurationSeconds>;

}

namespace Stroika::Foundation::Traversal::RangeTraits {

    /*
     *  Cannot #include Traversal/Range (easily) due to mutual include nightmare. So just forwward declare template, and still
     *  define it anyhow (only need to #include enuf for Openness enum).
     * 
     *  Then we can easily construct Ranges of DurationSeconds, and TimePointSeconds (time ranges).
     */
    template <typename T>
    struct Default;

    template <>
    struct Default<Time::DurationSeconds> {
        using value_type             = Time::DurationSeconds;
        using SignedDifferenceType   = Time::DurationSeconds;
        using UnsignedDifferenceType = Time::DurationSeconds;

        static constexpr inline Openness kLowerBoundOpenness{Openness::eClosed};
        static constexpr inline Openness kUpperBoundOpenness{Openness::eClosed};

        static constexpr inline value_type kLowerBound{Time::DurationSeconds::min ()};
        static constexpr inline value_type kUpperBound{Time::DurationSeconds::max ()};

        static value_type GetNext (value_type i);
        static value_type GetPrevious (value_type i);
    };

    template <>
    struct Default<Time::TimePointSeconds> {
        using value_type             = Time::TimePointSeconds;
        using SignedDifferenceType   = Time::DurationSeconds;
        using UnsignedDifferenceType = Time::DurationSeconds;

        static constexpr inline Openness kLowerBoundOpenness{Openness::eClosed};
        static constexpr inline Openness kUpperBoundOpenness{Openness::eClosed};

        static constexpr inline value_type kLowerBound{value_type{Time::DurationSeconds::min ()}};
        static constexpr inline value_type kUpperBound{value_type{Time::DurationSeconds::max ()}};

        static value_type GetNext (value_type i);
        static value_type GetPrevious (value_type i);
    };

    template <>
    struct Default<chrono::time_point<Time::DisplayedRealtimeClock, Time::DurationSeconds>> {
        using value_type             = chrono::time_point<Time::DisplayedRealtimeClock, Time::DurationSeconds>;
        using SignedDifferenceType   = Time::DurationSeconds;
        using UnsignedDifferenceType = Time::DurationSeconds;

        static constexpr inline Openness kLowerBoundOpenness{Openness::eClosed};
        static constexpr inline Openness kUpperBoundOpenness{Openness::eClosed};

        static constexpr inline value_type kLowerBound{value_type{Time::DurationSeconds::min ()}};
        static constexpr inline value_type kUpperBound{value_type{Time::DurationSeconds::max ()}};

        static value_type GetNext (value_type i);
        static value_type GetPrevious (value_type i);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Realtime.inl"

#endif /*_Stroika_Foundation_Time_Realtime_h_*/
