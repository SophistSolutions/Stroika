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
#include "Stroika/Foundation/Common/Locale.h"
#include "Stroika/Foundation/Common/StdCompat.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"

/**
 *  Numbers as text, in both directions (namespace Characters::Number):
 *      o   text -> number:     Number::Parse<T> (s, options) - T an integral or floating point type
 *      o   number -> text:     Characters::ToString (n, options) (ToString.h) - with the options types here
 *
 *  Each direction has its own Options - writing says exactly what to produce, reading what to accept - but where both describe the
 *  same thing (significant figures, say), they use the one type defined here, as std::chars_format serves both std::to_chars and
 *  std::from_chars.
 *
 *  Parse: text that is not a number of the type asked for - or is out of its range - is an error, reported in the expected<>
 *  returned, never a default value (String2Int's 0, ToFloat's NaN or infinity). By default the text must be just the number; options
 *  accept more (whitespace, digit separators, a radix prefix, a locale's decimal point).
 *
 *  \note Before Stroika v3.0d26 the types here were in Characters::FloatConversion.
 */

namespace Stroika::Foundation::Characters {

    /**
     * Control needless trailing zeros. For example, 3.000 instead of 3, or 4.2000 versus 4.2. 
     * 
     * Sometimes eDontTrimZeros desirable (to show precision): but often not.
     */
    enum class [[deprecated ("Since Stroika v3.0d23 use FloatFormatType 'trim' variant instead")]] TrimTrailingZerosType {
        eTrimZeros,
        eDontTrimZeros,

        Stroika_Define_Enum_Bounds (eTrimZeros, eDontTrimZeros)
    };
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    using TrimTrailingZerosType::eDontTrimZeros;
    using TrimTrailingZerosType::eTrimZeros;
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

}

namespace Stroika::Foundation::Characters::Number {

    /**
     *  Significant figures are the digits in a measured or calculated value that carry reliable information
     *  regarding its precision and accuracy, typically including all non-zero digits, zeros between non-zero digits, and trailing zeros in a decimal.
     *  They define the limit of a measurement's certainty.
     * 
     *  This is used for specifying how to format floating point numbers.
     * 
     *  @alias Precision (this was called 'Precision' before Stroika v3.0d23)
     * 
     *  Rules:
     *      o   Non-zero Digits: All digits from 1-9 are always significant (e.g., \(45.2\) has 3).
     *      o   Interior Zeros: Zeros between non-zero digits are always significant (e.g., \(1002\) has 4).
     *      o   Leading Zeros: Zeros to the left of the first non-zero digit are never significant; they are placeholders (e.g., \(0.0032\) has 2).
     *      o   Trailing Zeros (Decimal Present): Zeros at the end of a number that contains a decimal point are significant (e.g., \(92.00\) has 4).
     *      o   Trailing Zeros (No Decimal): Zeros at the end of a number without a decimal point are ambiguous and usually not significant (e.g., \(140\) has 2), unless indicated by a decimal point (e.g., \(140.\) has 3).
     *      o   Exact Numbers: Numbers from counting or definitions (e.g., \(12\) inches in a foot) have an infinite number of significant figures.
     *      o   Scientific Notation: In \(A\times 10^{b}\), all digits in the coefficient (\(A\)) are significant (e.g., \(1.020\times 10^{3}\) has 4).
     * 
     * Examples:
     *      "3.01"              =>      3
     *      "03.01"             =>      3
     *      "-44.21"            =>      4
     *      "+44.21"            =>      4
     *      "-44.21e2"          =>      4
     *      "-44.210e2"         =>      5
     *      "400"               =>      1
     *      "400."              =>      3
     *      "400.0"             =>      4
     *      "0.0000001234567"   =>      7
     *      "0.000000000"       =>      9       ; a bit ambiguous given the rules - leading zeros vs trailing zeros (decimal present)
     * 
     *  \note - This differs from the iostream library 'precision' where:
     *      Its exact meaning depends on whether the stream is using the default floating-point notation or the std::fixed or std::scientific
     *          Default Notation (defaultfloat): The precision value specifies the total number of significant digits to display.
     *          Fixed or Scientific Notation (fixed, scientific): The precision value specifies the exact number of digits to appear after the decimal point
     * 
     *  The special value SignificantFigures::kFullPrecision refers to when you wish the full precision that allows the exact value to be read back
     *  after being written:
     *
     *  \note - The c++ float 'precision' is always 1 less than the number of significant figures (since format is always N.xxxxeWW).
     *          Only possible slight exception would be for the number zero, where I'm not quite sure, but I think this is roughly right then too.
     * 
     *  \note
     *      \see https://stackoverflow.com/questions/22458355/what-is-the-purpose-of-max-digits10-and-how-is-it-different-from-digits10
     *          Roughly:
     *              o   digits10 is the number of decimal digits guaranteed to survive text → float → text round-trip.
     *              o   max_digits10 is the number of decimal digits needed to guarantee correct float → text → float round-trip.
     */
    struct SignificantFigures {
    public:
        using RepType = unsigned int; // maybe use uint16_t?

    public:
        /**
         *  Flag indicating full precision (see kFullPrecision) - max_digits10
         */
        enum FullFlag {
            eFull
        };

    public:
        /**
         *  SignificantFigures ()/0
         *      Same as kDefault: 6
         *  SignificantFigures(FullFlag): 
         *      special magic value, so depending on type 'T' in call to GetEffectiveSignificantFigures () - gets full precision for that type
         */
        constexpr SignificantFigures () = default;
        constexpr SignificantFigures (RepType p);
        constexpr SignificantFigures (FullFlag);

    public:
        constexpr bool operator== (const SignificantFigures&) const = default;

    public:
        /**
         *  Return the used precision. This will be the value specified in the constructor, or
         *  if 'Full' precision given in the constructor, the desired floating point type will be used to compute
         *  the appropriate precision.
         * 
         *  NOTE - its NOT super clear what this should be. But our definition of it is sufficient precision so that when
         *  you read the value back in, (serialize then deserialize) - you get the original value.
         * 
         *  Docs like https://en.cppreference.com/w/cpp/types/numeric_limits/digits10 - seem to suggest this might be 
         *          std::numeric_limits<T>::digits10 or std::numeric_limits<T>::digits10-1 for floating point types.
         * 
         *  Docs like https://en.cppreference.com/w/cpp/io/manip/setprecision - suggest it might be:
         *          std::numeric_limits<T>::digits10 + 1;
         *
         *  Docs like https://en.cppreference.com/w/cpp/utility/to_chars - (3) - suggest none of the above - you must call
         *      to_chars() without specifying a precision.
         * 
         *  https://stackoverflow.com/questions/22458355/what-is-the-purpose-of-max-digits10-and-how-is-it-different-from-digits10
         *  Selected numeric_limits<T>::max_digits10 cuz used to map float -> text, and then hopefully someday back to float value preserving
         */
        template <floating_point T>
        constexpr RepType GetEffectiveSignificantFigures () const;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    public:
        /**
         *      kDefault is 6
         */
        static const SignificantFigures kDefault;

    public:
        /**
         * @brief Sentinal value that is interpretted differently depending on the type passed to GetEffectiveSignificantFigures()
         */
        static const SignificantFigures kFullPrecision;

    public:
        /**
         *  Calculate the precision - number of significant digits - in the given number. For this purpose, count trailing
         *  zeros. So basically string length, minus 1 for '.', minus 1 for any leading +-, minus any characters in exponential
         *  specifier.
         */
        template <IStdBasicStringCompatibleCharacter CHAR>
        static constexpr auto Calculate (span<const CHAR> number) -> RepType;

    private:
        /**
         *  nullopt means kFullPrecision
         */
        optional<RepType> fSignificantFigures_{6};
    };

    DISABLE_COMPILER_MSC_WARNING_START (4996)
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")

    /**
     */
    enum class FloatFormatType {

        /**
         *  corresponds to unsetf (floatfield) - which may be different than scientific or fixed point.
         *  This is basically what the 'C' standard decided would be the default way to format floating point numbers.
         * 
         *  Precision Field: The stream's precision setting (managed by std::setprecision or std::ios_base::precision()) 
         *  specifies the maximum number of meaningful digits to display in total (both before and after the decimal point),
         *  not a fixed number of digits after the decimal point.
         * 
         *  Trailing Zeros: It does not pad the output with trailing zeros if the number can be displayed with 
         *  fewer digits than the precision.
         * 
         *  Notation: It automatically switches between fixed-point and scientific notation as needed to best 
         *  represent the value within the given precision. For example, a large number or a number very close 
         *  to zero might be shown in scientific notation, while others will be in fixed-point notation. 
         * 
         *  For example (first 3 from https://en.cppreference.com/w/cpp/io/manip/fixed.html)
         *      number              │   output
         *       -------------------│----------------------
         *       0.0                │   0
         *       0.01               │   0.01
         *       0.00001            │   1e-05           (probably could be 0.00001)
         *       3.12e12            │   3.12e+12        (or could be 3.12000e+12 for example using libc++ >= 21)
         *       3.12               │   3.12
         *       212312345.0        │   2.12312e+08
         *       -44.2              │  -44.2
         *       0.0000001234567    │   1.23457e-07
         */
        eDefaultFloat,

        /**
         *  corresponds to ios_base::fixed (numbers are displayed without an exponent part, not actually fixed with display)
         * 
         *  For example (first 3 from https://en.cppreference.com/w/cpp/io/manip/fixed.html)
         *      number              │   output
         *       -------------------│----------------------
         *       0.0                │   0.000000
         *       0.01               │   0.010000
         *       0.00001            │   0.000010
         *       3.12e12            │   3120000000000
         *       3.12               │   3.12
         *       212312345.0        │   212312345
         *       -44.2              │   -44.2
         *       0.0000001234567    │   0.000000
         */
        eFixedPointWithoutWhitespaceTrimmed,

        /**
         * @brief eFixedPointWithoutWhitespaceTrimmed
         */
        eFixedPoint = eFixedPointWithoutWhitespaceTrimmed,

        /**
         *  For example (first 3 from https://en.cppreference.com/w/cpp/io/manip/fixed.html)
         *      number              │   output
         *       -------------------│----------------------
         *       0.0                │   0
         *       0.01               │   0.01
         *       0.00001            │   0.00001
         *       3.12e12            │   3120000000000
         *       3.12               │   3.12
         *       212312345.0        │   212312345
         *       -44.2              │   -44.2
         *       0.0000001234567    │   0
         */
        eFixedPointWithWhitespaceTrimmed,

        /**
         *  corresponds to ios_base::scientific
         * 
         *  For example (first 3 from https ://en.cppreference.com/w/cpp/io/manip/fixed.html)
         *      number              │   output
         *       -------------------│----------------------
         *       0.0                │   0.000000e+00
         *       0.01               │   1.000000e-02
         *       0.00001            │   1.000000e-05
         *       3.12e12            │   3.12000e+12
         *       3.12               │   3.12000e+00
         *       212312345.0        │   2.12312e+08
         *       -44.2              │  -4.42000e+01
         *       0.0000001234567    │   1.23457e-07
         */
        eScientificWithoutWhitespaceTrimmed,

        /**
         * @brief eScientificWithoutWhitespaceTrimmed
         */
        eScientific = eScientificWithoutWhitespaceTrimmed,

        /**
         *  For example (first 3 from https ://en.cppreference.com/w/cpp/io/manip/fixed.html)
         *      number              │   output
         *       -------------------│----------------------
         *       0.0                │   0.0e+00
         *       0.01               │   1.0e-02
         *       0.00001            │   1.0e-05
         *       3.12e12            │   3.12e+12
         *       3.12               │   3.12e+00
         *       212312345.0        │   2.12312e+08
         *       -44.2              │  -4.42e+01
         *       0.0000001234567    │   1.23457e-07
         */
        eScientificWithWhitespaceTrimmed,

        /**
         *  \brief Somewhat like defaultfloat, but never uses scientific notation.
         * 
         *  not scientific (no e+nn), but otherwise somewhat like fixed point or defaultfloat.
         * 
         *  Really no good name for this (unscientific, defaultfloat means something differnt in C++).
         *  Sort of like defaultfloat but NEVER deciding to use scientific notation.
         * 
         *  For example (first 3 from https://en.cppreference.com/w/cpp/io/manip/fixed.html)
         *      number              │   output
         *       -------------------│----------------------
         *       0.0                │   0
         *       0.01               │   0.01
         *       0.00001            │   0.00001
         *       3.12e12            │   3120000000000
         *       3.12               │   3.12
         *       212312345.0        │   212312345
         *       -44.2              │   -44.2
         *       0.0000001234567    │   0.000000123457
         */
        eStandard,

        eAutomaticScientific [[deprecated ("Since Stroika v3.0d23 use eDefaultFloat instead")]],

        /**
         * If its good enuf for C, its good enuf for me ;-). Seriously - sensible default.
         */
        eDEFAULT = eDefaultFloat,

        Stroika_Define_Enum_Bounds (eDefaultFloat, eAutomaticScientific)
    };
    using FloatFormatType::eAutomaticScientific;
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    using FloatFormatType::eDefaultFloat;
    using FloatFormatType::eFixedPoint;
    using FloatFormatType::eFixedPointWithWhitespaceTrimmed;
    using FloatFormatType::eScientific;
    using FloatFormatType::eScientificWithWhitespaceTrimmed;
    using FloatFormatType::eStandard;

    /**
     *  These are options for ToString (floating point) - Characters::ToString (f, options)
     *
     *  ToString uses the locale specified by FloatToStringOptions, but defaults to the "C" locale.
     *
     *  By default (eDefaultFloat) it writes no needless trailing zeros; see FloatFormatType.
     *
     *  NaN and infinity are written as the standard library writes them - by default "nan", "inf" or "-inf" (a NaN may be "-nan") -
     *  which ParseFloat reads back.
     */
    struct FloatToStringOptions {
    public:
        /**
         * Default is to use the C-locale
         *  \note - if ios_base::fmtflags are specified, these REPLACE - not merged - with
         *          basic ios flags
         */
        constexpr FloatToStringOptions () = default;
        constexpr FloatToStringOptions (Common::PredefinedLocale p);
        FloatToStringOptions (const locale& l);
        constexpr FloatToStringOptions (ios_base::fmtflags fmtFlags);
        constexpr FloatToStringOptions (SignificantFigures precision);
        constexpr FloatToStringOptions (FloatFormatType floatFormat);
        DISABLE_COMPILER_MSC_WARNING_START (4996)
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        [[deprecated ("Since Stroika v3.0d23 use appropriate FloatFormatType 'trim' variant instead")]] constexpr FloatToStringOptions (TrimTrailingZerosType trimTrailingZeros);
        DISABLE_COMPILER_MSC_WARNING_END (4996)
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        constexpr FloatToStringOptions (const FloatToStringOptions& b1, const FloatToStringOptions& b2);
        template <typename... ARGS>
        constexpr FloatToStringOptions (const FloatToStringOptions& b1, const FloatToStringOptions& b2, ARGS&&... args);

    public:
        constexpr optional<SignificantFigures> GetSignificantFigures () const;

    public:
        [[deprecated ("Since Stroika v3.0d23 use FloatFormatType 'trim' variant instead")]]
        constexpr optional<bool> GetTrimTrailingZeros () const;

    public:
        /**
         *  \brief return the selected locale object
         * 
         *  \note before Stroika v3.0d1, this returned optional, and for the case of locale::classic, it retuned none
         */
        nonvirtual locale GetUseLocale () const;

    public:
        /**
         *  \brief return true if locale used is locale::classic() - the 'C' locale; mostly used as optimization/special case
         */
        nonvirtual bool GetUsingLocaleClassic () const;

    public:
        constexpr optional<FloatFormatType> GetFloatFormat () const;

    public:
        constexpr optional<ios_base::fmtflags> GetIOSFmtFlags () const;

    public:
        [[deprecated ("Since Stroika v3.0d23 use FloatFormatType 'trim' variant instead")]]
        static constexpr bool kDefaultTrimTrailingZeros{true};

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        optional<SignificantFigures> fSignificantFigures_;
        optional<ios_base::fmtflags> fFmtFlags_;
        bool                         fUseCurrentLocale_{false}; // dynamically calculated current locale
        optional<locale>             fUseLocale_;               // if missing, use locale::classic (unless fUseCurrentLocale_)
        optional<bool>               fTrimTrailingZeros_;       // keep while deprecated
        optional<FloatFormatType>    fFloatFormat_;
    };

    /**
     *  \brief How ToString writes an integer: by default in base 10 - what ParseInteger reads by default
     *
     *  Writing says exactly what to produce, so the matching IntegerParseOptions reads it back: the same fRadix,
     *  and fAcceptRadixPrefix for fEmitRadixPrefix.
     *
     *  \par Example Usage
     *      \code
     *          ToString (255, IntegerToStringOptions{.fRadix = 16});                                                  // "ff"
     *          ToString (-26, IntegerToStringOptions{.fRadix = 16, .fEmitRadixPrefix = true, .fUpperCase = true});   // "-0X1A"
     *      \endcode
     *
     *  \note Not here yet - to add when a caller needs one: a minimum width or zero padding; digit grouping or separators (1,000).
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
         *  Also accept this character between two digits - a separator people use to make long numbers readable: 1'000'000 (C++),
         *  1_000_000 (Python, Rust), or a locale's grouping, 1,000,000 (its thousands separator: use_facet<numpunct<wchar_t>> (l).thousands_sep ()).
         *
         *  Only between two digits - not first, last, or twice together; the sizes of the groups are not checked.
         */
        optional<Character> fDigitSeparator;

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
     *          Number::Parse<double> ("1,5", {.fLocale = locale{"de_DE.UTF-8"}});                              // 1.5 - its decimal point is a comma
     *          Number::Parse<double> ("1.000,5", {.fLocale = locale{"de_DE.UTF-8"}, .fDigitSeparator = '.'});  // 1000.5 - and its grouping a dot
     *          Number::Parse<double> ("1_000.5", {.fDigitSeparator = '_'});                                    // 1000.5
     *      \endcode
     *
     *  \note Not here yet - to add when a caller needs one: refusing nan and inf as text (accepted now, as ToFloat accepted them).
     */
    struct FloatParseOptions {
        /**
         *  The locale whose decimal point the text uses - the "C" locale ('.') by default, so the same text parses the same in every
         *  program, whatever its global locale. Common::eUseCurrentLocale means the global locale when Parse is called. A '.' is not
         *  the decimal point in a locale whose decimal point is something else (it may be fDigitSeparator).
         */
        Common::LocaleSpecifier fLocale{Common::PredefinedLocale::eUseCLocale};

        /**
         *  Also accept this character between two digits - as IntegerParseOptions::fDigitSeparator: 1_000.5, or a locale's grouping,
         *  1,000.5 (its thousands separator: use_facet<numpunct<wchar_t>> (l).thousands_sep ()).
         */
        optional<Character> fDigitSeparator;

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
