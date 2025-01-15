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
 *
 */

namespace Stroika::Frameworks::Auth::OAuth {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Containers::Sequence;

    using DataExchange::ObjectVariantMapper;

    // DOCUMENT - GIVE EXAMPLES - WHERE TO FIND IN GOOGLE CLOUD UI TO CONFIGURE - ETC... AND WHAT I CAN REMEMBER OF AZURE LIKEWISE
    // see these (IMPERFECT ) docs for example - https://developers.google.com/identity/protocols/oauth2/javascript-implicit-flow

    /**
     *  \brief sometimes called ClientID, and sometimes called applicationID
     *  
     *  \note though this is often a GUID, it cannot be assumed to be a GUID (google for example, doesn't use GUIDs).
     */
    using ApplicationIDType = String;

    /**
    * often require things like no #/fragments
     */
    using RedirectURLType = IO::Network::URI;

    /**
     *  \brief Track configuration data about stuff that differentiates different
     *         OAuth providers - what URLs to use, base url, relative off that urls for login/upgrade token/refresh etc.
     *         ALL very prelim at this stage.
     *
     *      see javascript frameworks - for doing auth2 - convert token to access token etc...
     *      stuff to fetch 'keys' like I vaguely remember from openid... to validate jwts...\
     */
    class ProviderConfiguration {
    public:
        /* IO::Network::URI forThisOp;
        IO::Network::URI forTahtOp;*/
        String name;
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
    using ProvidersConfigurations = Containers::KeyedCollection<ProviderConfiguration, String, Private_::My_Traits_>;

    /**
     *  a predefined set of configurations, but you may need to update/roll your own, as this could get out of date.
     */
    extern const ProvidersConfigurations kDefaultProviderConfigurations;

    /**
     *  \note logically, we want to aggregate ProviderConfiguration inside ClientConfiguration, but
            since it can generally be static and unchanged, we keep it separate, and just link up/reference by 'name'
     */
    struct ClientConfiguration {
        String                    fProvider; // refers to some element of ProvidersConfigurationType
        ApplicationIDType         fApplicationID;
        Sequence<RedirectURLType> fRedirectURLs;

        // @todo maybe add 'scopes'

        static const ObjectVariantMapper kMapper;

        String ToString () const;
    };

    /**
     *    {
     *       { "google", "003...", [] },
     *       { "apple", "003...", [] },
     *       { "twitter", "003...", [] },
     *       { "facebook", "003...", [] },
     *  }
     */
    using ClientConfigurations = Containers::Sequence<ClientConfiguration>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Configuration.inl"

#endif /*_Stroika_Frameworks_Auth_OAuth_Configuration_h_*/
