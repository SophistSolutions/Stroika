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
 *      @todo   Find way to cleanup/better share code with ExplicitRangeTraits<> code
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {

            namespace Private_ {
                using namespace Traversal;
                struct DateRangeTraitsType_ {
                    typedef Date                ElementType;
                    typedef int    SignedDifferenceType;
                    typedef unsigned int  UnsignedDifferenceType;

                    static  constexpr   RangeBase::Openness    kBeginOpenness  =   RangeBase::Openness::eClosed;
                    static  constexpr   RangeBase::Openness    kEndOpenness    =   RangeBase::Openness::eClosed;

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
