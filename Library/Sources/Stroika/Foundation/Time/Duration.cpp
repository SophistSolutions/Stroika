/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cmath>

#include    "../Characters/CString/Utilities.h"
#include    "../Characters/Format.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"
#include    "../Linguistics/Words.h"
#include    "../Math/Common.h"

#include    "Duration.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Time;

using   Debug::TraceContextBumper;

using   namespace   Time;









/*
 ********************************************************************************
 ********************** Duration::FormatException *******************************
 ********************************************************************************
 */
Duration::FormatException::FormatException ()
    : StringException (L"Invalid Duration Format")
{
}













/*
 ********************************************************************************
 ********************* Private_::Duration_ModuleData_ ***************************
 ********************************************************************************
 */

Time::Private_::Duration_ModuleData_::Duration_ModuleData_ ()
    : fMin (numeric_limits<Duration::InternalNumericFormatType_>::lowest ())
    , fMax (numeric_limits<Duration::InternalNumericFormatType_>::max ())
{
}



/*
 ********************************************************************************
 *********************************** Duration ***********************************
 ********************************************************************************
 */
const   Duration&    Duration::kMin = Execution::ModuleInitializer<Time::Private_::Duration_ModuleData_>::Actual ().fMin;
const   Duration&    Duration::kMax = Execution::ModuleInitializer<Time::Private_::Duration_ModuleData_>::Actual ().fMax;

const   Duration::PrettyPrintInfo   Duration::kDefaultPrettyPrintInfo = {
    {
        L"year", L"years",
        L"month", L"months",
        L"day", L"days",
        L"hour", L"hours",
        L"minute", L"minutes",
        L"second", L"seconds",
        L"ms", L"ms",
        L"µs", L"µs",
        L"ns", L"ns"
    }
};

Duration::Duration ()
    : fDurationRep_ ()
{
}

Duration::Duration (const String& durationStr)
    : fDurationRep_ (durationStr.AsASCII ())
{
    (void)ParseTime_ (fDurationRep_);    // call for the side-effect of throw if bad format src string
}

Duration::Duration (int duration)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (long duration)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (long long duration)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (float duration)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (double duration)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (long double duration)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (const std::chrono::duration<double>& d)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (d.count ())))
{
}

Duration::Duration (const std::chrono::milliseconds& d)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (d.count ()) / 1000.0))
{
}

Duration::Duration (const std::chrono::microseconds& d)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (d.count ()) / (1000.0 * 1000.0)))
{
}

Duration::Duration (const std::chrono::nanoseconds& d)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (d.count ()) / (1000.0 * 1000.0 * 1000.0)))
{
}

void    Duration::clear ()
{
    fDurationRep_.clear ();
}

bool    Duration::empty () const
{
    return fDurationRep_.empty ();
}

template    <>
time_t  Duration::As () const
{
    return static_cast<time_t> (ParseTime_ (fDurationRep_));     // could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

template    <>
float  Duration::As () const
{
    return static_cast<float> (ParseTime_ (fDurationRep_));     // could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

template    <>
double  Duration::As () const
{
    return ParseTime_ (fDurationRep_);                           // could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

template    <>
long double  Duration::As () const
{
    return ParseTime_ (fDurationRep_);                           // could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

#if     !qCompilerAndStdLib_SupportsTemplateSpecializationInAnyNS
namespace   Stroika {
    namespace   Foundation {
        namespace   Time {
#endif

            template    <>
            std::chrono::duration<double>  Duration::As () const
            {
                return std::chrono::duration<double> (ParseTime_ (fDurationRep_));
            }
            template    <>
            std::chrono::milliseconds  Duration::As () const
            {
                return std::chrono::milliseconds (static_cast<std::chrono::milliseconds::rep> (ParseTime_ (fDurationRep_) * 1000));
            }
            template    <>
            std::chrono::microseconds  Duration::As () const
            {
                return std::chrono::microseconds (static_cast<std::chrono::microseconds::rep> (ParseTime_ (fDurationRep_) * 1000 * 1000));
            }
            template    <>
            std::chrono::nanoseconds  Duration::As () const
            {
                return std::chrono::nanoseconds (static_cast<std::chrono::nanoseconds::rep> (ParseTime_ (fDurationRep_) * 1000.0 * 1000.0 * 1000.0));
            }
            template    <>
            String  Duration::As () const
            {
                return ASCIIStringToWide (fDurationRep_);
            }

#if     !qCompilerAndStdLib_SupportsTemplateSpecializationInAnyNS
        }
    }
}
#endif

template    <>
wstring Duration::As () const
{
    return ASCIIStringToWide (fDurationRep_);
}

namespace   {
    string::const_iterator  SkipWhitespace_ (string::const_iterator i, string::const_iterator end)
    {
        // GNU LIBC code (header) says that whitespace is allowed (though I've found no external docs to support this).
        // Still - no harm in accepting this - so long as we don't ever generate it...
        while (i != end and isspace (*i)) {
            ++i;
        }
        return i;
    }
    string::const_iterator  FindFirstNonDigitOrDot_ (string::const_iterator i, string::const_iterator end)
    {
        while (i != end and (isdigit (*i) or * i == '.')) {
            ++i;
        }
        return i;
    }

    const   time_t  kSecondsPerMinute   =   60;
    const   time_t  kSecondsPerHour     =   kSecondsPerMinute * 60;
    const   time_t  kSecondsPerDay      =   kSecondsPerHour * 24;
    const   time_t  kSecondsPerWeek     =   kSecondsPerDay * 7;
    const   time_t  kSecondsPerMonth    =   kSecondsPerDay * 30;
    const   time_t  kSecondsPerYear     =   kSecondsPerDay * 365;
}

wstring Duration::PrettyPrint (const PrettyPrintInfo& prettyPrintInfo) const
{
    /*
     *  TODO:
     *          o   Fix feeble attempt at rounding. We more or less round correctly for seconds, but not other units.
     */
    InternalNumericFormatType_  t           =   As<InternalNumericFormatType_> ();
    bool    isNeg       =   (t < 0);
    InternalNumericFormatType_  timeLeft    =   t < 0 ? -t : t;
    wstring result;
    result.reserve (50);
    if (timeLeft >= kSecondsPerYear) {
        unsigned int    nYears = static_cast<unsigned int> (timeLeft / kSecondsPerYear);
        if (nYears != 0) {
            if (not result.empty ()) {
                result += L", ";
            }
            result += CString::Format (L"%d ", nYears) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fYear, prettyPrintInfo.fLabels.fYears, static_cast<int> (nYears));
            timeLeft -= nYears * kSecondsPerYear;
        }
    }
    if (timeLeft >= kSecondsPerMonth) {
        unsigned int    nMonths = static_cast<unsigned int> (timeLeft / kSecondsPerMonth);
        if (nMonths != 0) {
            if (not result.empty ()) {
                result += L", ";
            }
            result += CString::Format (L"%d ", nMonths) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fMonth, prettyPrintInfo.fLabels.fMonths, static_cast<int> (nMonths));
            timeLeft -= nMonths * kSecondsPerMonth;
        }
    }
    if (timeLeft >= kSecondsPerDay) {
        unsigned int    nDays = static_cast<unsigned int> (timeLeft / kSecondsPerDay);
        if (nDays != 0) {
            if (not result.empty ()) {
                result += L", ";
            }
            result += CString::Format (L"%d ", nDays) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fDay, prettyPrintInfo.fLabels.fDays, static_cast<int> (nDays));
            timeLeft -= nDays * kSecondsPerDay;
        }
    }
    if (timeLeft != 0) {
        if (timeLeft >= kSecondsPerHour) {
            unsigned int    nHours = static_cast<unsigned int> (timeLeft / kSecondsPerHour);
            if (nHours != 0) {
                if (not result.empty ()) {
                    result += L", ";
                }
                result += CString::Format (L"%d ", nHours) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fHour, prettyPrintInfo.fLabels.fHours, static_cast<int> (nHours));
                timeLeft -= nHours * kSecondsPerHour;
            }
        }
        if (timeLeft >= kSecondsPerMinute) {
            unsigned int    nMinutes = static_cast<unsigned int> (timeLeft / kSecondsPerMinute);
            if (nMinutes != 0) {
                if (not result.empty ()) {
                    result += L", ";
                }
                result += CString::Format (L"%d ", nMinutes) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fMinute, prettyPrintInfo.fLabels.fMinutes, static_cast<int> (nMinutes));
                timeLeft -= nMinutes * kSecondsPerMinute;
            }
        }
        if (timeLeft > 0) {
            int timeLeftAsInt   =   static_cast<int> (timeLeft);
            if (timeLeftAsInt != 0) {
                Assert (timeLeftAsInt > 0);
                if (not result.empty ()) {
                    result += L", ";
                }
                // Map 3.242 to printing out 3.242, but 0.234 prints out as 234 milliseconds
                if (fabs (timeLeft - timeLeftAsInt) < 0.001) {
                    result += CString::Format (L"%d ", static_cast<int> (timeLeft)) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fSecond, prettyPrintInfo.fLabels.fSeconds, timeLeftAsInt);
                    timeLeft -= static_cast<int> (timeLeft);
                }
                else {
                    result += CString::Format (L"%.3f ", timeLeft) + prettyPrintInfo.fLabels.fSeconds;
                    timeLeft = 0.0;
                }
            }
        }
        if (timeLeft > 0) {
            // DO nano, micro, milliseconds here
            uint32_t    nNanoSeconds    =   uint32_t (timeLeft * 1000 * 1000 * 1000 + 0.5); // round
            if (not result.empty ()) {
                result += L", ";
            }
            if (nNanoSeconds < 1000) {
                result += CString::Format (L"%d ", nNanoSeconds) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fNanoSecond, prettyPrintInfo.fLabels.fNanoSeconds, nNanoSeconds);
            }
            else if (nNanoSeconds < 1000 * 1000) {
                result += CString::Format (L"%d ", nNanoSeconds / 1000) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fMicroSecond, prettyPrintInfo.fLabels.fMicroSeconds, nNanoSeconds / 1000);
            }
            else  {
                result += CString::Format (L"%d ", nNanoSeconds / (1000 * 1000)) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fMilliSecond, prettyPrintInfo.fLabels.fMilliSeconds, nNanoSeconds / (1000 * 1000));
            }
        }
    }
    if (isNeg) {
        result = L"-" + result;
    }
    return result;
}

Duration    Duration::operator- () const
{
    wstring tmp =   As<wstring> ();
    if (tmp.empty ()) {
        return *this;
    }
    if (tmp[0] == '-') {
        return Duration (tmp.substr (1));
    }
    else {
        return Duration (L"-" + tmp);
    }
}

int Duration::Compare (const Duration& rhs) const
{
    Duration::InternalNumericFormatType_    n   =   As<Duration::InternalNumericFormatType_> () - rhs.As<Duration::InternalNumericFormatType_> ();
    if (n < 0) {
        return -1;
    }
    if (n > 0) {
        return 1;
    }
    return 0;
}

Duration::InternalNumericFormatType_    Duration::ParseTime_ (const string& s)
{
    //Debug::TraceContextBumper   ctx (SDKSTR ("Duration::ParseTime_"));
    //DbgTrace ("(s = %s)", s.c_str ());
    if (s.empty ()) {
        return 0;
    }
    InternalNumericFormatType_  curVal  =   0;
    bool    isNeg   =   false;
    // compute and throw if bad...
    string::const_iterator  i   =   SkipWhitespace_ (s.begin (), s.end ());
    if  (*i == '-') {
        isNeg = true;
        i = SkipWhitespace_ (i + 1, s.end ());
    }
    if (*i == 'P') {
        i = SkipWhitespace_ (i + 1, s.end ());
    }
    else {
        Execution::DoThrow (FormatException ());
    }
    bool    timePart    =   false;
    while (i != s.end ()) {
        if (*i == 'T') {
            timePart = true;
            i = SkipWhitespace_ (i + 1, s.end ());
            continue;
        }
        string::const_iterator  firstDigitI =   i;
        string::const_iterator  lastDigitI  =   FindFirstNonDigitOrDot_ (i, s.end ());
        if (lastDigitI == s.end ()) {
            Execution::DoThrow (FormatException ());
        }
        if (firstDigitI == lastDigitI) {
            Execution::DoThrow (FormatException ());
        }
        /*
         *  According to http://en.wikipedia.org/wiki/ISO_8601
         *      "The smallest value used may also have a decimal fraction, as in "P0.5Y" to indicate
         *      half a year. This decimal fraction may be specified with either a comma or a full stop,
         *      as in "P0,5Y" or "P0.5Y"."
         *
         *  @todo   See todo in header: the first/lasrt digit ragne could use '.' or ',' and I'm not sure atof is as flexible
         *  test/verify!!!
         */
        InternalNumericFormatType_  n   =   atof (string (firstDigitI, lastDigitI).c_str ());
        switch (*lastDigitI) {
            case    'Y':
                curVal += n * kSecondsPerYear;
                break;
            case    'M':
                curVal += n * (timePart ? kSecondsPerMinute : kSecondsPerMonth);
                break;
            case    'W':
                curVal += n * kSecondsPerWeek;
                break;
            case    'D':
                curVal += n * kSecondsPerDay;
                break;
            case    'H':
                curVal += n * kSecondsPerHour;
                break;
            case    'S':
                curVal += n;
                break;
        }
        i = SkipWhitespace_ (lastDigitI + 1, s.end ());
    }
    return isNeg ? -curVal : curVal;
}

namespace {
    // take 3.1340000 and return 3.13
    // take 300 and return 300
    // take 300.0 and return 300
    //
    void    TrimTrailingZerosInPlace_ (char* sWithMaybeTrailingZeros)
    {
        RequireNotNull (sWithMaybeTrailingZeros);
        char*   pDot = sWithMaybeTrailingZeros;
        for (; *pDot != '.' and * pDot != '\0'; ++pDot)
            ;
        Assert (*pDot == '\0' or * pDot == '.');
        if (*pDot != '\0') {
            char*   pPastDot = pDot + 1;
            char*   pPastLastZero = pPastDot + strlen (pPastDot);
            Assert (*pPastLastZero == '\0');
            for (; (pPastLastZero - 1) > pPastDot; --pPastLastZero) {
                Assert (sWithMaybeTrailingZeros + 1 <= pPastLastZero);  // so ptr ref always valid
                if (*(pPastLastZero - 1) == '0') {
                    *(pPastLastZero - 1) = '\0';
                }
                else {
                    break;
                }
            }
            if (strcmp (pDot, ".0") == 0) {
                *pDot = '\0';
            }
        }
    }
#if     qDebug
    struct Tester_ {
        Tester_ ()
        {
            {
                char buf[1024] = "3.1340000";
                TrimTrailingZerosInPlace_ (buf);
                Assert (string (buf) == "3.134");
            }
            {
                char buf[1024] = "300";
                TrimTrailingZerosInPlace_ (buf);
                Assert (string (buf) == "300");
            }
            {
                char buf[1024] = "300.0";
                TrimTrailingZerosInPlace_ (buf);
                Assert (string (buf) == "300");
            }
        }
    }   s_Tester_;
#endif
}

#if     qCompilerAndStdLib_GCC_48_OptimizerBug
// This code fails with -O2 or greater! Tried to see which particular optimization failed but not obvious...
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif
string  Duration::UnParseTime_ (InternalNumericFormatType_ t)
{
    //Debug::TraceContextBumper   ctx (SDKSTR ("Duration::UnParseTime_"));
    //DbgTrace ("(t = %f)", t);
    bool                        isNeg       =   (t < 0);
    InternalNumericFormatType_  timeLeft    =   t < 0 ? -t : t;
    string  result;
    result.reserve (50);
    if (isNeg) {
        result += "-";
    }
    result += "P";
    if (timeLeft >= kSecondsPerYear) {
        InternalNumericFormatType_    nYears = trunc (timeLeft / kSecondsPerYear);
        Assert (nYears > 0.0);
        if (nYears > 0.0) {
            char buf[10 * 1024];
            (void)snprintf (buf, sizeof (buf), "%.0LfY", static_cast<long double> (nYears));
            result += buf;
            timeLeft -= nYears * kSecondsPerYear;
            if (std::isinf (timeLeft)) {
                // some date numbers are so large, we cannot compute a number of days, weeks etc
                timeLeft = 0.0;
            }
        }
    }
    Assert (timeLeft < kSecondsPerYear);
    if (timeLeft >= kSecondsPerMonth) {
        unsigned int    nMonths = static_cast<unsigned int> (timeLeft / kSecondsPerMonth);
        if (nMonths != 0) {
            char buf[1024];
            (void)snprintf (buf, sizeof (buf), "%dM", nMonths);
            result += buf;
            timeLeft -= nMonths * kSecondsPerMonth;
        }
    }
    if (timeLeft >= kSecondsPerDay) {
        unsigned int    nDays = static_cast<unsigned int> (timeLeft / kSecondsPerDay);
        if (nDays != 0) {
            char buf[1024];
            (void)snprintf (buf, sizeof (buf), "%dD", nDays);
            result += buf;
            timeLeft -= nDays * kSecondsPerDay;
        }
    }
    Assert (timeLeft >= 0.0);
    if (timeLeft > 0) {
        result += "T";
        if (timeLeft >= kSecondsPerHour) {
            unsigned int    nHours = static_cast<unsigned int> (timeLeft / kSecondsPerHour);
            if (nHours != 0) {
                char buf[1024];
                (void)snprintf (buf, sizeof (buf), "%dH", nHours);
                result += buf;
                timeLeft -= nHours * kSecondsPerHour;
            }
        }
        if (timeLeft >= kSecondsPerMinute) {
            unsigned int    nMinutes = static_cast<unsigned int> (timeLeft / kSecondsPerMinute);
            if (nMinutes != 0) {
                char buf[1024];
                (void)snprintf (buf, sizeof (buf), "%dM", nMinutes);
                result += buf;
                timeLeft -= nMinutes * kSecondsPerMinute;
            }
        }
        Assert (timeLeft >= 0.0);
        if (timeLeft > 0.0) {
            char buf[10 * 1024];
            buf[0] = '\0';
            (void)snprintf (buf, sizeof (buf), "%.1000f", static_cast<double> (timeLeft));
            TrimTrailingZerosInPlace_ (buf);
            result += buf;
            result += "S";
        }
    }
    return result;
}
#if     qCompilerAndStdLib_GCC_48_OptimizerBug
#pragma GCC pop_options
#endif
