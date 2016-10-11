/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTimeRange_h_
#define _Stroika_Foundation_Time_DateTimeRange_h_    1

#include    "../StroikaPreComp.h"

#include    "../Traversal/Range.h"

#include    "DateTime.h"
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
                struct  DateTimeRangeTraitsType_ : RangeTraits::ExplicitRangeTraitsWithoutMinMax<DateTime, Openness::eClosed, Openness::eClosed, Time::Duration, Time::Duration> {
                    static  constexpr DateTime  kLowerBound  { DateTime_kMin };
                    static  constexpr DateTime  kUpperBound  { DateTime_kMax };
                };
            }


            /**
             */
            using       DateTimeRange       =   Traversal::Range<DateTime, Time::Private_::DateTimeRangeTraitsType_>;


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
