/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Instruments/Memory.h"
#include    "Instruments/MountedFilesystemUsage.h"
#include    "Instruments/NetworkInterfaces.h"
#include    "Instruments/ProcessDetails.h"
#include    "Instruments/SystemCPU.h"

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
        Instruments::Memory::GetInstrument (),
        Instruments::NetworkInterfaces::GetInstrument (),
        Instruments::ProcessDetails::GetInstrument (),
        Instruments::MountedFilesystemUsage::GetInstrument (),
        Instruments::SystemCPU::GetInstrument (),
    };
    return kInstruments_;
}
