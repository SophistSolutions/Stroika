/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/SystemPerformance/Instruments/CPU.h"
#include "Stroika/Frameworks/SystemPerformance/Instruments/Filesystem.h"
#include "Stroika/Frameworks/SystemPerformance/Instruments/Memory.h"
#include "Stroika/Frameworks/SystemPerformance/Instruments/Network.h"
#include "Stroika/Frameworks/SystemPerformance/Instruments/Process.h"

#include "AllInstruments.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

/*
 ********************************************************************************
 ******************* SystemPerformance::GetAllInstruments ***********************
 ********************************************************************************
 */
Set<Instrument> SystemPerformance::GetAllInstruments ()
{
    /*
     *  OK to cache - copyable - but construct AFTER main (only when called here).
     *
     *  Return a sorted set. Not strictly required, but looks better in some uses, and doesn't cost anything.
     */
    static Set<Instrument> kInstruments_ = SortedSet<Instrument>{
        Instruments::CPU::Instrument{},     Instruments::Filesystem::Instrument{}, Instruments::Memory::Instrument{},
        Instruments::Network::Instrument{}, Instruments::Process::Instrument{},
    };
    return kInstruments_;
}
