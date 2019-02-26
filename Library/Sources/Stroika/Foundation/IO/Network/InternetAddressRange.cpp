/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "InternetAddressRange.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

/*
 ********************************************************************************
 ********************* IO::Network::InternetAddressRange ************************
 ********************************************************************************
 */
const InternetAddress Network::Private_::InternetAddressRangeTraits_::kLowerBound;
const InternetAddress Network::Private_::InternetAddressRangeTraits_::kUpperBound = InternetAddress{array<uint8_t, 16>{
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
}};

InternetAddress IO::Network::Private_::InternetAddressRangeTraits_::GetNext (InternetAddress n)
{
    return n == kUpperBound ? n : n.Offset (1);
}
