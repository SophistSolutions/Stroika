/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTime_h_
#define _Stroika_Foundation_Time_DateTime_h_ 1

#include "../StroikaPreComp.h"

#include <climits>
#include <compare>
#include <ctime>
#include <locale>
#include <string>

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Configuration/Enumeration.h"
#include "../Math/Common.h"

#include "Common.h"
#include "Date.h"
#include "TimeOfDay.h"
#include "Timezone.h"

#include "../Traversal/Range.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>
 *
 * TODO:
 *      @todo - https://stroika.atlassian.net/browse/STK-671 - DateTime::Format and Parse () incorrectly handle the format strings %z and %Z (sort of)
 *
 *      @todo   Support various 64bit int (epoch time) types - even if time_t is 32-bit (such as on AIX).
 *              Be careful about overflow in underlying types like Date and TimeOfDay() however.
 *
 *      @todo   I'm not sure eCurrentLocale_WithZerosStripped is a good idea. Not sure if better
 *              to use separate format print arg or???
 *
 *      @todo   Should we PIN or throw OVERFLOW exception on values/requests which are out of range?
 *
 *      @todo   Future directions consider representing as big struct
 *          o   And maybe store cached string reps for common cases as optimization and
 *              store cached second-offset (mutable) for quick compares
 *          o   Note for timeofday it COULD be enhanced in the future to store TOD as
 *              fractional number of seconds. COULD use LINUX style struct with number of seconds and fixed
 *              point like number of nanoseconds (or some such)
 */

namespace Stroika::Foundation::Time {

    using Characters::String;

    class Duration; // forward declare for Difference ()

    /*
     *  Description:
     *
     *      DateTime is more than just a combination of Date, and Time. It also introduces the notion of TIMEZONE.
     *
     *      Timezone may be "unknown" (missing), or a Timezone object (@see Timezone).
     *
     *  \note   DateTime constructors REQUIRE valid inputs, and any operations which might overflow throw range_error
     *          instead of creating invalid values.
     *
     *  \note   DateTime values are (optionally) associated with a particular timezone. If the value of the timezone is localetime and
     *          localtime changes, the DateTime then is relative to that new localetime. If the associated timezone is localtime, the
     *          interpretation of that timezone happens at the time a request requires it.
     *
     *  \note   Why no kISO8601Format string and use of enum eISO8601 instead? https://en.cppreference.com/w/cpp/locale/time_put/put may provide
     *          enough information (a little unclear on the Z/timezone part) to WRITE an ISO8601 format string for date time, but 
     *          https://en.cppreference.com/w/cpp/locale/time_get/get doesnt come very close to suporting ISO 8601 format.
     * 
     *  \todo   consider if eRFC1123 could be done as kRFC1123Format; see note about about kISO8601Format/eISO8601, but the same issues
     *          apply (unclear good enuf support for timezones). BUT could reconsider.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *        o Standard Stroika Comparison support (operator<=>,operator==, etc);
     *
     *          Also note - if the datetimes differ in their GetTimeZone() value, they are not necessarily 
     *          considered different. If either one is unknown, they will both be treated as the same timezone. 
     *          Otherwise, they will BOTH be converted to GMT, and compared as GMT.
     *
     *          This coertion to GMT can be avoided by optional constructor argument to DateTime::ThreeWayComparer
     */
    class DateTime {
    public:
        /**
         *  Construct a DateTime record with the given date and time value. Presume that these values
         *  apply to the given (argument) timezone. This does not adjust the value of date/time for the given tz -
         *  but just records that this datetime refers to the given timezone.
         *
         *  To change TO a target timezone, use AsUTC () or AsLocalTime ().
         *
         *  explicit DateTime (time_t unixEpochTime) noexcept
         *      Creates a DateTime object in UTC, using UNIX Epoch time.
         *
         *  explicit DateTime (const FILETIME& fileTime, const optional<Timezone>& tz = Timezone::UTC ()) noexcept;
         *      Most windows APIs return filetimes in UTC (or so it appears). Because of this,
         *      our default interpretation of a FILETIME structure as as UTC.
         *      Call DateTime (ft).AsLocalTime () to get the value returned in local time.
         *
         *  \note DateTime (time_t unixEpochTime) returns a datetime in UTC
         *
         *  \note All DateTime constructors REQUIRE valid (in range) arguments.
         *
         *  \note TimeOfDay arguments *must* not be 'empty' - instead use optional<TimeOfDay> {nullopt} overload (since Stroika 2.1d4)
         */
        constexpr DateTime (DateTime&& src) noexcept = default;
        constexpr DateTime (const DateTime& src)     = default;
        constexpr DateTime (const Date& d) noexcept;
        constexpr DateTime (const DateTime& dt, const Date& updateDate) noexcept;
        constexpr DateTime (const DateTime& dt, const TimeOfDay& updateTOD) noexcept;
        constexpr DateTime (const Date& date, const optional<TimeOfDay>& timeOfDay, const optional<Timezone>& tz = Timezone::kUnknown) noexcept;
        constexpr DateTime (const Date& date, const TimeOfDay& timeOfDay, const optional<Timezone>& tz = Timezone::kUnknown) noexcept;
        explicit DateTime (time_t unixEpochTime) noexcept;
        explicit DateTime (const ::tm& tmTime, const optional<Timezone>& tz = Timezone::kUnknown) noexcept;
        explicit DateTime (const ::timespec& tmTime, const optional<Timezone>& tz = Timezone::kUnknown) noexcept;
#if qPlatform_POSIX
        explicit DateTime (const ::timeval& tmTime, const optional<Timezone>& tz = Timezone::kUnknown) noexcept;
#elif qPlatform_Windows
        explicit DateTime (const ::SYSTEMTIME& sysTime, const optional<Timezone>& tz = Timezone::kLocalTime) noexcept;
        explicit DateTime (const ::FILETIME& fileTime, const optional<Timezone>& tz = Timezone::kUTC) noexcept;
#endif

    public:
        /**
         */
        nonvirtual DateTime& operator= (DateTime&& rhs) noexcept = default;
        nonvirtual DateTime& operator= (const DateTime& rhs)     = default;

    public:
        /**
         *  \brief  LocaleIndependentFormat is a representation which a datetime can be transformed to & from
         *
         *  eISO8601:
         *      This is the best (IMHO) preferred format for DateTime objects. Its simple, readable, 
         *      sort order works naturally (alphabetical == by time) and its probably the most widely used
         *      portable date format.
         *
         *  eRFC1123:
         *      eRFC1123  is a very old format (same as RFC822 except 4 digit year intead of 2-digit year), but is still used in the 
         *      current HTTP specification (e.g. for cookies).
         *      The spec is originally documented in 
         *          https://tools.ietf.org/html/rfc1123#5.2.14
         *          https://tools.ietf.org/html/rfc822#section-5
         *      EXAMPLE:  
         *          Tue, 6 Nov 2018 06:25:51 -0800 (PST)
         */
        enum class LocaleIndependentFormat {
            eISO8601,
            eRFC1123,

            Stroika_Define_Enum_Bounds (eISO8601, eRFC1123)
        };

    public:
        /**
         *      This is the best (IMHO) preferred format for DateTime objects. Its simple, readable, 
         *      sort order works naturally (alphabetical == by time) and its probably the most widely used
         *      portable date format.
         */
        static constexpr auto kISO8601Format = LocaleIndependentFormat::eISO8601;

    public:
        /**
         *      RFC1123  is a very old format (same as RFC822 except 4 digit year intead of 2-digit year), but is still used in the 
         *      current HTTP specification (e.g. for cookies).
         *      The spec is originally documented in 
         *          https://tools.ietf.org/html/rfc1123#5.2.14
         *          https://tools.ietf.org/html/rfc822#section-5
         *      EXAMPLE:  
         *          Tue, 6 Nov 2018 06:25:51 -0800 (PST)
         */
        static constexpr auto kRFC1123Format = LocaleIndependentFormat::eRFC1123;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get and https://en.cppreference.com/w/cpp/locale/time_put/put
         */
        static constexpr wstring_view kLocaleStandardFormat = L"%c"sv;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get and https://en.cppreference.com/w/cpp/locale/time_put/put
         */
        static constexpr wstring_view kLocaleStandardAlternateFormat = L"%Ec"sv;

    public:
        /**
         *  Format patterns defined in:
         *      https://en.cppreference.com/w/cpp/locale/time_get/get
         *      https://en.cppreference.com/w/cpp/locale/time_put/put
         *
         *  The default is locale-dependent DATE format followed by space and locale-dependent TIME.
         *
         *  Note - there were two good choices here (all locale dependent):
         *      o   %c                  - writes standard date and time string
         *      o   %x %X               - writes localized date representation / writes localized time representation
         * 
         *  \note   VERY UNRELIABLE for year handling - @see Time::StdCPctxTraits
         */
        static constexpr wstring_view kShortLocaleFormatPattern = L"%x %X"sv;

    public:
        /**
         *  Format patterns defined in:
         *      https://en.cppreference.com/w/cpp/locale/time_get/get
         *      https://en.cppreference.com/w/cpp/locale/time_put/put
         *
         *  Default formats used by Date::Parse () to parse DateTime strings. The first of these - %c and %x %X, are
         *  locale-specific time formats.
         */
        static const Traversal::Iterable<String> kDefaultParseFormats;

    public:
        class FormatException;

    public:
        /**
         *  Parse will throw if the argument cannot be parsed as a valid DateTime.
         *
         *  \note If the timezone cannot be identified in the source string, it will be returned as 'unknown'.
         * 
         *  \note for Parse () overloads which don't specify a locale, the default locale (locale{}) is used (unless the formatPattern is locale independent)
         *
         *  For formatPattern, see https://en.cppreference.com/w/cpp/locale/time_get/get, and defaults to kLocaleStandardFormat
         *
         *  \note A locale has no associated timezone (despite somewhat confusing documentation relating to this).
         *        @see https://stackoverflow.com/questions/52839648/does-a-c-locale-have-an-associated-timezone-and-if-yes-how-do-you-access-it
         *
         *  \note an empty string produces BadFormat exception (whereas before 2.1d11 it produced an empty DateTime object (DateTime {}).
         *
         *  \note   @todo - https://stroika.atlassian.net/browse/STK-671 - DateTime::Format and Parse () incorrectly handle the format strings %z and %Z (sort of)
         */
        static DateTime Parse (const String& rep, LocaleIndependentFormat format);
        static DateTime Parse (const String& rep, const locale& l = locale{});
        static DateTime Parse (const String& rep, const locale& l, const String& formatPattern);
        static DateTime Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns);
        static DateTime Parse (const String& rep, const String& formatPattern);

    public:
        /**
         *  \brief like Parse(), but returns nullopt on parse error, not throwing exception.
         * if locale is missing, and formatPattern is not locale independent, the current locale (locale{}) is used.
         *  if rep is empty, this will return nullopt
         */
        static optional<DateTime> ParseQuietly (const String& rep, LocaleIndependentFormat format);
        static optional<DateTime> ParseQuietly (const String& rep, const String& formatPattern);
        static optional<DateTime> ParseQuietly (const String& rep, const locale& l, const String& formatPattern);

    private:
        // this rquires rep!= ""
        static optional<DateTime> ParseQuietly_ (const wstring& rep, const time_get<wchar_t>& tmget, const String& formatPattern);

    public:
        /**
         *  Return the current DateTime (in LocalTime)
         */
        static DateTime Now () noexcept;

    public:
        /**
         *  Return the current Date (in LocalTime - local timezone)
         */
        static Date GetToday () noexcept;

    public:
        /**
         *  DateTime::kMin is the first date this DateTime class supports representing.
         */
        static const DateTime kMin; // defined constexpr

    public:
        /**
         * DateTime::kMax is the last date this DateTime class supports representing.
         */
        static const DateTime kMax; // defined constexpr

    public:
        /**
         */
        nonvirtual constexpr optional<Timezone> GetTimezone () const noexcept;

    public:
        /** 
         *  Creates a new DateTime object known to be in localtime. If this DateTime timezone is unknown, then the
         *  the datetime will be just assumed to have been in localtime.
         *
         *  \ens result.GetTimezone () == Timezone::LocalTime ()
         */
        nonvirtual DateTime AsLocalTime () const;

    public:
        /**
         *  Creates a new DateTime object known to be in UTC. If this DateTime timezone is unknown, then the
         *  the datetime will be just assumed to have been in UTC.
         *
         *  Either way, this will always produce a result in the timezone UTC.
         *
         *  \ens result.GetTimezone () == Timezone::UTC ()
         *
         *  @see AsTimezone ()
         */
        nonvirtual DateTime AsUTC () const;

    public:
        /**
         *  Return this DateTime converted to the given timezone. If the current timezone is unknown, this just
         *  sets the timezone. But if it is known, the time value is adjusted to correspond for the given timezone.
         *
         *  Either way, this will always produce a result in the timezone tz.
         *
         *  \ens result.GetTimezone () == Timezone::UTC ()
         */
        nonvirtual DateTime AsTimezone (Timezone tz) const;

    public:
        /**
         *  Convert the given datetime to a floating point offset in seconds based on the same reference as the tickcount.
         *
         *  \note   Time::GetTickCount () normally returns a positive number but if you go back in time before
         *          its zero point, ToTickCount () will return negative numbers.
         *
         *  \par Example Usage
         *      \code
         *          Assert (Math::NearlyEquals (Time::GetTickCount (), DateTime::Now ().ToTickCount ());    // roughly true
         *      \endcode
         *
         *  @see FromTickCount
         *
         *  \note - if this DateTime is out of range with respect to TickCount, then this function will throw a range_error.
         */
        nonvirtual DurationSecondsType ToTickCount () const;

    public:
        /**
         *  This is like a constructor, but with a more specific static name to avoid confusion with overloads.
         *  This returns a datetime in localtime.
         *
         *  @see ToTickCount
         *
         *  This function Requires a valid tickCount argument (in range - so meaning > 0)
         */
        static DateTime FromTickCount (DurationSecondsType tickCount);

    public:
        /**
         *  return true if known true, and false if known false, and {} otherwise.
         */
        nonvirtual optional<bool> IsDaylightSavingsTime () const;

    public:
        /**
         *  \brief  NonStandardPrintFormat is a representation which a datetime can be transformed into
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
         *  For formatPattern, see http://en.cppreference.com/w/cpp/locale/time_put/put and defaults to kLocaleStandardFormat
         *  If only formatPattern specified, and no locale, use default (global) locale.
         *
         *  \note A locale has no associated timezone (despite somewhat confusing documentation relating to this).
         *        @see https://stackoverflow.com/questions/52839648/does-a-c-locale-have-an-associated-timezone-and-if-yes-how-do-you-access-it
         * 
         *  \note for apis with the locale not specified, its assumed to be the default (locale{}), except (perhaps) where the formatString is locale-independent.
         * 
         *  \note the default for Format() with no arguments is to use the default locale, but with the eCurrentLocale_WithZerosStripped flag set.
         *
         *  \note   @todo - https://stroika.atlassian.net/browse/STK-671 - DateTime::Format and Parse () incorrectly handle the format strings %z and %Z (sort of)
         */
        nonvirtual String Format (NonStandardPrintFormat pf = NonStandardPrintFormat::eDEFAULT) const;
        nonvirtual String Format (LocaleIndependentFormat format) const;
        nonvirtual String Format (const locale& l) const;
        nonvirtual String Format (const locale& l, const String& formatPattern) const;
        nonvirtual String Format (const String& formatPattern) const;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    public:
        /**
         *  Returns number of days since this point - relative to NOW. Never less than zero.
         */
        nonvirtual Date::JulianRepType DaysSince () const;

    public:
        /**
         * Defined for
         *      time_t
         *      struct tm
         *      struct timespec
         *      SYSTEMTIME          (WINDOWS ONLY)
         *      Date
         *      String              (Format (PrintFormat::eDEFAULT))
         *
         *  NB: Intentionally NOT defined for TimeOfDay () - cuz it wouldn't make sense. A DateTime IS a Date, but its not a TimeOfDay. Time of day just
         *  logically extends Date with extra (TOD) information.
         *
         *  \note  As<time_t> Returns seconds since midnight 1970 UTC (UNIX 'Epoch time')
         *
         *  \note  As<struct tm> Returns a struct tm record without paying attention to timezone (in whatever timezone *this is) - but will throw if the date is before 1900
         *
         *  \note  As<struct timespec> inherits the limitations (UTC since 1970 - since it uses time_t internally)
         *
         *  \note  As<Date> ignores timezone, and returns the date object in this DateTime's timezone
         *
         *  \note As<> will throw range_error() if it cannot perform the required conversions and produce a valid value.
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        /**
         *  returns the Date part of the DateTime object (in this datetime's timezone).
         */
        nonvirtual constexpr Date GetDate () const noexcept;

    public:
        /**
         *  returns the TimeOfDay part of the DateTime object (in this datetime's timezone). It returns 'missing' if there
         *  is no timeofday associated with this DateTime object.
         *
         *  \ens (not result.has_value () or not result->empty ())
         *
         *  \note Operations on a DateTime which require a 'TimeOfDay' will implicity create a '0' time of day. So for example,
         *        if you AddSeconds (1) to a DateTime with a missing TimeOfDay it creates a TimeOfDay with value 12:00:01.
         */
        nonvirtual constexpr optional<TimeOfDay> GetTimeOfDay () const noexcept;

    public:
        /**
         *  Add the given amount of time to construct a new DateTime object. This funtion does NOT change 
         *  the timezone value nor adjust for timezone issues. 
         *
         *  Add doesn't modify *this.
         *
         *  \note This function will throw range_error() if it cannot perform the required conversions and produce a valid value.
         */
        nonvirtual DateTime Add (const Duration& d) const;

    public:
        /**
         *  Add the given number of days to construct a new DateTime object. This funtion does NOT change 
         *  the timezone value nor adjust for timezone issues. 
         *
         *  AddDays doesn't modify this.
         *
         *  \note This function will throw range_error() if it cannot perform the required conversions and produce a valid value.
         */
        nonvirtual DateTime AddDays (int days) const;

    public:
        /**
         *  Add the given number of seconds to construct a new DateTime object. This funtion does NOT change 
         *  the timezone value nor adjust for timezone issues. 
         *
         *  AddSeconds doesn't modify this.
         *
         *  \note This function will throw range_error() if it cannot perform the required conversions and produce a valid value.
         *
         *  \note Operations on a DateTime which require a 'TimeOfDay' will implicity create a '0' time of day. So for example,
         *        if you AddSeconds (1) to a DateTime with a missing TimeOfDay it creates a TimeOfDay with value 12:00:01.
         */
        nonvirtual DateTime AddSeconds (int64_t seconds) const;

    public:
        /**
         *  Returns the difference between the two DateTime records. This can then be easily converted to 
         *  seconds, or days, or whatever.
         *
         *  If the DateTime objects have different timezones, they are converted to a common timezone before the difference
         *  is computed.
         */
        nonvirtual Duration Difference (const DateTime& rhs) const;

    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const DateTime& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const DateTime& rhs) const;

    public:
        struct ThreeWayComparer;

    private:
        optional<Timezone>  fTimezone_;
        Date                fDate_;
        optional<TimeOfDay> fTimeOfDay_; // for now - still can be 'empty' - but API (as of v2.1d4) disallows passing in or getting out empty TimeOfDay
    };

    class DateTime::FormatException : public Execution::RuntimeErrorException<> {
    public:
        FormatException ();

    public:
        /**
         */
        static const FormatException kThe;
    };
    inline const DateTime::FormatException DateTime::FormatException::kThe;

    template <>
    time_t DateTime::As () const;
    template <>
    tm DateTime::As () const;
    template <>
    timespec DateTime::As () const;
#if qPlatform_Windows
    template <>
    SYSTEMTIME DateTime::As () const;
#endif
    template <>
    Date DateTime::As () const;

    inline const Traversal::Iterable<String> DateTime::kDefaultParseFormats{
        kLocaleStandardFormat,
        kLocaleStandardAlternateFormat,
        L"%x %X"sv,
        L"%Ex %EX"sv,
        L"%Y-%b-%d %H:%M:%S"sv, // no obvious reference for this so maybe not a good idea
        L"%D%t%T"sv,            // no obvious reference for this so maybe not a good idea
        L"%D%t%r"sv,            // no obvious reference for this so maybe not a good idea
        L"%D%t%R"sv,            // no obvious reference for this so maybe not a good idea
        L"%a %b %e %T %Y"sv,    // no obvious reference for this so maybe not a good idea
    };

    /**
     */
    struct DateTime::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        /**
         */
        constexpr ThreeWayComparer (bool coerceToCommonTimezone = true);

        /**
         */
        nonvirtual strong_ordering operator() (const DateTime& lhs, const DateTime& rhs) const;
        bool                       fCoerceToCommonTimezone;
    };

    /**
     *  Syntactic sugar on Add()
     */
    DateTime operator+ (const DateTime& lhs, const Duration& rhs);

    /**
     *  Syntactic sugar on Difference()
     */
    Duration operator- (const DateTime& lhs, const DateTime& rhs);

    /**
     *  Syntactic sugar on Add() or Difference()
     */
    DateTime operator- (const DateTime&, const Duration& rhs);
    Duration operator- (const DateTime&, const DateTime& rhs);

}

namespace Stroika::Foundation::Math {
    bool NearlyEquals (Time::DateTime l, Time::DateTime r);
    bool NearlyEquals (Time::DateTime l, Time::DateTime r, Time::DurationSecondsType epsilon);
    bool NearlyEquals (Time::DateTime l, Time::DateTime r, const Time::Duration& epsilon);
}

namespace Stroika::Foundation::Time {
    class Duration;
}

namespace Stroika::Foundation::Traversal::RangeTraits {

    template <>
    struct DefaultOpenness<Time::DateTime> : ExplicitOpenness<Openness::eClosed, Openness::eClosed> {
    };
    template <>
    struct DefaultDifferenceTypes<Time::DateTime> : ExplicitDifferenceTypes<Time::Duration> {
    };
    /**
     *  \note   Default<Time::DateTime> properties (kLowerBound/kUpperBound) can only be used after static initialization, and before
     *          static de-initialization.
     */
    template <>
    struct Default<Time::DateTime> : ExplicitOpennessAndDifferenceType<Time::DateTime> {
        static const Time::DateTime kLowerBound;
        static const Time::DateTime kUpperBound;

        // @todo - not important (just used for PinValue I think)
        // static Time::DateTime GetNext (Time::DateTime n);
        // static Time::DateTime GetPrevious (Time::DateTime n);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DateTime.inl"

#endif /*_Stroika_Foundation_Time_DateTime_h_*/
