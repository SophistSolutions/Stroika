/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Clock_h_
#define _Stroika_Foundation_Time_Clock_h_ 1

#include "../StroikaPreComp.h"

#include <chrono>

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>
 */

namespace Stroika::Foundation::Time {

    /*
     *  \brief like std::chrono::clock_cast, but supports steady_clock, and others not explicitly supported by std::chrono::clock_cast (through experiment/approximation)
     * 
     *  \see commentary in https://stackoverflow.com/questions/35282308/convert-between-c11-clocks
     * 
     *  \note - for some cases, the conversion is estimated, and may vary slightly from run run to run.
     */
    template <typename DESTINATION_CLOCK_T, typename SOURCE_CLOCK_T, typename DURATION_T>
    typename DESTINATION_CLOCK_T::time_point clock_cast (chrono::time_point<SOURCE_CLOCK_T, DURATION_T> tp);

    /**
     *  AppStartZeroedClock is just like std::chrono::steady_clock, except that its time values are magically adjusted so that
     *  zero corresponds to when the application (code starts) begins.
     */
    template <typename BASE_CLOCK_T>
    struct AppStartZeroedClock {
    private:
        using Implementation_ = BASE_CLOCK_T;

    public:
        using rep                       = Implementation_::rep;
        using period                    = Implementation_::period;
        using duration                  = Implementation_::duration;
        using time_point                = Implementation_::time_point;
        static constexpr bool is_steady = Implementation_::is_steady;

    public:
        static [[nodiscard]] time_point now () noexcept
        {
            return Implementation_::now () - kTimeAppStartedOffset_;
        }

    private:
        static const inline duration kTimeAppStartedOffset_ = chrono::duration_cast<duration> (chrono::steady_clock::now ().time_since_epoch ());
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Clock.inl"

#endif /*_Stroika_Foundation_Time_Clock_h_*/
