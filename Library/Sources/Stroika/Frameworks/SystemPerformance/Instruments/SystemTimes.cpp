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

#include    "../CommonMeasurementTypes.h"

#include    "SystemTimes.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;




#if     qSupport_SystemPerformance_Instruments_SystemTimes
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
}
#endif




/*
 ********************************************************************************
 *************** Instruments::SystemTimes::GetInstrument ************************
 ********************************************************************************
 */
#if     qSupport_SystemPerformance_Instruments_SystemTimes
Instrument  SystemPerformance::Instruments::SystemTimes::GetInstrument (Time::DurationSecondsType measureInterval)
{
    // NB: Cannot cache systemTimesInstrument since takes capture argument
    Instrument  systemTimesInstrument    = Instrument (
            InstrumentNameType (String_Constant (L"System-Times")),
    [measureInterval] () -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = DateTime::Now ();

        double  baselineIdleTime = 0;
        double  baselineKernelTime = 0;
        double  baselineUserTime = 0;
        GetSysTimes_ (&baselineIdleTime, &baselineKernelTime, &baselineUserTime);

        Execution::Sleep (measureInterval);

        double  secondIdleTime = 0;
        double  secondKernelTime = 0;
        double  secondUserTime = 0;
        GetSysTimes_ (&secondIdleTime, &secondKernelTime, &secondUserTime);

        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());

        double  idleTimeOverInterval = secondIdleTime - baselineIdleTime;
        double  kernelTimeOverInterval = secondKernelTime - baselineKernelTime;
        double  userTimeOverInterval = secondUserTime - baselineUserTime;

        double sys = kernelTimeOverInterval + userTimeOverInterval;
        Assert (sys > 0);
        double cpu =  (sys - idleTimeOverInterval) * 100 / sys;

        Measurement m;
        m.fValue = cpu;
        m.fType = kPercentCPUUsage;
        results.fMeasurements.Add (m);
        return results;
    },
    {kPercentCPUUsage},
    DataExchange::ObjectVariantMapper ()
                                           );
    return systemTimesInstrument;
}
#endif
