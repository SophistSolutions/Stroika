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

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Clock.inl"

#endif /*_Stroika_Foundation_Time_Clock_h_*/
