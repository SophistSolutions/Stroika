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
 ************************** Number::SignificantFigures **************************
 ********************************************************************************
 */
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
    sb << "{"sv;
    if (fSignificantFigures_) {
        sb << "SignificantFigures:"sv << *fSignificantFigures_ << ","sv;
    }
    if (fFmtFlags_) {
        sb << "Fmt-Flags:"sv << Characters::ToString ((int)*fFmtFlags_, ios_base::hex) << ","sv;
    }
    if (fPunctuation_) {
        if (const Punctuation* p = get_if<Punctuation> (&*fPunctuation_)) {
            sb << "Punctuation:"sv << p->ToString () << ","sv;
        }
        else {
            sb << "Punctuation: "sv
               << (get<Common::PredefinedLocale> (*fPunctuation_) == Common::eUseCurrentLocale ? "current locale's"sv : "C"sv) << ","sv;
        }
    }
    if (fTrimTrailingZeros_) {
        sb << "Trim-Trailing-Zeros: "sv << *fTrimTrailingZeros_ << ","sv;
    }
    if (fFloatFormat_) {
        sb << "Scientific-Notation: "sv << (int)*fFloatFormat_ << ","sv;
    }
    sb << "}"sv;
    return sb;
}
