/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
 *              Also - it appears strangely to be less than the 
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
         *  This is natural number (typically, 4, or 8, or something like that). But this CAN change over time
         *              @see /sys/devices/system/cpu/online
         *              @see https://www.kernel.org/doc/Documentation/cpu-hotplug.txt
         *  Crazy - right?
         * 
         *  This is can be used to NORMALIZE the value of fTotalCPUUsage or fTotalProcessCPUUsage to be 0..1
         */
        optional<unsigned int> fTotalLogicalCores{};

        /**
         *  This is a number from 0..fTotalLogicalCores, and is the weighted average across all CPU cores.
         *  If you have 4 logical cores, all fully occupied, this will return 4.0.
         * 
         *  @see GetTotalCPURatio
         */
        optional<double> fTotalCPUUsage{};

        /**
         *  \brief like fTotalCPUUsage, but returns # 0..1 (so 'percentage' of all CPU available on that machine)
         */
        optional<double> GetTotalCPURatio () const
        {
            if (fTotalLogicalCores and fTotalCPUUsage) {
                return *fTotalCPUUsage / *fTotalLogicalCores;
            }
            return nullopt;
        }

        /**
         *  \brief Same as fTotalCPUUsage, except not counting time spent handling interrupts
         *
         *  This is a number from 0..fTotalLogicalCores, and is the weighted average across all CPU cores.
         *  If you have 4 logical cores, all fully occupied, this will return 4.0.
         *
         *  This restricts to process usage identifyable attributed to a process (including system processes).
         *  It does not count time handling interupts.
         * 
         *  @see GetTotalProcessCPUUsage
         */
        optional<double> fTotalProcessCPUUsage{};

        /**
         *  \brief like fTotalCPUUsage, but returns # 0..1 (so 'percentage' of all CPU available on that machine)
         */
        optional<double> GetTotalProcessCPUUsage () const
        {
            if (fTotalLogicalCores and fTotalProcessCPUUsage) {
                return *fTotalProcessCPUUsage / *fTotalLogicalCores;
            }
            return nullopt;
        }

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
     *  To control the behavior of the instrument.
     */
    struct Options {
        /**
         *  To compute averages, the instrument may keep around some earlier snapshots of data. This time interval is regulated by how often
         *  the capture is called (typically the Captureset::'run interval'. However, this value can be used to override that partly, and provide
         *  a minimum time for averaging.
         *
         *  If you call capture more frequently than this interval, some (averaged) items maybe missing from the result.
         *
         *  \req fMinimumAveragingInterval > 0
         */
        Time::DurationSecondsType fMinimumAveragingInterval{1.0};
    };

    /**
     *  This class is designed to be object-sliced into just the SystemPerformance::Instrument
     * 
     *  \note Constructing the instrument does no capturing (so sb quick/cheap) - capturing starts when you
     *        first call i.Capture()
     */
    struct Instrument : SystemPerformance::Instrument {
    public:
        Instrument (const Options& options = Options{});

    public:
        /**
         *  For Instruments::CPU::Info, etc types.
         */
        static const ObjectVariantMapper kObjectVariantMapper;
    };

    [[deprecated ("Since Stroika 2.1b12, use CPU::Instrument instead of CPU::GetInstrument()")]] inline SystemPerformance::Instrument GetInstrument (Options options = Options{})
    {
        return Instrument{options};
    }
    [[deprecated ("Since Stroika 2.1b12, use CPU::Instrument instead of CPU::Instrument::kObjectVariantMapper")]] inline ObjectVariantMapper GetObjectVariantMapper ()
    {
        return Instrument::kObjectVariantMapper;
    }

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
