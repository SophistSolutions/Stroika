/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <array>
#include <cctype>
#include <charconv>
#include <chrono>
#include <cmath>
#include <exception>
#include <filesystem>
#include <functional>
#include <ranges>
#include <sstream>
#include <thread>
#include <typeindex>
#include <typeinfo>
#include <variant>
#include <wchar.h>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/Number.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/Enumeration.h"
#include "Stroika/Foundation/Common/StdCompat.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Math/Statistics.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

namespace Stroika::Foundation::Time {
    class Duration;
}
namespace Stroika::Foundation::Characters {

    namespace Private_ {

        /**
         *  \brief checks if the given type has a .ToString () const method returning a string
         */
        template <typename T>
        concept has_ToStringMethod_v = requires (T t) {
            { t.ToString () } -> convertible_to<Characters::String>;
        };
        /**
         *  \brief this given type appears to be a 'pair' of some sort
         */
        template <typename T>
        concept has_pair_v = requires (T t) {
            t.first;
            t.second;
        };
        /**
         *  \brief this given type appears to be a 'KeyValuePair' of some sort
         */
        template <typename T>
        concept has_KeyValuePair_v = requires (T t) {
            t.fKey;
            t.fValue;
        };
        /**
         *  \brief this given type appears to be a 'CountedValue' of some sort
         */
        template <typename T>
        concept has_CountedValue_v = requires (T t) {
            t.fValue;
            t.fCount;
        };

        // t as printf's %d, %o or 0x%x writes it - a negative value in octal or hexadecimal as its two's complement
        template <integral T>
        inline String IntegerToString_ (T t, ios_base::fmtflags flags)
        {
            if constexpr (same_as<T, bool>) {
                return IntegerToString_ (static_cast<int> (t), flags);
            }
            else {
                char  buf[numeric_limits<T>::digits + 4]; // enough for octal, a sign or the 0x
                char* p = std::begin (buf);
                switch (flags) {
                    case ios_base::dec:
                        return String{span<const char>{std::begin (buf), to_chars (p, std::end (buf), t).ptr}};
                    case ios_base::oct:
                        return String{
                            span<const char>{std::begin (buf), to_chars (p, std::end (buf), static_cast<make_unsigned_t<T>> (t), 8).ptr}};
                    case ios_base::hex:
                        *p++ = '0';
                        *p++ = 'x';
                        return String{
                            span<const char>{std::begin (buf), to_chars (p, std::end (buf), static_cast<make_unsigned_t<T>> (t), 16).ptr}};
                    default:
                        AssertNotReached ();
                        return String{};
                }
            }
        }

    }

    /**
     *  Collect all the default ToString() implementations, templates, overloads etc, all in one namespace.
     * 
     *  Users of the Stroika library may specialize Characters::ToString(), but (???) probably should not add overloads ot the
     *  ToStringDefaults namespace --LGP 2023-11-21.
     */
    /*
     *  Writing a floating point number - ToString (f, Number::FloatToStringOptions) - as Number::Precision says: the digits come from
     *  std::to_chars (the shortest round-trip ones, or the number correctly rounded), and are laid out here, so the text is the same
     *  on every platform. snprintf stands in only where std::to_chars cannot write the number (kHasToChars_).
     */
    namespace Private_ {
#if defined(_LIBCPP_VERSION) and defined(_LIBCPP_AVAILABILITY_HAS_TO_CHARS_FLOATING_POINT)
        // Apple's libc++ has floating point std::to_chars only from a macOS 13.3 deployment target on
        inline constexpr bool kLibraryHasFloatToChars_ = _LIBCPP_AVAILABILITY_HAS_TO_CHARS_FLOATING_POINT;
#else
        inline constexpr bool kLibraryHasFloatToChars_ = true;
#endif
        // std::to_chars writes a T exactly; libc++'s writes a long double as a double (as of libc++ 22) - not exact where a long
        // double is wider
        template <floating_point T>
        inline constexpr bool kHasToChars_ = kLibraryHasFloatToChars_ and
#if defined(_LIBCPP_VERSION)
                                             (not same_as<T, long double> or numeric_limits<long double>::digits == numeric_limits<double>::digits);
#else
                                             true;
#endif

        // 10^0 ... 10^39 - past any type's digits10 (33, for a quad precision long double)
        inline constexpr auto kPowersOf10_ = [] () {
            array<long double, 40> r{};
            long double            p = 1;
            for (long double& i : r) {
                i = p;
                p *= 10;
            }
            return r;
        }();

        // A finite number's decimal digits: d1.d2...dn x 10^fExponent. fDigits has no leading zeros (zero is "0", or "0" padded with
        // zeros), but may have trailing ones.
        struct Decimal_ {
            bool                           fNegative{false};
            Memory::InlineBuffer<char, 64> fDigits;
            int                            fExponent{0};

            // the digits after the decimal point, written without an exponent
            int PlainFractionDigits () const
            {
                return max (0, static_cast<int> (fDigits.size ()) - 1 - fExponent);
            }
        };

        // the number text [-]ddd[.ddd][e[+-]ddd] - std::to_chars's, or snprintf's, whose decimal point may be the C locale's - as d
        inline void ParseDecimal_ (string_view text, Decimal_* d)
        {
            d->fDigits.resize (0);
            size_t i     = 0;
            d->fNegative = i < text.size () and text[i] == '-';
            if (d->fNegative) {
                ++i;
            }
            int  digitsBeforePoint      = 0;
            int  leadingZerosAfterPoint = 0;
            bool seenPoint              = false;
            bool leading                = true;
            for (; i < text.size () and text[i] != 'e' and text[i] != 'E'; ++i) {
                char c = text[i];
                if ('0' <= c and c <= '9') {
                    if (leading and c == '0') {
                        if (seenPoint) {
                            ++leadingZerosAfterPoint;
                        }
                        continue;
                    }
                    leading = false;
                    d->fDigits.push_back (c);
                    if (not seenPoint) {
                        ++digitsBeforePoint;
                    }
                }
                else {
                    seenPoint = true; // the decimal point, whatever its character
                }
            }
            int exponent = 0;
            if (i < text.size ()) {
                ++i; // the e
                bool negativeExponent = i < text.size () and text[i] == '-';
                if (i < text.size () and (text[i] == '-' or text[i] == '+')) {
                    ++i;
                }
                for (; i < text.size (); ++i) {
                    exponent = exponent * 10 + (text[i] - '0');
                }
                if (negativeExponent) {
                    exponent = -exponent;
                }
            }
            if (d->fDigits.empty ()) {
                d->fDigits.push_back ('0');
                d->fExponent = 0;
                return;
            }
            d->fExponent = (digitsBeforePoint > 0 ? digitsBeforePoint - 1 : -1 - leadingZerosAfterPoint) + exponent;
        }

        // snprintf's text for f, with %.*Le or %.*Lf - for where std::to_chars cannot write it
        template <floating_point T>
        string_view SNPrintF_ (Memory::InlineBuffer<char, 128>* buf, bool scientific, int precision, T f)
        {
            int n = scientific ? ::snprintf (buf->data (), buf->size (), "%.*Le", precision, static_cast<long double> (f))
                               : ::snprintf (buf->data (), buf->size (), "%.*Lf", precision, static_cast<long double> (f));
            Verify (n > 0 and static_cast<size_t> (n) < buf->size ());
            return string_view{buf->data (), static_cast<size_t> (n)};
        }
        template <floating_point T>
        T ReadBack_ (const char* text)
        {
            if constexpr (same_as<T, float>) {
                return ::strtof (text, nullptr);
            }
            else if constexpr (same_as<T, double>) {
                return ::strtod (text, nullptr);
            }
            else {
                return ::strtold (text, nullptr);
            }
        }

        // f's shortest round-trip digits
        template <floating_point T>
        void ShortestDigits_ (T f, Decimal_* d)
        {
            Memory::InlineBuffer<char, 128> buf{Memory::eUninitialized, static_cast<size_t> (numeric_limits<T>::max_digits10) + 16};
            if constexpr (kHasToChars_<T>) {
                auto r = to_chars (buf.begin (), buf.end (), f, chars_format::scientific);
                Assert (r.ec == errc{});
                ParseDecimal_ (string_view{buf.data (), r.ptr}, d);
            }
            else {
                // the fewest digits that read back as f - max_digits10 always do
                for (int p = 0;; ++p) {
                    string_view text = SNPrintF_ (&buf, true, p, f);
                    if (p + 1 >= numeric_limits<T>::max_digits10 or ReadBack_<T> (buf.data ()) == f) {
                        ParseDecimal_ (text, d);
                        return;
                    }
                }
            }
        }
        // f correctly rounded to n significant digits
        template <floating_point T>
        void SignificantDigits_ (T f, unsigned int n, Decimal_* d)
        {
            Require (n >= 1);
            Memory::InlineBuffer<char, 128> buf{Memory::eUninitialized, n + 16};
            if constexpr (kHasToChars_<T>) {
                auto r = to_chars (buf.begin (), buf.end (), f, chars_format::scientific, static_cast<int> (n - 1));
                Assert (r.ec == errc{});
                ParseDecimal_ (string_view{buf.data (), r.ptr}, d);
            }
            else {
                ParseDecimal_ (SNPrintF_ (&buf, true, static_cast<int> (n - 1), f), d);
            }
        }
        // f correctly rounded to n digits after the decimal point
        template <floating_point T>
        void FractionDigits_ (T f, unsigned int n, Decimal_* d)
        {
            // the digits before the point: at most log10 (2) of the binary exponent's
            int                             wholeDigits = f == 0 ? 1 : max (1, static_cast<int> (ilogb (f) * 0.30103) + 2);
            Memory::InlineBuffer<char, 128> buf{Memory::eUninitialized, static_cast<size_t> (wholeDigits) + n + 8};
            if constexpr (kHasToChars_<T>) {
                auto r = to_chars (buf.begin (), buf.end (), f, chars_format::fixed, static_cast<int> (n));
                Assert (r.ec == errc{});
                ParseDecimal_ (string_view{buf.data (), r.ptr}, d);
            }
            else {
                ParseDecimal_ (SNPrintF_ (&buf, false, static_cast<int> (n), f), d);
            }
        }

        // the trailing zeros past minDigits digits dropped, and zeros added up to minDigits
        inline void TrimAndPadDigits_ (Decimal_* d, unsigned int minDigits)
        {
            size_t keep = max<size_t> (minDigits, 1);
            while (d->fDigits.size () > keep and d->fDigits[d->fDigits.size () - 1] == '0') {
                d->fDigits.resize (d->fDigits.size () - 1);
            }
            while (d->fDigits.size () < minDigits) {
                d->fDigits.push_back ('0');
            }
        }
        // the trailing zeros past minFraction digits after the point dropped, and zeros added up to minFraction (written plain)
        inline void TrimAndPadFraction_ (Decimal_* d, unsigned int minFraction)
        {
            while (d->PlainFractionDigits () > static_cast<int> (minFraction) and d->fDigits[d->fDigits.size () - 1] == '0') {
                d->fDigits.resize (d->fDigits.size () - 1);
            }
            int needed = d->fExponent + 1 + static_cast<int> (minFraction);
            while (static_cast<int> (d->fDigits.size ()) < needed) {
                d->fDigits.push_back ('0');
            }
        }

        // the digits written, as Number::Precision says: the shortest round-trip digits where they fit, else f correctly rounded
        template <floating_point T>
        void Digits_ (T f, Number::Notation notation, const Number::Precision& precision, Decimal_* d)
        {
            constexpr unsigned int kTypeDigits = numeric_limits<T>::digits10;
            auto                   significant = [&] (Number::Precision::MinMax digits) {
                if (digits.fMax <= kTypeDigits) {
                    SignificantDigits_ (f, digits.fMax, d); // within the type's precision, the shortest digits - where they fit - are these
                }
                else {
                    ShortestDigits_ (f, d);
                    if (d->fDigits.size () > digits.fMax) {
                        SignificantDigits_ (f, digits.fMax, d);
                    }
                }
                TrimAndPadDigits_ (d, digits.fMin);
            };
            if (precision.IsShortestRoundTrip ()) {
                ShortestDigits_ (f, d);
            }
            else if (optional<Number::Precision::MinMax> s = precision.GetSignificantDigits ()) {
                significant (*s);
            }
            else if (optional<Number::Precision::MinMax> fraction = precision.GetFractionDigits ()) {
                if (notation == Number::Notation::eScientific) {
                    // the digits after the first
                    significant (Number::Precision::MinMax{fraction->fMin + 1, fraction->fMax + 1});
                }
                else {
                    FractionDigits_ (f, fraction->fMax, d);
                    if (d->fDigits.size () > kTypeDigits) {
                        // more digits than the type holds: the shortest, where they fit
                        Decimal_ shortest;
                        ShortestDigits_ (f, &shortest);
                        if (shortest.PlainFractionDigits () <= static_cast<int> (fraction->fMax)) {
                            *d = shortest;
                        }
                    }
                    TrimAndPadFraction_ (d, fraction->fMin);
                }
            }
        }

        // eGeneral: scientific where the exponent is below -4, or at least P - printf's %g
        inline bool Scientific_ (Number::Notation notation, const Number::Precision& precision, const Decimal_& d)
        {
            switch (notation) {
                case Number::Notation::eScientific:
                    return true;
                case Number::Notation::ePlain:
                    return false;
                case Number::Notation::eGeneral:
                    break;
            }
            if (precision.GetFractionDigits ()) {
                return false;
            }
            // std::to_chars, without a precision, writes chars_format::general as %g with 6
            int p = precision.IsShortestRoundTrip () ? 6 : static_cast<int> (precision.GetSignificantDigits ()->fMax);
            return d.fExponent < -4 or d.fExponent >= p;
        }

        // d as text
        inline void Layout_ (const Decimal_& d, bool scientific, Memory::InlineBuffer<char, 128>* out)
        {
            out->resize (0);
            if (d.fNegative) {
                out->push_back ('-');
            }
            int n = static_cast<int> (d.fDigits.size ());
            if (scientific) {
                out->push_back (d.fDigits[0]);
                if (n > 1) {
                    out->push_back ('.');
                    for (int i = 1; i < n; ++i) {
                        out->push_back (d.fDigits[i]);
                    }
                }
                out->push_back ('e');
                out->push_back (d.fExponent < 0 ? '-' : '+');
                unsigned int e = static_cast<unsigned int> (d.fExponent < 0 ? -d.fExponent : d.fExponent);
                char         exponentDigits[12];
                int          nExponentDigits = 0;
                do {
                    exponentDigits[nExponentDigits++] = static_cast<char> ('0' + e % 10);
                    e /= 10;
                } while (e != 0);
                if (nExponentDigits < 2) {
                    out->push_back ('0'); // at least two exponent digits, as printf writes
                }
                while (nExponentDigits > 0) {
                    out->push_back (exponentDigits[--nExponentDigits]);
                }
            }
            else if (d.fExponent < 0) {
                out->push_back ('0');
                out->push_back ('.');
                for (int i = 0; i < -d.fExponent - 1; ++i) {
                    out->push_back ('0');
                }
                for (int i = 0; i < n; ++i) {
                    out->push_back (d.fDigits[i]);
                }
            }
            else {
                for (int i = 0; i <= d.fExponent; ++i) {
                    out->push_back (i < n ? d.fDigits[i] : '0'); // past the digits, the zeros of the number's size
                }
                if (n > d.fExponent + 1) {
                    out->push_back ('.');
                    for (int i = d.fExponent + 1; i < n; ++i) {
                        out->push_back (d.fDigits[i]);
                    }
                }
            }
        }

        // the "C" locale's punctuation: '.', and no grouping
        inline bool IsCPunctuation_ (const Number::PunctuationSpecifier& p)
        {
            if (const Number::Punctuation* given = get_if<Number::Punctuation> (&p)) {
                return *given == Number::Punctuation{};
            }
            return get<Common::PredefinedLocale> (p) == Common::PredefinedLocale::eUseCLocale;
        }

    }

    namespace Private_ {
        // digits, with p's digit separator between their groups (p.fGroupSize digits at the right, p.fSecondaryGroupSize - or
        // fGroupSize - in each further left) - or as they are, where p gives no group size
        inline void AppendGrouped_ (StringBuilder<>& sb, string_view digits, const Number::Punctuation& p)
        {
            size_t n = digits.size ();
            if (not p.fDigitSeparator or p.fGroupSize == 0 or n <= p.fGroupSize) {
                sb << digits;
                return;
            }
            size_t secondary = p.fSecondaryGroupSize != 0 ? p.fSecondaryGroupSize : p.fGroupSize;
            size_t left      = n - p.fGroupSize; // the digits left of the rightmost group
            size_t first     = left % secondary == 0 ? secondary : left % secondary;
            sb << digits.substr (0, first);
            for (size_t i = first; i < left; i += secondary) {
                sb << *p.fDigitSeparator << digits.substr (i, secondary);
            }
            sb << *p.fDigitSeparator << digits.substr (left);
        }
        // text - a number in the "C" locale's format (ASCII: a sign, digits, '.', an exponent; or nan or inf) - punctuated as p says:
        // its whole part's digits grouped, and its '.' made p.fDecimalPoint
        inline String Punctuate_ (const String& text, const Number::Punctuation& p)
        {
            if (p == Number::Punctuation{}) {
                return text;
            }
            string t = text.AsASCII ();
            size_t b = not t.empty () and (t[0] == '-' or t[0] == '+') ? 1 : 0;
            size_t e = b;
            while (e < t.size () and isdigit (static_cast<unsigned char> (t[e]))) {
                ++e;
            }
            StringBuilder<> sb;
            sb << string_view{t}.substr (0, b);
            AppendGrouped_ (sb, string_view{t}.substr (b, e - b), p);
            for (size_t i = e; i < t.size (); ++i) {
                if (t[i] == '.') {
                    sb << p.fDecimalPoint;
                }
                else {
                    sb << t[i];
                }
            }
            return sb.str ();
        }
        template <floating_point FLOAT_TYPE>
        String FloatToString_ (FLOAT_TYPE f, const Number::FloatToStringOptions& options)
        {
            if constexpr (numeric_limits<FLOAT_TYPE>::digits > numeric_limits<double>::digits) {
                // a long double holding exactly a double is written as that double: its long double digits would only show the
                // error of widening it (0.1 as 0.10000000000000000555) - the text reads back as the double it was
                if (isfinite (f) and static_cast<FLOAT_TYPE> (static_cast<double> (f)) == f) {
                    return FloatToString_ (static_cast<double> (f), options);
                }
            }
            if (not isfinite (f)) [[unlikely]] {
                // as std::to_chars writes them - the same on every platform
                return isnan (f) ? (signbit (f) ? "-nan"sv : "nan"sv) : (f < 0 ? "-inf"sv : "inf"sv);
            }
            bool cPunctuation = IsCPunctuation_ (options.fPunctuation);
            if constexpr (kHasToChars_<FLOAT_TYPE>) {
                // Where std::to_chars's text is just what Number::Precision says, one call, its text as it is: printf's %g (the
                // default), %e and %f - each within the type's precision, where shortest round-trip digits that fit are the
                // correctly rounded ones - and the shortest chars_format::general (kRoundTrip)
                if (cPunctuation) {
                    constexpr unsigned int kTypeDigits = numeric_limits<FLOAT_TYPE>::digits10;
                    char                   buf[numeric_limits<FLOAT_TYPE>::max_digits10 + 16];
                    auto                   text = [&] (to_chars_result r) {
                        Assert (r.ec == errc{});
                        return String{span<const char>{buf, r.ptr}};
                    };
                    optional<Number::Precision::MinMax> s        = options.fPrecision.GetSignificantDigits ();
                    optional<Number::Precision::MinMax> fraction = options.fPrecision.GetFractionDigits ();
                    switch (options.fNotation) {
                        case Number::Notation::eGeneral:
                            if (options.fPrecision.IsShortestRoundTrip ()) {
                                return text (to_chars (std::begin (buf), std::end (buf), f, chars_format::general));
                            }
                            if (s and s->fMin <= 1 and s->fMax <= kTypeDigits) {
                                return text (to_chars (std::begin (buf), std::end (buf), f, chars_format::general, static_cast<int> (s->fMax)));
                            }
                            break;
                        case Number::Notation::eScientific:
                            if (s and s->fMin == s->fMax and s->fMax <= kTypeDigits) {
                                return text (to_chars (std::begin (buf), std::end (buf), f, chars_format::scientific, static_cast<int> (s->fMax - 1)));
                            }
                            if (fraction and fraction->fMin == fraction->fMax and fraction->fMax < kTypeDigits) {
                                return text (to_chars (std::begin (buf), std::end (buf), f, chars_format::scientific,
                                                       static_cast<int> (fraction->fMax)));
                            }
                            break;
                        case Number::Notation::ePlain:
                            // under 10^(kTypeDigits - n), its whole and n fraction digits are within the type's precision
                            if (fraction and fraction->fMin == fraction->fMax and fraction->fMax <= kTypeDigits and
                                abs (f) < kPowersOf10_[kTypeDigits - fraction->fMax]) {
                                return text (to_chars (std::begin (buf), std::end (buf), f, chars_format::fixed, static_cast<int> (fraction->fMax)));
                            }
                            break;
                    }
                }
            }
            Decimal_ d;
            Digits_ (f, options.fNotation, options.fPrecision, &d);
            Memory::InlineBuffer<char, 128> text;
            Layout_ (d, Scientific_ (options.fNotation, options.fPrecision, d), &text);
            String result{span<const char>{text.data (), text.size ()}};
            return cPunctuation ? result : Punctuate_ (result, options.GetPunctuation ());
        }
    }

    namespace ToStringDefaults {

        // IN CPP FILE
        String ToString (const exception_ptr& t);
        String ToString (const exception& t);
        String ToString (const type_info& t);
        String ToString (const type_index& t);
        String ToString (const thread::id& t);

        template <Private_::has_ToStringMethod_v T>
        inline String ToString (const T& t)
        {
            return t.ToString ();
        }
        template <ranges::range T>
        String ToString (const T& t)
            requires (not Private_::has_ToStringMethod_v<T> and not is_convertible_v<T, String>)
        {
            StringBuilder sb;
            sb << "["sv;
            bool didFirst{false};
            for (const auto& i : t) {
                if (didFirst) {
                    sb << ", "_k;
                }
                else {
                    sb << " "_k;
                }
                sb << Characters::ToString (i);
                didFirst = true;
            }
            if (didFirst) {
                sb << " "_k;
            }
            sb << "]"_k;
            return sb;
        }
        template <typename T>
        inline String ToString (const T& t)
            requires (is_convertible_v<T, String>)
        {
            return static_cast<String> (t);
        }
        template <typename T>
        inline String ToString ([[maybe_unused]] const T& t)
            requires (is_convertible_v<T, tuple<>>)
        {
            return "{}"sv;
        }
        template <IToString T1>
        inline String ToString (const atomic<T1>& t)
        {
            return Characters::ToString (t.load ());
        }
        template <typename T1>
        String ToString (const tuple<T1>& t)
        {
            StringBuilder sb;
            sb << "{"sv << Characters::ToString (get<0> (t)) << "}"sv;
            return sb;
        }
        template <typename T1, typename T2>
        String ToString (const tuple<T1, T2>& t)
        {
            StringBuilder sb;
            sb << "{"sv << Characters::ToString (get<0> (t)) << ", "sv << Characters::ToString (get<1> (t)) << "}"sv;
            return sb;
        }
        template <typename T1, typename T2, typename T3>
        String ToString (const tuple<T1, T2, T3>& t)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << Characters::ToString (get<0> (t)) << ", "sv << Characters::ToString (get<1> (t)) << ", "sv << Characters::ToString (get<2> (t));
            sb << "}"sv;
            return sb;
        }
        template <typename T1, typename T2, typename T3, typename T4>
        String ToString (const tuple<T1, T2, T3, T4>& t)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << Characters::ToString (get<0> (t)) << ", "sv << Characters::ToString (get<1> (t)) << ", "sv
               << Characters::ToString (get<2> (t)) << ", "sv << Characters::ToString (get<3> (t));
            sb << "}"sv;
            return sb;
        }
        template <typename... TYPES>
        String ToString (const variant<TYPES...>& v)
        {
            // Characters::ToString, qualified: an unqualified ToString here would see only the overloads declared above this
            // one - so an alternative whose overload comes later (the integers) would convert to bool, and print "true"
            return visit ([] (const auto& a) { return Characters::ToString (a); }, v);
        }
        template <typename T>
        String ToString (const T& t)
            requires (Private_::has_pair_v<T>)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << Characters::ToString (t.first) << ": "sv << Characters::ToString (t.second);
            sb << "}"sv;
            return sb;
        }
        template <Private_::has_KeyValuePair_v T>
        String ToString (const T& t)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << Characters::ToString (t.fKey) << ": "sv << Characters::ToString (t.fValue);
            sb << "}"sv;
            return sb;
        }
        template <Private_::has_CountedValue_v T>
        String ToString (const T& t)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << "'" << Characters::ToString (t.fValue) << "': "sv << Characters::ToString (t.fCount);
            sb << "}"sv;
            return sb;
        }
        template <typename T>
        inline String ToString (const T& t)
            requires (is_enum_v<T>)
        {
            if constexpr (not Common::IBoundedEnum<T>) {
                return Characters::ToString (underlying_type_t<T> (t));
            }
            else if constexpr (Common::DefaultNames<T>{}.size () == 0) {
                // emit as number if no EnumNames<> declared
                return Characters::ToString (underlying_type_t<T> (t));
            }
            else {
                return Common::DefaultNames<T>{}.GetName (t);
            }
        }
        template <typename OPTIONS>
        inline String ToString (const StringBuilder<OPTIONS>& sb)
        {
            return sb.template As<String> ();
        }
        template <floating_point T>
        inline String ToString (T t)
        {
            return Private_::FloatToString_ (t, Number::FloatToStringOptions{});
        }
        template <floating_point T>
        inline String ToString (T t, const Number::FloatToStringOptions& o)
        {
            return Private_::FloatToString_ (t, o);
        }
        template <floating_point T>
        inline String ToString (T t, const Number::Precision& p)
        {
            return Private_::FloatToString_ (t, Number::FloatToStringOptions{}.With (p));
        }
        template <floating_point T>
        inline String ToString (T t, Number::Notation n)
        {
            return Private_::FloatToString_ (t, Number::FloatToStringOptions{}.With (n));
        }
        template <typename T>
        inline String ToString (const Math::CommonStatistics<T>& t)
        {
            Characters::StringBuilder sb;

            sb << "{"sv;
            if (t.fMin) {
                sb << "min: " << *t.fMin;
            }
            if (t.fMax) {
                sb << "max: " << *t.fMax;
            }
            if (t.fMean) {
                sb << "mean: " << *t.fMean;
            }
            if (t.fMedian) {
                sb << "median: " << *t.fMedian;
            }
            if (t.fStandardDeviation) {
                sb << "standard-deviation: " << *t.fStandardDeviation;
            };
            sb << "}"sv;
            return sb;
        }
        template <typename T>
        inline String ToString (const shared_ptr<T>& pt)
        {
            if (pt == nullptr) {
                return "nullptr"sv;
            }
            if constexpr (IToString<decltype (*pt)>) {
                return String{Common::StdCompat::format (L"*{}", Characters::ToString (*pt))};
            }
            return String{Common::StdCompat::format (L"{}", static_cast<const void*> (pt.get ()))};
        }
        template <typename T>
        inline String ToString (const unique_ptr<T>& pt)
        {
            if (pt == nullptr) {
                return "nullptr"sv;
            }
            if constexpr (IToString<decltype (*pt)>) {
                return String{Common::StdCompat::format (L"*{}", Characters::ToString (*pt))};
            }
            return String{Common::StdCompat::format (L"{}", static_cast<const void*> (pt.get ()))};
        }
        // A raw pointer prints its address, as "{}"_f prints a void* (0x...) - never what it points to, which may dangle. Not a
        // character pointer (a C string: the is_convertible_v<T, String> overload), nor an array (taken by reference, an array
        // does not decay, so T is not a pointer)
        template <typename T>
        inline String ToString (const T& t)
            requires (is_pointer_v<T> and not is_function_v<remove_pointer_t<T>> and not is_convertible_v<T, String>)
        {
            return String{Common::StdCompat::format (L"{}", static_cast<const void*> (t))};
        }
        template <typename T>
        inline String ToString (const optional<T>& o)
        {
            return o.has_value () ? Characters::ToString (*o) : "[missing]"sv;
        }
        template <typename FUNCTION_SIGNATURE>
        inline String ToString (const function<FUNCTION_SIGNATURE>& f)
        {
            return Common::StdCompat::format (L"{}", static_cast<const void*> (f.template target<remove_cvref_t<FUNCTION_SIGNATURE>> ()));
        }
        inline String ToString (const chrono::duration<double>& t)
        {
            return Characters::ToString (t.count ()) + " seconds"sv;
        }
        String ToString (const Time::Duration& t, const Number::Precision& secondsPrecision);
        template <typename CLOCK_T>
        inline String ToString (const chrono::time_point<CLOCK_T, chrono::duration<double>>& t)
        {
            return Characters::ToString (t.time_since_epoch ().count ()) + " seconds"sv;
        }
        template <integral T>
        inline String ToString (T t, ios_base::fmtflags flags)
        {
            return Private_::IntegerToString_ (t, flags);
        }
        template <integral T>
        String ToString (T t, const Number::IntegerToStringOptions& o)
            requires (not same_as<T, bool>)
        {
            Require (2 <= o.fRadix and o.fRadix <= 36);
            char digits[numeric_limits<T>::digits + 2]; // enough for base 2, and a sign
            auto [ptr, ec] = to_chars (std::begin (digits), std::end (digits), t, static_cast<int> (o.fRadix));
            Assert (ec == errc{});
            const char* d = digits;
            string      r;
            if (*d == '-') {
                r += *d++; // the prefix comes after the sign, as Number::Parse reads it
            }
            if (o.fEmitRadixPrefix and (o.fRadix == 16 or o.fRadix == 8 or o.fRadix == 2)) {
                r += '0';
                r += o.fRadix == 16 ? 'x' : (o.fRadix == 8 ? 'o' : 'b');
            }
            r.append (d, static_cast<size_t> (ptr - d));
            if (o.fUpperCase) {
                for (char& c : r) {
                    c = static_cast<char> (toupper (static_cast<unsigned char> (c)));
                }
            }
            Number::Punctuation p = Number::ResolvePunctuation (o.fPunctuation);
            if (not p.fDigitSeparator or p.fGroupSize == 0) {
                return String{r};
            }
            size_t          head = r.size () - static_cast<size_t> (ptr - d); // the sign and prefix, before the digits
            StringBuilder<> sb;
            sb << string_view{r}.substr (0, head);
            Private_::AppendGrouped_ (sb, string_view{r}.substr (head), p);
            return sb.str ();
        }
        // A template, so nothing converts to bool to get here: an int, a pointer or a class with an operator bool must
        // find its own overload, or not compile - never print as true/false
        template <same_as<bool> T>
        inline String ToString (T t)
        {
            // static: copying a String only counts a reference; making one from "true"sv allocates a rep, each call
            static const String kTrue_{"true"sv};
            static const String kFalse_{"false"sv};
            return t ? kTrue_ : kFalse_;
        }
        template <signed_integral T>
        inline String ToString (T t)
        {
            return Characters::ToString (t, ios_base::dec);
        }
        template <unsigned_integral T>
        inline String ToString (T t)
            requires (not same_as<T, bool>) // bool is an unsigned_integral
        {
            // no overwhelmingly good reason todo it this way, but this matches what we had in Stroika 2.1, and its reasonable...
            if constexpr (sizeof (T) == 1) {
                return Characters::ToString (t, ios_base::hex);
            }
            else {
                return Characters::ToString (t, ios_base::dec);
            }
        }
        inline String ToString (byte t)
        {
            return Characters::ToString (static_cast<unsigned char> (t), ios_base::hex);
        }
        inline String ToString (const filesystem::path& t)
        {
            return t.wstring ();
        }

    }

    namespace Private_ {
        // IToString's test - here, after the last ToStringDefaults overload: a qualified name in a template is looked up where
        // the template is defined, so any earlier would miss the overloads declared after it
        template <typename T>
        struct HasToStringDefault_ {
            static constexpr bool value = requires (T t) { ToStringDefaults::ToString (t); };
        };
    }

    /*
     ********************************************************************************
     *************************** Characters::ToString *******************************
     ********************************************************************************
     */
    template <typename T, typename... ARGS>
    inline String ToString (T&& t, ARGS... args)
    {
        return ToStringDefaults::ToString (forward<T> (t), forward<ARGS> (args)...);
    }

    /*
     ********************************************************************************
     *********************** Characters::UnoverloadedToString ***********************
     ********************************************************************************
     */
    template <typename T>
    inline String UnoverloadedToString (const T& t)
    {
        return ToString (t);
    }

    //DEPRECATED
    namespace Private_ {
        template <typename T>
        using has_ToString_t = decltype (static_cast<Characters::String> (declval<T&> ().ToString ()));
    }
    /*
     *  \brief Return true iff Characters::ToString (T) is well defined.
     */
    template <typename T>
    [[deprecated ("Since Stroika v3.0d5 use IToString")]] constexpr inline bool has_ToString_v =
        Common::is_detected_v<Private_::has_ToString_t, T>;

}

namespace Stroika::Foundation::Traversal {
    template <typename T>
    inline Characters::String Iterable<T>::Join (const Characters::String& separator) const
    {
        return Join (separator, nullopt);
    }
    template <typename T>
    inline Characters::String Iterable<T>::Join (const Characters::String& separator, const optional<Characters::String>& finalSeparator) const
    {
        using namespace Characters;
#if qCompilerAndStdLib_kDefaultToStringConverter_Buggy
        function<String (T)> cvt;
        if constexpr (same_as<T, String>) {
            cvt = Common::Identity{};
        }
        else {
            cvt = UnoverloadedToString<T>;
        }
        return this->Join (cvt, StringCombiner<String>{.fSeparator = separator, .fSpecialSeparatorForLastPair = finalSeparator});
#else
        return this->Join (kDefaultToStringConverter<String>,
                           StringCombiner<String>{.fSeparator = separator, .fSpecialSeparatorForLastPair = finalSeparator});
#endif
    }
    template <typename T>
    template <typename RESULT_T, invocable<T> CONVERT_TO_RESULT>
    inline RESULT_T Iterable<T>::Join (const CONVERT_TO_RESULT& convertToResult, const RESULT_T& separator, const optional<RESULT_T>& finalSeparator) const
        requires (convertible_to<invoke_result_t<CONVERT_TO_RESULT, T>, RESULT_T>)
    {
        return this->Join<RESULT_T> (
            convertToResult, Characters::StringCombiner<RESULT_T>{.fSeparator = separator, .fSpecialSeparatorForLastPair = finalSeparator});
    }

}
