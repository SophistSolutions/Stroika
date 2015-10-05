/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_FloatConversion_h_
#define _Stroika_Foundation_Characters_FloatConversion_h_    1

#include    "../StroikaPreComp.h"

#include    <ios>
#include    <locale>

#include    "../Configuration/Common.h"
#include    "../Memory/Optional.h"

#include    "String.h"



/**
 * TODO:
 *
 *      @todo   Consider moving notion of Precision into Math module. And if so - and maybe otherwise - make
 *              correct.
 *
 *              using PrecisionType = uint16_t;
 *
 *      @todo   Consider augmenting the Float2StringOptions::Precision support with Float2StringOptions::MantisaLength
 *              which is the number of decimals after the decimal point.
 *
 *      @todo   Consider more Float2StringOptions CTOR overloads (e.g. to specify precision and fTrimTrailingZeros
 *              at the same time).
 *
 *      @todo   Consdier if String2Float should take a locale, or
 *              always using C/currnet locale. For the most part - I find it best to use the C locale.
 *              But DOCUMENT in all cases!!! And maybe implement variants...
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /**
             *  Note - Float2String uses the locale specified by Float2StringOptions, but defaults to
             *  the "C" locale.
             *
             *  Precision (here) is defined to be the number of significant digits (including before and after decimal point).
             *
             *  This prints and trims any trailing zeros (after the decimal point - fTrimTrailingZeros -
             *  by deafult.
             *
             *  Float2String () maps NAN valeus to the string "NAN", and negative infinite values to "-INF", and positive infinite
             *  values to "INF".
             *      @see http://en.cppreference.com/w/cpp/string/byte/strtof
             */
            struct  Float2StringOptions {
                enum UseCLocale { eUseCLocale };
                enum UseCurrentLocale { eUseCurrentLocale };
                struct Precision {
                    Precision (unsigned int p);
                    unsigned int fPrecision;
                };
                Float2StringOptions () = default;
                Float2StringOptions (UseCLocale);   // same as default
                Float2StringOptions (UseCurrentLocale);
                Float2StringOptions (const std::locale& l);
                Float2StringOptions (std::ios_base::fmtflags fmtFlags);
                Float2StringOptions (Precision precision);

                Memory::Optional<unsigned int>              fPrecision;
                Memory::Optional<std::ios_base::fmtflags>   fFmtFlags;
                Memory::Optional<std::locale>               fUseLocale; // if empty, use C-locale
                bool                                        fTrimTrailingZeros { true };
            };
            String Float2String (float f, const Float2StringOptions& options = Float2StringOptions ());
            String Float2String (double f, const Float2StringOptions& options = Float2StringOptions ());
            String Float2String (long double f, const Float2StringOptions& options = Float2StringOptions ());


            /**
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
             *  @see strtod(), or @see wcstod (). This is a simple wrapper on strtod() / wcstod () /
             *  strtold, etc... except that it returns nan() on invalid data, instead of zero.
             *
             *  strtod() /etc are more flexible. This is merely meant to be an often convenient wrapper.
             *  Use strtod etc directly to see if the string parsed properly.
             */
            template    <typename T = double>
            T  String2Float (const String& s);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "FloatConversion.inl"

#endif  /*_Stroika_Foundation_Characters_FloatConversion_h_*/
