/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Timezone_h_
#define _Stroika_Foundation_Time_Timezone_h_    1

#include    "../StroikaPreComp.h"

#include    <ctime>

#include    "../Characters/String.h"
#include    "../Memory/Optional.h"



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
             *  Information about the current system timezone setting.
             *
             *  A timezone contains a name for BOTH 'standard time' - most of the year when you are in the nomral
             *  time range - and 'daylight savings time' - a period of the year when you have an alternate timezone name
             *  and offset.
             *
             *  @see http://en.wikipedia.org/wiki/Tz_database
             *
             *  e.g:
             *      ID                  STD ABBR    STD NAME                DST ABBR    DST NAME                GMT offset  DST adjustment  DST Start Date rule     Start time  DST End date rule   End time
             *
             *      America/New_York    EST         Eastern Standard Time   EDT         Eastern Daylight Time   -05:00:00   +01:00:00       2;0;3                   +02:00:00   1;0;11              +02:00:00
             *
             */
            struct  TimeZoneInformationType {
                struct  Details {
                    Memory::Optional<String>    fName;
                    Memory::Optional<String>    fAbbreviation;
                    Memory::Optional<int>       fBiasInMinutesFromUTC;
                };
                Details fStandardTime;
                Details fDaylightSavingsTime;

                // This is the 'olsen database name' (e..g. 'America/New_York')
                Memory::Optional<String>    fID;
            };

            TimeZoneInformationType    GetTimezoneInfo ();

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
