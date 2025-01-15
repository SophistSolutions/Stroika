/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"

#include "Configuration.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Auth::OAuth;

/*
 ********************************************************************************
 ************************ Auth::OAuth::ClientConfiguration **********************
 ********************************************************************************
 */
String ClientConfiguration::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "provider: " << fProvider;
    sb << ", applicationID: " << fApplicationID;
    sb << ", redirectURLs: " << fRedirectURLs;
    sb << "}"sv;
    return sb;
}
