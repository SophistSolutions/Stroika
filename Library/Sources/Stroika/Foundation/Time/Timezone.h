/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Timezone_h_
#define _Stroika_Foundation_Time_Timezone_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>
#include <ctime>
#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/DataExchange/ValidationStrategy.h"
#include "Stroika/Foundation/Traversal/Range.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   See https://en.cppreference.com/w/cpp/chrono/time_zone - and support this. But the C++20 doesn't REPLACE
 *              the Stroika timezone - just an extra thing a Stroika timezone can reference (may replace stroika tz database).
 *              But Stroika tz allows representing explicit stuff like +04:00 etc, which cannot be represented by time_zone
 *              class.  --LGP 2022-11-07
 *
 *      @todo   https://github.com/SophistSolutions/Stroika/issues/772 (STK-636)
 *              Enhance Timezone class to support real timezones (e.g. American/NewYork) and handle all the cases
 *              of mapping offsets (applying all teh rules for different dates)
 *
 *              Could use boost or  http://www.iana.org/time-zones/repository/releases/tzdata2015b.tar.gz  or other.
 *
 *              But good news - I think I have API right now. Just construct a Timezone object with enum or data read from file
 *              and can be used as is throughout rest of DateTime code (well, TimeZoneInformationType probably needs changes).
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
     *  A timezone contains a name for BOTH 'standard time' - most of the year when you are in the normal
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
     *  @see https://github.com/SophistSolutions/Stroika/issues/772 (STK-636)
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
         *  This is the 'Olsen database name' (e..g. 'America/New_York').
         *
         *  It is OFTEN not available (on older OSes).
         */
        optional<Characters::String> fID;
    };

    /**
     *  \brief return a summary of information about the currently configured default timezone on this system - the one
     *         Timezone::kLocalTime is (so on Windows, Windows' own setting: TZ is examined only on POSIX)
     */
    [[nodiscard]] TimeZoneInformationType GetCurrentLocaleTimezoneInfo ();

    /**
     *  The Timezone class represents what one would think of as a timezone - mostly.
     *
     *  But - it only supports these timezones:
     *      o   UTC
     *      o   kLocalTime - special: floating - the local time zone at the moment the value is used, not when it was made
     *      o   Fixed Offset from UTC (suitable for reading and writing ISO8601 times)
     *  and a DateTime may have none (kUnknown).
     *
     *  But it currently does NOT support the notion of timezone like American/NewYork (unless that happens to be localtime)
     *  That feature may be eventually added - https://github.com/SophistSolutions/Stroika/issues/772 (STK-636)
     *
     *  \par Local time
     *      Handling local time is complicated, and not quite portable. Local time is:
     *          o   POSIX: the C library's local time zone (mktime) - the TZ environment variable, else the system's (/etc/localtime)
     *          o   Windows: Windows' own time zone setting, by each year's rules (GetTimeZoneInformationForYear - a year outside
     *              1601-30827 as standard time). TZ is never examined on Windows.
     *      It is looked up each time it is used, not when a DateTime is made: a DateTime in kLocalTime is a wall-clock date and
     *      time, meaning whatever moment that is in the local time zone of then. Its offset from UTC depends on the date and time
     *      (daylight saving time), so GetBiasFromUTC () and the like take them.
     *      A time to store or exchange is safest in UTC (AsUTC ()): a local one means another moment on another machine, and one
     *      in the hour daylight saving time repeats is ambiguous.
     *
     *  \par Where Stroika's local time can differ from what other code says
     *      o   Windows, TZ set (Cygwin passes its TZ, an IANA name, to the programs it starts): the C library's time functions
     *          (localtime, mktime, strftime's %z %Z) follow it, reading an IANA name as UTC; Stroika, Windows and
     *          std::chrono::current_zone () (MSVC, as of 19.51) ignore it.
     *      o   Windows, a past year: the C library applies this year's daylight saving rules (and refuses dates before 1970);
     *          Stroika uses each year's own.
     *      o   POSIX, TZ set: std::chrono::current_zone () (libstdc++, as of 16) ignores it - it names /etc/localtime's zone.
     *
     *  @todo Date::As<time_point> (local midnight) and DateTime::Format's %z %Z still take local time from the C library, so on
     *        Windows follow TZ (@see https://github.com/SophistSolutions/Stroika/issues/806).
     *
     *  @see https://msdn.microsoft.com/en-us/library/system.timezone(v=vs.110).aspx
     *
     *  \note <a href="Design-Overview.md#Comparisons">Comparisons</a>:
     *          static_assert (totally_ordered<Timezone>);
     *          o   comparison of < not 100% intuitive, but unambiguous, so these can be compared (by numeric value of flags/offsets).
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
         *  if ValidationStrategy is eAssert (DEFAULT if not specified)
         *      \pre kBiasInMinutesFromUTCTypeValidRange.Contains (biasInMinutesFromUTC)
         *  else throws if out of range.
         */
        Timezone () = delete;
        constexpr explicit Timezone (BiasInMinutesFromUTCType biasInMinutesFromUTC) noexcept;
        constexpr Timezone (BiasInMinutesFromUTCType biasInMinutesFromUTC, DataExchange::ValidationStrategy validationStrategy);
        constexpr Timezone (const Timezone& src)     = default;
        constexpr Timezone (Timezone&& src) noexcept = default;

    public:
        /**
         */
        nonvirtual Timezone& operator= (Timezone&& rhs) noexcept = default;
        nonvirtual Timezone& operator= (const Timezone& rhs)     = default;

    public:
        static const Timezone kUTC; // defined constexpr

    public:
        /**
         *  \brief Local time, whatever it is when used - floating, not a zone fixed when a DateTime is made
         *
         *  A DateTime in kLocalTime holds a wall-clock date and time, read in the operating system's local time zone each time it
         *  is converted (AsUTC (), comparing with another time zone, writing an offset). So the same value can mean a different
         *  moment later - after the machine's time zone (or, on POSIX, TZ) changes - or on another machine.
         *
         *  The local time zone is, on POSIX, the C library's (TZ, else /etc/localtime); on Windows, Windows' own setting (TZ is
         *  never examined there).
         *
         *  @see Timezone - how each platform defines local time, and where Stroika's can differ from what the C and C++ libraries say
         *  @see GetCurrentLocaleTimezoneInfo () - its names and offsets
         */
        static const Timezone kLocalTime; // defined constexpr

    public:
        /**
         *  \brief No time zone - for a DateTime whose date and time are in none said: read as UTC by AsUTC (), as local time by
         *         AsLocalTime (), and compared with any other DateTime as if in its time zone
         */
#if qCompilerAndStdLib_reflection_static_member_type_instantiated_while_incomplete_Buggy
        static const optional<Timezone>& kUnknown; // defined constexpr
#else
        static const optional<Timezone> kUnknown; // defined constexpr
#endif

    public:
        /**
         *  Parse string of the form:
         *      [+-]?HHMM, or [+-]?HH:MM, so for example -0500 is Timezone (-5*60), or -04:00 would be Timezone (-4*60).
         *
         *  On empty string, return nullopt, but on ill-formed timezone offset string (including out of range), 
         *  throw (even if the string is EST, or some such - this requires numeric offset).
         * 
         *  This parse function ignores any bad data at the end of tzStr (perhaps future version will be optionally more picky).
         */
        static optional<Timezone> ParseTimezoneOffsetString (const char* tzStr);
        static optional<Timezone> ParseTimezoneOffsetString (const wchar_t* tzStr);
        static optional<Timezone> ParseTimezoneOffsetString (const Characters::String& tzStr);

    public:
        /**
         *  generate a string of the form:
         *      [+-]?HHMM, or [+-]?HH:MM, so for example -0500 is Timezone (-5*60), or -04:00 would be Timezone (-4*60).
         *
         *  \note Date/Time required in case Timezone is 'localtime' to determine DST
         */
        nonvirtual String AsHHMM (const Date& date, const TimeOfDay& tod, bool insertColon) const;

    public:
        /**
         *  @see https://tools.ietf.org/html/rfc822#section-5
         *
         *  Can generate "GMT" or AsHMM(false) above.
         *
         *  \note Date/Time required in case Timezone is 'localtime' to determine DST
         *
         *  @aliases AsRFC822 ()
         */
        nonvirtual String AsRFC1123 (const Date& date, const TimeOfDay& tod) const;

    public:
        /**
         *  Depending on the form of the timezone, the offset from UTC could depend on the date (cuz of daylight savings time)
         *
         *  This offset (number of minutes) - is added to a UTC time to get the time in that local timezone.
         *
         *  \post  (kBiasInMinutesFromUTCTypeValidRange.Contains (fBiasInMinutesFromUTC_));
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

    public:
        /**
         */
        nonvirtual constexpr auto operator<=> (const Timezone& rhs) const = default;

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
    static_assert (sizeof (Timezone) <= sizeof (_HACK_2_TEST_4_static_assert_),
                   "Timezone can/should be packed as much as practical since we could use a single uint16_ probably"); // make sure full struct as small as possible
    static_assert (totally_ordered<Timezone>);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Timezone.inl"

#endif /*_Stroika_Foundation_Time_Timezone_h_*/
