/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cmath>
#include <cstdio>

#include "../Characters/CString/Utilities.h"
#include "../Characters/FloatConversion.h"
#include "../Characters/Format.h"
#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"
#include "../Linguistics/MessageUtilities.h"
#include "../Math/Common.h"

#include "Duration.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Time;

using Debug::TraceContextBumper;

using namespace Time;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********************** Duration::FormatException *******************************
 ********************************************************************************
 */
Duration::FormatException::FormatException ()
    : inherited{L"Invalid Duration Format"sv}
{
}
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
const Duration::FormatException Duration::FormatException::kThe;
#endif

/*
 ********************************************************************************
 *********************************** Duration ***********************************
 ********************************************************************************
 */
const Duration::PrettyPrintInfo Duration::kDefaultPrettyPrintInfo = {
    {
        L"year"sv,
        L"years"sv,
        L"month"sv,
        L"months"sv,
        L"week"sv,
        L"weeks"sv,
        L"day"sv,
        L"days"sv,
        L"hour"sv,
        L"hours"sv,
        L"minute"sv,
        L"minutes"sv,
        L"second"sv,
        L"seconds"sv,
        L"ms"sv,
        L"ms"sv,
        L"µs"sv,
        L"µs"sv,
        L"ns"sv,
        L"ns"sv,
        L"ps"sv,
        L"ps"sv,
    }};

const Duration::AgePrettyPrintInfo Duration::kDefaultAgePrettyPrintInfo = {
    {
        L"now"sv,
        L"ago"sv,
        L"from now"sv,
    },
    12 * 60 /*fNowThreshold*/
};

Duration::Duration (const Characters::String& durationStr)
    : Duration (durationStr.AsASCII ()) // buggy - must throw bad format if not ascii
{
}

Duration& Duration::operator+= (const Duration& rhs)
{
    *this = *this + rhs;
    return *this;
}

Duration& Duration::operator-= (const Duration& rhs)
{
    *this = *this - rhs;
    return *this;
}

template <>
wstring Duration::As () const
{
    switch (fRepType_) {
        case eEmpty_:
            return wstring{};
        case eString_:
            return ASCIIStringToWide (fStringRep_);
        case eNumeric_:
            return ASCIIStringToWide (UnParseTime_ (count ()));
    }
    AssertNotReached ();
    return wstring{};
}

namespace {
    template <typename T>
    inline T DoPin_ (Duration::rep d, Duration::rep multiplier)
    {
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 10)) || (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 12))
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wimplicit-int-float-conversion\""); // warning: implicit conversion from 'std::__1::chrono::duration<long long, std::__1::ratio<1, 1> >::rep' (aka 'long long') to 'double' changes value from 9223372036854775807 to 9223372036854775808
#endif
        if (d > T::max ().count () / multiplier) {
            return T::max ();
        }
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 10)) || (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 12))
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wimplicit-int-float-conversion\"");
#endif
        return T (typename T::rep (d * multiplier));
    }
}

template <>
chrono::seconds Duration::AsPinned () const
{
    return DoPin_<chrono::seconds> (count (), 1.0);
}

template <>
chrono::milliseconds Duration::AsPinned () const
{
    return DoPin_<chrono::milliseconds> (count (), 1000.0);
}

template <>
chrono::microseconds Duration::AsPinned () const
{
    return DoPin_<chrono::microseconds> (count (), 1000.0 * 1000.0);
}

template <>
chrono::nanoseconds Duration::AsPinned () const
{
    return DoPin_<chrono::nanoseconds> (count (), 1000.0 * 1000.0 * 1000.0);
}

namespace {
    string::const_iterator SkipWhitespace_ (string::const_iterator i, string::const_iterator end)
    {
        // GNU LIBC code (header) says that whitespace is allowed (though I've found no external docs to support this).
        // Still - no harm in accepting this - so long as we don't ever generate it...
        while (i != end and isspace (*i)) {
            ++i;
        }
        Ensure (i <= end);
        return i;
    }
    string::const_iterator FindFirstNonDigitOrDot_ (string::const_iterator i, string::const_iterator end)
    {
        while (i != end and (isdigit (*i) or *i == '.')) {
            ++i;
        }
        Ensure (i <= end);
        return i;
    }

    constexpr time_t kSecondsPerMinute_ = 60;
    constexpr time_t kSecondsPerHour_   = kSecondsPerMinute_ * 60;
    constexpr time_t kSecondsPerDay_    = kSecondsPerHour_ * 24;
    constexpr time_t kSecondsPerWeek_   = kSecondsPerDay_ * 7;
    constexpr time_t kSecondsPerMonth_  = kSecondsPerDay_ * 30;
    constexpr time_t kSecondsPerYear_   = kSecondsPerDay_ * 365;
}

String Duration::PrettyPrint (const PrettyPrintInfo& prettyPrintInfo) const
{
    auto                lingMgr = Linguistics::MessageUtiltiesManager::Get ();
    static const String kCommaSpace_{L", "sv};
    if (empty ()) {
        return String{};
    }
    /*
     *  From http://physics.nist.gov/cuu/Units/checklist.html
     *      There is a space between the numerical value and unit symbol, even when the value is used
     *      in an adjectival sense, except in the case of superscript units for plane angle.
     */
    static const String kSpaceBeforeUnit_{L" "sv};

    InternalNumericFormatType_ t        = As<InternalNumericFormatType_> ();
    bool                       isNeg    = (t < 0);
    InternalNumericFormatType_ timeLeft = t < 0 ? -t : t;
    String                     result;
    if (timeLeft >= kSecondsPerYear_) {
        unsigned int nYears = static_cast<unsigned int> (timeLeft / kSecondsPerYear_);
        if (nYears != 0) {
            if (not result.empty ()) {
                result += kCommaSpace_;
            }
            result += Characters::Format (L"%d", nYears) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fYear, prettyPrintInfo.fLabels.fYears, static_cast<int> (nYears));
            timeLeft -= nYears * kSecondsPerYear_;
        }
    }
    if (timeLeft >= kSecondsPerMonth_) {
        unsigned int nMonths = static_cast<unsigned int> (timeLeft / kSecondsPerMonth_);
        if (nMonths != 0) {
            if (not result.empty ()) {
                result += kCommaSpace_;
            }
            result += Characters::Format (L"%d", nMonths) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fMonth, prettyPrintInfo.fLabels.fMonths, static_cast<int> (nMonths));
            timeLeft -= nMonths * kSecondsPerMonth_;
        }
    }
    if (timeLeft >= kSecondsPerDay_) {
        unsigned int nDays = static_cast<unsigned int> (timeLeft / kSecondsPerDay_);
        if (nDays != 0) {
            if (not result.empty ()) {
                result += kCommaSpace_;
            }
            result += Characters::Format (L"%d", nDays) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fDay, prettyPrintInfo.fLabels.fDays, static_cast<int> (nDays));
            timeLeft -= nDays * kSecondsPerDay_;
        }
    }
    if (timeLeft != 0) {
        if (timeLeft >= kSecondsPerHour_) {
            unsigned int nHours = static_cast<unsigned int> (timeLeft / kSecondsPerHour_);
            if (nHours != 0) {
                if (not result.empty ()) {
                    result += kCommaSpace_;
                }
                result += Characters::Format (L"%d", nHours) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fHour, prettyPrintInfo.fLabels.fHours, static_cast<int> (nHours));
                timeLeft -= nHours * kSecondsPerHour_;
            }
        }
        if (timeLeft >= kSecondsPerMinute_) {
            unsigned int nMinutes = static_cast<unsigned int> (timeLeft / kSecondsPerMinute_);
            if (nMinutes != 0) {
                if (not result.empty ()) {
                    result += kCommaSpace_;
                }
                result += Characters::Format (L"%d", nMinutes) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fMinute, prettyPrintInfo.fLabels.fMinutes, static_cast<int> (nMinutes));
                timeLeft -= nMinutes * kSecondsPerMinute_;
            }
        }
        if (timeLeft > 0) {
            int timeLeftAsInt = static_cast<int> (timeLeft);
            if (timeLeftAsInt != 0) {
                Assert (timeLeftAsInt > 0);
                if (not result.empty ()) {
                    result += kCommaSpace_;
                }
                // Map 3.242 to printing out 3.242, but 0.234 prints out as 234 milliseconds
                if (fabs (timeLeft - timeLeftAsInt) < 0.001) {
                    result += Characters::Format (L"%d", static_cast<int> (timeLeft)) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fSecond, prettyPrintInfo.fLabels.fSeconds, timeLeftAsInt);
                    timeLeft -= static_cast<int> (timeLeft);
                }
                else {
                    result += Characters::Format (L"%.3f", timeLeft) + kSpaceBeforeUnit_ + prettyPrintInfo.fLabels.fSeconds;
                    timeLeft = 0.0;
                }
            }
        }
        if (timeLeft > 0) {
            // DO nano, micro, milliseconds here

            static const FloatConversion::ToStringOptions kFinalFloatOptions_{FloatConversion::ToStringOptions::eTrimZeros};

            static constexpr bool kFirstSubSecondUnitDoDecimalPlaceImmediately_{true};

            uint16_t nMilliseconds = static_cast<uint16_t> (floor (timeLeft * 1.0e3));
            Assert (0 <= nMilliseconds and nMilliseconds < 1000);
            // intentionally show something like 1003 us as 1003 us, not 1.003 ms
            if (nMilliseconds > 2 or Math::NearlyEquals (timeLeft, .001) or Math::NearlyEquals (timeLeft, .002)) {
                if (not result.empty ()) {
                    result += kCommaSpace_;
                }
                if (kFirstSubSecondUnitDoDecimalPlaceImmediately_) {
                    result += Characters::FloatConversion::ToString (timeLeft * 1.0e3, kFinalFloatOptions_) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fMilliSecond, prettyPrintInfo.fLabels.fMilliSeconds, nMilliseconds);
                    timeLeft = 0;
                }
                else {
                    result += Characters::Format (L"%d", int (nMilliseconds)) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fMilliSecond, prettyPrintInfo.fLabels.fMilliSeconds, nMilliseconds);
                    timeLeft -= 1.0e-3 * nMilliseconds;
                }
            }
            uint16_t nMicroSeconds = static_cast<uint16_t> (floor (timeLeft * 1.0e6));
            if (nMicroSeconds > 0) {
                if (not result.empty ()) {
                    result += kCommaSpace_;
                }
                if (kFirstSubSecondUnitDoDecimalPlaceImmediately_) {
                    result += Characters::FloatConversion::ToString (timeLeft * 1.0e6, kFinalFloatOptions_) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fMicroSecond, prettyPrintInfo.fLabels.fMicroSeconds, nMicroSeconds);
                    timeLeft = 0;
                }
                else {
                    result += Characters::Format (L"%d", int (nMicroSeconds)) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fMicroSecond, prettyPrintInfo.fLabels.fMicroSeconds, nMicroSeconds);
                    timeLeft -= 1.0e-6 * nMicroSeconds;
                }
            }
            uint16_t nNanoSeconds = static_cast<uint16_t> (floor (timeLeft * 1.0e9));
            if (nNanoSeconds > 0) {
                if (not result.empty ()) {
                    result += kCommaSpace_;
                }
                if (kFirstSubSecondUnitDoDecimalPlaceImmediately_) {
                    result += Characters::FloatConversion::ToString (timeLeft * 1.0e9, kFinalFloatOptions_) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fNanoSecond, prettyPrintInfo.fLabels.fNanoSeconds, nNanoSeconds);
                    timeLeft = 0;
                }
                else {
                    result += Characters::Format (L"%d", int (nMicroSeconds)) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fNanoSecond, prettyPrintInfo.fLabels.fNanoSecond, nNanoSeconds);
                    timeLeft -= 1.0e-9 * nNanoSeconds;
                }
            }
            Time::DurationSecondsType nPicoSeconds = timeLeft * 1.0e12;
            if (nPicoSeconds > 1.0e-5) {
                if (not result.empty ()) {
                    result += kCommaSpace_;
                }
                Time::DurationSecondsType extraBits = nPicoSeconds - floor (nPicoSeconds);
                if (extraBits > 1.0e-2) {
                    result += Characters::Format (L"%f", nPicoSeconds) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fPicoSecond, prettyPrintInfo.fLabels.fPicoSeconds, 2);
                }
                else {
                    result += Characters::Format (L"%d", int (nPicoSeconds)) + kSpaceBeforeUnit_ + lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fPicoSecond, prettyPrintInfo.fLabels.fPicoSeconds, int (nPicoSeconds));
                }
            }
        }
    }
    if (result.empty ()) {
        result = L"0" + kSpaceBeforeUnit_ + prettyPrintInfo.fLabels.fSeconds;
    }
    if (isNeg) {
        static const String kNeg_{L"-"sv};
        result = kNeg_ + result;
    }
    return result;
}

Characters::String Duration::PrettyPrintAge (const AgePrettyPrintInfo& agePrettyPrintInfo, const PrettyPrintInfo& prettyPrintInfo) const
{
    InternalNumericFormatType_ t     = As<InternalNumericFormatType_> ();
    bool                       isNeg = (t < 0);
    InternalNumericFormatType_ absT  = isNeg ? -t : t;
    if (absT < agePrettyPrintInfo.fNowThreshold) {
        return agePrettyPrintInfo.fLabels.fNow;
    }

    Characters::String suffix = isNeg ? agePrettyPrintInfo.fLabels.fAgo : agePrettyPrintInfo.fLabels.fFromNow;

    auto fmtDate = [suffix] (int timeInSelectedUnit, const String& singularUnit, const String& pluralUnit) -> String {
        String label = Linguistics::MessageUtiltiesManager::Get ()->PluralizeNoun (singularUnit, pluralUnit, timeInSelectedUnit);
        return Characters::Format (L"%d %s %s", timeInSelectedUnit, label.c_str (), suffix.c_str ());
    };

    constexpr InternalNumericFormatType_ kShowAsMinutesIfLess_ = 55 * kSecondsPerMinute_;
    constexpr InternalNumericFormatType_ kShowHoursIfLess_     = 23 * kSecondsPerHour_;
    constexpr InternalNumericFormatType_ kShowDaysIfLess_      = 14 * kSecondsPerDay_;
    constexpr InternalNumericFormatType_ kShowWeeksIfLess_     = 59 * kSecondsPerDay_;
    constexpr InternalNumericFormatType_ kShowMonthsIfLess_    = 11 * kSecondsPerMonth_;

    if (absT < kShowAsMinutesIfLess_) {
        return fmtDate (Math::Round<int> (absT / kSecondsPerMinute_), prettyPrintInfo.fLabels.fMinute, prettyPrintInfo.fLabels.fMinutes);
    }
    if (absT < kShowHoursIfLess_) {
        return fmtDate (Math::Round<int> (absT / kSecondsPerHour_), prettyPrintInfo.fLabels.fHour, prettyPrintInfo.fLabels.fHours);
    }
    if (absT < kShowDaysIfLess_ and not Math::NearlyEquals (absT, static_cast<InternalNumericFormatType_> (kSecondsPerWeek_), 1.0)) {
        return fmtDate (Math::Round<int> (absT / kSecondsPerDay_), prettyPrintInfo.fLabels.fDay, prettyPrintInfo.fLabels.fDays);
    }
    if (absT < kShowWeeksIfLess_ and not Math::NearlyEquals (absT, static_cast<InternalNumericFormatType_> (kSecondsPerMonth_), 1.0)) {
        return fmtDate (Math::Round<int> (absT / kSecondsPerWeek_), prettyPrintInfo.fLabels.fWeek, prettyPrintInfo.fLabels.fWeeks);
    }
    if (absT < kShowMonthsIfLess_) {
        return fmtDate (Math::Round<int> (absT / kSecondsPerMonth_), prettyPrintInfo.fLabels.fMonth, prettyPrintInfo.fLabels.fMonths);
    }
    return fmtDate (Math::Round<int> (absT / kSecondsPerYear_), prettyPrintInfo.fLabels.fYear, prettyPrintInfo.fLabels.fYears);
}

Duration Duration::operator- () const
{
    wstring tmp = As<wstring> ();
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

Duration::InternalNumericFormatType_ Duration::ParseTime_ (const string& s)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Duration::ParseTime_");
    DbgTrace ("(s = %s)", s.c_str ());
#endif
    if (s.empty ()) {
        return kValueWhenEmptyRenderedAsNumber_;
    }
    InternalNumericFormatType_ curVal = 0;
    bool                       isNeg  = false;
    // compute and throw if bad...
    string::const_iterator i = SkipWhitespace_ (s.begin (), s.end ());
    if (*i == '-') {
        isNeg = true;
        i     = SkipWhitespace_ (i + 1, s.end ());
    }
    if (*i == 'P') [[LIKELY_ATTR]] {
        i = SkipWhitespace_ (i + 1, s.end ());
    }
    else {
        Execution::Throw (FormatException::kThe);
    }
    bool timePart = false;
    while (i != s.end ()) {
        if (*i == 'T') {
            timePart = true;
            i        = SkipWhitespace_ (i + 1, s.end ());
            continue;
        }
        string::const_iterator firstDigitI = i;
        string::const_iterator lastDigitI  = FindFirstNonDigitOrDot_ (i, s.end ());
        if (lastDigitI == s.end ()) [[UNLIKELY_ATTR]] {
            Execution::Throw (FormatException::kThe);
        }
        if (firstDigitI == lastDigitI) [[UNLIKELY_ATTR]] {
            Execution::Throw (FormatException::kThe);
        }
        /*
         *  According to http://en.wikipedia.org/wiki/ISO_8601
         *      "The smallest value used may also have a decimal fraction, as in "P0.5Y" to indicate
         *      half a year. This decimal fraction may be specified with either a comma or a full stop,
         *      as in "P0,5Y" or "P0.5Y"."
         *
         *  @todo   See todo in header: the first/last digit range could use '.' or ',' and I'm not sure atof is as flexible
         *  test/verify!!!
         */
        InternalNumericFormatType_ n = atof (string (firstDigitI, lastDigitI).c_str ());
        switch (*lastDigitI) {
            case 'Y':
                curVal += n * kSecondsPerYear_;
                break;
            case 'M':
                curVal += n * (timePart ? kSecondsPerMinute_ : kSecondsPerMonth_);
                break;
            case 'W':
                curVal += n * kSecondsPerWeek_;
                break;
            case 'D':
                curVal += n * kSecondsPerDay_;
                break;
            case 'H':
                curVal += n * kSecondsPerHour_;
                break;
            case 'S':
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
    void TrimTrailingZerosInPlace_ (char* sWithMaybeTrailingZeros)
    {
        RequireNotNull (sWithMaybeTrailingZeros);
        char* pDot = sWithMaybeTrailingZeros;
        for (; *pDot != '.' and *pDot != '\0'; ++pDot)
            ;
        Assert (*pDot == '\0' or *pDot == '.');
        if (*pDot != '\0') {
            char* pPastDot      = pDot + 1;
            char* pPastLastZero = pPastDot + ::strlen (pPastDot);
            Assert (*pPastLastZero == '\0');
            for (; (pPastLastZero - 1) > pPastDot; --pPastLastZero) {
                Assert (sWithMaybeTrailingZeros + 1 <= pPastLastZero); // so ptr ref always valid
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
#if qDebug
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
    } s_Tester_;
#endif
}

DISABLE_COMPILER_MSC_WARNING_START (6262) // stack usage OK
string Duration::UnParseTime_ (InternalNumericFormatType_ t)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"Duration::UnParseTime_", L"t = %e", t);
#endif
    bool                       isNeg    = (t < 0);
    InternalNumericFormatType_ timeLeft = t < 0 ? -t : t;
    string                     result;
    result.reserve (50);
    if (isNeg) {
        result += "-";
    }
    result += "P";
    if (timeLeft >= kSecondsPerYear_) {
        InternalNumericFormatType_ nYears = trunc (timeLeft / kSecondsPerYear_);
        Assert (nYears > 0.0);
        if (nYears > 0.0) {
            char buf[10 * 1024];
            (void)snprintf (buf, sizeof (buf), "%.0LfY", static_cast<long double> (nYears));
            result += buf;
            timeLeft -= nYears * kSecondsPerYear_;
            if (isinf (timeLeft) or timeLeft < 0) {
                // some date numbers are so large, we cannot compute a number of days, weeks etc
                // Also, for reasons which elude me (e.g. 32 bit gcc builds) this can go negative.
                // Not strictly a bug (I don't think). Just roundoff.
                timeLeft = 0.0;
            }
        }
    }
    Assert (0.0 <= timeLeft and timeLeft < kSecondsPerYear_);
    if (timeLeft >= kSecondsPerMonth_) {
        unsigned int nMonths = static_cast<unsigned int> (timeLeft / kSecondsPerMonth_);
        if (nMonths != 0) {
            char buf[1024];
            (void)snprintf (buf, sizeof (buf), "%dM", nMonths);
            result += buf;
            timeLeft -= nMonths * kSecondsPerMonth_;
        }
    }
    Assert (0.0 <= timeLeft and timeLeft < kSecondsPerMonth_);
    if (timeLeft >= kSecondsPerDay_) {
        unsigned int nDays = static_cast<unsigned int> (timeLeft / kSecondsPerDay_);
        if (nDays != 0) {
            char buf[1024];
            (void)snprintf (buf, sizeof (buf), "%dD", nDays);
            result += buf;
            timeLeft -= nDays * kSecondsPerDay_;
        }
    }
    Assert (0.0 <= timeLeft and timeLeft < kSecondsPerDay_);
    if (timeLeft > 0) {
        result += "T";
        if (timeLeft >= kSecondsPerHour_) {
            unsigned int nHours = static_cast<unsigned int> (timeLeft / kSecondsPerHour_);
            if (nHours != 0) {
                char buf[1024];
                (void)snprintf (buf, sizeof (buf), "%dH", nHours);
                result += buf;
                timeLeft -= nHours * kSecondsPerHour_;
            }
        }
        Assert (0.0 <= timeLeft and timeLeft < kSecondsPerHour_);
        if (timeLeft >= kSecondsPerMinute_) {
            unsigned int nMinutes = static_cast<unsigned int> (timeLeft / kSecondsPerMinute_);
            if (nMinutes != 0) {
                char buf[1024];
                (void)snprintf (buf, sizeof (buf), "%dM", nMinutes);
                result += buf;
                timeLeft -= nMinutes * kSecondsPerMinute_;
            }
        }
        Assert (0.0 <= timeLeft and timeLeft < kSecondsPerMinute_);
        if (timeLeft > 0.0) {
            char buf[10 * 1024];
            buf[0] = '\0';
            // We used to use 1000, but that failed silently on AIX 7.1/pcc. And its a waste anyhow.
            // I'm pretty sure we never need more than 20 or so digits here. And it wastes time.
            // (100 works on AIX 7.1/gcc 4.9.2).
            //
            // Pick a slightly more aggressive number for now, to avoid the bugs/performance cost,
            // and eventually totally rewrite how we handle this.
            Verify (snprintf (buf, sizeof (buf), "%.50f", static_cast<double> (timeLeft)) >= 52);
            TrimTrailingZerosInPlace_ (buf);
            result += buf;
            result += "S";
        }
    }
    if (result.length () == 1) {
        result += "T0S";
    }
    return result;
}
DISABLE_COMPILER_MSC_WARNING_END (6262)

/*
 ********************************************************************************
 ******************************** Math::Abs *************************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Math {
    template <>
    Time::Duration Abs (Time::Duration v)
    {
        return (v.As<DurationSecondsType> () < 0) ? -v : v;
    }
}

#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
/*
 ********************************************************************************
 ************************** Traversal::RangeTraits::Default *********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Traversal::RangeTraits {
    const Time::Duration Default<Time::Duration>::kLowerBound = Time::Duration::min ();
    const Time::Duration Default<Time::Duration>::kUpperBound = Time::Duration::max ();
}
#endif
