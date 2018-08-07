/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
#include "../Execution/Exceptions.h"
#if qPlatform_Windows
#include "../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include "../Linguistics/Words.h"
#include "../Memory/SmallStackBuffer.h"
#include "DateTime.h"

#include "Date.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Time;

using namespace Time;

#if qPlatform_Windows
namespace {
    SYSTEMTIME toSYSTEM_ (const Date& date)
    {
        MonthOfYear m = MonthOfYear::eEmptyMonthOfYear;
        DayOfMonth  d = DayOfMonth::eEmptyDayOfMonth;
        Year        y = Year::eEmptyYear;
        date.mdy (&m, &d, &y);
        SYSTEMTIME st{};
        st.wYear  = static_cast<WORD> (y);
        st.wMonth = static_cast<WORD> (m);
        st.wDay   = static_cast<WORD> (d);
        return st;
    }
}
#endif

namespace {
    tm Date2TM_ (const Date& d)
    {
        struct tm tm {
        };
        tm.tm_year = static_cast<int> (d.GetYear ()) - 1900;
        tm.tm_mon  = static_cast<int> (d.GetMonth ()) - 1;
        tm.tm_mday = static_cast<int> (d.GetDayOfMonth ());
        return tm;
    }
}

// Just turn on while debuging this code
// or testing new compilers
#ifndef qDo_Aggressive_InternalChekcingOfUnderlyingLibrary_To_Debug_Locale_Date_Issues_
#define qDo_Aggressive_InternalChekcingOfUnderlyingLibrary_To_Debug_Locale_Date_Issues_ 0
#endif

/*
 *  This code is used to test/valdiate the underlying locale/stdc++ library, which we've had alot of trouble
 *  with!
 */
#if qDebug && qDo_Aggressive_InternalChekcingOfUnderlyingLibrary_To_Debug_Locale_Date_Issues_
namespace {
    void TestDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ (int tm_Year, int tm_Mon, int tm_mDay, const locale& l)
    {
        tm origDateTM{};
        origDateTM.tm_year = tm_Year;
        origDateTM.tm_mon  = tm_Mon;
        origDateTM.tm_mday = tm_mDay;

        const wchar_t kPattern[] = L"%x"; // http://www.cplusplus.com/reference/ctime/strftime/ ... (%x is date representation, ...the specifiers marked with an asterisk (*) are locale-dependent)

        wstring tmpStringRep;
        {
            const time_put<wchar_t>& tmput = use_facet<time_put<wchar_t>> (l);
            tm                       when  = origDateTM;
            wostringstream           oss;
            tmput.put (oss, oss, ' ', &when, begin (kPattern), begin (kPattern) + ::wcslen (kPattern));
            tmpStringRep = oss.str ();
        }
        tm resultTM{};
        {
            const time_get<wchar_t>&     tmget = use_facet<time_get<wchar_t>> (l);
            ios::iostate                 state = ios::goodbit;
            wistringstream               iss (tmpStringRep);
            istreambuf_iterator<wchar_t> itbegin (iss); // beginning of iss
            istreambuf_iterator<wchar_t> itend;         // end-of-stream
            tmget.get (itbegin, itend, iss, state, &resultTM, std::begin (kPattern), std::begin (kPattern) + ::wcslen (kPattern));
        }
        Assert (origDateTM.tm_year == resultTM.tm_year);
        Assert (origDateTM.tm_mon == resultTM.tm_mon);
        Assert (origDateTM.tm_mday == resultTM.tm_mday);
    }
    void TestDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ (Date d, const locale& l)
    {
        struct tm t = Date2TM_ (d);
        // skip test if year < 0 cuz VS.net 2k13 asserts out - not sure if they are right or not?
        if (t.tm_year >= 0) {
            TestDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ (t.tm_year, t.tm_mon, t.tm_mday, l);
        }
    }

    struct BuggyCasesTeReportToMSFT_ {
        BuggyCasesTeReportToMSFT_ ()
        {
            TestDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ (105, 5, 1, locale::classic ());
        }
    } _force_test_;
}
#endif

/*
 ********************************************************************************
 ************************** Date::FormatException *******************************
 ********************************************************************************
 */
const Date::FormatException Date::FormatException::kThe;

Date::FormatException::FormatException ()
    : StringException (String_Constant (L"Invalid Date Format"))
{
}

/*
 ********************************************************************************
 ************************************** Date ************************************
 ********************************************************************************
 */
#if qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
const Date Date::kMin{Date::min ()};
const Date Date::kMax{Date::max ()};
#else
constexpr Date Date::kMin;
constexpr Date Date::kMax;
#endif

constexpr Date::JulianRepType Date::kMinJulianRep;
constexpr Date::JulianRepType Date::kEmptyJulianRep;

Date Date::Parse (const String& rep, ParseFormat pf)
{
    if (rep.empty ()) {
        return Date{};
    }
    switch (pf) {
        case ParseFormat::eCurrentLocale: {
#if qPlatform_Windows
            /*
             * Windows Parser does better job than POSIX one - for reasons which elude me.
             * Automated test has some test cases to help close the gap...
             *      -- LGP 2011-10-08
             */
            return Parse (rep, LOCALE_USER_DEFAULT);
#else
            return Parse (rep, locale ());
#endif
        } break;
        case ParseFormat::eISO8601:
        case ParseFormat::eXML: {
            /*
             * We intentionally ignore TZ here - if any - because there is no notion of TZ in Date module - just DateTime...
             */
            int year  = 0;
            int month = 0;
            int day   = 0;
            DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
            int nItems = ::swscanf (rep.c_str (), L"%d-%d-%d", &year, &month, &day);
            DISABLE_COMPILER_MSC_WARNING_END (4996)
            if (nItems == 3) {
                return Date (Safe_jday_ (MonthOfYear (month), DayOfMonth (day), Year (year)));
            }
            else {
                return Date{}; // at some point maybe we should throw for badly-formatted dates? -- LGP 2011-10-08
            }
        } break;
        case ParseFormat::eJavascript: {
            int year  = 0;
            int month = 0;
            int day   = 0;
            DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
            int nItems = ::swscanf (rep.c_str (), L"%d/%d/%d", &month, &day, &year);
            DISABLE_COMPILER_MSC_WARNING_END (4996)
            Date result;
            if (nItems == 3) {
                result = Date (Safe_jday_ (MonthOfYear (month), DayOfMonth (day), Year (year)));
            }
#if qPlatform_Windows
            const LCID kUS_ENGLISH_LOCALE = MAKELCID (MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
            Ensure (result == Parse (rep, kUS_ENGLISH_LOCALE));
#endif
            return result;
        } break;
    }
    AssertNotReached ();
    return Date{};
}

Date Date::Parse (const String& rep, const locale& l)
{
    size_t consumedCharsInStringUpTo = 0;
    return Date::Parse (rep, l, &consumedCharsInStringUpTo);
}

namespace {
    size_t ComputeIdx_ (const istreambuf_iterator<wchar_t>& s, const istreambuf_iterator<wchar_t>& c)
    {
        size_t result = 0;
        for (auto i = s; i != c; ++i, ++result)
            ;
        return result;
    }
}
Date Date::Parse (const String& rep, const locale& l, size_t* consumedCharsInStringUpTo)
{
    RequireNotNull (consumedCharsInStringUpTo);
    if (rep.empty ()) {
        return Date{};
    }
    const time_get<wchar_t>&     tmget = use_facet<time_get<wchar_t>> (l);
    ios::iostate                 state = ios::goodbit;
    wistringstream               iss (rep.As<wstring> ());
    istreambuf_iterator<wchar_t> itbegin (iss); // beginning of iss
    istreambuf_iterator<wchar_t> itend;         // end-of-stream
    tm                           when{};
    istreambuf_iterator<wchar_t> i = tmget.get_date (itbegin, itend, iss, state, &when);
    if (state & ios::failbit) {
        Execution::Throw (FormatException::kThe);
    }
    *consumedCharsInStringUpTo = ComputeIdx_ (itbegin, i);
#if qDebug && qDo_Aggressive_InternalChekcingOfUnderlyingLibrary_To_Debug_Locale_Date_Issues_
    TestDateLocaleRoundTripsForDateWithThisLocaleLib_ (AsDate_ (when), l);
#endif
    return AsDate_ (when);
}

#if qPlatform_Windows
Date Date::Parse (const String& rep, LCID lcid)
{
    if (rep.empty ()) {
        return Date{};
    }
    DATE d{};
    try {
        using namespace Execution::Platform::Windows;
        ThrowIfErrorHRESULT (::VarDateFromStr (Characters::Platform::Windows::SmartBSTR (rep.c_str ()), lcid, VAR_DATEVALUEONLY, &d));
    }
    catch (...) {
        Execution::Throw (FormatException::kThe);
    }
    // SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
    SYSTEMTIME sysTime{};
    Verify (::VariantTimeToSystemTime (d, &sysTime));
    return Date (Safe_jday_ (MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay), Year (sysTime.wYear)));
}
#endif

String Date::Format (PrintFormat pf) const
{
    if (empty ()) {
        return String{};
    }
    switch (pf) {
        case PrintFormat::eCurrentLocale: {
            return Format (locale ());
        } break;
        case PrintFormat::eCurrentLocale_WithZerosStripped: {
            String tmp = Format (locale ());
            /*
             *  This logic probably needs to be locale-specific, but this is good enuf for now...
             *  Map things like:
             *      01:03:05 to 1:03:05
             *
             *  and map
             *      12/05/00 to 12/05, but DONT map 12/15/2000 to 12/15/2000
             */
            static const String_Constant kZero_ = String_Constant (L"0");
            optional<size_t>             i      = 0;
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
        case PrintFormat::eISO8601:
        case PrintFormat::eXML: {
            wchar_t     buf[20]; // really only  11 needed (so long as no negatives - which I dont think is allowed)
            MonthOfYear m = MonthOfYear::eEmptyMonthOfYear;
            DayOfMonth  d = DayOfMonth::eEmptyDayOfMonth;
            Year        y = Year::eEmptyYear;
            mdy (&m, &d, &y);
            Verify (::swprintf (buf, NEltsOf (buf), L"%04d-%02d-%02d", y, m, d) == 10);
            return buf;
        } break;
        case PrintFormat::eJavascript: {
            /*
             *  From
             *      http://msdn.microsoft.com/library/default.asp?url=/library/en-us/script56/html/ed737e50-6398-4462-8779-2af3c03f8325.asp
             *
             *          parse Method (JScript 5.6)
             *          ...
             *          The following rules govern what the parse method can successfully parse:
             *          Short dates can use either a "/" or "-" date separator, but must follow the month/day/year format, for example "7/20/96".
             *
             *  @see    explicit Date (const String& rep, Javascript);
             */
            wchar_t     buf[20]; // really only  11 needed (so long as no negatives - which I dont think is allowed)
            MonthOfYear m = MonthOfYear::eEmptyMonthOfYear;
            DayOfMonth  d = DayOfMonth::eEmptyDayOfMonth;
            Year        y = Year::eEmptyYear;
            mdy (&m, &d, &y);
            Verify (::swprintf (buf, NEltsOf (buf), L"%02d/%02d/%04d", m, d, y) == 10);
            return buf;
        } break;
    }
    AssertNotReached ();
    return String{};
}

String Date::Format (const locale& l) const
{
    return Format (l, String_Constant{L"%x"}); // http://www.cplusplus.com/reference/ctime/strftime/ ... (%x is date representation, ...the specifiers marked with an asterisk (*) are locale-dependent)
}

String Date::Format (const String& formatPattern) const
{
    return Format (locale (), formatPattern);
}

String Date::Format (const locale& l, const String& formatPattern) const
{
    if (empty ()) {
        return String ();
    }
#if qDebug && qDo_Aggressive_InternalChekcingOfUnderlyingLibrary_To_Debug_Locale_Date_Issues_
    TestDateLocaleRoundTripsForDateWithThisLocaleLib_ (AsDate_ (when), l);
#endif

    // http://new.cplusplus.com/reference/std/locale/time_put/put/
    tm                       when  = Date2TM_ (*this);
    const time_put<wchar_t>& tmput = use_facet<time_put<wchar_t>> (l);
    wostringstream           oss;
    tmput.put (oss, oss, ' ', &when, formatPattern.c_str (), formatPattern.c_str () + formatPattern.length ());
    return oss.str ();
}

#if qPlatform_Windows
String Date::Format (LCID lcid) const
{
    if (empty ()) {
        return String ();
    }
    SYSTEMTIME st      = toSYSTEM_ (*this);
    int        nTChars = ::GetDateFormat (lcid, DATE_SHORTDATE, &st, nullptr, nullptr, 0);
    if (nTChars == 0) {
        return String ();
    }
    else {
        SmallStackBuffer<TCHAR> buf (nTChars + 1);
        (void)::GetDateFormat (lcid, DATE_SHORTDATE, &st, nullptr, buf, nTChars + 1);
        return SDKString2Wide (static_cast<const TCHAR*> (buf));
    }
}

String Date::Format (LCID lcid, const String& format) const
{
    if (empty ()) {
        return String ();
    }
    SYSTEMTIME st      = toSYSTEM_ (*this);
    int        nTChars = ::GetDateFormatW (lcid, 0, &st, format.c_str (), nullptr, 0);
    if (nTChars == 0) {
        return String ();
    }
    else {
        SmallStackBuffer<wchar_t> buf (nTChars + 1);
        (void)::GetDateFormatW (lcid, 0, &st, format.c_str (), buf, nTChars + 1);
        return static_cast<const wchar_t*> (buf);
    }
}
#endif

#if qPlatform_Windows
String Date::LongFormat (LCID lcid) const
{
    if (empty ()) {
        return String ();
    }
    SYSTEMTIME st     = toSYSTEM_ (*this);
    int        nChars = ::GetDateFormatW (lcid, DATE_LONGDATE, &st, nullptr, nullptr, 0);
    if (nChars == 0) {
        return String ();
    }
    else {
        SmallStackBuffer<wchar_t> buf (nChars + 1);
        (void)::GetDateFormatW (lcid, DATE_LONGDATE, &st, nullptr, buf, nChars + 1);
        return static_cast<const wchar_t*> (buf);
    }
}
#endif

Date Date::AsDate_ (const tm& when)
{
    return Date (Safe_jday_ (MonthOfYear (when.tm_mon + 1), DayOfMonth (when.tm_mday), Year (when.tm_year + 1900)));
}

Date Date::AddDays (SignedJulianRepType dayCount) const
{
    // then assume was supposed to be relative to today
    Date result = empty () ? DateTime::GetToday () : *this;
    result.fJulianDateRep_ += dayCount;
    if (result.fJulianDateRep_ < Date::kMinJulianRep) {
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
    Ensure (empty () or (1 <= static_cast<int> (m) and static_cast<int> (m) <= 12));
    Ensure (0 <= static_cast<int> (m) and static_cast<int> (m) <= 12);
    return m;
}

DayOfMonth Date::GetDayOfMonth () const
{
    MonthOfYear m = MonthOfYear::eEmptyMonthOfYear;
    DayOfMonth  d = DayOfMonth::eEmptyDayOfMonth;
    Year        y = Year::eEmptyYear;
    mdy (&m, &d, &y);
    Ensure (empty () or (1 <= static_cast<int> (d) and static_cast<int> (d) <= 31));
    Ensure (0 <= static_cast<int> (d) and static_cast<int> (d) <= 31);
    return d;
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
    if (fJulianDateRep_ == kEmptyJulianRep) {
        *month = MonthOfYear::eEmptyMonthOfYear;
        *day   = DayOfMonth::eEmptyDayOfMonth;
        *year  = Year::eEmptyYear;
        return;
    }
    JulianRepType m;
    JulianRepType d;
    JulianRepType y;

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
        y++;
    }
    Ensure (1 <= m and m <= 12);
    *month = static_cast<MonthOfYear> (m);
    Ensure (1 <= d and d <= 31);
    *day  = static_cast<DayOfMonth> (d);
    *year = static_cast<Year> (y);
}

/*
 ********************************************************************************
 *************************** Date::DayDifference ********************************
 ********************************************************************************
 */

Date::SignedJulianRepType Time::DayDifference (const Date& lhs, const Date& rhs)
{
    Require (not lhs.empty ());
    Require (not rhs.empty ()); // since unclear what diff would mean
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
    return l < r ? -1 : 1;
    return lhs.GetJulianRep () - rhs.GetJulianRep ();
}

/*
 ********************************************************************************
 *************************** Date::YearDifference *******************************
 ********************************************************************************
 */

int Time::YearDifference (const Date& lhs, const Date& rhs)
{
    Require (not lhs.empty ()); // since meaning of diff wouldn't make much sense
    Require (not rhs.empty ()); // ditto

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
    Require (not lhs.empty ());                // since meaning of diff wouldn't make much sense
    Require (not rhs.empty ());                // ditto
    return DayDifference (lhs, rhs) / 365.25f; //tmphack
}

/*
 ********************************************************************************
 ***************************** GetFormattedAge **********************************
 ********************************************************************************
 */
String Time::GetFormattedAge (const Date& birthDate, const Date& deathDate)
{
    if (birthDate.empty ()) {
        return String_Constant (L"?");
    }
    else {
        int yearDiff = deathDate.empty () ? YearDifference (DateTime::GetToday (), birthDate) : YearDifference (deathDate, birthDate);
        return Format (L"%d", yearDiff);
    }
}

String Time::GetFormattedAgeWithUnit (const Date& birthDate, const Date& deathDate, bool abbrevUnit)
{
    if (birthDate.empty ()) {
        return String_Constant (L"?");
    }
    else {
        int yearDiff = deathDate.empty () ? YearDifference (DateTime::GetToday (), birthDate) : YearDifference (deathDate, birthDate);
        if (yearDiff >= 0 and yearDiff < 2) {
            float   yearDiffF = deathDate.empty () ? YearDifferenceF (DateTime::GetToday (), birthDate) : YearDifferenceF (deathDate, birthDate);
            int     months    = int(yearDiffF * 12.0f + 0.4999f);
            wstring unitBase  = abbrevUnit ? L"mo" : L"month";
            return Format (L"%d %s", months, Linguistics::PluralizeNoun (unitBase, months).c_str ());
        }
        else {
            wstring unitBase = abbrevUnit ? L"yr" : L"year";
            return Format (L"%d %s", yearDiff, Linguistics::PluralizeNoun (unitBase, yearDiff).c_str ());
        }
    }
}
