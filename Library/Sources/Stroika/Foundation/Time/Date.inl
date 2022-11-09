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

    enum class [[deprecated ("Since Stroika v3.0d1, unused and use std::chrono")]] DayOfYear : uint16_t{
        eFirstDayOfYear = 1,
        eLastDayOfYear  = 366,

        Stroika_Define_Enum_Bounds (eFirstDayOfYear, eLastDayOfYear)};

    /*
     ********************************************************************************
     ********************************** DayOfWeek ***********************************
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
     ********************************** MonthOfYear *********************************
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
    constexpr Year::Year (year y, DataExchange::ValidationStrategy validationStrategy)
        : year{y}
    {
        // stdc++ allows this to contain any number 1..255 (queer)
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not ok ()) {
                Execution::Throw (Date::FormatException::kThe);
            }
        }
        Require (ok ());
    }
    constexpr Year::Year (int y, DataExchange::ValidationStrategy validationStrategy)
        : year{y}
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (y <= 0 or not ok ()) {
                Execution::Throw (Date::FormatException::kThe);
            }
        }
        Require (ok ());
    }
    constexpr Year::Year (unsigned int y, DataExchange::ValidationStrategy validationStrategy)
        : Year{static_cast<int> (y), validationStrategy}
    {
    }

    /*
     ********************************************************************************
     ********************************* IsLeapYear ***********************************
     ********************************************************************************
     */
    [[deprecated ("Since Stroika v3.0d1, use year{}.is_leap ()")]] inline bool IsLeapYear (Year y)
    {
        return y.is_leap ();
    }
    [[deprecated ("Since Stroika v3.0d1, use year{}.is_leap ()")]] inline bool IsLeapYear (int y)
    {
        return Year{y}.is_leap ();
    }

    /*
     ********************************************************************************
     *************************************** Date ***********************************
     ********************************************************************************
     */
    constexpr inline Date::JulianRepType Date::ToJulianRep (month m, day d, year y, DataExchange::ValidationStrategy validationStrategy)
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not m.ok () or not d.ok () or not y.ok () or y < 1752y or (y == 1752y and (m < September or (m == September and d < 14d)))) {
                Execution::Throw (FormatException::kThe);
            }
        }
        Require (y.ok () and m.ok () and d.ok ());
        Require (static_cast<int> (y) > 1752 or (static_cast<int> (y) == 1752 and (m > September or (m == September and d >= 14d))));
        /*
         * Convert Gregorian calendar date to the corresponding Julian day number
         * j.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
         * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
         * This function not valid before that.
         *
         * (This code originally from NIHCL)
         */

        if (static_cast<unsigned int> (m) > 2) {
            m = m - months{3};
        }
        else {
            m = m + months{9};
            --y;
        }
        Date::JulianRepType c  = static_cast<int> (y) / 100;
        Date::JulianRepType ya = static_cast<int> (y) - 100 * c;
        return ((146097 * c) >> 2) + ((1461 * ya) >> 2) + (153 * static_cast<unsigned int> (m) + 2) / 5 + static_cast<unsigned int> (d) + 1721119;
    }
    constexpr auto Date::ToJulianRep (year_month_day ymd, DataExchange::ValidationStrategy validationStrategy) -> JulianRepType
    {
        return ToJulianRep (ymd.month (), ymd.day (), ymd.year (), validationStrategy);
    }
    constexpr year_month_day Date::FromJulianRep (JulianRepType jr, DataExchange::ValidationStrategy validationStrategy)
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not(kMinJulianRep <= jr and jr <= kMaxJulianRep)) {
                Execution::Throw (FormatException::kThe);
            }
        }
        Require (kMinJulianRep <= jr and jr <= kMaxJulianRep);
        /*
         * Convert a Julian day number to its corresponding Gregorian calendar
         * date.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
         * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
         * This function not valid before that.
         *
         * (This code originally from NIHCL)
         */
        JulianRepType m;
        JulianRepType d;
        JulianRepType y;
        // A reference for this formula (not original I used) - can be found at:
        //      http://aa.usno.navy.mil/faq/docs/JD_Formula.php
        // at least close, and I could probably switch to that...
        //
        JulianRepType j = jr - 1721119;
        y               = (((j << 2) - 1) / 146097);
        j               = (j << 2) - 1 - 146097 * y;
        d               = (j >> 2);
        j               = ((d << 2) + 3) / 1461;
        d               = ((d << 2) + 3 - 1461 * j);
        d               = (d + 4) >> 2;
        m               = (5 * d - 3) / 153;
        d               = 5 * d - 3 - 153 * m;
        d               = (d + 5) / 5;
        y               = (100 * y + j);
        if (m < 10) {
            m += 3;
        }
        else {
            m -= 9;
            ++y;
        }
        return year_month_day{Year{y}, MonthOfYear{m}, DayOfMonth{d}};
    }
    inline constexpr Date::JulianRepType Date::kMinJulianRep = Date::ToJulianRep (September, 14d, 1752y);
    inline constexpr Date::JulianRepType Date::kMaxJulianRep = Date::ToJulianRep (December, 31d, 8099y);
    static_assert (Date::kMinJulianRep == 2361222); // not important, but if that ever failed, would indicate serious bug or we changed definition
    inline constexpr Date::Date (JulianRepType julianRep, DataExchange::ValidationStrategy validationStrategy)
        : fRep_{FromJulianRep (julianRep, validationStrategy)}
    {
    }
    constexpr inline Date::Date (year_month_day ymd, DataExchange::ValidationStrategy validationStrategy)
        : fRep_{ymd}
    {
        (void)ToJulianRep (ymd, validationStrategy); // this is overkill, and we do be slightly more efficient - just for side-effect of checks
    }
    constexpr inline Date::Date (year y, month m, day d, DataExchange::ValidationStrategy validationStrategy)
        : Date{year_month_day{y, m, d}, validationStrategy}
    {
    }
    template <>
    constexpr year_month_day Date::As () const
    {
        return fRep_;
    }
    template <>
    constexpr ::tm Date::As () const
    {
        ::tm tm{};
        tm.tm_year = static_cast<int> (GetYear ()) - kTM_Year_RelativeToYear_;
        tm.tm_mon  = static_cast<unsigned int> (GetMonth ()) - 1;
        tm.tm_mday = static_cast<unsigned int> (GetDayOfMonth ());
        return tm;
    }
    inline constexpr Date::JulianRepType Date::GetJulianRep () const
    {
        return ToJulianRep (fRep_);
    }
    inline constexpr year Date::GetYear () const
    {
        return fRep_.year ();
    }
    inline constexpr month Date::GetMonth () const
    {
        Ensure (January <= fRep_.month () and fRep_.month () <= December);
        return fRep_.month ();
    }
    inline constexpr day Date::GetDayOfMonth () const
    {
        Ensure (1d <= fRep_.day () and fRep_.day () <= 31d);
        return fRep_.day ();
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
    inline Date Date::Add (int dayCount) const
    {
        return Add (days{dayCount});
    }
    inline Date Date::operator+ (int daysOffset) const
    {
        return this->Add (daysOffset);
    }
    inline Date Date::operator+ (days daysOffset) const
    {
        return this->Add (daysOffset);
    }
    inline days Date::Since (Date dStart, Date dEnd)
    {
        if (dStart < dEnd) {
            return dEnd - dStart;
        }
        return days{0};
    }
    inline auto Date::Difference (const Date& rhs) const -> days
    {
        return chrono::sys_days{fRep_} - chrono::sys_days{rhs.fRep_};
    }
    inline auto Date::operator- (const Date& rhs) const -> days
    {
        return Difference (rhs);
    }
    inline Date Date::operator- (days daysOffset) const
    {
        return Add (-daysOffset);
    }
    inline Date Date::operator- (int daysOffset) const
    {
        return Add (-daysOffset);
    }
    inline constexpr Date Date::kMin{Date::kMinJulianRep};
    inline constexpr Date Date::kMax{Date::kMaxJulianRep};

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
        return n + 1;
    }
    inline Time::Date Default<Time::Date>::GetPrevious (Time::Date n)
    {
        return n - 1;
    }

};

#endif /*_Stroika_Foundation_Time_Date_inl_*/
