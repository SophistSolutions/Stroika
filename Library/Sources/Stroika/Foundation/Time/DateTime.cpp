/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <ctime>
#include    <sstream>

#if     qPlatform_Windows
#include    <atlbase.h>     // For CComBSTR
#endif

#include    "../Characters/Format.h"
#include    "../Characters/String_Constant.h"
#include    "../Debug/Assertions.h"
#include    "../Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include    "../Memory/SmallStackBuffer.h"
#include    "../Linguistics/Words.h"
#include    "Duration.h"

#include    "DateTime.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Time;

using   Debug::TraceContextBumper;



/*
*  Subtle implementation note:
*    http://www.cplusplus.com/reference/ctime/tm/
*
*          tm.year is years  since 1900!
*/




#if     qPlatform_Windows
namespace   {
    TimeOfDay   mkTimeOfDay_ (const SYSTEMTIME& sysTime)
    {
        WORD    hour = max (sysTime.wHour, static_cast<WORD> (0));
        hour = min (hour, static_cast<WORD> (23));
        WORD    minute = max (sysTime.wMinute, static_cast<WORD> (0));
        minute = min (minute, static_cast<WORD> (59));
        WORD    secs = max (sysTime.wSecond, static_cast<WORD> (0));
        secs = min (secs, static_cast<WORD> (59));
        return TimeOfDay ((hour * 60 + minute) * 60 + secs);
    }
    Date    mkDate_ (const SYSTEMTIME& sysTime)
    {
        return Date (Year (sysTime.wYear), MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay));
    }
}
#endif



namespace   {
#if     qPlatform_Windows
    SYSTEMTIME  toSysTime_ (TimeOfDay tod)
    {
        SYSTEMTIME  t;
        memset (&t, 0, sizeof (t));
        if (not tod.empty ()) {
            unsigned int    seconds =   tod.GetAsSecondsCount ();
            unsigned int    minutes =   seconds / 60;
            unsigned int    hours   =   minutes / 60;

            hours = min (hours, 23U);
            t.wHour = hours;

            minutes -= hours * 60;
            minutes = min (minutes, 59U);
            t.wMinute = minutes;

            seconds -= (60 * 60 * hours + 60 * minutes);
            seconds = min (seconds, 59U);
            t.wSecond = seconds;
        }
        return t;
    }
#endif
}


namespace   {
#if     qPlatform_Windows
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
#endif
}








/*
 ********************************************************************************
 ********************* Private_::DateTime_ModuleData_ ***************************
 ********************************************************************************
 */

Time::Private_::DateTime_ModuleData_::DateTime_ModuleData_ ()
//  unclear if this should use empty time or midnight?
//  unclear if this should use end of day time or not?
    : fMin (DateTime (Date (Date::JulianRepType (Date::kMinJulianRep)), TimeOfDay (0)))
    , fMax (DateTime (Date (Date::JulianRepType (UINT_MAX - 1)), TimeOfDay (24 * 60 * 60 - 1)))
{
}



/*
 ********************************************************************************
 *********************************** DateTime ***********************************
 ********************************************************************************
 */
#if 0
namespace   {
    // Once we have all our compilers supporting constexp - this can go in header (Date.inl) - and then be shared
    // across OBJS
    const   Date    kMin_   =   Date (Date::JulianRepType (Date::kMinJulianRep));   //year eFirstYear  - January
    const   Date    kMax_   =   Date (Date::JulianRepType (UINT_MAX - 1));

    const   TimeOfDay   kMinT_  =   TimeOfDay (0);
    const   TimeOfDay   kMaxT_  =   TimeOfDay (24 * 60 * 60 - 1);
}
#endif
;
const   DateTime&    DateTime::kMin  =   Execution::ModuleInitializer<Time::Private_::DateTime_ModuleData_>::Actual ().fMin;
const   DateTime&    DateTime::kMax  =   Execution::ModuleInitializer<Time::Private_::DateTime_ModuleData_>::Actual ().fMax;

DateTime::DateTime (time_t unixTime, Timezone tz)
    : fTimezone_ (tz)
    , fDate_ ()
    , fTimeOfDay_ ()
{
    struct  tm  tmTime;
    memset (&tmTime, 0, sizeof (tmTime));
#if qPlatform_Windows
    (void)::_gmtime64_s (&tmTime, &unixTime);
#else
    (void)::gmtime_r  (&unixTime, &tmTime);
#endif
    fDate_ = Date (Year (tmTime.tm_year + 1900), MonthOfYear (tmTime.tm_mon + 1), DayOfMonth (tmTime.tm_mday));
    fTimeOfDay_ = TimeOfDay (tmTime.tm_sec + (tmTime.tm_min * 60) + (tmTime.tm_hour * 60 * 60));
}

DateTime::DateTime (const tm& tmTime, Timezone tz)
    : fTimezone_ (tz)
    , fDate_ (Year (tmTime.tm_year + 1900), MonthOfYear (tmTime.tm_mon + 1), DayOfMonth (tmTime.tm_mday))
    , fTimeOfDay_ ((tmTime.tm_hour * 60 + tmTime.tm_min) * 60 + tmTime.tm_sec)
{
}

#if     qPlatform_POSIX
DateTime::DateTime (const timespec& tmTime, Timezone tz)
    : fTimezone_ (tz)
    , fDate_ ()
    , fTimeOfDay_ ()
{
    time_t      unixTime    =   tmTime.tv_sec;          // IGNORE tv_nsec because we currently don't support fractional seconds in DateTime
    struct  tm  tmTimeData;
    memset (&tmTimeData, 0, sizeof (tmTimeData));
#if qPlatform_Windows
    (void)::_gmtime64_s (&tmTimeData, &unixTime);
#else
    (void)::gmtime_r  (&unixTime, &tmTimeData);
#endif
    fDate_ = Date (Year (tmTimeData.tm_year + 1900), MonthOfYear (tmTimeData.tm_mon + 1), DayOfMonth (tmTimeData.tm_mday));
    fTimeOfDay_ = TimeOfDay (tmTimeData.tm_sec + (tmTimeData.tm_min * 60) + (tmTimeData.tm_hour * 60 * 60));
}
#endif
#if     qPlatform_Windows
DateTime::DateTime (const SYSTEMTIME& sysTime, Timezone tz)
    : fTimezone_ (tz)
    , fDate_ (mkDate_ (sysTime))
    , fTimeOfDay_ (mkTimeOfDay_ (sysTime))
{
}

DateTime::DateTime (const FILETIME& fileTime, Timezone tz)
    : fTimezone_ (tz)
    , fDate_ ()
    , fTimeOfDay_ ()
{
    SYSTEMTIME sysTime;
    (void)::memset (&sysTime, 0, sizeof (sysTime));
    if (::FileTimeToSystemTime (&fileTime, &sysTime)) {
        fDate_ = mkDate_ (sysTime);
        fTimeOfDay_ = mkTimeOfDay_ (sysTime);
    }
}
#endif

DateTime    DateTime::Parse (const String& rep, ParseFormat pf)
{
    if (rep.empty ()) {
        return DateTime ();
    }
    switch (pf) {
        case    ParseFormat::eCurrentLocale: {
#if         qPlatform_Windows
                /*
                 * Windows Parser does better job than POSIX one - for reasons which elude me.
                 * Automated test has some test cases to help close the gap...
                 *      -- LGP 2011-10-08
                 */
                return Parse (rep, LOCALE_USER_DEFAULT);
#else
                return Parse (rep, locale ());
#endif
                return DateTime ();
            }
            break;
        case    ParseFormat::eISO8601:
        case    ParseFormat::eXML: {
                int year    =   0;
                int month   =   0;
                int day     =   0;
                int hour    =   0;
                int minute  =   0;
                int second  =   0;
                int tzHr    =   0;
                int tzMn    =   0;
                DISABLE_COMPILER_MSC_WARNING_START(4996)      // MSVC SILLY WARNING ABOUT USING swscanf_s
                int nItems  =   0;
                bool    tzKnown =   false;
                bool    tzUTC   =   false;
                if (rep[rep.length () - 1] == 'Z') {
                    nItems = ::swscanf (rep.c_str (), L"%d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hour, &minute, &second);
                    tzKnown = true;
                    tzUTC = true;
                }
                else {
                    nItems = ::swscanf (rep.c_str (), L"%d-%d-%dT%d:%d:%d-%d:%d", &year, &month, &day, &hour, &minute, &second, &tzHr, &tzMn);
                    tzKnown = (nItems >= 7);
                }
                DISABLE_COMPILER_MSC_WARNING_END(4996)
                Date        d;
                TimeOfDay   t;
                if (nItems >= 3) {
                    d = Date (Year (year), MonthOfYear (month), DayOfMonth (day));
                }
                if (nItems >= 5) {
                    t = TimeOfDay (hour * 60 * 60 + minute * 60 + second);
                }
                Timezone    tz  =   Timezone::eUnknown;
                if (tzKnown) {
                    if (tzUTC) {
                        tz = Timezone::eUTC;   // really wrong - should map given time to UTC??? - check HR value ETC
                    }
                    else {
                        tz = Timezone::eLocalTime; // really wrong -- we're totally ignoring the TZ +xxx info! Not sure what todo with it though...
                    }

                    // CHECK TZ
                    // REALLY - must check TZ - but must adjust value if currentmachine timezone differs from one found in file...
                    // not sure what todo if READ tz doesn't match localtime? Maybe convert to GMT??
                }
                else {
                    tz = Timezone::eLocalTime;
                }
                return DateTime (d, t, tz);
            }
            break;
        default: {
                AssertNotReached ();
                return DateTime ();
            }
            break;
    }
}

DateTime    DateTime::Parse (const String& rep, const locale& l)
{
    if (rep.empty ()) {
        return DateTime ();
    }
    const time_get<wchar_t>& tmget = use_facet <time_get<wchar_t>> (l);
    ios::iostate state  =   ios::goodbit;
    wistringstream iss (rep.As<wstring> ());
    istreambuf_iterator<wchar_t> itbegin (iss);  // beginning of iss
    istreambuf_iterator<wchar_t> itend;          // end-of-stream
    tm when;
    memset (&when, 0, sizeof (when));
    tmget.get_date (itbegin, itend, iss, state, &when);
#if     qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy
    // This is a crazy correction. I have almost no idea why (unless its some Y2K workaround gone crazy). I hope this fixes it???
    // -- LGP 2011-10-09
    if (not (-200 <= when.tm_year and when.tm_year < 200)) {
        when.tm_year -= 1900;
    }
#endif
    return DateTime (when);
}

#if     qPlatform_Windows
DateTime    DateTime::Parse (const String& rep, LCID lcid)
{
    if (rep.empty ()) {
        return DateTime ();
    }
#if     qCompilerAndStdLib_VarDateFromStrOnFirstTry_Buggy
    {
        static  bool    sDidOnce_ = false;
        if (not sDidOnce_) {
            DATE        d;
            (void)::memset(&d, 0, sizeof (d));
            ::VarDateFromStr(CComBSTR(L"7/26/1972 12:00:00 AM"), 1024, 0, &d);
            sDidOnce_ = true;
        }
    }
#endif
    DATE        d;
    (void)::memset (&d, 0, sizeof (d));
    try {
        ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (rep.c_str ()), lcid, 0, &d));
    }
    catch (...) {
        // though COULD be time format exception?
        Execution::DoThrow (Date::FormatException ());
    }
    // SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
    SYSTEMTIME  sysTime;
    memset (&sysTime, 0, sizeof (sysTime));
    Verify (::VariantTimeToSystemTime (d, &sysTime));
    return DateTime (mkDate_ (sysTime),  mkTimeOfDay_ (sysTime));
}
#endif

DateTime    DateTime::AsLocalTime () const
{
    if (GetTimezone () == Timezone::eUTC) {
        DateTime    tmp =   AddSeconds (-GetLocaltimeToGMTOffset (*this));
        return DateTime (tmp.GetDate (), tmp.GetTimeOfDay (), Timezone::eLocalTime);
    }
    else {
        // treat BOTH unknown and localetime as localtime
        return *this;
    }
}

DateTime    DateTime::AsUTC () const
{
    if (GetTimezone () == Timezone::eUTC) {
        return *this;
    }
    else {
        DateTime    tmp =   AddSeconds (GetLocaltimeToGMTOffset (*this));
        return DateTime (tmp.GetDate (), tmp.GetTimeOfDay (), Timezone::eUTC);
    }
}

DateTime    DateTime::Now ()
{
#if     qPlatform_Windows
    SYSTEMTIME  st;
    memset (&st, 0, sizeof (st));
    ::GetLocalTime (&st);
    return DateTime (st, Timezone::eLocalTime);
#elif   qPlatform_POSIX
    // time() returns the time since the Epoch (00:00:00 UTC, January 1, 1970), measured in seconds.
    // Convert to LocalTime - just for symetry with the windows version (and cuz our API spec say so)
    return DateTime (time (nullptr), Timezone::eUTC).AsLocalTime ();
#else
    AssertNotImplemented ();
    return DateTime ();
#endif
}

namespace {
    // Compute the DateTime which corresponds to a tickcount of zero.
    DateTime    GetTimeZeroOffset_ ()
    {
        static  DateTime sTimeZero_ = [] () {
            DateTime    now = DateTime::Now ();
            return now.AddSeconds (-static_cast<time_t> (Time::GetTickCount ()));
        } ();
        return sTimeZero_;
    }
}

DurationSecondsType    DateTime::ToTickCount () const
{
    // quick hack impl
    return (*this - GetTimeZeroOffset_ ()).As<DurationSecondsType> ();
}

DateTime    DateTime::FromTickCount (DurationSecondsType tickCount)
{
    return GetTimeZeroOffset_ ().AddSeconds (static_cast<time_t> (round (tickCount)));
}

String DateTime::Format (PrintFormat pf) const
{
    if (empty ()) {
        return wstring ();
    }
    switch (pf) {
        case    PrintFormat::eCurrentLocale: {
                return Format (locale ());
            }
            break;
        case    PrintFormat::eCurrentLocale_WithZerosStripped:  {
                /*
                 *  Not sure what todo here - becaue I'm not sure its locale neutral to put the date first, but thats
                 *  what we do in Format (locale) anyhow - with the format string was pass in.
                 *
                 *  Good enuf for now...
                 *      -- LGP 2013-03-02
                 */
                String  dateStr { fDate_.Format (Date::PrintFormat::eCurrentLocale_WithZerosStripped) };
                return fTimeOfDay_.empty () ? dateStr : (dateStr + L" " + fTimeOfDay_.Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped));
            }
        case    PrintFormat::eISO8601:
        case    PrintFormat::eXML: {
                String  r       =   fDate_.Format ((pf == PrintFormat::eISO8601) ? Date::PrintFormat::eISO8601 : Date::PrintFormat::eXML);
                String  timeStr =   fTimeOfDay_.Format ((pf == PrintFormat::eISO8601) ? TimeOfDay::PrintFormat::eISO8601 : TimeOfDay::PrintFormat::eXML);
                if (not timeStr.empty ()) {
                    r += Characters::String_Constant (L"T") + timeStr;
                    if (GetTimezone () == Timezone::eUTC) {
                        r += String_Constant (L"Z");
                    }
                    else {
                        time_t  tzBias      =   -GetLocaltimeToGMTOffset (IsDaylightSavingsTime (*this));
                        int minuteBias      =   abs (static_cast<int> (tzBias)) / 60;
                        int hrs             =   minuteBias / 60;
                        int mins            =   minuteBias - hrs * 60;
                        r +=  ::Format (L"%s%.2d:%.2d", (tzBias < 0 ? L"-" : L"+"), hrs, mins);
                    }
                }
#if     qDebug
                {
                    // TODO:
                    //      This probably shouldn't be needed!!! - think through more carefully.
                    //          --LGP 2011-10-07
                    DateTime    parsed  =   DateTime::Parse (r, ParseFormat::eXML);
                    if (parsed.GetTimezone () != GetTimezone ()) {
                        parsed = DateTime (parsed.GetDate (), parsed.GetTimeOfDay (), parsed.GetTimezone ());
                    }
                    Assert (parsed == *this);
                }
#endif
                return r;
            }
            break;
        default: {
                AssertNotReached ();
                return String ();
            }
            break;
    }
}

String DateTime::Format (const locale& l) const
{
    if (empty ()) {
        return String ();
    }
    if (GetTimeOfDay ().empty ()) {
        // otherwise we get a 'datetime' of 'XXX ' - with a space at the end
        return GetDate ().Format (l);
    }
    // http://new.cplusplus.com/reference/std/locale/time_put/put/
    const time_put<wchar_t>& tmput = use_facet <time_put<wchar_t>> (l);
    tm when =   As<struct tm> ();
    wostringstream oss;
    // Read docs - not sure how to use this to get the local-appropriate format
    // %X MAYBE just what we want  - locale DEPENDENT!!!
    const wchar_t kPattern[] = L"%x %X";
    tmput.put (oss, oss, ' ', &when, std::begin (kPattern), std::begin (kPattern) + ::wcslen (kPattern));
    return oss.str ();
}

#if     qPlatform_Windows
String DateTime::Format (LCID lcid) const
{
    if (empty ()) {
        return String ();
    }
    else {
        String r   =   fDate_.Format (lcid);
        Assert (not r.empty ());
        String tod =   fTimeOfDay_.Format (lcid);
        if (not tod.empty ()) {
            r += String_Constant (L" ") + tod;
        }
        return r;
    }
}
#endif

Date::JulianRepType DateTime::DaysSince () const
{
    int r   =   DayDifference (GetToday (), As<Date> ());
    if (r < 0) {
        return 0;
    }
    else {
        return r;
    }
}

#if     qPlatform_Windows
namespace   {
    time_t  OLD_GetUNIXEpochTime_ (const DateTime& dt)
    {
        SYSTEMTIME  st  =   dt.As<SYSTEMTIME> ();
        struct tm tm;
        memset(&tm, 0, sizeof(tm));
        tm.tm_year = st.wYear - 1900;
        tm.tm_mon = st.wMonth - 1;
        tm.tm_mday = st.wDay;
        tm.tm_hour = st.wHour;
        tm.tm_min = st.wMinute;
        tm.tm_sec = st.wSecond;
        return mktime (&tm);
    }
}
#endif

template    <>
time_t  DateTime::As () const
{
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    tm.tm_year = static_cast<int> (fDate_.GetYear ()) - 1900;
    tm.tm_mon = static_cast<int> (fDate_.GetMonth ()) - 1;
    tm.tm_mday = static_cast<int> (fDate_.GetDayOfMonth ());
    unsigned int    totalSecondsRemaining   =   fTimeOfDay_.GetAsSecondsCount ();
    tm.tm_hour = totalSecondsRemaining / (60 * 60);
    totalSecondsRemaining -= tm.tm_hour * 60 * 60;
    tm.tm_min = totalSecondsRemaining / 60;
    totalSecondsRemaining -= tm.tm_min * 60;
    tm.tm_sec = totalSecondsRemaining;
    time_t  result  =   mktime (&tm);
#if     qPlatform_Windows
    Ensure (result == OLD_GetUNIXEpochTime_ (*this));       // OLD WINDOZE code was WRONG - neglecting the coorect for mktime () timezone nonsense
#endif
    /*
     * This is PURELY to correct for the fact that mktime() uses the current timezone - and has NOTHING todo with the timezone assocaited with teh given
     * DateTime() object.
     */
    result -= Time::GetLocaltimeToGMTOffset (false);
    return result;
}

template    <>
tm  DateTime::As () const
{
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    tm.tm_year = static_cast<int> (fDate_.GetYear ()) - 1900;
    tm.tm_mon = static_cast<int> (fDate_.GetMonth ()) - 1;
    tm.tm_mday = static_cast<int> (fDate_.GetDayOfMonth ());
    unsigned int    totalSecondsRemaining   =   fTimeOfDay_.GetAsSecondsCount ();
    tm.tm_hour = totalSecondsRemaining / (60 * 60);
    totalSecondsRemaining -= tm.tm_hour * 60 * 60;
    tm.tm_min = totalSecondsRemaining / 60;
    totalSecondsRemaining -= tm.tm_min * 60;
    tm.tm_sec = totalSecondsRemaining;
    tm.tm_isdst = -1;
    Ensure (0 <= tm.tm_hour and tm.tm_hour <= 23);
    Ensure (0 <= tm.tm_min and tm.tm_min <= 59);
    Ensure (0 <= tm.tm_sec and tm.tm_sec <= 59);
    return tm;
}

#if     qPlatform_POSIX
template    <>
timespec    DateTime::As () const
{
    timespec    tspec;
    tspec.tv_sec = As<time_t> ();
    tspec.tv_nsec = 0;                  // IGNORE tv_nsec because we currently don't support fractional seconds in DateTime
    return tspec;
}
#endif

#if     qPlatform_Windows
template    <>
SYSTEMTIME  DateTime::As () const
{
    // CAN GET RID OF toSYSTEM_/toSysTime_ and just inline logic here...
    SYSTEMTIME  d   =   toSYSTEM_ (fDate_);
    SYSTEMTIME  t   =   toSysTime_ (fTimeOfDay_);
    SYSTEMTIME  r   =   d;
    r.wHour = t.wHour;
    r.wMinute = t.wMinute;
    r.wSecond = t.wSecond;
    r.wMilliseconds = t.wMilliseconds;
    return r;
}
#endif

namespace Stroika {
    namespace Foundation {
        namespace Time {
            template    <>
            String  DateTime::As () const
            {
                return Format ();
            }
        }
    }
}

DateTime    DateTime::Add (const Duration& d) const
{
    return AddSeconds (d.As<time_t> ());
}

DateTime    DateTime::AddDays (int days) const
{
    Date    d   =   GetDate ();
    d = d.AddDays (days);
    return DateTime (d, GetTimeOfDay (), GetTimezone ());
}

DateTime    DateTime::AddSeconds (time_t seconds) const
{
    /* TODO - SHOULD BE MORE CAREFUL ABOUT OVERFLOW */
    time_t  n   =   GetTimeOfDay ().GetAsSecondsCount ();
    n += seconds;
    int dayDiff =   0;
    if (n < 0) {
        dayDiff = int (- (-n + time_t (TimeOfDay::kMaxSecondsPerDay) - 1) / time_t (TimeOfDay::kMaxSecondsPerDay));
        Assert (dayDiff < 0);
    }
    n -= dayDiff * static_cast<time_t> (TimeOfDay::kMaxSecondsPerDay);
    Assert (n >= 0);

    // Now see if we overflowed
    if (n >= static_cast<time_t> (TimeOfDay::kMaxSecondsPerDay)) {
        Assert (dayDiff == 0);
        dayDiff = int (n / time_t (TimeOfDay::kMaxSecondsPerDay));
        n -= dayDiff * static_cast<time_t> (TimeOfDay::kMaxSecondsPerDay);
    }
    Assert (n >= 0);

    Ensure (0 <= n and n < static_cast<time_t> (TimeOfDay::kMaxSecondsPerDay));
    return DateTime (GetDate ().AddDays (dayDiff), TimeOfDay (static_cast<uint32_t> (n)), GetTimezone ());
}

Duration    DateTime::Difference (const DateTime& rhs) const
{
    if (GetTimezone () == rhs.GetTimezone ()) {
        return Duration (As<time_t> () - rhs.As<time_t> ());
    }
    else {
        return Duration (AsUTC ().As<time_t> () - rhs.AsUTC ().As<time_t> ());
    }
}

int DateTime::Compare (const DateTime& rhs) const
{
    if (empty ()) {
        return rhs.empty () ? 0 : -1;
    }
    else {
        if (rhs.empty ()) {
            return 1;
        }
    }
    Assert (not empty () and not rhs.empty ());
    if (GetTimezone () == rhs.GetTimezone () or (GetTimezone () == Timezone::eUnknown)  or (rhs.GetTimezone () == Timezone::eUnknown)) {
        int cmp =   GetDate ().Compare (rhs.GetDate ());
        if (cmp == 0) {
            cmp = GetTimeOfDay ().Compare (rhs.GetTimeOfDay ());
        }
        return cmp;
    }
    else {
        return AsUTC ().Compare (rhs.AsUTC ());
    }
}

DateTime    Time::operator+ (const DateTime& lhs, const Duration& rhs)
{
    // Define in .cpp file to avoid #include Duration in DateTime.h
    return lhs.Add (rhs);
}

DateTime    Time::operator- (const DateTime& lhs, const Duration& rhs)
{
    // Define in .cpp file to avoid #include Duration in DateTime.h
    return lhs.Add (-rhs);
}

Duration    Time::operator- (const DateTime& lhs, const DateTime& rhs)
{
    // Define in .cpp file to avoid #include Duration in DateTime.h
    return lhs.Difference (rhs);
}
