/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            namespace Private_ {
                using namespace Traversal;
                struct DurationRangeTraitsType_ : RangeTraits::ExplicitRangeTraitsWithoutMinMax<Duration, Openness::eClosed, Openness::eClosed, int, unsigned int> {
                    static  const ElementType kLowerBound;
                    static  const ElementType kUpperBound;
                };
            };


            /**
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
