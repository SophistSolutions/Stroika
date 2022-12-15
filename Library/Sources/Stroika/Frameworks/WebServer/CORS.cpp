/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/ToString.h"

#include "CORS.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

/*
 ********************************************************************************
 ******************************* WebServer::CORSOptions *************************
 ********************************************************************************
 */
Characters::String CORSOptions::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    if (fAllowCredentials) {
        sb += L"Allow-Credentials" + Characters::ToString (fAllowCredentials) + L", ";
    }
    if (fAccessControlMaxAge) {
        sb += L"Access-Control-Max-Age" + Characters::ToString (fAccessControlMaxAge) + L", ";
    }
    if (fAllowedOrigins) {
        sb += L"Allowed-Origins" + Characters::ToString (fAllowedOrigins) + L", ";
    }
    if (fAllowedHeaders) {
        sb += L"Allowed-Headers" + Characters::ToString (fAllowedHeaders) + L", ";
    }
    sb += L"}";
    return sb.str ();
}
