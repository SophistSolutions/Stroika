/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_CPU_h_
#define _Stroika_Framework_SystemPerformance_Instruments_CPU_h_ 1

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../../Foundation/DataExchange/ObjectVariantMapper.h"

#include "../Instrument.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Windows fRunQLength value is a point-in-time snapshot, and it SHOULD be (if we can find out how)
 *              an average over the measurement interval.
 *
 *      @todo   Consider making fTotalProcessCPUUsage and fTotalCPUUsage from Info struct - 0..N where N is numebr of CPUs.
 *              Reason is just for consistency sake with data returned in Process  module. OR - make the inverse
 *              change there? But it would be better if these were consistent.
 *
 *      @todo   GetSystemConfiguration_CPU ().GetNumberOfLogicalCores () is cached on Linux, but CAN change while running (crazy - right?)
 *              But so rare and so costly to recompute. Find a way to compromise.
 *              MAYBE the issue is differnt - hot plug cpu - CPU online/offline. Maybe total cannot change, but just number
 *              online?
 *
 *              @see /sys/devices/system/cpu/online
 *              @see https://www.kernel.org/doc/Documentation/cpu-hotplug.txt
 */

namespace Stroika::Frameworks::SystemPerformance::Instruments::CPU {

// @todo now we say iff Linux, but also available on BSD, Solaris, and could fetch with procfs
#ifndef qSupport_SystemPerformance_Instruments_CPU_LoadAverage
#define qSupport_SystemPerformance_Instruments_CPU_LoadAverage (qPlatform_Linux)
#endif

    using DataExchange::ObjectVariantMapper;

    /**
     *
     */
    struct Info {
#if qSupport_SystemPerformance_Instruments_CPU_LoadAverage
        struct LoadAverage {
            double f1MinuteAve{};
            double f5MinuteAve{};
            double f15MinuteAve{};
            LoadAverage () = default;
            LoadAverage (double oneMinuteAve, double fiveMinuteAve, double fifteenMinuteAve);
        };
        optional<LoadAverage> fLoadAverage;
#endif

        /**
         *  This is a number from 0..1, and is the weighted average across all CPU cores (so
         *  even on a 4 core machine, this can never execeed 1).
         *
         *  This restricts to process usage identifyable attributed to a process (including system processes).
         *  It does not count time handling interupts.
         */
        double fTotalProcessCPUUsage{};

        /**
         *  This is a number from 0..1, and is the weighted average across all CPU cores (so
         *  even on a 4 core machine, this can never execeed 1).
         */
        double fTotalCPUUsage{};

        /**
         *  This is the average number of threads waiting in the run-q (status runnable) / number of logical cores.
         *  If the system is not busy, it should be zero. When more than 4 or 5 (depends alot on system) - performance maybe degraded.
         *
         *  \note   On some systems, this may included threads in disk wait. We try to avoid that, but depend
         *          on low level data, and may not be able to avoid it.
         *
         *  This is essentially the same as the UNIX 'load average' concept, except that the time frame
         *  is not 1/5/15 minutes, but the time frame over which you've sampled, and that we normalize the result
         *  to be if we had a single CPU core.
         *
         *  \note   This is scaled to the number of CPUs, so a load average of 4 on a 4 CPU system is like
         *          would produce a 'RunQLength' of 1.
         *
         *  \note   This is very similar to Windows System \ Processor Queue Length, except that that doesn't take
         *          into account running threads, and fRunQLength does.
         *
         *  \note   We chose to define this differently than 'windows Processor Queue Length' and UNIX 'loadave' so that
         *          o   Same def on UNIX/Windows
         *          o   Largely independent measure of CPU load regardless of how many logical cores machine has
         *              (0 means no use, 1 means ALL cores fully used with no Q, and 2 means all cores fully utilized and
         *              each core with a Q length of 1).
         */
        optional<double> fRunQLength{};

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  For Info type.
     */
    ObjectVariantMapper GetObjectVariantMapper ();

    /**
     *  To control the behavior of the instrument.
     */
    struct Options {
        /**
         *  \req fMinimumAveragingInterval >= 0
         */
        Time::DurationSecondsType fMinimumAveragingInterval{1.0};
    };

    /**
     */
    Instrument GetInstrument (Options options = Options ());
}

namespace Stroika::Frameworks::SystemPerformance {

    /*
     *  Specialization to improve performance
     */
    template <>
    Instruments::CPU::Info Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Framework_SystemPerformance_Instruments_CPU_h_*/
