/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cinttypes>
#include <random>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/DataExchange/CheckedConverter.h"
#include "Stroika/Foundation/DataExchange/DefaultSerializer.h"
#include "Stroika/Foundation/Memory/BLOB.h"

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
Common::GUID Common::GUID::mk_ (const string& src)
{
    Common::GUID r;

    // Allow on parsing EITHER {} delimited, or not
    DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
    int nchars  = -1;
    int nFields = ::sscanf (src.c_str (), "{" G32 "-" G16 "-" G16 "-" G8 G8 "-" G8 G8 G8 G8 G8 G8 "}%n", &r.Data1, &r.Data2, &r.Data3,
                            &r.Data4[0], &r.Data4[1], &r.Data4[2], &r.Data4[3], &r.Data4[4], &r.Data4[5], &r.Data4[6], &r.Data4[7], &nchars);
    if (nFields != 11 || nchars != 38) {
        nchars  = -1;
        nFields = ::sscanf (src.c_str (), G32 "-" G16 "-" G16 "-" G8 G8 "-" G8 G8 G8 G8 G8 G8 "%n", &r.Data1, &r.Data2, &r.Data3,
                            &r.Data4[0], &r.Data4[1], &r.Data4[2], &r.Data4[3], &r.Data4[4], &r.Data4[5], &r.Data4[6], &r.Data4[7], &nchars);
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
    if (nFields != 11 and nchars != 36) {
        static const auto kException_ = DataExchange::BadFormatException{"Badly formatted GUID"sv};
        Execution::Throw (kException_);
    }
    return r;
}

Common::GUID::GUID (const Memory::BLOB& src)
{
    if (src.size () != 16) {
        static const auto kException_ = DataExchange::BadFormatException{"GUID from BLOB must be 16 bytes"sv};
        Execution::Throw (kException_);
    }
    ::memcpy (this, src.begin (), 16);
}

Characters::String Common::GUID::ToString () const
{
    return Characters::Format ("{:08x}-{:04x}-{:04x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}"_f, Data1, Data2, Data3, Data4[0],
                               Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]);
}

Common::GUID::operator Memory::BLOB () const
{
    Assert ((end () - begin ()) == 16);
    return Memory::BLOB{begin (), end ()};
}

Common::GUID Common::GUID::GenerateNew () noexcept
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
