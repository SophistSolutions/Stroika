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
 *      @todo   IF we ever fix DateTime to use constexpr for its kMin/kMax, then we can consider doing likewise here, and
 *              losing the ModuleInit<> code (instead using static  constexpr DateTime  kLowerBound;).
 *
 *              If we fix, lose note about kLowerBound/etc on DateTimeRange type.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {

            namespace Private_ {
                using namespace Traversal;
                struct DateTimeRangeTraitsType_ : RangeTraits::ExplicitRangeTraitsWithoutMinMax<DateTime, Openness::eClosed, Openness::eClosed, int, unsigned int> {
                    static  const DateTime& kLowerBound;
                    static  const DateTime& kUpperBound;
                };
            };


            /**
             *  \note   This type properties (kLowerBound/kUpperBound) can only be used after static initialization, and before
             *          static de-initializaiton.
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
