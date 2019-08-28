/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Date_h_
#define _Stroika_Foundation_Time_Date_h_ 1

#include "../StroikaPreComp.h"

#include <climits>
#include <string>

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Configuration/Enumeration.h"
#include "../Execution/Exceptions.h"

#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Complete removeal of deprecated empty and no-arg constructor
 *
 *      @todo   Consider losing eEmptyDayOfMonth and eEmptyMonthOfYear and using optional instead
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

    /**
     *  \note   Configuration::DefaultNames<> supported
     */
    enum class DayOfWeek : uint8_t {
        eSunday    = 0,
        eMonday    = 1,
        eTuesday   = 2,
        eWednesday = 3,
        eThursday  = 4,
        eFriday    = 5,
        eSaturday  = 6,

        Stroika_Define_Enum_Bounds (eSunday, eSaturday)
    };

    /**
     *  \note   Configuration::DefaultNames<> supported
     */
    enum class MonthOfYear : uint8_t {
        eEmptyMonthOfYear = 0, // only zero if date empty
        eJanuary          = 1,
        eFebruary         = 2,
        eMarch            = 3,
        eApril            = 4,
        eMay              = 5,
        eJune             = 6,
        eJuly             = 7,
        eAugust           = 8,
        eSeptember        = 9,
        eOctober          = 10,
        eNovember         = 11,
        eDecember         = 12,

        Stroika_Define_Enum_Bounds (eJanuary, eDecember)
    };
    int operator- (MonthOfYear m1, MonthOfYear m2);

    /**
     */
    enum class DayOfMonth : uint8_t {
        eEmptyDayOfMonth = 0, // only zero if date empty
        e1               = 1,
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
        eEmptyYear = SHRT_MIN,
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
     *      'empty' concept:
     *          *** DEPRECATED ***
     *          Treat it as DISTINCT from any other Date. However, when converting it to a number of seconds
     *          or days (JulienRep), treat empty as Date::kMin. For format routine, return empty string.
     *          And for COMPARIONS (=,<,<=, etc) treat it as LESS THAN Date::kMin. This is a bit like the floating
     *          point concept of negative infinity.
     *
     *  \note   Date constructors REQUIRE valid inputs, and any operations which might overflow throw range_error
     *          instead of creating invalid values.
     *
     *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
     *
     *  \note   Would like to make Date inherit from Debug::AssertExternallySynchronizedLock to assure its not accidentially modified, but
     *          thats difficult beacuse its sometimes uses as a constexpr
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
        static constexpr JulianRepType kEmptyJulianRep = UINT_MAX;

    public:
        class FormatException;

    public:
        /**
         *  the Date/0 CTOR returns an empty date (see @Date::empty ())
         *
         *  \req kMinJulianRep <= julianRep <= kMaxJulianRep OR julianRep == kEmptyJulianRep
         */
        [[deprecated ("Use optional<Date> instead of Date no-arg constructor - as of v2.1d11;")]] constexpr Date ();
        constexpr Date (const Date& src) = default;
        constexpr Date (Date&& src)      = default;
        explicit constexpr Date (JulianRepType julianRep);
        constexpr explicit Date (Year year, MonthOfYear month, DayOfMonth day);

    public:
        /**
        */
        nonvirtual Date& operator= (const Date& rhs) = default;
        nonvirtual Date& operator= (Date&& rhs) = default;

    public:
        /**
         *  \brief  ParseFormat is a representation which a date can be transformed out of
         *
         *  eCurrentLocale
         *      Note this is the current C++ locale, which may not be the same as the platform default locale.
         *      @see Configuration::GetPlatformDefaultLocale, Configuration::UsePlatformDefaultLocaleAsDefaultLocale ()
         *
         *  \note Before Stroika v2.1d11, we supported eXML, but this is defined to be the same as eISO8601, except for supporting
         *        timezones (which we don't support in this class because it wouldn't make sense).
         *
         *  \note   Configuration::DefaultNames<> supported
         */
        enum class ParseFormat : uint8_t {
            eCurrentLocale,
            eISO8601,
            eXML [[deprecated ("since Stroika v2.1d11 - use eISO8601")]],
            eJavascript,

            Stroika_Define_Enum_Bounds (eCurrentLocale, eJavascript)
        };

    public:
        /**
         *  \note sometimes represented as %F (see https://en.cppreference.com/w/cpp/chrono/c/wcsftime), but that's not supported in https://en.cppreference.com/w/cpp/locale/time_get/get.
         *        so equivilent to %Y-%m-%d
         */
        static constexpr wchar_t kISO8601FormatArray[] = L"%Y-%m-%d";
        static const String      kISO8601Format;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get
         */
        static constexpr wchar_t kLocaleStandardFormatArray[] = L"%x";
        static const String      kLocaleStandardFormat;

    public:
        /**
         *  \note https://en.cppreference.com/w/cpp/locale/time_get/get 
         */
        static constexpr wchar_t kLocaleStandardAlternateFormatArray[] = L"%Ex";
        static const String      kLocaleStandardAlternateFormat;

    public:
        /**
         *  Default formats used by Date::Parse () to parse time strings. The first of these - %X, is
         *  the locale-specific time format.
         */
        static const Traversal::Iterable<String> kDefaultParseFormats;

    public:
        /**
         * Note that for the consumedCharsInStringUpTo overload, the consumedCharsInStringUpTo is filled in with the position after the last
         * character read (so before the next character to be read).
         *
         *  \note Parse (... locale) with no formats specified, defaults to parsing with kDefaultParseFormats formats.
         *
         *  \note an empty string produces BadFormat exception (whereas before 2.1d11 it produced an empty Date object (Date {}).
         *
         *  \see https://en.cppreference.com/w/cpp/locale/time_get/get for allowed formatPatterns
         */
        static Date Parse (const String& rep, ParseFormat pf);
        static Date Parse (const String& rep, const locale& l);
        static Date Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns);
        static Date Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns, size_t* consumedCharsInStringUpTo);
        static Date Parse (const String& rep, const locale& l, size_t* consumedCharsInStringUpTo);
#if qPlatform_Windows
        [[deprecated ("Use Locale APIs instead of LCID APIS - https://docs.microsoft.com/en-us/windows/desktop/api/datetimeapi/nf-datetimeapi-getdateformata says Microsoft is migrating toward the use of locale names instead of locale identifiers - Since Stroika v2.1d11")]] static Date Parse (const String& rep, LCID lcid);
#endif

    private:
        static Date Parse_ (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns, size_t* consumedCharsInStringUpTo);

    public:
        /**
         *  Date::min () is the first date this Date class supports representing.
         *  Defined constexpr if compiler supports.
         *
         *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
         */
        static constexpr Date min ();

    public:
        /**
         * Date::max () is the last date this Date class supports representing.
         *
         *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
         */
        static constexpr Date max ();

    public:
        /**
         */
        nonvirtual Year GetYear () const;

    public:
        /**
         */
        nonvirtual MonthOfYear GetMonth () const;

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
         */
        nonvirtual void mdy (MonthOfYear* month, DayOfMonth* day, Year* year) const;

    public:
        /**
         *  \brief  DisplayFormat is a representation which a date can be transformed in and out of
         *
         *  eCurrentLocale
         *      Note this is the current C++ locale, which may not be the same as the platform default locale.
         *      @see Configuration::GetPlatformDefaultLocale, Configuration::UsePlatformDefaultLocaleAsDefaultLocale ()
         *
         *  eCurrentLocale_WithZerosStripped
         *      eCurrentLocale_WithZerosStripped is eCurrentLocale, but with many cases of leading zero's,
         *      stripped, so for example, 03/05/2013 becomes 3/5/2013. This only affects the day/month, and not the
         *      year.
         *
         *  \note Before Stroika v2.1d11, we supported eXML, but this is defined to be the same as eISO8601, except for supporting
         *        timezones (which we don't support in this class because it wouldn't make sense).
         *
         *  \note   Configuration::DefaultNames<> supported
         */
        enum class PrintFormat : uint8_t {
            eCurrentLocale,
            eISO8601,
            eXML [[deprecated ("since Stroika v2.1d11 - use eISO8601")]],
            eJavascript,
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

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    public:
        /**
         * Returns a new Date object based on this Date, with 'dayCount' days added.
         *
         *  In the special case where Date is 'empty' - the starting reference (for adding dayCount)
         *  is DateTime::GetToday ();
         */
        nonvirtual Date AddDays (SignedJulianRepType dayCount) const;

    public:
        /**
         * Note - in the special case of 'empty' - this returns Date::kMin.GetJulianRep ()
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
        nonvirtual Date operator++ (int);

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
        nonvirtual Date operator- (SignedJulianRepType daysOffset) const;

    public:
        struct ThreeWayComparer;

    private:
        constexpr static JulianRepType jday_ (MonthOfYear month, DayOfMonth day, Year year);

    private:
        constexpr static JulianRepType Safe_jday_ (MonthOfYear month, DayOfMonth day, Year year);

    private:
        static Date AsDate_ (const tm& when);

    private:
        JulianRepType fJulianDateRep_;
    };

    class Date::FormatException : public Execution::RuntimeErrorException<> {
    public:
        FormatException ();

    public:
        /**
         */
        static const FormatException kThe;
    };

    /**
     *  @see Common::ThreeWayComparer<> template
     *
     *  @todo https://stroika.atlassian.net/browse/STK-692 - debug threewaycompare/spaceship operator and replicate
     */
    struct Date::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        constexpr int operator() (const Date& lhs, const Date& rhs) const;
    };

    /**
     *  Basic operator overloads with the obivous meaning, and simply indirect to @Common::ThreeWayCompare
     *
     *  @todo https://stroika.atlassian.net/browse/STK-692 - debug threewaycompare/spaceship operator and replicate
     */
    constexpr bool operator< (const Date& lhs, const Date& rhs);
    constexpr bool operator<= (const Date& lhs, const Date& rhs);
    constexpr bool operator== (const Date& lhs, const Date& rhs);
    constexpr bool operator!= (const Date& lhs, const Date& rhs);
    constexpr bool operator>= (const Date& lhs, const Date& rhs);
    constexpr bool operator> (const Date& lhs, const Date& rhs);

    Date::SignedJulianRepType DayDifference (const Date& lhs, const Date& rhs);
    int                       YearDifference (const Date& lhs, const Date& rhs);
    float                     YearDifferenceF (const Date& lhs, const Date& rhs);

    String GetFormattedAge (const optional<Date>& birthDate, const optional<Date>& deathDate = {});                                 // returns ? if not a good src date
    String GetFormattedAgeWithUnit (const optional<Date>& birthDate, const optional<Date>& deathDate = {}, bool abbrevUnit = true); // returns ? if not a good src date

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Date.inl"

#endif /*_Stroika_Foundation_Time_Date_h_*/
