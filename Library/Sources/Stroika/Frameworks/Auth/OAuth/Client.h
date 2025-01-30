/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Auth_OAuth_Client_h_
#define _Stroika_Frameworks_Auth_OAuth_Client_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/TypedBLOB.h"
#include "Stroika/Foundation/IO/Network/URI.h"

#include "Stroika/Frameworks/Auth/OAuth/Configuration.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Frameworks::Auth::OAuth {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Containers::Set;
    using DataExchange::TypedBLOB;
    using IO::Network::URI;
    using Time::DateTime;

    using DataExchange::ObjectVariantMapper;

    /**
    *   MEANT to be provider independent, but best docs I've found so far...
    * 
     *  https://developers.google.com/identity/protocols/oauth2/web-server#httprest_3
     */
    struct TokenRequest {

        String           client_id;
        String           code;
        String           grant_type;
        optional<String> client_secret;
        optional<URI>    redirect_uri;

        nonvirtual String ToString () const;

        nonvirtual TypedBLOB ToWireFormat () const;
        static TokenRequest  FromWireFormat (const TypedBLOB& src);

        static const ObjectVariantMapper kMapper;
    };

    /**
     *   MEANT to be provider independent, but best docs I've found so far...
     * 
     *  https://developers.google.com/identity/protocols/oauth2/web-server#httprest_3
     */
    struct TokenResponse {
        /**
         */
        String access_token;

        /** OAuth uses expires_in, but we convert to an expires_at since better to track (in UTC) */
        DateTime expires_at = DateTime::Now ();

        Set<String>      scope;
        optional<String> refresh_token;
        optional<String> id_token;
        optional<String> token_type;

        nonvirtual String ToString () const;

        nonvirtual TypedBLOB ToWireFormat () const;
        static TokenResponse FromWireFormat (const TypedBLOB& src);

        static const ObjectVariantMapper kMapper;
    };

    /**
     *  \brief simple wrapper on IO::Network::Transfer to do fetching (more configurability to do)
     */
    class Fetcher {
    public:
        Fetcher ()               = delete;
        Fetcher (const Fetcher&) = default;
        Fetcher (const ProviderConfiguration& providerConfiguration, const ClientConfiguration& clientConfig);

    public:
        /**
         */
        nonvirtual TokenResponse Token (const TokenRequest& tr) const;

    private:
        const ProviderConfiguration fProviderConfiguration_;
        const ClientConfiguration   fClientConfig_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Client.inl"

#endif /*_Stroika_Frameworks_Auth_OAuth_Client_h_*/
