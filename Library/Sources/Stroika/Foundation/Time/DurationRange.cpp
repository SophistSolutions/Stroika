/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "DurationRange.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Time;

using   namespace   Time;

#if 1
const Duration Time::Private_::DurationRangeTraitsType_::kLowerBound   =   Duration::kMin;
const Duration Time::Private_::DurationRangeTraitsType_::kUpperBound   =   Duration::kMax;;
#endif