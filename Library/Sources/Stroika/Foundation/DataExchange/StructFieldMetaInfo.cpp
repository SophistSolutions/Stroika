/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"

#include "Stroika/Foundation/DataExchange/StructFieldMetaInfo.h"

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
    Characters::StringBuilder sb;
    sb << "{"sv;
    sb << "TypeInfo:"sv << fTypeInfo_ << ", "sv;
    sb << "PointerToMember:"sv << fPTR2MEM_;
    sb << "}"sv;
    return sb;
}
