/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Float.h"
#include    "../../../Foundation/DataExchange/CharacterDelimitedLines/Reader.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Execution/Sleep.h"
#include    "../../../Foundation/IO/FileSystem/BinaryFileInputStream.h"
#include    "../../../Foundation/Math/Common.h"

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
    ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
        mapper.AddClass<Info::LoadAverage> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::LoadAverage, f1MinuteAve), String_Constant (L"1-minute") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::LoadAverage, f5MinuteAve), String_Constant (L"5-minute") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::LoadAverage, f15MinuteAve), String_Constant (L"15-minute") },
        });
        mapper.AddCommonType<Optional_Indirect_Storage<Info::LoadAverage>> ();
#endif
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fLoadAverage), String_Constant (L"Load-Average"), StructureFieldInfo::NullFieldHandling::eOmit },
#endif
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fTotalProcessCPUUsage), String_Constant (L"Total-Process-CPU-Usage") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fTotalCPUUsage), String_Constant (L"Total-CPU-Usage") },
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
        DateTime                    fLastCapturedAt;
        DurationSecondsType         fMinimumAveragingInterval_;
        CapturerWithContext_COMMON_ (const Options& options)
            : fOptions_ (options)
            , fMinimumAveragingInterval_ (options.fMinimumAveragingInterval)
        {
        }
        DateTime    GetLastCaptureAt () const { return fLastCapturedAt; }
        void    NoteCompletedCapture_ ()
        {
            fPostponeCaptureUntil_ = Time::GetTickCount () + fMinimumAveragingInterval_;
            fLastCapturedAt = DateTime::Now ();
        }
    };
}




#if     qPlatform_POSIX
namespace {
    struct  CapturerWithContext_POSIX_ : CapturerWithContext_COMMON_ {
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
        CapturerWithContext_POSIX_ (const Options& options)
            : CapturerWithContext_COMMON_ (options)
        {
            capture ();    // Force fill of context - ignore results
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
            using   IO::FileSystem::BinaryFileInputStream;
            using   Characters::String2Float;
            DataExchange::CharacterDelimitedLines::Reader reader {{' ', '\t' }};
            const   String_Constant kFileName_ { L"/proc/stat" };
            // Note - /procfs files always unseekable
            for (Sequence<String> line : reader.ReadMatrix (BinaryFileInputStream::mk (kFileName_, BinaryFileInputStream::eNotSeekable))) {
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
            double totalProcessCPUUsage =  processNonIdleTime * 100.0 / totalTime;
            double totalCPUUsage =  nonIdleTime * 100.0 / totalTime;

            return CPUUsageTimes_ { Math::PinInRange<double> (totalProcessCPUUsage, 0, 100), Math::PinInRange<double> (totalCPUUsage, 0, 100) };
        }
        Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            Info    result;
#if     qSupport_SystemPerformance_Instruments_CPU_LoadAverage
            {
                double loadAve[3];
                int lr = ::getloadavg (loadAve, 3);
                if (lr == 3) {
                    result.fLoadAverage = Info::LoadAverage (loadAve[0], loadAve[1], loadAve[2]);
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
        struct  WinSysTimeCaptureContext_ {
            double  IdleTime;
            double  KernelTime;
            double  UserTime;
        };
        WinSysTimeCaptureContext_   fContext_;
        CapturerWithContext_Windows_ (const Options& options)
            : CapturerWithContext_COMMON_ (options)
        {
            capture ();    // Force fill of context
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
            FILETIME    curIdleTime_;
            FILETIME    curKernelTime_;
            FILETIME    curUserTime_;
            memset (&curIdleTime_, 0, sizeof (curIdleTime_));
            memset (&curKernelTime_, 0, sizeof (curKernelTime_));
            memset (&curUserTime_, 0, sizeof (curUserTime_));
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
            double cpu =  (sys - idleTimeOverInterval) * 100 / sys;
            return Math::PinInRange<double> (cpu, 0, 100);
        }
        Info capture ()
        {
            Info    result;
            Execution::SleepUntil (fPostponeCaptureUntil_);
            result.fTotalCPUUsage = cputime_ ();
            result.fTotalProcessCPUUsage = result.fTotalCPUUsage;   // @todo fix - remove irq time etc from above? Or add into above if missing
            NoteCompletedCapture_ ();
            return result;
        }
    };
}
#endif










namespace {
    struct  CapturerWithContext_
            : Debug::AssertExternallySynchronizedLock
#if     qPlatform_POSIX
            , CapturerWithContext_POSIX_
#elif   qPlatform_Windows
            , CapturerWithContext_Windows_
#endif
    {
#if     qPlatform_POSIX
        using inherited = CapturerWithContext_POSIX_;
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
            return inherited::capture ();
        }
    };
}




namespace {
    const   MeasurementType kCPUMeasurment_         =   MeasurementType { String_Constant { L"CPU-Usage" } };
}





namespace {
    class   MyCapturer_ : public ICapturer {
        CapturerWithContext_ fCaptureContext;
    public:
        MyCapturer_ (const CapturerWithContext_& ctx)
            : fCaptureContext (ctx)
        {
        }
        virtual MeasurementSet  Capture ()
        {
            MeasurementSet  results;
            results.fMeasurements.Add (Measurement { kCPUMeasurment_, GetObjectVariantMapper ().FromObject (Capture_Raw (&results.fMeasuredAt))});
            return results;
        }
        nonvirtual Info  Capture_Raw (DateTimeRange* outMeasuredAt)
        {
            DateTime    before = fCaptureContext.GetLastCaptureAt ();
            Info rawMeasurement = fCaptureContext.capture ();
            if (outMeasuredAt != nullptr) {
                *outMeasuredAt = DateTimeRange (before, fCaptureContext.GetLastCaptureAt ());
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
        CapturerCallback    fCapturerCallback;
    };
}








/*
 ********************************************************************************
 ************************* Instruments::CPU::GetInstrument **********************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::CPU::GetInstrument (Options options)
{
    CapturerWithContext_ useCaptureContext { options };  // capture context so copyable in mutable lambda
    return Instrument (
               InstrumentNameType (String_Constant (L"CPU")),
#if     qCompilerAndStdLib_make_unique_Buggy
               Instrument::SharedByValueCaptureRepType (unique_ptr<ICapturer> (new MyCapturer_ (CapturerWithContext_ { options }))),
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
Instruments::CPU::Info   SystemPerformance::Instrument::CaptureOneMeasurement (DateTimeRange* measurementTimeOut)
{
    MyCapturer_*    myCap = dynamic_cast<MyCapturer_*> (fCapFun_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}

