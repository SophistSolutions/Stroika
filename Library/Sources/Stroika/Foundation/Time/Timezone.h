/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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
 * TODO:
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-555 - Improve Timezone object so that we can read time with +500, and respect that
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
             *  @see http://en.wikipedia.org/wiki/List_of_tz_database_time_zones
             *  @see http://www.iana.org/time-zones
             *  @see http://www.iana.org/time-zones/repository/releases/tzdata2015b.tar.gz (this appears to be where the zones are defined)
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
                    Memory::Optional<int>       fBiasInMinutesFromUTC;      // UTC + bias => local time, so for example, EDT this is -300 (aka -5 hrs)
                };
                Details fStandardTime;
                Details fDaylightSavingsTime;

                /**
                 *  This is the 'olsen database name' (e..g. 'America/New_York').
                 *
                 *  It is OFTEN not available (on older OSes).
                 */
                Memory::Optional<String>    fID;
            };

            /**
             */
            TimeZoneInformationType    GetTimezoneInfo ();

            /**
             *  \brief EARLY ROUGH DRAFT
             *
             *      \note   USELESS without corresponding changes to DateTime code.
             *
             * OLD OBSOLETE DOCS FROM OLD TIMEZONE:
             *
             *  Most of the time applications will utilize localtime. But occasionally its useful to use
             *  different timezones, and our approach to this is to simply convert everything to GMT.
             *
             *  eUnknown - for the most part - is treated as if it were localtime (except with compare).
             *  However - the "Kind" function returns Unknown in case your application wants to treat it
             *  differently.
             *
             *  @todo see https://msdn.microsoft.com/en-us/library/system.timezone(v=vs.110).aspx
             */
            class   Timezone {
            private:
                enum class TZ_ {
                    eLocalTime,
                    eUTC,
                };
                TZ_ fTZ_;
                constexpr   Timezone (TZ_ tz) noexcept
                    : fTZ_ (tz)
                {
                }

            public:
                bool operator== (const Timezone& rhs) const { return fTZ_ == rhs.fTZ_; }
                bool operator!= (const Timezone& rhs) const { return fTZ_ != rhs.fTZ_; }

            public:
                static  const   Timezone                    kUTC;
                static  const   Timezone                    kLocalTime;
                static  const   Memory::Optional<Timezone>  kUnknown;

            public:
                _Deprecated_ ("USE kLocalTime instead - deprecated v2.0a185")
                static  const   Timezone    eLocalTime;
                _Deprecated_ ("USE kUTC instead - deprecated v2.0a185")
                static  const   Timezone    eUTC;
                _Deprecated_ ("USE Timezone_kUnknown instead - deprecated v2.0a185")
                static  const   Memory::Optional<Timezone>  eUnknown;
            };


            /**
             */
            constexpr   Memory::Optional<Timezone>    Timezone_kUnknown{};


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



/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include    "Timezone.inl"

#endif  /*_Stroika_Foundation_Time_Timezone_h_*/
