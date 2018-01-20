/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "DurationRange.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Time;

using namespace Time;

/*
 ********************************************************************************
 ************************* Private_::DurationRange_ModuleData_ ******************
 ********************************************************************************
 */
Time::Private_::DurationRange_ModuleData_::DurationRange_ModuleData_ ()
    : fLowerBound (Execution::ModuleInitializer<Time::Private_::Duration_ModuleData_>::Actual ().fMin)
    , fUpperBound (Execution::ModuleInitializer<Time::Private_::Duration_ModuleData_>::Actual ().fMax)
{
}

/*
 ********************************************************************************
 ************************* Private_::DurationRangeTraitsType_ *******************
 ********************************************************************************
 */
const Duration& Time::Private_::DurationRangeTraitsType_::kLowerBound = Execution::ModuleInitializer<Time::Private_::DurationRange_ModuleData_>::Actual ().fLowerBound;
const Duration& Time::Private_::DurationRangeTraitsType_::kUpperBound = Execution::ModuleInitializer<Time::Private_::DurationRange_ModuleData_>::Actual ().fUpperBound;
