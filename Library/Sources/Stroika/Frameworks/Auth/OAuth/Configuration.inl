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
            {"applicationID", &ClientConfiguration::fApplicationID},
            {"redirectURLs", &ClientConfiguration::fRedirectURLs},
        });
        return mapper;
    }();

}
