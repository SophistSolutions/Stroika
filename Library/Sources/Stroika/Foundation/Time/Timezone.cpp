/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cstdlib>
#include    <cstring>
#include    <ctime>

#if     qPlatform_POSIX
#include    <time.h>
#endif

#include    "../Configuration/Common.h"
#include    "../Characters/String.h"
#include    "../Characters/String_Constant.h"
#include    "../Containers/Mapping.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"
#include    "DateTime.h"
#include    "../Execution/ProcessRunner.h"
#include    "../IO/FileSystem/BinaryFileInputStream.h"
#include    "../Streams/TextInputStreamBinaryAdapter.h"

#include    "Timezone.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Time;





/*
 ********************************************************************************
 ***************************** Time::GetTimezoneInfo ****************************
 ********************************************************************************
 */
TimeZoneInformationType    Time::GetTimezoneInfo ()
{
    TimeZoneInformationType result;
#if     qPlatform_POSIX
    try {
        result.fID = Streams::TextInputStreamBinaryAdapter (IO::FileSystem::BinaryFileInputStream::mk (String_Constant (L"/etc/timezone"))).ReadAll ().Trim ();
    }
    catch (...) {
        DbgTrace ("Ignoring missing ID from /etc/timezone");
    }
    if (result.fID.IsMissing ()) {
        try {
            result.fID = Execution::ProcessRunner (L"date +%Z").Run (String ()).Trim ();
        }
        catch (...) {
            DbgTrace ("Ignoring missing ID from date +%Z");
        }
    }
    if (result.fID.IsMissing ()) {
        // We could look to see if /etc/localtime is a symlink or a copy of any named file from /usr/share/zoneinfo, but
        // hope thats not needed!
    }
    // @see http://pubs.opengroup.org/onlinepubs/7908799/xsh/tzset.html
    result.fStandardTime.fName = String::FromSDKString (tzname[0]);
    result.fDaylightSavingsTime.fName = String::FromSDKString (tzname[1]);
#elif   qPlatform_Windows
    using   Containers::Mapping;
    using   Common::KeyValuePair;
    static const    Mapping<String, String> kWinDoze2OlsonName_ =  {
        KeyValuePair<String, String> { L"Afghanistan Standard Time", L"Asia/Kabul" },
        KeyValuePair<String, String> { L"Alaskan Standard Time", L"America/Juneau" },
        KeyValuePair<String, String> { L"Arab Standard Time", L"Asia/Riyadh" },
        KeyValuePair<String, String> { L"Arabian Standard Time", L"Asia/Muscat" },
        KeyValuePair<String, String> { L"Arabic Standard Time", L"Asia/Baghdad" },
        KeyValuePair<String, String> { L"Argentina Standard Time", L"America/Rosario" },
        KeyValuePair<String, String> { L"Atlantic Standard Time", L"America/Halifax" },
        KeyValuePair<String, String> { L"AUS Central Standard Time", L"Australia/Darwin" },
        KeyValuePair<String, String> { L"AUS Eastern Standard Time", L"Australia/Sydney" },
        KeyValuePair<String, String> { L"Azerbaijan Standard Time", L"Asia/Baku" },
        KeyValuePair<String, String> { L"Azores Standard Time", L"Atlantic/Azores" },
        KeyValuePair<String, String> { L"Bahia Standard Time", L"America/Maceio" },
        KeyValuePair<String, String> { L"Bangladesh Standard Time", L"Asia/Dhaka" },
        KeyValuePair<String, String> { L"Canada Central Standard Time", L"America/Swift_Current" },
        KeyValuePair<String, String> { L"Cape Verde Standard Time", L"Atlantic/Cape_Verde" },
        KeyValuePair<String, String> { L"Caucasus Standard Time", L"Asia/Yerevan" },
        KeyValuePair<String, String> { L"Cen. Australia Standard Time", L"Australia/Adelaide" },
        KeyValuePair<String, String> { L"Central America Standard Time", L"America/Tegucigalpa" },
        KeyValuePair<String, String> { L"Central Asia Standard Time", L"Asia/Almaty" },
        KeyValuePair<String, String> { L"Central Brazilian Standard Time", L"America/Cuiaba" },
        KeyValuePair<String, String> { L"Central Europe Standard Time", L"Europe/Prague" },
        KeyValuePair<String, String> { L"Central European Standard Time", L"Europe/Skopje" },
        KeyValuePair<String, String> { L"Central Pacific Standard Time", L"Pacific/Guadalcanal" },
        KeyValuePair<String, String> { L"Central Standard Time", L"America/Chicago" },
        KeyValuePair<String, String> { L"Central Standard Time (Mexico)", L"America/Monterrey" },
        KeyValuePair<String, String> { L"China Standard Time", L"Asia/Urumqi" },
        KeyValuePair<String, String> { L"E. Africa Standard Time", L"Africa/Nairobi" },
        KeyValuePair<String, String> { L"E. Australia Standard Time", L"Australia/Brisbane" },
        KeyValuePair<String, String> { L"E. Europe Standard Time", L"Europe/Bucharest" },
        KeyValuePair<String, String> { L"E. South America Standard Time", L"America/Sao_Paulo" },
        KeyValuePair<String, String> { L"Eastern Standard Time", L"America/New_York" },
        KeyValuePair<String, String> { L"Egypt Standard Time", L"Africa/Cairo" },
        KeyValuePair<String, String> { L"Ekaterinburg Standard Time", L"Asia/Yekaterinburg" },
        KeyValuePair<String, String> { L"Fiji Standard Time", L"Pacific/Fiji" },
        KeyValuePair<String, String> { L"FLE Standard Time", L"Europe/Helsinki" },
        KeyValuePair<String, String> { L"Georgian Standard Time", L"Asia/Tbilisi" },
        KeyValuePair<String, String> { L"GMT Standard Time", L"Europe/London" },
        KeyValuePair<String, String> { L"Greenland Standard Time", L"America/Godthab" },
        KeyValuePair<String, String> { L"Greenwich Standard Time", L"Atlantic/Reykjavik" },
        KeyValuePair<String, String> { L"GTB Standard Time", L"Europe/Bucharest" },
        KeyValuePair<String, String> { L"Hawaiian Standard Time", L"Pacific/Honolulu" },
        KeyValuePair<String, String> { L"India Standard Time", L"Asia/Calcutta" },
        KeyValuePair<String, String> { L"Iran Standard Time", L"Asia/Tehran" },
        KeyValuePair<String, String> { L"Jerusalem Standard Time", L"Asia/Jerusalem" },
        KeyValuePair<String, String> { L"Jordan Standard Time", L"Asia/Amman" },
        KeyValuePair<String, String> { L"Kaliningrad Standard Time", L"Europe/Kaliningrad" },
        KeyValuePair<String, String> { L"Korea Standard Time", L"Asia/Seoul" },
        KeyValuePair<String, String> { L"Libya Standard Time", L"Africa/Tripoli" },
        KeyValuePair<String, String> { L"Magadan Standard Time", L"Asia/Magadan" },
        KeyValuePair<String, String> { L"Mauritius Standard Time", L"Indian/Mauritius" },
        KeyValuePair<String, String> { L"Middle East Standard Time", L"Asia/Beirut" },
        KeyValuePair<String, String> { L"Montevideo Standard Time", L"America/Montevideo" },
        KeyValuePair<String, String> { L"Morocco Standard Time", L"Africa/Casablanca" },
        KeyValuePair<String, String> { L"Mountain Standard Time", L"America/Denver" },
        KeyValuePair<String, String> { L"Mountain Standard Time (Mexico)", L"America/Mazatlan" },
        KeyValuePair<String, String> { L"Myanmar Standard Time", L"Asia/Rangoon" },
        KeyValuePair<String, String> { L"N. Central Asia Standard Time", L"Asia/Novosibirsk" },
        KeyValuePair<String, String> { L"Namibia Standard Time", L"Africa/Windhoek" },
        KeyValuePair<String, String> { L"Nepal Standard Time", L"Asia/Katmandu" },
        KeyValuePair<String, String> { L"New Zealand Standard Time", L"Pacific/Auckland" },
        KeyValuePair<String, String> { L"Newfoundland Standard Time", L"America/St_Johns" },
        KeyValuePair<String, String> { L"North Asia East Standard Time", L"Asia/Irkutsk" },
        KeyValuePair<String, String> { L"North Asia Standard Time", L"Asia/Krasnoyarsk" },
        KeyValuePair<String, String> { L"Pacific SA Standard Time", L"America/Santiago" },
        KeyValuePair<String, String> { L"Pacific Standard Time", L"America/Los_Angeles" },
        KeyValuePair<String, String> { L"Pacific Standard Time (Mexico)", L"America/Tijuana" },
        KeyValuePair<String, String> { L"Pakistan Standard Time", L"Asia/Karachi" },
        KeyValuePair<String, String> { L"Paraguay Standard Time", L"America/Asuncion" },
        KeyValuePair<String, String> { L"Romance Standard Time", L"Europe/Paris" },
        KeyValuePair<String, String> { L"Russian Standard Time", L"Europe/Moscow" },
        KeyValuePair<String, String> { L"SA Eastern Standard Time", L"America/Cayenne" },
        KeyValuePair<String, String> { L"SA Pacific Standard Time", L"America/Lima" },
        KeyValuePair<String, String> { L"SA Western Standard Time", L"America/La_Paz" },
        KeyValuePair<String, String> { L"SE Asia Standard Time", L"Asia/Jakarta" },
        KeyValuePair<String, String> { L"Malay Peninsula Standard Time", L"Asia/Singapore" },
        KeyValuePair<String, String> { L"South Africa Standard Time", L"Africa/Harare" },
        KeyValuePair<String, String> { L"Syria Standard Time", L"Asia/Damascus" },
        KeyValuePair<String, String> { L"Taipei Standard Time", L"Asia/Taipei" },
        KeyValuePair<String, String> { L"Tasmania Standard Time", L"Australia/Hobart" },
        KeyValuePair<String, String> { L"Tokyo Standard Time", L"Asia/Tokyo" },
        KeyValuePair<String, String> { L"Tonga Standard Time", L"Pacific/Tongatapu" },
        KeyValuePair<String, String> { L"Turkey Standard Time", L"Asia/Istanbul" },
        KeyValuePair<String, String> { L"Ulaanbaatar Standard Time", L"Asia/Ulaanbaatar" },
        KeyValuePair<String, String> { L"US Eastern Standard Time", L"America/Indianapolis" },
        KeyValuePair<String, String> { L"US Mountain Standard Time", L"America/Denver" },
        KeyValuePair<String, String> { L"Venezuela Standard Time", L"America/Caracas" },
        KeyValuePair<String, String> { L"Vladivostok Standard Time", L"Asia/Vladivostok" },
        KeyValuePair<String, String> { L"W. Australia Standard Time", L"Australia/Perth" },
        KeyValuePair<String, String> { L"W. Central Africa Standard Time", L"Africa/Brazzaville" },
        KeyValuePair<String, String> { L"W. Europe Standard Time", L"Europe/Vienna" },
        KeyValuePair<String, String> { L"West Asia Standard Time", L"Asia/Tashkent" },
        KeyValuePair<String, String> { L"West Pacific Standard Time", L"Pacific/Port_Moresby" },
        KeyValuePair<String, String> { L"Yakutsk Standard Time", L"Asia/Yakutsk" }
    };
    TIME_ZONE_INFORMATION   tzInfo;
    memset (&tzInfo, 0, sizeof (tzInfo));
    (void)::GetTimeZoneInformation (&tzInfo);
    result.fStandardTime.fName = tzInfo.StandardName;
    result.fDaylightSavingsTime.fName = tzInfo.DaylightName;
    result.fID = kWinDoze2OlsonName_.LookupValue (tzInfo.StandardName, tzInfo.StandardName);
#else
    AssertNotImplemented ();
    return String ();
#endif
    return result;
}


/*
 ********************************************************************************
 ********************************* Time::GetTimezone ****************************
 ********************************************************************************
 */
String    Time::GetTimezone ()
{
    return GetTimezone (DateTime::Now ());
}

String    Time::GetTimezone (bool applyDST)
{
#if     qPlatform_Windows
    TIME_ZONE_INFORMATION   tzInfo;
    memset (&tzInfo, 0, sizeof (tzInfo));
    (void)::GetTimeZoneInformation (&tzInfo);
    return tzInfo.StandardName;
#elif   qPlatform_POSIX
    // @see http://pubs.opengroup.org/onlinepubs/7908799/xsh/tzset.html
    return String::FromSDKString (IsDaylightSavingsTime (DateTime::Now ()) ? tzname[1] : tzname[0]);
#else
    AssertNotImplemented ();
    return String ();
#endif
}

String    Time::GetTimezone (const DateTime& d)
{
    return GetTimezone (IsDaylightSavingsTime (d));
}





/*
 ********************************************************************************
 *********************** Time::IsDaylightSavingsTime ****************************
 ********************************************************************************
 */
bool    Time::IsDaylightSavingsTime (const DateTime& d)
{
    struct  tm  asTM    =   d.As<struct tm> ();
    asTM.tm_isdst = -1; // force calc of correct daylight savings time flag
    // THINK this is true - not totally clear - docs on mktime () don't specify unambiguously that this should work...
    // So far it seems too however, --LGP 2011-10-15
    time_t  result  =   mktime (&asTM);
    return asTM.tm_isdst >= 1;
}







/*
 ********************************************************************************
 ********************* Time::GetLocaltimeToGMTOffset ****************************
 ********************************************************************************
 */
time_t  Time::GetLocaltimeToGMTOffset (bool applyDST)
{
#if     0
    // WRONG - but COULD use this API - but not sure needed
#if     qPlatform_Windows
    TIME_ZONE_INFORMATION   tzInfo;
    memset (&tzInfo, 0, sizeof (tzInfo));
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
    struct tm tm;
    memset (&tm, 0, sizeof(tm));
    tm.tm_year = 70;
    tm.tm_mon = 0;      // Jan
    tm.tm_mday = 1;
    tm.tm_isdst = applyDST;
    time_t  result  =   mktime (&tm);
    return result;
}

time_t  Time::GetLocaltimeToGMTOffset (const DateTime& forTime)
{
    return GetLocaltimeToGMTOffset (IsDaylightSavingsTime (forTime));
}