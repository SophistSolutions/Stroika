/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Frameworks::Auth::OAuth {

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
            {"provider", &ClientConfiguration::fProvider},
            {"applicationID", &ClientConfiguration::fApplicationID},
            {"redirectURLs", &ClientConfiguration::fRedirectURLs},
        });
        return mapper;
    }();

    /*
     ********************************************************************************
     ******************** Auth::OAuth::kDefaultProviderConfigurations ***************
     ********************************************************************************
     */
#if 1
    // @todo debug why initializer list not working???
    inline const ProvidersConfigurations kDefaultProviderConfigurations = [] () {
        ProvidersConfigurations r;
        r += ProviderConfiguration{.name                        = "google"sv,
                                   .auth_uri                    = "https://accounts.google.com/o/oauth2/auth"sv,
                                   .token_uri                   = "https://oauth2.googleapis.com/token"sv,
                                   .auth_provider_x509_cert_url = "https://www.googleapis.com/oauth2/v1/certs"sv};
        r += ProviderConfiguration{.name = "twitter"sv};
        r += ProviderConfiguration{.name = "facebook"sv};
        return r;
    }();
#else
    inline const ProvidersConfigurations kDefaultProviderConfigurations{
        ProviderConfiguration{.name                        = "google"sv,
                              .auth_uri                    = "https://accounts.google.com/o/oauth2/auth"sv,
                              .token_uri                   = "https://oauth2.googleapis.com/token"sv,
                              .auth_provider_x509_cert_url = "https://www.googleapis.com/oauth2/v1/certs"sv},
        ProviderConfiguration{.name = "twitter"sv},
        ProviderConfiguration{.name = "facebook"sv},
    };
#endif

}
