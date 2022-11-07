﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <ctime>
#include <sstream>

#include "../Characters/Format.h"
#include "../Characters/String_Constant.h"
#if qPlatform_Windows
#include "../Characters/Platform/Windows/SmartBSTR.h"
#endif
#include "../Debug/Assertions.h"
#include "../Execution/Throw.h"
#if qPlatform_Windows
#include "../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include "../Linguistics/MessageUtilities.h"
#include "DateTime.h"

#include "Date.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Time;

using namespace Time;

namespace {
    constexpr int kTM_Year_RelativeToYear_{1900}; // see https://man7.org/linux/man-pages/man3/ctime.3.html
}

namespace {
    inline ::tm Date2TM_ (const Date& d)
    {
        ::tm tm{};
        tm.tm_year = static_cast<int> (d.GetYear ()) - kTM_Year_RelativeToYear_;
        tm.tm_mon  = static_cast<int> (d.GetMonth ()) - 1;
        tm.tm_mday = static_cast<int> (d.GetDayOfMonth ());
        return tm;
    }
}

/*
 ********************************************************************************
 **************************** Date::FormatException *****************************
 ********************************************************************************
 */
Date::FormatException::FormatException ()
    : Execution::RuntimeErrorException<>{L"Invalid Date Format"sv}
{
}

/*
 ********************************************************************************
 *********************************** Date ***************************************
 ********************************************************************************
 */
Date Date::Parse_ (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns, size_t* consumedCharsInStringUpTo)
{
    if (rep.empty ()) {
        Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty Date{}
    }
    wstring                  wRep  = rep.As<wstring> ();
    const time_get<wchar_t>& tmget = use_facet<time_get<wchar_t>> (l);
    for (const auto& formatPattern : formatPatterns) {
        if (auto r = ParseQuietly_ (wRep, tmget, formatPattern, consumedCharsInStringUpTo)) {
            return *r;
        }
    }
    Execution::Throw (FormatException::kThe);
}

optional<Date> Date::LocaleFreeParseQuietly_kMonthDayYearFormat_ (const wstring& rep, size_t* consumedCharsInStringUpTo)
{
    // parse locale independent "%m/%d/%Y" - from - https://en.cppreference.com/w/cpp/locale/time_get/get - including validation
    int year  = 0;
    int month = 0;
    int day   = 0;
    DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
    unsigned int pos{};                       // doesn't count fixed characters, just % characters
    int          nItems = ::swscanf (rep.c_str (), L"%d/%d/%d%n", &month, &day, &year, &pos);
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    if (nItems == 3) {
        if (consumedCharsInStringUpTo != nullptr) {
            Assert (pos + 2 < rep.length ());
            *consumedCharsInStringUpTo = pos + 2;
        }
        if ((1 <= month and month <= 12) and (1 <= day and day <= 31) and (year > 0)) {
            return Date{Safe_jday_ (MonthOfYear (month), DayOfMonth (day), Year (year))};
        }
    }
    return nullopt;
}

optional<Date> Date::ParseQuietly_ (const wstring& rep, const time_get<wchar_t>& tmget, const String& formatPattern, size_t* consumedCharsInStringUpTo)
{
    Require (not rep.empty ());
    if constexpr (qCompilerAndStdLib_locale_time_get_PCTM_RequiresLeadingZero_Buggy) {
        if (formatPattern == kMonthDayYearFormat) {
            return LocaleFreeParseQuietly_kMonthDayYearFormat_ (rep, consumedCharsInStringUpTo);
        }
    }
    auto computeIdx = [] (const istreambuf_iterator<wchar_t>& s, const istreambuf_iterator<wchar_t>& c) -> size_t {
        size_t result = 0;
        for (auto i = s; i != c; ++i, ++result)
            ;
        return result;
    };
    ios::iostate                 errState = ios::goodbit;
    tm                           when{};
    wistringstream               iss{rep};
    istreambuf_iterator<wchar_t> itbegin{iss}; // beginning of iss
    istreambuf_iterator<wchar_t> itend;        // end-of-stream
    istreambuf_iterator<wchar_t> i = tmget.get (itbegin, itend, iss, errState, &when, formatPattern.c_str (), formatPattern.c_str () + formatPattern.length ());
    if ((errState & ios::badbit) or (errState & ios::failbit)) [[unlikely]] {
        return nullopt;
    }
    else {
        if (consumedCharsInStringUpTo != nullptr) {
            *consumedCharsInStringUpTo = computeIdx (itbegin, i);
        }
        return AsDate_ (when);
    }
}

String Date::Format (NonStandardPrintFormat pf) const
{
    switch (pf) {
        case eCurrentLocale_WithZerosStripped: {
            String tmp = Format (locale{});
            /*
             *  This logic probably needs to be locale-specific, but this is good enuf for now...
             *  Map things like:
             *      01:03:05 to 1:03:05
             *
             *  and map
             *      12/05/00 to 12/05, but DON'T map 12/15/2000 to 12/15/2000
             */
            static const String kZero_ = L"0"sv;
            optional<size_t>    i      = 0;
            while ((i = tmp.Find (kZero_, *i))) {
                // any 0N (where n a digit) is replaced with a single '0'
                Assert (tmp[*i] == '0');
                bool isLeadingZero = false;
                if (*i + 1 < tmp.length () and tmp[*i + 1].IsDigit ()) {
                    if (*i == 0 or not tmp[*i - 1].IsDigit ()) {
                        // don't strip leading zeros if its the YEAR - the last part of a X/Y/Z combo...
                        //
                        // this test is quite inadequate...
                        if (*i + 2 < tmp.length ()) {
                            isLeadingZero = true;
                        }
                    }
                }
                if (isLeadingZero) {
                    tmp = tmp.substr (0, *i) + tmp.substr (*i + 1);
                }
                else {
                    i = *i + 1;
                }
            }
            return tmp;
        }
    }
    AssertNotReached ();
    return String{};
}

String Date::Format (const locale& l) const
{
    return Format (l, kLocaleStandardFormat);
}

String Date::Format (const String& formatPattern) const
{
    return Format (locale{}, formatPattern);
}

String Date::Format (const locale& l, const String& formatPattern) const
{
    // http://new.cplusplus.com/reference/std/locale/time_put/put/
    ::tm                     when  = Date2TM_ (*this);
    const time_put<wchar_t>& tmput = use_facet<time_put<wchar_t>> (l);
    wostringstream           oss;
    tmput.put (oss, oss, ' ', &when, formatPattern.c_str (), formatPattern.c_str () + formatPattern.length ());
    return oss.str ();
}

Date Date::AsDate_ (const ::tm& when)
{
    return Date{Safe_jday_ (MonthOfYear (when.tm_mon + 1), DayOfMonth (when.tm_mday), Year (when.tm_year + kTM_Year_RelativeToYear_))};
}

Date Date::AddDays (SignedJulianRepType dayCount) const
{
    /*
     * empty () calls to AddDays were interpretted as DateTime::GetToday () until Stroika v2.1d6;
     *  surprising semantics - say what you mean - don't use empty for this
     *  And it violates documented princple that 'empty' means like negative infinity, a little less than kMin.
     *      WAS:
     *          Date result = empty () ? DateTime::GetToday () : *this;
     */
    Date result = *this;
    result.fJulianDateRep_ += dayCount;
    if (result.fJulianDateRep_ < Date::kMinJulianRep) [[unlikely]] {
        static const range_error kRangeErrror_{"Date::AddDays cannot add days to go before the first julian calandar day"};
        Execution::Throw (kRangeErrror_);
    }
    return result;
}

Date::JulianRepType Date::DaysSince () const
{
    SignedJulianRepType r = DayDifference (DateTime::GetToday (), *this);
    if (r < 0) {
        return 0;
    }
    else {
        return r;
    }
}

Year Date::GetYear () const
{
    MonthOfYear m = MonthOfYear::eEmptyMonthOfYear;
    DayOfMonth  d = DayOfMonth::eEmptyDayOfMonth;
    Year        y = Year::eEmptyYear;
    mdy (&m, &d, &y);
    return y;
}

MonthOfYear Date::GetMonth () const
{
    MonthOfYear m = MonthOfYear::eEmptyMonthOfYear;
    DayOfMonth  d = DayOfMonth::eEmptyDayOfMonth;
    Year        y = Year::eEmptyYear;
    mdy (&m, &d, &y);
    Ensure (1 <= static_cast<int> (m) and static_cast<int> (m) <= 12);
    Ensure (0 <= static_cast<int> (m) and static_cast<int> (m) <= 12);
    return m;
}

DayOfMonth Date::GetDayOfMonth () const
{
    MonthOfYear m = MonthOfYear::eEmptyMonthOfYear;
    DayOfMonth  d = DayOfMonth::eEmptyDayOfMonth;
    Year        y = Year::eEmptyYear;
    mdy (&m, &d, &y);
    Ensure (1 <= static_cast<int> (d) and static_cast<int> (d) <= 31);
    Ensure (0 <= static_cast<int> (d) and static_cast<int> (d) <= 31);
    return d;
}

DayOfWeek Date::GetDayOfWeek () const
{
    /*
     * From https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
     *     Gauss's algorithm:
     *     ...
     *     The weekday of 1 January in year number A is
     *          R(1+5R(A-1,4)+4R(A-1,100)+6R(A-1,400),7)  
     *              - where {\displaystyle R(y,m)} R(y,m) is the remainder after division of y by m,[6] or y modulo m.
     */
    unsigned int y             = static_cast<unsigned int> (GetYear ());
    auto         R             = [] (unsigned int i, unsigned int r) { return i % r; };
    unsigned int weekdayOfJan1 = R (1 + 5 * R (y - 1, 4) + 4 * R (y - 1, 100) + 6 * R (y - 1, 400), 7);
    // this assumes Sunday is ZERO and the rest of the days follow it...

    unsigned int month0 = static_cast<unsigned int> (GetMonth () - MonthOfYear::eJanuary);

    static constexpr unsigned int kDayOfWeekOffsetPerMonth_[12] = {
        3,
        0, // february special - add one for leap year
        3,
        2,
        3,
        2,
        3,
        3,
        2,
        3,
        2,
        3,
    };
    unsigned int targetDayOfWeek = weekdayOfJan1;
    for (unsigned int i = 0; i < month0; ++i) {
        targetDayOfWeek += kDayOfWeekOffsetPerMonth_[i];
    }
    if (month0 > 1 and IsLeapYear (GetYear ())) {
        targetDayOfWeek += 1;
    }

    // add which day of the month (offset from first)
    targetDayOfWeek += (GetDayOfMonth () - DayOfMonth::e1);

    return static_cast<DayOfWeek> (R (targetDayOfWeek, 7) + static_cast<unsigned int> (DayOfWeek::eSunday));
}

template <>
::tm Date::As () const
{
    return Date2TM_ (*this);
}

/*
 * Convert a Julian day number to its corresponding Gregorian calendar
 * date.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
 * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
 * This function not valid before that.
 *
 * (This code originally from NIHCL)
 */
void Date::mdy (MonthOfYear* month, DayOfMonth* day, Year* year) const
{
    RequireNotNull (month);
    RequireNotNull (day);
    RequireNotNull (year);
    JulianRepType m;
    JulianRepType d;
    JulianRepType y;
    // A reference for this formula (not original I used) - can be found at:
    //      http://aa.usno.navy.mil/faq/docs/JD_Formula.php
    // at least close, and I could probably switch to that...
    //
    JulianRepType j = fJulianDateRep_ - 1721119;
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
    Ensure (1 <= m and m <= 12);
    *month = static_cast<MonthOfYear> (m);
    Ensure (1 <= d and d <= 31);
    *day  = static_cast<DayOfMonth> (d);
    *year = static_cast<Year> (y);
}

#if qCompilerAndStdLib_linkerLosesInlinesSoCannotBeSeenByDebugger_Buggy && qDebug
String Date::ToString () const
{
    return Format ();
}
#endif

/*
 ********************************************************************************
 *************************** Date::DayDifference ********************************
 ********************************************************************************
 */
Date::SignedJulianRepType Time::DayDifference (const Date& lhs, const Date& rhs)
{
    Date::JulianRepType l = lhs.GetJulianRep ();
    Date::JulianRepType r = rhs.GetJulianRep ();
    if (l == r) {
        return 0;
    }
    if (l < r) {
        unsigned int diff = r - l;
        Assert (INT_MIN <= -INT_MAX);
        if (diff >= static_cast<unsigned int> (INT_MAX)) {
            return INT_MIN;
        }
        else {
            return -static_cast<int> (diff);
        }
    }
    else {
        unsigned int diff = l - r;
        if (diff >= static_cast<unsigned int> (INT_MAX)) {
            return INT_MAX;
        }
        else {
            return static_cast<int> (diff);
        }
    }
    return static_cast<Date::SignedJulianRepType> (lhs.GetJulianRep ()) - static_cast<Date::SignedJulianRepType> (rhs.GetJulianRep ());
}

/*
 ********************************************************************************
 *************************** Date::YearDifference *******************************
 ********************************************************************************
 */
int Time::YearDifference (const Date& lhs, const Date& rhs)
{
    MonthOfYear lm = MonthOfYear::eEmptyMonthOfYear;
    DayOfMonth  ld = DayOfMonth::eEmptyDayOfMonth;
    Year        ly = Year::eEmptyYear;
    lhs.mdy (&lm, &ld, &ly);

    MonthOfYear rm = MonthOfYear::eEmptyMonthOfYear;
    DayOfMonth  rd = DayOfMonth::eEmptyDayOfMonth;
    Year        ry = Year::eEmptyYear;
    rhs.mdy (&rm, &rd, &ry);

    int diff = static_cast<int> (ly) - static_cast<int> (ry);

    if (lm < rm or (lm == rm and ld < rd)) {
        diff--;
    }
    return diff;
}

float Time::YearDifferenceF (const Date& lhs, const Date& rhs)
{
    return DayDifference (lhs, rhs) / 365.25f; //tmphack
}

/*
 ********************************************************************************
 ***************************** GetFormattedAge **********************************
 ********************************************************************************
 */
String Time::GetFormattedAge (const optional<Date>& birthDate, const optional<Date>& deathDate)
{
    if (birthDate.has_value ()) {
        int yearDiff = deathDate.has_value () ? YearDifference (*deathDate, *birthDate) : YearDifference (DateTime::GetToday (), *birthDate);
        return Format (L"%d", yearDiff);
    }
    else {
        return L"?"sv;
    }
}

/*
 ********************************************************************************
 ************************** GetFormattedAgeWithUnit *****************************
 ********************************************************************************
 */
String Time::GetFormattedAgeWithUnit (const optional<Date>& birthDate, const optional<Date>& deathDate, bool abbrevUnit)
{
    if (birthDate.has_value ()) {
        int yearDiff = deathDate.has_value () ? YearDifference (*deathDate, *birthDate) : YearDifference (DateTime::GetToday (), *birthDate);
        if (yearDiff >= 0 and yearDiff < 2) {
            float   yearDiffF = deathDate.has_value () ? YearDifferenceF (*deathDate, *birthDate) : YearDifferenceF (DateTime::GetToday (), *birthDate);
            int     months    = int (yearDiffF * 12.0f + 0.4999f);
            wstring unitBase  = abbrevUnit ? L"mo" : L"month";
            return Format (L"%d %s", months, Linguistics::MessageUtiltiesManager::Get ()->PluralizeNoun (unitBase, months).c_str ());
        }
        else {
            wstring unitBase = abbrevUnit ? L"yr" : L"year";
            return Format (L"%d %s", yearDiff, Linguistics::MessageUtiltiesManager::Get ()->PluralizeNoun (unitBase, yearDiff).c_str ());
        }
    }
    else {
        return L"?"sv;
    }
}
