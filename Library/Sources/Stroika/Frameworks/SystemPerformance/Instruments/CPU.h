/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_CPU_h_
#define _Stroika_Framework_SystemPerformance_Instruments_CPU_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/DataExchange/ObjectVariantMapper.h"

#include    "../Instrument.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Windows fRunQLength value is a point-in-time snapshot, and it SHOULD be (if we can find out how)
 *              an average over the measurement interval.
 *
 *      @todo   Consider making fTotalProcessCPUUsage and fTotalCPUUsage from Info struct - 0..N where N is numebr of CPUs.
 *              Reason is just for consistency sake with data returned in Process  module. OR - make the inverse
 *              change there? But it would be better if these were consistent.
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {
                namespace   CPU {


                    // @todo now we say iff Linux, but also available on BSD, Solaris, and could fetch with procfs
#ifndef qSupport_SystemPerformance_Instruments_CPU_LoadAverage
#define qSupport_SystemPerformance_Instruments_CPU_LoadAverage  (qPlatform_Linux || qPlatform_AIX)
#endif


                    using   DataExchange::ObjectVariantMapper;
                    using   Foundation::Memory::Optional;
                    using   Foundation::Memory::Optional_Indirect_Storage;


                    /**
                     *
                     */
                    struct  Info {
#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
                        struct  LoadAverage;
                        Optional_Indirect_Storage<LoadAverage>  fLoadAverage;
#endif

                        /**
                         *  This is a number from 0..1, and is the weighted average across all CPU cores (so
                         *  even on a 4 core machine, this can never execeed 1).
                         *
                         *  This restricts to process usage identifyable attributed to a process (including system processes).
                         *  It does not count time handling interupts.
                         */
                        double  fTotalProcessCPUUsage {};

                        /**
                         *  This is a number from 0..1, and is the weighted average across all CPU cores (so
                         *  even on a 4 core machine, this can never execeed 1).
                         */
                        double  fTotalCPUUsage {};

                        /**
                         *  This is the average number of threads waiting in the run-q (status runnable). If the system is not busy, it
                         *  should be zero. When more than 4 or 5 (depends alot on system) - performance maybe degraded.
                         *
                         *  \note   On some systems, load average includes threads in disk wait. We try to avoid that, but depend
                         *          on low level data, and may not be able to avoid it.
                         *
                         *  This is essentially the same as the UNIX 'load average' concept, except that the time frame
                         *  is not 1/5/15 minutes, but the time frame over which you've sampled.
                         */
                        Optional<double>    fRunQLength {};
                    };


#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
                    struct  Info::LoadAverage {
                        double  f1MinuteAve {};
                        double  f5MinuteAve {};
                        double  f15MinuteAve {};
                        LoadAverage () = default;
                        LoadAverage (double oneMinuteAve, double fiveMinuteAve, double fifteenMinuteAve);
                    };
#endif

                    /**
                     *  For Info type.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();


                    /**
                     *  To control the behavior of the instrument.
                     */
                    struct  Options {
                        /**
                         *  \req fMinimumAveragingInterval >= 0
                         */
                        Time::DurationSecondsType   fMinimumAveragingInterval { 1.0 };
                    };


                    /**
                     */
                    Instrument          GetInstrument (Options options = Options ());


                }
            }


            /*
             *  Specialization to improve performance
             */
            template    <>
            Instruments::CPU::Info   Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_CPU_h_*/
