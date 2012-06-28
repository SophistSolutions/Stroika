/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

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
 *********************************** Duration ***********************************
 ********************************************************************************
 */
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
    : fDurationRep ()
{
}

Duration::Duration (const wstring& durationStr)
    : fDurationRep (WideStringToASCII (durationStr))
{
    (void)ParseTime_ (fDurationRep);    // call for the side-effect of throw if bad format src string
}

Duration::Duration (int32_t duration)
    : fDurationRep (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (int64_t duration)
    : fDurationRep (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (double duration)
    : fDurationRep (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

#if     qCompilerAndStdLib_Supports_stdchrono
Duration::Duration (const std::chrono::duration<double>& d)
    : fDurationRep (UnParseTime_ (static_cast<InternalNumericFormatType_> (d.count ())))
{
}
#endif

void    Duration::clear ()
{
    fDurationRep.clear ();
}

bool    Duration::empty () const
{
    return fDurationRep.empty ();
}

template    <>
time_t  Duration::As () const
{
    return static_cast<time_t> (ParseTime_ (fDurationRep));     // could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

template    <>
double  Duration::As () const
{
    return ParseTime_ (fDurationRep);                           // could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

#if     qCompilerAndStdLib_Supports_stdchrono
#if 1
namespace   Stroika {
    namespace   Foundation {
        namespace   Time {
            // GCC 4.6 requires this above extra namesapce stuff. Not sure reasonable or bug? Investigate before creating bug workaround define
            // -- LGP 2012-05-26
            template    <>
            std::chrono::duration<double>  Duration::As () const
            {
                return std::chrono::duration<double> (ParseTime_ (fDurationRep));
            }
        }
    }
}
#else
template    <>
std::chrono::duration<double>  Duration::As () const
{
    return std::chrono::duration<double> (ParseTime_ (fDurationRep));
}
#endif
#endif

template    <>
wstring Duration::As () const
{
    return ASCIIStringToWide (fDurationRep);
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
            result += Format (L"%d ", nYears) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fYear, prettyPrintInfo.fLabels.fYears, static_cast<int> (nYears));
            timeLeft -= nYears * kSecondsPerYear;
        }
    }
    if (timeLeft >= kSecondsPerMonth) {
        unsigned int    nMonths = static_cast<unsigned int> (timeLeft / kSecondsPerMonth);
        if (nMonths != 0) {
            if (not result.empty ()) {
                result += L", ";
            }
            result += Format (L"%d ", nMonths) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fMonth, prettyPrintInfo.fLabels.fMonths, static_cast<int> (nMonths));
            timeLeft -= nMonths * kSecondsPerMonth;
        }
    }
    if (timeLeft >= kSecondsPerDay) {
        unsigned int    nDays = static_cast<unsigned int> (timeLeft / kSecondsPerDay);
        if (nDays != 0) {
            if (not result.empty ()) {
                result += L", ";
            }
            result += Format (L"%d ", nDays) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fDay, prettyPrintInfo.fLabels.fDays, static_cast<int> (nDays));
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
                result += Format (L"%d ", nHours) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fHour, prettyPrintInfo.fLabels.fHours, static_cast<int> (nHours));
                timeLeft -= nHours * kSecondsPerHour;
            }
        }
        if (timeLeft >= kSecondsPerMinute) {
            unsigned int    nMinutes = static_cast<unsigned int> (timeLeft / kSecondsPerMinute);
            if (nMinutes != 0) {
                if (not result.empty ()) {
                    result += L", ";
                }
                result += Format (L"%d ", nMinutes) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fMinute, prettyPrintInfo.fLabels.fMinutes, static_cast<int> (nMinutes));
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
                    result += Format (L"%d ", static_cast<int> (timeLeft)) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fSecond, prettyPrintInfo.fLabels.fSeconds, timeLeftAsInt);
                    timeLeft -= static_cast<int> (timeLeft);
                }
                else {
                    result += Format (L"%.3f ", timeLeft) + prettyPrintInfo.fLabels.fSeconds;
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
                result += Format (L"%d ", nNanoSeconds) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fNanoSecond, prettyPrintInfo.fLabels.fNanoSeconds, nNanoSeconds);
            }
            else if (nNanoSeconds < 1000 * 1000) {
                result += Format (L"%d ", nNanoSeconds / 1000) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fMicroSecond, prettyPrintInfo.fLabels.fMicroSeconds, nNanoSeconds / 1000);
            }
            else  {
                result += Format (L"%d ", nNanoSeconds / (1000 * 1000)) + Linguistics::PluralizeNoun (prettyPrintInfo.fLabels.fMilliSecond, prettyPrintInfo.fLabels.fMilliSeconds, nNanoSeconds / (1000 * 1000));
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

string  Duration::UnParseTime_ (InternalNumericFormatType_ t)
{
    bool                        isNeg       =   (t < 0);
    InternalNumericFormatType_  timeLeft    =   t < 0 ? -t : t;
    string  result  =   "P";
    result.reserve (50);
    if (timeLeft >= kSecondsPerYear) {
        unsigned int    nYears = static_cast<unsigned int> (timeLeft / kSecondsPerYear);
        if (nYears != 0) {
            result += Format ("%dY", nYears);
            timeLeft -= nYears * kSecondsPerYear;
        }
    }
    if (timeLeft >= kSecondsPerMonth) {
        unsigned int    nMonths = static_cast<unsigned int> (timeLeft / kSecondsPerMonth);
        if (nMonths != 0) {
            result += Format ("%dM", nMonths);
            timeLeft -= nMonths * kSecondsPerMonth;
        }
    }
    if (timeLeft >= kSecondsPerDay) {
        unsigned int    nDays = static_cast<unsigned int> (timeLeft / kSecondsPerDay);
        if (nDays != 0) {
            result += Format ("%dD", nDays);
            timeLeft -= nDays * kSecondsPerDay;
        }
    }
    if (timeLeft != 0) {
        result += "T";
        if (timeLeft >= kSecondsPerHour) {
            unsigned int    nHours = static_cast<unsigned int> (timeLeft / kSecondsPerHour);
            if (nHours != 0) {
                result += Format ("%dH", nHours);
                timeLeft -= nHours * kSecondsPerHour;
            }
        }
        if (timeLeft >= kSecondsPerMinute) {
            unsigned int    nMinutes = static_cast<unsigned int> (timeLeft / kSecondsPerMinute);
            if (nMinutes != 0) {
                result += Format ("%dM", nMinutes);
                timeLeft -= nMinutes * kSecondsPerMinute;
            }
        }
        if (timeLeft != 0) {
            result += Format ("%fS", timeLeft);
        }
    }
    if (isNeg) {
        result = "-" + result;
    }
    return result;
}

