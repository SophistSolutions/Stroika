/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/Number.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Linguistics/MessageUtilities.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Time/Duration.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
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
    : inherited{"Invalid Duration Format"sv}
{
}

Duration::FormatException::FormatException (const String& details)
    : inherited{"Invalid Duration Format: "sv + details}
{
}

/*
 ********************************************************************************
 *********************************** Duration ***********************************
 ********************************************************************************
 */
namespace {
    // sigh - not sure why so hard still -- LGP 2023-01-22 ;-)
    constexpr wstring_view kMS_ = L"\u00b5s"sv; // u8"µs"sv
    static_assert (kMS_.size () == 2 && kMS_[0] == 0x00B5 and kMS_[1] == 's');
    //static_assert (sizeof (u8"µ") == 3 and u8"µ"[0] == 0xC2 and u8"µ"[1] == 0xB5); // be sure above encoding working, since appantly not well standardized (file has UTF8 BOM)
}
const Duration::PrettyPrintInfo Duration::kDefaultPrettyPrintInfo = {{
    "year"sv,    "years"sv,  "month"sv,   "months"sv, "week"sv, "weeks"sv, "day"sv, "days"sv, "hour"sv, "hours"sv, "minute"sv,
    "minutes"sv, "second"sv, "seconds"sv, "ms"sv,     "ms"sv,   kMS_,      kMS_,    "ns"sv,   "ns"sv,   "ps"sv,    "ps"sv,
}};

const Duration::AgePrettyPrintInfo Duration::kDefaultAgePrettyPrintInfo = {
    {
        "now"sv,
        "ago"sv,
        "from now"sv,
    },
    12 * 60 /*fNowThreshold*/
};

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

namespace {
    // ISO 8601's digits: '0' to '9'. Not std::isdigit: undefined for a negative char, and a std::string's bytes reach the
    // parser unchecked for ASCII. Not Character::IsDigit: Character (char) throws on such a byte
    constexpr bool IsDigit_ (char c)
    {
        return '0' <= c and c <= '9';
    }
    // the end of a number's text: digits and decimal points, then any exponent - 'e' or 'E', a sign, digits (no designator
    // is an E) - so an error names the whole number. What it holds is checked when it is read
    string_view::const_iterator FindNumberEnd_ (string_view::const_iterator i, string_view::const_iterator end)
    {
        while (i != end and (IsDigit_ (*i) or *i == '.' or *i == ',')) {
            ++i;
        }
        if (i != end and (*i == 'e' or *i == 'E')) {
            ++i;
            if (i != end and (*i == '+' or *i == '-')) {
                ++i;
            }
            while (i != end and IsDigit_ (*i)) {
                ++i;
            }
        }
        return i;
    }
    // ISO 8601's number: digits, then optionally a decimal point ('.' or ',') and digits
    bool IsISONumber_ (string_view n)
    {
#if qCompilerAndStdLib_stdlib_ranges_pretty_broken_Buggy
        auto i = find_if_not (n.begin (), n.end (), IsDigit_);
#else
        auto i = ranges::find_if_not (n, IsDigit_);
#endif
        if (i == n.begin ()) {
            return false;
        }
        if (i == n.end ()) {
            return true;
        }
        return (*i == '.' or *i == ',') and i + 1 != n.end () and all_of (i + 1, n.end (), IsDigit_);
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
    auto                lingMgr = Linguistics::MessageUtilities::Manager::sThe.LookupHandler ();
    static const String kCommaSpace_{", "sv};
    /*
     *  From http://physics.nist.gov/cuu/Units/checklist.html
     *      There is a space between the numerical value and unit symbol, even when the value is used
     *      in an adjectival sense, except in the case of superscript units for plane angle.
     */
    static const String kSpaceBeforeUnit_{" "sv};

    InternalNumericFormatType_ t        = As<InternalNumericFormatType_> ();
    bool                       isNeg    = (t < 0);
    InternalNumericFormatType_ timeLeft = t < 0 ? -t : t;
    StringBuilder              result;
    if (timeLeft >= kSecondsPerYear_) {
        unsigned int nYears = static_cast<unsigned int> (timeLeft / kSecondsPerYear_);
        if (nYears != 0) {
            if (not result.empty ()) {
                result << kCommaSpace_;
            }
            result << nYears << kSpaceBeforeUnit_
                   << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fYear, prettyPrintInfo.fLabels.fYears, static_cast<int> (nYears));
            timeLeft -= nYears * kSecondsPerYear_;
        }
    }
    if (timeLeft >= kSecondsPerMonth_) {
        unsigned int nMonths = static_cast<unsigned int> (timeLeft / kSecondsPerMonth_);
        if (nMonths != 0) {
            if (not result.empty ()) {
                result << kCommaSpace_;
            }
            result << "{}"_f(nMonths) << kSpaceBeforeUnit_
                   << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fMonth, prettyPrintInfo.fLabels.fMonths, static_cast<int> (nMonths));
            timeLeft -= nMonths * kSecondsPerMonth_;
        }
    }
    if (timeLeft >= kSecondsPerDay_) {
        unsigned int nDays = static_cast<unsigned int> (timeLeft / kSecondsPerDay_);
        if (nDays != 0) {
            if (not result.empty ()) {
                result << kCommaSpace_;
            }
            result << "{}"_f(nDays) << kSpaceBeforeUnit_
                   << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fDay, prettyPrintInfo.fLabels.fDays, static_cast<int> (nDays));
            timeLeft -= nDays * kSecondsPerDay_;
        }
    }
    if (timeLeft != 0) {
        if (timeLeft >= kSecondsPerHour_) {
            unsigned int nHours = static_cast<unsigned int> (timeLeft / kSecondsPerHour_);
            if (nHours != 0) {
                if (not result.empty ()) {
                    result << kCommaSpace_;
                }
                result << "{}"_f(nHours) << kSpaceBeforeUnit_
                       << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fHour, prettyPrintInfo.fLabels.fHours, static_cast<int> (nHours));
                timeLeft -= nHours * kSecondsPerHour_;
            }
        }
        if (timeLeft >= kSecondsPerMinute_) {
            unsigned int nMinutes = static_cast<unsigned int> (timeLeft / kSecondsPerMinute_);
            if (nMinutes != 0) {
                if (not result.empty ()) {
                    result << kCommaSpace_;
                }
                result << "{}"_f(nMinutes) << kSpaceBeforeUnit_
                       << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fMinute, prettyPrintInfo.fLabels.fMinutes, static_cast<int> (nMinutes));
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
                    result << "{}"_f(static_cast<int> (timeLeft)) << kSpaceBeforeUnit_
                           << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fSecond, prettyPrintInfo.fLabels.fSeconds, timeLeftAsInt);
                    timeLeft -= static_cast<int> (timeLeft);
                }
                else {
                    result << "{:.3f}"_f(timeLeft) << kSpaceBeforeUnit_ << prettyPrintInfo.fLabels.fSeconds;
                    timeLeft = 0.0;
                }
            }
        }
        if (timeLeft > 0) {
            // DO nano, micro, milliseconds here

            static constexpr Number::FloatToStringOptions kFinalFloatOptions_{};

            static constexpr bool kFirstSubSecondUnitDoDecimalPlaceImmediately_{true};

            uint16_t nMilliseconds = static_cast<uint16_t> (floor (timeLeft * 1.0e3));
            Assert (0 <= nMilliseconds and nMilliseconds < 1000);
            // intentionally show something like 1003 us as 1003 us, not 1.003 ms
            if (nMilliseconds > 2 or Math::NearlyEquals (timeLeft, .001) or Math::NearlyEquals (timeLeft, .002)) {
                if (not result.empty ()) {
                    result << kCommaSpace_;
                }
                if (kFirstSubSecondUnitDoDecimalPlaceImmediately_) {
                    result << Characters::ToString (timeLeft * 1.0e3, kFinalFloatOptions_) << kSpaceBeforeUnit_
                           << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fMilliSecond, prettyPrintInfo.fLabels.fMilliSeconds, nMilliseconds);
                    timeLeft = 0;
                }
                else {
                    result << int (nMilliseconds) << kSpaceBeforeUnit_
                           << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fMilliSecond, prettyPrintInfo.fLabels.fMilliSeconds, nMilliseconds);
                    timeLeft -= 1.0e-3 * nMilliseconds;
                }
            }
            uint16_t nMicroSeconds = static_cast<uint16_t> (floor (timeLeft * 1.0e6));
            if (nMicroSeconds > 0) {
                if (not result.empty ()) {
                    result << kCommaSpace_;
                }
                if (kFirstSubSecondUnitDoDecimalPlaceImmediately_) {
                    result << Characters::ToString (timeLeft * 1.0e6, kFinalFloatOptions_) << kSpaceBeforeUnit_
                           << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fMicroSecond, prettyPrintInfo.fLabels.fMicroSeconds, nMicroSeconds);
                    timeLeft = 0;
                }
                else {
                    result << int (nMicroSeconds) << kSpaceBeforeUnit_
                           << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fMicroSecond, prettyPrintInfo.fLabels.fMicroSeconds, nMicroSeconds);
                    timeLeft -= 1.0e-6 * nMicroSeconds;
                }
            }
            uint16_t nNanoSeconds = static_cast<uint16_t> (floor (timeLeft * 1.0e9));
            if (nNanoSeconds > 0) {
                if (not result.empty ()) {
                    result << kCommaSpace_;
                }
                if (kFirstSubSecondUnitDoDecimalPlaceImmediately_) {
                    result << Characters::ToString (timeLeft * 1.0e9, kFinalFloatOptions_) << kSpaceBeforeUnit_
                           << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fNanoSecond, prettyPrintInfo.fLabels.fNanoSeconds, nNanoSeconds);
                    timeLeft = 0;
                }
                else {
                    result << int (nMicroSeconds) << kSpaceBeforeUnit_
                           << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fNanoSecond, prettyPrintInfo.fLabels.fNanoSecond, nNanoSeconds);
                    timeLeft -= 1.0e-9 * nNanoSeconds;
                }
            }
            Duration::InternalNumericFormatType_ nPicoSeconds = timeLeft * 1.0e12;
            if (nPicoSeconds > 1.0e-5) {
                if (not result.empty ()) {
                    result << kCommaSpace_;
                }
                Duration::InternalNumericFormatType_ extraBits = nPicoSeconds - floor (nPicoSeconds);
                if (extraBits > 1.0e-2) {
                    result << nPicoSeconds << kSpaceBeforeUnit_
                           << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fPicoSecond, prettyPrintInfo.fLabels.fPicoSeconds, 2);
                }
                else {
                    result << int (nPicoSeconds) << kSpaceBeforeUnit_
                           << lingMgr->PluralizeNoun (prettyPrintInfo.fLabels.fPicoSecond, prettyPrintInfo.fLabels.fPicoSeconds, int (nPicoSeconds));
                }
            }
        }
    }
    if (result.empty ()) {
        result << "0"sv << kSpaceBeforeUnit_ << prettyPrintInfo.fLabels.fSeconds;
    }
    if (isNeg) {
        static const String kNeg_{"-"sv};
        result = kNeg_ + result;
    }
    return result;
}

String Duration::PrettyPrintAge (const AgePrettyPrintInfo& agePrettyPrintInfo, const PrettyPrintInfo& prettyPrintInfo) const
{
    InternalNumericFormatType_ t     = As<InternalNumericFormatType_> ();
    bool                       isNeg = (t < 0);
    InternalNumericFormatType_ absT  = isNeg ? -t : t;
    if (absT < agePrettyPrintInfo.fNowThreshold) {
        return agePrettyPrintInfo.fLabels.fNow;
    }

    String suffix = isNeg ? agePrettyPrintInfo.fLabels.fAgo : agePrettyPrintInfo.fLabels.fFromNow;

    auto fmtDate = [suffix] (int timeInSelectedUnit, const String& singularUnit, const String& pluralUnit) -> String {
        String label = Linguistics::MessageUtilities::Manager::sThe.PluralizeNoun (singularUnit, pluralUnit, timeInSelectedUnit);
        return "{} {} {}"_f(timeInSelectedUnit, label, suffix);
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
    switch (fRepType_) {
        case eNumeric_:
            return Duration{-count ()};
        case eString_: {
            // the text negated, so As<String> still gives back the text read (which starts with a '-' or the 'P')
            Duration result;
            static_cast<inherited&> (result) = inherited{-count ()};
            new (&result.fStringRep_) string{fStringRep_.starts_with ('-') ? fStringRep_.substr (fStringRep_.find ('P')) : "-" + fStringRep_};
            result.fRepType_ = eString_;
            return result;
        }
    }
    AssertNotReached ();
    return *this;
}

auto Duration::Parse (const String& text, const ParseOptions& options) -> Common::StdCompat::expected<Duration, FormatException>
{
    if (optional<string> ascii = text.AsASCIIQuietly ()) {
        return Parse_ (*ascii, options);
    }
    return Common::StdCompat::unexpected<FormatException>{FormatException{"\"{}\" - not ASCII"_f(text)}};
}
#if qCompilerAndStdLib_default_member_initializer_needed_within_enclosing_class_Buggy
auto Duration::Parse (const String& text) -> Common::StdCompat::expected<Duration, FormatException>
{
    return Parse (text, ParseOptions{});
}
#endif

auto Duration::Parse_ (string_view text, const ParseOptions& options) -> Common::StdCompat::expected<Duration, FormatException>
{
    auto t = ParseTime_ (text, options);
    if (not t) {
        return Common::StdCompat::unexpected<FormatException>{t.error ()};
    }
    Duration result;
    static_cast<inherited&> (result) = inherited{*t};
    new (&result.fStringRep_) string{text};
    result.fRepType_ = eString_;
    return result;
}

auto Duration::ParseTime_ (string_view s, [[maybe_unused]] const ParseOptions& options)
    -> Common::StdCompat::expected<InternalNumericFormatType_, FormatException>
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Duration::ParseTime_", "s = {}"_f, String::FromLatin1 (span<const char>{s})};
#endif
    auto error = [&] (const String& why) {
        return Common::StdCompat::unexpected<FormatException>{FormatException{"\"{}\" - {}"_f(String::FromLatin1 (span<const char>{s}), why)}};
    };
    auto i     = s.begin ();
    bool isNeg = false;
    if (i != s.end () and *i == '-') {
        isNeg = true;
        ++i;
    }
    if (i == s.end () or *i != 'P') {
        return error ("does not start with P, or -P"sv);
    }
    ++i;
    /*
     *  Each element's designator ranks: Y M W D, then after the T, H M S. Each element ranks after the one before it, on its
     *  own side of the T; weeks stand alone; and only the last element's number has a fraction.
     */
    InternalNumericFormatType_ result          = 0;
    bool                       timePart        = false;
    bool                       partEmpty       = true; // no element yet after the P, or after the T
    int                        lastRank        = -1;
    bool                       weeks           = false;
    bool                       lastHadFraction = false;
    while (i != s.end ()) {
        if (*i == 'T') {
            if (timePart) {
                return error ("a second T"sv);
            }
            timePart  = true;
            partEmpty = true;
            ++i;
            continue;
        }
        auto        numberEnd = FindNumberEnd_ (i, s.end ());
        string_view number{i, numberEnd};
        if (number.empty ()) {
            return error ("expected a number"sv);
        }
        if (numberEnd == s.end ()) {
            return error ("a number with no designator after it"sv);
        }
        if (not IsISONumber_ (number)) {
            return error ("\"{}\" is not digits, with perhaps a decimal point and digits"_f(String{number}));
        }
        /*
         *  According to http://en.wikipedia.org/wiki/ISO_8601
         *      "The smallest value used may also have a decimal fraction, as in "P0.5Y" to indicate
         *      half a year. This decimal fraction may be specified with either a comma or a full stop,
         *      as in "P0,5Y" or "P0.5Y"."
         *
         *  Read in the "C" locale, whatever the global one; one decimal point at most, so 1.2.3 is an error, not 1.2.
         */
        string n{number};
#if qCompilerAndStdLib_stdlib_ranges_pretty_broken_Buggy
        replace (n.begin (), n.end (), ',', '.');
#else
        ranges::replace (n, ',', '.');
#endif
        auto value = Number::Parse<InternalNumericFormatType_> (span<const char>{n});
        if (not value) {
            return error ("\"{}\" is not a number"_f(String{number}));
        }
        String designator = String::FromLatin1 (span<const char>{&*numberEnd, 1});
        int    rank;
        time_t unit; // seconds
        switch (*numberEnd) {
            case 'Y':
                rank = 0;
                unit = kSecondsPerYear_;
                break;
            case 'M':
                rank = timePart ? 5 : 1;
                unit = timePart ? kSecondsPerMinute_ : kSecondsPerMonth_;
                break;
            case 'W':
                rank = 2;
                unit = kSecondsPerWeek_;
                break;
            case 'D':
                rank = 3;
                unit = kSecondsPerDay_;
                break;
            case 'H':
                rank = 4;
                unit = kSecondsPerHour_;
                break;
            case 'S':
                rank = 6;
                unit = 1;
                break;
            default:
                return error ("'{}' is not a designator"_f(designator));
        }
        if ((rank >= 4) != timePart) {
            return error ("{} {} the T"_f(designator, timePart ? "after"sv : "before"sv));
        }
        if (rank <= lastRank) {
            return error ("{} out of order, or repeated"_f(designator));
        }
        if (weeks or (rank == 2 and lastRank >= 0)) {
            return error ("weeks with other elements"sv);
        }
        if (lastHadFraction) {
            return error ("a fraction before the last element"sv);
        }
        result += *value * unit;
        partEmpty       = false;
        lastRank        = rank;
        weeks           = weeks or rank == 2;
        lastHadFraction = number.find_first_of (".,") != string_view::npos;
        i               = numberEnd + 1;
    }
    if (partEmpty) {
        return error (timePart ? "nothing after the T"sv : "nothing after the P"sv);
    }
    return isNeg ? -result : result;
}

String Duration::UnParseTime_ (InternalNumericFormatType_ t, const AsStringOptions& options)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Duration::UnParseTime_", "t = {:e}, seconds-precision={}"_f, t, options.fSecondsPrecision};
#endif
    Require (not isnan (t));
    bool                       isNeg    = (t < 0);
    InternalNumericFormatType_ timeLeft = t < 0 ? -t : t;
    StringBuilder              result;
    if (isNeg) {
        result += "-"sv;
    }
    result += "P"sv;
    if (timeLeft >= kSecondsPerYear_) {
        InternalNumericFormatType_ nYears = trunc (timeLeft / kSecondsPerYear_);
        Assert (nYears > 0.0);
        if (nYears > 0.0) {
            Stroika_ATTRIBUTE_INDETERMINATE char buf[10 * 1024];
            (void)::snprintf (buf, sizeof (buf), "%.0LfY", static_cast<long double> (nYears));
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
            Stroika_ATTRIBUTE_INDETERMINATE char buf[1024];
            (void)::snprintf (buf, sizeof (buf), "%dM", nMonths);
            result += buf;
            timeLeft -= nMonths * kSecondsPerMonth_;
        }
    }
    Assert (0.0 <= timeLeft and timeLeft < kSecondsPerMonth_);
    if (timeLeft >= kSecondsPerDay_) {
        unsigned int nDays = static_cast<unsigned int> (timeLeft / kSecondsPerDay_);
        if (nDays != 0) {
            Stroika_ATTRIBUTE_INDETERMINATE char buf[1024];
            (void)::snprintf (buf, sizeof (buf), "%dD", nDays);
            result += buf;
            timeLeft -= nDays * kSecondsPerDay_;
        }
    }
    Assert (0.0 <= timeLeft and timeLeft < kSecondsPerDay_);
    if (timeLeft > 0) {
        result += "T"sv;
        if (timeLeft >= kSecondsPerHour_) {
            unsigned int nHours = static_cast<unsigned int> (timeLeft / kSecondsPerHour_);
            if (nHours != 0) {
                Stroika_ATTRIBUTE_INDETERMINATE char buf[1024];
                (void)::snprintf (buf, sizeof (buf), "%dH", nHours);
                result += buf;
                timeLeft -= nHours * kSecondsPerHour_;
            }
        }
        Assert (0.0 <= timeLeft and timeLeft < kSecondsPerHour_);
        if (timeLeft >= kSecondsPerMinute_) {
            unsigned int nMinutes = static_cast<unsigned int> (timeLeft / kSecondsPerMinute_);
            if (nMinutes != 0) {
                Stroika_ATTRIBUTE_INDETERMINATE char buf[1024];
                (void)::snprintf (buf, sizeof (buf), "%dM", nMinutes);
                result += buf;
                timeLeft -= nMinutes * kSecondsPerMinute_;
            }
        }
        Assert (0.0 <= timeLeft and timeLeft < kSecondsPerMinute_);
        if (timeLeft > 0.0) {
            result += Characters::ToString (
                timeLeft, Number::FloatToStringOptions{.fNotation = Number::Notation::ePlain, .fPrecision = options.fSecondsPrecision});
            result += "S"sv;
        }
    }
    if (result.length () == 1) {
        result += "T0S"sv;
    }
    return result;
}

/*
 ********************************************************************************
 ******************************** Math::Abs *************************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Math {
    Time::Duration Abs (Time::Duration v)
    {
        return (v.count () < 0) ? -v : v;
    }
}
