/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <ctime>
#include    <sstream>

#if     qPlatform_Windows
#include    <atlbase.h>     // For CComBSTR
#elif   qPlatform_POSIX
#endif

#include    "../Characters/Format.h"
#include    "../Debug/Assertions.h"
#include    "../Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include    "../Memory/SmallStackBuffer.h"
#include    "../Linguistics/Words.h"
#include    "DateTime.h"

#include    "Date.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Time;

using   Debug::TraceContextBumper;

using   namespace   Time;




#if     qPlatform_Windows
namespace   {
    SYSTEMTIME toSYSTEM_ (const Date& date)
    {
        SYSTEMTIME  st;
        memset (&st, 0, sizeof (st));
        MonthOfYear m   =   MonthOfYear::eEmptyMonthOfYear;
        DayOfMonth  d   =   DayOfMonth::eEmptyDayOfMonth;
        Year        y   =   Year::eEmptyYear;
        date.mdy (&m, &d, &y);
        st.wYear = static_cast<WORD> (y);
        st.wMonth = static_cast<WORD> (m);
        st.wDay = static_cast<WORD> (d);
        return st;
    }
}
#endif



namespace   {
    /*
     * Convert Gregorian calendar date to the corresponding Julian day number
     * j.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
     * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
     * This function not valid before that.
     *
     * (This code originally from NIHCL)
     */
    Date::JulianRepType jday_ (MonthOfYear month, DayOfMonth day, Year year)
    {
        if (month == MonthOfYear::eEmptyMonthOfYear or day == DayOfMonth::eEmptyDayOfMonth or year == Year::eEmptyYear) {
            return Date::kEmptyJulianRep;
        }

        Require (static_cast<int> (year) > 1752 or (static_cast<int> (year) == 1752 and (month > MonthOfYear::eSeptember or (month == MonthOfYear::eSeptember and static_cast<int> (day) >= 14))));

        Date::JulianRepType c;
        Date::JulianRepType ya;
        if (static_cast<int> (month) > 2) {
            month = static_cast<MonthOfYear> (static_cast<int> (month) - 3);
        }
        else {
            month = static_cast<MonthOfYear> (static_cast<int> (month) + 9);
            year = static_cast<Year> (static_cast<int> (year) - 1);
        }
        c = static_cast<int> (year) / 100;
        ya = static_cast<int> (year) - 100 * c;
        return (((146097 * c) >> 2) + ((1461 * ya) >> 2) + (153 * static_cast<int> (month) + 2) / 5 + static_cast<int> (day) + 1721119);
    }
}

namespace   {
    Date::JulianRepType Safe_jday_ (MonthOfYear month, DayOfMonth day, Year year)
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
}


namespace   {
    Date    AsDate_ (const tm& when)
    {
        return Date (Safe_jday_ (MonthOfYear (when.tm_mon + 1), DayOfMonth (when.tm_mday), Year (when.tm_year + 1900)));
    }
}

namespace   {
    tm  Date2TM_ (const Date& d)
    {
        struct tm tm;
        memset(&tm, 0, sizeof(tm));
        tm.tm_year = static_cast<int> (d.GetYear ()) - 1900;
        tm.tm_mon = static_cast<int> (d.GetMonth ()) - 1;
        tm.tm_mday = static_cast<int> (d.GetDayOfMonth ());
        return tm;
    }
}









/*
 ********************************************************************************
 ************************** Date::FormatException *******************************
 ********************************************************************************
 */
Date::FormatException::FormatException ()
    : StringException (L"Invalid Date Format")
{
}







/*
 ********************************************************************************
 ************************************** Date ************************************
 ********************************************************************************
 */
const   Date    Date::kMin  =   Date (Date::JulianRepType (Date::kMinJulianRep));
const   Date    Date::kMax  =   Date (Date::JulianRepType (UINT_MAX - 1));

Date::Date ()
    : fJulianDateRep_ (kEmptyJulianRep)
{
}

Date::Date (JulianRepType julianRep)
    : fJulianDateRep_ (julianRep)
{
}

Date::Date (Year year, MonthOfYear month, DayOfMonth day)
    : fJulianDateRep_ (jday_ (month, day, year))
{
}

Date    Date::Parse (const String& rep, PrintFormat pf)
{
    if (rep.empty ()) {
        return Date ();
    }
    switch (pf) {
        case    PrintFormat::eCurrentLocale: {
#if     qPlatform_Windows
                /*
                 * Windows Parser does better job than POSIX one - for reasons which elude me.
                 * Automated test has some test cases to help close the gap...
                 *      -- LGP 2011-10-08
                 */
                return Parse (rep, LOCALE_USER_DEFAULT);
#else
                return Parse (rep, locale ());
#endif
            }
            break;
        case    PrintFormat::eXML: {
                /*
                 * We intentionally ignore TZ here - if any - because there is no notion of TZ in Date module - just DateTime...
                 */
                int year    =   0;
                int month   =   0;
                int day     =   0;
#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (push)
#pragma warning (4 : 4996)      // MSVC SILLY WARNING ABOUT USING swscanf_s
#endif
                int nItems  =   ::swscanf (rep.c_str (), L"%d-%d-%d", &year, &month, &day);
#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (pop)
#endif
                if (nItems == 3) {
                    return Date (Safe_jday_ (MonthOfYear (month), DayOfMonth (day), Year (year)));
                }
                else {
                    return Date (); // at some point maybe we should throw for badly-formatted dates? -- LGP 2011-10-08
                }
            }
            break;
        case    PrintFormat::eJavascript: {
                int year    =   0;
                int month   =   0;
                int day     =   0;
#pragma warning (push)
#pragma warning (4 : 4996)      // MSVC SILLY WARNING ABOUT USING swscanf_s
                int nItems  =   ::swscanf (rep.c_str (), L"%d/%d/%d", &month, &day, &year);
#pragma warning (pop)
                Date    result;
                if (nItems == 3) {
                    result = Date (Safe_jday_ (MonthOfYear (month), DayOfMonth (day), Year (year)));
                }
#if     qPlatform_Windows
                const   LCID    kUS_ENGLISH_LOCALE  =   MAKELCID (MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
                Ensure (result == Parse (rep, kUS_ENGLISH_LOCALE));
#endif
                return result;
            }
            break;
        default: {
                AssertNotReached ();
                return Date ();
            }
            break;
    }
}

Date    Date::Parse (const String& rep, const locale& l)
{
    if (rep.empty ()) {
        return Date ();
    }
    const time_get<wchar_t>& tmget = use_facet <time_get<wchar_t> > (l);
    ios::iostate state  =   ios::goodbit;
    wistringstream iss (rep.As<wstring> ());
    istreambuf_iterator<wchar_t> itbegin (iss);  // beginning of iss
    istreambuf_iterator<wchar_t> itend;          // end-of-stream
    tm when;
    memset (&when, 0, sizeof (when));
    tmget.get_date (itbegin, itend, iss, state, &when);
#if     qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear
    // This is a crazy correction. I have almost no idea why (unless its some Y2K workaround gone crazy). I hope this fixes it???
    // -- LGP 2011-10-09
    if (not (-200 <= when.tm_year and when.tm_year < 200)) {
        when.tm_year -= 1900;
    }
#endif
    return AsDate_ (when);
}

#if     qPlatform_Windows
Date    Date::Parse (const String& rep, LCID lcid)
{
    if (rep.empty ()) {
        return Date ();
    }
    DATE        d;
    (void)::memset (&d, 0, sizeof (d));
    try {
        ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (rep.c_str ()), lcid, VAR_DATEVALUEONLY, &d));
    }
    catch (...) {
        Execution::DoThrow (FormatException ());
    }
    // SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
    SYSTEMTIME  sysTime;
    memset (&sysTime, 0, sizeof (sysTime));
    Verify (::VariantTimeToSystemTime (d, &sysTime));
    return Date (Safe_jday_ (MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay), Year (sysTime.wYear)));
}
#endif

String Date::Format (PrintFormat pf) const
{
    if (empty ()) {
        return String ();
    }
    switch (pf) {
        case    PrintFormat::eCurrentLocale: {
#if     qPlatform_Windows
                return Format (LOCALE_USER_DEFAULT);
#else
                AssertNotImplemented ();
                return String ();
#endif
            }
            break;
        case    PrintFormat::eXML: {
                wchar_t buf[20];    // really only  11 needed (so long as no negatives - which I dont think is allowed)
                MonthOfYear m   =   MonthOfYear::eEmptyMonthOfYear;
                DayOfMonth  d   =   DayOfMonth::eEmptyDayOfMonth;
                Year        y   =   Year::eEmptyYear;
                mdy (&m, &d, &y);
#if     qSupportValgrindQuirks
                // Makes little sense - even buf[0] not sufficient - but this silences lots of warnings.
                // -- LGP 2012-05-19
                memset (buf, 0, sizeof(buf));
#endif
                Verify (::swprintf (buf, NEltsOf (buf), L"%04d-%02d-%02d", y, m, d) == 10);
                return buf;
            }
            break;
        case    PrintFormat::eJavascript: {
                /*
                 *  From
                 *      http://msdn.microsoft.com/library/default.asp?url=/library/en-us/script56/html/ed737e50-6398-4462-8779-2af3c03f8325.asp
                 *
                 *          parse Method (JScript 5.6)
                 *          ...
                 *          The following rules govern what the parse method can successfully parse:
                 *          Short dates can use either a "/" or "-" date separator, but must follow the month/day/year format, for example "7/20/96".
                 *
                 *  See also        explicit Date (const String& rep, Javascript);
                 */
                wchar_t buf[20];    // really only  11 needed (so long as no negatives - which I dont think is allowed)
                MonthOfYear m   =   MonthOfYear::eEmptyMonthOfYear;
                DayOfMonth  d   =   DayOfMonth::eEmptyDayOfMonth;
                Year        y   =   Year::eEmptyYear;
                mdy (&m, &d, &y);
#if     qSupportValgrindQuirks
                // Makes little sense - even buf[0] not sufficient - but this silences lots of warnings.
                // -- LGP 2012-05-19
                memset (buf, 0, sizeof(buf));
#endif
                Verify (::swprintf (buf, NEltsOf (buf), L"%02d/%02d/%04d", m, d, y) == 10);
                return buf;
            }
            break;
        default: {
                AssertNotReached ();
                return String ();
            }
            break;
    }
}

String Date::Format (const locale& l) const
{
    if (empty ()) {
        return String ();
    }
    // http://new.cplusplus.com/reference/std/locale/time_put/put/
    const time_put<wchar_t>& tmput = use_facet <time_put<wchar_t> > (l);
    tm when =   Date2TM_ (*this);
    wostringstream oss;
    // Read docs - not sure how to use this to get the local-appropriate format
    // %X MAYBE just what we want  - locale DEPENDENT!!!
    wchar_t pattern[] = L"%x";
    tmput.put (oss, oss, ' ', &when, StartOfArray (pattern), StartOfArray (pattern) + wcslen (pattern));
    return oss.str ();
}

#if     qPlatform_Windows
String Date::Format (LCID lcid) const
{
    if (empty ()) {
        return String ();
    }
    SYSTEMTIME  st  =   toSYSTEM_ (*this);
    int nTChars =   ::GetDateFormat (lcid, DATE_SHORTDATE, &st, nullptr, nullptr, 0);
    if (nTChars == 0) {
        return String ();
    }
    else {
        SmallStackBuffer<TCHAR> buf (nTChars + 1);
        (void)::GetDateFormat (lcid, DATE_SHORTDATE, &st, nullptr, buf, nTChars + 1);
        return TString2Wide (static_cast<const TCHAR*> (buf));
    }
}

String Date::Format (const String& format, LCID lcid) const
{
    if (empty ()) {
        return String ();
    }
    SYSTEMTIME  st  =   toSYSTEM_ (*this);
    int nTChars =   ::GetDateFormat (lcid, 0, &st, format.c_str (), nullptr, 0);
    if (nTChars == 0) {
        return String ();
    }
    else {
        SmallStackBuffer<TCHAR> buf (nTChars + 1);
        (void)::GetDateFormat (lcid, 0, &st, format.c_str (), buf, nTChars + 1);
        return TString2Wide (static_cast<const TCHAR*> (buf));
    }
}
#endif

#if     qPlatform_Windows
String Date::LongFormat (LCID lcid) const
{
    if (empty ()) {
        return String ();
    }
    SYSTEMTIME  st  =   toSYSTEM_ (*this);
    int nTChars =   ::GetDateFormat (lcid, DATE_LONGDATE, &st, nullptr, nullptr, 0);
    if (nTChars == 0) {
        return String ();
    }
    else {
        SmallStackBuffer<TCHAR> buf (nTChars + 1);
        (void)::GetDateFormat (lcid, DATE_LONGDATE, &st, nullptr, buf, nTChars + 1);
        return TString2Wide (static_cast<const TCHAR*> (buf));
    }
}
#endif

Date    Date::AddDays (int dayCount)
{
    if (empty ()) {
        // then assume was supposed to be relative to today
        *this = DateTime::GetToday ();
    }
    fJulianDateRep_ += dayCount;
    return *this;
}

Date::JulianRepType Date::DaysSince () const
{
    int r   =   DayDifference (DateTime::GetToday (), *this);
    if (r < 0) {
        return 0;
    }
    else {
        return r;
    }
}

Year    Date::GetYear () const
{
    MonthOfYear m   =   MonthOfYear::eEmptyMonthOfYear;
    DayOfMonth  d   =   DayOfMonth::eEmptyDayOfMonth;
    Year        y   =   Year::eEmptyYear;
    mdy (&m, &d, &y);
    return y;
}

MonthOfYear Date::GetMonth () const
{
    MonthOfYear m   =   MonthOfYear::eEmptyMonthOfYear;
    DayOfMonth  d   =   DayOfMonth::eEmptyDayOfMonth;
    Year        y   =   Year::eEmptyYear;
    mdy (&m, &d, &y);
    Ensure (empty () or 1 <= static_cast<int> (m) and static_cast<int> (m) <= 12);
    Ensure (0 <= static_cast<int> (m) and static_cast<int> (m) <= 12);
    return m;
}

DayOfMonth  Date::GetDayOfMonth () const
{
    MonthOfYear m   =   MonthOfYear::eEmptyMonthOfYear;
    DayOfMonth  d   =   DayOfMonth::eEmptyDayOfMonth;
    Year        y   =   Year::eEmptyYear;
    mdy (&m, &d, &y);
    Ensure (empty () or 1 <= static_cast<int> (d) and static_cast<int> (d) <= 31);
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
void    Date::mdy (MonthOfYear* month, DayOfMonth* day, Year* year) const
{
    RequireNotNull (month);
    RequireNotNull (day);
    RequireNotNull (year);
    if (fJulianDateRep_ == kEmptyJulianRep) {
        *month = MonthOfYear::eEmptyMonthOfYear;
        *day = DayOfMonth::eEmptyDayOfMonth;
        *year = Year::eEmptyYear;
        return;
    }
    JulianRepType   m;
    JulianRepType   d;
    JulianRepType   y;

    JulianRepType j = fJulianDateRep_ - 1721119;
    y = (((j << 2) - 1) / 146097);
    j = (j << 2) - 1 - 146097 * y;
    d = (j >> 2);
    j = ((d << 2) + 3) / 1461;
    d = ((d << 2) + 3 - 1461 * j);
    d = (d + 4) >> 2;
    m = (5 * d - 3) / 153;
    d = 5 * d - 3 - 153 * m;
    d = (d + 5) / 5;
    y = (100 * y + j);
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
    *day = static_cast<DayOfMonth> (d);
    *year = static_cast<Year> (y);
}





/*
 ********************************************************************************
 *************************** Date::DayDifference ********************************
 ********************************************************************************
 */

int Time::DayDifference (const Date& lhs, const Date& rhs)
{
    Require (not lhs.empty ());
    Require (not rhs.empty ());     // since unclear what diff would mean
    Date::JulianRepType l   =   lhs.GetJulianRep ();
    Date::JulianRepType r   =   rhs.GetJulianRep ();
    if (l == r) {
        return 0;
    }
    if (l < r) {
        unsigned int    diff    =   r - l;
        Assert (INT_MIN <= -INT_MAX);
        if (diff >= static_cast<unsigned int> (INT_MAX)) {
            return INT_MIN;
        }
        else {
            return -static_cast<int> (diff);
        }
    }
    else {
        unsigned int    diff    =   l - r;
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
    Require (not lhs.empty ());     // since meaning of diff wouldn't make much sense
    Require (not rhs.empty ());     // ditto

    MonthOfYear lm  =   MonthOfYear::eEmptyMonthOfYear;
    DayOfMonth  ld  =   DayOfMonth::eEmptyDayOfMonth;
    Year        ly  =   Year::eEmptyYear;
    lhs.mdy (&lm, &ld, &ly);

    MonthOfYear rm  =   MonthOfYear::eEmptyMonthOfYear;
    DayOfMonth  rd  =   DayOfMonth::eEmptyDayOfMonth;
    Year        ry  =   Year::eEmptyYear;
    rhs.mdy (&rm, &rd, &ry);

    int diff    =   static_cast<int> (ly) - static_cast<int> (ry);

    if (lm < rm or (lm == rm and ld < rd)) {
        diff--;
    }
    return diff;
}

float   Time::YearDifferenceF (const Date& lhs, const Date& rhs)
{
    Require (not lhs.empty ());     // since meaning of diff wouldn't make much sense
    Require (not rhs.empty ());     // ditto
    return DayDifference (lhs, rhs) / 365.25f;  //tmphack
}






/*
 ********************************************************************************
 ***************************** GetFormattedAge **********************************
 ********************************************************************************
 */
String Time::GetFormattedAge (const Date& birthDate, const Date& deathDate)
{
    if (birthDate.empty ()) {
        return L"?";
    }
    else {
        int yearDiff    =   deathDate.empty () ? YearDifference (DateTime::GetToday (), birthDate) : YearDifference (deathDate, birthDate);
        return Format (L"%d", yearDiff);
    }
}

String Time::GetFormattedAgeWithUnit (const Date& birthDate, const Date& deathDate, bool allowMonths, bool abbrevUnit)
{
    if (birthDate.empty ()) {
        return L"?";
    }
    else {
        int yearDiff    =   deathDate.empty () ? YearDifference (DateTime::GetToday (), birthDate) : YearDifference (deathDate, birthDate);
        if (yearDiff >= 0 and yearDiff < 2) {
            float   yearDiffF   =   deathDate.empty () ? YearDifferenceF (DateTime::GetToday (), birthDate) : YearDifferenceF (deathDate, birthDate);
            int     months      =   int (yearDiffF * 12.0f + 0.4999f);
            wstring unitBase    =   abbrevUnit ? L"mo" : L"month";
            return Format (L"%d %s", months, Linguistics::PluralizeNoun (unitBase, months).c_str ());
        }
        else {
            wstring unitBase    =   abbrevUnit ? L"yr" : L"year";
            return Format (L"%d %s", yearDiff, Linguistics::PluralizeNoun (unitBase, yearDiff).c_str ());
        }
    }
}











