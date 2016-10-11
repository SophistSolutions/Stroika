/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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



#if     qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
/*
 ********************************************************************************
 ************************* Private_::DateRange_ModuleData_ **********************
 ********************************************************************************
 */
Time::Private_::DateRange_ModuleData_::DateRange_ModuleData_ ()
    : fLowerBound (Execution::ModuleInitializer<Time::Private_::Date_ModuleData_>::Actual ().fMin)
    , fUpperBound (Execution::ModuleInitializer<Time::Private_::Date_ModuleData_>::Actual ().fMax)
{
}
#endif



/*
 ********************************************************************************
 ************************* Private_::DateRangeTraitsType_ ***********************
 ********************************************************************************
 */
#if     qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
const Date& Time::Private_::DateRangePlainTraitsType_::kLowerBound   =   Execution::ModuleInitializer<Time::Private_::DateRange_ModuleData_>::Actual ().fLowerBound;
const Date& Time::Private_::DateRangePlainTraitsType_::kUpperBound   =   Execution::ModuleInitializer<Time::Private_::DateRange_ModuleData_>::Actual ().fUpperBound;
#endif
