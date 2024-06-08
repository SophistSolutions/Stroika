/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Clock_h_
#define _Stroika_Foundation_Time_Clock_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <chrono>

#include "Stroika/Foundation/Common/Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Time {

    /*
     *  \brief like std::chrono::clock_cast, but supports steady_clock, and others not explicitly supported by std::chrono::clock_cast (through experiment/approximation), and ranges of time_points...
     * 
     *  \see commentary in https://stackoverflow.com/questions/35282308/convert-between-c11-clocks
     * 
     *  \note - for some cases, the conversion is estimated, and may vary slightly from run run to run.
     *  \note - range overload is both HANDY for normal case, and CRITICAL for case where we approximate, so that valid ranges
     *          always map to valid ranges (one jitter, not two). Note also - use of template/template param for RANGE is to avoid mutual inclusion issues.
     */
    template <typename DESTINATION_CLOCK_T, typename SOURCE_CLOCK_T, typename DURATION_T>
    typename DESTINATION_CLOCK_T::time_point clock_cast (chrono::time_point<SOURCE_CLOCK_T, DURATION_T> tp);
    template <typename DESTINATION_CLOCK_T, template <typename> typename RANGE, typename SOURCE_CLOCK_T, typename DURATION_T>
    RANGE<typename DESTINATION_CLOCK_T::time_point> clock_cast (RANGE<chrono::time_point<SOURCE_CLOCK_T, DURATION_T>> tpRange);

    /**
     *  AppStartZeroedClock is just like BASE_CLOCK_T, except that its time values are magically adjusted so that
     *  zero corresponds to when the application (code starts) begins. This defaults to using the base clocks
     *  duration representation, but its often simpler to use Time::DurationSeconds as this value (so stuff natively prints in seconds).
     */
    template <typename BASE_CLOCK_T, typename DURATION_T = typename BASE_CLOCK_T::duration>
    struct AppStartZeroedClock {
    private:
        using Implementation_ = BASE_CLOCK_T;

    public:
        using duration                  = DURATION_T;
        using rep                       = typename duration::rep;
        using period                    = typename duration::period;
        using time_point                = chrono::time_point<AppStartZeroedClock<BASE_CLOCK_T, DURATION_T>>;
        static constexpr bool is_steady = Implementation_::is_steady;

    public:
        [[nodiscard]] static time_point now () noexcept;

    private:
        static const inline duration kTimeAppStartedOffset_ = chrono::duration_cast<duration> (BASE_CLOCK_T::now ().time_since_epoch ());
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Clock.inl"

#endif /*_Stroika_Foundation_Time_Clock_h_*/
