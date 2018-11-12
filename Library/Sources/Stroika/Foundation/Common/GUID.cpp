/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"

#include "GUID.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Common;

using Characters::String_Constant;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 *********************************** Common::GUID *******************************
 ********************************************************************************
 */
Characters::String Common::GUID::ToString () const
{
    return Characters::Format (L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                               Data1, Data2, Data3,
                               Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]);
}
