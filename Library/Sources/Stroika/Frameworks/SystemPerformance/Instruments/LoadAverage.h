/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_LoadAverage_h_
#define _Stroika_Framework_SystemPerformance_Instruments_LoadAverage_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Containers/Set.h"
#include    "../../../Foundation/DataExchange/Atom.h"

#include    "../Instrument.h"


/*
 * TODO:
 *      @todo
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {


                using   namespace   Stroika::Foundation;
                using   Characters::String;
                using   Containers::Set;


#ifndef qSupport_SystemPerformance_Instruments_LoadAverage
#define qSupport_SystemPerformance_Instruments_LoadAverage  qPlatform_POSIX
#endif


#if     qSupport_SystemPerformance_Instruments_LoadAverage
                Instrument  GetLoadAverage ();
#endif


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_LoadAverage_h_*/
