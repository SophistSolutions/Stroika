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
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {
                namespace   CPU {


                    // @todo now we say iff Linux, but also available on BSD, Solaris, and could fetch with procfs
#ifndef qSupport_SystemPerformance_Instruments_CPU_LoadAverage
#define qSupport_SystemPerformance_Instruments_CPU_LoadAverage  qPlatform_Linux
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
                         *  This is the average number of threads waiting in the runq. If the system is not busy, it
                         *  should be zero. When more than 4 or 5 (depends alot on system) - performance maybe degraded.
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
