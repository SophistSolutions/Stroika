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
 ************************ Auth::OAuth::ProviderConfiguration ********************
 ********************************************************************************
 */
String ProviderConfiguration::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "name: " << name;
    sb << ", openid_configuration_uri: " << openid_configuration_uri;
    sb << ", auth_uri: " << auth_uri;
    sb << ", token_uri: " << token_uri;
    sb << ", auth_provider_x509_cert_url: " << auth_provider_x509_cert_url;
    sb << "}"sv;
    return sb;
}

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
    sb << ", scopes: " << fScopes;
    sb << ", clientSecret: " << fClientSecret;
    sb << "}"sv;
    return sb;
}
