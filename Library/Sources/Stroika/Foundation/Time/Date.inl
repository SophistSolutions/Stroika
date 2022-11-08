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
     ******************************* DayOfWeek **************************************
     ********************************************************************************
     */
    constexpr DayOfWeek::DayOfWeek (weekday w, DataExchange::ValidationStrategy validationStrategy)
        : weekday{w}
    {
        // stdc++ allows this to contain any number 1..255 (queer)
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not ok ()) {
                Execution::Throw (Date::FormatException::kThe);
            }
        }
        Require (ok ());
    }
    constexpr DayOfWeek::DayOfWeek (int w, DataExchange::ValidationStrategy validationStrategy)
        : weekday{static_cast<unsigned int> (w)}
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (w <= 0 or not ok ()) {
                Execution::Throw (Date::FormatException::kThe);
            }
        }
        Require (ok ());
    }
    constexpr DayOfWeek::DayOfWeek (unsigned int w, DataExchange::ValidationStrategy validationStrategy)
        : DayOfWeek{static_cast<int> (w), validationStrategy}
    {
    }

    /*
     ********************************************************************************
     ******************************* MonthOfYear ************************************
     ********************************************************************************
     */
    constexpr MonthOfYear::MonthOfYear (month m, DataExchange::ValidationStrategy validationStrategy)
        : month{m}
    {
        // stdc++ allows this to contain any number 1..255 (queer)
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not ok ()) {
                Execution::Throw (Date::FormatException::kThe);
            }
        }
        Require (ok ());
    }
    constexpr MonthOfYear::MonthOfYear (int m, DataExchange::ValidationStrategy validationStrategy)
        : month{static_cast<unsigned int> (m)}
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (m <= 0 or not ok ()) {
                Execution::Throw (Date::FormatException::kThe);
            }
        }
        Require (ok ());
    }
    constexpr MonthOfYear::MonthOfYear (unsigned int m, DataExchange::ValidationStrategy validationStrategy)
        : MonthOfYear{static_cast<int> (m), validationStrategy}
    {
    }

    /*
     ********************************************************************************
     ********************************** DayOfMonth **********************************
     ********************************************************************************
     */
    constexpr DayOfMonth::DayOfMonth (day d, DataExchange::ValidationStrategy validationStrategy)
        : day{d}
    {
        // stdc++ allows this to contain any number 1..255 (queer)
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not ok ()) {
                Execution::Throw (Date::FormatException::kThe);
            }
        }
        Require (ok ());
    }
    constexpr DayOfMonth::DayOfMonth (int d, DataExchange::ValidationStrategy validationStrategy)
        : day{static_cast<unsigned int> (d)}
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (d <= 0 or not ok ()) {
                Execution::Throw (Date::FormatException::kThe);
            }
        }
        Require (ok ());
    }
    constexpr DayOfMonth::DayOfMonth (unsigned int d, DataExchange::ValidationStrategy validationStrategy)
        : DayOfMonth{static_cast<int> (d), validationStrategy}
    {
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
        if (y % 4 == 0) [[unlikely]] {
            if (y % 100 == 0) {
                return y % 400 == 0;
            }
            else {
                return true;
            }
        }
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
    constexpr inline Date::JulianRepType Date::jday_ (month m, day d, Year year)
    {
        /*
         * Convert Gregorian calendar date to the corresponding Julian day number
         * j.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
         * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
         * This function not valid before that.
         *
         * (This code originally from NIHCL)
         */
        Require (m.ok ());
        Require (static_cast<int> (year) > 1752 or (static_cast<int> (year) == 1752 and (m > September or (m == September and d >= day{14}))));

        if (static_cast<unsigned int> (m) > 2) {
            m = m - months{3};
        }
        else {
            m    = m + months{9};
            year = static_cast<Year> (static_cast<int> (year) - 1);
        }
        Date::JulianRepType c  = static_cast<int> (year) / 100;
        Date::JulianRepType ya = static_cast<int> (year) - 100 * c;
        return ((146097 * c) >> 2) + ((1461 * ya) >> 2) + (153 * static_cast<unsigned int> (m) + 2) / 5 + static_cast<unsigned int> (d) + 1721119;
    }
    inline constexpr Date::Date (JulianRepType julianRep)
        : fJulianDateRep_{julianRep}
    {
        Require ((kMinJulianRep <= julianRep and julianRep <= kMaxJulianRep));
    }
    inline constexpr Date::Date (JulianRepType julianRep, DataExchange::ValidationStrategy validationStrategy)
        : fJulianDateRep_{julianRep}
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not((kMinJulianRep <= julianRep and julianRep <= kMaxJulianRep))) {
                Execution::Throw (FormatException::kThe);
            }
        }
        Require ((kMinJulianRep <= julianRep and julianRep <= kMaxJulianRep));
    }
    constexpr inline Date::Date (Year year, month m, day d)
        : fJulianDateRep_{jday_ (m, d, year)}
    {
        // Gregorian calendar started on Sep. 14, 1752
        Require (year >= Year::eFirstYear);
        Require (year > Year{1752} or (m > September) or (m == September and d >= DayOfMonth{14}));
    }
    constexpr inline Date::Date (Year year, month m, day d, DataExchange::ValidationStrategy validationStrategy)
        : fJulianDateRep_{0}
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not(year >= Year::eFirstYear)) {
                Execution::Throw (FormatException::kThe);
            }
            if (not(year > Year{1752} or (m > September) or (m == September and d >= DayOfMonth{14}))) {
                Execution::Throw (FormatException::kThe);
            }
        }
        // Gregorian calendar started on Sep. 14, 1752
        Require (year >= Year::eFirstYear);
        Require (year > Year{1752} or (m > September) or (m == September and d >= DayOfMonth{14}));
        fJulianDateRep_ = jday_ (m, d, year);
    }
    inline constexpr Date::JulianRepType Date::GetJulianRep () const
    {
        return fJulianDateRep_;
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
            Execution::Throw (FormatException::kThe);
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
            Execution::Throw (FormatException::kThe);
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
            Execution::Throw (FormatException::kThe);
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
            Execution::Throw (FormatException::kThe);
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

}

namespace Stroika::Foundation::Configuration {
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
