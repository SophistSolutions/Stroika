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
 *      @todo   Find way to cleanup/better share code with ExplicitRangeTraits<> code
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {

            namespace Private_ {
                using namespace Traversal;
                struct DurationRangeTraitsType_ {
                    typedef Duration                ElementType;
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
