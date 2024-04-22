/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "Stroika/Foundation/Characters/Format.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Characters/Platform/Windows/SmartBSTR.h"
#endif
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Throw.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include "Stroika/Foundation/Linguistics/Words.h"

#include "TimeOfDay.h"

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
        if (::swscanf (rep.As<wstring> ().c_str (), L"%d:%d:%d", &hour, &minute, &secs) >= 2) {
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
        return Characters::Format ("{:02}:{:02}:{:02}"_f, hour, minutes, secs);
    }
}

/*
 ********************************************************************************
 *********************************** TimeOfDay **********************************
 ********************************************************************************
 */
TimeOfDay::FormatException::FormatException ()
    : Execution::RuntimeErrorException<>{"Invalid Time Format"sv}
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
            Execution::Throw (TimeOfDay::FormatException::kThe);
        }
        ::DATE d{};
        try {
            ThrowIfErrorHRESULT (::VarDateFromStr (Characters::Platform::Windows::SmartBSTR{rep.As<wstring> ().c_str ()}, lcid, VAR_TIMEVALUEONLY, &d));
        }
        catch (...) {
            // Apparently military time (e.g. 1300 hours - where colon missing) - is rejected as mal-formed.
            // Detect that - and try to interpret it appropriately.
            String newRep = rep;
            if (newRep.length () == 4 and newRep[0].IsDigit () and newRep[1].IsDigit () and newRep[2].IsDigit () and newRep[3].IsDigit ()) {
                newRep = newRep.substr (0, 2) + ":"sv + newRep.substr (2, 2);
                ThrowIfErrorHRESULT (::VarDateFromStr (Characters::Platform::Windows::SmartBSTR{newRep.c_str ()}, lcid, VAR_TIMEVALUEONLY, &d));
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

TimeOfDay::TimeOfDay (unsigned int hour, unsigned int minute, unsigned int seconds, DataExchange::ValidationStrategy validationStrategy)
    : TimeOfDay
{
#if qDebug
    0
#else
    hour, minute, seconds
#endif
}
{
    // Subtle - but we can let the base constructor run on the unvalidated data in NO-DEBUG mode, cuz it will just compute a bogus
    // value that will be ignored because of the below exception
    //
    // But for the qDebug case, we have to initialize with a valid value and only assign if it passes muster
    if (hour >= 24 or minute >= 60 or seconds > 60) {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            Execution::Throw (FormatException::kThe);
        }
        else {
            Require (false);
        }
    }
#if qDebug
    *this = TimeOfDay{hour, minute, seconds};
#endif
}
TimeOfDay::TimeOfDay (uint32_t t, DataExchange::ValidationStrategy validationStrategy)
    : fTime_{t}
{
    switch (validationStrategy) {
        case DataExchange::ValidationStrategy::eAssertion:
            Require (t < kMaxSecondsPerDay);
            break;
        case DataExchange::ValidationStrategy::eThrow:
            if (not(t < kMaxSecondsPerDay)) {
                Execution::Throw (FormatException::kThe);
            }
            break;
    }
    Assert (fTime_ < kMaxSecondsPerDay);
}

TimeOfDay TimeOfDay::Parse (const String& rep, const locale& l)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"TimeOfDay::Parse", L"rep='%s', l='%s'", rep.c_str (),
                                                                                 String::FromNarrowSDKString (l.name ()).c_str ())};
#endif
    if (rep.empty ()) {
        Execution::Throw (FormatException::kThe);
    }
    auto result = Parse (rep, l, kDefaultParseFormats);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("returning {}"_f, result);
#endif
    return result;
}

TimeOfDay TimeOfDay::Parse (const String& rep, const locale& l, const Traversal::Iterable<String>& formatPatterns)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("TimeOfDay::Parse", "rep='{}', l='{}', formatPatterns={}"_f,
                                                                                 rep, String::FromNarrowSDKString (l.name ()), formatPatterns)};
#endif
    if (rep.empty ()) {
        Execution::Throw (FormatException::kThe);
    }
    wstring                  wRep  = rep.As<wstring> ();
    const time_get<wchar_t>& tmget = use_facet<time_get<wchar_t>> (l);
    for (const auto& formatPattern : formatPatterns) {
        if (auto o = ParseQuietly_ (wRep, tmget, formatPattern)) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("returning {}"_f, *o);
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
        Execution::Throw (FormatException::kThe);
    }
    if (auto o = ParseQuietly_ (rep.As<wstring> (), formatPattern)) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("returning {}"_f, *o);
#endif
        return *o;
    }
    Execution::Throw (FormatException::kThe);
}

TimeOfDay TimeOfDay::Parse (const String& rep, const locale& l, const String& formatPattern)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"TimeOfDay::Parse", L"rep='%s', l='%s'", rep.c_str (),
                                                                                 String::FromNarrowSDKString (l.name ()).c_str ())};
#endif
    if (rep.empty ()) {
        Execution::Throw (FormatException::kThe);
    }
    if (auto o = ParseQuietly_ (rep.As<wstring> (), use_facet<time_get<wchar_t>> (l), formatPattern)) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("returning {}"_f, *o);
#endif
        return *o;
    }
    Execution::Throw (FormatException::kThe);
}

optional<TimeOfDay> TimeOfDay::ParseQuietly (const String& rep, const String& formatPattern)
{
    if (rep.empty ()) {
        return nullopt;
    }
    return ParseQuietly_ (rep.As<wstring> (), formatPattern);
}

optional<TimeOfDay> TimeOfDay::ParseQuietly (const String& rep, const locale& l, const String& formatPattern)
{
    if (rep.empty ()) {
        return nullopt;
    }
    return ParseQuietly_ (rep.As<wstring> (), use_facet<time_get<wchar_t>> (l), formatPattern);
}

optional<TimeOfDay> TimeOfDay::ParseQuietly_ (const wstring& rep, const String& formatPattern)
{
    if (kLocaleIndependent_iso8601_PerformanceOptimization_ and formatPattern == kISO8601Format) {
        return LocaleIndependent_Parse_iso8601_ (rep);
    }
    return ParseQuietly_ (rep, use_facet<time_get<wchar_t>> (locale{}), formatPattern);
}

optional<TimeOfDay> TimeOfDay::ParseQuietly_ (const wstring& rep, const time_get<wchar_t>& tmget, const String& formatPattern)
{
    ios::iostate                 errState = ios::goodbit;
    tm                           when{};
    wistringstream               iss{rep};
    istreambuf_iterator<wchar_t> itbegin{iss}; // beginning of iss
    istreambuf_iterator<wchar_t> itend;        // end-of-stream
#if qCompilerAndStdLib_std_get_time_pctx_Buggy
    if (formatPattern == "%X"sv) {
        tmget.get_time (itbegin, itend, iss, errState, &when);
    }
    else {
        // Best I can see to do to workaround this bug
        return Parse_ (rep, LOCALE_USER_DEFAULT);
    }
#else
    wstring wsFormatPattern = formatPattern.As<wstring> ();
    (void)tmget.get (itbegin, itend, iss, errState, &when, wsFormatPattern.c_str (), wsFormatPattern.c_str () + wsFormatPattern.length ());
#endif
    if ((errState & ios::badbit) or (errState & ios::failbit)) [[unlikely]] {
#if qCompilerAndStdLib_locale_get_time_needsStrptime_sometimes_Buggy
        errState = (::strptime (String{rep}.AsNarrowSDKString ().c_str (), formatPattern.AsNarrowSDKString ().c_str (), &when) == nullptr)
                       ? ios::failbit
                       : ios::goodbit;
#endif
    }
    if ((errState & ios::badbit) or (errState & ios::failbit)) [[unlikely]] {
        return nullopt;
    }

    Ensure (0 <= when.tm_hour and when.tm_hour <= 23);
    Ensure (0 <= when.tm_min and when.tm_min <= 59);
    Ensure (0 <= when.tm_sec and when.tm_sec <= 59);
    auto result = TimeOfDay{static_cast<unsigned> (when.tm_hour), static_cast<unsigned> (when.tm_min), static_cast<unsigned> (when.tm_sec)};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("returning {}"_f, result);
#endif
    return result;
}

String TimeOfDay::Format (NonStandardPrintFormat pf) const
{
    switch (pf) {
        case eCurrentLocale_WithZerosStripped: {
            String tmp = Format (locale{});
            /*
             * This logic probably needs to be locale-specific, but this is good enuf for now...
             */
            optional<size_t> i;
            while ((i = tmp.RFind (":00"sv))) {
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
    when.tm_hour                             = GetHours ();
    when.tm_min                              = GetMinutes ();
    when.tm_sec                              = GetSeconds ();
    const time_put<wchar_t>& tmput           = use_facet<time_put<wchar_t>> (l);
    wstring                  wsFormatPattern = formatPattern.As<wstring> ();
    wostringstream           oss;
    tmput.put (oss, oss, ' ', &when, wsFormatPattern.c_str (), wsFormatPattern.c_str () + wsFormatPattern.length ());
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

#if qCompilerAndStdLib_linkerLosesInlinesSoCannotBeSeenByDebugger_Buggy && qDebug
String TimeOfDay::ToString () const
{
    return Format ();
}
#endif
