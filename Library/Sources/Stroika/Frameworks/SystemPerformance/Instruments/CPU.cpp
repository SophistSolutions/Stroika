/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_AIX
#include    <libperfstat.h>
#elif   qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../../../Foundation/Characters/FloatConversion.h"
#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/DataExchange/CharacterDelimitedLines/Reader.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Execution/ErrNoException.h"
#include    "../../../Foundation/Execution/ProcessRunner.h"
#include    "../../../Foundation/Execution/Sleep.h"
#include    "../../../Foundation/IO/FileSystem/FileInputStream.h"
#include    "../../../Foundation/Math/Common.h"
#include    "../../../Foundation/Streams/MemoryStream.h"
#include    "../../../Foundation/Streams/TextReader.h"

#include    "CPU.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::CPU;

using   Characters::String_Constant;
using   Time::DurationSecondsType;







// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1






#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_       qPlatform_Windows
#endif


#if     qUseWMICollectionSupport_
#include    "../Support/WMICollector.h"

using   SystemPerformance::Support::WMICollector;
#endif




#if     qUseWMICollectionSupport_
namespace {
    const   String_Constant     kInstanceName_          { L"" };

    const   String_Constant     kProcessorQueueLength_  { L"Processor Queue Length" };
}
#endif










#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
/*
 ********************************************************************************
 *********************** Instruments::CPU::Info::LoadAverage ********************
 ********************************************************************************
 */
Instruments::CPU::Info::LoadAverage::LoadAverage (double oneMinuteAve, double fiveMinuteAve, double fifteenMinuteAve)
    : f1MinuteAve (oneMinuteAve)
    , f5MinuteAve (fiveMinuteAve)
    , f15MinuteAve (fifteenMinuteAve)
{
}
#endif






/*
 ********************************************************************************
 ***************** Instruments::CPU::GetObjectVariantMapper *********************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::CPU::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    static  const   ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
        mapper.AddClass<Info::LoadAverage> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::LoadAverage, f1MinuteAve), String_Constant (L"1-minute") },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::LoadAverage, f5MinuteAve), String_Constant (L"5-minute") },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info::LoadAverage, f15MinuteAve), String_Constant (L"15-minute") },
        });
        mapper.AddCommonType<Optional_Indirect_Storage<Info::LoadAverage>> ();
#endif
        mapper.AddCommonType<Optional<double>> ();
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fLoadAverage), String_Constant (L"Load-Average"), StructureFieldInfo::NullFieldHandling::eOmit },
#endif
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fTotalProcessCPUUsage), String_Constant (L"Total-Process-CPU-Usage") },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fTotalCPUUsage), String_Constant (L"Total-CPU-Usage") },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fRunQLength), String_Constant (L"Run-Q-Length"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        return mapper;
    } ();
    return sMapper_;
}






namespace {
    struct  CapturerWithContext_COMMON_ {
        Options                     fOptions_;
        DurationSecondsType         fPostponeCaptureUntil_ { 0 };
        DurationSecondsType         fLastCapturedAt {};
        DurationSecondsType         fMinimumAveragingInterval_;
        CapturerWithContext_COMMON_ (const Options& options)
            : fOptions_ (options)
            , fMinimumAveragingInterval_ (options.fMinimumAveragingInterval)
        {
        }
        DurationSecondsType    GetLastCaptureAt () const { return fLastCapturedAt; }
        void    NoteCompletedCapture_ ()
        {
            auto now = Time::GetTickCount ();
            fPostponeCaptureUntil_ = now + fMinimumAveragingInterval_;
            fLastCapturedAt = now;
        }
    };
}




#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
namespace {
    template    <typename ELT>
    double  EstimateRunQFromLoadAveArray_ (Time::DurationSecondsType backNSeconds, ELT loadAveArray[3])
    {
        // NB: Currently this is TOO simple. We should probably fit a curve to 3 points and use that to extrapolate. Maybe just fit 4 line segments?
        Require (backNSeconds >= 0);
        double  backNMinutes    =   backNSeconds / 60.0;
        if (backNMinutes <= 1) {
            return static_cast<double> (loadAveArray[0]);
        }
        else if (backNMinutes <= 5) {
            double  distFrom1 = (backNMinutes - 1);
            double  distFrom5 = (5.0 - backNMinutes);
            return static_cast<double> (loadAveArray[0]) * (1.0 - distFrom1 / 4) + static_cast<double> (loadAveArray[1]) * (1.0 - distFrom5 / 4);
        }
        else if (backNMinutes <= 15) {
            double  distFrom5 = (backNMinutes - 5);
            double  distFrom15 = (15.0 - backNMinutes);
            return static_cast<double> (loadAveArray[1]) * (1.0 - distFrom5 / 10) + static_cast<double> (loadAveArray[2]) * (1.0 - distFrom15 / 10);
        }
        else {
            return static_cast<double> (loadAveArray[2]);
        }
    }
}
#endif


#if     qPlatform_AIX
namespace {
    struct  CapturerWithContext_AIX_ : CapturerWithContext_COMMON_ {
        CapturerWithContext_AIX_ (const Options& options)
            : CapturerWithContext_COMMON_ (options)
        {
            // Force fill of context - ignore results
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
        }
        struct  CPUUsageTimes_ {
            double              fProcessCPUUsage;
            double              fTotalCPUUsage;
            double              fRunQLength;
#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
            Info::LoadAverage   fLoadAverage;
#endif
        };
        CPUUsageTimes_  cputime_ ()
        {
            return cputime_perfstat_ ();
        }
        Optional<perfstat_cpu_total_t>  fPrev;
        CPUUsageTimes_  cputime_perfstat_ ()
        {
            Debug::TraceContextBumper ctx ("{}::CapturerWithContext_AIX_::cputime_perfstat_");
            perfstat_cpu_total_t    tmp;
            Execution::ThrowErrNoIfNegative (::perfstat_cpu_total (nullptr, &tmp, sizeof(tmp), 1));
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("tmp.user=%lld, tmp.sys=%lld, tmp.idle=%lld, tmp.wait=%lld", tmp.user, tmp.sys, tmp.idle, tmp.wait);
#endif
            CPUUsageTimes_  result {};
            if (fPrev) {
                u_longlong_t    total {};
                total += tmp.user - fPrev->user;
                total += tmp.sys - fPrev->sys;
                total += tmp.idle - fPrev->idle;
                total += tmp.wait - fPrev->wait;

                u_longlong_t    pcpuNumerator {};
                pcpuNumerator += tmp.user - fPrev->user;
                pcpuNumerator += tmp.sys - fPrev->sys;

                u_longlong_t    idleNumerator {};
                idleNumerator += tmp.idle - fPrev->idle;

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("lbolt=%lld", tmp.lbolt);
                DbgTrace ("runocc=%lld", tmp.runocc);
                DbgTrace ("loadavg[0]=%f, loadavg[1]=%f, loadavg[2]=%f", static_cast<double> (tmp.loadavg[0]) / (1 << SBITS), static_cast<double> (tmp.loadavg[1]) / (1 << SBITS), static_cast<double> (tmp.loadavg[2]) / (1 << SBITS));
#endif
                double  runQLength; // intentionally uninitialized
                Info::LoadAverage   loadAverage;
                {
                    double loadAve[3] = { static_cast<double> (tmp.loadavg[0]) / (1 << SBITS), static_cast<double> (tmp.loadavg[1]) / (1 << SBITS), static_cast<double> (tmp.loadavg[2]) / (1 << SBITS) };
#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
                    loadAverage  = Info::LoadAverage (loadAve[0], loadAve[1], loadAve[2]);
#endif
                    runQLength = EstimateRunQFromLoadAveArray_ (Time::GetTickCount () - GetLastCaptureAt () , loadAve);
                }

                result = CPUUsageTimes_ {
                    static_cast<double> (pcpuNumerator) / static_cast<double> (total),
                    1.0 - static_cast<double> (idleNumerator) / static_cast<double> (total),
                    runQLength
#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
                    , loadAverage
#endif
                };

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("fPrev.user=%lld, fPrev.sys=%lld, fPrev.idle=%lld, fPrev.wait=%lld", fPrev->user, fPrev->sys, fPrev->idle, fPrev->wait);
                DbgTrace ("total = %lld", total);
                DbgTrace ("100.0 * (double) DELTA(user) /  total = %f", (100.0 * (double) (tmp.user - fPrev->user) / total));
                DbgTrace ("100.0 * (double) DELTA(sys) /  total = %f", (100.0 * (double) (tmp.sys - fPrev->sys) / total));
                DbgTrace ("100.0 * (double) DELTA(idle) /  total = %f", (100.0 * (double) (tmp.idle - fPrev->idle) / total));
                DbgTrace ("100.0 * (double) DELTA(wait) /  total = %f", (100.0 * (double) (tmp.wait - fPrev->wait) / total));
#endif
            }
            fPrev = tmp;
            return result;
        }
        Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Info capture_ ()
        {
            Info    result;
            auto tmp = cputime_ ();
            result.fTotalProcessCPUUsage = tmp.fProcessCPUUsage;
            result.fTotalCPUUsage = tmp.fTotalCPUUsage;
            result.fRunQLength = tmp.fRunQLength;
#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
            result.fLoadAverage = tmp.fLoadAverage;
#endif
            NoteCompletedCapture_ ();
            return result;
        }
    };
}
#endif






#if   qPlatform_Linux
namespace {
    struct  CapturerWithContext_Linux_ : CapturerWithContext_COMMON_ {
        struct  POSIXSysTimeCaptureContext_ {
            double  user;
            double  nice;
            double  system;
            double  idle;
            double  iowait;
            double  irq;
            double  softirq;
            double  steal;
            double  guest;
            double  guest_nice;
        };
        POSIXSysTimeCaptureContext_     fContext_ {};
        CapturerWithContext_Linux_ (const Options& options)
            : CapturerWithContext_COMMON_ (options)
        {
            // Force fill of context - ignore results
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
        }
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
        static  inline  POSIXSysTimeCaptureContext_    GetSysTimes_ ()
        {
            POSIXSysTimeCaptureContext_   result;
            using   IO::FileSystem::FileInputStream;
            using   Characters::String2Float;
            DataExchange::CharacterDelimitedLines::Reader reader {{' ', '\t' }};
            const   String_Constant kFileName_ { L"/proc/stat" };
            // Note - /procfs files always unseekable
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kFileName_, FileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::CPU::capture_GetSysTimes_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                size_t  sz = line.size ();
                if (sz >= 5 and line[0] == L"cpu") {
                    result.user = String2Float<double> (line[1]);
                    result.nice = String2Float<double> (line[2]);
                    result.system = String2Float<double> (line[3]);
                    result.idle = String2Float<double> (line[4]);
                    if (sz >= 6) {
                        result.iowait = String2Float<double> (line[5]);
                    }
                    if (sz >= 7) {
                        result.irq = String2Float<double> (line[6]);
                    }
                    if (sz >= 8) {
                        result.softirq = String2Float<double> (line[7]);
                    }
                    if (sz >= 9) {
                        result.steal = String2Float<double> (line[8]);
                    }
                    break;  // once found no need to read the rest...
                }
            }
            return result;
        }
        struct  CPUUsageTimes_ {
            double  fProcessCPUUsage;
            double  fTotalCPUUsage;
        };
        CPUUsageTimes_  cputime_ ()
        {
            POSIXSysTimeCaptureContext_   baseline = fContext_;
            POSIXSysTimeCaptureContext_   newVal = GetSysTimes_ ();
            fContext_ = newVal;

            // from http://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
            //      Idle=idle+iowait
            //      NonIdle=user+nice+system+irq+softirq+steal
            //      Total=Idle+NonIdle # first line of file for all cpus

            double  idleTime = 0;
            idleTime += (newVal.idle - baseline.idle);
            idleTime += (newVal.iowait - baseline.iowait);

            double  processNonIdleTime = 0;
            processNonIdleTime += (newVal.user - baseline.user);
            processNonIdleTime += (newVal.nice - baseline.nice);
            processNonIdleTime += (newVal.system - baseline.system);

            double  nonIdleTime = processNonIdleTime;
            nonIdleTime += (newVal.irq - baseline.irq);
            nonIdleTime += (newVal.softirq - baseline.softirq);

            double  totalTime = idleTime + nonIdleTime;
            if (Math::NearlyEquals<double> (totalTime, 0)) {
                // can happen if called too quickly together. No good answer
                DbgTrace ("Warning - times too close together for cputime_");
                return CPUUsageTimes_ {};
            }
            Assert (totalTime > 0);
            double totalProcessCPUUsage =  processNonIdleTime / totalTime;
            double totalCPUUsage =  nonIdleTime / totalTime;
            return CPUUsageTimes_ { totalProcessCPUUsage, totalCPUUsage };
        }
        Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Info capture_ ()
        {
            Info    result;
#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
            {
                double loadAve[3];
                int lr = ::getloadavg (loadAve, NEltsOf (loadAve));
                if (lr == 3) {
                    result.fLoadAverage = Info::LoadAverage (loadAve[0], loadAve[1], loadAve[2]);
                    result.fRunQLength = EstimateRunQFromLoadAveArray_ (Time::GetTickCount () - GetLastCaptureAt () , loadAve);
                }
                else {
                    DbgTrace ("getloadave failed - with result = %d", lr);
                }
            }
#endif
            auto tmp = cputime_ ();
            result.fTotalProcessCPUUsage = tmp.fProcessCPUUsage;
            result.fTotalCPUUsage = tmp.fTotalCPUUsage;
            NoteCompletedCapture_ ();
            return result;
        }
    };
}
#endif











#if   qPlatform_Windows
namespace {
    struct  CapturerWithContext_Windows_ : CapturerWithContext_COMMON_ {
#if     qUseWMICollectionSupport_
        WMICollector            fSystemWMICollector_ { String_Constant { L"System" }, {kInstanceName_},  {kProcessorQueueLength_} };
#endif
        struct  WinSysTimeCaptureContext_ {
            double  IdleTime;
            double  KernelTime;
            double  UserTime;
        };
        WinSysTimeCaptureContext_   fContext_;
        CapturerWithContext_Windows_ (const Options& options)
            : CapturerWithContext_COMMON_ (options)
        {
            capture_ ();    // Force fill of context
        }
        static  inline  double  GetAsSeconds_ (FILETIME ft)
        {
            ULARGE_INTEGER  ui;
            ui.LowPart = ft.dwLowDateTime;
            ui.HighPart = ft.dwHighDateTime;
            // convert from 100-nanosecond units
            return static_cast<double> (ui.QuadPart) / 10000000;
        }
        static  inline  WinSysTimeCaptureContext_    GetSysTimes_ ()
        {
            FILETIME    curIdleTime_ {};
            FILETIME    curKernelTime_ {};
            FILETIME    curUserTime_ {};
            Verify (::GetSystemTimes (&curIdleTime_, &curKernelTime_, &curUserTime_));
            return WinSysTimeCaptureContext_ { GetAsSeconds_ (curIdleTime_), GetAsSeconds_ (curKernelTime_), GetAsSeconds_ (curUserTime_) };
        }
        double  cputime_ ()
        {
            /*
             *  This logic seems queer (sys = kern + user, and why isnt numerator userTime?), but is cribbed from
             *      http://en.literateprograms.org/CPU_usage_%28C,_Windows_XP%29
             *      http://www.codeproject.com/Articles/9113/Get-CPU-Usage-with-GetSystemTimes
             */
            WinSysTimeCaptureContext_   baseline = fContext_;
            WinSysTimeCaptureContext_   newVal = GetSysTimes_ ();
            fContext_ = newVal;

            double  idleTimeOverInterval = newVal.IdleTime - baseline.IdleTime;
            double  kernelTimeOverInterval = newVal.KernelTime - baseline.KernelTime;
            double  userTimeOverInterval = newVal.UserTime - baseline.UserTime;

            double sys = kernelTimeOverInterval + userTimeOverInterval;
            Assert (sys > 0);
            double cpu =  (sys - idleTimeOverInterval) / sys;
            return cpu;
        }
        Info capture_ ()
        {
            Info    result;
            result.fTotalCPUUsage = cputime_ ();
            result.fTotalProcessCPUUsage = result.fTotalCPUUsage;   // @todo fix - remove irq time etc from above? Or add into above if missing
#if     qUseWMICollectionSupport_
            fSystemWMICollector_.Collect ();
            fSystemWMICollector_.PeekCurrentValue (kInstanceName_, kProcessorQueueLength_).CopyToIf (&result.fRunQLength);
#endif
            NoteCompletedCapture_ ();
            return result;
        }
        Info capture ()
        {
            Info    result;
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
    };
}
#endif










namespace {
    struct  CapturerWithContext_
            : Debug::AssertExternallySynchronizedLock
#if     qPlatform_AIX
            , CapturerWithContext_AIX_
#elif     qPlatform_Linux
            , CapturerWithContext_Linux_
#elif   qPlatform_Windows
            , CapturerWithContext_Windows_
#endif
    {
#if     qPlatform_AIX
        using inherited = CapturerWithContext_AIX_;
#elif     qPlatform_Linux
        using inherited = CapturerWithContext_Linux_;
#elif   qPlatform_Windows
        using inherited = CapturerWithContext_Windows_;
#endif
        CapturerWithContext_ (Options options)
            : inherited (options)
        {
        }
        Info capture ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::CPU capture");
#endif
            Info    result  =   inherited::capture ();
            // Since values externally acquired, force/assure they are all legit, in range
            result.fTotalCPUUsage = Math::PinInRange<double> (result.fTotalCPUUsage, 0, 1);
            result.fTotalProcessCPUUsage = Math::PinInRange<double> (result.fTotalProcessCPUUsage, 0, result.fTotalCPUUsage);   // all process usage is CPU usage (often same but <=)
            return result;
        }
    };
}




namespace {
    const   MeasurementType kCPUMeasurment_         =   MeasurementType { String_Constant { L"CPU-Usage" } };
}





namespace {
    class   MyCapturer_ : public Instrument::ICapturer {
        CapturerWithContext_ fCaptureContext;
    public:
        MyCapturer_ (const CapturerWithContext_& ctx)
            : fCaptureContext (ctx)
        {
        }
        virtual MeasurementSet  Capture () override
        {
            MeasurementSet  results;
            results.fMeasurements.Add (Measurement { kCPUMeasurment_, GetObjectVariantMapper ().FromObject (Capture_Raw (&results.fMeasuredAt))});
            return results;
        }
        nonvirtual Info  Capture_Raw (Range<DurationSecondsType>* outMeasuredAt)
        {
            DurationSecondsType before = fCaptureContext.GetLastCaptureAt ();
            Info                rawMeasurement = fCaptureContext.capture ();
            if (outMeasuredAt != nullptr) {
                *outMeasuredAt = Range<DurationSecondsType> (before, fCaptureContext.GetLastCaptureAt ());
            }
            return rawMeasurement;
        }
        virtual unique_ptr<ICapturer>   Clone () const override
        {
#if     qCompilerAndStdLib_make_unique_Buggy
            return unique_ptr<ICapturer> (new MyCapturer_ (fCaptureContext));
#else
            return make_unique<MyCapturer_> (fCaptureContext);
#endif
        }
    };
}








/*
 ********************************************************************************
 ************************ Instruments::CPU::GetInstrument ***********************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::CPU::GetInstrument (Options options)
{
    CapturerWithContext_ useCaptureContext { options };  // capture context so copyable in mutable lambda
    return Instrument (
               InstrumentNameType (String_Constant (L"CPU")),
#if     qCompilerAndStdLib_make_unique_Buggy
               Instrument::SharedByValueCaptureRepType (unique_ptr<MyCapturer_> (new MyCapturer_ (CapturerWithContext_ { options }))),
#else
               Instrument::SharedByValueCaptureRepType (make_unique<MyCapturer_> (CapturerWithContext_ { options })),
#endif
    { kCPUMeasurment_ },
    GetObjectVariantMapper ()
           );
}





/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template    <>
Instruments::CPU::Info   SystemPerformance::Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut)
{
    MyCapturer_*    myCap = dynamic_cast<MyCapturer_*> (fCapFun_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}

