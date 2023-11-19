/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Realtime_inl_
#define _Stroika_Foundation_Time_Realtime_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/Assertions.h"
#include "../Math/Common.h"

namespace Stroika::Foundation::Time {

    namespace Private_ {
        inline TimePointSeconds::duration GetAppStartOffset_ ()
        {
            // @todo not sure this is right --LGP 2023-11-16 - at least if it is needs better comment/explanation...
            static const TimePointSeconds kTimeAppStarted_ = chrono::time_point_cast<TimePointSeconds::duration> (chrono::steady_clock::now ());
            return kTimeAppStarted_.time_since_epoch ();
        }
    }

    inline TimePointSeconds FromAppStartRelative (const TimePointSeconds& tp)
    {
        return tp + Private_::GetAppStartOffset_ ();
    }

    inline TimePointSeconds ToAppStartRelative (const TimePointSeconds& tp)
    {
        return tp - Private_::GetAppStartOffset_ ();
    }

    /*
     ********************************************************************************
     ******************************* Time::GetTickCount *****************************
     ********************************************************************************
     */
    inline TimePointSeconds GetTickCount () noexcept
    {
        return chrono::steady_clock::now ();
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

#endif /*_Stroika_Foundation_Time_Realtime_inl_*/
