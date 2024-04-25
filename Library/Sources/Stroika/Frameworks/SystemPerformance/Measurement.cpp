/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"

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
    sb << "Type: "sv << fType << ", "sv;
    sb << "Value: "sv << fValue;
    sb << "}"sv;
    return sb.str ();
}
