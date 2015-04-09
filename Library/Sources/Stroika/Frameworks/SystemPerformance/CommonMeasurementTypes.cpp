/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/String_Constant.h"

#include    "CommonMeasurementTypes.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;

using   Characters::String_Constant;


const   MeasurementType SystemPerformance::kMemoryUsage         =   MeasurementType (String_Constant (L"Memory-Usage"));
const   MeasurementType SystemPerformance::kMountedVolumeUsage  =   MeasurementType (String_Constant (L"Mounted-Volume-Usage"));
