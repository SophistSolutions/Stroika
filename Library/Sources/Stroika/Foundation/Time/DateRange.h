/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateRange_h_
#define _Stroika_Foundation_Time_DateRange_h_    1

#include    "../StroikaPreComp.h"

#include    "../Traversal/Range.h"

#include    "Date.h"



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
                struct DateRangeTraitsType_ : RangeTraits::ExplicitRangeTraitsWithoutMinMax<Date, Openness::eClosed, Openness::eClosed, int, unsigned int> {
                    static  const ElementType kLowerBound;
                    static  const ElementType kUpperBound;
                };
            };


            /**
            */
            typedef Traversal::Range<Date, Time::Private_::DateRangeTraitsType_> DateRange;
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DateRange.inl"

#endif  /*_Stroika_Foundation_Time_DateRange_h_*/
