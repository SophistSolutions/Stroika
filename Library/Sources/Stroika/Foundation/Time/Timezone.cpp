/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstdlib>
#include <cstring>
#include <ctime>

#if qPlatform_POSIX
#include <time.h>
#endif

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

namespace {
    tm Date2TM_ (const Date& d, const TimeOfDay& tod)
    {
        struct tm tm {
        };
        tm.tm_year                         = static_cast<int> (d.GetYear ()) - 1900;
        tm.tm_mon                          = static_cast<int> (d.GetMonth ()) - 1;
        tm.tm_mday                         = static_cast<int> (d.GetDayOfMonth ());
        unsigned int totalSecondsRemaining = tod.GetAsSecondsCount ();
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
    bool IsDaylightSavingsTime_ (const Date& date, const TimeOfDay& tod);
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
#else
constexpr Timezone           Timezone::kLocalTime;
constexpr Timezone           Timezone::kUTC;
constexpr optional<Timezone> Timezone::kUnknown;
#endif

Timezone::BiasInMinutesFromUTCType Timezone::GetBiasInMinutesFromUTCType (const Date& date, const TimeOfDay& tod) const
{
    switch (fTZ_) {
        case TZ_::eUTC:
            return 0;
        case TZ_::eFixedOffsetBias:
            return fBiasInMinutesFromUTC_;
        case TZ_::eLocalTime:
            return static_cast<BiasInMinutesFromUTCType> (-GetLocaltimeToGMTOffset_ (IsDaylightSavingsTime_ (date, tod)));
        default:
            AssertNotReached ();
            return 0;
    }
}

optional<bool> Timezone::IsDaylightSavingsTime (const Date& date, const TimeOfDay& tod)
{
    // @todo - fix for other (not fixed) timezones - like America/NewYork
    if (fTZ_ == TZ_::eLocalTime) {
        return IsDaylightSavingsTime_ (date, tod);
    }
    else {
        return {};
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
        result.fID = Streams::TextReader::New (IO::FileSystem::FileInputStream::New (String_Constant{L"/etc/timezone"})).ReadAll ().Trim ();
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
        KeyValuePair<String, String>{String_Constant{L"Afghanistan Standard Time"}, String_Constant{L"Asia/Kabul"}},
        KeyValuePair<String, String>{String_Constant{L"Alaskan Standard Time"}, String_Constant{L"America/Juneau"}},
        KeyValuePair<String, String>{String_Constant{L"Arab Standard Time"}, String_Constant{L"Asia/Riyadh"}},
        KeyValuePair<String, String>{String_Constant{L"Arabian Standard Time"}, String_Constant{L"Asia/Muscat"}},
        KeyValuePair<String, String>{String_Constant{L"Arabic Standard Time"}, String_Constant{L"Asia/Baghdad"}},
        KeyValuePair<String, String>{String_Constant{L"Argentina Standard Time"}, String_Constant{L"America/Rosario"}},
        KeyValuePair<String, String>{String_Constant{L"Atlantic Standard Time"}, String_Constant{L"America/Halifax"}},
        KeyValuePair<String, String>{String_Constant{L"AUS Central Standard Time"}, String_Constant{L"Australia/Darwin"}},
        KeyValuePair<String, String>{String_Constant{L"AUS Eastern Standard Time"}, String_Constant{L"Australia/Sydney"}},
        KeyValuePair<String, String>{String_Constant{L"Azerbaijan Standard Time"}, String_Constant{L"Asia/Baku"}},
        KeyValuePair<String, String>{String_Constant{L"Azores Standard Time"}, String_Constant{L"Atlantic/Azores"}},
        KeyValuePair<String, String>{String_Constant{L"Bahia Standard Time"}, String_Constant{L"America/Maceio"}},
        KeyValuePair<String, String>{String_Constant{L"Bangladesh Standard Time"}, String_Constant{L"Asia/Dhaka"}},
        KeyValuePair<String, String>{String_Constant{L"Canada Central Standard Time"}, String_Constant{L"America/Swift_Current"}},
        KeyValuePair<String, String>{String_Constant{L"Cape Verde Standard Time"}, String_Constant{L"Atlantic/Cape_Verde"}},
        KeyValuePair<String, String>{String_Constant{L"Caucasus Standard Time"}, String_Constant{L"Asia/Yerevan"}},
        KeyValuePair<String, String>{String_Constant{L"Cen. Australia Standard Time"}, String_Constant{L"Australia/Adelaide"}},
        KeyValuePair<String, String>{String_Constant{L"Central America Standard Time"}, String_Constant{L"America/Tegucigalpa"}},
        KeyValuePair<String, String>{String_Constant{L"Central Asia Standard Time"}, String_Constant{L"Asia/Almaty"}},
        KeyValuePair<String, String>{String_Constant{L"Central Brazilian Standard Time"}, String_Constant{L"America/Cuiaba"}},
        KeyValuePair<String, String>{String_Constant{L"Central Europe Standard Time"}, String_Constant{L"Europe/Prague"}},
        KeyValuePair<String, String>{String_Constant{L"Central European Standard Time"}, String_Constant{L"Europe/Skopje"}},
        KeyValuePair<String, String>{String_Constant{L"Central Pacific Standard Time"}, String_Constant{L"Pacific/Guadalcanal"}},
        KeyValuePair<String, String>{String_Constant{L"Central Standard Time"}, String_Constant{L"America/Chicago"}},
        KeyValuePair<String, String>{String_Constant{L"Central Standard Time (Mexico)"}, String_Constant{L"America/Monterrey"}},
        KeyValuePair<String, String>{String_Constant{L"China Standard Time"}, String_Constant{L"Asia/Urumqi"}},
        KeyValuePair<String, String>{String_Constant{L"E. Africa Standard Time"}, String_Constant{L"Africa/Nairobi"}},
        KeyValuePair<String, String>{String_Constant{L"E. Australia Standard Time"}, String_Constant{L"Australia/Brisbane"}},
        KeyValuePair<String, String>{String_Constant{L"E. Europe Standard Time"}, String_Constant{L"Europe/Bucharest"}},
        KeyValuePair<String, String>{String_Constant{L"E. South America Standard Time"}, String_Constant{L"America/Sao_Paulo"}},
        KeyValuePair<String, String>{String_Constant{L"Eastern Standard Time"}, String_Constant{L"America/New_York"}},
        KeyValuePair<String, String>{String_Constant{L"Egypt Standard Time"}, String_Constant{L"Africa/Cairo"}},
        KeyValuePair<String, String>{String_Constant{L"Ekaterinburg Standard Time"}, String_Constant{L"Asia/Yekaterinburg"}},
        KeyValuePair<String, String>{String_Constant{L"Fiji Standard Time"}, String_Constant{L"Pacific/Fiji"}},
        KeyValuePair<String, String>{String_Constant{L"FLE Standard Time"}, String_Constant{L"Europe/Helsinki"}},
        KeyValuePair<String, String>{String_Constant{L"Georgian Standard Time"}, String_Constant{L"Asia/Tbilisi"}},
        KeyValuePair<String, String>{String_Constant{L"GMT Standard Time"}, String_Constant{L"Europe/London"}},
        KeyValuePair<String, String>{String_Constant{L"Greenland Standard Time"}, String_Constant{L"America/Godthab"}},
        KeyValuePair<String, String>{String_Constant{L"Greenwich Standard Time"}, String_Constant{L"Atlantic/Reykjavik"}},
        KeyValuePair<String, String>{String_Constant{L"GTB Standard Time"}, String_Constant{L"Europe/Bucharest"}},
        KeyValuePair<String, String>{String_Constant{L"Hawaiian Standard Time"}, String_Constant{L"Pacific/Honolulu"}},
        KeyValuePair<String, String>{String_Constant{L"India Standard Time"}, String_Constant{L"Asia/Calcutta"}},
        KeyValuePair<String, String>{String_Constant{L"Iran Standard Time"}, String_Constant{L"Asia/Tehran"}},
        KeyValuePair<String, String>{String_Constant{L"Jerusalem Standard Time"}, String_Constant{L"Asia/Jerusalem"}},
        KeyValuePair<String, String>{String_Constant{L"Jordan Standard Time"}, String_Constant{L"Asia/Amman"}},
        KeyValuePair<String, String>{String_Constant{L"Kaliningrad Standard Time"}, String_Constant{L"Europe/Kaliningrad"}},
        KeyValuePair<String, String>{String_Constant{L"Korea Standard Time"}, String_Constant{L"Asia/Seoul"}},
        KeyValuePair<String, String>{String_Constant{L"Libya Standard Time"}, String_Constant{L"Africa/Tripoli"}},
        KeyValuePair<String, String>{String_Constant{L"Magadan Standard Time"}, String_Constant{L"Asia/Magadan"}},
        KeyValuePair<String, String>{String_Constant{L"Mauritius Standard Time"}, String_Constant{L"Indian/Mauritius"}},
        KeyValuePair<String, String>{String_Constant{L"Middle East Standard Time"}, String_Constant{L"Asia/Beirut"}},
        KeyValuePair<String, String>{String_Constant{L"Montevideo Standard Time"}, String_Constant{L"America/Montevideo"}},
        KeyValuePair<String, String>{String_Constant{L"Morocco Standard Time"}, String_Constant{L"Africa/Casablanca"}},
        KeyValuePair<String, String>{String_Constant{L"Mountain Standard Time"}, String_Constant{L"America/Denver"}},
        KeyValuePair<String, String>{String_Constant{L"Mountain Standard Time (Mexico)"}, String_Constant{L"America/Mazatlan"}},
        KeyValuePair<String, String>{String_Constant{L"Myanmar Standard Time"}, String_Constant{L"Asia/Rangoon"}},
        KeyValuePair<String, String>{String_Constant{L"N. Central Asia Standard Time"}, String_Constant{L"Asia/Novosibirsk"}},
        KeyValuePair<String, String>{String_Constant{L"Namibia Standard Time"}, String_Constant{L"Africa/Windhoek"}},
        KeyValuePair<String, String>{String_Constant{L"Nepal Standard Time"}, String_Constant{L"Asia/Katmandu"}},
        KeyValuePair<String, String>{String_Constant{L"New Zealand Standard Time"}, String_Constant{L"Pacific/Auckland"}},
        KeyValuePair<String, String>{String_Constant{L"Newfoundland Standard Time"}, String_Constant{L"America/St_Johns"}},
        KeyValuePair<String, String>{String_Constant{L"North Asia East Standard Time"}, String_Constant{L"Asia/Irkutsk"}},
        KeyValuePair<String, String>{String_Constant{L"North Asia Standard Time"}, String_Constant{L"Asia/Krasnoyarsk"}},
        KeyValuePair<String, String>{String_Constant{L"Pacific SA Standard Time"}, String_Constant{L"America/Santiago"}},
        KeyValuePair<String, String>{String_Constant{L"Pacific Standard Time"}, String_Constant{L"America/Los_Angeles"}},
        KeyValuePair<String, String>{String_Constant{L"Pacific Standard Time (Mexico)"}, String_Constant{L"America/Tijuana"}},
        KeyValuePair<String, String>{String_Constant{L"Pakistan Standard Time"}, String_Constant{L"Asia/Karachi"}},
        KeyValuePair<String, String>{String_Constant{L"Paraguay Standard Time"}, String_Constant{L"America/Asuncion"}},
        KeyValuePair<String, String>{String_Constant{L"Romance Standard Time"}, String_Constant{L"Europe/Paris"}},
        KeyValuePair<String, String>{String_Constant{L"Russian Standard Time"}, String_Constant{L"Europe/Moscow"}},
        KeyValuePair<String, String>{String_Constant{L"SA Eastern Standard Time"}, String_Constant{L"America/Cayenne"}},
        KeyValuePair<String, String>{String_Constant{L"SA Pacific Standard Time"}, String_Constant{L"America/Lima"}},
        KeyValuePair<String, String>{String_Constant{L"SA Western Standard Time"}, String_Constant{L"America/La_Paz"}},
        KeyValuePair<String, String>{String_Constant{L"SE Asia Standard Time"}, String_Constant{L"Asia/Jakarta"}},
        KeyValuePair<String, String>{String_Constant{L"Malay Peninsula Standard Time"}, String_Constant{L"Asia/Singapore"}},
        KeyValuePair<String, String>{String_Constant{L"South Africa Standard Time"}, String_Constant{L"Africa/Harare"}},
        KeyValuePair<String, String>{String_Constant{L"Syria Standard Time"}, String_Constant{L"Asia/Damascus"}},
        KeyValuePair<String, String>{String_Constant{L"Taipei Standard Time"}, String_Constant{L"Asia/Taipei"}},
        KeyValuePair<String, String>{String_Constant{L"Tasmania Standard Time"}, String_Constant{L"Australia/Hobart"}},
        KeyValuePair<String, String>{String_Constant{L"Tokyo Standard Time"}, String_Constant{L"Asia/Tokyo"}},
        KeyValuePair<String, String>{String_Constant{L"Tonga Standard Time"}, String_Constant{L"Pacific/Tongatapu"}},
        KeyValuePair<String, String>{String_Constant{L"Turkey Standard Time"}, String_Constant{L"Asia/Istanbul"}},
        KeyValuePair<String, String>{String_Constant{L"Ulaanbaatar Standard Time"}, String_Constant{L"Asia/Ulaanbaatar"}},
        KeyValuePair<String, String>{String_Constant{L"US Eastern Standard Time"}, String_Constant{L"America/Indianapolis"}},
        KeyValuePair<String, String>{String_Constant{L"US Mountain Standard Time"}, String_Constant{L"America/Denver"}},
        KeyValuePair<String, String>{String_Constant{L"Venezuela Standard Time"}, String_Constant{L"America/Caracas"}},
        KeyValuePair<String, String>{String_Constant{L"Vladivostok Standard Time"}, String_Constant{L"Asia/Vladivostok"}},
        KeyValuePair<String, String>{String_Constant{L"W. Australia Standard Time"}, String_Constant{L"Australia/Perth"}},
        KeyValuePair<String, String>{String_Constant{L"W. Central Africa Standard Time"}, String_Constant{L"Africa/Brazzaville"}},
        KeyValuePair<String, String>{String_Constant{L"W. Europe Standard Time"}, String_Constant{L"Europe/Vienna"}},
        KeyValuePair<String, String>{String_Constant{L"West Asia Standard Time"}, String_Constant{L"Asia/Tashkent"}},
        KeyValuePair<String, String>{String_Constant{L"West Pacific Standard Time"}, String_Constant{L"Pacific/Port_Moresby"}},
        KeyValuePair<String, String>{String_Constant{L"Yakutsk Standard Time"}, String_Constant{L"Asia/Yakutsk"}},
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
    bool IsDaylightSavingsTime_ (const Date& date, const TimeOfDay& tod)
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
            return IsDaylightSavingsTime_ (d.GetDate (), d.GetTimeOfDay ());
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
