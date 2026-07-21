/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_FloatConversion_h_
#define _Stroika_Foundation_Characters_FloatConversion_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <ios>
#include <locale>
#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"

/**
 * TODO:
 *      @todo   REWRITE much of the backend impl, once 3.0 alpha or beta starts. I think API here OK, but impl is weak (and probably slow).
 * 
 *      @todo   ToFloat code needs OPTIONS optional argument, to support locales etc.
 * 
 *      @todo   Then maybe we can lose ios::format_flags option (maybe keep as ARG, but just grab these fields). Maybe name OK as is, but
 *              just add option for "FIXEDWIDTH", and keep idea of changeing backended arg for ios_flags...
 */

namespace Stroika::Foundation::Characters::FloatConversion {

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

    enum class PredefinedLocale {
        /**
         *  Use the hardwired 'C' locale (not exactly a real locale in the sense of localization. But a good choice
         *  for the 'locale' to use for non-localized code (e.g. much serverside code).
         */
        eUseCLocale,

        /**
         *  \note - this selects the current locale at the time the preference is used, whereas
         *          in Stroika v2.1, it used the current locale at the time the preference object was created.
         */
        eUseCurrentLocale
    };
    using PredefinedLocale::eUseCLocale;
    using PredefinedLocale::eUseCurrentLocale;

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
     *
     * TODO:
     *      @todo   rewrite with  https://en.cppreference.com/w/cpp/utility/to_chars

     */
    struct SignificantFigures {
    public:
        using RepType = unsigned int; // maybe use uint16_t?

    public:
        /**
         *  Flag indicating full precision (see Precision class docs for explanation) - max_digits10
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
         *  Return the used precision. This will be the value specified in the Precision constructor, or
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
         *  Internally treat fSignificantFigures_ as meaning kFullPrecision
         */
        optional<RepType> fSignificantFigures_{6};
    };

    using Precision [[deprecated ("Since Stroika v3.0d23 use SignificantFigures instead")]] =
        SignificantFigures; // for backward compatibility - but maybe should be removed in future

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
     *  These are options for the FloatConversion::ToString () function
     *
     *  FloatConversion::ToString uses the locale specified by ToStringOptions, but defaults to
     *  the "C" locale.
     *
     *  This prints and trims any trailing zeros (after the decimal point - fTrimTrailingZeros -
     *  by default.
     *
     *  Float2String () maps NAN values to the string "NAN", and negative infinite values to "-INF", and positive infinite
     *  values to "INF" (ignoring case).
     *      @see http://en.cppreference.com/w/cpp/string/byte/strtof
     */
    struct ToStringOptions {
    public:
        /**
         * Default is to use use C-locale
         *  \note - if ios_base::fmtflags are specified, these REPLACE - not merged - with
         *          basic ios flags
         */
        constexpr ToStringOptions () = default;
        constexpr ToStringOptions (PredefinedLocale p);
        ToStringOptions (const locale& l);
        constexpr ToStringOptions (ios_base::fmtflags fmtFlags);
        constexpr ToStringOptions (SignificantFigures precision);
        constexpr ToStringOptions (FloatFormatType floatFormat);
        DISABLE_COMPILER_MSC_WARNING_START (4996)
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        [[deprecated ("Since Stroika v3.0d23 use appropriate FloatFormatType 'trim' variant instead")]] constexpr ToStringOptions (TrimTrailingZerosType trimTrailingZeros);
        DISABLE_COMPILER_MSC_WARNING_END (4996)
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        constexpr ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2);
        template <typename... ARGS>
        constexpr ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2, ARGS&&... args);

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
     *  ToString converts a floating point number to a string, controlled by parameterized options. 
     *
     *  @see ToStringOptions
     *
     *  ToString () maps NAN values to the string "NAN", and negative infinite values to "-INF", and positive infinite
     *  values to "INF" (note NAN/INF are case insensitive).
     *      @see http://en.cppreference.com/w/cpp/string/byte/strtof
     * 
     *  The supported type values for FLOAT_TYPE are:
     *      o   float
     *      o   double
     *      o   long double
     *
     *  The supported type values for RESULT_TYPE are:
     *      o   String
     *      o   string
     *      o   wstring
     *      o           ... but this could sensibly be extended in the future
     */
    template <Common::IAnyOf<String, string, wstring> STRING_TYPE = String, floating_point FLOAT_TYPE = float>
    STRING_TYPE ToString (FLOAT_TYPE f, const ToStringOptions& options = {});

    /**
     *  ToFloat all overloads:
     *      Convert the given decimal-format floating point string to an float, double, or long double.
     *
     *      ToFloat will return nan () if no valid parse (for example, -1.#INF000000000000 is,
     *      invalid and returns nan, despite the fact that this is often emitted by the MSFT sprintf() for inf values).
     *
     *      The overloads taking string or const char* arguments Require() that the input is ASCII ('C' locale required/assumed).
     *      (@todo revisit this point --LGP 2022-12-28)
     *
     *      If the argument value is too large or too small to fit in 'T' (ERANGE) - then the value will be
     *      pinned to -numeric_limits<T>::infinity () or numeric_limits<T>::infinity ().
     *
     *      If the input string is INF or INFINITY (with an optional +/- prefix) - the returned
     *      value will be the appropriate version of infinity.
     *
     *      If the argument is the string "NAN", a quiet NAN will be returned. If the string -INF or -INFINITY,
     *      a negative infinite float will be returned, and if INF or INFINITY is passed, a positive infinite
     *      value will be returned:
     *          @see http://en.cppreference.com/w/cpp/string/byte/strtof
     *
     *      @todo TBD/TOCHANGE if using strtod or from_chars - about to add OPTIONS PARAM to decide
     *      For now - tries both
     *
     *  ToFloat (no remainder parameter):
     *      The argument should be pre-trimmed (whitespace). If there is any leading or trailing garbage (even whitespace)
     *      this function will return nan() (**note - unlike overload with 'remainder' arg**).
     *
     *  ToFloat (with remainder parameter):
     *      Logically a simple wrapper on std::wcstof, std::wcstod, std::wcstold - except using String class, and returns the
     *      unused portion of the string in the REQUIRED remainder OUT parameter.
     * 
     *      This means it ALLOWS leading whitespace (skipped). And it allows junk at the end (remainder parameter filled in with what).
     * 
     *  \note SEE http://stroika-bugs.sophists.com/browse/STK-748
     *        We will PROBABLY change this API to take a ToFloatOptions parameter to handle proper locale/conversions of strings to numbers
     *        but so far I've not been able to get any of that working, so ignore for now...
     *
     *  \pre start <= end; for overloads with start/end, and must point to valid string in that range
     *  \pre remainder != nullptr
     * 
     *      // @todo redo all these with some concept to make it shorter - like ISCOVNERTIBLE TO STRING
     *
     *  
     * \note when called with CHAR_T=char, we REQUIRE the argument string is ALL ASCII
     */
    template <floating_point T = double, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    T ToFloat (span<const CHAR_T> s);
    template <floating_point T = double, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    T ToFloat (span<const CHAR_T> s, typename span<const CHAR_T>::iterator* remainder);
    template <floating_point T = double, typename STRINGISH_ARG>
    T ToFloat (STRINGISH_ARG&& s)
        requires (IConvertibleToString<STRINGISH_ARG> or is_convertible_v<STRINGISH_ARG, std::string>);
    template <floating_point T = double>
    T ToFloat (const String& s, String* remainder);

}

namespace Stroika::Foundation::Characters {
    using FloatConversion::ToFloat;
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FloatConversion.inl"

#endif /*_Stroika_Foundation_Characters_FloatConversion_h_*/
