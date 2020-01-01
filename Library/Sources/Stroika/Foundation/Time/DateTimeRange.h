/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTimeRange_h_
#define _Stroika_Foundation_Time_DateTimeRange_h_ 1

#include "../StroikaPreComp.h"

#include "../Traversal/Range.h"

#include "DateTime.h"
#include "Duration.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Beta</a>
 */

namespace Stroika::Foundation::Time {

    namespace Private_ {
        using namespace Traversal;
        struct DateTimeRangeTraitsType_ : RangeTraits::ExplicitRangeTraitsWithoutMinMax<DateTime, Openness::eClosed, Openness::eClosed, Time::Duration, Time::Duration> {
            static constexpr DateTime kLowerBound{DateTime::min ()};
            static constexpr DateTime kUpperBound{DateTime::max ()};
        };
    }

    /**
     */
    using DateTimeRange = Traversal::Range<DateTime, Time::Private_::DateTimeRangeTraitsType_>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DateTimeRange.inl"

#endif /*_Stroika_Foundation_Time_DateTimeRange_h_*/
