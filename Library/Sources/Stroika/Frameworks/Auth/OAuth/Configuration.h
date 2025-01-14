/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Auth_OAuth_Configuration_h_
#define _Stroika_Frameworks_Auth_OAuth_Configuration_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/IO/Network/URI.h"

/**
 *  \file
 *
 */

namespace Stroika::Frameworks::Auth::OAuth {

    using namespace Stroika::Foundation;
    // DOCUMENT - GIVE EXAMPLES - WHERE TO FIND IN GOOGLE CLOUD UI TO CONFIGURE - ETC... AND WHAT I CAN REMEMBER OF AZURE LIKEWISE
    // see these (IMPERFECT ) docs for example - https://developers.google.com/identity/protocols/oauth2/javascript-implicit-flow

    /**
     *  \brief sometimes called ClientID, and sometimes called applicationID
     */
    using ApplicationIDType = Common::GUID;
    using Characters::String;
    using Containers::Sequence;

    using DataExchange::ObjectVariantMapper;

    /**
    * often require things like no #/fragments
     */
    using RedirectURLType = IO::Network::URI;

    struct ClientConfiguration {
        ApplicationIDType         fApplicationID;
        Sequence<RedirectURLType> fRedirectURLs;

        // maybe add 'scopes'

        static const ObjectVariantMapper kMapper;

        String ToString () const;
    };

/// DRAFT OF MORE STUFF TO ADD - see javascript frameworks - for doing auth2 - convert token to access token etc...
// Provide predefined ones for google, facebook, etc
// stuff to fetch 'keys' like I vaguely remember from openid... to validate jwts...
#if 0
    struct ConfigurationProviderType {
        IO::Network::URI forThisOp;
        IO::Network::URI forTahtOp;
        String name;
    };

    using ConfigurationProvidersType = Sequence<ConfigurationProviderType>;


    using M = Mapping<IDOFCONFIGPROVIDER (like google, apple, twitter), ClientConfiguration);
    // this is often what we need in configuration - maybe not mapping but array of key pairs cuz order matters - first
    // is default
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Configuration.inl"

#endif /*_Stroika_Frameworks_Auth_OAuth_Configuration_h_*/
