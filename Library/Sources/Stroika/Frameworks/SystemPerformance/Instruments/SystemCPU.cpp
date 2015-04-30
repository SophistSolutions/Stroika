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

#include    "SystemCPU.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::SystemCPU;

using   Characters::String_Constant;
using   Time::DurationSecondsType;







// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1






#if     qSupport_SystemPerformance_Instruments_SystemCPU_LoadAverage
/*
 ********************************************************************************
 ********************* Instruments::SystemCPU::Info::LoadAverage ****************
 ********************************************************************************
 */
Instruments::SystemCPU::Info::LoadAverage::LoadAverage (double oneMinuteAve, double fiveMinuteAve, double fifteenMinuteAve)
    : f1MinuteAve (oneMinuteAve)
    , f5MinuteAve (fiveMinuteAve)
    , f15MinuteAve (fifteenMinuteAve)
{
}
#endif






/*
 ********************************************************************************
 ***************** Instruments::SystemCPU::GetObjectVariantMapper ***************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::SystemCPU::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
#if     qSupport_SystemPerformance_Instruments_SystemCPU_LoadAverage
        mapper.AddClass<Info::LoadAverage> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::LoadAverage, f1MinuteAve), String_Constant (L"1-minute") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::LoadAverage, f5MinuteAve), String_Constant (L"5-minute") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::LoadAverage, f15MinuteAve), String_Constant (L"15-minute") },
        });
        mapper.AddCommonType<Optional_Indirect_Storage<Info::LoadAverage>> ();
#endif
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
#if     qSupport_SystemPerformance_Instruments_SystemCPU_LoadAverage
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fLoadAverage), String_Constant (L"Load-Average"), StructureFieldInfo::NullFieldHandling::eOmit },
#endif
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fTotalCPUUsage), String_Constant (L"Total-CPU-Usage") },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        return mapper;
    } ();
    return sMapper_;
}







#if     qPlatform_POSIX
namespace {
    struct  CapturerWithContext_POSIX_ {
        struct  POSIXSysTimeCaptureContext_ {
            double  user;
            //double  nice;
            double  system;
            double  idle;
        };
        Options                         fOptions_;
        DurationSecondsType             fPostponeCaptureUntil_ { 0 };
        POSIXSysTimeCaptureContext_     fContext_ {};
        CapturerWithContext_POSIX_ (const Options& options)
            : fOptions_ (options)
        {
            capture_ ();    // Force fill of context - ignore results
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
         *                The amount of time, measured in units of USER_HZ
         *                (1/100ths of a second on most architectures, use
         *                sysconf(_SC_CLK_TCK) to obtain the right value), that
         *                the system spent in various states:
         *
         *                user   (1) Time spent in user mode.
         *
         *                nice   (2) Time spent in user mode with low priority
         *                       (nice).
         *
         *                system (3) Time spent in system mode.
         *
         *                idle   (4) Time spent in the idle task.  This value
         *                       should be USER_HZ times the second entry in the
         *                       /proc/uptime pseudo-file.
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
                DbgTrace (L"***in Instruments::SystemCPU::capture_GetSysTimes_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                if (line.size () >= 5 and line[0] == L"cpu") {
                    result.user = String2Float<double> (line[1]);
                    result.system = String2Float<double> (line[3]);
                    result.idle = String2Float<double> (line[4]);
                    break;  // once found no need to read the rest...
                }
            }
            return result;
        }
        double  cputime_ ()
        {
            POSIXSysTimeCaptureContext_   baseline = fContext_;
            POSIXSysTimeCaptureContext_   newVal = GetSysTimes_ ();
            fContext_ = newVal;

            double  usedSysTime = (newVal.user - baseline.user) + (newVal.system - baseline.system);
            double  totalTime = usedSysTime + (newVal.idle - baseline.idle);
            if (Math::NearlyEquals<double> (totalTime, 0)) {
                // can happen if called too quickly together. No good answer
                DbgTrace ("Warning - times too close together for cputime_");
                return 0;
            }
            Assert (totalTime > 0);
            double cpu =  usedSysTime * 100 / totalTime;
            return Math::PinInRange<double> (cpu, 0, 100);
        }
        Info capture_ ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            Info    result;
#if     qSupport_SystemPerformance_Instruments_SystemCPU_LoadAverage
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
            result.fTotalCPUUsage = cputime_ ();
            fPostponeCaptureUntil_ = Time::GetTickCount () + fOptions_.fMinimumAveragingInterval;
            return result;
        }
    };
}
#endif











#if   qPlatform_Windows
namespace {
    struct  CapturerWithContext_Windows_ {
        struct  WinSysTimeCaptureContext_ {
            double  IdleTime;
            double  KernelTime;
            double UserTime;
        };
        Options                     fOptions_;
        DurationSecondsType         fPostponeCaptureUntil_ { 0 };
        WinSysTimeCaptureContext_   fContext_;
        CapturerWithContext_Windows_ (const Options& options)
            : fOptions_ (options)
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
        Info capture_ ()
        {
            Info    result;
            Execution::SleepUntil (fPostponeCaptureUntil_);
            result.fTotalCPUUsage = cputime_ ();
            fPostponeCaptureUntil_ = Time::GetTickCount () + fOptions_.fMinimumAveragingInterval;
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
        Info capture_ ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::SystemCPU capture_");
#endif
            return inherited::capture_ ();
        }
    };
}








/*
 ********************************************************************************
 ******************* Instruments::SystemCPU::GetInstrument **********************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::SystemCPU::GetInstrument (Options options)
{
    CapturerWithContext_ useCaptureContext { options };  // capture context so copyable in mutable lambda
    static  const   MeasurementType kSystemCPUMeasurment_         =   MeasurementType (String_Constant (L"System-CPU-Usage"));
    return Instrument (
               InstrumentNameType (String_Constant (L"System-CPU")),
    [useCaptureContext] () mutable -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = DateTime::Now ();
        Info rawMeasurement = useCaptureContext.capture_ ();
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        Measurement m;
        m.fValue = GetObjectVariantMapper ().FromObject (rawMeasurement);
        m.fType = kSystemCPUMeasurment_;
        results.fMeasurements.Add (m);
        return results;
    },
    {kSystemCPUMeasurment_},
    GetObjectVariantMapper ()
           );
}
