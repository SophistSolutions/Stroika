/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
 *
 * TODO:
 *      @todo   IF we ever fix Duration to use constexpr for its kMin/kMax, then we can consider doing likewise here, and
 *              losing the ModuleInit<> code (instead using static  constexpr Duration  kLowerBound;).
 *
 *              If we fix, lose note about kLowerBound/etc on DurationRange type.
 */

namespace Stroika::Foundation::Time {

    namespace Private_ {
        using namespace Traversal;
        struct DurationRangeTraitsType_ : RangeTraits::ExplicitRangeTraitsWithoutMinMax<Duration, Openness::eClosed, Openness::eClosed, DurationSecondsType, DurationSecondsType> {
            static const Duration& kLowerBound;
            static const Duration& kUpperBound;

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
