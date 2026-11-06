/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"

#include "Number.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

/*
 ********************************************************************************
 ******************************* Number::Precision ******************************
 ********************************************************************************
 */
String Characters::Number::Precision::ToString () const
{
    StringBuilder sb;
    if (IsShortestRoundTrip ()) {
        sb << "ShortestRoundTrip"sv;
    }
    else if (auto s = GetSignificantDigits ()) {
        sb << "SignificantDigits:{"sv << s->fMin << "..."sv << s->fMax << "}"sv;
    }
    else if (auto f = GetFractionDigits ()) {
        sb << "Fraction:{"sv << f->fMin << "..."sv << f->fMax << "}"sv;
    }
    return sb;
}

/*
 ********************************************************************************
 ************************** Number::SignificantFigures **************************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (4996)
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
String Characters::Number::SignificantFigures::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (fSignificantFigures_) {
        sb << "SignificantFigures:"sv << *fSignificantFigures_;
    }
    else {
        sb << "FULL"sv;
    }
    sb << "}"sv;
    return sb;
}
DISABLE_COMPILER_MSC_WARNING_END (4996)
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

/*
 ********************************************************************************
 ****************************** Number::Punctuation *****************************
 ********************************************************************************
 */
String Characters::Number::Punctuation::ToString () const
{
    StringBuilder sb;
    sb << "{Decimal-Point: '"sv << fDecimalPoint << "'"sv;
    if (fDigitSeparator) {
        sb << ", Digit-Separator: '"sv << *fDigitSeparator << "', Group-Size: "sv << fGroupSize;
        if (fSecondaryGroupSize != 0) {
            sb << ", Secondary-Group-Size: "sv << fSecondaryGroupSize;
        }
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************* Number::FloatToStringOptions *************************
 ********************************************************************************
 */
String Characters::Number::FloatToStringOptions::ToString () const
{
    StringBuilder sb;
    sb << "{Notation: "sv;
    switch (fNotation) {
        case Notation::eGeneral:
            sb << "General"sv;
            break;
        case Notation::ePlain:
            sb << "Plain"sv;
            break;
        case Notation::eScientific:
            sb << "Scientific"sv;
            break;
    }
    sb << ", Precision: "sv << fPrecision.ToString () << ", Punctuation: "sv;
    if (const Punctuation* p = get_if<Punctuation> (&fPunctuation)) {
        sb << p->ToString ();
    }
    else {
        sb << (get<Common::PredefinedLocale> (fPunctuation) == Common::eUseCurrentLocale ? "current locale's"sv : "C"sv);
    }
    sb << "}"sv;
    return sb;
}
