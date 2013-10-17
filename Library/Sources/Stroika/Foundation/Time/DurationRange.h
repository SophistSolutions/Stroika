/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
 *      @todo   Find way to cleanup/better share code with ExplicitRangeTraits<> code (better - and fix difference tyeps)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            namespace Private_ {
                using namespace Traversal;
#if 1
                typedef ExplicitRangeTraits_NC<Duration, Duration::kMin, Duration::kMax, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, int, unsigned int> DurationRangeTraitsType_;
#else
                struct DurationRangeTraitsType_ : ExplicitRangeTraitsWithoutMinMax<Duration, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, int, unsigned int> {
                    static  const ElementType kMin;
                    static  const ElementType kMax;
                };
#endif
            };


            /**
            */
            typedef Traversal::Range<Duration, Time::Private_::DurationRangeTraitsType_> DurationRange;
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
