/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"

#include "JSONRPC.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
********************************************************************************
****************************** JSONRPC::Error **********************************
********************************************************************************
*/
nonvirtual String JSONRPC::Error::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "code: "sv << code << " ,"sv;
    sb << "message: "sv << message << " ,"sv;
    if (data) {
        sb << "data: "sv << data;
    }
    sb << "}"sv;
    return sb;
}
