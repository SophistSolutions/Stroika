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
#include "Stroika/Foundation/Common/Enumeration.h"

_DeprecatedFile_ ("DEPRECATED FloatConversion.h in v3.0d26 - use ToString.h (Characters::ToString) and Number.h (Number::Parse, and "
                  "the types)");

namespace Stroika::Foundation::Characters::FloatConversion {

    DISABLE_COMPILER_MSC_WARNING_START (4996)
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    using Characters::eDontTrimZeros;
    using Characters::eTrimZeros;
    using Characters::TrimTrailingZerosType;
    using PredefinedLocale [[deprecated ("Since Stroika v3.0d26 use Common::PredefinedLocale (Common/Locale.h)")]] = Common::PredefinedLocale;
    [[deprecated ("Since Stroika v3.0d26 use Common::eUseCLocale (Common/Locale.h)")]] inline constexpr Common::PredefinedLocale eUseCLocale =
        Common::eUseCLocale;
    [[deprecated ("Since Stroika v3.0d26 use Common::eUseCurrentLocale (Common/Locale.h)")]] inline constexpr Common::PredefinedLocale eUseCurrentLocale =
        Common::eUseCurrentLocale;
    using SignificantFigures [[deprecated ("Since Stroika v3.0d26 use Characters::Number::Precision (Number.h)")]] = Characters::Number::SignificantFigures;
    using Precision [[deprecated ("Since Stroika v3.0d23 use SignificantFigures instead")]] = Characters::Number::SignificantFigures;

    /**
     *  How a float was written before Stroika v3.0d26, when Number::Notation and Number::Precision replaced it - each value's
     *  equivalent beside it, for a precision n.
     */
    enum class [[deprecated ("Since Stroika v3.0d26 use Characters::Number::Notation and Characters::Number::Precision (Number.h) - "
                             "FloatConversion.h gives each value's equivalent")]] FloatFormatType {
        eDefaultFloat,                                     ///< {eGeneral, MaxSignificantDigits (n)}
        eFixedPointWithoutWhitespaceTrimmed,               ///< {ePlain, FixedFraction (n)}
        eFixedPoint = eFixedPointWithoutWhitespaceTrimmed, ///< {ePlain, FixedFraction (n)}
        eFixedPointWithWhitespaceTrimmed,                  ///< {ePlain, MinMaxFraction (0, n)}
        eScientificWithoutWhitespaceTrimmed,               ///< {eScientific, FixedSignificantDigits (n)}
        eScientific = eScientificWithoutWhitespaceTrimmed, ///< {eScientific, FixedSignificantDigits (n)}
        eScientificWithWhitespaceTrimmed,                  ///< {eScientific, MinMaxSignificantDigits (2, n)}
        eStandard, ///< {ePlain, MaxSignificantDigits (n)} - which rounds the whole part too; eStandard wrote it in full
        eAutomaticScientific [[deprecated ("Since Stroika v3.0d23 use eDefaultFloat instead")]],
        eDEFAULT = eDefaultFloat,
    };
    constexpr Common::EnumBounds<FloatFormatType> EnumBoundsOf (FloatFormatType)
    {
        return {FloatFormatType::eDefaultFloat, FloatFormatType::eAutomaticScientific};
    }
    using FloatFormatType::eAutomaticScientific;
    using FloatFormatType::eDefaultFloat;
    using FloatFormatType::eFixedPoint;
    using FloatFormatType::eFixedPointWithWhitespaceTrimmed;
    using FloatFormatType::eScientific;
    using FloatFormatType::eScientificWithWhitespaceTrimmed;
    using FloatFormatType::eStandard;

    /**
     *  These were the options for ToString (floating point) before Stroika v3.0d26: each constructor gave one setting, and the
     *  combining constructors overlaid them. It converts to the Number::FloatToStringOptions that writes the same - but for its
     *  ios_base::fmtflags, which it ignores.
     */
    struct [[deprecated ("Since Stroika v3.0d26 use Characters::Number::FloatToStringOptions (Number.h) - an aggregate of a Notation, a "
                         "Precision and the punctuation - and its With (...)")]] ToStringOptions {
    public:
        constexpr ToStringOptions () = default;
        constexpr ToStringOptions (Common::PredefinedLocale p);
        ToStringOptions (const locale& l);
        constexpr ToStringOptions (const Number::Punctuation& p);
        constexpr ToStringOptions (ios_base::fmtflags fmtFlags);
        constexpr ToStringOptions (Number::SignificantFigures precision);
        constexpr ToStringOptions (FloatFormatType floatFormat);
        constexpr ToStringOptions (TrimTrailingZerosType trimTrailingZeros);
        constexpr ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2);
        template <typename... ARGS>
        constexpr ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2, ARGS&&... args);

    public:
        /**
         *  \brief The Number::FloatToStringOptions that writes the same, as FloatFormatType says (its ios_base::fmtflags ignored)
         */
        constexpr operator Number::FloatToStringOptions () const;

    public:
        constexpr optional<Number::SignificantFigures> GetSignificantFigures () const;
        constexpr optional<bool>                       GetTrimTrailingZeros () const;
        nonvirtual Number::Punctuation         GetPunctuation () const;
        nonvirtual locale                      GetUseLocale () const;
        nonvirtual bool                        GetUsingLocaleClassic () const;
        constexpr optional<FloatFormatType>    GetFloatFormat () const;
        constexpr optional<ios_base::fmtflags> GetIOSFmtFlags () const;

    public:
        static constexpr bool kDefaultTrimTrailingZeros{true};

    public:
        nonvirtual String ToString () const;

    private:
        optional<Number::SignificantFigures>   fSignificantFigures_;
        optional<ios_base::fmtflags>           fFmtFlags_;
        optional<Number::PunctuationSpecifier> fPunctuation_;
        optional<bool>                         fTrimTrailingZeros_;
        optional<FloatFormatType>              fFloatFormat_;
    };

    template <Common::IAnyOf<String, string, wstring> STRING_TYPE = String, floating_point FLOAT_TYPE = float>
    [[deprecated ("Since Stroika v3.0d26 use Characters::ToString (f, Number::FloatToStringOptions) - .AsASCII () or .As<wstring> () for "
                  "a string or wstring")]] STRING_TYPE
    ToString (FLOAT_TYPE f, const ToStringOptions& options = {});
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

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
