/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Instruments/CPU.h"
#include    "Instruments/Memory.h"
#include    "Instruments/Filesystem.h"
#include    "Instruments/Network.h"
#include    "Instruments/Process.h"

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
    /*
     *  OK to cache - copyable - but construct AFTER main (only when called here).
     *
     *  Return a sorted set. Not strictly required, but looks better in some uses, and doesn't cost anything.
     */
    static  InstrumentSet   kInstruments_ = SortedSet<Instrument, Private_::InstrumentSetTraits_> {
        Instruments::CPU::GetInstrument (),
        Instruments::Memory::GetInstrument (),
        Instruments::Filesystem::GetInstrument (),
        Instruments::Network::GetInstrument (),
        Instruments::Process::GetInstrument (),
    };
    return kInstruments_;
}
