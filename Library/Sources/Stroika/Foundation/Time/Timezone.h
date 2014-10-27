/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Timezone_h_
#define _Stroika_Foundation_Time_Timezone_h_    1

#include    "../StroikaPreComp.h"

#include    <ctime>



/**
 *  \file
 *
 *  \version    <a href="code_status.html">Alpha-Late</a>
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            class   DateTime;


            /**
             * Returns the string (in what format???) identifying the current system timezone.
             *
             *  On UNIX, this amounts to TZ environment variable???Or tz from locale?
             *
             *  @todo CLARIFY and think through better, but this is at least a start..
             */
            String    GetTimezone ();
            String    GetTimezone (bool applyDST);
            String    GetTimezone (const DateTime& d);


            /**
             * Checks if the given DateTime is daylight savings time
             */
            bool    IsDaylightSavingsTime (const DateTime& d);


            /**
             * Return the number of seconds which must be added to a LocalTime value to get GMT.
             */
            time_t  GetLocaltimeToGMTOffset (bool applyDST);
            time_t  GetLocaltimeToGMTOffset (const DateTime& forTime);


        }
    }
}



#endif  /*_Stroika_Foundation_Time_Timezone_h_*/
