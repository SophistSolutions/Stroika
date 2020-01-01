/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
    return String{L"{"} +
           L"fOffset: " + Characters::ToString (fOffset) + L", " +
           L"fTypeInfo: " + Characters::ToString (fTypeInfo) +
           L"}";
}
