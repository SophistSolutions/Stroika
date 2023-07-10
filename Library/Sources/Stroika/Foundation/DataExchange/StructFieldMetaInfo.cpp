/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/ToString.h"

#include "StructFieldMetaInfo.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;

/*
 ********************************************************************************
 ********************* DataExchange::StructFieldMetaInfo ************************
 ********************************************************************************
 */
Characters::String StructFieldMetaInfo::ToString () const
{
    return "{"_k + "fOffset: " + Characters::ToString (fOffset) + ", "sv + "fTypeInfo: " + Characters::ToString (fTypeInfo) + "}";
}
