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

_DeprecatedFile_ ("DEPRECATED FloatConversion.h in v3.0d26 - use ToString.h (Characters::ToString), ParseNumber.h (ParseFloat) and "
                  "NumberFormat.h");

namespace Stroika::Foundation::Characters::FloatConversion {

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
    using PredefinedLocale [[deprecated ("Since Stroika v3.0d26 use Characters::PredefinedLocale (NumberFormat.h)")]] = Characters::PredefinedLocale;
    [[deprecated ("Since Stroika v3.0d26 use Characters::eUseCLocale (NumberFormat.h)")]] inline constexpr Characters::PredefinedLocale eUseCLocale =
        Characters::eUseCLocale;
    [[deprecated ("Since Stroika v3.0d26 use Characters::eUseCurrentLocale (NumberFormat.h)")]] inline constexpr Characters::PredefinedLocale eUseCurrentLocale =
        Characters::eUseCurrentLocale;
    using SignificantFigures [[deprecated ("Since Stroika v3.0d26 use Characters::SignificantFigures (NumberFormat.h)")]] = Characters::SignificantFigures;
    using Precision [[deprecated ("Since Stroika v3.0d23 use SignificantFigures instead")]] = Characters::SignificantFigures;
    using FloatFormatType [[deprecated ("Since Stroika v3.0d26 use Characters::FloatFormatType (NumberFormat.h)")]] = Characters::FloatFormatType;
    [[deprecated ("Since Stroika v3.0d26 use Characters::eDefaultFloat (NumberFormat.h)")]] inline constexpr Characters::FloatFormatType eDefaultFloat =
        Characters::eDefaultFloat;
    [[deprecated ("Since Stroika v3.0d26 use Characters::eFixedPoint (NumberFormat.h)")]] inline constexpr Characters::FloatFormatType eFixedPoint =
        Characters::eFixedPoint;
    [[deprecated ("Since Stroika v3.0d26 use Characters::eFixedPointWithWhitespaceTrimmed (NumberFormat.h)")]] inline constexpr Characters::FloatFormatType eFixedPointWithWhitespaceTrimmed =
        Characters::eFixedPointWithWhitespaceTrimmed;
    [[deprecated ("Since Stroika v3.0d26 use Characters::eScientific (NumberFormat.h)")]] inline constexpr Characters::FloatFormatType eScientific =
        Characters::eScientific;
    [[deprecated ("Since Stroika v3.0d26 use Characters::eScientificWithWhitespaceTrimmed (NumberFormat.h)")]] inline constexpr Characters::FloatFormatType eScientificWithWhitespaceTrimmed =
        Characters::eScientificWithWhitespaceTrimmed;
    [[deprecated ("Since Stroika v3.0d26 use Characters::eStandard (NumberFormat.h)")]] inline constexpr Characters::FloatFormatType eStandard =
        Characters::eStandard;
    using ToStringOptions [[deprecated ("Since Stroika v3.0d26 use Characters::FloatToStringOptions (NumberFormat.h)")]] = Characters::FloatToStringOptions;

    template <Common::IAnyOf<String, string, wstring> STRING_TYPE = String, floating_point FLOAT_TYPE = float>
    [[deprecated ("Since Stroika v3.0d26 use Characters::ToString (f, FloatToStringOptions) - .AsASCII () or .As<wstring> () for a "
                  "string or wstring")]] STRING_TYPE
    ToString (FLOAT_TYPE f, const Characters::FloatToStringOptions& options = {});

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
