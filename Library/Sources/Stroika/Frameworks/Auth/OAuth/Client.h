/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Auth_OAuth_Client_h_
#define _Stroika_Frameworks_Auth_OAuth_Client_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/TypedBLOB.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/VirtualLockable.h"
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
    using Execution::InternallySynchronized;
    using IO::Network::URI;
    using Time::DateTime;

    using DataExchange::ObjectVariantMapper;

    /**
     *  @brief this is the argument to the Fetcher::GetToken () API. It typically consists of a client_id, client_secret, authorization 'code' and other things in OAUTH client auto token request.
     * 
     *   MEANT to be provider independent, but best docs I've found so far...
     * 
     *  \see https://developers.google.com/identity/protocols/oauth2/web-server#httprest_3
     */
    struct TokenRequest {
        /**
         * The client ID obtained from the Cloud Console Clients page (https://console.cloud.google.com/auth/clients).
         */
        String client_id;

        /**
         * The authorization code returned from the request (https://developers.google.com/identity/protocols/oauth2/web-server#httprest_1)
         *
         *   \note refresh_token and code are mutually exclusive (not using variant cuz same type and no name)
         * 
         *  implies grant_type: authorization_code
         */
        optional<String> code;

        /**
         *   \note refresh_token and code are mutually exclusive (not using variant cuz same type and no name)
         * 
         *  implies grant_type: refresh_token
         */
        optional<String> refresh_token;

#if 0
        /**
         * \brief set to 'authorization_code' when exchanging authorization code for access token
         * 
         *  https://datatracker.ietf.org/doc/html/rfc6749#section-1.3.1
         */
        String grant_type{"authorization_code"sv};
#endif

        /**
         *  The client secret obtained from the Cloud Console Clients page (https://console.cloud.google.com/auth/clients).
         *  It is also often found in ClientConfiguration::fClientSecret
         */
        optional<String> client_secret;

        /**
         * One of the redirect URIs listed for your project in the Cloud Console Clients page (https://console.cloud.google.com/auth/clients) for the given client_id.
         * Needed for most flows, but not all.
         */
        optional<URI> redirect_uri;

        /**
         * WAG - FIND DOCS ON THIS---
         *  \note only applies to 
         */
        optional<String> code_verifier;

        /**
         */
        nonvirtual String ToString () const;

        /**
         */
        nonvirtual TypedBLOB ToWireFormat () const;

        /**
         */
        static TokenRequest FromWireFormat (const TypedBLOB& src);

        /**
         * @brief Compare by string value of various fields.
         */
        auto operator<=> (const TokenRequest& rhs) const = default;

        static const ObjectVariantMapper kMapper;
    };

    /**
     *  @brief this is the response to the Fetcher::GetToken () API. It typically provides an 'access token' with a set of scopes, and other things about the provided access.
     * 
     *   MEANT to be provider independent, but best docs I've found so far...
     * 
     *  https://developers.google.com/identity/protocols/oauth2/web-server#exchange-authorization-code
     */
    struct TokenResponse {
        /**
         *      https://datatracker.ietf.org/doc/html/rfc6749#section-1.4
         */
        String access_token;

        /**
         * OAuth uses expires_in, but we convert to an expires_at since better to track (in UTC)
         */
        DateTime expires_at = DateTime::Now ();

        Set<String> scope;

        /**
         *  https://datatracker.ietf.org/doc/html/rfc6749#section-1.5
         */
        optional<String> refresh_token;

        /**
         */
        optional<String> id_token;

        /**
         */
        optional<String> token_type;

        nonvirtual String ToString () const;

        nonvirtual TypedBLOB ToWireFormat () const;
        static TokenResponse FromWireFormat (const TypedBLOB& src);

        static const ObjectVariantMapper kMapper;
    };

    /**
     */
    struct TokenRevocationRequest {
        String           access_token;
        optional<String> refresh_token;
        optional<String> client_id;
        optional<String> client_secret;

        nonvirtual String ToString () const;

        nonvirtual TypedBLOB ToWireFormat () const;

        static const ObjectVariantMapper kMapper;
    };

    /**
     * @brief RFC 7662 compatible API for finding info about a token - https://datatracker.ietf.org/doc/html/rfc7662
     * 
     *  FOR NOW - we only use expires_at field and only support google token_info API
     */
    struct TokenIntrospectionResponse {

        /**
         * OAuth uses expires_in, but we convert to an expires_at since better to track (in UTC)
         */
        DateTime expires_at = DateTime::Now ();

        nonvirtual String ToString () const;

        nonvirtual TypedBLOB              ToWireFormat () const;
        static TokenIntrospectionResponse FromWireFormat (const TypedBLOB& src);

        static const ObjectVariantMapper kMapper;
    };

    /**
     */
    struct UserInfo {

        /**
         */
        optional<String> name;

        /**
         */
        optional<String> given_name;

        /**
         */
        optional<String> family_name;

        /**
         */
        optional<String> email;

        /**
         * image of user (thumbnail)
         */
        optional<URI> picture;

        nonvirtual String ToString () const;

        static UserInfo FromWireFormat (const TypedBLOB& src);

        static const ObjectVariantMapper kMapper;
    };

    /**
     *  \brief simple wrapper on IO::Network::Transfer to do fetching (more configurability to do)
     * 
     *  \note often you will want to use CachingFetcher
     */
    class Fetcher {
    public:
        struct Options {
            bool                   fCaching{false};
            InternallySynchronized fInternallySyncrhonized{InternallySynchronized::eNotKnownInternallySynchronized};
        };

    public:
        /**
         *  \par Example Usage
         *      \code
         *          // rarely used, no caching
         *          ProviderConfiguration providerConfiguration{Auth::OAuth::kDefaultProviderConfigurations.LookupChecked (
         *                                                      GetUseProvider_ (wsi), RuntimeErrorException{"Unrecognized provider name"sv})};
         *          if (wsi and wsi->fBearerToken) {
         *              Auth::OAuth::Fetcher  f{providerConfiguration};
         *              Auth::OAuth::UserInfo clientUserInfo = f.GetUserInfo (wsi->fBearerToken.value_or (String{}));
         *              return clientUserInfo;
         *          }
         *      \endcode
         * 
         *  \note if you create a COPY of a Fetcher, it will NOT contain the same CACHE (so you can use this to clear/lose the cache).
         *        But it will contain all the same OPTIONS settings
         */
        Fetcher () = delete;
        Fetcher (const Fetcher& src);
        Fetcher (const ProviderConfiguration& providerConfiguration);
        Fetcher (const ProviderConfiguration& providerConfiguration, const Options& options);

    public:
        /**
         *  https://developers.google.com/identity/protocols/oauth2/web-server#exchange-authorization-code
         *  https://developers.google.com/identity/protocols/oauth2/web-server#offline
         * 
         *  This can be used to convert EITHER an authorization_code (code parameter) or a refresh_token
         *  to a new access_code (and other TokenResponse info).
         * 
         *  \note - confusingly - despite docs above to the contrary, if you are not getting a refresh_token back it could
         *          be because google doesn't return it except on the first get token call
         *          (https://stackoverflow.com/questions/10827920/not-receiving-google-oauth-refresh-token)
         */
        nonvirtual TokenResponse GetToken (const TokenRequest& tr) const;

    public:
        /**
         *  Try to revoke the given refresh/access tokens. If no URI found in the ProviderConfiguration, its assumed the provider
         *  doesn't support this, and nothing todo. If an error occurs (say because of bad or missing client_id, or client_secret or already revoked?)
         *  an exception will be reported.
         *
         *  \see https://datatracker.ietf.org/doc/html/rfc7009 (but this is woefully insufficient/incomplete/NOT what I followed)
         *  \see https://github.com/openid/AppAuth-JS/blob/master/src/revoke_token_request.ts - really got impl from here
         */
        nonvirtual void RevokeTokens (const TokenRevocationRequest& tr) const;

    public:
        /**
         * curl -v -H "Authorization: Bearer ddd" https://www.googleapis.com/oauth2/v3/userinfo
         * 
         * @todo FIND DOCS FOR THIS - try docs on https://accounts.google.com/.well-known/openid-configuration
         * 
         *  \note this MAY generate a slightly abbreviated user-info object, if the original access token was retrieved
         *        with an id_token (parsed out of that). To avoid that, if you want the full userInfo from the endpoint,
         *        create a new Fetcher instance.
         */
        nonvirtual UserInfo GetUserInfo (const String& accessToken) const;

    private:
        //     Google TokenInfo Endpoint
        // You can use this endpoint to "introspect" an access token by sending a GET request:
        // Endpoint: https://oauth2.googleapis.com/tokeninfo
        // Parameter: access_token
        // Example Request
        // http
        // GET https://oauth2.googleapis.com

        // Expected JSON Response
        // If the token is valid, Google returns metadata including the expiration time:
        //
        // DOES NOT USE CACHE!!!
        // Only used so that when we are given an accessCode to get userInfo for - so we can know how long it remains active in the cache
        nonvirtual optional<TokenIntrospectionResponse> FetchTokenIntrospection_ (const String& accessToken) const;

    private:
        /*
         * NOTE that fMaybeLock_ applies to BOTH cache and ?? - not sure what else there is???
         * So why not  use Syncrhonized? Cuz we dont have maybe-syncrhonized?
         */
        const ProviderConfiguration        fProviderConfiguration_;
        const Options                      fOptions_;
        mutable Execution::VirtualLockable fMaybeLock_; // either Debug::AssertExternallySyncrhonized or std::recursive_mutex
        struct Cache_ {
            static constexpr auto  kClearMaxFrequency_{30s};
            Time::TimePointSeconds fNextClearAt_{Time::GetTickCount () + kClearMaxFrequency_};

            // @todo REIMPLEMENT with new Cache layer code to support this - TTLCacher
            Containers::Mapping<TokenRequest, TokenResponse> fTokens;
            Containers::Mapping<String, DateTime>            fAccessToken2Expiration;
            Containers::Mapping<String, UserInfo>            fAccessToken2UserInfo;
        };
        unique_ptr<Cache_> fCache_;

    private:
        nonvirtual void ClearOldStuffFromCache_ () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Client.inl"

#endif /*_Stroika_Frameworks_Auth_OAuth_Client_h_*/
