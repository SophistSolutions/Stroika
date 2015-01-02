/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_SystemTimes_h_
#define _Stroika_Framework_SystemPerformance_Instruments_SystemTimes_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Time/Realtime.h"

#include    "../Instrument.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {


#ifndef qSupport_SystemPerformance_Instruments_SystemTimes
#define qSupport_SystemPerformance_Instruments_SystemTimes  qPlatform_Windows
#endif
                namespace   SystemTimes {


                    /**
                     *  Instrument returning Info measurements (primarily windows only).
                     */
                    Instrument          GetInstrument (Time::DurationSecondsType measureInterval = 5.0);


                }


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
