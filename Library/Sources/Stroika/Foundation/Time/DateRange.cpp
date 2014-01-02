/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "DateRange.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Time;

using   namespace   Time;

#if 1
const Date Time::Private_::DateRangeTraitsType_::kLowerBound   =   Date::kMin;
const Date Time::Private_::DateRangeTraitsType_::kUpperBound   =   Date::kMax;;
#endif
