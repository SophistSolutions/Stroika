/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_CommonMeasurementTypes_h_
#define _Stroika_Framework_SystemPerformance_CommonMeasurementTypes_h_ 1

#include    "../StroikaPreComp.h"

#include    "Measurement.h"


/*
 * TODO:
 *      @todo   Consider losing this file, and storing these in the module defining the instrument?
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {


            /*
             */
            extern  const   MeasurementType kMemoryUsage;


            /*
             * Portable listing of mounted disk usage (like UNIX df)
             */
            extern  const   MeasurementType kMountedVolumeUsage;

        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_CommonMeasurementTypes_h_*/
