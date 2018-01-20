/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "DateTimeRange.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Time;

using namespace Time;

constexpr DateTime Time::Private_::DateTimeRangeTraitsType_::kLowerBound;
constexpr DateTime Time::Private_::DateTimeRangeTraitsType_::kUpperBound;
