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
    sb << "{"sv;
    sb << "Type: "sv << Characters::ToString (fType) << ", "sv;
    sb << "Value: "sv << Characters::ToString (fValue);
    sb << "}"sv;
    return sb.str ();
}
