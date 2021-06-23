/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cinttypes>
#include <random>

#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"
#include "../DataExchange/CheckedConverter.h"
#include "../DataExchange/DefaultSerializer.h"
#include "../Memory/BLOB.h"

#include "GUID.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::Characters;

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
        ::sscanf (src.c_str (), "{" G32 "-" G16 "-" G16 "-" G8 G8 "-" G8 G8 G8 G8 G8 G8 "}%n",
                  &Data1, &Data2, &Data3,
                  &Data4[0], &Data4[1], &Data4[2], &Data4[3], &Data4[4], &Data4[5], &Data4[6], &Data4[7],
                  &nchars);
    if (nfields != 11 || nchars != 38) {
        nchars = -1;
        nfields =
            ::sscanf (src.c_str (), G32 "-" G16 "-" G16 "-" G8 G8 "-" G8 G8 G8 G8 G8 G8 "%n",
                      &Data1, &Data2, &Data3,
                      &Data4[0], &Data4[1], &Data4[2], &Data4[3], &Data4[4], &Data4[5], &Data4[6], &Data4[7],
                      &nchars);
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
    if (nfields != 11 and nchars != 36) {
        Execution::Throw (DataExchange::BadFormatException{L"Badly formatted GUID"sv});
    }
}

Common::GUID::GUID (const Memory::BLOB& src)
{
    if (src.size () != 16) {
        Execution::Throw (DataExchange::BadFormatException{L"GUID from BLOB must be 16 bytes"sv});
    }
    ::memcpy (this, src.begin (), 16);
}

Common::GUID::GUID (const Characters::String& src)
    : GUID{src.AsASCII ()}
{
}

template <>
Characters::String Common::GUID::As () const
{
    return Characters::Format (L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                               Data1, Data2, Data3,
                               Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]);
}

template <>
string Common::GUID::As () const
{
    return Characters::CString::Format ("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                                        Data1, Data2, Data3,
                                        Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]);
}

Characters::String Common::GUID::ToString () const
{
    return Characters::Format (L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                               Data1, Data2, Data3,
                               Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]);
}

template <>
Memory::BLOB Common::GUID::As () const
{
    Assert ((end () - begin ()) == 16);
    return Memory::BLOB{begin (), end ()};
}

Common::GUID::operator Memory::BLOB () const
{
    Assert ((end () - begin ()) == 16);
    return Memory::BLOB{begin (), end ()};
}

Common::GUID Common::GUID::GenerateNew ()
{
    array<uint8_t, 16>         randomData;
    random_device              rd;
    mt19937                    gen{rd ()}; //Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<> uniformDist{0, 255};
    for (size_t i = 0; i < 16; ++i) {
        randomData[i] = static_cast<uint8_t> (uniformDist (gen));
    }
    return GUID{randomData};
}

/*
 ********************************************************************************
 ************** DataExchange::DefaultSerializer<Common::GUID> *******************
 ********************************************************************************
 */
Memory::BLOB DataExchange::DefaultSerializer<Common::GUID>::operator() (const Common::GUID& arg) const
{
    return Memory::BLOB{arg.begin (), arg.end ()};
}
