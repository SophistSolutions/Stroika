/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateRange_h_
#define _Stroika_Foundation_Time_DateRange_h_ 1

#include "../StroikaPreComp.h"

#include "../Traversal/DiscreteRange.h"

#include "Date.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>
 */

namespace Stroika {
    namespace Foundation {
        namespace Time {

            namespace Private_ {
                using namespace Traversal;
                struct DateRangePlainTraitsType_ : RangeTraits::ExplicitRangeTraitsWithoutMinMax<Date, Openness::eClosed, Openness::eClosed, int, unsigned int> {
                    static constexpr Date kLowerBound{Date::min ()};
                    static constexpr Date kUpperBound{Date::max ()};
                };
                struct DateRangeTraitsType_ : DateRangePlainTraitsType_ {
                    static Date GetNext (Date n)
                    {
                        return n.AddDays (1);
                    }
                    using RangeTraitsType = DateRangePlainTraitsType_;
                };
            };

            /**
             *  \brief  typically use DateRange, but SimpleDateRange can be used as constexpr (since its not iterable)
             *
             *  @see DateRange
             */
            using SimpleDateRange = Traversal::Range<Date, Time::Private_::DateRangePlainTraitsType_>;

            /**
             *
             *  @see SimpleDateRange
             */
            using DateRange = Traversal::DiscreteRange<Date, Time::Private_::DateRangeTraitsType_>;
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DateRange.inl"

#endif /*_Stroika_Foundation_Time_DateRange_h_*/
