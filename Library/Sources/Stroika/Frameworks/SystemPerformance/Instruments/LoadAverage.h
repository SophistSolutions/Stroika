/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_LoadAverage_h_
#define _Stroika_Framework_SystemPerformance_Instruments_LoadAverage_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/DataExchange/ObjectVariantMapper.h"

#include    "../Instrument.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 */
_DeprecatedFile_ ("OBSOLETE - as of Stroika 2.0a92");


namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {


#ifndef qSupport_SystemPerformance_Instruments_LoadAverage
#define qSupport_SystemPerformance_Instruments_LoadAverage  qPlatform_POSIX
#endif


                namespace   LoadAverage {


                    using   DataExchange::ObjectVariantMapper;

                    /**
                     *
                     */
                    struct  Info {
                        double  f1MinuteAve;
                        double  f5MinuteAve;
                        double  f15MinuteAve;
                        Info (double oneMinuteAve, double fiveMinuteAve, double fifteenMinuteAve);
                    };


                    /**
                     *  For Info type.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();


#if     qSupport_SystemPerformance_Instruments_LoadAverage
                    /**
                     *  Instrument returning Info measurements (mainly for Linux/POSIX).
                     */
                    Instrument          GetInstrument ();
#endif


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

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_LoadAverage_h_*/
