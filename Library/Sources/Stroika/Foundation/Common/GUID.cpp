/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cinttypes>

#include "../Characters/Format.h"
#include "../DataExchange/CheckedConverter.h"

#include "GUID.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::Characters;

using Characters::String_Constant;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#define G32 "%8" SCNx32
#define G16 "%4" SCNx16
#define G8 "%2" SCNx8

/*
 ********************************************************************************
 *********************************** Common::GUID *******************************
 ********************************************************************************
 */
Common::GUID::GUID (const string& src)
{
    // Allow on parsing EITHER {} delimited, or not
    DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
    int nchars = -1;
    int nfields =
        sscanf (src.c_str (), "{" G32 "-" G16 "-" G16 "-" G8 G8 "-" G8 G8 G8 G8 G8 G8 "}%n",
                &Data1, &Data2, &Data3,
                &Data4[0], &Data4[1], &Data4[2], &Data4[3], &Data4[4], &Data4[5], &Data4[6], &Data4[7],
                &nchars);
    if (nfields != 11 || nchars != 38) {
        nchars = -1;
        nfields =
            sscanf (src.c_str (), G32 "-" G16 "-" G16 "-" G8 G8 "-" G8 G8 G8 G8 G8 G8 "%n",
                    &Data1, &Data2, &Data3,
                    &Data4[0], &Data4[1], &Data4[2], &Data4[3], &Data4[4], &Data4[5], &Data4[6], &Data4[7],
                    &nchars);
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
    if (nfields != 11 and nchars != 36) {
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }
}

Common::GUID::GUID (const Characters::String& src)
    : GUID (DataExchange::CheckedConverter<string, DataExchange::ASCII, Characters::String> (src))
{
}

Characters::String Common::GUID::ToString () const
{
    return Characters::Format (L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                               Data1, Data2, Data3,
                               Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]);
}
