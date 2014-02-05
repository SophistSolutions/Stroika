/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_SystemTimes_h_
#define _Stroika_Framework_SystemPerformance_Instruments_SystemTimes_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Time/Realtime.h"

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


#ifndef qSupport_SystemPerformance_Instruments_SystemTimes
#define qSupport_SystemPerformance_Instruments_SystemTimes  qPlatform_Windows
#endif


#if     qSupport_SystemPerformance_Instruments_SystemTimes
                Instrument  GetSystemTimes (Time::DurationSecondsType measureInterval = 5.0);
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

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_SystemTimes_h_*/
