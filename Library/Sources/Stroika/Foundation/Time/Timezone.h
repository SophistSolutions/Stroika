/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Timezone_h_
#define _Stroika_Foundation_Time_Timezone_h_ 1

#include "../StroikaPreComp.h"

#include <ctime>

#include "../Characters/String.h"
#include "../Memory/Optional.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>
 *
 * TODO:
 *
 *      @todo   MAYBE use http://www.iana.org/time-zones/repository/releases/tzdata2015b.tar.gz and code - and have part of
 *              thirdpartylibs, and then compile the zone file there, and reference that somehow here? Or add something
 *              to that code to generate our own table which we can #include? Maybe add BuildTools (like Tools) - and
 *              there no dependnecies on stroika (execept makefile part) - and build code which we use to boostrap stroika
 *              building file in intermediatefiles which we #incldue as initailzied data for timezone module
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Time {

            class Date;
            class DateTime;
            class TimeOfDay;

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
            struct TimeZoneInformationType {
                struct Details {
                    Memory::Optional<String> fName;
                    Memory::Optional<String> fAbbreviation;
                    Memory::Optional<int>    fBiasInMinutesFromUTC; // UTC + bias => local time, so for example, EDT this is -300 (aka -5 hrs)
                };
                Details fStandardTime;
                Details fDaylightSavingsTime;

                /**
                 *  This is the 'olsen database name' (e..g. 'America/New_York').
                 *
                 *  It is OFTEN not available (on older OSes).
                 */
                Memory::Optional<String> fID;
            };

            /**
             */
            TimeZoneInformationType GetCurrentLocaleTimezoneInfo ();

            [[deprecated ("use GetCurrentLocaleTimezoneInfo ()")]] inline TimeZoneInformationType GetTimezoneInfo ()
            {
                return GetCurrentLocaleTimezoneInfo ();
            }

            /**
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
            class Timezone {
            private:
                enum class TZ_ : uint16_t {
                    eLocalTime,
                    eUTC,
                    eFixedOffsetBias
                };
                constexpr Timezone (TZ_ tz) noexcept;

            public:
                using BiasInMinutesFromUTCType = int16_t;

            public:
                Timezone () = delete;
                constexpr Timezone (BiasInMinutesFromUTCType biasInMinutesFromUTC);
                constexpr Timezone (const Timezone& src) = default;
                constexpr Timezone (Timezone&& src)      = default;

            public:
                /**
                 */
                nonvirtual Timezone& operator= (const Timezone& rhs) = default;
                nonvirtual Timezone& operator= (Timezone&& rhs) = default;

            public:
                /**
                 *  Returns Timezone object in UTC timezone.
                 *
                 *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
                 */
                static constexpr Timezone UTC ();

            public:
                /**
                 *  Returns Timezone object in localtime timezone.
                 *
                 *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
                 */
                static constexpr Timezone LocalTime ();

            public:
                /**
                 *  Returns Timezone object in localtime timezone.
                 *
                 *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
                 */
                static constexpr Memory::Optional<Timezone> Unknown ();

            public:
/**
                 *  @see Timezone::UTC ()
                 *
                 *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
                 */
#if qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
                static const Timezone kUTC;
#else
                static constexpr Timezone kUTC{TZ_::eUTC};
#endif

            public:
/**
                 *  @see Timezone::LocalTime ()
                 *
                 *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
                 */
#if qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
                static const Timezone kLocalTime;
#else
                static constexpr Timezone kLocalTime{TZ_::eLocalTime};
#endif

            public:
                /**
                 *  @see Timezone::Unknown ()
                 *
                 *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
                 *        BUT - this is trickier than the others cuz its not even Timezone but a type derived from it. Could be nice if it
                 *        worked!
                 */
                static const Memory::Optional<Timezone> kUnknown;

            public:
                /**
                 *  Depending on the form of the timezone, the offset from UTC could depned on the date (cuz of daylight savings time)
                 *
                 *  \note returns minutes
                 */
                nonvirtual BiasInMinutesFromUTCType GetBiasInMinutesFromUTCType (const Date& date, const TimeOfDay& tod) const;

            public:
                /**
                 *  Depending on the form of the timezone, the offset from UTC could depned on the date (cuz of daylight savings time)
                 *
                 *  \note returns seconds
                 */
                nonvirtual make_signed<time_t>::type GetOffset (const Date& date, const TimeOfDay& tod) const;

            public:
                /**
                 *  For some kinds of timezones, there is no way to know (e.g. +4:00), but return true if known true, and false if known false.
                 */
                nonvirtual Memory::Optional<bool> IsDaylightSavingsTime (const Date& date, const TimeOfDay& tod);

            public:
                nonvirtual constexpr bool operator== (const Timezone& rhs) const;
                nonvirtual constexpr bool operator!= (const Timezone& rhs) const;

            private:
                TZ_                      fTZ_ : 16;
                BiasInMinutesFromUTCType fBiasInMinutesFromUTC_ : 16;
            };
            namespace {
                struct _HACK_2_TEST_4_static_assert_ {
                    uint32_t a;
                };
            }
            static_assert (sizeof (Timezone) <= sizeof (_HACK_2_TEST_4_static_assert_), "Timezone can/should be packed as much as practical since we could use a single uint16_ probably"); // make sure full struct as small as possible

            /**
             *  @see Timezone::kUnknown :  to workaround qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
             */
            [[deprecated ("use Timezone::Unknown ()")]] constexpr Memory::Optional<Timezone> Timezone_kUnknown{};

            /**
             * Returns the string (in what format???) identifying the current system timezone.
             *
             *  On UNIX, this amounts to TZ environment variable???Or tz from locale?
             *
             *  @todo CLARIFY and think through better, but this is at least a start..
             */
            [[deprecated ("use Timezone::GetCurrentLocaleTimezoneInfo ()fStandardTime.fNameor fDaylightSavingsTime if currentlt DST")]] String GetTimezone ();
            [[deprecated ("use Timezone::GetCurrentLocaleTimezoneInfo (). {fDaylightSavingsTime OR fStandardTime}")]] String GetTimezone (bool applyDST);
            [[deprecated ("use Timezone::GetCurrentLocaleTimezoneInfo ()fDaylightSavingsTime OR fStandardTime")]] String GetTimezone (const DateTime& d);

            /**
             * Checks if the given DateTime is daylight savings time (in the current locale)
             */
            [[deprecated ("use Timezone::LocalTime ().IsDaylightSavingsTime ()")]] bool IsDaylightSavingsTime (const Date& date, const TimeOfDay& tod);
            [[deprecated ("use Timezone::LocalTime ().IsDaylightSavingsTime ()")]] bool IsDaylightSavingsTime (const DateTime& d);

            /**
             * Return the number of seconds which must be added to a LocalTime value to get GMT.
             */
            [[deprecated ("use - Timezone::LocalTime ().GetOffset ()")]] time_t GetLocaltimeToGMTOffset (bool applyDST);
            [[deprecated ("use - Timezone::LocalTime ().GetOffset ()")]] time_t GetLocaltimeToGMTOffset (const DateTime& forTime);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Timezone.inl"

#endif /*_Stroika_Foundation_Time_Timezone_h_*/
