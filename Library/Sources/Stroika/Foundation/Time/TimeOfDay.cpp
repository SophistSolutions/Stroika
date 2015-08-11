/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <ctime>
#include    <sstream>

#include    "../Characters/Format.h"
#include    "../Characters/String_Constant.h"
#include    "../Characters/StringBuilder.h"
#if     qPlatform_Windows
#include    "../Characters/Platform/Windows/SmartBSTR.h"
#endif
#include    "../Debug/Assertions.h"
#include    "../Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include    "../Memory/SmallStackBuffer.h"
#include    "../Linguistics/Words.h"
#include    "DateTime.h"

#include    "TimeOfDay.h"




using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Time;

using   Characters::String_Constant;
using   Debug::TraceContextBumper;

using   namespace   Time;



#if     qPlatform_Windows
namespace   {
    String GetLocaleInfo_ (LCID Locale, LCTYPE LCType)
    {
        int sizeNeeded  =   ::GetLocaleInfoW (Locale, LCType, nullptr, 0);
        SmallStackBuffer<wchar_t> buf (sizeNeeded + 1);
        Verify (::GetLocaleInfoW (Locale, LCType, buf, sizeNeeded + 1));
        return String (buf);
    }
}
#endif


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
}
#endif

#if     qPlatform_Windows
namespace   {
    String  GenTimeStr4TOD_ (LCID lcid, unsigned int hour, unsigned int minutes, unsigned int seconds)
    {
// Consider rewriting using Win32 GetTimeFormat () - and just futzing with teh format string for the case
// of zero minutes/seconds?? That might be more robust in case of changes in adding special tokens for some cultures/etc?
//
// Anyhow - I think what i have no works OK...
//      -- LGP 2009-06-18
        Require (hour < 24);
        Require (minutes < 60);
        Require (seconds < 60);

        /*
         * From Windows Vista Regional Settings control panel:
         *
         *      h = hour, m = minute, s = second
         *      tt = A.M. or P.M.
         *
         *      h = 12 hour
         *      H = 24 hour
         *
         *      hh,mm,ss = display leading zero
         *      h,m,s = do not display leading zero.
         */

        // we could keep recomputing this, but why pay the runtime cost? Restart app to get new locale info
#if     qPlatform_Windows
        static  const   wstring kFormatStr  =   GetLocaleInfo_ (lcid, LOCALE_STIMEFORMAT).As<wstring> ();
#else
#endif

        // This is what I couldn't get the MSFT default locale display stuff todo
        // We want to show 3pm, not 3:00:00pm.
        bool    skipSeconds =   seconds == 0;
        bool    skipMinutes =   skipSeconds and minutes == 0 and kFormatStr.find (L"tt") != wstring::npos;  // only do if showing AM/PM, else show 1300 for time - not 13...

        StringBuilder outBuf;
        for (auto i = kFormatStr.begin (); i != kFormatStr.end (); ++i) {
            switch (*i) {
                case 'h': {
                        bool    leadingZero =   false;
                        if (i + 1 != kFormatStr.end () and * (i + 1) == 'h') {
                            ++i;
                            leadingZero = true;
                        }
                        unsigned int    useHour =   hour;
                        if (useHour > 12) {
                            useHour -= 12;
                        }
                        if (useHour == 0) {
                            useHour = 12;       // always print '12' as the hour for '0' - 12 o'clock is really in many ways 0 o'clock
                        }
                        outBuf += Format (leadingZero ? L"%02d" : L"%d", useHour);
                    }
                    break;
                case 'H': {
                        bool    leadingZero =   false;
                        if (i + 1 != kFormatStr.end () and * (i + 1) == 'H') {
                            ++i;
                            leadingZero = true;
                        }
                        outBuf += Format (leadingZero ? L"%02d" : L"%d", hour);
                    }
                    break;
                case 'm': {
                        bool    leadingZero =   false;
                        if (i + 1 != kFormatStr.end () and * (i + 1) == 'm') {
                            ++i;
                            leadingZero = true;
                        }
                        // This is what I couldn't get the MSFT default locale display stuff todo
                        // We want to show 3pm, not 3:00:00pm.
                        if (not skipMinutes) {
                            outBuf += Format (leadingZero ? L"%02d" : L"%d", minutes);
                        }
                    }
                    break;
                case 's': {
                        bool    leadingZero =   false;
                        if (i + 1 != kFormatStr.end () and * (i + 1) == 's') {
                            ++i;
                            leadingZero = true;
                        }
                        if (not skipSeconds) {
                            outBuf += Format (leadingZero ? L"%02d" : L"%d", seconds);
                        }
                    }
                    break;
                case 't': {
                        if (i + 1 != kFormatStr.end () and * (i + 1) == 't') {
                            /*
                             *  From the WinSDK docs...
                             *
                             *  LOCALE_S1159
                             *      String for the AM designator. The maximum number of characters allowed for this string is nine.
                             *  LOCALE_S2359
                             *      String for the PM designator. The maximum number of characters allowed for this string is nine.
                             */
                            static  const   String kAMSTR  =   GetLocaleInfo_ (LOCALE_USER_DEFAULT, LOCALE_S1159);
                            static  const   String kPMSTR  =   GetLocaleInfo_ (LOCALE_USER_DEFAULT, LOCALE_S2359);
                            ++i;
                            int pmFlag =    hour >= 12;
                            //outBuf += (pmFlag?L"PM":L"AM");
                            outBuf += (pmFlag ? kPMSTR : kAMSTR);
                        }
                        else {
                            outBuf.push_back (*i);
                        }
                    }
                    break;
                case ':': {
                        // Skip ':' separator if we are going to omit the following
                        // stuff...
                        if (i + 1 != kFormatStr.end () and * (i + 1) == 's' and skipSeconds) {
                            // SKIP
                        }
                        else if (i + 1 != kFormatStr.end () and * (i + 1) == 'm' and skipMinutes) {
                            // SKIP
                        }
                        else {
                            outBuf.push_back (*i);
                        }
                    }
                    break;
                default: {
                        outBuf.push_back (*i);
                    }
                    break;
            }
        }
        return outBuf.str ();
    }
}
#endif







/*
 ********************************************************************************
 *********************************** TimeOfDay **********************************
 ********************************************************************************
 */
TimeOfDay::FormatException::FormatException ()
    : StringException (String_Constant (L"Invalid Time Format"))
{
}






/*
 ********************************************************************************
 *********************************** TimeOfDay **********************************
 ********************************************************************************
 */
#if     qCompilerAndStdLib_constexpr_Buggy || qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy
const   TimeOfDay   TimeOfDay::kMin =   TimeOfDay (0);
const   TimeOfDay   TimeOfDay::kMax =   TimeOfDay (kMaxSecondsPerDay - 1);
#endif

TimeOfDay   TimeOfDay::Parse (const String& rep, ParseFormat pf)
{
    if (rep.empty ()) {
        return TimeOfDay ();
    }
    switch (pf) {
        case    ParseFormat::eCurrentLocale:  {
                return Parse (rep, locale ());
            }
        case    ParseFormat::eISO8601:
        case    ParseFormat::eXML: {
                int hour    =   0;
                int minute  =   0;
                int secs    =   0;
                DISABLE_COMPILER_MSC_WARNING_START(4996)// MSVC SILLY WARNING ABOUT USING swscanf_s
                if (::swscanf (rep.c_str (), L"%d:%d:%d", &hour, &minute, &secs) >= 2) {
                    hour = max (hour, 0);
                    hour = min (hour, 23);
                    minute = max (minute, 0);
                    minute = min (minute, 59);
                    secs = max (secs, 0);
                    secs = min (secs, 59);
                    return TimeOfDay ((hour * 60 + minute) * 60 + secs);
                }
                DISABLE_COMPILER_MSC_WARNING_END(4996)
                return TimeOfDay ();
            }
        default: {
                AssertNotReached ();
                return TimeOfDay ();
            }
    }
}

/*
 * As of vs2k13 - qxxx_supports_locale_time_get_facet_get_time compiles and does something, but produces the wrong
 * answers quite freqntly (e.g. TimeOfDay::Parse (L"12:00:02 AM", Configuration::FindNamedLocale (L"en", L"us"))
 */
#define qxxx_supports_locale_time_get_facet_get_time    1

/*
* As of vs2k13 - qxxx_supports_locale_time_get_facet_plain_get_PCT_X compiles and does something, but produces the wrong
* answers quite freqntly (e.g. TimeOfDay::Parse (L"12:00:02 AM", Configuration::FindNamedLocale (L"en", L"us"))
*/
#define qxxx_supports_locale_time_get_facet_plain_get_PCT_X 0

TimeOfDay   TimeOfDay::Parse (const String& rep, const locale& l)
{
    if (rep.empty ()) {
        return TimeOfDay ();
    }

    ios::iostate state = ios::goodbit;
    tm  when;
    memset (&when, 0, sizeof (when));

#if     qxxx_supports_locale_time_get_facet_get_time
    {
        const time_get<wchar_t>& tmget = use_facet <time_get<wchar_t>> (l);
        wistringstream iss (rep.As<wstring> ());
        //iss.imbue (l);        // not sure if/why needed/not/needed
        istreambuf_iterator<wchar_t> itbegin (iss);  // beginning of iss
        istreambuf_iterator<wchar_t> itend;          // end-of-stream
        tmget.get_time (itbegin, itend, iss, state, &when);
    }
#endif
#if     qxxx_supports_locale_time_get_facet_plain_get_PCT_X
    {
        const time_get<wchar_t>& tmget = use_facet <time_get<wchar_t>> (l);
        wistringstream iss (rep.As<wstring> ());
        //iss.imbue (l);        // not sure if/why needed/not/needed
        istreambuf_iterator<wchar_t> itbegin (iss);  // beginning of iss
        istreambuf_iterator<wchar_t> itend;          // end-of-stream
        const wchar_t kPattern[] = L"%X";
        //const wchar_t kPattern[] = L"%r";
        //const wchar_t kPattern[] = L"%H : %M : %S";
        tmget.get (itbegin, itend, iss, state, &when, std::begin (kPattern), std::begin (kPattern) + ::wcslen (kPattern));
    }
#endif

#if     qPlatform_POSIX
    //%t        Any white space.
    //%T        The time as %H : %M : %S.
    //%r        is the time as %I:%M:%S %p
    //%M        The minute [00,59]; leading zeros are permitted but not required.
    //%p        Either 'AM' or 'PM' according to the given time value, or the corresponding strings for the current locale. Noon is treated as 'pm' and midnight as 'am'.
    //%P        Like %p but in lowercase: 'am' or 'pm' or a corresponding string for the current locale. (GNU)
    //%S        The seconds [00,60]; leading zeros are permitted but not required.
    static  const   char*   kFmtStrs2Try[] = {
        "%X",
        "%EX",
        "%T",
        "%r",
        "%H:%M:%S",
        "%H:%M",
        "%I%p",
        "%I%P",
        "%I%t%p",
        "%I%t%P",
        "%I:%M%t%p",
        "%I:%M%t%P",
        "%I:%M:%S%t%p",
        "%I:%M:%S%t%P",
        "%I:%M",
        "%I:%M:%S",
    };
    if (state & ios::failbit) {
        string  tmp =   WideStringToNarrowSDKString (rep.As<wstring> ());
        for (auto i = std::begin (kFmtStrs2Try); (state & ios::failbit) and (i != std::end (kFmtStrs2Try)); ++i) {
            memset (&when, 0, sizeof (when));
            state = (strptime (tmp.c_str (), *i, &when) == nullptr) ? ios::failbit : ios::goodbit;
        }
    }
#endif

#if     qPlatform_Windows
    if (state & ios::failbit) {
        //string    ln  =   l.name();   // This doesn't seem to produce anything useful - or easily mapepd to an LCID?
        return Parse (rep, LOCALE_USER_DEFAULT);
    }
#endif

    if (state & ios::failbit) {
        Execution::DoThrow (FormatException ());
    }
    return TimeOfDay (when.tm_hour * 60 * 60 + when.tm_min * 60 + when.tm_sec);
}

#if     qPlatform_Windows
TimeOfDay   TimeOfDay::Parse (const String& rep, LCID lcid)
{
    if (rep.empty ()) {
        return TimeOfDay ();
    }
    DATE        d;
    (void)::memset (&d, 0, sizeof (d));
    try {
        ThrowIfErrorHRESULT (::VarDateFromStr (Characters::Platform::Windows::SmartBSTR (rep.c_str ()), lcid, VAR_TIMEVALUEONLY, &d));
    }
    catch (...) {
        // Apparently military time (e.g. 1300 hours - where colon missing) - is rejected as mal-formed.
        // Detect that - and try to interpret it appropriately.
        String newRep = rep;
        if (newRep.length () == 4 and
                newRep[0].IsDigit () and newRep[1].IsDigit () and newRep[2].IsDigit () and newRep[3].IsDigit ()
           ) {
            newRep = newRep.substr (0, 2) + L":" + newRep.substr (2, 2);
            ThrowIfErrorHRESULT (::VarDateFromStr (Characters::Platform::Windows::SmartBSTR (newRep.c_str ()), lcid, VAR_TIMEVALUEONLY, &d));
        }
        else {
            Execution::DoThrow (FormatException ());
        }
    }
    // SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
    SYSTEMTIME  sysTime;
    memset (&sysTime, 0, sizeof (sysTime));
    Verify (::VariantTimeToSystemTime (d, &sysTime));
    return mkTimeOfDay_ (sysTime);
}
#endif

String TimeOfDay::Format (PrintFormat pf) const
{
    if (empty ()) {
        return String ();
    }
    switch (pf) {
        case    PrintFormat::eCurrentLocale:  {
                return Format (locale ());
            }
        case    PrintFormat::eCurrentLocale_WithZerosStripped:  {
                String  tmp =    Format (locale ());
                /*
                 * This logic probably needs to be locale-specific, but this is good enuf for now...
                 *
                 *  This code also uses wstring stuff instead of String becuase my STRING API SUCKS!!!
                 *  Adjust String API so this code can be made clear!
                 *          -- LGP 2013-03-02
                 */
                size_t i;
                while ( (i = tmp.RFind (L":00")) != String::kBadIndex) {
                    // if its a TRAILING :00 - lose it...
                    bool trailing = false;
                    if (i + 3 == tmp.size ()) {
                        trailing = true;
                    }
                    else if (i + 3 < tmp.size () and tmp[i + 3] == ' ') {
                        trailing = true;
                    }
                    if (trailing) {
                        tmp = tmp.SubString (0, i) + tmp.SubString (i + 3);
                    }
                    else {
                        break;
                    }
                }
                // Next lose prefxing 0, as in 01:04
                if (not tmp.empty () and tmp[0] == '0') {
                    tmp = tmp.substr (1);
                }
                return tmp;
            }
        case    PrintFormat::eISO8601:
        case    PrintFormat::eXML: {
                uint32_t    hour = fTime_ / (60 * 60);
                uint32_t    minutes = (fTime_ - hour * 60 * 60) / 60;
                uint32_t    secs = fTime_ - hour * 60 * 60 - minutes * 60;
                Assert (hour >= 0 and hour < 24);
                Assert (minutes >= 0 and minutes < 60);
                Assert (secs >= 0 and secs < 60);
                return ::Format (L"%02d:%02d:%02d", hour, minutes, secs);
            }
        default: {
                AssertNotReached ();
                return String ();
            }
    }
}

String TimeOfDay::Format (const locale& l) const
{
    if (empty ()) {
        return String ();
    }
    // http://new.cplusplus.com/reference/std/locale/time_put/put/
    // http://en.cppreference.com/w/cpp/locale/time_put/put
#if     qCompilerAndStdLib_LocaleTM_time_put_crash_sometimes_Buggy
    const time_put<char>& tmput = use_facet <time_put<char>> (l);
    tm when;
    memset (&when, 0, sizeof (when));
    when.tm_hour = GetHours ();
    when.tm_min = GetMinutes ();
    when.tm_sec = GetSeconds ();
    ostringstream oss;
    //oss.imbue (l);        // not sure if/why needed/not/needed
    const char kPattern[] = "%X";      // %X locale dependent
    tmput.put (oss, oss, ' ', &when, std::begin (kPattern), std::begin (kPattern) + ::strlen (kPattern));
    return String::FromNarrowString (oss.str (), l);
#else
    const time_put<wchar_t>& tmput = use_facet <time_put<wchar_t>> (l);
    tm when;
    memset (&when, 0, sizeof (when));
    when.tm_hour = GetHours ();
    when.tm_min = GetMinutes ();
    when.tm_sec = GetSeconds ();
    wostringstream oss;
    //oss.imbue (l);        // not sure if/why needed/not/needed
    const wchar_t kPattern[] = L"%X";      // %X locale dependent
    tmput.put (oss, oss, ' ', &when, std::begin (kPattern), std::begin (kPattern) + ::wcslen (kPattern));
    return oss.str ();
#endif
}

#if     qPlatform_Windows
String TimeOfDay::Format (LCID lcid) const
{
    if (empty ()) {
        return String ();
    }
    else {
        uint32_t hour = fTime_ / (60 * 60);
        uint32_t minutes = (fTime_ - hour * 60 * 60) / 60;
        uint32_t secs = fTime_ - hour * 60 * 60 - minutes * 60;
        Assert (hour >= 0 and hour < 24);
        Assert (minutes >= 0 and minutes < 60);
        Assert (secs >= 0 and secs < 60);
        return GenTimeStr4TOD_ (lcid, hour, minutes, secs);
    }
}
#endif

void    TimeOfDay::ClearSecondsField ()
{
    int hour = fTime_ / (60 * 60);
    int minutes = (fTime_ - hour * 60 * 60) / 60;
    int secs = fTime_ - hour * 60 * 60 - minutes * 60;
    Assert (hour >= 0 and hour < 24);
    Assert (minutes >= 0 and minutes < 60);
    Assert (secs >= 0 and secs < 60);
    fTime_ -= secs;
}

