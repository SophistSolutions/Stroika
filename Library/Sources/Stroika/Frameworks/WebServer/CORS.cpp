/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
    sb += "{";
    if (fAllowCredentials) {
        sb += "Allow-Credentials" + Characters::ToString (fAllowCredentials) + ", ";
    }
    if (fAccessControlMaxAge) {
        sb += "Access-Control-Max-Age" + Characters::ToString (fAccessControlMaxAge) + ", ";
    }
    if (fAllowedOrigins) {
        sb += "Allowed-Origins" + Characters::ToString (fAllowedOrigins) + ", ";
    }
    if (fAllowedHeaders) {
        sb += "Allowed-Headers" + Characters::ToString (fAllowedHeaders) + ", ";
    }
    sb += "}";
    return sb.str ();
}
