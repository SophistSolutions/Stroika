/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
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
        sb << "allowCredentials"sv << fAllowCredentials;
    }
    if (fAccessControlMaxAge) {
        sb << ", accessControlMaxAge"sv << fAccessControlMaxAge;
    }
    if (fAllowedOrigins) {
        sb << ", allowedOrigins"sv << fAllowedOrigins;
    }
    if (fAllowedHeaders) {
        sb << ", allowedHeaders"sv << fAllowedHeaders;
    }
    sb << "}"sv;
    return sb;
}
