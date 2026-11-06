/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Number_h_
#define _Stroika_Foundation_Characters_Number_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <ios>
#include <locale>
#include <optional>
#include <span>
#include <variant>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/Locale.h"
#include "Stroika/Foundation/Common/StdCompat.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"

/**
 *  Numbers as text, in both directions (namespace Characters::Number):
 *      o   text -> number:     Number::Parse<T> (s, options) - T an integral or floating point type
 *      o   number -> text:     Characters::ToString (n, options) (ToString.h) - with the options types here
 *
 *  Each direction has its own Options - writing says exactly what to produce, reading what to accept - but where both describe the
 *  same thing (punctuation), they use the one type defined here, as std::chars_format serves both std::to_chars
 *  and std::from_chars.
 *
 *  Punctuation - a number's decimal point, and how its digits are grouped: 1,234,567.5 or 1.234.567,5 - is all a locale changes in
 *  how a number is written (its digits are 0 to 9 in every locale). Every options type here holds it as a Punctuation:
 *      o   by default the "C" locale's - '.', and no grouping - whatever the program's global locale: text one program writes for
 *          another (JSON, XML, configuration files, command lines) must read the same in every program, and most number text is that;
 *      o   Punctuation::FromLocale (l) gives a locale's, taken when called - for text written for people;
 *      o   Common::eUseCurrentLocale gives the global locale's at the time the options are used - late, where that matters.
 *  So each part of it has the one source: no options type holds a std::locale (Punctuation::ToLocale makes one, for code needing one).
 *
 *  Parse: text that is not a number of the type asked for - or is out of its range - is an error, reported in the expected<>
 *  returned, never a default value (String2Int's 0, ToFloat's NaN or infinity). By default the text must be just the number; options
 *  accept more (whitespace, a radix prefix, punctuation other than the "C" locale's). Digit grouping is checked as the punctuation
 *  gives it, so a separator cannot pass unnoticed: with German punctuation "1.000" is 1000, and "1.5" an error - not 15.
 *
 *  \note Before Stroika v3.0d26 the types here were in Characters::FloatConversion, and FloatToStringOptions held a std::locale.
 */

namespace Stroika::Foundation::Characters::Number {

    /**
     *  \brief Where a floating point number's exponent goes
     *
     *  Only where the exponent goes: how many digits are written, and what they count, is the Precision.
     *
     *  Extension points: ICU's engineering notation (an exponent that is a multiple of 3) and compact notation ("1.2K") would be
     *  more values here.
     */
    enum class Notation {
        /**
         *  An exponent only for a very large or very small number - printf's %g, std::chars_format::general: scientific where the
         *  exponent would be below -4, or at least P - P the precision's maximum significant digits, or 6 for ShortestRoundTrip
         *  (as std::to_chars writes chars_format::general without a precision). A fraction precision has no P: it is written ePlain.
         */
        eGeneral,

        /**
         *  Never an exponent (Java's "plain", ICU's "simple"): printf's %f is {ePlain, Precision::FixedFraction (n)}.
         *
         *  \note A very large or very small number is written in full: 1e300 is 301 characters.
         */
        ePlain,

        /**
         *  Always an exponent: one digit, the decimal point, the rest, then e+NN (at least two exponent digits) - printf's %e is
         *  {eScientific, Precision::FixedFraction (n)}, the fraction counted after the first digit.
         */
        eScientific,
    };

    /**
     *  \brief How many digits a floating point number is written with, and what they count: significant figures, or digits after
     *         the decimal point - or, ShortestRoundTrip, as many as read back as the same number
     *
     *  ICU's Precision, made only by the factories below - so a Precision always says what it counts, and C++'s 'precision' (which
     *  counts significant figures for %g, but digits after the decimal point for %f and %e) has no part in it.
     *
     *  The digits written are the number's shortest round-trip digits - the fewest that read back as the same value - where the
     *  precision has room for them, padded with zeros to its minimum; else the number correctly rounded to its maximum (its exact
     *  binary value, half to even, as printf and std::to_chars round). So within the type's precision (numeric_limits<T>::digits10
     *  significant digits: 15 for a double) the text is printf's; asked for more digits than the number has, it is the shortest
     *  round-trip text, not binary noise - 1.1 at 17 significant digits is "1.1", not "1.1000000000000001".
     *
     *  Extension points: ICU's increment rounding (to the nearest 0.05) and minimum integer digits ("007") would be more factories
     *  here; a rounding mode, a field of FloatToStringOptions.
     *
     *  \note Satisfies Concepts:
     *      o   Common::IConstexprable<Precision>
     */
    class Precision {
    public:
        /**
         *  \brief A least and a most number of digits
         */
        struct MinMax {
            unsigned int fMin;
            unsigned int fMax;

            constexpr bool operator== (const MinMax&) const = default;
        };

    public:
        /**
         *  \brief Exactly n significant digits - padded with zeros: 3.1 is "3.10000" for 6 (printf's %#.6g)
         *
         *  \pre n >= 1
         */
        static constexpr Precision FixedSignificantDigits (unsigned int n);

    public:
        /**
         *  \brief At most n significant digits - no trailing zeros: 3.1 is "3.1" for 6 (printf's %.6g)
         *
         *  \pre n >= 1
         */
        static constexpr Precision MaxSignificantDigits (unsigned int n);

    public:
        /**
         *  \brief At most maxDigits significant digits, padded with zeros to at least minDigits
         *
         *  \pre 1 <= maxDigits and minDigits <= maxDigits
         */
        static constexpr Precision MinMaxSignificantDigits (unsigned int minDigits, unsigned int maxDigits);

    public:
        /**
         *  \brief Exactly n digits after the decimal point - padded with zeros: 3.1 is "3.10" for 2 (with ePlain, printf's %.2f)
         */
        static constexpr Precision FixedFraction (unsigned int n);

    public:
        /**
         *  \brief At most n digits after the decimal point - no trailing zeros: 3.1 is "3.1" for 2
         */
        static constexpr Precision MaxFraction (unsigned int n);

    public:
        /**
         *  \brief At most maxDigits digits after the decimal point, padded with zeros to at least minDigits
         *
         *  \pre minDigits <= maxDigits
         */
        static constexpr Precision MinMaxFraction (unsigned int minDigits, unsigned int maxDigits);

    public:
        /**
         *  \brief The fewest digits that read back as the same number (ICU's unlimited; std::to_chars without a precision)
         */
        static constexpr Precision ShortestRoundTrip ();

    public:
        /**
         *  \brief The significant digits, for a ...SignificantDigits precision; else nullopt
         */
        constexpr optional<MinMax> GetSignificantDigits () const;

    public:
        /**
         *  \brief The digits after the decimal point, for a ...Fraction precision; else nullopt
         */
        constexpr optional<MinMax> GetFractionDigits () const;

    public:
        /**
         *  \brief Made by ShortestRoundTrip ()
         */
        constexpr bool IsShortestRoundTrip () const;

    public:
        constexpr bool operator== (const Precision&) const = default;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        enum class Kind_ : uint8_t {
            eSignificantDigits_,
            eFraction_,
            eShortestRoundTrip_
        };
        constexpr Precision (Kind_ kind, MinMax digits);

    private:
        Kind_  fKind_;
        MinMax fDigits_;
    };
    static_assert (Common::IConstexprable<Precision>); // see Satisfies Concepts

    /**
     *  \brief How a number's text is punctuated: its decimal point, and how its digits are grouped
     *
     *  The default - '.', and no grouping - is the "C" locale's. Parse accepts digits grouped as given, or not grouped at all, and
     *  never a separator out of place; ToString groups them as given.
     *
     *  \note Satisfies Concepts:
     *      o   Common::IConstexprable<Punctuation>
     *
     *  \par Example Usage
     *      \code
     *          Number::Punctuation{}                                                                    // 1234567.5 - the "C" locale's
     *          Number::Punctuation::FromLocale (locale{"de_DE.UTF-8"})                                   // 1.234.567,5
     *          Number::Punctuation{.fDigitSeparator = ',', .fGroupSize = 3}                             // 1,234,567.5
     *          Number::Punctuation{.fDigitSeparator = ',', .fGroupSize = 3, .fSecondaryGroupSize = 2}   // 12,34,567.5 - as India groups
     *          Number::Punctuation{.fDigitSeparator = '_'}                                              // 1_234_567.5 read, sizes unchecked
     *      \endcode
     */
    struct Punctuation {
        /**
         *  Between a floating point number's whole and fractional parts (an integer has none).
         */
        Character fDecimalPoint{'.'};

        /**
         *  Between groups of digits - nullopt: digits are not grouped.
         */
        optional<Character> fDigitSeparator;

        /**
         *  The digits in the rightmost group (3 in most locales); Parse requires each group so, ToString writes them so. 0: no size -
         *  Parse accepts fDigitSeparator anywhere between two digits (1_000_000, as C++, Python and Rust write literals), and
         *  ToString groups nothing.
         */
        unsigned int fGroupSize{0};

        /**
         *  The digits in each group left of the rightmost - 0: fGroupSize. India groups 12,34,56,789: 3, then 2.
         */
        unsigned int fSecondaryGroupSize{0};

        /**
         *  l's punctuation (its numpunct<wchar_t>) - taken now: a later change to l, or to the global locale, does not change it. A
         *  locale whose numpunct::grouping () is empty groups nothing - the "C" locale, though its thousands_sep () is ','.
         */
        static Punctuation FromLocale (const locale& l);

        /**
         *  A locale - base, with this punctuation (its numpunct facets replaced) - for code that needs a std::locale, as a stream
         *  imbued with one. Its numpunct<char> keeps base's separator and grouping where fDigitSeparator is not ASCII, and its decimal
         *  point where fDecimalPoint is not.
         */
        nonvirtual locale ToLocale (const locale& base = locale::classic ()) const;

        /**
         *  \see Characters::ToString ();
         */
        nonvirtual String ToString () const;

        constexpr bool operator== (const Punctuation&) const = default;
    };
    static_assert (Common::IConstexprable<Punctuation>); // see Satisfies Concepts

    /**
     *  The punctuation options give: a Punctuation - Punctuation{}, the "C" locale's, by default - or Common::eUseCurrentLocale: the
     *  global locale's at the time the options are used (Common::eUseCLocale is Punctuation{}).
     */
    using PunctuationSpecifier = variant<Punctuation, Common::PredefinedLocale>;

    /**
     *  \brief The Punctuation p gives - for Common::eUseCurrentLocale, the global locale's now
     */
    Punctuation ResolvePunctuation (const PunctuationSpecifier& p);

    /**
     *  \brief How Characters::ToString writes a floating point number: ToString (f, options)
     *
     *  The default, {Notation::eGeneral, Precision::MaxSignificantDigits (6)}, is C's - printf's %g, a stream's default; kRoundTrip
     *  writes text that reads back as the same number. The number is punctuated as fPunctuation says: by default as the "C" locale
     *  does, whatever the global locale.
     *
     *  NaN and infinity are written as the standard library writes them - "nan", "inf" or "-inf" (a NaN may be "-nan") - which
     *  Number::Parse reads back.
     *
     *  \par Example Usage
     *      \code
     *          ToString (3.14159265358979)                                                                  // "3.14159"
     *          ToString (0.1, FloatToStringOptions::kRoundTrip)                                             // "0.1" - reads back exactly
     *          ToString (3.1, FloatToStringOptions{.fNotation = Notation::ePlain, .fPrecision = Precision::FixedFraction (2)}) // "3.10" - %.2f
     *          ToString (x, FloatToStringOptions::kRoundTrip.With (Notation::eScientific))                  // a constant, one setting changed
     *          ToString (1234567.5, FloatToStringOptions{.fNotation   = Notation::ePlain,
     *                                .fPrecision  = Precision::MaxFraction (2),
     *                                .fPunctuation = Punctuation::FromLocale (locale{"de_DE.UTF-8"})})      // "1.234.567,5"
     *          ToString (x, FloatToStringOptions{}.With (Common::eUseCurrentLocale))                        // the global locale's, when written
     *      \endcode
     *
     *  Performance: the default and kRoundTrip are one std::to_chars call, as is any precision within the type's
     *  (numeric_limits<T>::digits10 significant digits - 15 for a double) - with the default punctuation, nothing more. Slower:
     *      o   other punctuation: a pass over the text;
     *      o   Common::eUseCurrentLocale: the global locale's punctuation looked up on every call - take Punctuation::FromLocale
     *          once, and keep it, instead;
     *      o   more significant digits than the type holds: two std::to_chars calls;
     *      o   printf, where std::to_chars cannot write the number: a long double that is not a double, with libc++ (whose
     *          std::to_chars writes a long double as a double, as of 22); every floating point type, for a macOS deployment
     *          target before 13.3 (Apple's libc++ has floating point std::to_chars from 13.3 on).
     *
     *  Extension points, each a field off by default: a rounding mode (ICU's RoundingMode - half to even is used, ICU's default;
     *  half away from zero is JavaScript's) - std::to_chars gives any float's exact decimal digits, so any mode can be built on
     *  them; a sign display (always write '+'); a decimal-point display (keep "3." with no fraction).
     *
     *  \note Satisfies Concepts:
     *      o   Common::IConstexprable<FloatToStringOptions>
     */
    struct FloatToStringOptions {
        /**
         *  Where the exponent goes
         */
        Notation fNotation{Notation::eGeneral};

        /**
         *  How many digits, and what they count
         */
        Precision fPrecision{Precision::MaxSignificantDigits (6)};

        /**
         *  The decimal point, and how the digits are grouped: by default the "C" locale's - '.', and no grouping
         */
        PunctuationSpecifier fPunctuation{Punctuation{}};

        /**
         *  \brief A copy, each argument - a Notation, Precision, Punctuation or Common::PredefinedLocale - replacing the setting of
         *         its type (a PredefinedLocale, the punctuation)
         */
        template <typename... ARGS>
        constexpr FloatToStringOptions With (ARGS&&... args) const;

        /**
         *  \brief The punctuation ToString writes with - for Common::eUseCurrentLocale, the global locale's now
         */
        nonvirtual Punctuation GetPunctuation () const;

        constexpr bool operator== (const FloatToStringOptions&) const = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

        /**
         *  {eGeneral, ShortestRoundTrip ()}: the fewest digits that read back as the same number - std::to_chars (f,
         *  chars_format::general)'s text
         */
        static const FloatToStringOptions kRoundTrip;

    private:
        constexpr void Set_ (Notation n);
        constexpr void Set_ (const Precision& p);
        constexpr void Set_ (const Punctuation& p);
        constexpr void Set_ (Common::PredefinedLocale p);
    };
    static_assert (Common::IConstexprable<FloatToStringOptions>); // see Satisfies Concepts

    /**
     *  \brief How ToString writes an integer: by default in base 10 - what ParseInteger reads by default
     *
     *  Writing says exactly what to produce, so the matching IntegerParseOptions reads it back: the same fRadix and fPunctuation,
     *  and fAcceptRadixPrefix for fEmitRadixPrefix.
     *
     *  \par Example Usage
     *      \code
     *          ToString (255, IntegerToStringOptions{.fRadix = 16});                                                  // "ff"
     *          ToString (-26, IntegerToStringOptions{.fRadix = 16, .fEmitRadixPrefix = true, .fUpperCase = true});   // "-0X1A"
     *          ToString (1234567, IntegerToStringOptions{.fPunctuation = Punctuation{.fDigitSeparator = ',', .fGroupSize = 3}}); // "1,234,567"
     *      \endcode
     *
     *  \note Not here yet - to add when a caller needs one: a minimum width or zero padding.
     */
    struct IntegerToStringOptions {
        /**
         *  2 to 36; for a radix over 10, letters for the digits past 9.
         */
        unsigned int fRadix{10};

        /**
         *  Write the radix's usual prefix, after any sign: 0x for 16, 0o for 8, 0b for 2 (other radixes have none).
         */
        bool fEmitRadixPrefix{false};

        /**
         *  The letters - digits past 9, and the prefix's x, o or b - in upper case: FF and 0X, not ff and 0x.
         */
        bool fUpperCase{false};

        /**
         *  How to group the digits (an integer has no decimal point) - by default not at all, as the "C" locale.
         */
        PunctuationSpecifier fPunctuation;
    };

    /**
     *  The options ToString takes for a T: FloatToStringOptions or IntegerToStringOptions - for generic code.
     */
    template <typename T>
    using ToStringOptions = conditional_t<floating_point<T>, FloatToStringOptions, IntegerToStringOptions>;

    /**
     *  \brief How Parse reads an integer: by default strictly - just the number, in base 10
     *
     *  Each option makes it accept more; the default accepts the least, so a lenient read says so where it is called.
     *
     *  \par Example Usage
     *      \code
     *          Number::Parse<unsigned int> ("0x1A", {.fRadix = 16, .fAcceptRadixPrefix = true});     // 26
     *          Number::Parse<int> (" 42 ", {.fIgnoreSurroundingWhitespace = true});                  // 42
     *          Number::Parse<int> ("1,234,567", {.fPunctuation = Number::Punctuation{.fDigitSeparator = ',', .fGroupSize = 3}});  // 1234567
     *      \endcode
     *
     *  \note Not here yet - to add when a caller needs one: a remainder overload of Parse for an integer, reading the number at the start
     *        of longer text (as a floating point number's) - say "0 kB".
     */
    struct IntegerParseOptions {
        /**
         *  2 to 36; for a radix over 10, letters for the digits past 9, in either case.
         */
        unsigned int fRadix{10};

        /**
         *  Also accept the radix's usual prefix, after any sign, in either case: 0x for 16, 0o for 8, 0b for 2 (other radixes have none) -
         *  as IntegerToStringOptions::fEmitRadixPrefix writes it.
         *
         *  Off by default: the radix is the caller's, so a prefix could only repeat or contradict it, and text formats differ on which
         *  they use (0x, #, $, &H...). Even on, the radix is never guessed from the text, as C's strtol (base 0) guesses it - a leading
         *  0 meaning octal, so "010" is 8, is a known source of bugs; a leading 0 is always just a digit.
         */
        bool fAcceptRadixPrefix{false};

        /**
         *  How the digits may be grouped (an integer has no decimal point) - by default not at all, as the "C" locale. Parse accepts
         *  digits grouped as given, or not grouped at all.
         */
        PunctuationSpecifier fPunctuation;

        /**
         *  Ignore whitespace before and after the number.
         */
        bool fIgnoreSurroundingWhitespace{false};
    };

    /**
     *  \brief s as an integer of type T - an error if it is not one, or is out of T's range (by default s must be just the
     *  number; options say what else to accept)
     *
     *  s is just the number: an optional + or - sign, then digits in options.fRadix - no whitespace, no radix prefix, no digit
     *  separators, nothing after (unless options say otherwise). The same text parses the same in any locale (this uses
     *  std::from_chars).
     *
     *  The error - a DataExchange::BadFormatException, a runtime error: bad data, not a bad call - quotes s, and says whether it is
     *  not an integer or out of T's range. ThrowIfFailed () throws it; value_or (0) gives String2Int's lenient behavior instead.
     *
     *  \par Example Usage
     *      \code
     *          uint16_t port = ThrowIfFailed (Number::Parse<uint16_t> (portText));   // throws if portText is not a port number
     *          int      n    = Number::Parse<int> ("33aaa"sv).value_or (0);                // 0 - not an integer
     *          if (auto p = Number::Parse<uint16_t> (portText)) {
     *              use (*p);
     *          }
     *          else {
     *              cerr << Characters::ToString (p.error ()) << endl;               // "70000" is out of range (0 to 65535)
     *          }
     *      \endcode
     *
     *  \pre 2 <= options.fRadix and options.fRadix <= 36
     */
    template <integral T, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> Parse (span<const CHAR_T> s, const IntegerParseOptions& options = {});
    template <integral T, IConvertibleToString STRINGISH_ARG>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> Parse (STRINGISH_ARG&& s, const IntegerParseOptions& options = {});

    /**
     *  \brief How Parse reads a floating point number: by default strictly - just the number, in the "C" locale's format
     *
     *  \par Example Usage
     *      \code
     *          auto de = Number::Punctuation::FromLocale (locale{"de_DE.UTF-8"});
     *          Number::Parse<double> ("1,5", {.fPunctuation = de});                                              // 1.5 - its decimal point is a comma
     *          Number::Parse<double> ("1.000,5", {.fPunctuation = de});                                          // 1000.5 - grouped by 3, with a dot
     *          Number::Parse<double> ("1.5", {.fPunctuation = de});                                              // an error - not 15
     *          Number::Parse<double> ("1_000.5", {.fPunctuation = Number::Punctuation{.fDigitSeparator = '_'}}); // 1000.5
     *      \endcode
     *
     *  \note Not here yet - to add when a caller needs one: refusing nan and inf as text (accepted now, as ToFloat accepted them).
     */
    struct FloatParseOptions {
        /**
         *  The decimal point, and how the digits may be grouped - by default the "C" locale's: '.', and no grouping, so the same text
         *  parses the same in every program, whatever its global locale. Parse accepts digits grouped as given, or not grouped at all;
         *  a '.' is not the decimal point where the decimal point is something else.
         */
        PunctuationSpecifier fPunctuation;

        /**
         *  Ignore whitespace before and after the number.
         */
        bool fIgnoreSurroundingWhitespace{false};
    };

    /**
     *  The options Parse<T> takes: IntegerParseOptions or FloatParseOptions - for generic code.
     */
    template <typename T>
    using ParseOptions = conditional_t<floating_point<T>, FloatParseOptions, IntegerParseOptions>;

    /**
     *  \brief s as a floating-point number of type T - an error if it is not one, or is out of T's range (by default s must be
     *  just the number; options say what else to accept)
     *
     *  s is just the number: an optional sign, digits with an optional decimal point and exponent (1.5, -2e10, .5) - or nan, inf or
     *  infinity (in any case, with an optional sign), which give NaN or infinity as such. No whitespace, nothing after (unless
     *  options say otherwise). By default the "C" locale's format, whatever the global locale - so the same text parses the same
     *  in every program (this uses std::from_chars - or, where the standard library cannot read floating point numbers with it, a
     *  stream in the "C" locale).
     *
     *  The error, as for an integer: "is not a number" or "is out of range" - too large for T, or so small it would be 0 (a
     *  subnormal number is in range). value_or (NAN) gives ToFloat's lenient behavior for text that is not a number.
     *
     *  The overload with a remainder reads the number at the start of s (after any whitespace), in the "C" locale's format, and puts
     *  what follows it in *remainder - for a number within longer text; its error is that s "does not start with a number". s may be
     *  *remainder, to read one number after another.
     *
     *  \par Example Usage
     *      \code
     *          double d = ThrowIfFailed (Number::Parse<double> (text));
     *          String rest;
     *          double e = ThrowIfFailed (Number::Parse<double> ("3.5 apples"_k, &rest));     // 3.5, and rest is " apples"
     *      \endcode
     *
     *  \pre remainder != nullptr
     */
    template <floating_point T, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> Parse (span<const CHAR_T> s, const FloatParseOptions& options = {});
    template <floating_point T, typename STRINGISH_ARG>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> Parse (STRINGISH_ARG&& s, const FloatParseOptions& options = {})
        requires (IConvertibleToString<STRINGISH_ARG> or is_convertible_v<STRINGISH_ARG, std::string>);
    template <floating_point T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> Parse (const String& s, String* remainder);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Number.inl"

#endif /*_Stroika_Foundation_Characters_Number_h_*/
