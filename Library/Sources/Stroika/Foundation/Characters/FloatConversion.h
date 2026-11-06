/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_FloatConversion_h_
#define _Stroika_Foundation_Characters_FloatConversion_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <ios>
#include <locale>
#include <optional>

#include "Stroika/Foundation/Characters/Number.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"

_DeprecatedFile_ ("DEPRECATED FloatConversion.h in v3.0d26 - use ToString.h (Characters::ToString) and Number.h (Number::Parse, and "
                  "the types)");

namespace Stroika::Foundation::Characters::FloatConversion {

    DISABLE_COMPILER_MSC_WARNING_START (4996)
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    using Characters::eDontTrimZeros;
    using Characters::eTrimZeros;
    using Characters::TrimTrailingZerosType;
    using Characters::Number::eAutomaticScientific;
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    using PredefinedLocale [[deprecated ("Since Stroika v3.0d26 use Common::PredefinedLocale (Common/Locale.h)")]] = Common::PredefinedLocale;
    [[deprecated ("Since Stroika v3.0d26 use Common::eUseCLocale (Common/Locale.h)")]] inline constexpr Common::PredefinedLocale eUseCLocale =
        Common::eUseCLocale;
    [[deprecated ("Since Stroika v3.0d26 use Common::eUseCurrentLocale (Common/Locale.h)")]] inline constexpr Common::PredefinedLocale eUseCurrentLocale =
        Common::eUseCurrentLocale;
    using SignificantFigures [[deprecated ("Since Stroika v3.0d26 use Characters::Number::SignificantFigures (Number.h)")]] =
        Characters::Number::SignificantFigures;
    using Precision [[deprecated ("Since Stroika v3.0d23 use SignificantFigures instead")]] = Characters::Number::SignificantFigures;
    using FloatFormatType [[deprecated ("Since Stroika v3.0d26 use Characters::Number::FloatFormatType (Number.h)")]] = Characters::Number::FloatFormatType;
    [[deprecated ("Since Stroika v3.0d26 use Characters::Number::eDefaultFloat (Number.h)")]] inline constexpr Characters::Number::FloatFormatType eDefaultFloat =
        Characters::Number::eDefaultFloat;
    [[deprecated ("Since Stroika v3.0d26 use Characters::Number::eFixedPoint (Number.h)")]] inline constexpr Characters::Number::FloatFormatType eFixedPoint =
        Characters::Number::eFixedPoint;
    [[deprecated (
        "Since Stroika v3.0d26 use Characters::Number::eFixedPointWithTrailingZerosTrimmed (Number.h)")]] inline constexpr Characters::Number::FloatFormatType eFixedPointWithWhitespaceTrimmed =
        Characters::Number::eFixedPointWithTrailingZerosTrimmed;
    [[deprecated ("Since Stroika v3.0d26 use Characters::Number::eScientific (Number.h)")]] inline constexpr Characters::Number::FloatFormatType eScientific =
        Characters::Number::eScientific;
    [[deprecated (
        "Since Stroika v3.0d26 use Characters::Number::eScientificWithTrailingZerosTrimmed (Number.h)")]] inline constexpr Characters::Number::FloatFormatType eScientificWithWhitespaceTrimmed =
        Characters::Number::eScientificWithTrailingZerosTrimmed;
    [[deprecated ("Since Stroika v3.0d26 use Characters::Number::eStandard (Number.h)")]] inline constexpr Characters::Number::FloatFormatType eStandard =
        Characters::Number::eStandard;
    using ToStringOptions [[deprecated ("Since Stroika v3.0d26 use Characters::Number::FloatToStringOptions (Number.h)")]] =
        Characters::Number::FloatToStringOptions;

    template <Common::IAnyOf<String, string, wstring> STRING_TYPE = String, floating_point FLOAT_TYPE = float>
    [[deprecated ("Since Stroika v3.0d26 use Characters::ToString (f, FloatToStringOptions) - .AsASCII () or .As<wstring> () for a "
                  "string or wstring")]] STRING_TYPE
    ToString (FLOAT_TYPE f, const Characters::Number::FloatToStringOptions& options = {});

    template <floating_point T = double, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    [[deprecated ("Since Stroika v3.0d26 use Number::Parse<T> (s) (Number.h) - .value_or (NAN) for this lenient behavior")]] T
    ToFloat (span<const CHAR_T> s);
    template <floating_point T = double, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    [[deprecated ("Since Stroika v3.0d26 use Number::Parse<T> (s) (Number.h) - .value_or (NAN) for this lenient behavior")]] T
    ToFloat (span<const CHAR_T> s, typename span<const CHAR_T>::iterator* remainder);
    template <floating_point T = double, typename STRINGISH_ARG>
    [[deprecated ("Since Stroika v3.0d26 use Number::Parse<T> (s) (Number.h) - .value_or (NAN) for this lenient behavior")]] T ToFloat (STRINGISH_ARG&& s)
        requires (IConvertibleToString<STRINGISH_ARG> or is_convertible_v<STRINGISH_ARG, std::string>);
    template <floating_point T = double>
    [[deprecated ("Since Stroika v3.0d26 use Number::Parse<T> (s) (Number.h) - .value_or (NAN) for this lenient behavior")]] T
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
