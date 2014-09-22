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
 *      @todo   IF we ever fix Date to use constexpr for its kMin/kMax, then we can consider doing likewise here, and
 *              losing the ModuleInit<> code (using static  constexpr Date      kLowerBound)
 *
 *              If we fix, lose note about kLowerBound/etc on DateTimeRange type.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            namespace Private_ {
                using namespace Traversal;
                struct  DateRangeTraitsType_ : RangeTraits::ExplicitRangeTraitsWithoutMinMax<Date, Openness::eClosed, Openness::eClosed, int, unsigned int> {
                    static  const Date&     kLowerBound;
                    static  const Date&     kUpperBound;
                };
            };


            /**
             *
             *  \note   This type properties (kLowerBound/kUpperBound) can only be used after static initialization, and before
             *          static de-initializaiton.
            */
            using       DateRange       =   Traversal::Range<Date, Time::Private_::DateRangeTraitsType_>;

		
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
