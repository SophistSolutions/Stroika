/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstdlib>
#include <cstring>
#include <ctime>

#if qPlatform_POSIX
#include <time.h>
#endif

#include "../Characters/Format.h"
#include "../Characters/String.h"
#include "../Characters/String_Constant.h"
#include "../Configuration/Common.h"
#include "../Containers/Mapping.h"
#include "../DataExchange/Variant/INI/Reader.h"
#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"
#include "../Execution/ProcessRunner.h"
#include "../IO/FileSystem/FileInputStream.h"
#include "../Streams/TextReader.h"

#include "DateTime.h"

#include "Timezone.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Time;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    tm Date2TM_ (const Date& d, const optional<TimeOfDay>& tod)
    {
        struct tm tm {
        };
        tm.tm_year                         = static_cast<int> (d.GetYear ()) - 1900;
        tm.tm_mon                          = static_cast<int> (d.GetMonth ()) - 1;
        tm.tm_mday                         = static_cast<int> (d.GetDayOfMonth ());
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

namespace {
    bool IsDaylightSavingsTime_ (const Date& date, const optional<TimeOfDay>& tod);
    bool IsDaylightSavingsTime_ (const DateTime& d);

    /**
    * Return the number of seconds which must be added to a LocalTime value to get GMT.
    */
    time_t GetLocaltimeToGMTOffset_ (bool applyDST);
    time_t GetLocaltimeToGMTOffset_ (const DateTime& forTime);
}

/*
 ********************************************************************************
 ********************************* Time::Timezone *******************************
 ********************************************************************************
 */
#if qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
const Timezone           Timezone::kLocalTime{Timezone::LocalTime ()};
const Timezone           Timezone::kUTC{Timezone::UTC ()};
const optional<Timezone> Timezone::kUnknown{Timezone::Unknown ()};
#elif qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
constexpr Timezone           Timezone::kLocalTime;
constexpr Timezone           Timezone::kUTC;
constexpr optional<Timezone> Timezone::kUnknown;
#endif

optional<Timezone> Timezone::ParseTimezoneOffsetString (const char* tzStr)
{
    RequireNotNull (tzStr);
    if (*tzStr == '\0') {
        return {};
    }
    else {
        int         tzHr    = 0;
        int         tzMn    = 0;
        const char* tStrPtr = tzStr;
        bool        isNeg   = (*tStrPtr == '-');
        if (*tStrPtr == '+' or *tStrPtr == '-') {
            tStrPtr++;
        }
        DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING sscanf_s
        int nTZItems = ::sscanf (tStrPtr, "%2d%2d", &tzHr, &tzMn);
        if (nTZItems == 1) {
            nTZItems = ::sscanf (tStrPtr, "%2d:%2d", &tzHr, &tzMn);
        }
        DISABLE_COMPILER_MSC_WARNING_END (4996)
        if (nTZItems == 2) {
            int16_t n = (isNeg ? -1 : 1) * static_cast<int16_t> (tzHr * 60 + tzMn);
            if (kBiasInMinutesFromUTCTypeValidRange.Contains (n)) {
                return Timezone (n);
            }
        }
        Execution::Throw (Execution::Exception (L"invalid timezone offset"sv));
    }
}

optional<Timezone> Timezone::ParseTimezoneOffsetString (const wchar_t* tzStr)
{
    RequireNotNull (tzStr);
    if (*tzStr == '\0') {
        return {};
    }
    else {
        int            tzHr    = 0;
        int            tzMn    = 0;
        const wchar_t* tStrPtr = tzStr;
        bool           isNeg   = (*tStrPtr == '-');
        if (*tStrPtr == '+' or *tStrPtr == '-') {
            tStrPtr++;
        }
        DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
        int nTZItems = ::swscanf (tStrPtr, L"%2d%2d", &tzHr, &tzMn);
        if (nTZItems == 1) {
            nTZItems = ::swscanf (tStrPtr, L"%2d:%2d", &tzHr, &tzMn);
        }
        DISABLE_COMPILER_MSC_WARNING_END (4996)
        if (nTZItems == 2) {
            int16_t n = (isNeg ? -1 : 1) * static_cast<int16_t> (tzHr * 60 + tzMn);
            if (kBiasInMinutesFromUTCTypeValidRange.Contains (n)) {
                return Timezone (n);
            }
        }
        Execution::Throw (Execution::Exception (L"invalid timezone offset"sv));
    }
}

optional<Timezone> Timezone::ParseTimezoneOffsetString (const Characters::String& tzStr)
{
    return ParseTimezoneOffsetString (tzStr.c_str ());
}

String Timezone::AsHHMM (const Date& date, const TimeOfDay& tod, bool insertColon) const
{
    int          minutes    = GetBiasInMinutesFromUTC (date, tod);
    bool         isNeg      = minutes < 0;
    unsigned int nMinOffset = abs (fBiasInMinutesFromUTC_);
    return Characters::Format (insertColon ? L"%s%02d:%02d" : L"%s%02d%02d", isNeg ? L"-" : L"+", nMinOffset / 60, nMinOffset % 60);
}

String Timezone::AsRFC1123 (const Date& date, const TimeOfDay& tod) const
{
    int minutes = GetBiasInMinutesFromUTC (date, tod);
    if (minutes == 0) {
        static const String kUTC_ = L"GMT"sv; // UT or GMT for UTC in  https://tools.ietf.org/html/rfc822#section-5
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
            auto result = static_cast<BiasInMinutesFromUTCType> (-GetLocaltimeToGMTOffset_ (IsDaylightSavingsTime_ (date, tod)) / 60);
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
        return IsDaylightSavingsTime_ (date, tod);
    }
    else {
        return {};
    }
}

Characters::String Timezone::ToString () const
{
    // This format is just comsetic and for information (debugging) purposes, and follows no standards
    static const String_Constant kUTC_{L"UTC"};
    static const String_Constant kLocaltime_{L"localtime"};
    switch (fTZ_) {
        case TZ_::eUTC:
            return kUTC_;
        case TZ_::eFixedOffsetBias: {
            Ensure (kBiasInMinutesFromUTCTypeValidRange.Contains (fBiasInMinutesFromUTC_));
            bool         isNeg      = fBiasInMinutesFromUTC_ < 0;
            unsigned int nMinOffset = abs (fBiasInMinutesFromUTC_);
            return Characters::Format (L"%s%02d:%02d", isNeg ? L"-" : L"+", nMinOffset / 60, nMinOffset % 60);
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
#if qPlatform_POSIX
    try {
        result.fID = Streams::TextReader::New (IO::FileSystem::FileInputStream::New (L"/etc/timezone"sv)).ReadAll ().Trim ();
    }
    catch (...) {
        DbgTrace ("Ignoring missing ID from /etc/timezone");
    }
#if 0
    //
    // COULD look at /etc/localtime, but very hard to map this to olson db name
    //
    // One maybe close way is to see if /etc/localtime is a slink or exact copy of file in /usr/share/zoneinfo - that subdir/name
    // is typically/often an Olson DB name.
    //
    if (not result.fID.has_value ()) {
        // WEAK but maybe effective way
        // http://www.linuxforums.org/forum/red-hat-fedora-linux/162483-changing-timezone-rhel-5-4-centos.html
        try {
            DataExchange::INI::Profile p = DataExchange::INI::Reader ().ReadProfile (IO::FileSystem::FileInputStream::New (String_Constant { L"/etc/sysconfig/clock" }));
            if (auto o = p.fUnnamedSection.fProperties.Lookup (String_Constant { L"ZONE" })) {
                result.fID =  *o;
            }
        }
        catch (...) {
            DbgTrace ("Missing Zone ID from /etc/sysconfig/clock");
        }
    }
#endif
#if 0
    if (not result.fID.has_value ()) {
        try {
            // Not a good approach because this returns a zone abbreviation, which doesn't uniquely define a zone.
            // For example, CDT could be Cocos Islands Time, or Central Daylight Time (North America) etc (see http://en.wikipedia.org/wiki/List_of_time_zone_abbreviations)
            String  tzAbbrev = Execution::ProcessRunner (L"date +%Z").Run (String ()).Trim ();
            using   Containers::Mapping;
            using   Common::KeyValuePair;
            // Table from Hand-coded for a few empirical cases around 2015-06-01
            static const    Mapping<String, String> kUNIXTZAbbrev2OlsonName_ =  {
                KeyValuePair<String, String> { String_Constant { L"CST" },      String_Constant { L"America/Chicago" } },
                KeyValuePair<String, String> { String_Constant { L"CDT" },      String_Constant { L"America/Chicago" } },
                KeyValuePair<String, String> { String_Constant { L"CST6CDT" },  String_Constant { L"America/Chicago" } },
                KeyValuePair<String, String> { String_Constant { L"EDT" },      String_Constant { L"America/New_York" } },
                KeyValuePair<String, String> { String_Constant { L"EST" },      String_Constant { L"America/New_York" } },
                KeyValuePair<String, String> { String_Constant { L"EST5EDT" },  String_Constant { L"America/New_York" } },
                KeyValuePair<String, String> { String_Constant { L"PDT" },      String_Constant { L"America/Los_Angeles" } },
                KeyValuePair<String, String> { String_Constant { L"PST" },      String_Constant { L"America/Los_Angeles" } },
                KeyValuePair<String, String> { String_Constant { L"PDT8PST" },  String_Constant { L"America/Los_Angeles" } },
            };
            result.fID = kUNIXTZAbbrev2OlsonName_.LookupValue (tzAbbrev, tzAbbrev);
        }
        catch (...) {
            DbgTrace ("Ignoring missing ID from date +%Z");
        }
    }
#endif
    if (not result.fID.has_value ()) {
        // We could look to see if /etc/localtime is a symlink or a copy of any named file from /usr/share/zoneinfo, but
        // hope thats not needed!
    }
    // @see http://pubs.opengroup.org/onlinepubs/7908799/xsh/tzset.html
    result.fStandardTime.fAbbreviation                = String::FromNarrowSDKString (tzname[0]);
    result.fStandardTime.fName                        = String::FromNarrowSDKString (tzname[0]);
    result.fStandardTime.fBiasInMinutesFromUTC        = -GetLocaltimeToGMTOffset_ (false) / 60;
    result.fDaylightSavingsTime.fAbbreviation         = String::FromNarrowSDKString (tzname[1]);
    result.fDaylightSavingsTime.fName                 = String::FromNarrowSDKString (tzname[1]);
    result.fDaylightSavingsTime.fBiasInMinutesFromUTC = -GetLocaltimeToGMTOffset_ (true) / 60;
#elif qPlatform_Windows
    using Common::KeyValuePair;
    using Containers::Mapping;
    // Table from Stering around 2015-05-01
    static const Mapping<String, String> kWinDoze2OlsonName_ = {
        KeyValuePair<String, String>{L"Afghanistan Standard Time"sv, L"Asia/Kabul"sv},
        KeyValuePair<String, String>{L"Alaskan Standard Time"sv, L"America/Juneau"sv},
        KeyValuePair<String, String>{L"Arab Standard Time"sv, L"Asia/Riyadh"sv},
        KeyValuePair<String, String>{L"Arabian Standard Time"sv, L"Asia/Muscat"sv},
        KeyValuePair<String, String>{L"Arabic Standard Time"sv, L"Asia/Baghdad"sv},
        KeyValuePair<String, String>{L"Argentina Standard Time"sv, L"America/Rosario"sv},
        KeyValuePair<String, String>{L"Atlantic Standard Time"sv, L"America/Halifax"sv},
        KeyValuePair<String, String>{L"AUS Central Standard Time"sv, L"Australia/Darwin"sv},
        KeyValuePair<String, String>{L"AUS Eastern Standard Time"sv, L"Australia/Sydney"sv},
        KeyValuePair<String, String>{L"Azerbaijan Standard Time"sv, L"Asia/Baku"sv},
        KeyValuePair<String, String>{L"Azores Standard Time"sv, L"Atlantic/Azores"sv},
        KeyValuePair<String, String>{L"Bahia Standard Time"sv, L"America/Maceio"sv},
        KeyValuePair<String, String>{L"Bangladesh Standard Time"sv, L"Asia/Dhaka"sv},
        KeyValuePair<String, String>{L"Canada Central Standard Time"sv, L"America/Swift_Current"sv},
        KeyValuePair<String, String>{L"Cape Verde Standard Time"sv, L"Atlantic/Cape_Verde"sv},
        KeyValuePair<String, String>{L"Caucasus Standard Time"sv, L"Asia/Yerevan"sv},
        KeyValuePair<String, String>{L"Cen. Australia Standard Time"sv, L"Australia/Adelaide"sv},
        KeyValuePair<String, String>{L"Central America Standard Time"sv, L"America/Tegucigalpa"sv},
        KeyValuePair<String, String>{L"Central Asia Standard Time"sv, L"Asia/Almaty"sv},
        KeyValuePair<String, String>{L"Central Brazilian Standard Time"sv, L"America/Cuiaba"sv},
        KeyValuePair<String, String>{L"Central Europe Standard Time"sv, L"Europe/Prague"sv},
        KeyValuePair<String, String>{L"Central European Standard Time"sv, L"Europe/Skopje"sv},
        KeyValuePair<String, String>{L"Central Pacific Standard Time"sv, L"Pacific/Guadalcanal"sv},
        KeyValuePair<String, String>{L"Central Standard Time"sv, L"America/Chicago"sv},
        KeyValuePair<String, String>{L"Central Standard Time (Mexico)"sv, L"America/Monterrey"sv},
        KeyValuePair<String, String>{L"China Standard Time"sv, L"Asia/Urumqi"sv},
        KeyValuePair<String, String>{L"E. Africa Standard Time"sv, L"Africa/Nairobi"sv},
        KeyValuePair<String, String>{L"E. Australia Standard Time"sv, L"Australia/Brisbane"sv},
        KeyValuePair<String, String>{L"E. Europe Standard Time"sv, L"Europe/Bucharest"sv},
        KeyValuePair<String, String>{L"E. South America Standard Time"sv, L"America/Sao_Paulo"sv},
        KeyValuePair<String, String>{L"Eastern Standard Time"sv, L"America/New_York"sv},
        KeyValuePair<String, String>{L"Egypt Standard Time"sv, L"Africa/Cairo"sv},
        KeyValuePair<String, String>{L"Ekaterinburg Standard Time"sv, L"Asia/Yekaterinburg"sv},
        KeyValuePair<String, String>{L"Fiji Standard Time"sv, L"Pacific/Fiji"sv},
        KeyValuePair<String, String>{L"FLE Standard Time"sv, L"Europe/Helsinki"sv},
        KeyValuePair<String, String>{L"Georgian Standard Time"sv, L"Asia/Tbilisi"sv},
        KeyValuePair<String, String>{L"GMT Standard Time"sv, L"Europe/London"sv},
        KeyValuePair<String, String>{L"Greenland Standard Time"sv, L"America/Godthab"sv},
        KeyValuePair<String, String>{L"Greenwich Standard Time"sv, L"Atlantic/Reykjavik"sv},
        KeyValuePair<String, String>{L"GTB Standard Time"sv, L"Europe/Bucharest"sv},
        KeyValuePair<String, String>{L"Hawaiian Standard Time"sv, L"Pacific/Honolulu"sv},
        KeyValuePair<String, String>{L"India Standard Time"sv, L"Asia/Calcutta"sv},
        KeyValuePair<String, String>{L"Iran Standard Time"sv, L"Asia/Tehran"sv},
        KeyValuePair<String, String>{L"Jerusalem Standard Time"sv, L"Asia/Jerusalem"sv},
        KeyValuePair<String, String>{L"Jordan Standard Time"sv, L"Asia/Amman"sv},
        KeyValuePair<String, String>{L"Kaliningrad Standard Time"sv, L"Europe/Kaliningrad"sv},
        KeyValuePair<String, String>{L"Korea Standard Time"sv, L"Asia/Seoul"sv},
        KeyValuePair<String, String>{L"Libya Standard Time"sv, L"Africa/Tripoli"sv},
        KeyValuePair<String, String>{L"Magadan Standard Time"sv, L"Asia/Magadan"sv},
        KeyValuePair<String, String>{L"Mauritius Standard Time"sv, L"Indian/Mauritius"sv},
        KeyValuePair<String, String>{L"Middle East Standard Time"sv, L"Asia/Beirut"sv},
        KeyValuePair<String, String>{L"Montevideo Standard Time"sv, L"America/Montevideo"sv},
        KeyValuePair<String, String>{L"Morocco Standard Time"sv, L"Africa/Casablanca"sv},
        KeyValuePair<String, String>{L"Mountain Standard Time"sv, L"America/Denver"sv},
        KeyValuePair<String, String>{L"Mountain Standard Time (Mexico)"sv, L"America/Mazatlan"sv},
        KeyValuePair<String, String>{L"Myanmar Standard Time"sv, L"Asia/Rangoon"sv},
        KeyValuePair<String, String>{L"N. Central Asia Standard Time"sv, L"Asia/Novosibirsk"sv},
        KeyValuePair<String, String>{L"Namibia Standard Time"sv, L"Africa/Windhoek"sv},
        KeyValuePair<String, String>{L"Nepal Standard Time"sv, L"Asia/Katmandu"sv},
        KeyValuePair<String, String>{L"New Zealand Standard Time"sv, L"Pacific/Auckland"sv},
        KeyValuePair<String, String>{L"Newfoundland Standard Time"sv, L"America/St_Johns"sv},
        KeyValuePair<String, String>{L"North Asia East Standard Time"sv, L"Asia/Irkutsk"sv},
        KeyValuePair<String, String>{L"North Asia Standard Time"sv, L"Asia/Krasnoyarsk"sv},
        KeyValuePair<String, String>{L"Pacific SA Standard Time"sv, L"America/Santiago"sv},
        KeyValuePair<String, String>{L"Pacific Standard Time"sv, L"America/Los_Angeles"sv},
        KeyValuePair<String, String>{L"Pacific Standard Time (Mexico)"sv, L"America/Tijuana"sv},
        KeyValuePair<String, String>{L"Pakistan Standard Time"sv, L"Asia/Karachi"sv},
        KeyValuePair<String, String>{L"Paraguay Standard Time"sv, L"America/Asuncion"sv},
        KeyValuePair<String, String>{L"Romance Standard Time"sv, L"Europe/Paris"sv},
        KeyValuePair<String, String>{L"Russian Standard Time"sv, L"Europe/Moscow"sv},
        KeyValuePair<String, String>{L"SA Eastern Standard Time"sv, L"America/Cayenne"sv},
        KeyValuePair<String, String>{L"SA Pacific Standard Time"sv, L"America/Lima"sv},
        KeyValuePair<String, String>{L"SA Western Standard Time"sv, L"America/La_Paz"sv},
        KeyValuePair<String, String>{L"SE Asia Standard Time"sv, L"Asia/Jakarta"sv},
        KeyValuePair<String, String>{L"Malay Peninsula Standard Time"sv, L"Asia/Singapore"sv},
        KeyValuePair<String, String>{L"South Africa Standard Time"sv, L"Africa/Harare"sv},
        KeyValuePair<String, String>{L"Syria Standard Time"sv, L"Asia/Damascus"sv},
        KeyValuePair<String, String>{L"Taipei Standard Time"sv, L"Asia/Taipei"sv},
        KeyValuePair<String, String>{L"Tasmania Standard Time"sv, L"Australia/Hobart"sv},
        KeyValuePair<String, String>{L"Tokyo Standard Time"sv, L"Asia/Tokyo"sv},
        KeyValuePair<String, String>{L"Tonga Standard Time"sv, L"Pacific/Tongatapu"sv},
        KeyValuePair<String, String>{L"Turkey Standard Time"sv, L"Asia/Istanbul"sv},
        KeyValuePair<String, String>{L"Ulaanbaatar Standard Time"sv, L"Asia/Ulaanbaatar"sv},
        KeyValuePair<String, String>{L"US Eastern Standard Time"sv, L"America/Indianapolis"sv},
        KeyValuePair<String, String>{L"US Mountain Standard Time"sv, L"America/Denver"sv},
        KeyValuePair<String, String>{L"Venezuela Standard Time"sv, L"America/Caracas"sv},
        KeyValuePair<String, String>{L"Vladivostok Standard Time"sv, L"Asia/Vladivostok"sv},
        KeyValuePair<String, String>{L"W. Australia Standard Time"sv, L"Australia/Perth"sv},
        KeyValuePair<String, String>{L"W. Central Africa Standard Time"sv, L"Africa/Brazzaville"sv},
        KeyValuePair<String, String>{L"W. Europe Standard Time"sv, L"Europe/Vienna"sv},
        KeyValuePair<String, String>{L"West Asia Standard Time"sv, L"Asia/Tashkent"sv},
        KeyValuePair<String, String>{L"West Pacific Standard Time"sv, L"Pacific/Port_Moresby"sv},
        KeyValuePair<String, String>{L"Yakutsk Standard Time"sv, L"Asia/Yakutsk"sv},
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

/*
 ********************************************************************************
 **************************** IsDaylightSavingsTime_ ****************************
 ********************************************************************************
 */
namespace {
    bool IsDaylightSavingsTime_ (const Date& date, const optional<TimeOfDay>& tod)
    {
        struct tm asTM = Date2TM_ (date, tod);
        /*
     *  From http://pubs.opengroup.org/onlinepubs/7908799/xsh/mktime.html:
     *
     *      A positive or 0 value for tm_isdst causes mktime() to presume initially that Daylight Savings Time, respectively,
     *      is or is not in effect for the specified time. A negative value for tm_isdst causes mktime() to attempt to determine
     *      whether Daylight Saving Time is in effect for the specified time.
     *
     *      Local timezone information is set as though mktime() called tzset().
     *
     *      Upon successful completion, the values of the tm_wday and tm_yday components of the structure are set appropriately,
     *      and the other components are set to represent the specified time since the Epoch, but with their values forced to the
     *      ranges indicated in the <time.h> entry; the final value of tm_mday is not set until tm_mon and tm_year are determined.
     *
     *  But http://linux.die.net/man/3/localtime says:
     *      The mktime() function modifies the fields of the tm structure as follows: tm_wday and tm_yday are set to values determined
     *      from the contents of the other fields; if structure members are outside their valid interval, they will be normalized
     *      (so that, for example, 40 October is changed into 9 November); tm_isdst is set (regardless of its initial value)
     *      to a positive value or to 0, respectively, to indicate whether DST is or is not in effect at the specified time.
     *
     *  The POSIX part is not totally clear - but the linux docs do make it clear - that we can use this to test if is daylight savings time.
     *
     *  APPEARS to work since... --LGP 2011-10-15
     */
        asTM.tm_isdst = -1; // force calc of correct daylight savings time flag
        // https://stroika.atlassian.net/browse/STK-515  only works back to 1970 (Unix epoch time) - else assume NOT daylight savings time
        if (::mktime (&asTM) == -1) {
            return false;
        }
        else {
            return asTM.tm_isdst >= 1;
        }
    }

    bool IsDaylightSavingsTime_ (const DateTime& d)
    {
        struct tm asTM = d.As<struct tm> ();
        if (d.GetTimezone () == Timezone::kLocalTime) {
            return IsDaylightSavingsTime_ (d.GetDate (), d.GetTimeOfDay ().has_value () ? *d.GetTimeOfDay () : TimeOfDay{0});
        }
        else {
            AssertNotImplemented (); // maybe OK to just assume false given CURRENT (as of 2018-01-15) design of Timezone, but hope to expand soon!
            return false;
        }
    }
}

/*
 ********************************************************************************
 ************************** GetLocaltimeToGMTOffset_ ****************************
 ********************************************************************************
 */
namespace {
    time_t GetLocaltimeToGMTOffset_ (bool applyDST)
    {
#if 0
    // WRONG - but COULD use this API - but not sure needed
#if qPlatform_Windows
    TIME_ZONE_INFORMATION   tzInfo {};
    (void)::GetTimeZoneInformation (&tzInfo);
    int unsignedBias    =   abs (tzInfo.Bias);
    int hrs =   unsignedBias / 60;
    int mins = unsignedBias - hrs * 60;
    tzBiasString = ::Format (L"%s%.2d:%.2d", (tzInfo.Bias >= 0 ? L"-" : L"+"), hrs, mins);
#endif
#endif

        /*
         * COULD this be cached? It SHOULD be - but what about when the timezone changes? there maybe a better way to compute this using the
         * timezone global var???
         */
        struct tm aTm {
        };
        aTm.tm_year = 70;
        aTm.tm_mon  = 0; // Jan
        aTm.tm_mday = 1;
        constexpr bool kImplErrorUnderflow_{true}; // Only KNOWN to be needed on windows with TZ=America/New_York, but probably we should always do this
        if (kImplErrorUnderflow_) {
            aTm.tm_mday++;
        }
        aTm.tm_isdst  = applyDST;
        time_t result = ::mktime (&aTm);
        Assert (result != -1); // this shouldn't fail
        if (kImplErrorUnderflow_) {
            result -= 24 * 60 * 60;
        }
        Ensure (-60 * 60 * 24 <= result and result <= 60 * 60 * 24); // sanity check
        return result;
    }

    time_t GetLocaltimeToGMTOffset_ (const DateTime& forTime)
    {
        return GetLocaltimeToGMTOffset_ (IsDaylightSavingsTime_ (forTime));
    }
}
