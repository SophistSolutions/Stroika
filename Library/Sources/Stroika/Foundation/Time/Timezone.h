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
             */
            String    GetTimeZone ();


            /**
             * Checks if it is NOW DaylightSavingsTime
             */
            bool    IsDaylightSavingsTime ();


            /**
             * Checks if the given date is daylight savings time
             */
            bool    IsDaylightSavingsTime (const DateTime& d);


            /**
             * Return the number of seconds which must be added to a LocalTime value to get GMT.
             */
            time_t  GetLocaltimeToGMTOffset (bool applyDST = IsDaylightSavingsTime ());


        }
    }
}



#endif  /*_Stroika_Foundation_Time_Timezone_h_*/
