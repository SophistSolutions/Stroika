/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Execution/Sleep.h"

#include    "SystemCPU.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::SystemCPU;

using   Characters::String_Constant;







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



#if     qPlatform_Windows
namespace {
    inline  double  GetAsSeconds_ (FILETIME ft)
    {
        ULARGE_INTEGER  ui;
        ui.LowPart = ft.dwLowDateTime;
        ui.HighPart = ft.dwHighDateTime;
        // convert from 100-nanosecond units
        return static_cast<double> (ui.QuadPart) / 10000000;
    }

    inline  void    GetSysTimes_ (double* idleTime, double* kernelTime, double* userTime)
    {
        RequireNotNull (idleTime);
        RequireNotNull (kernelTime);
        RequireNotNull (userTime);
        FILETIME    curIdleTime_;
        FILETIME    curKernelTime_;
        FILETIME    curUserTime_;
        memset (&curIdleTime_, 0, sizeof (curIdleTime_));
        memset (&curKernelTime_, 0, sizeof (curKernelTime_));
        memset (&curUserTime_, 0, sizeof (curUserTime_));
        Verify (::GetSystemTimes (&curIdleTime_, &curKernelTime_, &curUserTime_));
        *idleTime = GetAsSeconds_ (curIdleTime_);
        *kernelTime = GetAsSeconds_ (curKernelTime_);
        *userTime = GetAsSeconds_ (curUserTime_);
    }

    struct WinSysTimeCaptureContext_ {
        double  IdleTime {};
        double  KernelTime  {};
        double UserTime  {};
    };
    double  cputime_ (Optional<WinSysTimeCaptureContext_> prevContextObject = Optional<WinSysTimeCaptureContext_> (), WinSysTimeCaptureContext_* newContext = nullptr)
    {
        WinSysTimeCaptureContext_   baseline;
        if (prevContextObject) {
            baseline = *prevContextObject;
        }
        else {
            GetSysTimes_ (&baseline.IdleTime, &baseline.KernelTime, &baseline.UserTime);
            const Time::DurationSecondsType kUseIntervalIfNoBaseline_ { 1.0 };
            Execution::Sleep (kUseIntervalIfNoBaseline_);
        }
        WinSysTimeCaptureContext_   newVal;
        GetSysTimes_ (&newVal.IdleTime, &newVal.KernelTime, &newVal.UserTime);

        if (newContext != nullptr) {
            *newContext = newVal;
        }

        double  idleTimeOverInterval = newVal.IdleTime - baseline.IdleTime;
        double  kernelTimeOverInterval = newVal.KernelTime - baseline.KernelTime;
        double  userTimeOverInterval = newVal.UserTime - baseline.UserTime;

        double sys = kernelTimeOverInterval + userTimeOverInterval;
        Assert (sys > 0);
        double cpu =  (sys - idleTimeOverInterval) * 100 / sys;
        return cpu;
    }
}
#endif



namespace {
    Info capture_ ()
    {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx ("Instruments::SystemCPU capture_");
#endif
        Info    result;
#if     qSupport_SystemPerformance_Instruments_SystemCPU_LoadAverage
        {
            double loadAve[3];
            int result = ::getloadavg (loadAve, 3);
            if (result == 3) {
                result.fLoadAverage = Info::LoadAverage (loadAve[0], loadAve[1], loadAve[2]);
            }
            else {
                DbgTrace ("getloadave failed - with result = %d", result);
            }
        }
#endif
#if     qPlatform_POSIX
        //result.fTotalCPUUsage = cputime_ ();
#elif     qPlatform_Windows
        result.fTotalCPUUsage = cputime_ ();
#endif

        return result;
    }
}





/*
 ********************************************************************************
 ******************* Instruments::SystemCPU::GetInstrument **********************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::SystemCPU::GetInstrument ()
{
    static  const   MeasurementType kSystemCPUMeasurment_         =   MeasurementType (String_Constant (L"System-CPU-Usage"));
    static  Instrument  kInstrument_    = Instrument (
            InstrumentNameType (String_Constant (L"System-CPU")),
    [] () -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = DateTime::Now ();
        Info rawMeasurement = capture_ ();
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
    return kInstrument_;
}
