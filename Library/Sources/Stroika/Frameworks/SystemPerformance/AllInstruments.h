/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_AllInstruments_h_
#define _Stroika_Framework_SystemPerformance_AllInstruments_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Containers/Set.h"
#include "../../Foundation/DataExchange/Atom.h"

#include "Instrument.h"

/*
 * TODO:
 *      @todo
 */

namespace Stroika::Frameworks::SystemPerformance {

    /*
     *
     */
    Set<Instrument> GetAllInstruments ();
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Framework_SystemPerformance_AllInstruments_h_*/
