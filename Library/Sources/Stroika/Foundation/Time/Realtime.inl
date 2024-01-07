/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Realtime_inl_
#define _Stroika_Foundation_Time_Realtime_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <cmath>

#include "../Debug/Assertions.h"
#include "../Math/Common.h"

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     ******************************* Time::GetTickCount *****************************
     ********************************************************************************
     */
    inline TimePointSeconds GetTickCount () noexcept
    {
        return RealtimeClock::now ();
    }

    ////////////////// DEPRECATED STUFF BELOW

    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    [[deprecated ("Since Stroika v3.0d5 use kInfinity")]] constexpr DurationSeconds kInfinite =
        DurationSeconds{numeric_limits<DurationSeconds::rep>::infinity ()};
    using DurationSecondsType [[deprecated ("Since Stroika v3.0d5 - use DurationSeconds or TimePointSeconds")]] = double;

    template <typename Clock, typename Duration>
    [[deprecated ("Since Stroika v3.0d5 - use TimePointSeconds")]] inline DurationSecondsType
    time_point2DurationSeconds (const time_point<Clock, Duration>& tp)
    {
        return TimePointSeconds{tp}.time_since_epoch ().count ();
    }
    template <typename Clock, typename Duration>
    [[deprecated ("Since Stroika v3.0d5 - use TimePointSeconds")]] time_point<Clock, Duration> DurationSeconds2time_point (DurationSecondsType t)
    {
        Require (t >= 0);
        return TimePointSeconds{DurationSeconds{t}};
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

}

namespace Stroika::Foundation::Traversal::RangeTraits {

    /*
     ********************************************************************************
     *********** Traversal::RangeTraits::Default<Time::DurationSeconds> *************
     ********************************************************************************
     */
    inline auto Default<Time::DurationSeconds>::GetNext (value_type i) -> value_type
    {
        using namespace Time;
        return DurationSeconds{nextafter (i.count (), numeric_limits<DurationSeconds::rep>::max ())};
    }
    inline auto Default<Time::DurationSeconds>::GetPrevious (value_type i) -> value_type
    {
        using namespace Time;
        return DurationSeconds{nextafter (i.count (), numeric_limits<DurationSeconds::rep>::min ())};
    }

    /*
     ********************************************************************************
     *********** Traversal::RangeTraits::Default<Time::TimePointSeconds> ************
     ********************************************************************************
     */
    inline auto Default<Time::TimePointSeconds>::GetNext (value_type i) -> value_type
    {
        using namespace Time;
        return TimePointSeconds{
            TimePointSeconds::duration{nextafter (i.time_since_epoch ().count (), numeric_limits<TimePointSeconds::duration::rep>::max ())}};
    }
    inline auto Default<Time::TimePointSeconds>::GetPrevious (value_type i) -> value_type
    {
        using namespace Time;
        return TimePointSeconds{
            TimePointSeconds::duration{nextafter (i.time_since_epoch ().count (), numeric_limits<TimePointSeconds::duration::rep>::min ())}};
    }

    /*
     ********************************************************************************
     ** RangeTraits::Default<chrono::time_point<Time::DisplayedRealtimeClock, Time::DurationSeconds>> **
     ********************************************************************************
     */
    inline auto Default<chrono::time_point<Time::DisplayedRealtimeClock, Time::DurationSeconds>>::GetNext (value_type i) -> value_type
    {
        using namespace Time;
        return value_type{value_type::duration{nextafter (i.time_since_epoch ().count (), numeric_limits<value_type::duration::rep>::max ())}};
    }
    inline auto Default<chrono::time_point<Time::DisplayedRealtimeClock, Time::DurationSeconds>>::GetPrevious (value_type i) -> value_type
    {
        using namespace Time;
        return value_type{value_type::duration{nextafter (i.time_since_epoch ().count (), numeric_limits<value_type::duration::rep>::min ())}};
    }

}

#endif /*_Stroika_Foundation_Time_Realtime_inl_*/
