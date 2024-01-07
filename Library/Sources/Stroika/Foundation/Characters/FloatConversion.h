/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_FloatConversion_h_
#define _Stroika_Foundation_Characters_FloatConversion_h_ 1

#include "../StroikaPreComp.h"

#include <ios>
#include <locale>
#include <optional>

#include "../Configuration/Common.h"

#include "String.h"

/**
 * TODO:
 *      @todo   REDO the FromFloat code with span/modern C++ the way I did the ToFloat code
 * 
 *      @todo   ToFloat code needs OPTIONS optional argument, to support locales etc.
 * 
 *      @todo   Consider moving notion of Precision into Math module. And if so - and maybe otherwise - make
 *              correct.
 *
 *              using PrecisionType = uint16_t;
 *
 *      @todo   Consider augmenting the Float2StringOptions::Precision support with Float2StringOptions::MantisaLength
 *              which is the number of decimals after the decimal point.
 *
 *      @todo   Then maybe we can lose ios::format_flags option (maybe keep as ARG, but just grab these fields). Maybe name OK as is, but
 *              just add option for "FIXEDWIDTH", and keep idea of changeing backended arg for ios_flags...
 */

namespace Stroika::Foundation::Characters::FloatConversion {

    /**
     * Control needless trailing zeros. For example, 3.000 instead of 3, or 4.2000 versus 4.2. Sometimes desirable (to show precision).
     * But often not.
     */
    enum class TrimTrailingZerosType {
        eTrim,
        eDontTrim,

        Stroika_Define_Enum_Bounds (eTrim, eDontTrim)
    };
    constexpr TrimTrailingZerosType eTrimZeros     = TrimTrailingZerosType::eTrim;
    constexpr TrimTrailingZerosType eDontTrimZeros = TrimTrailingZerosType::eDontTrim;

    enum class PredefinedLocale {
        eUseCLocale,
        eUseCurrentLocale
    };

    /**
     */
    constexpr PredefinedLocale eUseCLocale = PredefinedLocale::eUseCLocale;

    /**
     *  \note - this selects the current locale at the time the prefence is used, whereas
     *          in Stroika v2.1, it used the current locale at the time the preference object was created.
     */
    constexpr PredefinedLocale eUseCurrentLocale = PredefinedLocale::eUseCurrentLocale;

    /**
     *  Precision (here) is defined to be the number of significant digits (including before and after decimal point).
     */
    struct Precision {
        constexpr Precision (unsigned int p);
        unsigned int fPrecision;
    };

    /**
     * Automatic picks based on the precision and the number used, so for example, 0.0000001
     * will show as '1e-7', but 4 will show as '4'
     *
     *      eScientific corresponds to ios_base::scientific
     *      eFixedPoint corresponds to ios_base::fixed
     *      eDefaultFloat corresponds to unsetf (floatfield) - which may be different than scientific or fixed point
     */
    enum class FloatFormatType {
        eScientific,
        eDefaultFloat,
        eFixedPoint,
        eAutomatic,

        eDEFAULT = eDefaultFloat,

        Stroika_Define_Enum_Bounds (eScientific, eAutomatic)
    };
    constexpr FloatFormatType eScientific          = FloatFormatType::eScientific;
    constexpr FloatFormatType eDefaultFloat        = FloatFormatType::eDefaultFloat;
    constexpr FloatFormatType eFixedPoint          = FloatFormatType::eFixedPoint;
    constexpr FloatFormatType eAutomaticScientific = FloatFormatType::eAutomatic;

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
         * From http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf,
         * init (basic_streambuf...) initializes precision to 6
         * Stroika need not maintain that default here, but it seems a sensible one...
         */
        static const Precision kDefaultPrecision;

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
        constexpr ToStringOptions (Precision precision);
        constexpr ToStringOptions (FloatFormatType floatFormat);
        constexpr ToStringOptions (TrimTrailingZerosType trimTrailingZeros);
        ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2);
        template <typename... ARGS>
        ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2, ARGS&&... args);

    public:
        nonvirtual optional<unsigned int> GetPrecision () const;

    public:
        nonvirtual optional<bool> GetTrimTrailingZeros () const;

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
        nonvirtual optional<FloatFormatType> GetFloatFormat () const;

    public:
        nonvirtual optional<ios_base::fmtflags> GetIOSFmtFlags () const;

    public:
        static constexpr bool kDefaultTrimTrailingZeros{true};

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        optional<unsigned int>       fPrecision_;
        optional<ios_base::fmtflags> fFmtFlags_;
        bool                         fUseCurrentLocale_{false}; // dynamically calculated current locale
        optional<locale>             fUseLocale_;               // if missing, use locale::classic (unless fUseCurrentLocale_)
        optional<bool>               fTrimTrailingZeros_;
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
    template <typename STRING_TYPE = String, floating_point FLOAT_TYPE = float>
    STRING_TYPE ToString (FLOAT_TYPE f, const ToStringOptions& options = {});

    template <>
    String ToString (float f, const ToStringOptions& options);
    template <>
    String ToString (double f, const ToStringOptions& options);
    template <>
    String ToString (long double f, const ToStringOptions& options);
    template <>
    string ToString (float f, const ToStringOptions& options);
    template <>
    string ToString (double f, const ToStringOptions& options);
    template <>
    string ToString (long double f, const ToStringOptions& options);
    template <>
    wstring ToString (float f, const ToStringOptions& options);
    template <>
    wstring ToString (double f, const ToStringOptions& options);
    template <>
    wstring ToString (long double f, const ToStringOptions& options);

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
     *      value will be the appropriate verison of infinity.
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
     *  \note SEE https://stroika.atlassian.net/browse/STK-748
     *        We will PROBABLY change this API to take a ToFloatOptions parameter to handle proper locale/conversions of strings to numbers
     *        but so far I've not been able to get any of that working, so ignore for now...
     *
     *  \req start <= end; for overloads with start/end, and must point to valid string in that range
     *  \req remainder != nullptr
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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FloatConversion.inl"

#endif /*_Stroika_Foundation_Characters_FloatConversion_h_*/
