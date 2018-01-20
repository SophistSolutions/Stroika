/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
const InternetAddress Network::Private_::InternetAddressRangeTraits_::kUpperBound;

InternetAddress IO::Network::Private_::InternetAddressRangeTraits_::GetNext (InternetAddress n)
{
    AssertNotImplemented ();
    return n; //tmphack
}
