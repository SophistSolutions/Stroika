/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <ctime>
#include <sstream>

#include "../Characters/Format.h"
#include "../Characters/RegularExpression.h"
#include "../Characters/ToString.h"
#include "../Debug/Assertions.h"
#include "../Execution/Exceptions.h"
#include "../Execution/Throw.h"

#include "Duration.h"

#include "DateTime.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Time;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    constexpr int kSecondsPerMinute_ = 60;
    constexpr int kSecondsPerHour_   = 60 * kSecondsPerMinute_;
    constexpr int kSecondsPerDay_    = 24 * kSecondsPerHour_;
}

namespace {
    constexpr int kTM_Year_RelativeToYear_{1900}; // see https://man7.org/linux/man-pages/man3/ctime.3.html
}

namespace {
    constexpr bool kRequireImbueToUseFacet_ =
        false; // example uses it, and code inside windows tmget seems to reference it, but no logic for this, and no clear docs (and works same either way apparently)
}

/*
 *  Subtle implementation note:
 *    http://www.cplusplus.com/reference/ctime/tm/
 *
 *          tm.year is years  since 1900 (kTM_Year_RelativeToYear_)
 */

#if qPlatform_Windows
namespace {
    TimeOfDay mkTimeOfDay_ (const ::SYSTEMTIME& sysTime)
    {
        ::WORD hour   = max (sysTime.wHour, static_cast<WORD> (0));
        hour          = min (hour, static_cast<WORD> (23));
        ::WORD minute = max (sysTime.wMinute, static_cast<WORD> (0));
        minute        = min (minute, static_cast<WORD> (59));
        ::WORD secs   = max (sysTime.wSecond, static_cast<WORD> (0));
        secs          = min (secs, static_cast<WORD> (59));
        return TimeOfDay{hour, minute, secs, DataExchange::ValidationStrategy::eThrow};
    }
    Date mkDate_ (const SYSTEMTIME& sysTime)
    {
        return Date{Year (sysTime.wYear), MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay), DataExchange::ValidationStrategy::eThrow};
    }
}
#endif

namespace {
#if qPlatform_Windows
    ::SYSTEMTIME toSysTime_ (TimeOfDay tod)
    {
        ::SYSTEMTIME t{};
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
    ::SYSTEMTIME toSYSTEM_ (const Date& date)
    {
        ::SYSTEMTIME st{};
        st.wMonth = static_cast<::WORD> (static_cast<unsigned int> (date.As<year_month_day> ().month ()));
        st.wDay   = static_cast<::WORD> (static_cast<unsigned int> (date.As<year_month_day> ().day ()));
        st.wYear  = static_cast<::WORD> (static_cast<int> (date.As<year_month_day> ().year ()));
        return st;
    }
#endif
}

namespace {
    // @todo add error checking - so returns -1 outside UNIX EPOCH TIME
    time_t mkgmtime_ (const tm* ptm)
    {
        // On GLIBC systems, could use _mkgmtime64  - https://github.com/leelwh/clib/blob/master/c/mktime64.c
        // Based on https://stackoverflow.com/questions/12353011/how-to-convert-a-utc-date-time-to-a-time-t-in-c
        constexpr int kDaysOfMonth_[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        time_t secs = 0;
        int    year = ptm->tm_year + kTM_Year_RelativeToYear_;
        for (int y = 1970; y < year; ++y) {
            secs += (chrono::year{y}.is_leap () ? 366 : 365) * kSecondsPerDay_;
        }
        // tm_mon is month from 0..11
        for (int m = 0; m < ptm->tm_mon; ++m) {
            secs += kDaysOfMonth_[m] * kSecondsPerDay_;
            if (m == 1 && chrono::year{year}.is_leap ())
                secs += kSecondsPerDay_;
        }
        secs += (ptm->tm_mday - 1) * kSecondsPerDay_;
        secs += ptm->tm_hour * kSecondsPerHour_;
        secs += ptm->tm_min * kSecondsPerMinute_;
        secs += ptm->tm_sec;
#if qCompilerAndStdLib_Supported_mkgmtime64
        Assert (_mkgmtime64 (const_cast<tm*> (ptm)) == secs);
#endif
        return secs;
    }
}

/*
 ********************************************************************************
 ********************** DateTime::FormatException *******************************
 ********************************************************************************
 */
DateTime::FormatException::FormatException ()
    : RuntimeErrorException<>{"Invalid DateTime Format"sv}
{
}

/*
 ********************************************************************************
 *********************************** DateTime ***********************************
 ********************************************************************************
 */
DateTime::DateTime (time_t unixEpochTime) noexcept
    : fTimezone_{Timezone::kUTC}
    , fDate_{Date::kMinJulianRep} // avoid initialization warning
{
    ::tm tmTime{};
#if qPlatform_Windows
    (void)::_gmtime64_s (&tmTime, &unixEpochTime);
#else
    (void)::gmtime_r (&unixEpochTime, &tmTime);
#endif
    fDate_ = Date{Year (tmTime.tm_year + kTM_Year_RelativeToYear_), MonthOfYear (tmTime.tm_mon + 1), DayOfMonth (tmTime.tm_mday),
                  DataExchange::ValidationStrategy::eThrow};
    fTimeOfDay_ = TimeOfDay{static_cast<unsigned> (tmTime.tm_hour), static_cast<unsigned> (tmTime.tm_min), static_cast<unsigned> (tmTime.tm_sec)};
}

DateTime::DateTime (const ::tm& tmTime, const optional<Timezone>& tz) noexcept
    : fTimezone_{tz}
    , fDate_{Year (tmTime.tm_year + kTM_Year_RelativeToYear_), MonthOfYear (tmTime.tm_mon + 1), DayOfMonth (tmTime.tm_mday)}
    , fTimeOfDay_{TimeOfDay{static_cast<unsigned> (tmTime.tm_hour), static_cast<unsigned> (tmTime.tm_min),
                            static_cast<unsigned> (tmTime.tm_sec), DataExchange::ValidationStrategy::eThrow}}
{
}

DateTime::DateTime (const ::timespec& tmTime, const optional<Timezone>& tz) noexcept
    : fTimezone_{tz}
    , fDate_{Date::kMinJulianRep} // avoid initialization warning
{
    time_t unixTime = tmTime.tv_sec; // IGNORE tv_nsec FOR NOW because we currently don't support fractional seconds in DateTime
#if qPlatform_POSIX
    ::tm  tmTimeDataBuf{};
    ::tm* tmTimeData = ::gmtime_r (&unixTime, &tmTimeDataBuf);
#elif qPlatform_Windows
    ::tm tmTimeDataBuf{};
    if (errno_t e = ::gmtime_s (&tmTimeDataBuf, &unixTime)) {
        ThrowPOSIXErrNo (e);
    };
    ::tm*        tmTimeData = &tmTimeDataBuf;
#else
    ::tm* tmTimeData = ::gmtime (&unixTime); // not threadsafe
#endif
    fDate_      = Date{Year (tmTimeData->tm_year + kTM_Year_RelativeToYear_), MonthOfYear (tmTimeData->tm_mon + 1),
                  DayOfMonth (tmTimeData->tm_mday), DataExchange::ValidationStrategy::eThrow};
    fTimeOfDay_ = TimeOfDay{static_cast<unsigned> (tmTimeData->tm_hour), static_cast<unsigned> (tmTimeData->tm_min),
                            static_cast<unsigned> (tmTimeData->tm_sec), DataExchange::ValidationStrategy::eThrow};
}

#if qPlatform_POSIX
DateTime::DateTime (const timeval& tmTime, const optional<Timezone>& tz) noexcept
    : fTimezone_{tz}
    , fDate_{Date::kMinJulianRep} // avoid initialization warning
{
    time_t unixTime = tmTime.tv_sec; // IGNORE tv_usec FOR NOW because we currently don't support fractional seconds in DateTime
    tm     tmTimeData{};
    (void)::gmtime_r (&unixTime, &tmTimeData);
    fDate_      = Date{Year (tmTimeData.tm_year + kTM_Year_RelativeToYear_), MonthOfYear (tmTimeData.tm_mon + 1),
                  DayOfMonth (tmTimeData.tm_mday), DataExchange::ValidationStrategy::eThrow};
    fTimeOfDay_ = TimeOfDay{static_cast<unsigned> (tmTimeData.tm_hour), static_cast<unsigned> (tmTimeData.tm_min),
                            static_cast<unsigned> (tmTimeData.tm_sec), DataExchange::ValidationStrategy::eThrow};
}
#endif

#if qPlatform_Windows
DateTime::DateTime (const ::SYSTEMTIME& sysTime, const optional<Timezone>& tz) noexcept
    : fTimezone_{tz}
    , fDate_{mkDate_ (sysTime)}
    , fTimeOfDay_{mkTimeOfDay_ (sysTime)}
{
}

DateTime::DateTime (const ::FILETIME& fileTime, const optional<Timezone>& tz) noexcept
    : fTimezone_{tz}
    , fDate_{Date::kMinJulianRep} // avoid initialization warning
{
    ::SYSTEMTIME sysTime{};
    if (::FileTimeToSystemTime (&fileTime, &sysTime)) {
        fDate_      = mkDate_ (sysTime);
        fTimeOfDay_ = mkTimeOfDay_ (sysTime);
    }
}
#endif

DateTime DateTime::Parse (const String& rep, LocaleIndependentFormat format)
{
    size_t nCharsConsumed;
    if (auto o = ParseQuietly (rep, format, &nCharsConsumed); o and nCharsConsumed == rep.size ()) {
        return *o;
    }
    Execution::Throw (FormatException::kThe);
}

DateTime DateTime::Parse (const String& rep, const locale& l, const String& formatPattern)
{
    if (rep.empty ()) [[unlikely]] {
        Execution::Throw (FormatException::kThe);
    }
    size_t nCharsConsumed;
    if (auto o = ParseQuietly_ (rep.As<wstring> (), use_facet<time_get<wchar_t>> (l), formatPattern, &nCharsConsumed);
        o and nCharsConsumed == rep.size ()) {
        return *o;
    }
    Execution::Throw (FormatException::kThe);
}

DateTime DateTime::Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns)
{
    if (rep.empty ()) [[unlikely]] {
        Execution::Throw (FormatException::kThe);
    }
    wstring                  wRep  = rep.As<wstring> ();
    const time_get<wchar_t>& tmget = use_facet<time_get<wchar_t>> (l);
    for (const auto& formatPattern : formatPatterns) {
        size_t nCharsConsumed;
        if (auto o = ParseQuietly_ (wRep, tmget, formatPattern, &nCharsConsumed); o and nCharsConsumed == rep.size ()) {
            return *o;
        }
    }
    Execution::Throw (FormatException::kThe);
}

DateTime DateTime::Parse (const String& rep, const String& formatPattern)
{
    return Parse (rep, locale{}, formatPattern);
}

optional<DateTime> DateTime::ParseQuietly (const String& rep, LocaleIndependentFormat format, size_t* consumedCharacters)
{
    if (rep.empty ()) [[unlikely]] {
        return nullopt;
    }
    switch (format) {
        case LocaleIndependentFormat::eISO8601: {
            // SEE BNF in https://datatracker.ietf.org/doc/html/rfc3339#section-5.6
            int numCharsConsumed{};
            // full-date part
            optional<Date> d;
            {
                int year{};
                int month{};
                int day{};
                DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
                int nItems = ::swscanf (rep.As<wstring> ().c_str (), L"%d-%d-%d%n", &year, &month, &day, &numCharsConsumed);
                DISABLE_COMPILER_MSC_WARNING_END (4996)
                if (nItems < 3 or numCharsConsumed < 8) [[unlikely]] {
                    return nullopt;
                }
                d = Date{Year{year}, MonthOfYear (month), DayOfMonth (day)};
            }
            Assert (d);
            optional<TimeOfDay> t;
            {
                String         timePart        = rep.SubString (numCharsConsumed);
                const wchar_t* startOfTimePart = timePart.c_str ();
                // nb: OK to not check strlen cuz string NUL terminated
                // https://www.rfc-editor.org/rfc/rfc822#section-5 says can be upper or lower case T, or even ' ', but 'T' preferred/most common/recommended
                if (*startOfTimePart == 'T' or *startOfTimePart == 't' or *startOfTimePart == ' ') [[likely]] {
                    ++numCharsConsumed;
                    int   hour{};
                    int   minute{};
                    int   second{};
                    int   ncc{};
                    float secsFloat{};
                    DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
                    int nItems = ::swscanf (startOfTimePart + 1, L"%d:%d:%f%n", &hour, &minute, &secsFloat, &ncc);
                    DISABLE_COMPILER_MSC_WARNING_END (4996)
                    if (nItems == 3 and ncc >= 8) {
                        // for now we only support integral number of seconds, but allow reading to not fail if fractions given
                        second = static_cast<int> (secsFloat);
                    }
                    else {
                        return nullopt;
                    }
                    numCharsConsumed += ncc; // @todo fix - this is count of wchar_t not necessilarly full 'char32_t' characters
                    t = TimeOfDay{static_cast<unsigned> (hour), static_cast<unsigned> (minute), static_cast<unsigned> (second),
                                  DataExchange::ValidationStrategy::eThrow};
                }
            }
            // see about timezone (aka time-offset)
            optional<Timezone> tz;
            if (t) { // only can be present - so only check - if there is a time
                String         tzArea      = rep.SubString (numCharsConsumed);
                const wchar_t* startTZArea = tzArea.c_str ();
                if (*startTZArea == 'Z' or *startTZArea == 'z') { // nb: OK to not check strlen cuz string NUL terminated
                    tz = Timezone::kUTC;
                    numCharsConsumed += 1;
                }
                else {
                    int     tzHr{};
                    int     tzMn{};
                    wchar_t plusMinusChar{};
                    int     ncc{};
                    DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
                    int nItems = ::swscanf (startTZArea, L"%c%d:%d%n", &plusMinusChar, &tzHr, &tzMn, &ncc);
                    DISABLE_COMPILER_MSC_WARNING_END (4996)
                    if ((nItems == 3) and (plusMinusChar == '+' or plusMinusChar == '-')) {
                        if (plusMinusChar == '-') {
                            tzHr = -tzHr;
                            tzMn = -tzMn;
                        }
                        tz = Timezone{static_cast<int16_t> (tzHr * 60 + tzMn), DataExchange::ValidationStrategy::eThrow};
                        numCharsConsumed += ncc;
                    }
                    else if ((nItems == 2) and (plusMinusChar == '+' or plusMinusChar == '-')) {
                        // TZ can be -400 instead of -4:00
                        DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
                        nItems = ::swscanf (startTZArea, L"%c%d%n", &plusMinusChar, &tzMn, &ncc);
                        DISABLE_COMPILER_MSC_WARNING_END (4996)
                        // According to https://en.wikipedia.org/wiki/UTC_offset#:~:text=The%20UTC%20offset%20is%20the,%2C%20or%20%C2%B1%5Bhh%5D a 4 digit timezone
                        // offset means HHMM, so adjust
                        switch (ncc) {
                            case 3:
                                tzMn *= 60;
                                break;
                            case 4:
                            case 5: {
                                int hrs = tzMn / 100;
                                int min = tzMn % 100;
                                tzMn    = hrs * 60 + min;
                            } break;
                            default:
                                return nullopt;
                        }
                        if (plusMinusChar == '-') {
                            tzMn = -tzMn;
                        }
                        tz = Timezone{static_cast<int16_t> (tzMn), DataExchange::ValidationStrategy::eThrow};
                        numCharsConsumed += ncc;
                    }
                    else {
                        // if nItems == 0, this is OK, just means not specified. Else probbaly an issue, but caught by checking number of charcters consumed
                    }
                }
            }
            Assert (0 <= numCharsConsumed and numCharsConsumed <= static_cast<int> (rep.length ()));
            if (consumedCharacters != nullptr) {
                *consumedCharacters = numCharsConsumed;
            }
            return t.has_value () ? DateTime{*d, t, tz} : *d;
        } break;
        case LocaleIndependentFormat::eRFC1123: {
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
             */
            unsigned int numCharsConsumed{};
            String       tmp = rep;
            if (auto i = tmp.Find (',')) {
                tmp = tmp.SubString (*i + 1).LTrim (); // we can ignore the day of the week (string) since optional and redundant.
                numCharsConsumed += static_cast<unsigned int> (rep.length () - tmp.length ());
            }
            int     year{};
            int     month{};
            int     day{};
            int     hour{};
            int     minute{};
            int     second{};
            wchar_t monthStr[4]{};
            wchar_t tzStr[101]{};
            int     nItems;
            {
                int ncc{};
                DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
                nItems = ::swscanf (tmp.c_str (), L"%d %3ls %d %d:%d:%d %100ls%n", &day, &monthStr, &year, &hour, &minute, &second, &tzStr, &ncc);
                DISABLE_COMPILER_MSC_WARNING_END (4996)

                // tzStr captures the first token after the time, but there are often extra (ignored) tokens
                // (e.g. +400 (PST))
                // So just pretend we used the entire string
                if (nItems == 7) {
                    // ncc += 1 + ::wcslen (tzStr);
                    ncc = static_cast<int> (tmp.size ());
                }
                numCharsConsumed += ncc;
            }

            constexpr wchar_t kMonths_[12][4] = {L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
                                                 L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"};
            for (size_t i = 0; i < NEltsOf (kMonths_); ++i) {
                if (::wcscmp (monthStr, kMonths_[i]) == 0) {
                    month = static_cast<int> (i + 1); // one-based numbering
                    break;
                }
            }
            if (nItems < 3) {
                return nullopt;
            }
            Date                d = Date{Year (year), MonthOfYear (month), DayOfMonth (day), DataExchange::ValidationStrategy::eThrow};
            optional<TimeOfDay> t;
            if (nItems >= 5) {
                t = TimeOfDay{static_cast<unsigned> (hour), static_cast<unsigned> (minute), static_cast<unsigned> (second),
                              DataExchange::ValidationStrategy::eThrow};
            }
            optional<Timezone>                       tz;
            constexpr pair<const wchar_t*, Timezone> kNamedTimezones_[]{
                {L"Z", Timezone::kUTC},      {L"UT", Timezone::kUTC},     {L"GMT", Timezone::kUTC},    {L"EST", Timezone{-5 * 60}},
                {L"EDT", Timezone{-4 * 60}}, {L"CST", Timezone{-6 * 60}}, {L"CDT", Timezone{-5 * 60}}, {L"MST", Timezone{-7 * 60}},
                {L"MDT", Timezone{-6 * 60}}, {L"PST", Timezone{-8 * 60}}, {L"PDT", Timezone{-7 * 60}}, {L"A", Timezone{-1 * 60}},
                {L"B", Timezone{-2 * 60}},   {L"C", Timezone{-3 * 60}},   {L"D", Timezone{-4 * 60}},   {L"E", Timezone{-5 * 60}},
                {L"F", Timezone{-6 * 60}},   {L"G", Timezone{-7 * 60}},   {L"H", Timezone{-8 * 60}},   {L"I", Timezone{-9 * 60}},
                {L"K", Timezone{-10 * 60}},  {L"L", Timezone{-11 * 60}},  {L"M", Timezone{-12 * 60}},  {L"N", Timezone{1 * 60}},
                {L"O", Timezone{2 * 60}},    {L"P", Timezone{3 * 60}},    {L"Q", Timezone{4 * 60}},    {L"R", Timezone{5 * 60}},
                {L"S", Timezone{6 * 60}},    {L"T", Timezone{7 * 60}},    {L"U", Timezone{8 * 60}},    {L"V", Timezone{9 * 60}},
                {L"W", Timezone{10 * 60}},   {L"X", Timezone{11 * 60}},   {L"Y", Timezone{12 * 60}},
            };
            for (size_t i = 0; i < NEltsOf (kNamedTimezones_); ++i) {
                if (::wcscmp (tzStr, kNamedTimezones_[i].first) == 0) {
                    tz = kNamedTimezones_[i].second;
                    break;
                }
            }
            if (not tz.has_value ()) {
                tz = Timezone::ParseTimezoneOffsetString (tzStr);
            }
            Assert (0 <= numCharsConsumed and numCharsConsumed <= rep.length ());
            if (consumedCharacters != nullptr) {
                *consumedCharacters = numCharsConsumed;
            }
            return t.has_value () ? DateTime{d, *t, tz} : d;
        } break;
        default: {
            AssertNotReached ();
        } break;
    }
    return nullopt;
}

optional<DateTime> DateTime::ParseQuietly_ (const wstring& rep, const time_get<wchar_t>& tmget, const String& formatPattern, size_t* consumedCharacters)
{
    Require (not rep.empty ());

    ios::iostate errState = ios::goodbit;
    tm           when{};
    size_t       nCharsConsumed{};
    {
        wstring                      formatPatternWS = formatPattern.As<wstring> ();
        wistringstream               iss{rep};
        istreambuf_iterator<wchar_t> itbegin{iss}; // beginning of iss
        istreambuf_iterator<wchar_t> i = tmget.get (itbegin, istreambuf_iterator<wchar_t>{}, iss, errState, &when, formatPatternWS.c_str (),
                                                    formatPatternWS.c_str () + formatPatternWS.length ());
        if (errState & ios::eofbit) {
            nCharsConsumed = rep.size ();
        }
        else {
            //tmphack workaround msft bug
            nCharsConsumed = static_cast<size_t> (distance (itbegin, i));
        }
    }

    if constexpr (qCompilerAndStdLib_locale_time_get_reverses_month_day_with_2digit_year_Buggy) {
        // Now that I've understood this bug better, I think I can do a better/wider workaround, not just this special case...
        if (formatPattern == "%x %X"sv) {
            // It now appears this MSFT-only issue is that if you have a 2-digit year, their %x-parse code reverses the month and day
            wistringstream               iss{rep};
            istreambuf_iterator<wchar_t> itbegin{iss};
            istreambuf_iterator<wchar_t> itend;
            errState = ios::goodbit;
            (void)tmget.get_date (itbegin, itend, iss, errState, &when); // just overwrite date portion - assume time portion remains unchanged by this
        }
    }

    if ((errState & ios::badbit) or (errState & ios::failbit)) [[unlikely]] {
        return nullopt;
    }
    Assert (0 <= nCharsConsumed and nCharsConsumed <= rep.length ());
    if (consumedCharacters != nullptr) {
        *consumedCharacters = nCharsConsumed;
    }
    // @todo probably could read TIMEZONE (occasionally) from the when output (maybe look at format string to tell if its being set)
    // SEE https://stroika.atlassian.net/browse/STK-671
    return DateTime{when, Timezone::kUnknown};
}

DateTime DateTime::AsLocalTime () const
{
    if (GetTimezone () == Timezone::kUTC) {
        DateTime tmp = AddSeconds (Timezone::kLocalTime.GetBiasFromUTC (fDate_, Memory::NullCoalesce (fTimeOfDay_, TimeOfDay{0})));
        return DateTime{tmp.GetDate (), tmp.GetTimeOfDay (), Timezone::kLocalTime};
    }
    else if (GetTimezone () == Timezone::kLocalTime) {
        return *this;
    }
    else if (GetTimezone () == Timezone::kUnknown) {
        return DateTime{GetDate (), GetTimeOfDay (), Timezone::kLocalTime};
    }
    else {
        // Convert to UTC, and then back to localtime
        return AsUTC ().AsLocalTime ();
    }
}

DateTime DateTime::AsUTC () const
{
    [[maybe_unused]] auto oldCode = [&] () {
        if (GetTimezone () == Timezone::kUTC) {
            return *this;
        }
        else {
            DateTime tmp = fTimezone_.has_value ()
                               ? AddSeconds (-fTimezone_->GetBiasFromUTC (fDate_, Memory::NullCoalesce (fTimeOfDay_, TimeOfDay{0})))
                               : *this;
            return DateTime{tmp.GetDate (), tmp.GetTimeOfDay (), Timezone::kUTC};
        }
    };
    Ensure (AsTimezone (Timezone::kUTC) == oldCode ());
    return AsTimezone (Timezone::kUTC);
}

DateTime DateTime::AsTimezone (Timezone tz) const
{
    if (GetTimezone () == tz) {
        return *this;
    }
    else {
        DateTime tmp =
            fTimezone_.has_value () ? AddSeconds (-fTimezone_->GetBiasFromUTC (fDate_, Memory::NullCoalesce (fTimeOfDay_, TimeOfDay{0}))) : *this;
        return DateTime{tmp.GetDate (), tmp.GetTimeOfDay (), tz};
    }
}

DateTime DateTime::Now () noexcept
{
#if qPlatform_POSIX
    // time() returns the time since the Epoch (00:00:00 UTC, January 1, 1970), measured in seconds.
    // Convert to LocalTime - just for symetry with the windows version (and cuz our API spec say so)
    return DateTime{::time (nullptr)}.AsLocalTime ();
#elif qPlatform_Windows
    ::SYSTEMTIME st{};
    ::GetLocalTime (&st);
    return DateTime{st, Timezone::kLocalTime};
#else
    AssertNotImplemented ();
    return DateTime{};
#endif
}

namespace {
    // Compute the DateTime which corresponds to a tickcount of zero.
    DateTime GetDateTimeTickCountZeroOffset_ ()
    {
        static DateTime sTimeZero_ = [] () {
            DateTime now = DateTime::Now ();
            return now.AddSeconds (-static_cast<int64_t> (Time::GetTickCount ().time_since_epoch ().count ()));
        }();
        return sTimeZero_;
    }
}

Time::TimePointSeconds DateTime::ToTickCount () const
{
    return TimePointSeconds{(AsLocalTime () - GetDateTimeTickCountZeroOffset_ ()).As<Time::DurationSeconds> ()};
}

DateTime DateTime::FromTickCount (Time::TimePointSeconds tickCount)
{
    Assert (GetDateTimeTickCountZeroOffset_ ().GetTimezone () == Timezone::kLocalTime);
    return GetDateTimeTickCountZeroOffset_ ().AddSeconds (Math::Round<int64_t> (tickCount.time_since_epoch ().count ()));
}

optional<bool> DateTime::IsDaylightSavingsTime () const
{
    if (optional<Timezone> otz = GetTimezone ()) {
        return otz->IsDaylightSavingsTime (GetDate (), GetTimeOfDay ());
    }
    return {};
}

String DateTime::Format (LocaleIndependentFormat format) const
{
    switch (format) {
        case LocaleIndependentFormat::eISO8601: {
            String r = fDate_.Format (Date::kISO8601Format);
            if (fTimeOfDay_.has_value ()) {
                String timeStr = fTimeOfDay_->Format (TimeOfDay::kISO8601Format);
                r += "T"sv + timeStr;
                if (fTimezone_) {
                    if (fTimezone_ == Timezone::kUTC) {
                        static const String kZ_{"Z"sv};
                        r += kZ_;
                    }
                    else {
                        auto tzBias     = fTimezone_->GetBiasFromUTC (fDate_, Memory::NullCoalesce (fTimeOfDay_, TimeOfDay{0}));
                        int  minuteBias = abs (static_cast<int> (tzBias)) / 60;
                        int  hrs        = minuteBias / 60;
                        int  mins       = minuteBias - hrs * 60;
                        r += ::Format (L"%s%.2d:%.2d", (tzBias < 0 ? L"-" : L"+"), hrs, mins);
                    }
                }
            }
            return r;
        } break;
        case LocaleIndependentFormat::eRFC1123: {
            optional<Timezone>  tz     = GetTimezone ();
            static const String kFMT_  = "%a, %d %b %Y %H:%M:%S"_k;
            String              result = Format (locale::classic (), {kFMT_});
            if (tz == Timezone::kUnknown) {
                return result;
            }
            else {
                return result + " "sv + tz->AsRFC1123 (fDate_, Memory::NullCoalesce (fTimeOfDay_, TimeOfDay{0}));
            }
        } break;
        default: {
            RequireNotReached ();
            return String{};
        }
    }
}

String DateTime::Format (NonStandardPrintFormat pf) const
{
    switch (pf) {
        case eCurrentLocale_WithZerosStripped: {
            /*
             *  Use basic current locale formatting, and then use regexp to find special case 0s to strip.
             *
             *  Test regexp with test string "Sun Jun 04, 2017 Sun Jun 004 2001 00 10/17/18 13:15:39    04/03/2222 Jun 03, 2004 is 1/1/03   04/04/03 4/4/04" and https://regex101.com/
             */
            String                         mungedData = Format (locale{});
            static const RegularExpression kZero2StripPattern_{"\\b0+"sv};
            constexpr bool                 kKeepZeroOnLastOfYear_ = true; // (MM / DD / 03 should keep the 0 in 03)
            if constexpr (kKeepZeroOnLastOfYear_) {
                size_t startAt = 0;
                while (auto o = mungedData.Find (kZero2StripPattern_, startAt)) {
                    // Look for preceding / and all digits to end of string
                    if (o->first != 0 and mungedData[o->first - 1] == '/' and
                        mungedData.SubString (o->second).All ([] (Character c) { return c.IsDigit (); })) {
                        // skip this case
                        startAt = o->second; // but don't encounter it again
                    }
                    else {
                        Assert (o->first >= startAt);
                        mungedData = mungedData.RemoveAt (*o);
                    }
                }
            }
            else {
                mungedData = mungedData.ReplaceAll (kZero2StripPattern_, String{});
            }
            return mungedData;
        }
    }
    AssertNotReached ();
    return String{};
}

String DateTime::Format (const locale& l) const
{
    if (GetTimeOfDay ().has_value ()) {
        return Format (l, kLocaleStandardFormat);
    }
    else {
        // otherwise we get a 'datetime' of 'XXX ' - with a space at the end
        return GetDate ().Format (l);
    }
}

String DateTime::Format (const String& formatPattern) const
{
    return Format (locale{}, formatPattern);
}

String DateTime::Format (const locale& l, const String& formatPattern) const
{
    // https://en.cppreference.com/w/cpp/locale/time_put/put
    const time_put<wchar_t>& tmput = use_facet<time_put<wchar_t>> (l);
    wostringstream           oss;

    if constexpr (kRequireImbueToUseFacet_) {
        oss.imbue (l);
    }

    tm when = As<tm> ();

    if constexpr (qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy) {
        if (l == locale::classic () and formatPattern == kLocaleStandardFormat) {
            // this seems a weird format, but from https://en.cppreference.com/w/cpp/chrono/c/strftime: writes standard date and time string, e.g. Sun Oct 17 04:41:13 2010 (locale dependent)
            static const wstring_view kAltPattern_{L"%a %b %e %T %Y"sv};
            tmput.put (oss, oss, ' ', &when, kAltPattern_.data (), kAltPattern_.data () + kAltPattern_.length ());
            return String{oss.str ()};
        }
    }

    wstring formatPatternWS = formatPattern.As<wstring> ();
    tmput.put (oss, oss, ' ', &when, formatPatternWS.c_str (), formatPatternWS.c_str () + formatPatternWS.length ());
    // docs aren't clear about expectations, but glibc (gcc8) produces trailing whitespace which
    // is not good. Unsure if thats glibc bug or my correction here makes sense -- LGP 2018-10-16
    return String{oss.str ()}.RTrim ();
}

String DateTime::ToString () const
{
    // @todo - reconsider how we format this cuz unclear if Format() already incldues timezone -- LGP 2018-10-16
    String tmp = Format ();
    if (const auto& tz = GetTimezone ()) {
        tmp += " "sv + Characters::ToString (*tz);
    }
    return tmp;
}

Date::JulianDayNumber DateTime::DaysSince () const
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

    if (useDT.GetDate ().GetYear () < Year{1970}) [[unlikely]] {
        static const range_error kRangeErrror_{"DateTime cannot be convered to time_t - before 1970"};
        Execution::Throw (kRangeErrror_);
    }

    ::tm tm{};
    tm.tm_year                         = static_cast<int> (d.GetYear ()) - kTM_Year_RelativeToYear_;
    tm.tm_mon                          = static_cast<unsigned int> (d.GetMonth ()) - 1;
    tm.tm_mday                         = static_cast<unsigned int> (d.GetDayOfMonth ());
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
    if (GetDate ().GetYear () < Year{kTM_Year_RelativeToYear_}) [[unlikely]] {
        static const range_error kRangeErrror_{"DateTime cannot be convered to time_t - before 1900"};
        Execution::Throw (kRangeErrror_);
    }
    tm tm{};
    tm.tm_year                         = static_cast<int> (fDate_.GetYear ()) - kTM_Year_RelativeToYear_;
    tm.tm_mon                          = static_cast<unsigned int> (fDate_.GetMonth ()) - 1;
    tm.tm_mday                         = static_cast<unsigned int> (fDate_.GetDayOfMonth ());
    tm.tm_wday                         = fDate_.GetDayOfWeek ().c_encoding ();
    unsigned int totalSecondsRemaining = fTimeOfDay_.has_value () ? fTimeOfDay_->GetAsSecondsCount () : 0;
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

template <>
timespec DateTime::As () const
{
    timespec tspec;
    tspec.tv_sec  = As<time_t> ();
    tspec.tv_nsec = 0; // IGNORE tv_nsec because we currently don't support fractional seconds in DateTime
    return tspec;
}

#if qPlatform_Windows
template <>
::SYSTEMTIME DateTime::As () const
{
    // CAN GET RID OF toSYSTEM_/toSysTime_ and just inline logic here...
    ::SYSTEMTIME d  = toSYSTEM_ (fDate_);
    ::SYSTEMTIME t  = toSysTime_ (Memory::NullCoalesce (fTimeOfDay_, TimeOfDay{0}));
    ::SYSTEMTIME r  = d;
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
    return DateTime{GetDate ().Add (days), GetTimeOfDay (), GetTimezone ()};
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
        return DateTime{GetDate ().Add (days{dayDiff}), GetTimeOfDay (), GetTimezone ()};
    }
    else {
        return DateTime{GetDate ().Add (days{dayDiff}), TimeOfDay{static_cast<uint32_t> (n)}, GetTimezone ()};
    }
}

Duration DateTime::Difference (const DateTime& rhs) const
{
    if (GetTimezone () == rhs.GetTimezone ()) {
        int64_t dayDiff = static_cast<int64_t> (GetDate ().GetJulianRep ()) - static_cast<int64_t> (rhs.GetDate ().GetJulianRep ());
        Time::DurationSeconds intraDaySecDiff = static_cast<Time::DurationSeconds> (GetSecondCount_ (GetTimeOfDay ())) -
                                                static_cast<Time::DurationSeconds> (GetSecondCount_ (rhs.GetTimeOfDay ()));
        return Duration{Time::DurationSeconds (kSecondsPerDay_ * dayDiff) + intraDaySecDiff};
    }
    else {
        return AsUTC ().Difference (rhs.AsUTC ());
    }
}

/*
 ********************************************************************************
 ************************* DateTime::ThreeWayComparer ***************************
 ********************************************************************************
 */
strong_ordering DateTime::ThreeWayComparer::operator() (const DateTime& lhs, const DateTime& rhs) const
{
    if (lhs.GetTimezone () == rhs.GetTimezone () or (lhs.GetTimezone () == Timezone::kUnknown) or (rhs.GetTimezone () == Timezone::kUnknown)) {
        if (auto cmp = lhs.GetDate () <=> rhs.GetDate (); cmp != strong_ordering::equal) {
            return cmp;
        }
#if qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy or qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
        return Common::compare_three_way_BWA{}(lhs.GetTimeOfDay (), rhs.GetTimeOfDay ());
#else
        return lhs.GetTimeOfDay () <=> rhs.GetTimeOfDay ();
#endif
    }
    else if (fCoerceToCommonTimezone) {
        return operator() (lhs.AsUTC (), rhs.AsUTC ());
    }
    else {
        // if not coercing to common timezone, unclear how best to compare times. Probably default to the way we already compare datetime
        // with first index being date, and then time, and only if those are the same use timezone as tie breaker
        //
        // This isn't a clearly good choice, so leave open the possability of changing this in the future -- LGP 2020-05-24
        if (auto cmp = lhs.GetDate () <=> rhs.GetDate (); cmp != strong_ordering::equal) {
            return cmp;
        }
#if qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy or qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
        if (auto cmp = Common::compare_three_way_BWA{}(lhs.GetTimeOfDay (), rhs.GetTimeOfDay ()); cmp != strong_ordering::equal) {
            return cmp;
        }
        return Common::compare_three_way_BWA{}(lhs.GetTimezone (), rhs.GetTimezone ());
#else
        if (auto cmp = lhs.GetTimeOfDay () <=> rhs.GetTimeOfDay (); cmp != strong_ordering::equal) {
            return cmp;
        }
        return lhs.GetTimezone () <=> rhs.GetTimezone ();
#endif
    }
}

/*
 ********************************************************************************
 ************************** DateTime operators  *********************************
 ********************************************************************************
 */
DateTime Time::operator+ (const DateTime& lhs, const Duration& rhs)
{
    // Define in .cpp file to avoid #include Duration in DateTime.h
    return lhs.Add (rhs);
}

DateTime Time::operator- (const DateTime& lhs, const Duration& rhs)
{
    // Define in .cpp file to avoid #include Duration in DateTime.h
    return lhs.Add (-rhs);
}

Duration Time::operator- (const DateTime& lhs, const DateTime& rhs)
{
    // Define in .cpp file to avoid #include Duration in DateTime.h
    return lhs.Difference (rhs);
}

/*
 ********************************************************************************
 ************************** Math::NearlyEquals **********************************
 ********************************************************************************
 */
bool Math::NearlyEquals (Time::DateTime l, Time::DateTime r)
{
    return NearlyEquals (l, r, static_cast<Time::DurationSeconds> (1.0));
}

bool Math::NearlyEquals (Time::DateTime l, Time::DateTime r, Time::DurationSeconds epsilon)
{
    return l == r or Math::NearlyEquals (static_cast<Time::DurationSeconds::rep> (l.As<time_t> ()),
                                         static_cast<Time::DurationSeconds::rep> (r.As<time_t> ()), epsilon.count ());
}
