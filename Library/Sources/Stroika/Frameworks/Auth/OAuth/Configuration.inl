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
        r += ProviderConfiguration{.name = "google"sv};
        r += ProviderConfiguration{.name = "twitter"sv};
        r += ProviderConfiguration{.name = "facebook"sv};
        return r;
    }();
#else
    inline const ProvidersConfigurations kDefaultProviderConfigurations{
        ProviderConfiguration{.name = "google"sv},
        ProviderConfiguration{.name = "twitter"sv},
        ProviderConfiguration{.name = "facebook"sv},
    };
#endif

}
