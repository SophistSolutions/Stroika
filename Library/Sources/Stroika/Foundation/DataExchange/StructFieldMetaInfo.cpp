/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
    Characters::StringBuilder sb;
    sb << "{"sv;
    sb << "TypeInfo:"sv << Characters::ToString (fTypeInfo_) << ", "sv;
    sb << ",PointerToMember:"sv << Characters::ToString (fPTR2MEM_);
    sb << "}"sv;
    return sb.str ();
}
