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
        mapper.AddClass<ProviderConfiguration> ({
            {"provider"sv, &ProviderConfiguration::name},
            {"openid_configuration_uri"sv, &ProviderConfiguration::openid_configuration_uri},
            {"auth_uri"sv, &ProviderConfiguration::auth_uri},
            {"token_uri"sv, &ProviderConfiguration::token_uri},
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
        mapper.AddCommonType<RedirectURLType> ();
        mapper.AddCommonType<Sequence<RedirectURLType>> ();
        mapper.AddClass<ClientConfiguration> ({
            {"provider"sv, &ClientConfiguration::fProvider},
            {"applicationID"sv, &ClientConfiguration::fApplicationID},
            {"redirectURLs"sv, &ClientConfiguration::fRedirectURLs},
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
                              .auth_provider_x509_cert_url = "https://www.googleapis.com/oauth2/v1/certs"sv},
        ProviderConfiguration{.name = "twitter"sv, .auth_uri = "https://api.twitter.com/oauth/authorize"sv, .token_uri = "https://api.twitter.com/oauth/request_token"sv},
        ProviderConfiguration{.name = "facebook"sv, .openid_configuration_uri = "https://www.facebook.com/.well-known/openid-configuration"sv},
    };

}
