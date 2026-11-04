/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_FloatConversion_h_
#define _Stroika_Foundation_Characters_FloatConversion_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <ios>
#include <locale>
#include <optional>

#include "Stroika/Foundation/Characters/NumberFormat.h"
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

    /*
     *  Since Stroika v3.0d26 these are in Characters (NumberFormat.h), shared by ToString and ParseNumber.h; the names here are aliases.
     */
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    using Characters::eAutomaticScientific;
    using Characters::eDontTrimZeros;
    using Characters::eTrimZeros;
    using Characters::TrimTrailingZerosType;
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    using Characters::eUseCLocale;
    using Characters::eUseCurrentLocale;
    using Characters::PredefinedLocale;
    using Characters::SignificantFigures;
    using Precision [[deprecated ("Since Stroika v3.0d23 use SignificantFigures instead")]] = SignificantFigures;
    using Characters::eDefaultFloat;
    using Characters::eFixedPoint;
    using Characters::eFixedPointWithWhitespaceTrimmed;
    using Characters::eScientific;
    using Characters::eScientificWithWhitespaceTrimmed;
    using Characters::eStandard;
    using Characters::FloatFormatType;
    using ToStringOptions = Characters::FloatToStringOptions;

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
     *  DEPRECATED since Stroika v3.0d26: use ParseFloat (ParseNumber.h) - strict: an error, not NaN or infinity, for text that
     *  is not a number or is too large; ParseFloat<T> (s).value_or (NAN) for this lenient behavior.
     *
     *  \note SEE https://github.com/SophistSolutions/Stroika/issues/882 (STK-748)
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
    [[deprecated ("Since Stroika v3.0d26 use ParseFloat<T> (s) (ParseNumber.h) - .value_or (NAN) for this lenient behavior")]] T
    ToFloat (span<const CHAR_T> s);
    template <floating_point T = double, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    [[deprecated ("Since Stroika v3.0d26 use ParseFloat<T> (s) (ParseNumber.h) - .value_or (NAN) for this lenient behavior")]] T
    ToFloat (span<const CHAR_T> s, typename span<const CHAR_T>::iterator* remainder);
    template <floating_point T = double, typename STRINGISH_ARG>
    [[deprecated ("Since Stroika v3.0d26 use ParseFloat<T> (s) (ParseNumber.h) - .value_or (NAN) for this lenient behavior")]] T ToFloat (STRINGISH_ARG&& s)
        requires (IConvertibleToString<STRINGISH_ARG> or is_convertible_v<STRINGISH_ARG, std::string>);
    template <floating_point T = double>
    [[deprecated ("Since Stroika v3.0d26 use ParseFloat<T> (s) (ParseNumber.h) - .value_or (NAN) for this lenient behavior")]] T
    ToFloat (const String& s, String* remainder);

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
