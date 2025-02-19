/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _StroikaSample_WebServices_IWSAPI_h_
#define _StroikaSample_WebServices_IWSAPI_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/DataExchange/TypedBLOB.h"
#include "Stroika/Foundation/Memory/BLOB.h"

#include "Stroika/Frameworks/Auth/CurrentIdentity.h"
#include "Stroika/Frameworks/WebService/OpenAPI/Specification.h"

#include "Model.h"

/**
 */

namespace Stroika::Samples::HTMLUI {
    using Stroika::Foundation::Characters::String;
    using Stroika::Foundation::Common::GUID;
    using Stroika::Foundation::Containers::Collection;
    using Stroika::Foundation::DataExchange::TypedBLOB;
    using Stroika::Frameworks::Auth::CurrentIdentityManager;

    using namespace Model;

    /**
     * @brief identity inferred from webserver authentication information
     */
    struct WebServiceIdentity {
        optional<String> fBearerToken;

        String ToString () const;
    };

    /**
     * \par Example Usage:
     *      // In WebServer, to establish thread_local ID value - use CurrentIdentityAuthInterceptor
     * 
     *      // Then in WSAPI instance
     *      if (auto c = CurrentAuthManager::Get ()) {
     *      }
     */
    using CurrentAuthManager = CurrentIdentityManager<optional<WebServiceIdentity>>;

    /**
     *  \brief: IWSAPI defines an abstract version of the web service API provided by this program.
     */
    class IWSAPI {
    protected:
        IWSAPI () = default;

    public:
        IWSAPI (const IWSAPI&) = delete;
        virtual ~IWSAPI ()     = default;

    public:
        /**
         */
        virtual Stroika::Frameworks::WebService::OpenAPI::Specification GetOpenAPISpecification () const = 0;

    public:
        /**
         * @brief  
         */
        virtual Auth::Configuration auth_oauth_configuration_GET () const = 0;

    public:
        /**
         * @brief  
         */
        virtual Auth::TokenResponse auth_oauth_tokens_POST (const Auth::TokenRequest& tr) const = 0;

    public:
        /**
         * @brief  
         */
        virtual void auth_oauth_tokens_revoke_POST (const Auth::TokenRevocationRequest& tr) const = 0;

    public:
        /**
         * @brief  
         * 
         * @return Auth::UserInfo 
         */
        virtual Auth::UserInfo auth_oauth_user_info_GET () const = 0;

    public:
        /**
         */
        virtual About about_GET () const = 0;

    public:
        /**
         */
        virtual HealthStatus healthcheck_GET () const = 0;

    public:
        virtual TypedBLOB resource_GET (const String& name) const = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IWSAPI.inl"

#endif /*_StroikaSample_WebServices_IWSAPI_h_*/
