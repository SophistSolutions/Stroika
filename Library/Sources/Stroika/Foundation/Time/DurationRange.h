/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DurationRange_h_
#define _Stroika_Foundation_Time_DurationRange_h_ 1

#include "../StroikaPreComp.h"

#include "../Traversal/Range.h"

#include "Duration.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>
 */

namespace Stroika::Foundation::Time {

    namespace Private_ {
        using namespace Traversal;
        struct DurationRangeTraitsType_ : RangeTraits::ExplicitRangeTraitsWithoutMinMax<Duration, Openness::eClosed, Openness::eClosed, DurationSecondsType, DurationSecondsType> {
            static inline const Duration kLowerBound = Duration::min ();
            static inline const Duration kUpperBound = Duration::max ();

            static Duration GetNext (Duration i);
            static Duration GetPrevious (Duration i);
        };
    };

    /**
     *  \note   This type properties (kLowerBound/kUpperBound) can only be used after static initialization, and before
     *          static de-initializaiton.
     */
    using DurationRange = Traversal::Range<Duration, Time::Private_::DurationRangeTraitsType_>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DurationRange.inl"

#endif /*_Stroika_Foundation_Time_DurationRange_h_*/
