/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Frameworks::Auth::OAuth {

    /*
     ********************************************************************************
     ************************ Auth::OAuth::ProviderConfiguration ********************
     ********************************************************************************
     */
    inline const ObjectVariantMapper ProviderConfiguration::kMapper = [] () {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<URI> ();
        mapper.AddCommonType<optional<URI>> ();
        mapper.AddClass<ProviderConfiguration> ({
            {"provider"sv, &ProviderConfiguration::name},
            {"openid_configuration_uri"sv, &ProviderConfiguration::openid_configuration_uri},
            {"auth_uri"sv, &ProviderConfiguration::auth_uri},
            {"token_uri"sv, &ProviderConfiguration::token_uri},
            {"userinfo_endpoint"sv, &ProviderConfiguration::userinfo_endpoint},
            {"revocation_endpoint"sv, &ProviderConfiguration::revocation_endpoint},
            {"auth_provider_x509_cert_url"sv, &ProviderConfiguration::auth_provider_x509_cert_url},
        });
        return mapper;
    }();

    /*
     ********************************************************************************
     ************************ Auth::OAuth::ClientConfiguration **********************
     ********************************************************************************
     */
    inline const ObjectVariantMapper ClientConfiguration::kMapper = [] () {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<optional<String>> ();
        mapper.AddCommonType<RedirectURLType> ();
        mapper.AddCommonType<Sequence<RedirectURLType>> ();
        mapper.AddCommonType<Set<String>> ();
        mapper.AddClass<ClientConfiguration> ({
            {"provider"sv, &ClientConfiguration::fProvider},
            {"applicationID"sv, &ClientConfiguration::fApplicationID},
            {"redirectURLs"sv, &ClientConfiguration::fRedirectURLs},
            {"scopes"sv, &ClientConfiguration::fScopes},
            {"clientSecret"sv, &ClientConfiguration::fClientSecret},
        });
        return mapper;
    }();

    /*
     ********************************************************************************
     ****************** Auth::OAuth::kDefaultProviderConfigurations *****************
     ********************************************************************************
     */
    inline const ProvidersConfigurations kDefaultProviderConfigurations{
        ProviderConfiguration{.name                        = "google"sv,
                              .openid_configuration_uri    = "https://accounts.google.com/.well-known/openid-configuration"sv,
                              .auth_uri                    = "https://accounts.google.com/o/oauth2/auth"sv,
                              .token_uri                   = "https://oauth2.googleapis.com/token"sv,
                              .userinfo_endpoint           = "https://openidconnect.googleapis.com/v1/userinfo"sv,
                              .revocation_endpoint         = "https://oauth2.googleapis.com/revoke"sv,
                              .auth_provider_x509_cert_url = "https://www.googleapis.com/oauth2/v1/certs"sv},
        ProviderConfiguration{.name = "microsoft"sv,
                              .openid_configuration_uri = "https://login.microsoftonline.com/common/v2.0/.well-known/openid-configuration"sv,
                              .auth_uri                    = "https://login.microsoftonline.com/common/oauth2/v2.0/authorize"sv,
                              .token_uri                   = "https://login.microsoftonline.com/common/oauth2/v2.0/token"sv,
                              .userinfo_endpoint           = "https://graph.microsoft.com/oidc/userinfo"sv,
                              .auth_provider_x509_cert_url = "https://login.microsoftonline.com/common/discovery/v2.0/keys"sv},
        ProviderConfiguration{.name                        = "apple"sv,
                              .openid_configuration_uri    = "https://account.apple.com/.well-known/openid-configuration"sv,
                              .auth_uri                    = "https://appleid.apple.com/auth/authorize"sv,
                              .token_uri                   = "https://appleid.apple.com/auth/token"sv,
                              .auth_provider_x509_cert_url = "https://appleid.apple.com/auth/keys"sv},
        ProviderConfiguration{.name                        = "facebook"sv,
                              .openid_configuration_uri    = "https://www.facebook.com/.well-known/openid-configuration"sv,
                              .auth_uri                    = "https://facebook.com/dialog/oauth/"sv,
                              .auth_provider_x509_cert_url = "https://www.facebook.com/.well-known/oauth/openid/jwks/"sv},
        ProviderConfiguration{.name = "twitter"sv, .auth_uri = "https://api.twitter.com/oauth/authorize"sv, .token_uri = "https://api.twitter.com/oauth/request_token"sv},
    };

}
