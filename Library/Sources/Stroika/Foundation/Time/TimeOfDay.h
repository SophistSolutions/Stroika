/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_TimeOfDay_h_
#define _Stroika_Foundation_Time_TimeOfDay_h_ 1

#include "../StroikaPreComp.h"

#include <climits>
#include <locale>
#include <string>

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Configuration/Enumeration.h"
#include "../Execution/StringException.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Complete removeal of deprecated empty and no-arg constructor
 *
 *      @todo   Need DefaultNames<> for enums in TimeOfDay module
 *
 *      @todo   Consider having some way to support double as TimeOfDay (or maybe float). Don't want the
 *              complexity of the chrono code, but some of the power ;-). Not sure how to compromise.
 *
 *      @todo   I'm not sure eCurrentLocale_WithZerosStripped is a good idea. Not sure if better
 *              to use separate format print arg or???
 *
 *      @todo   Should we PIN or throw OVERFLOW exception on values/requests which are out of range?
 *
 *      @todo   (medium) Consider using strftime and strptime with %FT%T%z.
 *              Same format
 *              That doesn�t use std::locale()
 *              En.cppreference.com/w/cpp/io/manip/get_time
 *              istringstream xxx (�2011-feb��)
 *              ss.imbue(std::locale() (�de-DE�));
 *              ss >> std::get_time(&t, �%FT%T%z�)
 *
 *      @todo   Locale based parsing code seems quite poor. Haven't really evaluated locale-based
 *              print code (but I'm not optimistic). I'm not sure if I have it wrong, or if it just
 *              sucks (main issue is Vis Studio integration - doesn't appear to pay attention to
 *              local settings from regional settings control panel, and doesn't seem at all flexible
 *              about what it accepts). But also the %X output (again - at least for windows/vis studio)
 *              looks terrible - military format - full zero precision - even if not needed?).
 *              MAYBE try %EX is the locale's alternative time representation.
 *
 *      @todo   (minor) Consider if DateTime stuff should cache locale{} in some methods (static) �
 *              so can be re-used?? Performance tweek cuz current stuff doing new locale() does
 *              locking to bump refcount?
 *
 */

namespace Stroika::Foundation::Time {

    using Characters::String;

    /**
     * Description:
     *      A time value - which is assumed to be within a given day - e.g 2:30 pm.
     *
     *      NB: this implies NO NOTION of timezone. Its a time relative to midnight of a given day.
     *
     *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
     */
    class TimeOfDay {
    public:
        /**
         * NB: The maximum value in a TimeOfDay struct is one less than kMaxSecondsPerDay
         */
        static constexpr uint32_t kMaxSecondsPerDay = 60 * 60 * 24u; // nb: 86400: wont fit in uint16_t

    public:
        /**
         * If value out of range - pinned to kMax.
         * We normalize to be within a given day (seconds since midnight)
         *
         *  For the TimeOfDay, we allow out of range values and pin/accumulate. But you can still never have a time of day >= kMaxSecondsPerDay.
         *  And the first hour (1pm) is hour 0, so TimeOfDay (2, 0, 0) is 3am.
         *
         *  \req argument time-of-day (in seconds or hours/minutes/seconds) is in valid range for one day
         *  \req t < kMaxSecondsPerDay
         *  \req hour < 24
         *  \req minute < 60
         *  \req seconds < 60
         */
        [[deprecated ("Use optional<TimeOfDay> instead of TimeOfDay no-arg constructor - as of v2.1d11")]] constexpr TimeOfDay ();
        constexpr TimeOfDay (const TimeOfDay&) = default;
        constexpr TimeOfDay (TimeOfDay&& src)  = default;
        constexpr explicit TimeOfDay (uint32_t t);
        explicit constexpr TimeOfDay (unsigned int hour, unsigned int minute, unsigned int seconds);

    public:
        /**
         */
        nonvirtual TimeOfDay& operator= (const TimeOfDay&) = default;

    public:
        /**
         *  \brief  ParseFormat is a representation which a TimeOfDay can be transformed out of
         *
         *  eCurrentLocale
         *      Note this is the current C++ locale, which may not be the same as the platform default locale.
         *      @see Configuration::GetPlatformDefaultLocale, Configuration::UsePlatformDefaultLocaleAsDefaultLocale ()
         */
        enum class ParseFormat : uint8_t {
            eCurrentLocale,
            eXML [[deprecated ("since Stroika v2.1d11- use eISO8601")]],
            eISO8601,

            Stroika_Define_Enum_Bounds (eCurrentLocale, eISO8601)
        };

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get
         *        equivalent to "%H:%M:%S"
         */
        static constexpr wchar_t kISO8601FormatArray[] = L"%T";
        static const String      kISO8601Format;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get
         */
        static constexpr wchar_t kLocaleStandardFormatArray[] = L"%X";
        static const String      kLocaleStandardFormat;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get 
         */
        static constexpr wchar_t kLocaleStandardAlternateFormatArray[] = L"%EX";
        static const String      kLocaleStandardAlternateFormat;

    public:
        /**
         *  Default formats used by TimeOfDay::Parse () to parse time strings. The first of these - %X, is
         *  the locale-specific time format.
         */
        static const Traversal::Iterable<String> kDefaultParseFormats;

    public:
        /**
         *  Always produces a valid legal TimeOfDay, or throws an exception.
         *
         *  \note an empty string produces BadFormat exception (whereas before 2.1d11 it produced an empty TimeOfDay object (TimeOfDay {}).
         *
         *  \note the 2 argument locale overload uses each of kDefaultParseFormats formats to try to 
         *        parse the time string, but the default is locale specific standard time format.
         *
         *  \see https://en.cppreference.com/w/cpp/locale/time_get/get for allowed formatPatterns
         *
         *  The overload which takes a locale but no explicit format strings, defaults to trying
         *  each of kDefaultParseFormats strings in order, and returns the first match.
         *
         *  The overload taking an iterable of formats, tries each, and returns the timeofday for the first that succeeds, or throws
         *  FormatException if none succeed.
         */
        static TimeOfDay Parse (const String& rep, ParseFormat pf);
        static TimeOfDay Parse (const String& rep, const locale& l);
        static TimeOfDay Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns);
#if qPlatform_Windows
        [[deprecated ("Use Locale APIs instead of LCID APIS - https://docs.microsoft.com/en-us/windows/desktop/api/datetimeapi/nf-datetimeapi-getdateformata says Microsoft is migrating toward the use of locale names instead of locale identifiers - Since Stroika v2.1d11")]] static TimeOfDay Parse (const String& rep, LCID lcid);
#endif

    public:
#if qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
        [[deprecated ("use TimeOfDay::min - deprecated in Stroika v2.1d11")]] static const TimeOfDay kMin;
#else
        [[deprecated ("use TimeOfDay::min - deprecated in Stroika v2.1d11")]] static constexpr TimeOfDay kMin{0};
#endif

    public:
#if qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
        [[deprecated ("use TimeOfDay::max - deprecated in Stroika v2.1d11")]] static const TimeOfDay kMax;
#else
        [[deprecated ("use TimeOfDay::max - deprecated in Stroika v2.1d11")]] static constexpr TimeOfDay kMax{kMaxSecondsPerDay - 1};
#endif

    public:
        /**
         *  min () is the first date this TimeOfDay class supports representing.
         *
         *  @see constexpr TimeOfDay::kMin
         *
         *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
         */
        static constexpr TimeOfDay min ();

    public:
        /**
         *  max () is the last date this TimeOfDay class supports representing. This is a legal TimeOfDay, and
         *  not like 'end' - one past the last legal value.
         *
         *  @see constexpr TimeOfDay::kMax
         *
         *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
         */
        static constexpr TimeOfDay max ();

    public:
        class FormatException;

    public:
        /**
         *  \ensure {return} < kMaxSecondsPerDay
         */
        nonvirtual constexpr uint32_t GetAsSecondsCount () const; // seconds since StartOfDay (midnight)

    public:
        /**
         */
        [[deprecated ("Use optional<TimeOfDay> instead of TimeOfDay no-arg constructor - as of v2.1d11")]] nonvirtual constexpr bool empty () const;

    public:
        /**
         */
        nonvirtual void ClearSecondsField ();

    public:
        /**
         * returns 0..23
         */
        nonvirtual constexpr uint8_t GetHours () const;

    public:
        /**
         *  returns 0..59
         */
        nonvirtual constexpr uint8_t GetMinutes () const;

    public:
        /**
         *  returns 0..59
         */
        nonvirtual constexpr uint8_t GetSeconds () const;

    public:
        /**
         *  \brief  PrintFormat is a representation which a TimeOfDay can be transformed into
         *
         *  eCurrentLocale
         *      Note this is the current C++ locale, which may not be the same as the platform default locale.
         *      @see Configuration::GetPlatformDefaultLocale, Configuration::UsePlatformDefaultLocaleAsDefaultLocale ()
         *
         *  eCurrentLocale_WithZerosStripped
         *      eCurrentLocale_WithZerosStripped is eCurrentLocale, but with many cases of trailing zero's,
         *      and sometimes leading zeros, stripped, so for example, 01:03:05 PM will become 1:03:05 PM,
         *      and 04:06:00 PM will become 4:06 PM.
         */
        enum class PrintFormat : uint8_t {
            eCurrentLocale,
            eISO8601,
            eXML [[deprecated ("since Stroika v2.1d11- use eISO8601")]],
            eCurrentLocale_WithZerosStripped,

            eDEFAULT = eCurrentLocale_WithZerosStripped,

            Stroika_Define_Enum_Bounds (eCurrentLocale, eCurrentLocale_WithZerosStripped)
        };

    public:
        /**
         *  For formatPattern, see http://en.cppreference.com/w/cpp/locale/time_put/put
         *  If only formatPattern specified, and no locale, use default (global) locale.
         */
        nonvirtual String Format (PrintFormat pf = PrintFormat::eDEFAULT) const;
        nonvirtual String Format (const locale& l) const;
        nonvirtual String Format (const locale& l, const String& formatPattern) const;
        nonvirtual String Format (const String& formatPattern) const;
#if qPlatform_Windows
        [[deprecated ("Use Locale APIs instead of LCID APIS - https://docs.microsoft.com/en-us/windows/desktop/api/datetimeapi/nf-datetimeapi-getdateformata says Microsoft is migrating toward the use of locale names instead of locale identifiers - Since Stroika v2.1d11")]] nonvirtual String Format (LCID lcid) const;
#endif

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    public:
        /**
         *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
         */
        nonvirtual int Compare (const TimeOfDay& rhs) const;
        static int     Compare (const optional<TimeOfDay>& lhs, const optional<TimeOfDay>& rhs);

    private:
        uint32_t fTime_;
    };

    class TimeOfDay::FormatException : public Execution::StringException {
    public:
        FormatException ();

    public:
        /**
         */
        static const FormatException kThe;
    };

    /**
     *  operator indirects to TimeOfDay::Compare()
     */
    bool operator< (TimeOfDay lhs, TimeOfDay rhs);

    /**
     *  operator indirects to TimeOfDay::Compare()
     */
    bool operator<= (TimeOfDay lhs, TimeOfDay rhs);

    /**
     *  operator indirects to TimeOfDay::Compare()
     */
    bool operator== (TimeOfDay lhs, TimeOfDay rhs);

    /**
     *  operator indirects to TimeOfDay::Compare()
     */
    bool operator!= (TimeOfDay lhs, TimeOfDay rhs);

    /**
     *  operator indirects to TimeOfDay::Compare()
     */
    bool operator>= (TimeOfDay lhs, TimeOfDay rhs);

    /**
     *  operator indirects to TimeOfDay::Compare()
     */
    bool operator> (TimeOfDay lhs, TimeOfDay rhs);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimeOfDay.inl"

#endif /*_Stroika_Foundation_Time_TimeOfDay_h_*/
