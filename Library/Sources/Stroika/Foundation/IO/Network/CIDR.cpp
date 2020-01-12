/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/String2Int.h"
#include "../../Characters/StringBuilder.h"
#include "../../Characters/String_Constant.h"
#include "../../Debug/Trace.h"
#include "../../Execution/Exceptions.h"
#include "../../Math/Common.h"
#include "../../Memory/SmallStackBuffer.h"

#include "CIDR.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

/*
 ********************************************************************************
 ******************************** Network::CIDR *********************************
 ********************************************************************************
 */
namespace {
    InternetAddress MakeAddressBySignificantBits_ (const InternetAddress& internetAddress, unsigned int significantBits)
    {
        // Mask address by significant bits
        vector<uint8_t> r;
        unsigned int    sigBitsLeft = significantBits;
        for (uint8_t b : internetAddress.As<vector<uint8_t>> ()) {
            if (sigBitsLeft >= 8) {
                r.push_back (b);
                sigBitsLeft -= 8;
            }
            else {
                unsigned int topBit = 8;
                unsigned int botBit = topBit - sigBitsLeft;
                r.push_back (BitSubstring<uint8_t> (b, botBit, topBit) << botBit);
                sigBitsLeft = 0;
            }
        }
        return InternetAddress (r, internetAddress.GetAddressFamily ());
    }
}
CIDR::CIDR (const InternetAddress& internetAddress, unsigned int significantBits)
    : fBaseAddress_ (MakeAddressBySignificantBits_ (internetAddress, significantBits))
    , fSignificantBits_ (significantBits)
{
}

namespace {
    CIDR read_ (const String& cidrNotation, InternetAddress::AddressFamily addressFamily)
    {
        if (auto i = cidrNotation.RFind ('/')) {
            InternetAddress ia{cidrNotation.SubString (0, *i), addressFamily};
            unsigned int    nBits = Characters::String2Int<unsigned int> (cidrNotation.SubString (*i + 1));
            if (not ia.GetAddressSize ().has_value ())
                [[UNLIKELY_ATTR]]
                {
                    Execution::Throw (Execution::RuntimeErrorException (L"CIDR format exception: cannot use CIDR notation with that type of internet address"sv));
                }
            if (*ia.GetAddressSize () * 8 < nBits)
                [[UNLIKELY_ATTR]]
                {
                    Execution::Throw (Execution::RuntimeErrorException (L"CIDR format exception: number of significant bits too large"sv));
                }
            return CIDR{ia, nBits};
        }
        Execution::Throw (Execution::RuntimeErrorException (L"CIDR format exception: doesn't contain a / character"sv));
    }
}

CIDR::CIDR (const String& cidrNotation, InternetAddress::AddressFamily addressFamily)
    : CIDR (read_ (cidrNotation, addressFamily))
{
}

String Network::CIDR::ToString () const
{
    return Characters::ToString (fBaseAddress_) + L"/" + Characters::ToString ((int)fSignificantBits_);
}

IO::Network::InternetAddressRange Network::CIDR::GetRange () const
{
    Require (fBaseAddress_.GetAddressSize ().has_value ());
    size_t               offset = *fBaseAddress_.GetAddressSize () * 8 - fSignificantBits_;
    InternetAddressRange result{fBaseAddress_, fBaseAddress_.PinLowOrderBitsToMax (static_cast<unsigned int> (offset))};
    return result;
}
