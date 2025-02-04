/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Common/StdCompat.h"
#include "Stroika/Foundation/Containers/SortedSet.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"

#include "Configuration.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
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
    if (openid_configuration_uri) {
        sb << ", openid_configuration_uri: " << openid_configuration_uri;
    }
    if (auth_uri) {
        sb << ", auth_uri: " << auth_uri;
    }
    if (token_uri) {
        sb << ", token_uri: " << token_uri;
    }
    if (userinfo_endpoint) {
        sb << ", userinfo_endpoint: " << userinfo_endpoint;
    }
    if (auth_provider_x509_cert_url) {
        sb << ", auth_provider_x509_cert_url: " << auth_provider_x509_cert_url;
    }
    sb << "}"sv;
    return sb;
}

ProviderConfiguration ProviderConfiguration::FetchAdditionsFromOpenIDConfigurationURI () const
{
    URI  configURI  = ValueOfOrThrow (openid_configuration_uri, RuntimeErrorException{"no openid_configuration_uri"sv});
    auto connection = IO::Network::Transfer::Connection::New ();
    try {
        IO::Network::Transfer::Response r = connection.GET (configURI);
        // empirical - not sure where documented/defined
        struct openid_configuration_ {
            optional<URI> issuer;
            optional<URI> authorization_endpoint;
            optional<URI> device_authorization_endpoint;
            optional<URI> token_endpoint;
            optional<URI> userinfo_endpoint;
            // ..more ignored for now
            optional<URI> jwks_uri;
        };
        ObjectVariantMapper mapper;
        mapper.AddCommonType<URI> ();
        mapper.AddCommonType<optional<URI>> ();
        mapper.AddClass<openid_configuration_> ({
            {"issuer", &openid_configuration_::issuer},
            {"authorization_endpoint", &openid_configuration_::authorization_endpoint},
            {"device_authorization_endpoint", &openid_configuration_::device_authorization_endpoint},
            {"token_endpoint", &openid_configuration_::token_endpoint},
            {"userinfo_endpoint", &openid_configuration_::userinfo_endpoint},
            {"jwks_uri", &openid_configuration_::jwks_uri},
        });
        openid_configuration_ cfgRead = mapper.ToObject<openid_configuration_> (DataExchange::Variant::JSON::Reader{}.Read (r.GetData ()));
        ProviderConfiguration result  = *this;
        if (cfgRead.authorization_endpoint) {
            result.auth_uri = *cfgRead.authorization_endpoint;
        }
        if (cfgRead.token_endpoint) {
            result.token_uri = *cfgRead.token_endpoint;
        }
        if (cfgRead.userinfo_endpoint) {
            result.userinfo_endpoint = *cfgRead.userinfo_endpoint;
        }
        if (cfgRead.jwks_uri) {
            result.auth_provider_x509_cert_url = *cfgRead.jwks_uri;
        }
        return result;
    }
    catch (...) {
        DbgTrace ("Fetcher::Token: exception={}"_f, current_exception ());
        Execution::ReThrow ();
    }
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

auto ClientConfiguration::operator<=> (const ClientConfiguration& rhs) const
{
#if qCompilerAndStdLib_tie_trick_spaceship_impl_Buggy
    if (auto o = fProvider <=> rhs.fProvider; o != strong_ordering::equal) {
        return o;
    }
    if (auto o = fApplicationID <=> rhs.fApplicationID; o != strong_ordering::equal) {
        return o;
    }
    if (auto o = fRedirectURLs <=> rhs.fRedirectURLs; o != strong_ordering::equal) {
        return o;
    }
    if (auto o = Common::StdCompat::compare_three_way{}(fClientSecret, rhs.fClientSecret); o != strong_ordering::equal) {
        return o;
    }
    return SortedSet<String>{fScopes} <=> SortedSet<String>{rhs.fScopes};
#else
    strong_ordering tmp = tie (fProvider, fApplicationID, fRedirectURLs, fClientSecret) <=>
                          std::tie (rhs.fProvider, rhs.fApplicationID, rhs.fRedirectURLs, rhs.fClientSecret);
    if (tmp == strong_ordering::equal) {
        return SortedSet<String>{fScopes} <=> SortedSet<String>{rhs.fScopes};
    }
    return tmp;
#endif
}