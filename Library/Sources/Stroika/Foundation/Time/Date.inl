/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
        return ((146097 * c) >> 2) + ((1461 * ya) >> 2) + (153 * static_cast<int> (month) + 2) / 5 + static_cast<int> (day) + 1721119;
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
        : fJulianDateRep_{julianRep}
    {
        Require ((kMinJulianRep <= julianRep and julianRep <= kMaxJulianRep) or julianRep == kEmptyJulianRep);
    }
    constexpr inline Date::Date (Year year, MonthOfYear month, DayOfMonth day)
        : fJulianDateRep_{jday_ (month, day, year)}
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
    inline Date Date::Parse (const String& rep, const locale& l, const String& formatPattern)
    {
        if (rep.empty ()) {
            Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty Date{}
        }
        wstring                  wRep  = rep.As<wstring> ();
        const time_get<wchar_t>& tmget = use_facet<time_get<wchar_t>> (l);
        if (auto r = ParseQuietly_ (wRep, tmget, formatPattern, nullptr)) {
            return *r;
        }
        Execution::Throw (FormatException::kThe);
    }
    inline Date Date::Parse (const String& rep, const locale& l, const String& formatPattern, size_t* consumedCharsInStringUpTo)
    {
        RequireNotNull (consumedCharsInStringUpTo);
        if (rep.empty ()) {
            Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty Date{}
        }
        wstring                  wRep  = rep.As<wstring> ();
        const time_get<wchar_t>& tmget = use_facet<time_get<wchar_t>> (l);
        if (auto r = ParseQuietly_ (wRep, tmget, formatPattern, consumedCharsInStringUpTo)) {
            return *r;
        }
        Execution::Throw (FormatException::kThe);
    }
    inline Date Date::Parse (const String& rep, const Traversal::Iterable<String>& formatPatterns)
    {
        return Date::Parse_ (rep, locale::classic (), formatPatterns, nullptr);
    }
    inline Date Date::Parse (const String& rep, const Traversal::Iterable<String>& formatPatterns, size_t* consumedCharsInStringUpTo)
    {
        RequireNotNull (consumedCharsInStringUpTo);
        return Date::Parse_ (rep, locale::classic (), formatPatterns, consumedCharsInStringUpTo);
    }
    inline Date Date::Parse (const String& rep, const String& formatPattern)
    {
        if (rep.empty ()) {
            Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty Date{}
        }
        wstring                  wRep  = rep.As<wstring> ();
        const time_get<wchar_t>& tmget = use_facet<time_get<wchar_t>> (locale::classic ());
        if (auto r = ParseQuietly_ (wRep, tmget, formatPattern, nullptr)) {
            return *r;
        }
        Execution::Throw (FormatException::kThe);
    }
    inline Date Date::Parse (const String& rep, const String& formatPattern, size_t* consumedCharsInStringUpTo)
    {
        RequireNotNull (consumedCharsInStringUpTo);
        if (rep.empty ()) {
            Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty Date{}
        }
        wstring                  wRep  = rep.As<wstring> ();
        const time_get<wchar_t>& tmget = use_facet<time_get<wchar_t>> (locale::classic ());
        if (auto r = ParseQuietly_ (wRep, tmget, formatPattern, consumedCharsInStringUpTo)) {
            return *r;
        }
        Execution::Throw (FormatException::kThe);
    }
    inline optional<Date> Date::ParseQuietly (const String& rep, const String& formatPattern)
    {
        return ParseQuietly (rep, locale::classic (), formatPattern);
    }
    inline optional<Date> Date::ParseQuietly (const String& rep, const locale& l, const String& formatPattern)
    {
        if (rep.empty ()) {
            return nullopt;
        }
        return ParseQuietly_ (rep.As<wstring> (), use_facet<time_get<wchar_t>> (l), formatPattern, nullptr);
    }
#if not(qCompilerAndStdLib_linkerLosesInlinesSoCannotBeSeenByDebugger_Buggy && qDebug)
    inline String Date::ToString () const
    {
        return Format ();
    }
#endif
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
    inline constexpr Date Date::kMin{Date::kMinJulianRep};
    inline constexpr Date Date::kMax{UINT_MAX - 1};

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ************************* Time::Date operators *********************************
     ********************************************************************************
     */
    constexpr inline bool operator< (const Date& lhs, const Date& rhs)
    {
        return Common::ThreeWayCompare (lhs.GetJulianRep (), rhs.GetJulianRep ()) < 0;
    }
    constexpr inline bool operator<= (const Date& lhs, const Date& rhs)
    {
        return Common::ThreeWayCompare (lhs.GetJulianRep (), rhs.GetJulianRep ()) <= 0;
    }
    constexpr inline bool operator== (const Date& lhs, const Date& rhs)
    {
        return Common::ThreeWayCompare (lhs.GetJulianRep (), rhs.GetJulianRep ()) == 0;
    }
    constexpr inline bool operator!= (const Date& lhs, const Date& rhs)
    {
        return Common::ThreeWayCompare (lhs.GetJulianRep (), rhs.GetJulianRep ()) != 0;
    }
    constexpr inline bool operator>= (const Date& lhs, const Date& rhs)
    {
        return Common::ThreeWayCompare (lhs.GetJulianRep (), rhs.GetJulianRep ()) >= 0;
    }
    constexpr inline bool operator> (const Date& lhs, const Date& rhs)
    {
        return Common::ThreeWayCompare (lhs.GetJulianRep (), rhs.GetJulianRep ()) > 0;
    }
#endif
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
    constexpr EnumNames<Stroika::Foundation::Time::Date::NonStandardPrintFormat> DefaultNames<Stroika::Foundation::Time::Date::NonStandardPrintFormat>::k{
        EnumNames<Stroika::Foundation::Time::Date::NonStandardPrintFormat>::BasicArrayInitializer{{
            {Stroika::Foundation::Time::Date::NonStandardPrintFormat::eCurrentLocale_WithZerosStripped, L"Current-Locale-With-Zeros-Stripped"},
        }}};

}

namespace Stroika::Foundation::Traversal::RangeTraits {

    inline constexpr Time::Date Default<Time::Date>::kLowerBound{Time::Date::kMin};
    inline constexpr Time::Date Default<Time::Date>::kUpperBound{Time::Date::kMax};

    /// need getNext/GetPrev here excelt if causes constexpr issues
    inline Time::Date Default<Time::Date>::GetNext (Time::Date n)
    {
        return n.AddDays (1);
    }
    inline Time::Date Default<Time::Date>::GetPrevious (Time::Date n)
    {
        return n.AddDays (-1);
    }

};

#endif /*_Stroika_Foundation_Time_Date_inl_*/
