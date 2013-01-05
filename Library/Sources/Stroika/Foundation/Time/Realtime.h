/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Realtime_h_
#define _Stroika_Foundation_Time_Realtime_h_    1

#include    "../StroikaPreComp.h"

#if     qCompilerAndStdLib_Supports_constexpr
#include    <limits>
#else
#include    <cfloat>
#endif

namespace   Stroika {
    namespace   Foundation {
        namespace   Time {

            typedef double  DurationSecondsType;

            /*
            @METHOD:        GetTickCount
            @DESCRIPTION:   <p>Get the number of seconds since some constant, system-specified reference time. This is used
                        to tell how much time has elapsed since a particular event.</p>
                            <p>Note - though this is based on the same reference time as an time values packed into event records,
                        it maybe differently normalized. These times are all in seconds, whereas event records are often in
                        other units (ticks - 1/60 of a second, or milliseconds).</p>
                            <p>In the case of X-Windows - this business is very complicated because there are two different times
                        one might be intersted in. There is the time on the client (where Led is running) and the time on the X-Server
                        (users computer screen). Alas - X11R4 appears to have quite weak support for time - and offers no way I've found
                        to accurately get the time from the users computer. As a result - with X-Windows - you must arrange to call
                        @'SyncronizeLedXTickCount' for each event that specifies a time value (as soon as that event arrives). This
                        data - together with time values from the client (where Led is running) computer will be used to provide a
                        better approximation of the true elapsed time.</p>
            */
            DurationSecondsType GetTickCount ();

#if     qCompilerAndStdLib_Supports_constexpr
            constexpr   DurationSecondsType kInfinite   =   numeric_limits<DurationSecondsType>::max ();
#else
            const   DurationSecondsType kInfinite       =   DBL_MAX;
#endif

        }
    }
}
#endif  /*_Stroika_Foundation_Time_Realtime_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Realtime.inl"
