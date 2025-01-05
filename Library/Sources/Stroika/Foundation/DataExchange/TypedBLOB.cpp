/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"

#include "TypedBLOB.h"

using namespace Stroika::Foundation;
using namespace Characters;
using namespace Stroika::Foundation::DataExchange;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

String TypedBLOB::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Type: " << fType << ","sv;
    sb << "Data: " << fData;
    sb << "}"sv;
    return sb;
}