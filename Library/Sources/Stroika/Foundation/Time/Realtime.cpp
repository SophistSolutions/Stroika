/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>

#if     qPlatform_Windows
#include    <Windows.h>
#elif   qPlatform_POSIX
#include    <time.h>
#endif

#include    "../Debug/Assertions.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "Realtime.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Time;










/*
 ********************************************************************************
 ************************* Time::GetTickCount ***********************************
 ********************************************************************************
 */
DurationSecondsType Stroika::Foundation::Time::GetTickCount ()
{
#if     qPlatform_MacOS
    return (float (::TickCount ()) / 60.0f);
#elif   qPlatform_POSIX
    timespec ts;
    Verify (clock_gettime (CLOCK_MONOTONIC, &ts) == 0);
    return ts.tv_sec + DurationSecondsType (ts.tv_nsec) / (1000.0 * 1000.0 * 1000.0);
#elif   qPlatform_Windows
    static  bool            sInited =   false;
    static  LARGE_INTEGER   sPerformanceFrequency;
    if (not sInited) {
        if (not ::QueryPerformanceFrequency (&sPerformanceFrequency)) {
            sPerformanceFrequency.QuadPart = 0;
        }
        sInited = true;
    }
    if (sPerformanceFrequency.QuadPart == 0) {
#if     (_WIN32_WINNT >= 0x0600)
        return (float (::GetTickCount64 ()) / 1000.0f);
#else
        DISABLE_COMPILER_MSC_WARNING_START(28159)
        return (float (::GetTickCount ()) / 1000.0f);
        DISABLE_COMPILER_MSC_WARNING_END(28159)
#endif
    }
    else {
        LARGE_INTEGER   counter;
        counter.QuadPart = 0;
        Verify (::QueryPerformanceCounter (&counter));
        return static_cast<DurationSecondsType> (static_cast<double> (counter.QuadPart) / static_cast<double> (sPerformanceFrequency.QuadPart));
    }
#else
    return time (0);    //tmphack... not good but better than assert erorr
#endif
}


