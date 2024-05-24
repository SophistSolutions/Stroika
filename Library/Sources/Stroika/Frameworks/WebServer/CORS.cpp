/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"

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
    sb << "{"sv;
    if (fAllowCredentials) {
        sb << "Allow-Credentials"sv << fAllowCredentials << ", "sv;
    }
    if (fAccessControlMaxAge) {
        sb << "Access-Control-Max-Age"sv << fAccessControlMaxAge << ", "sv;
    }
    if (fAllowedOrigins) {
        sb << "Allowed-Origins"sv << fAllowedOrigins << ", "sv;
    }
    if (fAllowedHeaders) {
        sb << "Allowed-Headers"sv << fAllowedHeaders;
    }
    sb << "}"sv;
    return sb;
}
