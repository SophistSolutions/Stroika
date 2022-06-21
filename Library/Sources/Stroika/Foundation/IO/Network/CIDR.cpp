/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/String2Int.h"
#include "../../Characters/StringBuilder.h"
#include "../../Debug/Trace.h"
#include "../../Execution/Exceptions.h"
#include "../../Math/Common.h"

#include "CIDR.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

/*
 ********************************************************************************
 ******************************** Network::CIDR *********************************
 ********************************************************************************
 */
CIDR::CIDR (const InternetAddress& internetAddress, unsigned int significantBits)
    : fBaseAddress_{internetAddress.KeepSignifcantBits (significantBits)}
    , fSignificantBits_{significantBits}
{
}

namespace {
    CIDR read_ (const String& cidrNotation, InternetAddress::AddressFamily addressFamily)
    {
        if (auto i = cidrNotation.RFind ('/')) {
            InternetAddress ia{cidrNotation.SubString (0, *i), addressFamily};
            unsigned int    nBits = Characters::String2Int<unsigned int> (cidrNotation.SubString (*i + 1));
            if (not ia.GetAddressSize ().has_value ()) [[UNLIKELY_ATTR]] {
                static const Execution::RuntimeErrorException k_ = Execution::RuntimeErrorException{L"CIDR format exception: cannot use CIDR notation with that type of internet address"sv};
                Execution::Throw (k_);
            }
            if (*ia.GetAddressSize () * 8 < nBits) [[UNLIKELY_ATTR]] {
                static const Execution::RuntimeErrorException k_ = Execution::RuntimeErrorException{L"CIDR format exception: number of significant bits too large"sv};
                Execution::Throw (k_);
            }
            return CIDR{ia, nBits};
        }
        static const Execution::RuntimeErrorException k_ = Execution::RuntimeErrorException{L"CIDR format exception: doesn't contain a / character"sv};
        Execution::Throw (k_);
    }
}

CIDR::CIDR (const String& cidrNotation, InternetAddress::AddressFamily addressFamily)
    : CIDR{read_ (cidrNotation, addressFamily)}
{
}

template <>
String CIDR::As<String> () const
{
    return fBaseAddress_.As<String> () + L"/"sv + Characters::ToString ((int)fSignificantBits_);
}

String Network::CIDR::ToString () const
{
    return Characters::ToString (fBaseAddress_) + L"/"sv + Characters::ToString ((int)fSignificantBits_);
}

Traversal::DiscreteRange<InternetAddress> Network::CIDR::GetRange () const
{
    Require (fBaseAddress_.GetAddressSize ().has_value ());
    size_t offset = *fBaseAddress_.GetAddressSize () * 8 - fSignificantBits_;
    return Traversal::DiscreteRange<InternetAddress>{fBaseAddress_, fBaseAddress_.PinLowOrderBitsToMax (static_cast<unsigned int> (offset))};
}
