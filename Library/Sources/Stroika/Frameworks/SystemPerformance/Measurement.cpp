/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/StringBuilder.h"

#include "Measurement.h"

using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

/*
 ********************************************************************************
 ******************** SystemPerformance::Instrument *****************************
 ********************************************************************************
 */

String Measurement::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"fType: " + Characters::ToString (fType) + L", ";
    sb += L"fValue: " + Characters::ToString (fValue);
    sb += L"}";
    return sb.str ();
}
