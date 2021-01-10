/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateRange_h_
#define _Stroika_Foundation_Time_DateRange_h_ 1

#include "../StroikaPreComp.h"

#include "../Traversal/DiscreteRange.h"

#include "Date.h"

/**
 *  \file
 *
 *  *** FILE DEPRECATED ***
 */

namespace Stroika::Foundation::Time {

    /**
     *  \brief  typically use DateRange, but SimpleDateRange can be used as constexpr (since its not iterable)
     *
     *  @see DateRange
     */
    using SimpleDateRange [[deprecated ("Since Stroika v2.1b8, just use Range<Date>")]] = Traversal::Range<Date>;

    /**
     *
     *  @see SimpleDateRange
     */
    using DateRange [[deprecated ("Since Stroika v2.1b8, just use DiscreteRange<Date>")]] = Traversal::DiscreteRange<Date>;

}

#endif /*_Stroika_Foundation_Time_DateRange_h_*/
