/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <optional>
#include <filesystem>

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Configuration/SystemConfiguration.h"
#include "Stroika/Foundation/DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/ProcessRunner.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/TextReader.h"
#include "Stroika/Frameworks/SystemPerformance/Support/InstrumentHelpers.h"

#include "CPU.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

using Instruments::CPU::Info;
using Instruments::CPU::Options;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_ qPlatform_Windows
#endif

#if qUseWMICollectionSupport_
#include "Stroika/Frameworks/SystemPerformance/Support/WMICollector.h"

using SystemPerformance::Support::WMICollector;
#endif

#if qUseWMICollectionSupport_
namespace {
    const String kInstanceName_{""sv};

    const String kProcessorQueueLength_{"Processor Queue Length"sv};
}
#endif

#if qSupport_SystemPerformance_Instruments_CPU_LoadAverage
/*
 ********************************************************************************
 *********************** Instruments::CPU::Info::LoadAverage ********************
 ********************************************************************************
 */
Instruments::CPU::Info::LoadAverage::LoadAverage (double oneMinuteAve, double fiveMinuteAve, double fifteenMinuteAve)
    : f1MinuteAve{oneMinuteAve}
    , f5MinuteAve{fiveMinuteAve}
    , f15MinuteAve{fifteenMinuteAve}
{
}
#endif

/*
 ********************************************************************************
 **************************** Instruments::CPU::Info ****************************
 ********************************************************************************
 */
String Instruments::CPU::Info::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (CPU::Instrument::kObjectVariantMapper.FromObject (*this));
}

namespace {
    template <typename CONTEXT>
    using InstrumentRepBase_ = SystemPerformance::Support::InstrumentRep_COMMON<Options, CONTEXT>;
}

#if qSupport_SystemPerformance_Instruments_CPU_LoadAverage
namespace {
    template <typename ELT>
    double EstimateRunQFromLoadAveArray_ (Time::DurationSeconds::rep backNSeconds, ELT loadAveArray[3])
    {
        // NB: Currently this is TOO simple. We should probably fit a curve to 3 points and use that to extrapolate. Maybe just fit 4 line segments?
        Require (backNSeconds >= 0);
        double backNMinutes = backNSeconds / 60.0;
        if (backNMinutes <= 1) {
            return static_cast<double> (loadAveArray[0]);
        }
        else if (backNMinutes <= 5) {
            double distFrom1 = (backNMinutes - 1);
            double distFrom5 = (5.0 - backNMinutes);
            return static_cast<double> (loadAveArray[0]) * (1.0 - distFrom1 / 4) + static_cast<double> (loadAveArray[1]) * (1.0 - distFrom5 / 4);
        }
        else if (backNMinutes <= 15) {
            double distFrom5  = (backNMinutes - 5);
            double distFrom15 = (15.0 - backNMinutes);
            return static_cast<double> (loadAveArray[1]) * (1.0 - distFrom5 / 10) + static_cast<double> (loadAveArray[2]) * (1.0 - distFrom15 / 10);
        }
        else {
            return static_cast<double> (loadAveArray[2]);
        }
    }
}
#endif

#if qPlatform_Linux
namespace {
    struct POSIXSysTimeCaptureContext_ {
        double user;
        double nice;
        double system;
        double idle;
        double iowait;
        double irq;
        double softirq;
        double steal;
        double guest;
        double guest_nice;
    };

    struct _Context : SystemPerformance::Support::Context {
        optional<POSIXSysTimeCaptureContext_> fLastSysTimeCapture;
    };

    struct InstrumentRep_Linux_ : InstrumentRepBase_<_Context> {

        using InstrumentRepBase_<_Context>::InstrumentRepBase_;

        /*
         *  /proc/stat
         *      EXAMPLE:
         *          lewis@LewisLinuxDevVM2:~$ cat /proc/stat
         *          cpu  361378 1170 50632 2812384 5609 3 2684 0 0 0
         *          cpu0 202907 894 27261 1382587 3014 3 2672 0 0 0
         *          cpu1 158471 276 23371 1429797 2595 0 12 0 0 0
         *
         *  From http://man7.org/linux/man-pages/man5/proc.5.html
         *
         *        kernel/system statistics.  Varies with architecture.  Common
         *         entries include:
         *
         *         cpu  3357 0 4313 1362393
         *              The amount of time, measured in units of USER_HZ
         *              (1/100ths of a second on most architectures, use
         *              sysconf(_SC_CLK_TCK) to obtain the right value), that
         *              the system spent in various states:
         *
         *              user   (1) Time spent in user mode.
         *
         *              nice   (2) Time spent in user mode with low priority
         *                       (nice).
         *
         *              system (3) Time spent in system mode.
         *
         *              idle   (4) Time spent in the idle task.  This value
         *                       should be USER_HZ times the second entry in the
         *                       /proc/uptime pseudo-file.
         *
         *              iowait (since Linux 2.5.41)
         *                   (5) Time waiting for I/O to complete.
         *
         *              irq (since Linux 2.6.0-test4)
         *                   (6) Time servicing interrupts.
         *
         *              softirq (since Linux 2.6.0-test4)
         *                       (7) Time servicing softirqs.
         *
         *              steal (since Linux 2.6.11)
         *                            (8) Stolen time, which is the time spent in
         *                           other operating systems when running in a
         *                           virtualized environment
         *
         *              guest (since Linux 2.6.24)
         *                            (9) Time spent running a virtual CPU for guest
         *                            operating systems under the control of the Linux
         *                           kernel.
         *
         *              guest_nice (since Linux 2.6.33)
         *                            (10) Time spent running a niced guest (virtual
         *                            CPU for guest operating systems under the
         *                             control of the Linux kernel).
         *
         */
        static POSIXSysTimeCaptureContext_ GetSysTimes_ ()
        {
            POSIXSysTimeCaptureContext_ result{};
            using Characters::FloatConversion::ToFloat;
            DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
            static const filesystem::path                          kFileName_{"/proc/stat"sv};
            // Note - /procfs files always unseekable
            for (const Sequence<String>& line :
                 reader.ReadMatrix (IO::FileSystem::FileInputStream::New (kFileName_, IO::FileSystem::FileInputStream::eNotSeekable))) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"in Instruments::CPU::capture_GetSysTimes_ linesize=%d, line[0]=%s", line.size (), line.empty () ? L"" : line[0].c_str ());
#endif
                size_t sz = line.size ();
                if (sz >= 5 and line[0] == "cpu"sv) {
                    result.user   = ToFloat<double> (line[1]);
                    result.nice   = ToFloat<double> (line[2]);
                    result.system = ToFloat<double> (line[3]);
                    result.idle   = ToFloat<double> (line[4]);
                    if (sz >= 6) {
                        result.iowait = ToFloat<double> (line[5]);
                    }
                    if (sz >= 7) {
                        result.irq = ToFloat<double> (line[6]);
                    }
                    if (sz >= 8) {
                        result.softirq = ToFloat<double> (line[7]);
                    }
                    if (sz >= 9) {
                        result.steal = ToFloat<double> (line[8]);
                    }
                    break; // once found no need to read the rest...
                }
            }
            return result;
        }
        struct CPUUsageTimes_ {
            double fProcessCPUUsage;
            double fTotalCPUUsage;
        };
        nonvirtual Info _InternalCapture ()
        {
            Info result;
#if qSupport_SystemPerformance_Instruments_CPU_LoadAverage
            {
                double loadAve[3];
                int    lr = ::getloadavg (loadAve, NEltsOf (loadAve));
                if (lr == 3) {
                    result.fLoadAverage = Info::LoadAverage (loadAve[0], loadAve[1], loadAve[2]);
                    auto tcNow          = Time::GetTickCount ();
                    result.fRunQLength  = EstimateRunQFromLoadAveArray_ ((tcNow - _GetCaptureContextTime ()).count (), loadAve);
                    Memory::AccumulateIf<double> (&result.fRunQLength, Configuration::GetNumberOfLogicalCPUCores (),
                                                  std::divides{}); // fRunQLength counts length normalized 0..1 with 1 menaing ALL CPU CORES
                }
                else {
                    DbgTrace ("getloadave failed - with result = {}"_f, lr);
                }
            }
#endif
            auto getCPUTime = [&] (POSIXSysTimeCaptureContext_* referenceValue) -> optional<CPUUsageTimes_> {
                POSIXSysTimeCaptureContext_ newVal = GetSysTimes_ ();
                *referenceValue                    = newVal;
                if (auto baseline = _fContext.load ()->fLastSysTimeCapture) {
                    // from http://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
                    //      Idle=idle+iowait
                    //      NonIdle=user+nice+system+irq+softirq+steal
                    //      Total=Idle+NonIdle # first line of file for all cpus
                    double idleTime = 0;
                    idleTime += (newVal.idle - baseline->idle);
                    idleTime += (newVal.iowait - baseline->iowait);

                    double processNonIdleTime = 0;
                    processNonIdleTime += (newVal.user - baseline->user);
                    processNonIdleTime += (newVal.nice - baseline->nice);
                    processNonIdleTime += (newVal.system - baseline->system);

                    double nonIdleTime = processNonIdleTime;
                    nonIdleTime += (newVal.irq - baseline->irq);
                    nonIdleTime += (newVal.softirq - baseline->softirq);

                    double totalTime = idleTime + nonIdleTime;
                    if (totalTime <= this->_fOptions.fMinimumAveragingInterval.count ()) {
                        // can happen if called too quickly together. No good answer
                        DbgTrace ("Warning - times too close together for cputime"_f);
                        return nullopt;
                    }
                    Assert (totalTime > 0);
                    double totalProcessCPUUsage = processNonIdleTime / totalTime;
                    double totalCPUUsage        = nonIdleTime / totalTime;
                    return CPUUsageTimes_{totalProcessCPUUsage, totalCPUUsage};
                }
                return nullopt;
            };
            POSIXSysTimeCaptureContext_ referenceValue{};
            if (auto tmp = getCPUTime (&referenceValue)) {
                unsigned int nLogicalCores   = Configuration::GetNumberOfLogicalCPUCores ();
                result.fTotalProcessCPUUsage = tmp->fProcessCPUUsage * nLogicalCores;
                result.fTotalCPUUsage        = tmp->fTotalCPUUsage * nLogicalCores;
                result.fTotalLogicalCores    = nLogicalCores;
            }
            _NoteCompletedCapture ();
            _fContext.rwget ().rwref ()->fLastSysTimeCapture = referenceValue;
            return result;
        }
    };
}
#endif

#if qPlatform_Windows
namespace {
    struct WinSysTimeCaptureContext_ {
        double IdleTime;
        double KernelTime;
        double UserTime;
    };
    struct _Context : SystemPerformance::Support::Context {
        optional<WinSysTimeCaptureContext_> fLastSysTimeCapture{};
#if qUseWMICollectionSupport_
        WMICollector fSystemWMICollector_{"System"sv, {kInstanceName_}, {kProcessorQueueLength_}};
#endif
    };

    struct InstrumentRep_Windows_ : InstrumentRepBase_<_Context> {

        using InstrumentRepBase_<_Context>::InstrumentRepBase_;

        nonvirtual Info _InternalCapture ()
        {
            auto getCPUTime = [=] (WinSysTimeCaptureContext_* newRawValueToStoreAsNextbaseline) -> optional<double> {
                RequireNotNull (newRawValueToStoreAsNextbaseline);
                auto getSysTimes = [] () -> WinSysTimeCaptureContext_ {
                    auto getAsSeconds = [] (const ::FILETIME& ft) {
                        ::ULARGE_INTEGER ui;
                        ui.LowPart  = ft.dwLowDateTime;
                        ui.HighPart = ft.dwHighDateTime;
                        return static_cast<double> (ui.QuadPart) / 10000000; // convert from 100-nanosecond units
                    };
                    ::FILETIME curIdleTime_{};
                    ::FILETIME curKernelTime_{};
                    ::FILETIME curUserTime_{};
                    Verify (::GetSystemTimes (&curIdleTime_, &curKernelTime_, &curUserTime_));
                    return WinSysTimeCaptureContext_{getAsSeconds (curIdleTime_), getAsSeconds (curKernelTime_), getAsSeconds (curUserTime_)};
                };

                WinSysTimeCaptureContext_ newVal  = getSysTimes ();
                *newRawValueToStoreAsNextbaseline = newVal;
                if (_fContext.load ()->fLastSysTimeCapture) {
                    WinSysTimeCaptureContext_ baseline               = *_fContext.load ()->fLastSysTimeCapture;
                    double                    idleTimeOverInterval   = newVal.IdleTime - baseline.IdleTime;
                    double                    kernelTimeOverInterval = newVal.KernelTime - baseline.KernelTime;
                    double                    userTimeOverInterval   = newVal.UserTime - baseline.UserTime;
                    /*
                     *  This logic seems queer (sys = kern + user, and why doesnt denominator include idletime?), but is cribbed from
                     *      http://en.literateprograms.org/CPU_usage_%28C,_Windows_XP%29
                     *      http://www.codeproject.com/Articles/9113/Get-CPU-Usage-with-GetSystemTimes
                     *  Must be that idle time is somehow INCLUDED in either kernel or user time.
                     */
                    double sys = kernelTimeOverInterval + userTimeOverInterval;
                    if (Time::Duration{sys} > _fOptions.fMinimumAveragingInterval) {
                        double cpu = 1 - idleTimeOverInterval / sys;
                        return cpu * ::GetNumberOfLogicalCPUCores ();
                    }
                }
                return nullopt;
            };

            Info                      result;
            WinSysTimeCaptureContext_ newRawValueToStoreAsNextbaseline;
            result.fTotalCPUUsage = getCPUTime (&newRawValueToStoreAsNextbaseline);
            result.fTotalProcessCPUUsage = result.fTotalCPUUsage; // @todo fix - WMI - remove irq time etc from above? Or add into above if missing (See counter PRocessor/% Interrupt time) - not from System - but Processor - so new collector object
            result.fTotalLogicalCores = Configuration::GetNumberOfLogicalCPUCores ();
#if qUseWMICollectionSupport_
            _fContext.rwget ().rwref ()->fSystemWMICollector_.Collect ();
            Memory::CopyToIf (&result.fRunQLength,
                              _fContext.rwget ().rwref ()->fSystemWMICollector_.PeekCurrentValue (kInstanceName_, kProcessorQueueLength_));
            // "if a computer has multiple processors, you need to divide this value by the number of processors servicing the workload"
            Memory::AccumulateIf<double> (&result.fRunQLength, Configuration::GetNumberOfLogicalCPUCores (), std::divides{}); // both normalized so '1' means all logical cores
#endif
            _NoteCompletedCapture ();
            _fContext.rwget ().rwref ()->fLastSysTimeCapture = newRawValueToStoreAsNextbaseline;
            return result;
        }
    };
}
#endif

namespace {
    const MeasurementType kCPUMeasurment_ = MeasurementType{"CPU-Usage"sv};
}

namespace {
    struct CPUInstrumentRep_
#if qPlatform_Linux
        : InstrumentRep_Linux_
#elif qPlatform_Windows
        : InstrumentRep_Windows_
#else
        : InstrumentRepBase_<SystemPerformance::Support::Context>
#endif
    {
#if qPlatform_Linux
        using inherited = InstrumentRep_Linux_;
#elif qPlatform_Windows
        using inherited = InstrumentRep_Windows_;
#else
        using inherited = InstrumentRepBase_<SystemPerformance::Support::Context>;
#endif
        CPUInstrumentRep_ (const Options& options, const shared_ptr<_Context>& context = make_shared<_Context> ())
            : inherited{options, context}
        {
            Require (_fOptions.fMinimumAveragingInterval > 0s);
        }
        virtual MeasurementSet Capture () override
        {
            Debug::TraceContextBumper ctx{"SystemPerformance::Instrument...CPU...CPUInstrumentRep_::Capture ()"};
            MeasurementSet            results;
            results.fMeasurements.Add (Measurement{
                kCPUMeasurment_, Instruments::CPU::Instrument::kObjectVariantMapper.FromObject (Capture_Raw (&results.fMeasuredAt))});
            return results;
        }
        nonvirtual Info Capture_Raw (Range<TimePointSeconds>* outMeasuredAt)
        {
            return Do_Capture_Raw<Info> ([this] () { return _InternalCapture (); }, outMeasuredAt);
        }
        virtual unique_ptr<IRep> Clone () const override
        {
            return make_unique<CPUInstrumentRep_> (_fOptions, _fContext.load ());
        }
        nonvirtual Info _InternalCapture ()
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"Instruments::CPU::{}CPUInstrumentRep_::_InternalCapture"};
#endif
#if qPlatform_Linux or qPlatform_Windows
            Info result = inherited::_InternalCapture ();
#else
            Info result;
#endif
            return result;
        }
    };
}

/*
 ********************************************************************************
 ************************ Instruments::CPU::Instrument **************************
 ********************************************************************************
 */
const ObjectVariantMapper Instruments::CPU::Instrument::kObjectVariantMapper = [] () -> ObjectVariantMapper {
    using StructFieldInfo = ObjectVariantMapper::StructFieldInfo;
    ObjectVariantMapper mapper;
#if qSupport_SystemPerformance_Instruments_CPU_LoadAverage
    mapper.AddClass<Info::LoadAverage> ({
        {"1-minute"_k, StructFieldMetaInfo{&Info::LoadAverage::f1MinuteAve}},
        {"5-minute"_k, StructFieldMetaInfo{&Info::LoadAverage::f5MinuteAve}},
        {"15-minute"_k, StructFieldMetaInfo{&Info::LoadAverage::f15MinuteAve}},
    });
    mapper.AddCommonType<optional<Info::LoadAverage>> ();
#endif
    mapper.AddClass<Info> ({
#if qSupport_SystemPerformance_Instruments_CPU_LoadAverage
        {"Load-Average"_k, StructFieldMetaInfo{&Info::fLoadAverage}, StructFieldInfo::eOmitNullFields},
#endif
            {"Total-Logical-Cores"_k, StructFieldMetaInfo{&Info::fTotalLogicalCores}, StructFieldInfo::eOmitNullFields},
            {"Total-Process-CPU-Usage"_k, StructFieldMetaInfo{&Info::fTotalProcessCPUUsage}, StructFieldInfo::eOmitNullFields},
            {"Total-CPU-Usage"_k, StructFieldMetaInfo{&Info::fTotalCPUUsage}, StructFieldInfo::eOmitNullFields},
            {"Run-Q-Length"_k, StructFieldMetaInfo{&Info::fRunQLength}, StructFieldInfo::eOmitNullFields},
    });
    return mapper;
}();

Instruments::CPU::Instrument::Instrument (const Options& options)
    : SystemPerformance::Instrument{InstrumentNameType{"CPU"_k},
                                    make_unique<CPUInstrumentRep_> (options),
                                    {kCPUMeasurment_},
                                    {KeyValuePair<type_index, MeasurementType>{typeid (Info), kCPUMeasurment_}},
                                    kObjectVariantMapper}
{
}

/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template <>
Instruments::CPU::Info SystemPerformance::Instrument::CaptureOneMeasurement (Range<TimePointSeconds>* measurementTimeOut)
{
    Debug::TraceContextBumper ctx{"SystemPerformance::Instrument::CaptureOneMeasurement<CPU::Info>"};
    CPUInstrumentRep_*        myCap = dynamic_cast<CPUInstrumentRep_*> (fCaptureRep_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}
