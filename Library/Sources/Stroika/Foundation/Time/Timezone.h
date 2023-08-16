/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Timezone_h_
#define _Stroika_Foundation_Time_Timezone_h_ 1

#include "../StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

#include <ctime>
#include <optional>

#include "../Characters/String.h"
#include "../Traversal/Range.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-636
 *              Enhance Timezone class to support real timezones (e.g. American/NewYork) and handle all the cases
 *              of mapping offsets (applying all teh rules for different dates)
 *
 *              Could use boost or  http://www.iana.org/time-zones/repository/releases/tzdata2015b.tar.gz  or other.
 *
 *              But good news - I think I have API right now. Just construct a Timezone object with enum or data read from file
 *              and can be used as is throughtout rest of DateTime code (well, TimeZoneInformationType probably needs changes).
 *
 *              REVIEW C++20 support here - I think they added support for named timezone map to offsets.
 */

namespace Stroika::Foundation::Time {

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
     *  @see https://stroika.atlassian.net/browse/STK-636
     */
    struct TimeZoneInformationType {
        struct Details {
            optional<Characters::String> fName;
            optional<Characters::String> fAbbreviation;
            optional<int>                fBiasInMinutesFromUTC; // UTC + bias => local time, so for example, EDT this is -300 (aka -5 hrs)
        };
        Details fStandardTime;
        Details fDaylightSavingsTime;

        /**
         *  This is the 'olsen database name' (e..g. 'America/New_York').
         *
         *  It is OFTEN not available (on older OSes).
         */
        optional<Characters::String> fID;
    };

    /**
     *  \brief return a summary of information about the currently configured default timezone on this system.
     */
    TimeZoneInformationType GetCurrentLocaleTimezoneInfo ();

    /**
     *  The Timezone class represents what one would think of as a timezone - mostly.
     *
     *  But - it only supports these timezones:
     *      o   UTC
     *      o   LocalTime () special - whatever timezone this computer is in
     *      o   Fixed Offset from UTC (suitable for reading and writing ISO8601 times)
     *
     *  But it currently does NOT support the notion of timezone like American/NewYork (unless that happens to be localtime)
     *  That feature may be eventually added - https://stroika.atlassian.net/browse/STK-636
     *
     *  @see https://msdn.microsoft.com/en-us/library/system.timezone(v=vs.110).aspx
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *          o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     *          o   comparison of < not 100% intuitive, but unambiguous, so these can be compared (by numeric value of flags/offets).
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
        /**
         *  According to https://en.wikipedia.org/wiki/List_of_UTC_time_offsets, these actually vary from -12, to 14. But logically, the only thing really crazy would be > 24 or < -24 hours.
         */
        static constexpr Traversal::Range<BiasInMinutesFromUTCType> kBiasInMinutesFromUTCTypeValidRange{-24 * 60, 24 * 60};

    public:
        /**
         *  \req kBiasInMinutesFromUTCTypeValidRange.Contains (biasInMinutesFromUTC)
         */
        Timezone () = delete;
        constexpr Timezone (BiasInMinutesFromUTCType biasInMinutesFromUTC);
        constexpr Timezone (const Timezone& src)     = default;
        constexpr Timezone (Timezone&& src) noexcept = default;

    public:
        /**
         */
        nonvirtual Timezone& operator= (Timezone&& rhs) noexcept = default;
        nonvirtual Timezone& operator= (const Timezone& rhs)     = default;

    public:
        /**
         *  Returns Timezone object in UTC timezone.
         */
        static const Timezone kUTC; // defined constexpr

    public:
        /**
         *  Returns Timezone object in localtime timezone.
         *
         *  \note - LocalTime is a STICKY property. This does NOT return the FIXED OFFSET for the current local-time, but rather a special
         *          Timezone which always references that global current timezone.
         */
        static const Timezone kLocalTime; // defined constexpr

    public:
        /**
         *  Returns Timezone object in localtime timezone.
         */
        static const optional<Timezone> kUnknown; // defined constexpr

    public:
        /**
         *  Parse string of the form:
         *      [+-]?HHMM, or [+-]?HH:MM, so for example -0500 is Timezone (-5*60), or -04:00 would be Timezone (-4*60).
         *
         *  On empty string, return {}, but on ill-formed timezone offset string (including out of range), 
         *  throw (even if the string is EST, or some such - this requires numeric offset).
         */
        static optional<Timezone> ParseTimezoneOffsetString (const char* tzStr);
        static optional<Timezone> ParseTimezoneOffsetString (const wchar_t* tzStr);
        static optional<Timezone> ParseTimezoneOffsetString (const Characters::String& tzStr);

    public:
        /**
         *  generate a string of the form:
         *      [+-]?HHMM, or [+-]?HH:MM, so for example -0500 is Timezone (-5*60), or -04:00 would be Timezone (-4*60).
         *
         *  \note Date/Time required in case Timezone is 'localtime' to determinte DST
         */
        nonvirtual String AsHHMM (const Date& date, const TimeOfDay& tod, bool insertColon) const;

    public:
        /**
         *  @see https://tools.ietf.org/html/rfc822#section-5
         *
         *  Can generate "GMT" or AsHMM(false) above.
         *
         *  \note Date/Time required in case Timezone is 'localtime' to determinte DST
         *
         *  \note alias - could have been called AsRFC822 ()
         */
        nonvirtual String AsRFC1123 (const Date& date, const TimeOfDay& tod) const;

    public:
        /**
         *  Depending on the form of the timezone, the offset from UTC could depned on the date (cuz of daylight savings time)
         *
         *  This offset (number of minutes) - is added to a UTC time to get the time in that local timezone.
         *
         *  \ens  (kBiasInMinutesFromUTCTypeValidRange.Contains (fBiasInMinutesFromUTC_));
         */
        nonvirtual BiasInMinutesFromUTCType GetBiasInMinutesFromUTC (const Date& date, const TimeOfDay& tod) const;

    public:
        /**
         *  Depending on the form of the timezone, the offset from UTC could depend on the date (cuz of daylight savings time)
         *
         *  This offset (number of seconds) - is added to a UTC time to get the time in that local timezone.
         */
        nonvirtual make_signed_t<time_t> GetBiasFromUTC (const Date& date, const TimeOfDay& tod) const;

    public:
        /**
         *  For some kinds of timezones, there is no way to know (e.g. +4:00), but return true if known true, and false if known false.
         */
        nonvirtual optional<bool> IsDaylightSavingsTime (const Date& date, const optional<TimeOfDay>& tod);

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual constexpr auto operator<=> (const Timezone& rhs) const = default;
#else
    public:
        /**
         */
        nonvirtual constexpr bool operator== (const Timezone& rhs) const;

    public:
        /**
         */
        nonvirtual constexpr bool operator!= (const Timezone& rhs) const;

    public:
        /**
         */
        nonvirtual constexpr bool operator< (const Timezone& rhs) const;
#endif

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;

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

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Timezone.inl"

#endif /*_Stroika_Foundation_Time_Timezone_h_*/
