/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTimeRange_h_
#define _Stroika_Foundation_Time_DateTimeRange_h_    1

#include    "../StroikaPreComp.h"

#include    "../Traversal/Range.h"

#include    "DateTime.h"



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
                struct DateTimeRangeTraitsType_ : RangeTraits::ExplicitRangeTraitsWithoutMinMax<DateTime, Openness::eClosed, Openness::eClosed, int, unsigned int> {
                    static  const ElementType kLowerBound;
                    static  const ElementType kUpperBound;
                };
            };


            /**
            */
            typedef Traversal::Range<DateTime, Time::Private_::DateTimeRangeTraitsType_> DateTimeRange;
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DateTimeRange.inl"

#endif  /*_Stroika_Foundation_Time_DateTimeRange_h_*/
