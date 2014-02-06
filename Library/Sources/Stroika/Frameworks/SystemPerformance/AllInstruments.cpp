/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Instruments/LoadAverage.h"
#include    "Instruments/MountedFilesystemUsage.h"
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
        Instruments::GetLoadAverage (),
#endif
#if     qSupport_SystemPerformance_Instruments_SystemTimes
        Instruments::GetSystemTimes (),
#endif
        Instruments::GetMountedFilesystemUsage (),
    };
    return kInstruments_;
}
