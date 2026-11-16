/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdlib>
#include <cstring>
#include <ctime>

#if qStroika_Platform_POSIX
#include <time.h>
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"

#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Timezone.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Time;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qStroika_Platform_POSIX
namespace {
    tm Date2TM_ (const Date& d, const optional<TimeOfDay>& tod)
    {
        struct tm tm{};
        tm.tm_year                         = static_cast<int> (d.GetYear ()) - 1900;
        tm.tm_mon                          = static_cast<unsigned int> (d.GetMonth ()) - 1;
        tm.tm_mday                         = static_cast<unsigned int> (d.GetDayOfMonth ());
        unsigned int totalSecondsRemaining = tod.has_value () ? tod->GetAsSecondsCount () : 0;
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
}
#endif

#if qStroika_Platform_POSIX
/*
 ********************************************************************************
 ******************************* ThisYearsBiases_ *******************************
 ********************************************************************************
 */
namespace {
    /*
     *  This year's bias from UTC in standard time, and in daylight saving time (the same, in a zone without it): wherever
     *  daylight saving time is used, it is in effect in one of January and July, and not in the other.
     */
    struct ThisYearsBiases_ {
        Timezone::BiasInMinutesFromUTCType fStandardTime;
        Timezone::BiasInMinutesFromUTCType fDaylightSavingsTime;
    };
    ThisYearsBiases_ GetThisYearsBiases_ ()
    {
        time_t now = ::time (nullptr);
        ::tm   today{};
        (void)::localtime_r (&now, &today);
        ThisYearsBiases_ result{static_cast<Timezone::BiasInMinutesFromUTCType> (today.tm_gmtoff / 60),
                                static_cast<Timezone::BiasInMinutesFromUTCType> (today.tm_gmtoff / 60)};
        for (int month : {0, 6}) {
            ::tm t{};
            t.tm_year  = today.tm_year;
            t.tm_mon   = month;
            t.tm_mday  = 1;
            t.tm_hour  = 12;
            t.tm_isdst = -1; // the C library decides whether daylight saving time is in effect then
            t.tm_wday  = -1; // mktime sets it only on success - (time_t)-1 is also a valid result
            (void)::mktime (&t);
            if (t.tm_wday != -1) {
                (t.tm_isdst > 0 ? result.fDaylightSavingsTime : result.fStandardTime) =
                    static_cast<Timezone::BiasInMinutesFromUTCType> (t.tm_gmtoff / 60);
            }
        }
        return result;
    }
}
#endif

/*
 ********************************************************************************
 ********************************* GetLocalTime_ ********************************
 ********************************************************************************
 */
namespace {
    /*
     *  At a local date and time, by the rules in effect then: the bias from UTC, and whether daylight saving time is in effect.
     *
     *  POSIX: the C library's local time zone - TZ, else /etc/localtime.
     *
     *  Windows: Windows' own time zone - which GetLocalTime (so DateTime::Now ()) reads - and never TZ: no Windows API reads
     *  TZ, only the C library's time functions, and those only in the old POSIX form (EST5EDT) - the IANA names Cygwin sets
     *  (America/New_York) they read as UTC (as of UCRT 10.0.26100).
     */
    struct LocalTime_ {
        Timezone::BiasInMinutesFromUTCType fBiasInMinutesFromUTC;
        bool                               fDaylightSavingsTime;
    };
    LocalTime_ GetLocalTime_ (const Date& date, const optional<TimeOfDay>& tod)
    {
#if qStroika_Platform_POSIX
        ::tm t    = Date2TM_ (date, tod);
        t.tm_wday = -1; // mktime sets it only on success - (time_t)-1 is also a valid result
        (void)::mktime (&t);
        if (t.tm_wday != -1) {
            return {static_cast<Timezone::BiasInMinutesFromUTCType> (t.tm_gmtoff / 60), t.tm_isdst > 0};
        }
        // a time the C library cannot convert (with a 32-bit time_t, one before 1901 or after 2038): standard time, by this year's rules
        return {GetThisYearsBiases_ ().fStandardTime, false};
#elif qStroika_Platform_Windows
        ::DYNAMIC_TIME_ZONE_INFORMATION zone{};
        (void)::GetDynamicTimeZoneInformation (&zone);
        int                     year = static_cast<int> (date.GetYear ());
        ::TIME_ZONE_INFORMATION thatYears{};
        if (1601 <= year and year <= 30827 and ::GetTimeZoneInformationForYear (static_cast<::USHORT> (year), &zone, &thatYears)) {
            ::SYSTEMTIME local = DateTime{date, tod}.As<::SYSTEMTIME> ();
            ::SYSTEMTIME utc{};
            if (::TzSpecificLocalTimeToSystemTime (&thatYears, &local, &utc)) {
                // both read as plain clock values - no time zone - so their difference is plain arithmetic
                auto bias = static_cast<Timezone::BiasInMinutesFromUTCType> (
                    (DateTime{local, Timezone::kUnknown} - DateTime{utc, Timezone::kUnknown}).count () / 60);
                return {bias, bias != -(thatYears.Bias + thatYears.StandardBias)};
            }
        }
        // a year Windows has no rules for (a SYSTEMTIME's are 1601-30827): standard time, by today's rules
        return {static_cast<Timezone::BiasInMinutesFromUTCType> (-(zone.Bias + zone.StandardBias)), false};
#endif
    }
}

/*
 ********************************************************************************
 ********************************* Time::Timezone *******************************
 ********************************************************************************
 */
optional<Timezone> Timezone::ParseTimezoneOffsetString (const char* tzStr)
{
    RequireNotNull (tzStr);
    if (*tzStr == '\0') {
        return nullopt;
    }
    else {
        int         tzHr    = 0;
        int         tzMn    = 0;
        const char* tStrPtr = tzStr;
        bool        isNeg   = (*tStrPtr == '-');
        if (*tStrPtr == '+' or *tStrPtr == '-') {
            ++tStrPtr;
        }
        DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING sscanf_s
        int nTZItems = ::sscanf (tStrPtr, "%2d%2d", &tzHr, &tzMn);
        if (nTZItems == 1) {
            nTZItems = ::sscanf (tStrPtr, "%2d:%2d", &tzHr, &tzMn);
        }
        DISABLE_COMPILER_MSC_WARNING_END (4996)
        if (nTZItems == 2) {
            int16_t n = (isNeg ? -1 : 1) * static_cast<int16_t> (tzHr * 60 + tzMn);
            return Timezone{n, DataExchange::ValidationStrategy::eThrow};
        }
        static const Execution::Exception<runtime_error> kException_{"invalid timezone offset"sv};
        Execution::Throw (kException_);
    }
}

optional<Timezone> Timezone::ParseTimezoneOffsetString (const wchar_t* tzStr)
{
    RequireNotNull (tzStr);
    if (*tzStr == '\0') {
        return nullopt;
    }
    else {
        int            tzHr    = 0;
        int            tzMn    = 0;
        const wchar_t* tStrPtr = tzStr;
        bool           isNeg   = (*tStrPtr == '-');
        if (*tStrPtr == '+' or *tStrPtr == '-') {
            ++tStrPtr;
        }
        DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
        int nTZItems = ::swscanf (tStrPtr, L"%2d%2d", &tzHr, &tzMn);
        if (nTZItems == 1) {
            nTZItems = ::swscanf (tStrPtr, L"%2d:%2d", &tzHr, &tzMn);
        }
        DISABLE_COMPILER_MSC_WARNING_END (4996)
        if (nTZItems == 2) {
            int16_t n = (isNeg ? -1 : 1) * static_cast<int16_t> (tzHr * 60 + tzMn);
            return Timezone{n, DataExchange::ValidationStrategy::eThrow};
        }
        static const Execution::Exception<runtime_error> kException_{"invalid timezone offset"sv};
        Execution::Throw (kException_);
    }
}

optional<Timezone> Timezone::ParseTimezoneOffsetString (const Characters::String& tzStr)
{
    return ParseTimezoneOffsetString (tzStr.As<wstring> ().c_str ());
}

String Timezone::AsHHMM (const Date& date, const TimeOfDay& tod, bool insertColon) const
{
    int          minutes    = GetBiasInMinutesFromUTC (date, tod);
    bool         isNeg      = minutes < 0;
    unsigned int nMinOffset = abs (minutes);
    return "{}{:02}{}{:02}"_f(isNeg ? L"-" : L"+", nMinOffset / 60, insertColon ? L":" : L"", nMinOffset % 60);
}

String Timezone::AsRFC1123 (const Date& date, const TimeOfDay& tod) const
{
    int minutes = GetBiasInMinutesFromUTC (date, tod);
    if (minutes == 0) {
        static const String kUTC_ = "GMT"sv; // UT or GMT for UTC in  https://tools.ietf.org/html/rfc822#section-5
        return kUTC_;
    }
    return AsHHMM (date, tod, false);
}

Timezone::BiasInMinutesFromUTCType Timezone::GetBiasInMinutesFromUTC (const Date& date, const TimeOfDay& tod) const
{
    switch (fTZ_) {
        case TZ_::eUTC:
            return 0;
        case TZ_::eFixedOffsetBias:
            Ensure (kBiasInMinutesFromUTCTypeValidRange.Contains (fBiasInMinutesFromUTC_));
            return fBiasInMinutesFromUTC_;
        case TZ_::eLocalTime: {
            auto result = GetLocalTime_ (date, tod).fBiasInMinutesFromUTC;
            Ensure (kBiasInMinutesFromUTCTypeValidRange.Contains (result));
            return result;
        }
        default:
            AssertNotReached ();
            return 0;
    }
}

optional<bool> Timezone::IsDaylightSavingsTime (const Date& date, const optional<TimeOfDay>& tod)
{
    // @todo - fix for other (not fixed) timezones - like America/NewYork
    if (fTZ_ == TZ_::eLocalTime) {
        return GetLocalTime_ (date, tod).fDaylightSavingsTime;
    }
    else {
        return {};
    }
}

Characters::String Timezone::ToString () const
{
    // This format is just comsetic and for information (debugging) purposes, and follows no standards
    static const String kUTC_{"UTC"sv};
    static const String kLocaltime_{"localtime"sv};
    switch (fTZ_) {
        case TZ_::eUTC:
            return kUTC_;
        case TZ_::eFixedOffsetBias: {
            Ensure (kBiasInMinutesFromUTCTypeValidRange.Contains (fBiasInMinutesFromUTC_));
            bool         isNeg      = fBiasInMinutesFromUTC_ < 0;
            unsigned int nMinOffset = abs (fBiasInMinutesFromUTC_);
            return "{}{:02}:{:02}"_f(isNeg ? L"-" : L"+", nMinOffset / 60, nMinOffset % 60);
        } break;
        case TZ_::eLocalTime: {
            return kLocaltime_;
        } break;
        default:
            AssertNotReached ();
            return String{};
    }
}

/*
 ********************************************************************************
 ********************** Time::GetCurrentLocaleTimezoneInfo **********************
 ********************************************************************************
 */
TimeZoneInformationType Time::GetCurrentLocaleTimezoneInfo ()
{
    TimeZoneInformationType result;
#if qStroika_Platform_POSIX
    try {
        result.fID = Streams::BinaryToText::Reader::New (IO::FileSystem::FileInputStream::New ("/etc/timezone"sv)).ReadAll ().Trim ();
    }
    catch (...) {
        DbgTrace ("Ignoring missing ID from /etc/timezone"_f);
    }
    if (not result.fID.has_value ()) {
        // We could look to see if /etc/localtime is a symlink or a copy of any named file from /usr/share/zoneinfo, but
        // hope thats not needed!
    }
    ThisYearsBiases_ biases = GetThisYearsBiases_ (); // first: it calls mktime, which sets tzname (as tzset does)
    // @see http://pubs.opengroup.org/onlinepubs/7908799/xsh/tzset.html
    result.fStandardTime.fAbbreviation                = String::FromNarrowSDKString (tzname[0]);
    result.fStandardTime.fName                        = String::FromNarrowSDKString (tzname[0]);
    result.fStandardTime.fBiasInMinutesFromUTC        = biases.fStandardTime;
    result.fDaylightSavingsTime.fAbbreviation         = String::FromNarrowSDKString (tzname[1]);
    result.fDaylightSavingsTime.fName                 = String::FromNarrowSDKString (tzname[1]);
    result.fDaylightSavingsTime.fBiasInMinutesFromUTC = biases.fDaylightSavingsTime;
#elif qStroika_Platform_Windows
    using Common::KeyValuePair;
    using Containers::Mapping;
    // Table from Stering around 2015-05-01
    static const Mapping<String, String> kWinDoze2OlsonName_ = {
        KeyValuePair<String, String>{"Afghanistan Standard Time"sv, "Asia/Kabul"sv},
        KeyValuePair<String, String>{"Alaskan Standard Time"sv, "America/Juneau"sv},
        KeyValuePair<String, String>{"Arab Standard Time"sv, "Asia/Riyadh"sv},
        KeyValuePair<String, String>{"Arabian Standard Time"sv, "Asia/Muscat"sv},
        KeyValuePair<String, String>{"Arabic Standard Time"sv, "Asia/Baghdad"sv},
        KeyValuePair<String, String>{"Argentina Standard Time"sv, "America/Rosario"sv},
        KeyValuePair<String, String>{"Atlantic Standard Time"sv, "America/Halifax"sv},
        KeyValuePair<String, String>{"AUS Central Standard Time"sv, "Australia/Darwin"sv},
        KeyValuePair<String, String>{"AUS Eastern Standard Time"sv, "Australia/Sydney"sv},
        KeyValuePair<String, String>{"Azerbaijan Standard Time"sv, "Asia/Baku"sv},
        KeyValuePair<String, String>{"Azores Standard Time"sv, "Atlantic/Azores"sv},
        KeyValuePair<String, String>{"Bahia Standard Time"sv, "America/Maceio"sv},
        KeyValuePair<String, String>{"Bangladesh Standard Time"sv, "Asia/Dhaka"sv},
        KeyValuePair<String, String>{"Canada Central Standard Time"sv, "America/Swift_Current"sv},
        KeyValuePair<String, String>{"Cape Verde Standard Time"sv, "Atlantic/Cape_Verde"sv},
        KeyValuePair<String, String>{"Caucasus Standard Time"sv, "Asia/Yerevan"sv},
        KeyValuePair<String, String>{"Cen. Australia Standard Time"sv, "Australia/Adelaide"sv},
        KeyValuePair<String, String>{"Central America Standard Time"sv, "America/Tegucigalpa"sv},
        KeyValuePair<String, String>{"Central Asia Standard Time"sv, "Asia/Almaty"sv},
        KeyValuePair<String, String>{"Central Brazilian Standard Time"sv, "America/Cuiaba"sv},
        KeyValuePair<String, String>{"Central Europe Standard Time"sv, "Europe/Prague"sv},
        KeyValuePair<String, String>{"Central European Standard Time"sv, "Europe/Skopje"sv},
        KeyValuePair<String, String>{"Central Pacific Standard Time"sv, "Pacific/Guadalcanal"sv},
        KeyValuePair<String, String>{"Central Standard Time"sv, "America/Chicago"sv},
        KeyValuePair<String, String>{"Central Standard Time (Mexico)"sv, "America/Monterrey"sv},
        KeyValuePair<String, String>{"China Standard Time"sv, "Asia/Urumqi"sv},
        KeyValuePair<String, String>{"E. Africa Standard Time"sv, "Africa/Nairobi"sv},
        KeyValuePair<String, String>{"E. Australia Standard Time"sv, "Australia/Brisbane"sv},
        KeyValuePair<String, String>{"E. Europe Standard Time"sv, "Europe/Bucharest"sv},
        KeyValuePair<String, String>{"E. South America Standard Time"sv, "America/Sao_Paulo"sv},
        KeyValuePair<String, String>{"Eastern Standard Time"sv, "America/New_York"sv},
        KeyValuePair<String, String>{"Egypt Standard Time"sv, "Africa/Cairo"sv},
        KeyValuePair<String, String>{"Ekaterinburg Standard Time"sv, "Asia/Yekaterinburg"sv},
        KeyValuePair<String, String>{"Fiji Standard Time"sv, "Pacific/Fiji"sv},
        KeyValuePair<String, String>{"FLE Standard Time"sv, "Europe/Helsinki"sv},
        KeyValuePair<String, String>{"Georgian Standard Time"sv, "Asia/Tbilisi"sv},
        KeyValuePair<String, String>{"GMT Standard Time"sv, "Europe/London"sv},
        KeyValuePair<String, String>{"Greenland Standard Time"sv, "America/Godthab"sv},
        KeyValuePair<String, String>{"Greenwich Standard Time"sv, "Atlantic/Reykjavik"sv},
        KeyValuePair<String, String>{"GTB Standard Time"sv, "Europe/Bucharest"sv},
        KeyValuePair<String, String>{"Hawaiian Standard Time"sv, "Pacific/Honolulu"sv},
        KeyValuePair<String, String>{"India Standard Time"sv, "Asia/Calcutta"sv},
        KeyValuePair<String, String>{"Iran Standard Time"sv, "Asia/Tehran"sv},
        KeyValuePair<String, String>{"Jerusalem Standard Time"sv, "Asia/Jerusalem"sv},
        KeyValuePair<String, String>{"Jordan Standard Time"sv, "Asia/Amman"sv},
        KeyValuePair<String, String>{"Kaliningrad Standard Time"sv, "Europe/Kaliningrad"sv},
        KeyValuePair<String, String>{"Korea Standard Time"sv, "Asia/Seoul"sv},
        KeyValuePair<String, String>{"Libya Standard Time"sv, "Africa/Tripoli"sv},
        KeyValuePair<String, String>{"Magadan Standard Time"sv, "Asia/Magadan"sv},
        KeyValuePair<String, String>{"Mauritius Standard Time"sv, "Indian/Mauritius"sv},
        KeyValuePair<String, String>{"Middle East Standard Time"sv, "Asia/Beirut"sv},
        KeyValuePair<String, String>{"Montevideo Standard Time"sv, "America/Montevideo"sv},
        KeyValuePair<String, String>{"Morocco Standard Time"sv, "Africa/Casablanca"sv},
        KeyValuePair<String, String>{"Mountain Standard Time"sv, "America/Denver"sv},
        KeyValuePair<String, String>{"Mountain Standard Time (Mexico)"sv, "America/Mazatlan"sv},
        KeyValuePair<String, String>{"Myanmar Standard Time"sv, "Asia/Rangoon"sv},
        KeyValuePair<String, String>{"N. Central Asia Standard Time"sv, "Asia/Novosibirsk"sv},
        KeyValuePair<String, String>{"Namibia Standard Time"sv, "Africa/Windhoek"sv},
        KeyValuePair<String, String>{"Nepal Standard Time"sv, "Asia/Katmandu"sv},
        KeyValuePair<String, String>{"New Zealand Standard Time"sv, "Pacific/Auckland"sv},
        KeyValuePair<String, String>{"Newfoundland Standard Time"sv, "America/St_Johns"sv},
        KeyValuePair<String, String>{"North Asia East Standard Time"sv, "Asia/Irkutsk"sv},
        KeyValuePair<String, String>{"North Asia Standard Time"sv, "Asia/Krasnoyarsk"sv},
        KeyValuePair<String, String>{"Pacific SA Standard Time"sv, "America/Santiago"sv},
        KeyValuePair<String, String>{"Pacific Standard Time"sv, "America/Los_Angeles"sv},
        KeyValuePair<String, String>{"Pacific Standard Time (Mexico)"sv, "America/Tijuana"sv},
        KeyValuePair<String, String>{"Pakistan Standard Time"sv, "Asia/Karachi"sv},
        KeyValuePair<String, String>{"Paraguay Standard Time"sv, "America/Asuncion"sv},
        KeyValuePair<String, String>{"Romance Standard Time"sv, "Europe/Paris"sv},
        KeyValuePair<String, String>{"Russian Standard Time"sv, "Europe/Moscow"sv},
        KeyValuePair<String, String>{"SA Eastern Standard Time"sv, "America/Cayenne"sv},
        KeyValuePair<String, String>{"SA Pacific Standard Time"sv, "America/Lima"sv},
        KeyValuePair<String, String>{"SA Western Standard Time"sv, "America/La_Paz"sv},
        KeyValuePair<String, String>{"SE Asia Standard Time"sv, "Asia/Jakarta"sv},
        KeyValuePair<String, String>{"Malay Peninsula Standard Time"sv, "Asia/Singapore"sv},
        KeyValuePair<String, String>{"South Africa Standard Time"sv, "Africa/Harare"sv},
        KeyValuePair<String, String>{"Syria Standard Time"sv, "Asia/Damascus"sv},
        KeyValuePair<String, String>{"Taipei Standard Time"sv, "Asia/Taipei"sv},
        KeyValuePair<String, String>{"Tasmania Standard Time"sv, "Australia/Hobart"sv},
        KeyValuePair<String, String>{"Tokyo Standard Time"sv, "Asia/Tokyo"sv},
        KeyValuePair<String, String>{"Tonga Standard Time"sv, "Pacific/Tongatapu"sv},
        KeyValuePair<String, String>{"Turkey Standard Time"sv, "Asia/Istanbul"sv},
        KeyValuePair<String, String>{"Ulaanbaatar Standard Time"sv, "Asia/Ulaanbaatar"sv},
        KeyValuePair<String, String>{"US Eastern Standard Time"sv, "America/Indianapolis"sv},
        KeyValuePair<String, String>{"US Mountain Standard Time"sv, "America/Denver"sv},
        KeyValuePair<String, String>{"Venezuela Standard Time"sv, "America/Caracas"sv},
        KeyValuePair<String, String>{"Vladivostok Standard Time"sv, "Asia/Vladivostok"sv},
        KeyValuePair<String, String>{"W. Australia Standard Time"sv, "Australia/Perth"sv},
        KeyValuePair<String, String>{"W. Central Africa Standard Time"sv, "Africa/Brazzaville"sv},
        KeyValuePair<String, String>{"W. Europe Standard Time"sv, "Europe/Vienna"sv},
        KeyValuePair<String, String>{"West Asia Standard Time"sv, "Asia/Tashkent"sv},
        KeyValuePair<String, String>{"West Pacific Standard Time"sv, "Pacific/Port_Moresby"sv},
        KeyValuePair<String, String>{"Yakutsk Standard Time"sv, "Asia/Yakutsk"sv},
    };
    TIME_ZONE_INFORMATION tzInfo{};
    (void)::GetTimeZoneInformation (&tzInfo);
    result.fStandardTime.fAbbreviation                = String{tzInfo.StandardName};
    result.fStandardTime.fName                        = String{tzInfo.StandardName};
    result.fStandardTime.fBiasInMinutesFromUTC        = -(tzInfo.StandardBias + tzInfo.Bias);
    result.fDaylightSavingsTime.fAbbreviation         = String{tzInfo.DaylightName};
    result.fDaylightSavingsTime.fName                 = String{tzInfo.DaylightName};
    result.fDaylightSavingsTime.fBiasInMinutesFromUTC = -(tzInfo.DaylightBias + tzInfo.Bias);
    result.fID                                        = kWinDoze2OlsonName_.LookupValue (tzInfo.StandardName, tzInfo.StandardName);
#else
    AssertNotImplemented ();
#endif
    return result;
}
