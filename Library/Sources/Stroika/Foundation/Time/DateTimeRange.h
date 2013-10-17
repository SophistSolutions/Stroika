/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
 *      @todo   Find way to cleanup/better share code with ExplicitRangeTraits<> code (BETTER) - and fix diff-type
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {

            namespace Private_ {
                using namespace Traversal;
                struct DateTimeRangeTraitsType_ : ExplicitRangeTraits<DateTime, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, int, unsigned int> {
                    static  const ElementType kMin;
                    static  const ElementType kMax;
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
