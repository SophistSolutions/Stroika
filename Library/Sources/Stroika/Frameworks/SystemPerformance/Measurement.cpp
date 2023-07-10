/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/StringBuilder.h"

#include "Measurement.h"

using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

/*
 ********************************************************************************
 ****************** SystemPerformance::Instrument::Measurement ******************
 ********************************************************************************
 */
String Measurement::ToString () const
{
    StringBuilder sb;
    sb += "{";
    sb += "Type: " + Characters::ToString (fType) + ", ";
    sb += "Value: " + Characters::ToString (fValue);
    sb += "}";
    return sb.str ();
}
