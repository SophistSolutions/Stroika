/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/StroikaVersion.h"
#include "Stroika/Foundation/Common/SystemConfiguration.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/TimingTrace.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "WSImpl.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Samples::HTMLUI;

/*
 ********************************************************************************
 ************************* HTMLUI::WebServiceIdentity ***************************
 ********************************************************************************
 */
String WebServiceIdentity::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (fBearerToken) {
        sb << ", bearer-token: "sv << fBearerToken;
    }
    sb << "}"sv;
    return sb;
}