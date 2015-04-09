/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Instruments/LoadAverage.h"
#include    "Instruments/MountedFilesystemUsage.h"
#include    "Instruments/NetworkInterfaces.h"
#include    "Instruments/ProcessDetails.h"
#include    "Instruments/SystemCPU.h"
#include    "Instruments/SystemTimes.h"

#include    "AllInstruments.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;





/*
 ********************************************************************************
 ******************* SystemPerformance::GetAllInstruments ***********************
 ********************************************************************************
 */
InstrumentSet   SystemPerformance::GetAllInstruments ()
{
    // OK to cache - copyable - but construct AFTER main (only when called here)
    static  InstrumentSet   kInstruments_ = {
#if     qSupport_SystemPerformance_Instruments_LoadAverage
        Instruments::LoadAverage::GetInstrument (),
#endif
#if     qSupport_SystemPerformance_Instruments_SystemTimes
        Instruments::SystemTimes::GetInstrument (),
#endif
        Instruments::NetworkInterfaces::GetInstrument (),
        Instruments::ProcessDetails::GetInstrument (),
        Instruments::MountedFilesystemUsage::GetInstrument (),
        Instruments::SystemCPU::GetInstrument (),
    };
    return kInstruments_;
}
