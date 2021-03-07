/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "../Characters/Format.h"
#include "../Characters/StringBuilder.h"
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

using Containers::Sequence;

using namespace Time;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if qPlatform_Windows
namespace {
    TimeOfDay mkTimeOfDay_ (const ::SYSTEMTIME& sysTime)
    {
        WORD hour   = max (sysTime.wHour, static_cast<WORD> (0));
        hour        = min (hour, static_cast<WORD> (23));
        WORD minute = max (sysTime.wMinute, static_cast<WORD> (0));
        minute      = min (minute, static_cast<WORD> (59));
        WORD secs   = max (sysTime.wSecond, static_cast<WORD> (0));
        secs        = min (secs, static_cast<WORD> (59));
        return TimeOfDay{hour, minute, secs};
    }
}
#endif

namespace {
    constexpr bool      kLocaleIndependent_iso8601_PerformanceOptimization_ = true; // TBD if this is actual performance help or not
    optional<TimeOfDay> LocaleIndependent_Parse_iso8601_ (const String& rep)
    {
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
            return TimeOfDay{static_cast<unsigned> (hour), static_cast<unsigned> (minute), static_cast<unsigned> (secs)};
        }
        DISABLE_COMPILER_MSC_WARNING_END (4996)
        return nullopt;
    }
    String LocaleIndependent_Format_iso8601_ (uint32_t timeInSeconds)
    {
        uint32_t hour    = timeInSeconds / (60 * 60);
        uint32_t minutes = (timeInSeconds - hour * 60 * 60) / 60;
        uint32_t secs    = timeInSeconds - hour * 60 * 60 - minutes * 60;
        Assert (hour >= 0 and hour < 24);
        Assert (minutes >= 0 and minutes < 60);
        Assert (secs >= 0 and secs < 60);
        return ::Format (L"%02d:%02d:%02d", hour, minutes, secs);
    }
}

/*
 ********************************************************************************
 *********************************** TimeOfDay **********************************
 ********************************************************************************
 */
TimeOfDay::FormatException::FormatException ()
    : Execution::RuntimeErrorException<>{L"Invalid Time Format"sv}
{
}

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
        ::DATE d{};
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
        ::SYSTEMTIME sysTime{};
        Verify (::VariantTimeToSystemTime (d, &sysTime));
        return mkTimeOfDay_ (sysTime);
    }
}
#endif

DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
DISABLE_COMPILER_MSC_WARNING_START (4996) // class deprecated but still need to implement it
TimeOfDay TimeOfDay::Parse (const String& rep, ParseFormat pf)
{
    switch (pf) {
        case ParseFormat::eCurrentLocale: {
            return Parse (rep, locale{});
        }
        case ParseFormat::eISO8601: {
            if (auto r = LocaleIndependent_Parse_iso8601_ (rep)) {
                return *r;
            }
        }
        default: {
            AssertNotReached ();
        }
    }
    Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty TimeOfDay{}
}
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
DISABLE_COMPILER_MSC_WARNING_END (4996) // class deprecated but still need to implement it

TimeOfDay TimeOfDay::Parse (const String& rep, const locale& l)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"TimeOfDay::Parse", L"rep='%s', l='%s'", rep.c_str (), String::FromNarrowSDKString (l.name ()).c_str ())};
#endif
    if (rep.empty ()) {
        Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty TimeOfDay{}
    }
    auto result = Parse (rep, l, kDefaultParseFormats);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"returning %s", Characters::ToString (result).c_str ());
#endif
    return result;
}

TimeOfDay TimeOfDay::Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"TimeOfDay::Parse", L"rep='%s', l='%s', formatPatterns=%s", rep.c_str (), String::FromNarrowSDKString (l.name ()).c_str (), Characters::ToString (formatPatterns).c_str ())};
#endif
    if (rep.empty ()) {
        Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty TimeOfDay{}
    }
    wstring                  wRep  = rep.As<wstring> ();
    const time_get<wchar_t>& tmget = use_facet<time_get<wchar_t>> (l);
    for (const auto& formatPattern : formatPatterns) {
        if (auto o = QuietParse_ (wRep, tmget, formatPattern)) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"returning %s", Characters::ToString (*o).c_str ());
#endif
            return *o;
        }
    }
    Execution::Throw (FormatException::kThe);
}

TimeOfDay TimeOfDay::Parse (const String& rep, const String& formatPattern)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"TimeOfDay::Parse", L"rep=%s", rep.c_str ())};
#endif
    if (rep.empty ()) {
        Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty TimeOfDay{}
    }
    if (auto o = QuietParse_ (rep.As<wstring> (), formatPattern)) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"returning %s", Characters::ToString (*o).c_str ());
#endif
        return *o;
    }
    Execution::Throw (FormatException::kThe);
}

TimeOfDay TimeOfDay::Parse (const String& rep, const locale& l, const String& formatPattern)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"TimeOfDay::Parse", L"rep='%s', l='%s'", rep.c_str (), String::FromNarrowSDKString (l.name ()).c_str ())};
#endif
    if (rep.empty ()) {
        Execution::Throw (FormatException::kThe); // NOTE - CHANGE in STROIKA v2.1d11 - this used to return empty TimeOfDay{}
    }
    if (auto o = QuietParse_ (rep.As<wstring> (), use_facet<time_get<wchar_t>> (l), formatPattern)) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"returning %s", Characters::ToString (*o).c_str ());
#endif
        return *o;
    }
    Execution::Throw (FormatException::kThe);
}

optional<TimeOfDay> TimeOfDay::QuietParse (const String& rep, const String& formatPattern)
{
    if (rep.empty ()) {
        return nullopt;
    }
    return QuietParse_ (rep.As<wstring> (), formatPattern);
}

optional<TimeOfDay> TimeOfDay::QuietParse (const String& rep, const locale& l, const String& formatPattern)
{
    if (rep.empty ()) {
        return nullopt;
    }
    return QuietParse_ (rep.As<wstring> (), use_facet<time_get<wchar_t>> (l), formatPattern);
}

optional<TimeOfDay> TimeOfDay::QuietParse_ (const wstring& rep, const String& formatPattern)
{
    if (kLocaleIndependent_iso8601_PerformanceOptimization_ and formatPattern == kISO8601Format) {
        return LocaleIndependent_Parse_iso8601_ (rep);
    }
    return QuietParse_ (rep, use_facet<time_get<wchar_t>> (locale{}), formatPattern);
}

optional<TimeOfDay> TimeOfDay::QuietParse_ (const wstring& rep, const time_get<wchar_t>& tmget, const String& formatPattern)
{
    ios::iostate                 errState = ios::goodbit;
    tm                           when{};
    wistringstream               iss{rep};
    istreambuf_iterator<wchar_t> itbegin{iss}; // beginning of iss
    istreambuf_iterator<wchar_t> itend;        // end-of-stream
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
    if ((errState & ios::badbit) or (errState & ios::failbit)) [[UNLIKELY_ATTR]] {
#if qCompilerAndStdLib_locale_get_time_needsStrptime_sometimes_Buggy
        errState = (::strptime (rep.AsNarrowSDKString ().c_str (), formatPattern.AsNarrowSDKString ().c_str (), &when) == nullptr) ? ios::failbit : ios::goodbit;
#endif
    }
    if ((errState & ios::badbit) or (errState & ios::failbit)) [[UNLIKELY_ATTR]] {
        return nullopt;
    }

    Ensure (0 <= when.tm_hour and when.tm_hour <= 23);
    Ensure (0 <= when.tm_min and when.tm_min <= 59);
    Ensure (0 <= when.tm_sec and when.tm_sec <= 59);
    auto result = TimeOfDay{static_cast<unsigned> (when.tm_hour), static_cast<unsigned> (when.tm_min), static_cast<unsigned> (when.tm_sec)};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"returning %s", Characters::ToString (result).c_str ());
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
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_MSC_WARNING_START (4996) // class deprecated but still need to implement it
        case PrintFormat::eISO8601: {
            return LocaleIndependent_Format_iso8601_ (fTime_);
        }
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_MSC_WARNING_END (4996) // class deprecated but still need to implement it
        default: {
            AssertNotReached ();
            return String{};
        }
    }
}

String TimeOfDay::Format (const locale& l) const
{
    return Format (l, kLocaleStandardFormat);
}

String TimeOfDay::Format (const String& formatPattern) const
{
    if (kLocaleIndependent_iso8601_PerformanceOptimization_ and formatPattern == kISO8601Format) {
        return LocaleIndependent_Format_iso8601_ (fTime_);
    }
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
