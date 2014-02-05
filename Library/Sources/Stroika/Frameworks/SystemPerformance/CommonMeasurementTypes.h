/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_CommonMeasurementTypes_h_
#define _Stroika_Framework_SystemPerformance_CommonMeasurementTypes_h_ 1

#include    "../StroikaPreComp.h"

#include    "Measurement.h"


/*
 * TODO:
 *      @todo
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {


            /*
             * This is the standard UNIX load average value - 1min, 5min, and 15min trailing averages of
             *  runq length. UNIX only
             */
            extern  const   MeasurementType kLoadAverage;

            /*
             * Portable average percent CPU usage(combines system and user time).
             */
            extern  const   MeasurementType kPercentCPUUsage;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_CommonMeasurementTypes_h_*/
