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
    Characters::StringBuilder sb;
    sb << "{";
    sb << "TypeInfo:" << Characters::ToString (fTypeInfo);
    sb << ",PointerToMember:" << Characters::ToString (fPTR2MEM_);
    sb << "}";
    return sb.str ();
}
