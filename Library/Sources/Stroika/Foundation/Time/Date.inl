/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Date_inl_
#define _Stroika_Foundation_Time_Date_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     ***************************** MonthOfYear **************************************
     ********************************************************************************
     */
    inline int operator- (MonthOfYear m1, MonthOfYear m2)
    {
        return static_cast<int> (m1) - static_cast<int> (m2);
    }

    /*
     ********************************************************************************
     ***************************** DayOfMonth **************************************
     ********************************************************************************
     */
    inline int operator- (DayOfMonth d1, DayOfMonth d2)
    {
        return static_cast<int> (d1) - static_cast<int> (d2);
    }

    /*
     ********************************************************************************
     ************************************ Year **************************************
     ********************************************************************************
     */
    inline int operator- (Year y1, Year y2)
    {
        return static_cast<int> (y1) - static_cast<int> (y2);
    }
    inline Year operator+ (Year y1, int offset)
    {
        return static_cast<Year> (static_cast<int> (y1) + offset);
    }
    template <typename T>
    inline T operator% (Year y1, T m)
    {
        return static_cast<T> (y1) % m;
    }

    /*
     ********************************************************************************
     ********************************* IsLeapYear ***********************************
     ********************************************************************************
     */
    inline bool IsLeapYear (Year y)
    {
        // clang-format off
        if (y % 4 == 0) [[UNLIKELY_ATTR]] {
            if (y % 100 == 0) {
                return y % 400 == 0;
            }
            else {
                return true;
            }
        }
        // clang-format on
        return false;
    }
    inline bool IsLeapYear (int y)
    {
        return IsLeapYear (static_cast<Year> (y));
    }

    /*
     ********************************************************************************
     *************************************** Date ***********************************
     ********************************************************************************
     */
    constexpr inline Date::JulianRepType Date::jday_ (MonthOfYear month, DayOfMonth day, Year year)
    {
        /*
            * Convert Gregorian calendar date to the corresponding Julian day number
            * j.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
            * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
            * This function not valid before that.
            *
            * (This code originally from NIHCL)
            */
        if (month == MonthOfYear::eEmptyMonthOfYear or day == DayOfMonth::eEmptyDayOfMonth or year == Year::eEmptyYear) {
            return Date::kEmptyJulianRep;
        }

        Require (static_cast<int> (year) > 1752 or (static_cast<int> (year) == 1752 and (month > MonthOfYear::eSeptember or (month == MonthOfYear::eSeptember and static_cast<int> (day) >= 14))));

        if (static_cast<int> (month) > 2) {
            month = static_cast<MonthOfYear> (static_cast<int> (month) - 3);
        }
        else {
            month = static_cast<MonthOfYear> (static_cast<int> (month) + 9);
            year  = static_cast<Year> (static_cast<int> (year) - 1);
        }
        Date::JulianRepType c  = static_cast<int> (year) / 100;
        Date::JulianRepType ya = static_cast<int> (year) - 100 * c;
        return (((146097 * c) >> 2) + ((1461 * ya) >> 2) + (153 * static_cast<int> (month) + 2) / 5 + static_cast<int> (day) + 1721119);
    }
    constexpr inline Date::JulianRepType Date::Safe_jday_ (MonthOfYear month, DayOfMonth day, Year year)
    {
        // 'Safe' version just avoids require that date values are legit for julian date range. If date would be invalid - return kEmptyJulianRep.

        if (month == MonthOfYear::eEmptyMonthOfYear or day == DayOfMonth::eEmptyDayOfMonth or year == Year::eEmptyYear) {
            return Date::kEmptyJulianRep;
        }
        if (static_cast<int> (year) > 1752 or (static_cast<int> (year) == 1752 and (month > MonthOfYear::eSeptember or (month == MonthOfYear::eSeptember and static_cast<int> (day) >= 14)))) {
            return jday_ (month, day, year);
        }
        else {
            return Date::kEmptyJulianRep;
        }
    }
    inline constexpr Date::Date (JulianRepType julianRep)
        : fJulianDateRep_ (julianRep)
    {
        Require ((kMinJulianRep <= julianRep and julianRep <= kMaxJulianRep) or julianRep == kEmptyJulianRep);
    }
    constexpr inline Date::Date (Year year, MonthOfYear month, DayOfMonth day)
        : fJulianDateRep_ (jday_ (month, day, year))
    {
        // Gregorian calendar started on Sep. 14, 1752
        Require (year >= Year::eFirstYear);
        Require (year > Year (1752) or (month > MonthOfYear::eSeptember) or (month == MonthOfYear::eSeptember and day >= DayOfMonth (14)));
    }
    inline constexpr Date::JulianRepType Date::GetJulianRep () const
    {
        return fJulianDateRep_ == kEmptyJulianRep ? kMinJulianRep : fJulianDateRep_;
    }
    inline Date Date::Parse (const String& rep, const locale& l)
    {
        return Date::Parse_ (rep, l, kDefaultParseFormats, nullptr);
    }
    inline Date Date::Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns)
    {
        return Date::Parse_ (rep, l, formatPatterns, nullptr);
    }
    inline Date Date::Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns, size_t* consumedCharsInStringUpTo)
    {
        RequireNotNull (consumedCharsInStringUpTo);
        return Date::Parse_ (rep, l, formatPatterns, consumedCharsInStringUpTo);
    }
    inline Date Date::Parse (const String& rep, const locale& l, size_t* consumedCharsInStringUpTo)
    {
        RequireNotNull (consumedCharsInStringUpTo);
        return Date::Parse_ (rep, l, kDefaultParseFormats, consumedCharsInStringUpTo);
    }
    inline String Date::ToString () const
    {
        return Format ();
    }
    inline Date& Date::operator++ ()
    {
        *this = this->AddDays (1);
        return *this;
    }
    inline Date Date::operator++ (int)
    {
        Date tmp = *this;
        *this    = this->AddDays (1);
        return tmp;
    }
    inline Date Date::operator+ (SignedJulianRepType daysOffset) const
    {
        return this->AddDays (daysOffset);
    }
    inline auto Date::Difference (const Date& rhs) const -> SignedJulianRepType
    {
        return fJulianDateRep_ - rhs.fJulianDateRep_;
    }
    inline auto Date::operator- (const Date& rhs) const -> SignedJulianRepType
    {
        return Difference (rhs);
    }
    inline Date Date::operator- (SignedJulianRepType daysOffset) const
    {
        return this->AddDays (-daysOffset);
    }
    inline constexpr Date Date::min ()
    {
        return Date{kMinJulianRep};
    }
    inline constexpr Date Date::max ()
    {
        return Date{UINT_MAX - 1};
    }

    /*
     ********************************************************************************
     ***************************** Date::ThreeWayComparer ***************************
     ********************************************************************************
     */
    constexpr int Date::ThreeWayComparer::operator() (const Date& lhs, const Date& rhs) const
    {
        // careful of signed/unsigned converstions - esp because of kMax which is very large
        JulianRepType l = lhs.GetJulianRep ();
        JulianRepType r = rhs.GetJulianRep ();
        if (l == r) {
            return 0;
        }
        return l < r ? -1 : 1;
    }

    /*
     ********************************************************************************
     ************************* Time::Date operators *********************************
     ********************************************************************************
     */
    constexpr inline bool operator< (const Date& lhs, const Date& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) < 0;
    }
    constexpr inline bool operator<= (const Date& lhs, const Date& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) <= 0;
    }
    constexpr inline bool operator== (const Date& lhs, const Date& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) == 0;
    }
    constexpr inline bool operator!= (const Date& lhs, const Date& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) != 0;
    }
    constexpr inline bool operator>= (const Date& lhs, const Date& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) >= 0;
    }
    constexpr inline bool operator> (const Date& lhs, const Date& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) > 0;
    }
}

namespace Stroika::Foundation::Configuration {
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Stroika::Foundation::Time::DayOfWeek> DefaultNames<Stroika::Foundation::Time::DayOfWeek>::k{
        EnumNames<Stroika::Foundation::Time::DayOfWeek>::BasicArrayInitializer{{
            {Stroika::Foundation::Time::DayOfWeek::eSunday, L"Sunday"},
            {Stroika::Foundation::Time::DayOfWeek::eMonday, L"Monday"},
            {Stroika::Foundation::Time::DayOfWeek::eTuesday, L"Tuesday"},
            {Stroika::Foundation::Time::DayOfWeek::eWednesday, L"Wednesday"},
            {Stroika::Foundation::Time::DayOfWeek::eThursday, L"Thursday"},
            {Stroika::Foundation::Time::DayOfWeek::eFriday, L"Friday"},
            {Stroika::Foundation::Time::DayOfWeek::eSaturday, L"Saturday"},
        }}};
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Stroika::Foundation::Time::MonthOfYear> DefaultNames<Stroika::Foundation::Time::MonthOfYear>::k{
        EnumNames<Stroika::Foundation::Time::MonthOfYear>::BasicArrayInitializer{{
            {Stroika::Foundation::Time::MonthOfYear::eJanuary, L"January"},
            {Stroika::Foundation::Time::MonthOfYear::eFebruary, L"February"},
            {Stroika::Foundation::Time::MonthOfYear::eMarch, L"March"},
            {Stroika::Foundation::Time::MonthOfYear::eApril, L"April"},
            {Stroika::Foundation::Time::MonthOfYear::eMay, L"May"},
            {Stroika::Foundation::Time::MonthOfYear::eJune, L"June"},
            {Stroika::Foundation::Time::MonthOfYear::eJuly, L"July"},
            {Stroika::Foundation::Time::MonthOfYear::eAugust, L"August"},
            {Stroika::Foundation::Time::MonthOfYear::eSeptember, L"September"},
            {Stroika::Foundation::Time::MonthOfYear::eOctober, L"October"},
            {Stroika::Foundation::Time::MonthOfYear::eNovember, L"November"},
            {Stroika::Foundation::Time::MonthOfYear::eDecember, L"December"},
        }}};
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Stroika::Foundation::Time::Date::ParseFormat> DefaultNames<Stroika::Foundation::Time::Date::ParseFormat>::k{
        EnumNames<Stroika::Foundation::Time::Date::ParseFormat>::BasicArrayInitializer{{
            {Stroika::Foundation::Time::Date::ParseFormat::eCurrentLocale, L"Current-Locale"},
            {Stroika::Foundation::Time::Date::ParseFormat::eISO8601, L"ISO-8601"},
            {Stroika::Foundation::Time::Date::ParseFormat::eJavascript, L"Javascript"},
        }}};

#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Stroika::Foundation::Time::Date::PrintFormat> DefaultNames<Stroika::Foundation::Time::Date::PrintFormat>::k{
        EnumNames<Stroika::Foundation::Time::Date::PrintFormat>::BasicArrayInitializer{{
            {Stroika::Foundation::Time::Date::PrintFormat::eCurrentLocale, L"Current-Locale"},
            {Stroika::Foundation::Time::Date::PrintFormat::eISO8601, L"ISO-8601"},
            {Stroika::Foundation::Time::Date::PrintFormat::eJavascript, L"Javascript"},
            {Stroika::Foundation::Time::Date::PrintFormat::eCurrentLocale_WithZerosStripped, L"Current-Locale-With-Zeros-Stripped"},
        }}};

}

#endif /*_Stroika_Foundation_Time_Date_inl_*/
