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
    sb << "{"sv;
    if (fAllowCredentials) {
        sb << "Allow-Credentials"sv << Characters::ToString (fAllowCredentials) << ", "sv;
    }
    if (fAccessControlMaxAge) {
        sb << "Access-Control-Max-Age"sv << Characters::ToString (fAccessControlMaxAge) << ", "sv;
    }
    if (fAllowedOrigins) {
        sb << "Allowed-Origins"sv << Characters::ToString (fAllowedOrigins) << ", "sv;
    }
    if (fAllowedHeaders) {
        sb << "Allowed-Headers"sv << Characters::ToString (fAllowedHeaders);
    }
    sb << "}"sv;
    return sb.str ();
}
