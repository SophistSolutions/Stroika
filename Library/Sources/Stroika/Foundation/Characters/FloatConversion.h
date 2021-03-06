/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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

namespace Stroika::Foundation::Characters {

    /**
     *  These are options for the Float2Stgring () function
     *
     *  Float2String uses the locale specified by Float2StringOptions, but defaults to
     *  the "C" locale.
     *
     *  Precision (here) is defined to be the number of significant digits (including before and after decimal point).
     *
     *  This prints and trims any trailing zeros (after the decimal point - fTrimTrailingZeros -
     *  by default.
     *
     *  Float2String () maps NAN values to the string "NAN", and negative infinite values to "-INF", and positive infinite
     *  values to "INF".
     *      @see http://en.cppreference.com/w/cpp/string/byte/strtof
     */
    struct Float2StringOptions {
        /**
         * Control needless trailing zeros. For example, 3.000 instead of 3, or 4.2000 versus 4.2. Sometimes desirable (to show precision).
         * But often not.
         */
        enum class TrimTrailingZerosType {
            eTrim,
            eDontTrim,

            Stroika_Define_Enum_Bounds (eTrim, eDontTrim)
        };
        static constexpr TrimTrailingZerosType eTrimZeros     = TrimTrailingZerosType::eTrim;
        static constexpr TrimTrailingZerosType eDontTrimZeros = TrimTrailingZerosType::eDontTrim;

        /**
         */
        enum class UseCLocale { eUseCLocale };
        static constexpr UseCLocale eUseCLocale = UseCLocale::eUseCLocale;

        /**
         */
        enum class UseCurrentLocale { eUseCurrentLocale };
        static constexpr UseCurrentLocale eUseCurrentLocale = UseCurrentLocale::eUseCurrentLocale;

        /**
         */
        struct Precision {
            constexpr Precision (unsigned int p);
            unsigned int fPrecision;
        };
        // From http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf,
        // init (basic_streambuf...) initializes precision to 6
        // Stroika need not maintain that default here, but it seems a sensible one...
        //
        //static constexpr Precision kDefaultPrecision{6};

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
        static constexpr FloatFormatType eScientific          = FloatFormatType::eScientific;
        static constexpr FloatFormatType eDefaultFloat        = FloatFormatType::eDefaultFloat;
        static constexpr FloatFormatType eFixedPoint          = FloatFormatType::eFixedPoint;
        static constexpr FloatFormatType eAutomaticScientific = FloatFormatType::eAutomatic;

        /**
         * Default is to use use C-locale
         *  \note - if ios_base::fmtflags are specified, these REPLACE - not merged - with
         *          basic ios flags
         */
        constexpr Float2StringOptions () = default;
        constexpr Float2StringOptions (UseCLocale); // same as default
        Float2StringOptions (UseCurrentLocale);
        Float2StringOptions (const locale& l);
        constexpr Float2StringOptions (ios_base::fmtflags fmtFlags);
        constexpr Float2StringOptions (Precision precision);
        constexpr Float2StringOptions (FloatFormatType floatFormat);
        constexpr Float2StringOptions (TrimTrailingZerosType trimTrailingZeros);
        Float2StringOptions (const Float2StringOptions& b1, const Float2StringOptions& b2);
        template <typename... ARGS>
        Float2StringOptions (const Float2StringOptions& b1, const Float2StringOptions& b2, ARGS&&... args);

    public:
        nonvirtual optional<unsigned int> GetPrecision () const;

    public:
        nonvirtual optional<bool> GetTrimTrailingZeros () const;

    public:
        nonvirtual optional<locale> GetUseLocale () const;

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
        optional<locale>             fUseLocale_; // if missing, use locale::classic
        optional<bool>               fTrimTrailingZeros_;
        optional<FloatFormatType>    fFloatFormat_;
    };

    /**
     *  Float2String converts a floating point number to a string, controlled by paramtererized options. 
     *
     *  @see Float2StringOptions
     *
     *  Float2String () maps NAN values to the string "NAN", and negative infinite values to "-INF", and positive infinite
     *  values to "INF".
     *      @see http://en.cppreference.com/w/cpp/string/byte/strtof
     */
    String Float2String (float f, const Float2StringOptions& options = Float2StringOptions{});
    String Float2String (double f, const Float2StringOptions& options = Float2StringOptions{});
    String Float2String (long double f, const Float2StringOptions& options = Float2StringOptions{});

    /**
     *  String2Float/1:
     *
     *  Convert the given decimal-format floating point string to an float,
     *  double, or long double.
     *
     *  String2Float will return nan () if no valid parse (for example, -1.#INF000000000000 is,
     *  invalid and returns nan, despite the fact that this is often emitted by the MSFT sprintf() for inf values).
     *
     *  If the argument value is too large or too small to fit in 'T' (ERANGE) - then the value will be
     *  pinned to -numeric_limits<T>::infinity () or numeric_limits<T>::infinity ().
     *
     *  If the input string is INF or INFINITY (with an optional +/- prefix) - the returned
     *  value will be the appropriate verison of infinity.
     *
     *  The argument should be pre-trimmed. If there is any leading or trailing garbage (even whitespace)
     *  this function will return nan().
     *
     *  If the argument is the string "NAN", a quiet NAN will be returned. If the string -INF or -INFINITY,
     *  a negative infinite float will be returned, and if INF or INFINITY is passed, a positive infinite
     *  value will be returned:
     *      @see http://en.cppreference.com/w/cpp/string/byte/strtof
     *
     *  @see strtod(), or @see wcstod (), or String2Float/2 (). This is a simple wrapper on strtod() / wcstod () /
     *  strtold, etc... except that it returns nan() on invalid data, instead of zero.
     *
     *  String2Float/2:
     *
     *  Simple wrapper on std::wcstof, std::wcstod, std::wcstold - except using String class, and returns the
     *  unused portion of the string in the REQUIRED remainder OUT parameter.
     *
     *  \note UNLIKE String2Float/1, this SKIPS leading spaces, and is OK with trailing extra characters.
     *
     *  \req remainder != nullptr
     *
     */
    template <typename T = double>
    T String2Float (const String& s);
    template <typename T = double>
    T String2Float (const String& s, String* remainder);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FloatConversion.inl"

#endif /*_Stroika_Foundation_Characters_FloatConversion_h_*/
