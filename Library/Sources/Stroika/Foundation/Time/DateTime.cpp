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

#include "Duration.h"

#include "DateTime.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Time;

namespace {
    constexpr int kSecondsPerMinute_ = 60;
    constexpr int kSecondsPerHour_   = 60 * kSecondsPerMinute_;
    constexpr int kSecondsPerDay_    = 24 * kSecondsPerHour_;
}

/*
 *  Subtle implementation note:
 *    http://www.cplusplus.com/reference/ctime/tm/
 *
 *          tm.year is years  since 1900!
 */

#if qPlatform_Windows
namespace {
    TimeOfDay mkTimeOfDay_ (const SYSTEMTIME& sysTime)
    {
        WORD hour   = max (sysTime.wHour, static_cast<WORD> (0));
        hour        = min (hour, static_cast<WORD> (23));
        WORD minute = max (sysTime.wMinute, static_cast<WORD> (0));
        minute      = min (minute, static_cast<WORD> (59));
        WORD secs   = max (sysTime.wSecond, static_cast<WORD> (0));
        secs        = min (secs, static_cast<WORD> (59));
        return TimeOfDay ((hour * 60 + minute) * 60 + secs);
    }
    Date mkDate_ (const SYSTEMTIME& sysTime)
    {
        return Date (Year (sysTime.wYear), MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay));
    }
}
#endif

namespace {
#if qPlatform_Windows
    SYSTEMTIME toSysTime_ (TimeOfDay tod)
    {
        SYSTEMTIME t{};
        if (not tod.empty ()) {
            unsigned int seconds = tod.GetAsSecondsCount ();
            unsigned int minutes = seconds / 60;
            unsigned int hours   = minutes / 60;

            hours   = min (hours, 23U);
            t.wHour = static_cast<WORD> (hours);

            minutes -= hours * 60;
            minutes   = min (minutes, 59U);
            t.wMinute = static_cast<WORD> (minutes);

            seconds -= (60 * 60 * hours + 60 * minutes);
            seconds   = min (seconds, 59U);
            t.wSecond = static_cast<WORD> (seconds);
        }
        return t;
    }
#endif
}

namespace {
    inline constexpr uint32_t GetSecondCount_ (const optional<TimeOfDay>& tod)
    {
        return tod.has_value () ? tod->GetAsSecondsCount () : 0;
    }
}

namespace {
#if qPlatform_Windows
    SYSTEMTIME toSYSTEM_ (const Date& date)
    {
        SYSTEMTIME  st{};
        MonthOfYear m = MonthOfYear::eEmptyMonthOfYear;
        DayOfMonth  d = DayOfMonth::eEmptyDayOfMonth;
        Year        y = Year::eEmptyYear;
        date.mdy (&m, &d, &y);
        st.wYear  = static_cast<WORD> (y);
        st.wMonth = static_cast<WORD> (m);
        st.wDay   = static_cast<WORD> (d);
        return st;
    }
#endif
}

namespace {
    // @todo add error checking - so returns -1 outside UNIX EPOCH TIME
    static time_t mkgmtime_ (const struct tm* ptm)
    {
        // On GLIBC systems, could use _mkgmtime64  - https://github.com/leelwh/clib/blob/master/c/mktime64.c
        // Based on https://stackoverflow.com/questions/12353011/how-to-convert-a-utc-date-time-to-a-time-t-in-c
        constexpr int kDaysOfMonth_[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        auto          isLeapYear        = [](int year) -> bool {
            if (year % 4 != 0)
                return false;
            if (year % 100 != 0)
                return true;
            return (year % 400) == 0;
        };

        time_t secs = 0;
        // tm_year is years since 1900
        int year = ptm->tm_year + 1900;
        for (int y = 1970; y < year; ++y) {
            secs += (isLeapYear (y) ? 366 : 365) * kSecondsPerDay_;
        }
        // tm_mon is month from 0..11
        for (int m = 0; m < ptm->tm_mon; ++m) {
            secs += kDaysOfMonth_[m] * kSecondsPerDay_;
            if (m == 1 && isLeapYear (year))
                secs += kSecondsPerDay_;
        }
        secs += (ptm->tm_mday - 1) * kSecondsPerDay_;
        secs += ptm->tm_hour * kSecondsPerHour_;
        secs += ptm->tm_min * kSecondsPerMinute_;
        secs += ptm->tm_sec;
#if qCompilerAndStdLib_Supported_mkgmtime64
        Assert (_mkgmtime64 (const_cast<struct tm*> (ptm)) == secs);
#endif
        return secs;
    }
}

/*
 ********************************************************************************
 *********************************** DateTime ***********************************
 ********************************************************************************
 */
#if qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
const DateTime DateTime::kMin = DateTime::min (); // deprecated
const DateTime DateTime::kMax = DateTime::max (); // deprecated
#else
const DateTime DateTime::kMin;
const DateTime DateTime::kMax;
#endif

DateTime::DateTime (time_t unixEpochTime) noexcept
    : fTimezone_ (Timezone::UTC ())
    , fDate_ ()
    , fTimeOfDay_ ()
{
    struct tm tmTime {
    };
#if qPlatform_Windows
    (void)::_gmtime64_s (&tmTime, &unixEpochTime);
#else
    (void)::gmtime_r (&unixEpochTime, &tmTime);
#endif
    fDate_      = Date (Year (tmTime.tm_year + 1900), MonthOfYear (tmTime.tm_mon + 1), DayOfMonth (tmTime.tm_mday));
    fTimeOfDay_ = TimeOfDay (tmTime.tm_sec + (tmTime.tm_min * 60) + (tmTime.tm_hour * 60 * 60));
}

DateTime::DateTime (const tm& tmTime, const optional<Timezone>& tz) noexcept
    : fTimezone_ (tz)
    , fDate_ (Year (tmTime.tm_year + 1900), MonthOfYear (tmTime.tm_mon + 1), DayOfMonth (tmTime.tm_mday))
    , fTimeOfDay_ ((tmTime.tm_hour * 60 + tmTime.tm_min) * 60 + tmTime.tm_sec)
{
}

#if qPlatform_POSIX
DateTime::DateTime (const timeval& tmTime, const optional<Timezone>& tz) noexcept
    : fTimezone_ (tz)
    , fDate_ ()
    , fTimeOfDay_ ()
{
    time_t    unixTime = tmTime.tv_sec; // IGNORE tv_usec FOR NOW because we currently don't support fractional seconds in DateTime
    struct tm tmTimeData {
    };
    (void)::gmtime_r (&unixTime, &tmTimeData);
    fDate_      = Date (Year (tmTimeData.tm_year + 1900), MonthOfYear (tmTimeData.tm_mon + 1), DayOfMonth (tmTimeData.tm_mday));
    fTimeOfDay_ = TimeOfDay (tmTimeData.tm_sec + (tmTimeData.tm_min * 60) + (tmTimeData.tm_hour * 60 * 60));
}

DateTime::DateTime (const timespec& tmTime, const optional<Timezone>& tz) noexcept
    : fTimezone_ (tz)
    , fDate_ ()
    , fTimeOfDay_ ()
{
    time_t    unixTime = tmTime.tv_sec; // IGNORE tv_nsec FOR NOW because we currently don't support fractional seconds in DateTime
    struct tm tmTimeData {
    };
    (void)::gmtime_r (&unixTime, &tmTimeData);
    fDate_      = Date (Year (tmTimeData.tm_year + 1900), MonthOfYear (tmTimeData.tm_mon + 1), DayOfMonth (tmTimeData.tm_mday));
    fTimeOfDay_ = TimeOfDay (tmTimeData.tm_sec + (tmTimeData.tm_min * 60) + (tmTimeData.tm_hour * 60 * 60));
}
#endif
#if qPlatform_Windows
DateTime::DateTime (const SYSTEMTIME& sysTime, const optional<Timezone>& tz) noexcept
    : fTimezone_ (tz)
    , fDate_ (mkDate_ (sysTime))
    , fTimeOfDay_ (mkTimeOfDay_ (sysTime))
{
}

DateTime::DateTime (const FILETIME& fileTime, const optional<Timezone>& tz) noexcept
    : fTimezone_ (tz)
    , fDate_ ()
    , fTimeOfDay_ ()
{
    SYSTEMTIME sysTime{};
    if (::FileTimeToSystemTime (&fileTime, &sysTime)) {
        fDate_      = mkDate_ (sysTime);
        fTimeOfDay_ = mkTimeOfDay_ (sysTime);
    }
}
#endif

DateTime DateTime::Parse (const String& rep, ParseFormat pf)
{
    if (rep.empty ()) {
        return DateTime{};
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
            return DateTime ();
        } break;
        case ParseFormat::eISO8601:
        case ParseFormat::eXML: {
            int year   = 0;
            int month  = 0;
            int day    = 0;
            int hour   = 0;
            int minute = 0;
            int second = 0;
            int tzHr   = 0;
            int tzMn   = 0;
            DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
            int  nItems  = 0;
            bool tzKnown = false;
            bool tzUTC   = false;
            if (rep[rep.length () - 1] == 'Z') {
                nItems  = ::swscanf (rep.c_str (), L"%d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hour, &minute, &second);
                tzKnown = true;
                tzUTC   = true;
            }
            else {
                wchar_t plusMinusChar{};
                nItems  = ::swscanf (rep.c_str (), L"%d-%d-%dT%d:%d:%d%c%d:%d", &year, &month, &day, &hour, &minute, &second, &plusMinusChar, &tzHr, &tzMn);
                tzKnown = (nItems >= 9) and (plusMinusChar == '+' or plusMinusChar == '-');
                if (not tzKnown and nItems >= 6) {
                    nItems = ::swscanf (rep.c_str (), L"%d-%d-%dT%d:%d:%d%c%2d%2d", &year, &month, &day, &hour, &minute, &second, &plusMinusChar, &tzHr, &tzMn);
                }
                tzKnown = (nItems >= 8) and (plusMinusChar == '+' or plusMinusChar == '-');
                if (tzKnown) {
                    if (plusMinusChar == '-') {
                        tzHr = -tzHr;
                        tzMn = -tzMn;
                    }
                }
            }
            DISABLE_COMPILER_MSC_WARNING_END (4996)
            Date      d;
            TimeOfDay t;
            if (nItems >= 3) {
                d = Date (Year (year), MonthOfYear (month), DayOfMonth (day));
            }
            if (nItems >= 5) {
                t = TimeOfDay (hour * 60 * 60 + minute * 60 + second);
            }
            optional<Timezone> tz;
            if (tzKnown) {
                if (tzUTC) {
                    tz = Timezone::UTC ();
                }
                else {
                    Assert (numeric_limits<int16_t>::min () <= tzHr * 60 + tzMn and tzHr * 60 + tzMn < numeric_limits<int16_t>::max ());
                    tz = Timezone (static_cast<int16_t> (tzHr * 60 + tzMn));
                }
            }
            // WRONG/BAD - did this up until Stroika 2.1d6 (and 2.0b6 on v2 branch)
            //else {
            //    tz = Timezone::LocalTime ();
            //}
            return t.empty () ? d : DateTime (d, t, tz);
        } break;
        case ParseFormat::eRFC1123: {
            /*
             *  From https://tools.ietf.org/html/rfc822#section-5
             *    5.1.  SYNTAX
             *
             *       date-time   =  [ day "," ] date time        ; dd mm yy
             *                                                   ;  hh:mm:ss zzz
             *     
             *       day         =  "Mon"  / "Tue" /  "Wed"  / "Thu"
             *                   /  "Fri"  / "Sat" /  "Sun"
             * 
             *       date        =  1*2DIGIT month 2DIGIT        ; day month year
             *                                                   ;  e.g. 20 Jun 82
             * 
             *       month       =  "Jan"  /  "Feb" /  "Mar"  /  "Apr"
             *                   /  "May"  /  "Jun" /  "Jul"  /  "Aug"
             *                   /  "Sep"  /  "Oct" /  "Nov"  /  "Dec"
             *
             *       time        =  hour zone                    ; ANSI and Military
             * 
             *       hour        =  2DIGIT ":" 2DIGIT [":" 2DIGIT]
             *                                                   ; 00:00:00 - 23:59:59
             *
             *       zone        =  "UT"  / "GMT"                ; Universal Time
             *                                                   ; North American : UT
             *                   /  "EST" / "EDT"                ;  Eastern:  - 5/ - 4
             *                   /  "CST" / "CDT"                ;  Central:  - 6/ - 5
             *                   /  "MST" / "MDT"                ;  Mountain: - 7/ - 6
             *                   /  "PST" / "PDT"                ;  Pacific:  - 8/ - 7
             *                   /  1ALPHA                       ; Military: Z = UT;
             *                                                   ;  A:-1; (J not used)
             *                                                   ;  M:-12; N:+1; Y:+12
             *                   / ( ("+" / "-") 4DIGIT )        ; Local differential
             *                                                   ;  hours+min. (HHMM)
             *            
             *  So we can ignore the day (string) since optional and redundant.
             */
            String tmp = rep;
            if (auto i = tmp.Find (',')) {
                tmp = tmp.SubString (*i + 1).LTrim ();
            }
            int     year   = 0;
            int     month  = 0;
            int     day    = 0;
            int     hour   = 0;
            int     minute = 0;
            int     second = 0;
            wchar_t monthStr[4]{};
            wchar_t tzStr[101]{};
            DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
            int nItems = ::swscanf (tmp.c_str (), L"%d %3s %d %d:%d:%d %100s", &day, &monthStr, &year, &hour, &minute, &second, tzStr);
            DISABLE_COMPILER_MSC_WARNING_END (4996)
            constexpr wchar_t kMonths_[12][4] = {L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"};
            for (size_t i = 0; i < NEltsOf (kMonths_); ++i) {
                if (wcscmp (monthStr, kMonths_[i]) == 0) {
                    month = i + 1; // one-based numbering
                    break;
                }
            }
            Date      d;
            TimeOfDay t;
            if (nItems >= 3) {
                d = Date (Year (year), MonthOfYear (month), DayOfMonth (day));
            }
            if (nItems >= 5) {
                t = TimeOfDay (hour * 60 * 60 + minute * 60 + second);
            }
            optional<Timezone>                              tz;
            static constexpr pair<const wchar_t*, Timezone> kNamedTimezones_[]{
                {L"Z", Timezone::UTC ()},
                {L"UT", Timezone::UTC ()},
                {L"GMT", Timezone::UTC ()},
                {L"EST", Timezone (-5 * 60)},
                {L"EDT", Timezone (-4 * 60)},
                {L"CST", Timezone (-6 * 60)},
                {L"CDT", Timezone (-5 * 60)},
                {L"MST", Timezone (-7 * 60)},
                {L"MDT", Timezone (-6 * 60)},
                {L"PST", Timezone (-8 * 60)},
                {L"PDT", Timezone (-7 * 60)},
            };
            for (size_t i = 0; i < NEltsOf (kNamedTimezones_); ++i) {
                if (wcscmp (tzStr, kNamedTimezones_[i].first) == 0) {
                    tz = kNamedTimezones_[i].second;
                    break;
                }
            }
            if (not tz.has_value ()) {
                wchar_t        plusMinusChar{};
                int            tzHr    = 0;
                int            tzMn    = 0;
                const wchar_t* tStrPtr = tzStr;
                bool           isNeg   = (*tStrPtr == '-');
                if (*tStrPtr == '+' or *tStrPtr == '-') {
                    tStrPtr++;
                }
                int nTZItems = ::swscanf (tStrPtr, L"%2d%2d", &tzHr, &tzMn);
                if (nTZItems == 2) {
                    tz = Timezone (static_cast<int16_t> (tzHr * 60 + tzMn));
                }
            }

            return t.empty () ? d : DateTime (d, t, tz);

        } break;
        default: {
            AssertNotReached ();
            return DateTime ();
        } break;
    }
}

DateTime DateTime::Parse (const String& rep, const locale& l)
{
    if (rep.empty ()) {
        return DateTime ();
    }
    const time_get<wchar_t>&     tmget = use_facet<time_get<wchar_t>> (l);
    ios::iostate                 state = ios::goodbit;
    wistringstream               iss (rep.As<wstring> ());
    istreambuf_iterator<wchar_t> itbegin (iss); // beginning of iss
    istreambuf_iterator<wchar_t> itend;         // end-of-stream
    tm                           when{};
    tmget.get_date (itbegin, itend, iss, state, &when);
    return DateTime (when);
}

#if qPlatform_Windows
DateTime DateTime::Parse (const String& rep, LCID lcid)
{
    if (rep.empty ()) {
        return DateTime{};
    }
    DATE d{};
    try {
        using namespace Execution::Platform::Windows;
        ThrowIfErrorHRESULT (::VarDateFromStr (Characters::Platform::Windows::SmartBSTR (rep.c_str ()), lcid, 0, &d));
    }
    catch (...) {
        // though COULD be time format exception?
        Execution::Throw (Date::FormatException::kThe);
    }
    // SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
    SYSTEMTIME sysTime{};
    Verify (::VariantTimeToSystemTime (d, &sysTime));
    return DateTime (mkDate_ (sysTime), mkTimeOfDay_ (sysTime));
}
#endif

DateTime DateTime::AsLocalTime () const
{
    if (GetTimezone () == Timezone::UTC ()) {
        DateTime tmp = AddSeconds (fTimezone_->GetBiasFromUTC (fDate_, fTimeOfDay_));
        return DateTime (tmp.GetDate (), tmp.GetTimeOfDay (), Timezone::LocalTime ());
    }
    else if (GetTimezone () == Timezone::LocalTime ()) {
        return *this;
    }
    else if (GetTimezone () == Timezone::Unknown ()) {
        return DateTime (GetDate (), GetTimeOfDay (), Timezone::LocalTime ());
    }
    else {
        // Convert to UTC, and then back to localtime
        return AsUTC ().AsLocalTime ();
    }
}

DateTime DateTime::AsUTC () const
{
    if (GetTimezone () == Timezone::UTC ()) {
        return *this;
    }
    else {
        DateTime tmp = fTimezone_.has_value () ? AddSeconds (-fTimezone_->GetBiasFromUTC (fDate_, fTimeOfDay_)) : *this;
        return DateTime (tmp.GetDate (), tmp.GetTimeOfDay (), Timezone::UTC ());
    }
}

DateTime DateTime::Now () noexcept
{
#if qPlatform_Windows
    SYSTEMTIME st{};
    ::GetLocalTime (&st);
    return DateTime{st, Timezone::LocalTime ()};
#elif qPlatform_POSIX
    // time() returns the time since the Epoch (00:00:00 UTC, January 1, 1970), measured in seconds.
    // Convert to LocalTime - just for symetry with the windows version (and cuz our API spec say so)
    return DateTime{::time (nullptr)}.AsLocalTime ();
#else
    AssertNotImplemented ();
    return DateTime{};
#endif
}

namespace {
    // Compute the DateTime which corresponds to a tickcount of zero.
    DateTime GetDateTimeTickCountZeroOffset_ ()
    {
        static DateTime sTimeZero_ = []() {
            DateTime now = DateTime::Now ();
            return now.AddSeconds (-static_cast<int64_t> (Time::GetTickCount ()));
        }();
        return sTimeZero_;
    }
}

DurationSecondsType DateTime::ToTickCount () const
{
    return (AsLocalTime () - GetDateTimeTickCountZeroOffset_ ()).As<DurationSecondsType> ();
}

DateTime DateTime::FromTickCount (DurationSecondsType tickCount)
{
    Assert (GetDateTimeTickCountZeroOffset_ ().GetTimezone () == Timezone::LocalTime ());
    return GetDateTimeTickCountZeroOffset_ ().AddSeconds (Math::Round<int64_t> (tickCount));
}

optional<bool> DateTime::IsDaylightSavingsTime () const
{
    if (optional<Timezone> otz = GetTimezone ()) {
        return otz->IsDaylightSavingsTime (GetDate (), GetTimeOfDay ());
    }
    return {};
}

String DateTime::Format (PrintFormat pf) const
{
    if (empty ()) {
        return String{};
    }
    switch (pf) {
        case PrintFormat::eCurrentLocale: {
            return Format (locale ());
        } break;
        case PrintFormat::eCurrentLocale_WithZerosStripped: {
            /*
             *  Not sure what todo here - becaue I'm not sure its locale neutral to put the date first, but thats
             *  what we do in Format (locale) anyhow - with the format string was pass in.
             *
             *  Good enuf for now...
             *      -- LGP 2013-03-02
             */
            String dateStr{fDate_.Format (Date::PrintFormat::eCurrentLocale_WithZerosStripped)};
            return fTimeOfDay_.empty () ? dateStr : (dateStr + L" " + fTimeOfDay_.Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped));
        }
        case PrintFormat::eISO8601:
        case PrintFormat::eXML: {
            String r = fDate_.Format ((pf == PrintFormat::eISO8601) ? Date::PrintFormat::eISO8601 : Date::PrintFormat::eXML);
            if (not fTimeOfDay_.empty ()) {
                String timeStr = fTimeOfDay_.Format ((pf == PrintFormat::eISO8601) ? TimeOfDay::PrintFormat::eISO8601 : TimeOfDay::PrintFormat::eXML);
                r += Characters::String_Constant (L"T") + timeStr;
                if (fTimezone_) {
                    if (fTimezone_ == Timezone::UTC ()) {
                        r += String_Constant (L"Z");
                    }
                    else {
                        auto tzBias     = fTimezone_->GetBiasFromUTC (fDate_, fTimeOfDay_);
                        int  minuteBias = abs (static_cast<int> (tzBias)) / 60;
                        int  hrs        = minuteBias / 60;
                        int  mins       = minuteBias - hrs * 60;
                        r += ::Format (L"%s%.2d:%.2d", (tzBias < 0 ? L"-" : L"+"), hrs, mins);
                    }
                }
            }
            return r;
        } break;
    }
    AssertNotReached ();
    return String ();
}

String DateTime::Format (const locale& l) const
{
    if (empty ()) {
        return String ();
    }
    if (GetTimeOfDay ().has_value ()) {
        // Read docs - not sure how to use this to get the local-appropriate format
        // %X MAYBE just what we want  - locale DEPENDENT!!!
        return Format (l, String_Constant{L"%x %X"});
    }
    else {
        // otherwise we get a 'datetime' of 'XXX ' - with a space at the end
        return GetDate ().Format (l);
    }
}

String DateTime::Format (const String& formatPattern) const
{
    return Format (locale (), formatPattern);
}

String DateTime::Format (const locale& l, const String& formatPattern) const
{
    // http://new.cplusplus.com/reference/std/locale/time_put/put/
    const time_put<wchar_t>& tmput = use_facet<time_put<wchar_t>> (l);
    tm                       when  = As<struct tm> ();
    wostringstream           oss;
    // Read docs - not sure how to use this to get the local-appropriate format
    // %X MAYBE just what we want  - locale DEPENDENT!!!
    constexpr wchar_t kPattern_[] = L"%x %X";
    tmput.put (oss, oss, ' ', &when, formatPattern.c_str (), formatPattern.c_str () + formatPattern.length ());
    return oss.str ();
}

#if qPlatform_Windows
String DateTime::Format (LCID lcid) const
{
    if (empty ()) {
        return String{};
    }
    else {
        String r = fDate_.Format (lcid);
        Assert (not r.empty ());
        String tod = fTimeOfDay_.Format (lcid);
        if (not tod.empty ()) {
            r += String_Constant{L" "} + tod;
        }
        return r;
    }
}
#endif

Date::JulianRepType DateTime::DaysSince () const
{
    int r = DayDifference (GetToday (), As<Date> ());
    if (r < 0) {
        return 0;
    }
    else {
        return r;
    }
}

template <>
time_t DateTime::As () const
{
    DateTime useDT = this->AsUTC (); // time_t defined in UTC
    Date     d     = useDT.GetDate ();

    if (useDT.GetDate ().GetYear () < Year (1970))
        [[UNLIKELY_ATTR]]
        {
            static const range_error kRangeErrror_{"DateTime cannot be convered to time_t - before 1970"};
            Execution::Throw (kRangeErrror_);
        }

    struct tm tm {
    };
    tm.tm_year                         = static_cast<int> (d.GetYear ()) - 1900;
    tm.tm_mon                          = static_cast<int> (d.GetMonth ()) - 1;
    tm.tm_mday                         = static_cast<int> (d.GetDayOfMonth ());
    unsigned int totalSecondsRemaining = GetSecondCount_ (useDT.GetTimeOfDay ());
    tm.tm_hour                         = totalSecondsRemaining / (60 * 60);
    totalSecondsRemaining -= tm.tm_hour * 60 * 60;
    tm.tm_min = totalSecondsRemaining / 60;
    totalSecondsRemaining -= tm.tm_min * 60;
    tm.tm_sec     = totalSecondsRemaining;
    time_t result = mkgmtime_ (&tm);
    // NB: This CAN return -1 - if outside unix EPOCH time
    return result;
}

template <>
tm DateTime::As () const
{
    if (GetDate ().GetYear () < Year (1900))
        [[UNLIKELY_ATTR]]
        {
            static const range_error kRangeErrror_{"DateTime cannot be convered to time_t - before 1900"};
            Execution::Throw (kRangeErrror_);
        }
    struct tm tm {
    };
    tm.tm_year                         = static_cast<int> (fDate_.GetYear ()) - 1900;
    tm.tm_mon                          = static_cast<int> (fDate_.GetMonth ()) - 1;
    tm.tm_mday                         = static_cast<int> (fDate_.GetDayOfMonth ());
    unsigned int totalSecondsRemaining = fTimeOfDay_.GetAsSecondsCount ();
    tm.tm_hour                         = totalSecondsRemaining / (60 * 60);
    totalSecondsRemaining -= tm.tm_hour * 60 * 60;
    Assert (0 <= totalSecondsRemaining and totalSecondsRemaining < 60 * 60); // cuz would have gone into hours
    tm.tm_min = totalSecondsRemaining / 60;
    totalSecondsRemaining -= tm.tm_min * 60;
    Assert (0 <= totalSecondsRemaining and totalSecondsRemaining < 60); // cuz would have gone into minutes
    tm.tm_sec   = totalSecondsRemaining;
    tm.tm_isdst = -1;
    Ensure (0 <= tm.tm_hour and tm.tm_hour <= 23);
    Ensure (0 <= tm.tm_min and tm.tm_min <= 59);
    Ensure (0 <= tm.tm_sec and tm.tm_sec <= 59);
    return tm;
}

#if qPlatform_POSIX
template <>
timespec DateTime::As () const
{
    timespec tspec;
    tspec.tv_sec  = As<time_t> ();
    tspec.tv_nsec = 0; // IGNORE tv_nsec because we currently don't support fractional seconds in DateTime
    return tspec;
}
#endif

#if qPlatform_Windows
template <>
SYSTEMTIME DateTime::As () const
{
    // CAN GET RID OF toSYSTEM_/toSysTime_ and just inline logic here...
    SYSTEMTIME d    = toSYSTEM_ (fDate_);
    SYSTEMTIME t    = toSysTime_ (fTimeOfDay_);
    SYSTEMTIME r    = d;
    r.wHour         = t.wHour;
    r.wMinute       = t.wMinute;
    r.wSecond       = t.wSecond;
    r.wMilliseconds = t.wMilliseconds;
    return r;
}
#endif

namespace Stroika::Foundation::Time {
    template <>
    String DateTime::As () const
    {
        return Format ();
    }
}

DateTime DateTime::Add (const Duration& d) const
{
    return AddSeconds (d.As<int64_t> ());
}

DateTime DateTime::AddDays (int days) const
{
    Date d = GetDate ();
    d      = d.AddDays (days);
    return DateTime (d, GetTimeOfDay (), GetTimezone ());
}

DateTime DateTime::AddSeconds (int64_t seconds) const
{
    /* @todo - SHOULD BE MORE CAREFUL ABOUT OVERFLOW */
    int64_t n = GetSecondCount_ (GetTimeOfDay ());
    n += seconds;
    int64_t dayDiff = 0;
    if (n < 0) {
        dayDiff = int64_t (-(-n + int64_t (TimeOfDay::kMaxSecondsPerDay) - 1) / int64_t (TimeOfDay::kMaxSecondsPerDay));
        Assert (dayDiff < 0);
    }
    n -= dayDiff * static_cast<int64_t> (TimeOfDay::kMaxSecondsPerDay);
    Assert (n >= 0);

    // Now see if we overflowed
    if (n >= static_cast<int64_t> (TimeOfDay::kMaxSecondsPerDay)) {
        Assert (dayDiff == 0);
        dayDiff = int64_t (n / int64_t (TimeOfDay::kMaxSecondsPerDay));
        n -= dayDiff * static_cast<int64_t> (TimeOfDay::kMaxSecondsPerDay);
    }
    Assert (n >= 0);

    Ensure (0 <= n and n < static_cast<int64_t> (TimeOfDay::kMaxSecondsPerDay));
    Assert (numeric_limits<int>::lowest () <= dayDiff and dayDiff <= numeric_limits<int>::max ());
    if (n == 0) {
        return DateTime (GetDate ().AddDays (static_cast<int> (dayDiff)), GetTimeOfDay (), GetTimezone ());
    }
    else {
        return DateTime (GetDate ().AddDays (static_cast<int> (dayDiff)), TimeOfDay (static_cast<uint32_t> (n)), GetTimezone ());
    }
}

Duration DateTime::Difference (const DateTime& rhs) const
{
    if (GetTimezone () == rhs.GetTimezone ()) {
        int64_t             dayDiff         = static_cast<int64_t> (GetDate ().GetJulianRep ()) - static_cast<int64_t> (rhs.GetDate ().GetJulianRep ());
        DurationSecondsType intraDaySecDiff = static_cast<DurationSecondsType> (GetSecondCount_ (GetTimeOfDay ())) - static_cast<DurationSecondsType> (GetSecondCount_ (rhs.GetTimeOfDay ()));
        return Duration{DurationSecondsType (kSecondsPerDay_ * dayDiff) + intraDaySecDiff};
    }
    else {
        return AsUTC ().Difference (rhs.AsUTC ());
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
    if (GetTimezone () == rhs.GetTimezone () or (GetTimezone () == Timezone::Unknown ()) or (rhs.GetTimezone () == Timezone::Unknown ())) {
        int cmp = GetDate ().Compare (rhs.GetDate ());
        if (cmp == 0) {
#if 1
            // @todo - fixup - lost simple impl when I lost use of Memory::Optional and swithc to new style compare logic
            // --LGP 2018-07-03
            if (not GetTimeOfDay ().has_value ()) {
                return rhs.GetTimeOfDay ().has_value () ? -1 : 0; // arbitrary choice - but assume if lhs is empty thats less than any T value
            }
            Assert (GetTimeOfDay ().has_value ());
            if (not rhs.GetTimeOfDay ().has_value ()) {
                return 1;
            }
            cmp = GetTimeOfDay ()->Compare (*rhs.GetTimeOfDay ());
#else
            cmp = GetTimeOfDay ().Compare (rhs.GetTimeOfDay ());
#endif
        }
        return cmp;
    }
    else {
        return AsUTC ().Compare (rhs.AsUTC ());
    }
}

DateTime Time::operator+ (const DateTime& lhs, const Duration& rhs)
{
    // Define in .cpp file to avoid #include Duration in DateTime.h
    return lhs.Add (rhs);
}

Duration Time::operator- (const DateTime& lhs, const DateTime& rhs)
{
    return lhs.Difference (rhs);
}

DateTime Time::operator- (const DateTime& lhs, const Duration& rhs)
{
    // Define in .cpp file to avoid #include Duration in DateTime.h
    return lhs.Add (-rhs);
}

/*
 ********************************************************************************
 ************************** Math::NearlyEquals **********************************
 ********************************************************************************
 */
bool Math::NearlyEquals (Time::DateTime l, Time::DateTime r)
{
    return NearlyEquals (l, r, static_cast<Time::DurationSecondsType> (1.0));
}

bool Math::NearlyEquals (Time::DateTime l, Time::DateTime r, Time::DurationSecondsType epsilon)
{
    return l == r or Math::NearlyEquals (static_cast<DurationSecondsType> (l.As<time_t> ()), static_cast<DurationSecondsType> (r.As<time_t> ()), epsilon);
}

bool Math::NearlyEquals (Time::DateTime l, Time::DateTime r, const Time::Duration& epsilon)
{
    return NearlyEquals (l, r, epsilon.As<Time::DurationSecondsType> ());
}
