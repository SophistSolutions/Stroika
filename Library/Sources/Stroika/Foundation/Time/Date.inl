/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
        eFirstDayOfYear = 1, eLastDayOfYear = 366,

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
    constexpr DayOfWeek::DayOfWeek (unsigned int w, DataExchange::ValidationStrategy validationStrategy)
        : weekday{w}
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not ok ()) {
                Execution::Throw (Date::FormatException::kThe);
            }
        }
        Require (ok ());
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
    constexpr MonthOfYear::MonthOfYear (unsigned int m, DataExchange::ValidationStrategy validationStrategy)
        : month{m}
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not ok ()) {
                Execution::Throw (Date::FormatException::kThe);
            }
        }
        Require (ok ());
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
    constexpr DayOfMonth::DayOfMonth (unsigned int d, DataExchange::ValidationStrategy validationStrategy)
        : day{d}
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not ok ()) {
                Execution::Throw (Date::FormatException::kThe);
            }
        }
        Require (ok ());
    }

    /*
     ********************************************************************************
     ************************************ Year **************************************
     ********************************************************************************
     */
    constexpr Year::Year (year y, DataExchange::ValidationStrategy validationStrategy)
        : year{y}
    {
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
            if (not ok ()) {
                Execution::Throw (Date::FormatException::kThe);
            }
        }
        Require (ok ());
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
    constexpr inline Date::JulianDayNumber Date::ToJulianRep (month m, day d, year y, DataExchange::ValidationStrategy validationStrategy)
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not m.ok () or not d.ok () or not y.ok () or y / m / d < kMinDateReference.fYMD) {
                Execution::Throw (FormatException::kThe);
            }
        }
        Require (y.ok () and m.ok () and d.ok ());
        Require (y / m / d >= kMinDateReference.fYMD);

        // @todo https://stroika.atlassian.net/browse/STK-976 - improve precision of map to/from JulianDateRep

        JulianDayNumber result = [&] () noexcept -> JulianDayNumber {
            if (y / m / d >= kGregorianCalendarEpoch.fYMD) {
                return static_cast<JulianDayNumber> ((chrono::sys_days{y / m / d} - chrono::sys_days{kGregorianCalendarEpoch.fYMD}).count ()) +
                       kGregorianCalendarEpoch.fJulianRep;
            }
            return static_cast<JulianDayNumber> ((chrono::sys_days{y / m / d} - chrono::sys_days{kStartOfJulianCalendar.fYMD}).count ()) +
                   kStartOfJulianCalendar.fJulianRep;
        }();

        if (not is_constant_evaluated ()) {
            [[maybe_unused]] JulianDayNumber stroikav21Algorithm = [&] () noexcept -> JulianDayNumber {
                // Do arithmatic as integer representations, not 'datetime' reps that do funny things like wrap
                // using m - months, etc..
                unsigned int mm{m};
                unsigned int dd{d};
                int          yy{y};
                /*
                 * Convert Gregorian calendar date to the corresponding Julian day number
                 * j.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
                 * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
                 * This function not valid before that.
                 *
                 * (This code originally from NIHCL)
                 */
                if (mm > 2) {
                    mm = mm - 3;
                }
                else {
                    mm = mm + 9;
                    --yy;
                }
                Date::JulianDayNumber c  = yy / 100;
                Date::JulianDayNumber ya = yy - 100 * c;
                return ((146097 * c) >> 2) + ((1461 * ya) >> 2) + (153 * mm + 2) / 5 + dd + 1721119;
            }();
            Assert (result == stroikav21Algorithm or (y / m / d < kGregorianCalendarEpoch.fYMD));
        }
        return result;
    }
    constexpr auto Date::ToJulianRep (year_month_day ymd, DataExchange::ValidationStrategy validationStrategy) -> JulianDayNumber
    {
        return ToJulianRep (ymd.month (), ymd.day (), ymd.year (), validationStrategy);
    }
    constexpr year_month_day Date::FromJulianRep (JulianDayNumber jr, DataExchange::ValidationStrategy validationStrategy)
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not(kMinJulianRep <= jr and jr <= kMaxJulianRep)) {
                Execution::Throw (FormatException::kThe);
            }
        }
        Require (kMinJulianRep <= jr and jr <= kMaxJulianRep);

        // @todo https://stroika.atlassian.net/browse/STK-976 - improve precision of map to/from JulianDateRep
        year_month_day result = [&] () noexcept -> year_month_day {
            if (jr >= kGregorianCalendarEpoch.fJulianRep) {
                return chrono::sys_days{kGregorianCalendarEpoch.fYMD} + days{(jr - kGregorianCalendarEpoch.fJulianRep)};
            }
            return chrono::sys_days{kStartOfJulianCalendar.fYMD} + days{(jr - kStartOfJulianCalendar.fJulianRep)};
        }();

        if (not is_constant_evaluated ()) {
            [[maybe_unused]] year_month_day legacyValue = [&] () {
                /*
                 * Convert a Julian day number to its corresponding Gregorian calendar
                 * date.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
                 * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
                 * This function not valid before that.
                 *
                 * (This code originally from NIHCL)
                 */
                JulianDayNumber m;
                JulianDayNumber d;
                JulianDayNumber y;
                // A reference for this formula (not original I used) - can be found at:
                //      http://aa.usno.navy.mil/faq/docs/JD_Formula.php
                // at least close, and I could probably switch to that...
                //
                JulianDayNumber j = jr - 1721119;
                y                 = (((j << 2) - 1) / 146097);
                j                 = (j << 2) - 1 - 146097 * y;
                d                 = (j >> 2);
                j                 = ((d << 2) + 3) / 1461;
                d                 = ((d << 2) + 3 - 1461 * j);
                d                 = (d + 4) >> 2;
                m                 = (5 * d - 3) / 153;
                d                 = 5 * d - 3 - 153 * m;
                d                 = (d + 5) / 5;
                y                 = (100 * y + j);
                if (m < 10) {
                    m += 3;
                }
                else {
                    m -= 9;
                    ++y;
                }
                return year_month_day{Year{static_cast<int> (y)}, MonthOfYear{m}, DayOfMonth{d}};
            }();
            Assert (result == legacyValue or jr < kGregorianCalendarEpoch.fJulianRep);
        }
        return result;
    }
    inline constexpr Date::JulianDayNumber Date::kMaxJulianRep = Date::ToJulianRep (December, 31d, Year::eLastYear);
    static_assert (Date::ToJulianRep (Date::kGregorianCalendarEpoch.fYMD) == Date::kGregorianCalendarEpoch.fJulianRep); // not important, but if that ever failed, would indicate serious bug or we changed definition

    inline constexpr Date::Date (JulianDayNumber julianRep, DataExchange::ValidationStrategy validationStrategy)
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
    inline constexpr Date::JulianDayNumber Date::GetJulianRep () const
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
    [[nodiscard]] inline Date Date::Add (int dayCount) const
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
    template <>
    constexpr EnumNames<Stroika::Foundation::Time::Date::NonStandardPrintFormat> DefaultNames<Stroika::Foundation::Time::Date::NonStandardPrintFormat>::k{
        EnumNames<Stroika::Foundation::Time::Date::NonStandardPrintFormat>::BasicArrayInitializer{{
            {Stroika::Foundation::Time::Date::NonStandardPrintFormat::eCurrentLocale_WithZerosStripped,
             L"Current-Locale-With-Zeros-Stripped"},
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
