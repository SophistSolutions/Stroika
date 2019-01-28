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
        Execution::Throw (Execution::StringException (L"invalid timezone offset"));
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
        Execution::Throw (Execution::StringException (L"invalid timezone offset"));
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
        static const String kUTC_ = L"GMT"_k; // UT or GMT for UTC in  https://tools.ietf.org/html/rfc822#section-5
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
        result.fID = Streams::TextReader::New (IO::FileSystem::FileInputStream::New (L"/etc/timezone"_k)).ReadAll ().Trim ();
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
        KeyValuePair<String, String>{L"Afghanistan Standard Time"_k, L"Asia/Kabul"_k},
        KeyValuePair<String, String>{L"Alaskan Standard Time"_k, L"America/Juneau"_k},
        KeyValuePair<String, String>{L"Arab Standard Time"_k, L"Asia/Riyadh"_k},
        KeyValuePair<String, String>{L"Arabian Standard Time"_k, L"Asia/Muscat"_k},
        KeyValuePair<String, String>{L"Arabic Standard Time"_k, L"Asia/Baghdad"_k},
        KeyValuePair<String, String>{L"Argentina Standard Time"_k, L"America/Rosario"_k},
        KeyValuePair<String, String>{L"Atlantic Standard Time"_k, L"America/Halifax"_k},
        KeyValuePair<String, String>{L"AUS Central Standard Time"_k, L"Australia/Darwin"_k},
        KeyValuePair<String, String>{L"AUS Eastern Standard Time"_k, L"Australia/Sydney"_k},
        KeyValuePair<String, String>{L"Azerbaijan Standard Time"_k, L"Asia/Baku"_k},
        KeyValuePair<String, String>{L"Azores Standard Time"_k, L"Atlantic/Azores"_k},
        KeyValuePair<String, String>{L"Bahia Standard Time"_k, L"America/Maceio"_k},
        KeyValuePair<String, String>{L"Bangladesh Standard Time"_k, L"Asia/Dhaka"_k},
        KeyValuePair<String, String>{L"Canada Central Standard Time"_k, L"America/Swift_Current"_k},
        KeyValuePair<String, String>{L"Cape Verde Standard Time"_k, L"Atlantic/Cape_Verde"_k},
        KeyValuePair<String, String>{L"Caucasus Standard Time"_k, L"Asia/Yerevan"_k},
        KeyValuePair<String, String>{L"Cen. Australia Standard Time"_k, L"Australia/Adelaide"_k},
        KeyValuePair<String, String>{L"Central America Standard Time"_k, L"America/Tegucigalpa"_k},
        KeyValuePair<String, String>{L"Central Asia Standard Time"_k, L"Asia/Almaty"_k},
        KeyValuePair<String, String>{L"Central Brazilian Standard Time"_k, L"America/Cuiaba"_k},
        KeyValuePair<String, String>{L"Central Europe Standard Time"_k, L"Europe/Prague"_k},
        KeyValuePair<String, String>{L"Central European Standard Time"_k, L"Europe/Skopje"_k},
        KeyValuePair<String, String>{L"Central Pacific Standard Time"_k, L"Pacific/Guadalcanal"_k},
        KeyValuePair<String, String>{L"Central Standard Time"_k, L"America/Chicago"_k},
        KeyValuePair<String, String>{L"Central Standard Time (Mexico)"_k, L"America/Monterrey"_k},
        KeyValuePair<String, String>{L"China Standard Time"_k, L"Asia/Urumqi"_k},
        KeyValuePair<String, String>{L"E. Africa Standard Time"_k, L"Africa/Nairobi"_k},
        KeyValuePair<String, String>{L"E. Australia Standard Time"_k, L"Australia/Brisbane"_k},
        KeyValuePair<String, String>{L"E. Europe Standard Time"_k, L"Europe/Bucharest"_k},
        KeyValuePair<String, String>{L"E. South America Standard Time"_k, L"America/Sao_Paulo"_k},
        KeyValuePair<String, String>{L"Eastern Standard Time"_k, L"America/New_York"_k},
        KeyValuePair<String, String>{L"Egypt Standard Time"_k, L"Africa/Cairo"_k},
        KeyValuePair<String, String>{L"Ekaterinburg Standard Time"_k, L"Asia/Yekaterinburg"_k},
        KeyValuePair<String, String>{L"Fiji Standard Time"_k, L"Pacific/Fiji"_k},
        KeyValuePair<String, String>{L"FLE Standard Time"_k, L"Europe/Helsinki"_k},
        KeyValuePair<String, String>{L"Georgian Standard Time"_k, L"Asia/Tbilisi"_k},
        KeyValuePair<String, String>{L"GMT Standard Time"_k, L"Europe/London"_k},
        KeyValuePair<String, String>{L"Greenland Standard Time"_k, L"America/Godthab"_k},
        KeyValuePair<String, String>{L"Greenwich Standard Time"_k, L"Atlantic/Reykjavik"_k},
        KeyValuePair<String, String>{L"GTB Standard Time"_k, L"Europe/Bucharest"_k},
        KeyValuePair<String, String>{L"Hawaiian Standard Time"_k, L"Pacific/Honolulu"_k},
        KeyValuePair<String, String>{L"India Standard Time"_k, L"Asia/Calcutta"_k},
        KeyValuePair<String, String>{L"Iran Standard Time"_k, L"Asia/Tehran"_k},
        KeyValuePair<String, String>{L"Jerusalem Standard Time"_k, L"Asia/Jerusalem"_k},
        KeyValuePair<String, String>{L"Jordan Standard Time"_k, L"Asia/Amman"_k},
        KeyValuePair<String, String>{L"Kaliningrad Standard Time"_k, L"Europe/Kaliningrad"_k},
        KeyValuePair<String, String>{L"Korea Standard Time"_k, L"Asia/Seoul"_k},
        KeyValuePair<String, String>{L"Libya Standard Time"_k, L"Africa/Tripoli"_k},
        KeyValuePair<String, String>{L"Magadan Standard Time"_k, L"Asia/Magadan"_k},
        KeyValuePair<String, String>{L"Mauritius Standard Time"_k, L"Indian/Mauritius"_k},
        KeyValuePair<String, String>{L"Middle East Standard Time"_k, L"Asia/Beirut"_k},
        KeyValuePair<String, String>{L"Montevideo Standard Time"_k, L"America/Montevideo"_k},
        KeyValuePair<String, String>{L"Morocco Standard Time"_k, L"Africa/Casablanca"_k},
        KeyValuePair<String, String>{L"Mountain Standard Time"_k, L"America/Denver"_k},
        KeyValuePair<String, String>{L"Mountain Standard Time (Mexico)"_k, L"America/Mazatlan"_k},
        KeyValuePair<String, String>{L"Myanmar Standard Time"_k, L"Asia/Rangoon"_k},
        KeyValuePair<String, String>{L"N. Central Asia Standard Time"_k, L"Asia/Novosibirsk"_k},
        KeyValuePair<String, String>{L"Namibia Standard Time"_k, L"Africa/Windhoek"_k},
        KeyValuePair<String, String>{L"Nepal Standard Time"_k, L"Asia/Katmandu"_k},
        KeyValuePair<String, String>{L"New Zealand Standard Time"_k, L"Pacific/Auckland"_k},
        KeyValuePair<String, String>{L"Newfoundland Standard Time"_k, L"America/St_Johns"_k},
        KeyValuePair<String, String>{L"North Asia East Standard Time"_k, L"Asia/Irkutsk"_k},
        KeyValuePair<String, String>{L"North Asia Standard Time"_k, L"Asia/Krasnoyarsk"_k},
        KeyValuePair<String, String>{L"Pacific SA Standard Time"_k, L"America/Santiago"_k},
        KeyValuePair<String, String>{L"Pacific Standard Time"_k, L"America/Los_Angeles"_k},
        KeyValuePair<String, String>{L"Pacific Standard Time (Mexico)"_k, L"America/Tijuana"_k},
        KeyValuePair<String, String>{L"Pakistan Standard Time"_k, L"Asia/Karachi"_k},
        KeyValuePair<String, String>{L"Paraguay Standard Time"_k, L"America/Asuncion"_k},
        KeyValuePair<String, String>{L"Romance Standard Time"_k, L"Europe/Paris"_k},
        KeyValuePair<String, String>{L"Russian Standard Time"_k, L"Europe/Moscow"_k},
        KeyValuePair<String, String>{L"SA Eastern Standard Time"_k, L"America/Cayenne"_k},
        KeyValuePair<String, String>{L"SA Pacific Standard Time"_k, L"America/Lima"_k},
        KeyValuePair<String, String>{L"SA Western Standard Time"_k, L"America/La_Paz"_k},
        KeyValuePair<String, String>{L"SE Asia Standard Time"_k, L"Asia/Jakarta"_k},
        KeyValuePair<String, String>{L"Malay Peninsula Standard Time"_k, L"Asia/Singapore"_k},
        KeyValuePair<String, String>{L"South Africa Standard Time"_k, L"Africa/Harare"_k},
        KeyValuePair<String, String>{L"Syria Standard Time"_k, L"Asia/Damascus"_k},
        KeyValuePair<String, String>{L"Taipei Standard Time"_k, L"Asia/Taipei"_k},
        KeyValuePair<String, String>{L"Tasmania Standard Time"_k, L"Australia/Hobart"_k},
        KeyValuePair<String, String>{L"Tokyo Standard Time"_k, L"Asia/Tokyo"_k},
        KeyValuePair<String, String>{L"Tonga Standard Time"_k, L"Pacific/Tongatapu"_k},
        KeyValuePair<String, String>{L"Turkey Standard Time"_k, L"Asia/Istanbul"_k},
        KeyValuePair<String, String>{L"Ulaanbaatar Standard Time"_k, L"Asia/Ulaanbaatar"_k},
        KeyValuePair<String, String>{L"US Eastern Standard Time"_k, L"America/Indianapolis"_k},
        KeyValuePair<String, String>{L"US Mountain Standard Time"_k, L"America/Denver"_k},
        KeyValuePair<String, String>{L"Venezuela Standard Time"_k, L"America/Caracas"_k},
        KeyValuePair<String, String>{L"Vladivostok Standard Time"_k, L"Asia/Vladivostok"_k},
        KeyValuePair<String, String>{L"W. Australia Standard Time"_k, L"Australia/Perth"_k},
        KeyValuePair<String, String>{L"W. Central Africa Standard Time"_k, L"Africa/Brazzaville"_k},
        KeyValuePair<String, String>{L"W. Europe Standard Time"_k, L"Europe/Vienna"_k},
        KeyValuePair<String, String>{L"West Asia Standard Time"_k, L"Asia/Tashkent"_k},
        KeyValuePair<String, String>{L"West Pacific Standard Time"_k, L"Pacific/Port_Moresby"_k},
        KeyValuePair<String, String>{L"Yakutsk Standard Time"_k, L"Asia/Yakutsk"_k},
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
