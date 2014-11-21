/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
#include    "../Debug/Assertions.h"
#include    "DateTime.h"
#include    "../IO/FileSystem/BinaryFileInputStream.h"
#include    "../Streams/TextInputStreamBinaryAdapter.h"

#include    "Timezone.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Time;





/*
 ********************************************************************************
 ***************************** Time::GetTimezoneInfo ****************************
 ********************************************************************************
 */
TimeZoneInformationType    Time::GetTimezoneInfo ()
{
    TimeZoneInformationType result;
#if     qPlatform_Windows
    TIME_ZONE_INFORMATION   tzInfo;
    memset (&tzInfo, 0, sizeof (tzInfo));
    (void)::GetTimeZoneInformation (&tzInfo);
    result.fStandardTime.fName = tzInfo.StandardName;
    result.fDaylightSavingsTime.fName = tzInfo.DaylightName;
#elif   qPlatform_POSIX
    try {
        result.fID = Streams::TextInputStreamBinaryAdapter (IO::FileSystem::BinaryFileInputStream (String_Constant (L"/etc/timezone"))).ReadAll ().Trim ();
    }
    catch (...) {
    }
    // @see http://pubs.opengroup.org/onlinepubs/7908799/xsh/tzset.html
    result.fStandardTime.fName = String::FromSDKString (tzname[0]);
    result.fStandardTime.fName = String::FromSDKString (tzname[1]);
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