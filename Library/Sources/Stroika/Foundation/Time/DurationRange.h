/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DurationRange_h_
#define _Stroika_Foundation_Time_DurationRange_h_    1

#include    "../StroikaPreComp.h"

#include    "../Traversal/Range.h"

#include    "Duration.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html">Alpha-Early</a>
 *
 * TODO:
 *      @todo   IF we ever fix Duration to use constexpr for its kMin/kMax, then we can consider doing likewise here, and
 *              losing the ModuleInit<> code (instead using static  constexpr Duration  kLowerBound;).
 *
 *              If we fix, lose note about kLowerBound/etc on DurationRange type.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            namespace Private_ {
                using namespace Traversal;
                struct  DurationRangeTraitsType_ : RangeTraits::ExplicitRangeTraitsWithoutMinMax<Duration, Openness::eClosed, Openness::eClosed, int, unsigned int> {
                    static  const Duration&     kLowerBound;
                    static  const Duration&     kUpperBound;
                };
            };


            /**
             *  \note   This type properties (kLowerBound/kUpperBound) can only be used after static initialization, and before
             *          static de-initializaiton.
            */
            using   DurationRange   =   Traversal::Range<Duration, Time::Private_::DurationRangeTraitsType_>;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DurationRange.inl"

#endif  /*_Stroika_Foundation_Time_DurationRange_h_*/
