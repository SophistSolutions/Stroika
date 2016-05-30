/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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
namespace {
    inline  DurationSecondsType GetTickCount_ () noexcept
    {
#if     qPlatform_MacOS
        return (DurationSecondsType (::TickCount ()) / 60.0);
#elif   qPlatform_POSIX
        timespec ts;
        Verify (::clock_gettime (CLOCK_MONOTONIC, &ts) == 0);
        return ts.tv_sec + DurationSecondsType (ts.tv_nsec) / (1000.0 * 1000.0 * 1000.0);
#elif   qPlatform_Windows
        static  DurationSecondsType sPerformanceFrequencyBasis_ = [] () -> DurationSecondsType {
            LARGE_INTEGER   performanceFrequency;
            if (::QueryPerformanceFrequency (&performanceFrequency) == 0)
            {
                return 0.0;
            }
            else {
                return static_cast<DurationSecondsType> (performanceFrequency.QuadPart);
            }
        } ();
        if (sPerformanceFrequencyBasis_ == 0.0) {
#if     (_WIN32_WINNT >= 0x0600)
            return (DurationSecondsType (::GetTickCount64 ()) / 1000.0);
#else
            DISABLE_COMPILER_MSC_WARNING_START(28159)
            return (DurationSecondsType (::GetTickCount ()) / 1000.0);
            DISABLE_COMPILER_MSC_WARNING_END(28159)
#endif
        }
        LARGE_INTEGER   counter;
        Verify (::QueryPerformanceCounter (&counter));
        return static_cast<DurationSecondsType> (counter.QuadPart) / sPerformanceFrequencyBasis_;
#else
        return ::time (0);    //tmphack... not good but better than assert error
#endif
    }
}
DurationSecondsType Stroika::Foundation::Time::GetTickCount () noexcept
{
    static  const   DurationSecondsType kFirstTC_   =   GetTickCount_ ();
#if     qStroika_FeatureSupported_Valgrind
    // I Think it may be a bug that helgrind doesnt recognize g++ magic statics
    VALGRIND_HG_DISABLE_CHECKING (&kFirstTC_, sizeof(kFirstTC_));
#endif
    return GetTickCount_ () - kFirstTC_;
}
