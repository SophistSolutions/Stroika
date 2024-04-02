/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_AllInstruments_h_
#define _Stroika_Framework_SystemPerformance_AllInstruments_h_ 1

#include "../StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/Atom.h"

#include "Instrument.h"

/*
 * TODO:
 *      @todo
 */

namespace Stroika::Frameworks::SystemPerformance {

    /*
     * Return a list of ALL the known (builtin to Stroika) Instruments (see @Instrument).
     */
    Set<Instrument> GetAllInstruments ();

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Framework_SystemPerformance_AllInstruments_h_*/
