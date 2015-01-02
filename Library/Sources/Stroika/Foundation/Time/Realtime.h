/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Realtime_h_
#define _Stroika_Foundation_Time_Realtime_h_    1

#include    "../StroikaPreComp.h"

#if     !qCompilerAndStdLib_constexpr_Buggy
#include    <limits>
#else
#include    <cfloat>
#endif



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 *      @todo   Consider making Time::GetTickCount () inline. Measure code-size impact.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            using       DurationSecondsType     =   double;


            /**
             *  Get the number of seconds since some constant, system-specified reference time. This is used
             *  to tell how much time has elapsed since a particular event.
             *
             *  Note - though this is based on the same reference time as an time values packed into event records,
             *  it maybe differently normalized. These times are all in seconds, whereas event records are often in
             *  other units (ticks - 1/60 of a second, or milliseconds).
             *
             *  In the case of X-Windows - this business is very complicated because there are two different times
             *  one might be intersted in. There is the time on the client (where Led is running) and the time on the X-Server
             *  (users computer screen). Alas - X11R4 appears to have quite weak support for time - and offers no way I've found
             *  to accurately get the time from the users computer. As a result - with X-Windows - you must arrange to call
             *  @'SyncronizeLedXTickCount' for each event that specifies a time value (as soon as that event arrives). This
             *  data - together with time values from the client (where Led is running) computer will be used to provide a
             *  better approximation of the true elapsed time.
             */
            DurationSecondsType GetTickCount ();


            /**
            */
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr   DurationSecondsType kInfinite   =   std::numeric_limits<DurationSecondsType>::max ();
#else
            const   DurationSecondsType kInfinite       =   DBL_MAX;
#endif


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Realtime.inl"

#endif  /*_Stroika_Foundation_Time_Realtime_h_*/
