/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_TimeOfDay_h_
#define _Stroika_Foundation_Time_TimeOfDay_h_ 1

#include "../StroikaPreComp.h"

#include <climits>
#include <compare>
#include <locale>
#include <string>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Configuration/Enumeration.h"
#include "../Execution/Exceptions.h"
#include "../Traversal/Iterable.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>
 *
 * TODO:
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
 *              That doesn't use std::locale()
 *              En.cppreference.com/w/cpp/io/manip/get_time
 *              istringstream xxx ('2011-feb')
 *              ss.imbue(std::locale() ('de-DE'));
 *              ss >> std::get_time(&t, '%FT%T%z')
 */

namespace Stroika::Foundation::Time {

    using Characters::String;

    /**
     * Description:
     *      A time value - which is assumed to be within a given day - e.g 2:30 pm.
     *
     *  \note this implies NO NOTION of timezone. Its a time relative to midnight of a given day.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *        o Standard Stroika Comparison support (operator<=>,operator==, etc);
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
         *  And the first hour (1pm) is hour 0, so TimeOfDay{2, 0, 0} is 3am.
         *
         *  \req argument time-of-day (in seconds or hours/minutes/seconds) is in valid range for one day
         *  \req t < kMaxSecondsPerDay
         *  \req hour < 24
         *  \req minute < 60
         *  \req seconds < 60
         */
        constexpr TimeOfDay (TimeOfDay&& src) noexcept = default;
        constexpr TimeOfDay (const TimeOfDay&)         = default;
        constexpr explicit TimeOfDay (uint32_t t);
        explicit constexpr TimeOfDay (unsigned int hour, unsigned int minute, unsigned int seconds);

    public:
        /**
         */
        nonvirtual TimeOfDay& operator= (TimeOfDay&&) noexcept = default;
        nonvirtual TimeOfDay& operator= (const TimeOfDay&) = default;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get and https://en.cppreference.com/w/cpp/locale/time_put/put
         *        equivalent to "%H:%M:%S"
         *
         *  \note leading zeros in hours, minutes, seconds, required, not optional
         *  \note this is locale-independent
         */
        static constexpr wstring_view kISO8601Format = L"%T"sv;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get and https://en.cppreference.com/w/cpp/locale/time_put/put
         */
        static constexpr wstring_view kLocaleStandardFormat = L"%X"sv;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get and https://en.cppreference.com/w/cpp/locale/time_put/put
         */
        static constexpr wstring_view kLocaleStandardAlternateFormat = L"%EX"sv;

    public:
        /**
         *  Default formats used by TimeOfDay::Parse () to parse time strings. The first of these - %X, is
         *  the locale-specific time format.
         *
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get and https://en.cppreference.com/w/cpp/locale/time_put/put
         */
        static const Traversal::Iterable<String> kDefaultParseFormats;

    public:
        /**
         *  Always produces a valid legal TimeOfDay, or throws an exception.
         *
         *  \note an empty string produces FormatException exception (whereas before 2.1d11 it produced an empty TimeOfDay object (TimeOfDay {}).
         *
         *  \note the 2 argument locale overload uses each of kDefaultParseFormats formats to try to 
         *        parse the time string, but the default is locale specific standard time format.
         * 
         *  \note overloads with the locale missing, default to locale{} - the default locale.
         *
         *  \note format strings defined by https://en.cppreference.com/w/cpp/locale/time_get/get and https://en.cppreference.com/w/cpp/locale/time_put/put
         *
         *  \see https://en.cppreference.com/w/cpp/locale/time_get/get for allowed formatPatterns
         *
         *  The overload which takes a locale but no explicit format strings, defaults to trying
         *  each of kDefaultParseFormats strings in order, and returns the first match.
         *
         *  The overload taking an iterable of formats, tries each, and returns the timeofday for the first that succeeds, or throws
         *  FormatException if none succeed.
         */
        static TimeOfDay Parse (const String& rep, const locale& l = locale{});
        static TimeOfDay Parse (const String& rep, const String& formatPattern);
        static TimeOfDay Parse (const String& rep, const locale& l, const String& formatPattern);
        static TimeOfDay Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns);

    public:
        /**
         *  \brief like Parse(), but returns nullopt on parse error, not throwing exception.
         * if locale is missing, and formatPattern is not locale independent, the current locale (locale{}) is used.
         *  if rep is empty, this will return nullopt
         */
        static optional<TimeOfDay> ParseQuietly (const String& rep, const String& formatPattern);
        static optional<TimeOfDay> ParseQuietly (const String& rep, const locale& l, const String& formatPattern);

    private:
        // this rquires rep!= ""
        static optional<TimeOfDay> ParseQuietly_ (const wstring& rep, const String& formatPattern);
        static optional<TimeOfDay> ParseQuietly_ (const wstring& rep, const time_get<wchar_t>& tmget, const String& formatPattern);

    public:
        /**
         *  kMin is the first date this TimeOfDay class supports representing.
         */
        static const TimeOfDay kMin; // defined constexpr

    public:
        /**
         *  kMax is the last date this TimeOfDay class supports representing. This is a legal TimeOfDay, and
         *  not like 'end' - one past the last legal value.
         */
        static const TimeOfDay kMax; // defined constexpr

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
         *  \brief  NonStandardPrintFormat is a representation which a TimeOfDay can be transformed into
         *
         *  eCurrentLocale_WithZerosStripped
         *      eCurrentLocale_WithZerosStripped is eCurrentLocale, but with many cases of trailing zero's,
         *      and sometimes leading zeros, stripped, so for example, 01:03:05 PM will become 1:03:05 PM,
         *      and 04:06:00 PM will become 4:06 PM.
         */
        enum class NonStandardPrintFormat : uint8_t {
            eCurrentLocale_WithZerosStripped,

            eDEFAULT = eCurrentLocale_WithZerosStripped,

            Stroika_Define_Enum_Bounds (eCurrentLocale_WithZerosStripped, eCurrentLocale_WithZerosStripped)
        };

    public:
        static constexpr NonStandardPrintFormat eCurrentLocale_WithZerosStripped = NonStandardPrintFormat::eCurrentLocale_WithZerosStripped;

    public:
        /**
         *  For formatPattern, see http://en.cppreference.com/w/cpp/locale/time_put/put
         *  If only formatPattern specified, and no locale, use default (global) locale.
         * 
         * \note if locale is missing (not specified as argument) the default locale (locale{}) is used.
         */
        nonvirtual String Format (NonStandardPrintFormat pf = NonStandardPrintFormat::eDEFAULT) const;
        nonvirtual String Format (const locale& l) const;
        nonvirtual String Format (const locale& l, const String& formatPattern) const;
        nonvirtual String Format (const String& formatPattern) const;

    public:
        /**
         */
        constexpr strong_ordering operator<=> (const TimeOfDay& rhs) const = default;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    private:
        uint32_t fTime_;
    };

    class TimeOfDay::FormatException : public Execution::RuntimeErrorException<> {
    public:
        FormatException ();

    public:
        /**
         */
        static const FormatException kThe;
    };

    inline const TimeOfDay::FormatException TimeOfDay::FormatException::kThe;

    //%t        Any white space.
    //%T        The time as %H : %M : %S. (iso8601 format)
    //%r        is the time as %I:%M:%S %p
    //%M        The minute [00,59]; leading zeros are permitted but not required.
    //%p        Either 'AM' or 'PM' according to the given time value, or the corresponding strings for the current locale. Noon is treated as 'pm' and midnight as 'am'.
    //%P        Like %p but in lowercase: 'am' or 'pm' or a corresponding string for the current locale. (GNU)
    //%S        The seconds [00,60]; leading zeros are permitted but not required.
    inline const Traversal::Iterable<String> TimeOfDay::kDefaultParseFormats{
        kLocaleStandardFormat,
        kLocaleStandardAlternateFormat,
        kISO8601Format,
        L"%r"sv,
        L"%H:%M"sv,
        L"%I%p"sv,
        L"%I%P"sv,
        L"%I%t%p"sv,
        L"%I%t%P"sv,
        L"%I:%M%t%p"sv,
        L"%I:%M%t%P"sv,
        L"%I:%M:%S%t%p"sv,
        L"%I:%M:%S%t%P"sv,
        L"%I:%M:%S"sv,
        L"%I:%M"sv,
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimeOfDay.inl"

#endif /*_Stroika_Foundation_Time_TimeOfDay_h_*/
