/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
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










DurationSecondsType Stroika::Foundation::Time::GetTickCount ()
{
#if     qPlatform_Windows
    static  bool            sInited =   false;
    static  LARGE_INTEGER   sPerformanceFrequency;
    if (not sInited) {
        if (not ::QueryPerformanceFrequency (&sPerformanceFrequency)) {
            sPerformanceFrequency.QuadPart = 0;
        }
        sInited = true;
    }
    if (sPerformanceFrequency.QuadPart == 0) {
        return (float (::GetTickCount ()) / 1000.0f);
    }
    else {
        LARGE_INTEGER   counter;
        counter.QuadPart = 0;
        Verify (::QueryPerformanceCounter (&counter));
        return static_cast<DurationSecondsType> (static_cast<double> (counter.QuadPart) / static_cast<double> (sPerformanceFrequency.QuadPart));
    }
#elif   qPlatform_POSIX
    timespec ts;
    Verify (clock_gettime (CLOCK_REALTIME, &ts) == 0);
    return ts.tv_sec + DurationSecondsType (ts.tv_nsec) / (1000.0 * 1000.0 * 1000.0);
#else
    return time (0);    //tmphack... not good but better than assert erorr
#endif
}


