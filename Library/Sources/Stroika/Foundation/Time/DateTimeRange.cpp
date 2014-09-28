/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "DateTimeRange.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Time;

using   namespace   Time;




/*
 ********************************************************************************
 ************************* Private_::DateTimeRange_ModuleData_ ******************
 ********************************************************************************
 */
#if     qCompilerAndStdLib_constexpr_Buggy || qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy
Time::Private_::DateTimeRange_ModuleData_::DateTimeRange_ModuleData_ ()
    : fLowerBound (Execution::ModuleInitializer<Time::Private_::DateTime_ModuleData_>::Actual ().fMin)
    , fUpperBound (Execution::ModuleInitializer<Time::Private_::DateTime_ModuleData_>::Actual ().fMax)
{
}
#endif


/*
 ********************************************************************************
 ************************* Private_::DateTimeRangeTraitsType_ *******************
 ********************************************************************************
 */
#if     qCompilerAndStdLib_constexpr_Buggy || qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy
const DateTime& Time::Private_::DateTimeRangeTraitsType_::kLowerBound   =   Execution::ModuleInitializer<Time::Private_::DateTimeRange_ModuleData_>::Actual ().fLowerBound;
const DateTime& Time::Private_::DateTimeRangeTraitsType_::kUpperBound   =   Execution::ModuleInitializer<Time::Private_::DateTimeRange_ModuleData_>::Actual ().fUpperBound;
#endif