/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Date_h_
#define _Stroika_Foundation_Time_Date_h_ 1

#include "../StroikaPreComp.h"

#include <chrono>
#include <climits>
#include <compare>
#include <string>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Configuration/Enumeration.h"
#include "../DataExchange/ValidationStrategy.h"
#include "../Execution/Exceptions.h"
#include "../Traversal/Range.h"

#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Could optimize the Format/Parse calls for case without locale to just hardwire implementaton
 *              using sprintf/scanf (as we had before 2.1b10); only performance optimization and unclear it would help
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-668 - Date class should support the full Julian Date Range -
 *              not just Gregorian calendar
 *
 *      @todo   I'm not sure eCurrentLocale_WithZerosStripped is a good idea. Not sure if better
 *              to use separate format print arg or???
 *
 *      @todo   It would be highly desirable to allow this date code to represent lareger/smaller dates
 *              (without the julian calendar restriction).
 *              That maybe a longer term issue.
 *
 *              Consider representing as big struct
 *              o   Like with STRUCT DATETIME or struct tm
 *                  o   Int year
 *                  o   Int month
 *              o   And maybe store cached string reps for common cases as optimization and
 *                  store cached second-offset (mutable) for quick compares
 *              o   Note in docs for future versions the min/max date COULD be expanded
 */

namespace Stroika::Foundation::Time {

    using Characters::String;

    //using namespace chrono;
    using chrono::month;
    using chrono::months;

    using chrono::April;
    using chrono::August;
    using chrono::December;
    using chrono::February;
    using chrono::January;
    using chrono::July;
    using chrono::June;
    using chrono::March;
    using chrono::May;
    using chrono::November;
    using chrono::October;
    using chrono::September;

    using chrono::Friday;
    using chrono::Monday;
    using chrono::Saturday;
    using chrono::Sunday;
    using chrono::Thursday;
    using chrono::Tuesday;
    using chrono::Wednesday;
    using chrono::weekday;

    /**
     *  \brief Simple wrapper on std::chrono::weekday, with some helpful validation properties (assures constructed 'ok'). But not necessary to use - use just 'weekday' in most places
     * 
     *  \note - DayOfWeek was an enum in Stroika v2.1, so this is a significant change.
     */
    struct DayOfWeek : weekday {
        /**
         *  For the purpose of integer constructors, 0==Sunday, 1==Monday, and so on
         */
        constexpr DayOfWeek (weekday w, DataExchange::ValidationStrategy validationStrategy = DataExchange::ValidationStrategy::eAssertion);
        constexpr DayOfWeek (unsigned int w, DataExchange::ValidationStrategy validationStrategy = DataExchange::ValidationStrategy::eAssertion);
        constexpr DayOfWeek (int w, DataExchange::ValidationStrategy validationStrategy = DataExchange::ValidationStrategy::eAssertion);

    public:
        [[deprecated ("Since Stroika v3.0d1, use chrono::Sunday")]] static constexpr weekday    eSunday{Sunday};
        [[deprecated ("Since Stroika v3.0d1, use chrono::Monday")]] static constexpr weekday    eMonday{Monday};
        [[deprecated ("Since Stroika v3.0d1, use chrono::Tuesday")]] static constexpr weekday   eTuesday{Tuesday};
        [[deprecated ("Since Stroika v3.0d1, use chrono::Wednesday")]] static constexpr weekday eWednesday{Wednesday};
        [[deprecated ("Since Stroika v3.0d1, use chrono::Thursday")]] static constexpr weekday  eThursday{Thursday};
        [[deprecated ("Since Stroika v3.0d1, use chrono::Friday")]] static constexpr weekday    eFriday{Friday};
        [[deprecated ("Since Stroika v3.0d1, use chrono::Saturday")]] static constexpr weekday  eSaturday{Saturday};
    };

    /**
     *  \brief Simple wrapper on std::chrono::month, with some helpful validation properties (assures constructed 'ok'). But not necessary to use - use just 'month' in most places
     * 
     *  \note - MonthOfYear was an enum in Stroika v2.1, so this is a significant change.
     */
    struct MonthOfYear : month {
        /**
         *  For the purpose of integer constructors, 1==January, 2==February and so on
         */
        constexpr MonthOfYear (month m, DataExchange::ValidationStrategy validationStrategy = DataExchange::ValidationStrategy::eAssertion);
        constexpr MonthOfYear (unsigned int m, DataExchange::ValidationStrategy validationStrategy = DataExchange::ValidationStrategy::eAssertion);
        constexpr MonthOfYear (int m, DataExchange::ValidationStrategy validationStrategy = DataExchange::ValidationStrategy::eAssertion);

    public:
        [[deprecated ("Since Stroika v3.0d1, use chrono::January")]] static constexpr chrono::month   eJanuary{January};
        [[deprecated ("Since Stroika v3.0d1, use chrono::February")]] static constexpr chrono::month  eFebruary{February};
        [[deprecated ("Since Stroika v3.0d1, use chrono::March")]] static constexpr chrono::month     eMarch{March};
        [[deprecated ("Since Stroika v3.0d1, use chrono::April")]] static constexpr chrono::month     eApril{April};
        [[deprecated ("Since Stroika v3.0d1, use chrono::May")]] static constexpr chrono::month       eMay{May};
        [[deprecated ("Since Stroika v3.0d1, use chrono::June")]] static constexpr chrono::month      eJune{June};
        [[deprecated ("Since Stroika v3.0d1, use chrono::July")]] static constexpr chrono::month      eJuly{July};
        [[deprecated ("Since Stroika v3.0d1, use chrono::August")]] static constexpr chrono::month    eAugust{August};
        [[deprecated ("Since Stroika v3.0d1, use chrono::September")]] static constexpr chrono::month eSeptember{September};
        [[deprecated ("Since Stroika v3.0d1, use chrono::October")]] static constexpr chrono::month   eOctober{October};
        [[deprecated ("Since Stroika v3.0d1, use chrono::November")]] static constexpr chrono::month  eNovember{November};
        [[deprecated ("Since Stroika v3.0d1, use chrono::December")]] static constexpr chrono::month  eDecember{December};
    };

    /**
     */
    enum class DayOfMonth : uint8_t {
        e1 = 1,
        e2,
        e3,
        e4,
        e5,
        e6,
        e7,
        e8,
        e9,
        e10,
        e11 = 11,
        e12,
        e13,
        e14,
        e15,
        e16,
        e17,
        e18,
        e19,
        e20,
        e21 = 21,
        e22,
        e23,
        e24,
        e25,
        e26,
        e27,
        e28,
        e29,
        e30,
        e31,

        Stroika_Define_Enum_Bounds (e1, e31)
    };
    int operator- (DayOfMonth d1, DayOfMonth d2);

    /**
     */
    enum class DayOfYear : uint16_t {
        eFirstDayOfYear = 1,
        eLastDayOfYear  = 366,

        Stroika_Define_Enum_Bounds (eFirstDayOfYear, eLastDayOfYear)
    };

    /**
     */
    enum class Year : short {
        eFirstYear = 1752, // Gregorian calendar started on Sep. 14, 1752.
        eLastfYear = SHRT_MAX - 1,

        Stroika_Define_Enum_Bounds (eFirstYear, eLastfYear)
    };
    int  operator- (Year y1, Year y2);
    Year operator+ (Year y1, int offset);
    template <typename T>
    T operator% (Year y1, T m);

    bool IsLeapYear (Year y);
    bool IsLeapYear (int y);

    /**
     *  \brief this defines undefined but important properties of the %x (read/write date) format string in stdc++ time_get/time_put
     * 
     *  This (rather important detail) appears to be left out of the std c++ specification for date/time formatting, and as a result
     *  each implementation varies. Attempt to capture the actual choices - so at least I can write regression tests that properly
     *  reflect expected Stroika behavior. And - perhaps - avoid the %x format string!
     * 
     *  @todo see if other format strings similarly unreliable. MAYBE the biggest lesson (obvious) is to not count on reading/writing dates
     *  in any locale-defined format (but for UIs this is a tricky constraint)?
     * 
     *  \note - These values all derived empirically (and checked in Stroika regression tests)
     */
    struct StdCPctxTraits {
        static constexpr bool kLocaleClassic_Write4DigitYear = false;
        static constexpr bool kLocaleENUS_Write4DigitYear =
#if defined(_MSC_VER)
            true
#elif defined(_GLIBCXX_RELEASE)
            false
#elif defined(_LIBCPP_VERSION)
            true
#else
            true
#endif
            ;
    };

    /**
     * Description:
     *      The Date class is based on SmallTalk-80, The Language & Its Implementation,
     *      page 108 (apx) - but changed to use gregorian instead of Julian calendar - even though rep date is
     *
     *  @todo CLEANUP / REWRITE THESE DOCS - ROUGH DRAFT BASED ON OLD STROIKA DOCS
     *
     *      o   Date represents a specific data since the start of the Gregorian (1752-09-14).
     *  Class Date knows about some obvious information:
     *      ->  there are seven days in a week, each day having a symbolic name and
     *          an index 1..7
     *      ->  there are twelve months in a year, each having a symbolic name and
     *          an index 1..12.
     *      ->  months have 28..31 days and
     *      ->  a particular year might be a leap year."
     *
     *          NB: Date implies NO NOTION of timezone.
     *
     *  \note   Date constructors REQUIRE valid inputs, and any operations which might overflow throw range_error
     *          instead of creating invalid values.
     *
     *  \note   Would like to make Date inherit from Debug::AssertExternallySynchronizedMutex to assure its not accidentially modified, but
     *          thats difficult beacuse its sometimes uses as a constexpr
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *        o Standard Stroika Comparison support (operator<=>,operator==, etc);
     */
    class Date {
    public:
        using JulianRepType = unsigned int;

    public:
        using SignedJulianRepType = make_signed_t<JulianRepType>;

    public:
        static constexpr JulianRepType kMinJulianRep = 2361222; // This number corresponds to 1752-09-14

    public:
        static constexpr JulianRepType kMaxJulianRep = UINT_MAX - 1;

    public:
        class FormatException;

    public:
        /**
         *  if DataExchange::ValidationStrategy is NOT specified, or == DataExchange::ValidationStrategy::eAssertion, then
         *      \req kMinJulianRep <= julianRep <= kMaxJulianRep
         *  else if eThrow, then throw when arguments out of range.
         */
        constexpr Date (Date&& src) noexcept = default;
        constexpr Date (const Date& src)     = default;
        explicit constexpr Date (JulianRepType julianRep);
        explicit constexpr Date (JulianRepType julianRep, DataExchange::ValidationStrategy validationStrategy);
        constexpr explicit Date (Year year, month m, DayOfMonth day);
        constexpr explicit Date (Year year, month m, DayOfMonth day, DataExchange::ValidationStrategy validationStrategy);

    public:
        /**
         */
        nonvirtual Date& operator= (Date&& rhs) noexcept = default;
        nonvirtual Date& operator= (const Date& rhs)     = default;

    public:
        /**
         *  \brief Y-M-D format - locale independant, and ISO-8601 date format standard
         *
         *  \note sometimes represented as %F (see https://en.cppreference.com/w/cpp/chrono/c/wcsftime), but that's not supported in https://en.cppreference.com/w/cpp/locale/time_get/get.
         *        so equivilent to %Y-%m-%d
         *  \note this is LOCALE-INDEPENDENT
         *  \see kMonthDayYearFormat
         * 
         *  \note also used for XML
         */
        static constexpr wstring_view kISO8601Format = L"%Y-%m-%d"sv;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get
         */
        static constexpr wstring_view kLocaleStandardFormat = L"%x"sv;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get 
         */
        static constexpr wstring_view kLocaleStandardAlternateFormat = L"%Ex"sv;

    public:
        /**
         *  \brief classic (american) month-day-year format, but unlike %D, this uses %Y, so the 4-digit form of year
         * 
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get 
         *  \note This format is LOCALE INDEPENDENT (according to https://en.cppreference.com/w/cpp/locale/time_get/get)
         *  \see kISO8601Format
         */
        static constexpr wstring_view kMonthDayYearFormat = L"%m/%d/%Y"sv;

    public:
        /**
         *  Default formats used by Date::Parse () to parse time strings. The first of these - kLocaleStandardFormat, is
         *  the locale-specific date format.
         */
        static const Traversal::Iterable<String> kDefaultParseFormats;

    public:
        /**
         * Note that for the consumedCharsInStringUpTo overload, the consumedCharsInStringUpTo is filled in with the position after the last
         * character read (so before the next character to be read).
         *
         *  \note Parse (... locale) with no formats specified, defaults to parsing with kDefaultParseFormats formats.
         *
         *  \note if the locale is not specified, its assumed to be the current locale (locale{}))
         *
         *  \note an empty string produces BadFormat exception.
         *
         *  \see https://en.cppreference.com/w/cpp/locale/time_get/get for allowed formatPatterns
         * 
         *  \note when calling Parse with a format string and no locale, the default locale is assumed
         */
        static Date Parse (const String& rep, const locale& l = locale{});
        static Date Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns);
        static Date Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns, size_t* consumedCharsInStringUpTo);
        static Date Parse (const String& rep, const locale& l, size_t* consumedCharsInStringUpTo);
        static Date Parse (const String& rep, const locale& l, const String& formatPattern);
        static Date Parse (const String& rep, const locale& l, const String& formatPattern, size_t* consumedCharsInStringUpTo);
        static Date Parse (const String& rep, const Traversal::Iterable<String>& formatPatterns);
        static Date Parse (const String& rep, const Traversal::Iterable<String>& formatPatterns, size_t* consumedCharsInStringUpTo);
        static Date Parse (const String& rep, const String& formatPattern);
        static Date Parse (const String& rep, const String& formatPattern, size_t* consumedCharsInStringUpTo);

    public:
        /**
         *  \brief like Parse(), but returns nullopt on parse error, not throwing exception.
         * if locale is missing, and formatPattern is not locale independent, the current locale (locale{}) is used.
         *  if rep is empty, this will return nullopt
         */
        static optional<Date> ParseQuietly (const String& rep, const String& formatPattern);
        static optional<Date> ParseQuietly (const String& rep, const locale& l, const String& formatPattern);

    private:
        static optional<Date> ParseQuietly_ (const wstring& rep, const time_get<wchar_t>& tmget, const String& formatPattern, size_t* consumedCharsInStringUpTo);

    private:
        static Date Parse_ (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns, size_t* consumedCharsInStringUpTo);

    public:
        /**
         *  Date::kMin is the first date this Date class supports representing.
         */
        static const Date kMin; // defined constexpr

    public:
        /**
         * Date::kMax is the last date this Date class supports representing.
         */
        static const Date kMax; // defined constexpr

    public:
        /**
         */
        nonvirtual Year GetYear () const;

    public:
        /**
         */
        nonvirtual month GetMonth () const;

    public:
        /**
         */
        nonvirtual DayOfMonth GetDayOfMonth () const;

    public:
        /**
         */
        nonvirtual DayOfWeek GetDayOfWeek () const;

    public:
        /**
         *  \brief Convert (internal representation) Julian day number to its corresponding Gregorian calendar date
         */
        nonvirtual tuple<month, DayOfMonth, Year> mdy () const;

    public:
        /**
         *  \brief  DisplayFormat is a representation which a date can be transformed in and out of
         *
         *  eCurrentLocale_WithZerosStripped
         *      eCurrentLocale_WithZerosStripped is locale{}, but with many cases of leading zero's,
         *      stripped, so for example, 03/05/2013 becomes 3/5/2013. This only affects the day/month, and not the
         *      year.
         *
         *  \note   Configuration::DefaultNames<> supported
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
         */
        nonvirtual String Format (NonStandardPrintFormat pf = NonStandardPrintFormat::eDEFAULT) const;
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
         * Returns a new Date object based on this Date, with 'dayCount' days added.
         */
        nonvirtual Date AddDays (SignedJulianRepType dayCount) const;

    public:
        /**
         */
        nonvirtual constexpr JulianRepType GetJulianRep () const;

    public:
        /**
         * returns number of days since this point - relative to NOW. Never less than zero
         */
        nonvirtual JulianRepType DaysSince () const;

    public:
        /**
         * Returns the difference (*this - rhs) between the two Date records as a SignedJulianRepType.
         */
        nonvirtual SignedJulianRepType Difference (const Date& rhs) const;

    public:
        /**
         *  \brief  Syntactic sure for *this = this->AddDays (1);
         */
        nonvirtual Date& operator++ ();
        nonvirtual Date  operator++ (int);

    public:
        /**
         *  \brief  Syntactic sure for AddDays (n);
         */
        nonvirtual Date operator+ (SignedJulianRepType daysOffset) const;

    public:
        /**
         *  SignedJulianRepType operator- (const Date& rhs): Syntactic sugar on Difference()
         *  Date                operator- (SignedJulianRepType daysOffset)  Syntactic sugar on AddDays(-arg)
         */
        nonvirtual SignedJulianRepType operator- (const Date& rhs) const;
        nonvirtual Date                operator- (SignedJulianRepType daysOffset) const;

    public:
        /**
         */
        constexpr strong_ordering operator<=> (const Date& rhs) const = default;

    public:
        /**
         * Defined for
         *      struct tm
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        [[deprecated ("Since Stroika v3.0d1, use mdy/0")]] void mdy (month* month, DayOfMonth* day, Year* year) const
        {
            RequireNotNull (month);
            RequireNotNull (day);
            RequireNotNull (year);
            auto r = mdy ();
            *month = get<0> (r);
            *day   = get<1> (r);
            *year  = get<2> (r);
        }

    private:
        constexpr static JulianRepType jday_ (month m, DayOfMonth day, Year year);

    private:
        static optional<Date> LocaleFreeParseQuietly_kMonthDayYearFormat_ (const wstring& rep, size_t* consumedCharsInStringUpTo);

    private:
        static Date AsDate_ (const ::tm& when);

    private:
        JulianRepType fJulianDateRep_;
    };

    template <>
    ::tm Date::As () const;

    class Date::FormatException : public Execution::RuntimeErrorException<> {
    public:
        FormatException ();

    public:
        /**
         */
        static const FormatException kThe;
    };
    inline const Date::FormatException       Date::FormatException::kThe;
    inline const Traversal::Iterable<String> Date::kDefaultParseFormats{
        kLocaleStandardFormat,          // x (kLocaleStandardFormat) parses the locale's standard date representation
        kLocaleStandardAlternateFormat, // Ex (kLocaleStandardAlternateFormat) parses the locale's alternative date representation, e.g. expecting 平成23年 (year Heisei 23) instead of 2011年 (year 2011) in ja_JP locale
        kMonthDayYearFormat,            // Before Stroika 2.1b10, this was L"%D" (=="%m/%d/%y) which is the 2-digit year
        kISO8601Format,
    };

    Date::SignedJulianRepType DayDifference (const Date& lhs, const Date& rhs);
    int                       YearDifference (const Date& lhs, const Date& rhs);
    float                     YearDifferenceF (const Date& lhs, const Date& rhs);

    String GetFormattedAge (const optional<Date>& birthDate, const optional<Date>& deathDate = {});                                 // returns ? if not a good src date
    String GetFormattedAgeWithUnit (const optional<Date>& birthDate, const optional<Date>& deathDate = {}, bool abbrevUnit = true); // returns ? if not a good src date

}

namespace Stroika::Foundation::Traversal::RangeTraits {

    template <>
    struct DefaultOpenness<Time::Date> : ExplicitOpenness<Openness::eClosed, Openness::eClosed> {
    };
    template <>
    struct DefaultDifferenceTypes<Time::Date> : ExplicitDifferenceTypes<int, unsigned int> {
    };
    template <>
    struct Default<Time::Date> : ExplicitOpennessAndDifferenceType<Time::Date> {
        static const Time::Date kLowerBound;
        static const Time::Date kUpperBound;

        static Time::Date GetNext (Time::Date n);
        static Time::Date GetPrevious (Time::Date n);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Date.inl"

#endif /*_Stroika_Foundation_Time_Date_h_*/
