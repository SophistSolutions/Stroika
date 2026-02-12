/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <charconv>
#include <cstdarg>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <sstream>

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Valgrind.h"
#include "Stroika/Foundation/Math/Common.h"

#include "FloatConversion.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;

/*
 ********************************************************************************
 ********************* FloatConversion::SignificantFigures **********************
 ********************************************************************************
 */
String FloatConversion::SignificantFigures::ToString () const
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
 ******************** FloatConversion::ToStringOptions **************************
 ********************************************************************************
 */
String FloatConversion::ToStringOptions::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (fSignificantFigures_) {
        sb << "SignificantFigures:"sv << *fSignificantFigures_ << ","sv;
    }
    if (fFmtFlags_) {
        sb << "Fmt-Flags:"sv << Characters::ToString ((int)*fFmtFlags_, ios_base::hex) << ","sv;
    }
    if (fUseLocale_) {
        sb << "Use-Locale"sv << String::FromNarrowSDKString (fUseLocale_->name ()) << ","sv;
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
