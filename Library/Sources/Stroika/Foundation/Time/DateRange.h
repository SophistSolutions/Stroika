/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
 *      @todo   Find way to cleanup/better share code with ExplicitRangeTraits<> code - better - and fix difference types
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            namespace Private_ {
                using namespace Traversal;
                struct DateRangeTraitsType_ : ExplicitRangeTraitsWithoutMinMax<Date, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, int, unsigned int> {
                    static  const ElementType kMin;
                    static  const ElementType kMax;
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
