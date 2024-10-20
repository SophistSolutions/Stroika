/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTime_h_
#define _Stroika_Foundation_Time_DateTime_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <climits>
#include <compare>
#include <ctime>
#include <locale>
#include <string>

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Time/Common.h"
#include "Stroika/Foundation/Time/Date.h"
#include "Stroika/Foundation/Time/TimeOfDay.h"
#include "Stroika/Foundation/Time/Timezone.h"
#include "Stroika/Foundation/Traversal/Range.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Release">Release</a>
 *
 * TODO:
 *      @todo - http://stroika-bugs.sophists.com/browse/STK-671 - DateTime::Format and Parse () incorrectly handle the format strings %z and %Z (sort of)
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
     *      Date part is always valid/required (use optional <DateTime> to support old DateTime{}.empty() behavior).
     *      TimeOfDay may be "unknown" (missing).
     *      Timezone may be "unknown" (missing), or a Timezone object (@see Timezone).
     *
     *  \note   DateTime constructors REQUIRE valid inputs, and any operations which might overflow throw range_error
     *          instead of creating invalid values.
     *
     *  \note   DateTime values are (optionally) associated with a particular timezone. If the value of the timezone is localtime and
     *          localtime changes, the DateTime then is relative to that new localtime. If the associated timezone is localtime, the
     *          interpretation of that timezone happens at the time a request requires it.
     *
     *  \note   Why no kISO8601Format string and use of enum eISO8601 instead? https://en.cppreference.com/w/cpp/locale/time_put/put may provide
     *          enough information (a little unclear on the Z/timezone part) to WRITE an ISO8601 format string for date time, but 
     *          https://en.cppreference.com/w/cpp/locale/time_get/get doesn't come very close to supporting ISO 8601 format.
     * 
     *  \todo   consider if eRFC1123 could be done as kRFC1123Format; see note about about kISO8601Format/eISO8601, but the same issues
     *          apply (unclear good enuf support for timezones). BUT could reconsider.
     * 
     *  \note   c++20 clocks - and clock_cast etc.
     *          This class roughly corresponds to system_clock. Sadly - for conversions to / from TickCount () - 
     *          clock_cast doesn't work - https://stackoverflow.com/questions/35282308/convert-between-c11-clocks
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
     *          This coercion to GMT can be avoided by optional constructor argument to DateTime::ThreeWayComparer
     */
    class [[nodiscard]] DateTime {
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
         */
        constexpr DateTime (DateTime&& src) noexcept = default;
        constexpr DateTime (const DateTime& src)     = default;
        constexpr explicit DateTime (const Date& d) noexcept;
        constexpr explicit DateTime (const DateTime& dt, const Date& updateDate) noexcept;
        constexpr explicit DateTime (const DateTime& dt, const TimeOfDay& updateTOD) noexcept;
        constexpr DateTime (const Date& date, const optional<TimeOfDay>& timeOfDay, const optional<Timezone>& tz = Timezone::kUnknown) noexcept;
        explicit DateTime (time_t unixEpochTime) noexcept;
        explicit DateTime (const ::tm& tmTime, const optional<Timezone>& tz = Timezone::kUnknown) noexcept;
        explicit DateTime (const ::timespec& tmTime, const optional<Timezone>& tz = Timezone::kUnknown) noexcept;
#if qPlatform_POSIX
        explicit DateTime (const ::timeval& tmTime, const optional<Timezone>& tz = Timezone::kUnknown) noexcept;
#elif qPlatform_Windows
        explicit DateTime (const ::SYSTEMTIME& sysTime, const optional<Timezone>& tz = Timezone::kLocalTime) noexcept;
        explicit DateTime (const ::FILETIME& fileTime, const optional<Timezone>& tz = Timezone::kUTC) noexcept;
#endif
        template <Configuration::ITimePoint T>
        explicit DateTime (T timePoint) noexcept;

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
         *      See https://datatracker.ietf.org/doc/html/rfc3339
         *
         *  eRFC1123:
         *      eRFC1123  is a very old format (same as RFC822 except 4 digit year instead of 2-digit year), but is still used in the 
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
         * 
         *          NOTE - in this example, the -0800 redundantly says the same thing as the PST. With this format, its common
         *          to have junk/comments like that at the end of the date, and so this format - when parsed, will just ignore that stuff
         *          and will allow for extra whitespace in and around the date.
         */
        static constexpr auto kRFC1123Format = LocaleIndependentFormat::eRFC1123;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get and https://en.cppreference.com/w/cpp/locale/time_put/put
         */
        static constexpr string_view kLocaleStandardFormat = "%c"sv;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get and https://en.cppreference.com/w/cpp/locale/time_put/put
         */
        static constexpr string_view kLocaleStandardAlternateFormat = "%Ec"sv;

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
        static constexpr string_view kShortLocaleFormatPattern = "%x %X"sv;

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
         *  \note   @todo - http://stroika-bugs.sophists.com/browse/STK-671 - DateTime::Format and Parse () incorrectly handle the format strings %z and %Z (sort of)
         * 
         *  \note   Handling of extra junk (including whitespace) at the start or end of the date MAY or MAY not be tollerated, depending on the
         *          format parameters given. But generally this API is struct, and will treat junk at the start or end of the date as a format
         *          exception. To get looser interpretation, use ParseQuietly().
         */
        static DateTime Parse (const String& rep, LocaleIndependentFormat format);
        static DateTime Parse (const String& rep, const locale& l = locale{});
        static DateTime Parse (const String& rep, const locale& l, const String& formatPattern);
        static DateTime Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns);
        static DateTime Parse (const String& rep, const String& formatPattern);

    public:
        /**
         *  \brief like Parse(), but returns nullopt on parse error, not throwing exception.
         *
         *  if locale is missing, and formatPattern is not locale independent, the current locale (locale{}) is used.
         *  if rep is empty, this will return nullopt
         * 
         *  if argument consumedCharacters != nullptr, and if ParseQuietly returns has_value (), then *consumedCharacters will contain
         *  the number of characters consumed from rep.
         */
        static optional<DateTime> ParseQuietly (const String& rep, LocaleIndependentFormat format, size_t* consumedCharacters = nullptr);
        static optional<DateTime> ParseQuietly (const String& rep, const String& formatPattern, size_t* consumedCharacters = nullptr);
        static optional<DateTime> ParseQuietly (const String& rep, const locale& l, const String& formatPattern, size_t* consumedCharacters = nullptr);
        static optional<DateTime> ParseQuietly (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns = kDefaultParseFormats,
                                                size_t* consumedCharacters = nullptr);

    private:
        // this rquires rep!= ""
        static optional<DateTime> ParseQuietly_ (const wstring& rep, const time_get<wchar_t>& tmget, const String& formatPattern, size_t* consumedCharacters);

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
         *  \note   @todo - http://stroika-bugs.sophists.com/browse/STK-671 - DateTime::Format and Parse () incorrectly handle the format strings %z and %Z (sort of)
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
        nonvirtual Date::JulianDayNumber DaysSince () const;

    public:
        /**
         * Defined for
         *      time_t
         *      struct tm
         *      struct timespec
         *      SYSTEMTIME                              (WINDOWS ONLY)
         *      Date
         *      String                                  (Format (PrintFormat::eDEFAULT))
         *      chrono::time_point<CLOCK,DURATION>      (satisfies Configuration::ITimePoint)
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
        nonvirtual T As () const
        // new bug define for clang/xcode? cannot do requires and tmeplate specailize?
#if !qCompilerAndStdLib_template_requires_doesnt_work_with_specialization_Buggy
            requires (Configuration::IAnyOf<T, time_t, struct tm, struct timespec, Date, Characters::String> or
#if qPlatform_Windows
                      same_as<T, SYSTEMTIME> or
#endif
                      Configuration::ITimePoint<T>)
#endif
        ;

    public:
        /**
         *  returns the Date part of the DateTime object (in this datetime's timezone).
         */
        nonvirtual constexpr Date GetDate () const noexcept;

    public:
        /**
         *  returns the TimeOfDay part of the DateTime object (in this datetime's timezone). It returns 'missing' if there
         *  is no TimeOfDay associated with this DateTime object.
         *
         *  \ens (not result.has_value () or not result->empty ())
         *
         *  \note Operations on a DateTime which require a 'TimeOfDay' will implicitly create a '0' time of day. So for example,
         *        if you AddSeconds (1) to a DateTime with a missing TimeOfDay it creates a TimeOfDay with value 12:00:01.
         */
        nonvirtual constexpr optional<TimeOfDay> GetTimeOfDay () const noexcept;

    public:
        /**
         *  Add the given amount of time to construct a new DateTime object. This function does NOT change 
         *  the timezone value nor adjust for timezone issues. 
         *
         *  Add doesn't modify *this.
         *
         *  \note This function will throw range_error() if it cannot perform the required conversions and produce a valid value.
         */
        nonvirtual DateTime Add (const Duration& d) const;

    public:
        /**
         *  Add the given number of days to construct a new DateTime object. This function does NOT change 
         *  the timezone value nor adjust for timezone issues. 
         *
         *  AddDays doesn't modify this.
         *
         *  \note This function will throw range_error() if it cannot perform the required conversions and produce a valid value.
         */
        nonvirtual DateTime AddDays (int days) const;

    public:
        /**
         *  Add the given number of seconds to construct a new DateTime object. This function does NOT change 
         *  the timezone value nor adjust for timezone issues. 
         *
         *  AddSeconds doesn't modify this.
         *
         *  \note This function will throw range_error() if it cannot perform the required conversions and produce a valid value.
         *
         *  \note Operations on a DateTime which require a 'TimeOfDay' will implicitly create a '0' time of day. So for example,
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
        template <typename T>
        nonvirtual T As_Simple_ () const
#if !qCompilerAndStdLib_template_requires_doesnt_work_with_specialization_Buggy
            requires (same_as<T, time_t> or same_as<T, struct tm> or same_as<T, struct timespec> or same_as<T, Date> or same_as<T, Characters::String>)
#endif
        ;
#if qPlatform_Windows
        nonvirtual SYSTEMTIME AsSYSTEMTIME_ () const;
#endif
        template <typename CLOCK_T, typename DURATION_T>
        nonvirtual time_point<CLOCK_T, DURATION_T> As_TP_ () const;

    private:
        optional<Timezone> fTimezone_;
        Date               fDate_;
        optional<TimeOfDay> fTimeOfDay_; // for now - still can be 'empty' - but API (as of v2.1d4) disallows passing in or getting out empty TimeOfDay

    public:
        [[deprecated ("Since Stroika v3.0d5  - just use AsLocalTime ().As<Time::TimePointSeconds> ()")]] Time::TimePointSeconds ToTickCount () const
        {
            return AsLocalTime ().As<Time::TimePointSeconds> ();
        }
        [[deprecated ("Since Stroika v3.0d5  - just construct the DateTime with the value of GetTickCount()")]] static DateTime
        FromTickCount (Time::TimePointSeconds tickCount)
        {
            return DateTime{tickCount};
        }
    };

    class DateTime::FormatException : public Execution::RuntimeErrorException<> {
    public:
        FormatException ();

    public:
        /**
         *  just a predefined instance of the FormatException you can use, to avoid creating your own.
         */
        static const FormatException kThe;
    };
    inline const DateTime::FormatException DateTime::FormatException::kThe;

    inline const Traversal::Iterable<String> DateTime::kDefaultParseFormats{
        kLocaleStandardFormat,
        kLocaleStandardAlternateFormat,
        "%x %X"sv,
        "%Ex %EX"sv,
        "%Y-%b-%d %H:%M:%S"sv, // no obvious reference for this so maybe not a good idea
        "%D%t%T"sv,            // no obvious reference for this so maybe not a good idea
        "%D%t%r"sv,            // no obvious reference for this so maybe not a good idea
        "%D%t%R"sv,            // no obvious reference for this so maybe not a good idea
        "%a %b %e %T %Y"sv,    // no obvious reference for this so maybe not a good idea
    };

    /**
     */
    struct DateTime::ThreeWayComparer : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eThreeWayCompare> {
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
    bool NearlyEquals (Time::DateTime l, Time::DateTime r, Time::DurationSeconds epsilon);
}

namespace Stroika::Foundation::Time {
    class Duration;
}

namespace Stroika::Foundation::Traversal::RangeTraits {

    template <>
    struct DefaultOpenness<Time::DateTime> : ExplicitOpenness<Openness::eClosed, Openness::eClosed> {};
    template <>
    struct DefaultDifferenceTypes<Time::DateTime> : ExplicitDifferenceTypes<Time::Duration> {};
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
