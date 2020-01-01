/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "../Characters/Format.h"
#include "../Characters/StringBuilder.h"
#include "../Characters/String_Constant.h"
#if qPlatform_Windows
#include "../Characters/Platform/Windows/SmartBSTR.h"
#endif
#include "../Characters/ToString.h"
#include "../Containers/Sequence.h"
#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"
#include "../Execution/Throw.h"
#if qPlatform_Windows
#include "../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include "../Linguistics/Words.h"
#include "../Memory/SmallStackBuffer.h"

#include "TimeOfDay.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Time;

using Characters::String_Constant;
using Containers::Sequence;

using namespace Time;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace {
    constexpr bool kRequireImbueToUseFacet_ = false; // example uses it, and code inside windows tmget seems to reference it, but no logic for this, and no clear docs (and works same either way apparently)
}

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
}
#endif

/*
 ********************************************************************************
 *********************************** TimeOfDay **********************************
 ********************************************************************************
 */
TimeOfDay::FormatException::FormatException ()
    : Execution::RuntimeErrorException<> (L"Invalid Time Format"sv)
{
}
const TimeOfDay::FormatException TimeOfDay::FormatException::kThe;

/*
 ********************************************************************************
 *********************************** TimeOfDay **********************************
 ********************************************************************************
 */
#if qPlatform_Windows
namespace {
    TimeOfDay Parse_ (const String& rep, LCID lcid)
    {
        using namespace Execution::Platform::Windows;
        if (rep.empty ()) {
            Execution::Throw (TimeOfDay::FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty TimeOfDay{}
                                                                 //        return TimeOfDay ();
        }
        DATE d{};
        try {
            ThrowIfErrorHRESULT (::VarDateFromStr (Characters::Platform::Windows::SmartBSTR (rep.c_str ()), lcid, VAR_TIMEVALUEONLY, &d));
        }
        catch (...) {
            // Apparently military time (e.g. 1300 hours - where colon missing) - is rejected as mal-formed.
            // Detect that - and try to interpret it appropriately.
            String newRep = rep;
            if (newRep.length () == 4 and
                newRep[0].IsDigit () and newRep[1].IsDigit () and newRep[2].IsDigit () and newRep[3].IsDigit ()) {
                newRep = newRep.substr (0, 2) + L":" + newRep.substr (2, 2);
                ThrowIfErrorHRESULT (::VarDateFromStr (Characters::Platform::Windows::SmartBSTR (newRep.c_str ()), lcid, VAR_TIMEVALUEONLY, &d));
            }
            else {
                Execution::Throw (TimeOfDay::FormatException::kThe);
            }
        }
        // SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
        SYSTEMTIME sysTime{};
        Verify (::VariantTimeToSystemTime (d, &sysTime));
        return mkTimeOfDay_ (sysTime);
    }
}
#endif

const String TimeOfDay::kLocaleStandardFormat          = String_Constant{kLocaleStandardFormatArray};
const String TimeOfDay::kLocaleStandardAlternateFormat = String_Constant{kLocaleStandardAlternateFormatArray};
const String TimeOfDay::kISO8601Format                 = String_Constant{kISO8601FormatArray}; // equivilent to String_Constant{L"%H:%M:%S"}

//%t        Any white space.
//%T        The time as %H : %M : %S. (iso8601 format)
//%r        is the time as %I:%M:%S %p
//%M        The minute [00,59]; leading zeros are permitted but not required.
//%p        Either 'AM' or 'PM' according to the given time value, or the corresponding strings for the current locale. Noon is treated as 'pm' and midnight as 'am'.
//%P        Like %p but in lowercase: 'am' or 'pm' or a corresponding string for the current locale. (GNU)
//%S        The seconds [00,60]; leading zeros are permitted but not required.
const Traversal::Iterable<String> TimeOfDay::kDefaultParseFormats{
    kLocaleStandardFormat,
    kLocaleStandardAlternateFormat,
    kISO8601Format,
    L"%r"sv,
    L"%H:%M"sv,
    L"%I%p"sv,
    L"%I%P"sv,
    L"%I%t%p"sv,
    L"%I%t%P"sv,
    L"%I:%M%t%p"sv,
    L"%I:%M%t%P"sv,
    L"%I:%M:%S%t%p"sv,
    L"%I:%M:%S%t%P"sv,
    L"%I:%M:%S"sv,
    L"%I:%M"sv,
};

TimeOfDay TimeOfDay::Parse (const String& rep, ParseFormat pf)
{
    switch (pf) {
        case ParseFormat::eCurrentLocale: {
            return Parse (rep, locale{});
        }
        case ParseFormat::eISO8601: {
            int hour   = 0;
            int minute = 0;
            int secs   = 0;
            DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
            if (::swscanf (rep.c_str (), L"%d:%d:%d", &hour, &minute, &secs) >= 2) {
                hour   = std::max (hour, 0);
                hour   = std::min (hour, 23);
                minute = std::max (minute, 0);
                minute = std::min (minute, 59);
                secs   = std::max (secs, 0);
                secs   = std::min (secs, 59);
                return TimeOfDay ((hour * 60 + minute) * 60 + secs);
            }
            DISABLE_COMPILER_MSC_WARNING_END (4996)
        }
        default: {
            AssertNotReached ();
        }
    }
    Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty TimeOfDay{}
}

TimeOfDay TimeOfDay::Parse (const String& rep, const locale& l)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"TimeOfDay::Parse", L"rep=%s, l=%s", rep.c_str (), String::FromNarrowSDKString (l.name ()).c_str ())};
#endif
    if (rep.empty ()) {
        Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty TimeOfDay{}
    }

    auto result = Parse (rep, l, kDefaultParseFormats);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"returning '%s'", Characters::ToString (result).c_str ());
#endif
    return result;
}

TimeOfDay TimeOfDay::Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"TimeOfDay::Parse", L"rep=%s, l=%s, formatPatterns=%s", rep.c_str (), String::FromNarrowSDKString (l.name ()).c_str (), Characters::ToString (formatPatterns).c_str ())};
#endif
    if (rep.empty ()) {
        Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty TimeOfDay{}
    }
    wstring wRep = rep.As<wstring> ();

    const time_get<wchar_t>& tmget    = use_facet<time_get<wchar_t>> (l);
    ios::iostate             errState = ios::goodbit;
    tm                       when{};

    for (auto&& formatPattern : formatPatterns) {
        errState = ios::goodbit;
        wistringstream iss (wRep);
        if constexpr (kRequireImbueToUseFacet_) {
            iss.imbue (l);
        }
        istreambuf_iterator<wchar_t> itbegin (iss); // beginning of iss
        istreambuf_iterator<wchar_t> itend;         // end-of-stream

#if qCompilerAndStdLib_std_get_time_pctx_Buggy
        if (formatPattern == L"%X") {
            tmget.get_time (itbegin, itend, iss, errState, &when);
        }
        else {
            // Best I can see to do to workaround this bug
            DISABLE_COMPILER_MSC_WARNING_START (4996);
            return Parse_ (rep, LOCALE_USER_DEFAULT);
            DISABLE_COMPILER_MSC_WARNING_END (4996);
        }
#else
        (void)tmget.get (itbegin, itend, iss, errState, &when, formatPattern.c_str (), formatPattern.c_str () + formatPattern.length ());
#endif
        if ((errState & ios::badbit) or (errState & ios::failbit))
            [[UNLIKELY_ATTR]]
            {
#if qCompilerAndStdLib_locale_get_time_needsStrptime_sometimes_Buggy
                {
                    errState = (::strptime (rep.AsNarrowSDKString ().c_str (), formatPattern.AsNarrowSDKString ().c_str (), &when) == nullptr) ? ios::failbit : ios::goodbit;
                    if (errState == ios::goodbit) {
                        break;
                    }
                }
#endif
                continue;
            }
        else {
            break;
        }
    }
    // clang-format off
    if ((errState & ios::badbit) or (errState & ios::failbit)) [[UNLIKELY_ATTR]] {
        Execution::Throw (FormatException::kThe);
    }
    // clang-format on

    Assert (0 <= when.tm_hour and when.tm_hour <= 23);
    Assert (0 <= when.tm_min and when.tm_min <= 59);
    Assert (0 <= when.tm_sec and when.tm_sec <= 59);
    auto result = TimeOfDay (when.tm_hour * 60 * 60 + when.tm_min * 60 + when.tm_sec);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"returning '%s'", Characters::ToString (result).c_str ());
#endif
    return result;
}

String TimeOfDay::Format (PrintFormat pf) const
{
    switch (pf) {
        case PrintFormat::eCurrentLocale: {
            return Format (locale{});
        }
        case PrintFormat::eCurrentLocale_WithZerosStripped: {
            String tmp = Format (locale{});
            /*
             * This logic probably needs to be locale-specific, but this is good enuf for now...
             */
            optional<size_t> i;
            while ((i = tmp.RFind (L":00"))) {
                // if its a TRAILING :00 - lose it...
                bool trailing = false;
                if (*i + 3 == tmp.size ()) {
                    trailing = true;
                }
                else if (*i + 3 < tmp.size () and tmp[*i + 3] == ' ') {
                    trailing = true;
                }
                if (trailing) {
                    tmp = tmp.SubString (0, *i) + tmp.SubString (*i + 3);
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
        case PrintFormat::eISO8601: {
            uint32_t hour    = fTime_ / (60 * 60);
            uint32_t minutes = (fTime_ - hour * 60 * 60) / 60;
            uint32_t secs    = fTime_ - hour * 60 * 60 - minutes * 60;
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
    return Format (l, String_Constant{L"%X"}); // %X locale dependent
}

String TimeOfDay::Format (const String& formatPattern) const
{
    return Format (locale{}, formatPattern);
}

String TimeOfDay::Format (const locale& l, const String& formatPattern) const
{
    // http://new.cplusplus.com/reference/std/locale/time_put/put/
    // http://en.cppreference.com/w/cpp/locale/time_put/put
    tm when{};
    when.tm_hour                   = GetHours ();
    when.tm_min                    = GetMinutes ();
    when.tm_sec                    = GetSeconds ();
    const time_put<wchar_t>& tmput = use_facet<time_put<wchar_t>> (l);
    wostringstream           oss;
    //oss.imbue (l);        // not sure if/why needed/not/needed
    tmput.put (oss, oss, ' ', &when, formatPattern.c_str (), formatPattern.c_str () + formatPattern.length ());
    return oss.str ();
}

void TimeOfDay::ClearSecondsField ()
{
    Assert (fTime_ < kMaxSecondsPerDay);
    int hour    = fTime_ / (60 * 60);
    int minutes = (fTime_ - hour * 60 * 60) / 60;
    int secs    = fTime_ - hour * 60 * 60 - minutes * 60;
    Assert (hour >= 0 and hour < 24);
    Assert (minutes >= 0 and minutes < 60);
    Assert (secs >= 0 and secs < 60);
    fTime_ -= secs;
    Assert (fTime_ < kMaxSecondsPerDay);
}
