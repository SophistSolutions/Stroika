/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Auth_OAuth_Configuration_h_
#define _Stroika_Frameworks_Auth_OAuth_Configuration_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/IO/Network/URI.h"

/**
 *  \file
 */

namespace Stroika::Frameworks::Auth::OAuth {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Containers::KeyedCollection;
    using Containers::Sequence;
    using Containers::Set;
    using IO::Network::URI;

    using DataExchange::ObjectVariantMapper;

    /**
     *  Documentation about these concepts:
     *      google  - https://developers.google.com/identity/protocols/oauth2/web-server#httprest
     *              - https://developers.google.com/identity/protocols/oauth2/javascript-implicit-flow
     */

    /**
     *  Documentation where to get/register application IDs
     *      google  - https://console.cloud.google.com/apis/credentials
     *      azure   - https://portal.azure.com/#view/Microsoft_AAD_RegisteredApps/ApplicationsListBlade
     *              - https://learn.microsoft.com/en-us/azure/active-directory-b2c/client-credentials-grant-flow?pivots=b2c-user-flow#app-registration-overview
     */

    /**
     *  \brief sometimes called ClientID, and sometimes called applicationID
     *  
     *  \note though this is often a GUID, it cannot be assumed to be a GUID (google for example, doesn't use GUIDs).
     */
    using ApplicationIDType = String;

    /**
     * often require things like no #/fragments
     */
    using RedirectURLType = URI;

    /**
     *  \brief Track configuration data about stuff that differentiates different
     *         OAuth providers - what URLs to use, base url, relative off that URLs for login/upgrade token/refresh etc.
     *         ALL very prelim at this stage.
     *
     *      see javascript frameworks - for doing auth2 - convert token to access token etc...
     *      stuff to fetch 'keys' like I vaguely remember from openid... to validate JWTs...\
     */
    class ProviderConfiguration {
    public:
        String name;
        URI    openid_configuration_uri;
        URI    auth_uri;
        URI    token_uri;
        URI    auth_provider_x509_cert_url;

        static const ObjectVariantMapper kMapper;

        nonvirtual String ToString () const;
    };

    namespace Private_ {
        using My_Extractor_ = decltype ([] (const ProviderConfiguration& t) -> String { return t.name; });
        using My_Traits_    = Containers::KeyedCollection_DefaultTraits<ProviderConfiguration, String, My_Extractor_>;
    }
    /**
     *  A list of definitions for configurations.
     * 
     *  @todo provide predefined one inside this framework, and allow it to be updated/revised in applications.
     *  REFERENED IMPLICITLY in ClientConfiguration
     */
    using ProvidersConfigurations = KeyedCollection<ProviderConfiguration, String, Private_::My_Traits_>;

    /**
     *  a predefined set of configurations, but you may need to update/roll your own, as this could get out of date.
     */
    extern const ProvidersConfigurations kDefaultProviderConfigurations;

    /**
     *  \note logically, we want to aggregate ProviderConfiguration inside ClientConfiguration, but
     *        since it can generally be static and unchanged, we keep it separate, and just link up/reference by 'name'
     */
    struct ClientConfiguration {
        String fProvider; // refers to some element of ProvidersConfigurationType

        /**
         *  This value is sometimes referred to in OAUTH as ClientID
         */
        ApplicationIDType fApplicationID;

        /**
         *  List of acceptable redirect URLs allowed in (which flows) authorization_code acquisition; only one will be actually used
         *  in the authentication_code request.
         */
        Sequence<RedirectURLType> fRedirectURLs;

        /**
         *  List of acceptable scopes requested. Some subset (often all) will be requested at in the authentication_code request.
         */
        Set<String> fScopes;

        /**
         *  This is used in some places, and must be hidden in others (so optional).
         */
        optional<String> fClientSecret;

        static const ObjectVariantMapper kMapper;

        nonvirtual String ToString () const;
    };

    /**
     *  \par Example Usage
     *      \code
     *          {
     *              { .fProvider = "google", .fApplicationID = "003...", .fRedirectURLs = ["http://localhost"], .fScopes = ["openid"] },
     *              { .fProvider = "apple", .fApplicationID = "003...", .fRedirectURLs = ["http://localhost"], .fScopes = ["openid"] },
     *              { .fProvider = "twitter", .fApplicationID = "003...", .fRedirectURLs = ["http://localhost"], .fScopes = ["openid"] },
     *              { .fProvider = "facebook", .fApplicationID = "003...", .fRedirectURLs = ["http://localhost"], .fScopes = ["openid"] },
     *          }
     *      \endcode
     */
    using ClientConfigurations = Sequence<ClientConfiguration>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Configuration.inl"

#endif /*_Stroika_Frameworks_Auth_OAuth_Configuration_h_*/
