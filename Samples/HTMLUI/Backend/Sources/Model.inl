/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Samples::HTMLUI::Model {

    /*
     ********************************************************************************
     ******************************* Auth::TokenRequest *****************************
     ********************************************************************************
     */
    template <>
    inline Stroika::Frameworks::Auth::OAuth::TokenRequest Auth::TokenRequest::As () const
    {
        return Stroika::Frameworks::Auth::OAuth::TokenRequest{.client_id     = fApplicationID,
                                                              .code          = fAuthorizationCode,
                                                              .grant_type    = "authorization_code"sv,
                                                              .client_secret = nullopt,
                                                              .redirect_uri  = fRedirectURL,
                                                              .code_verifier = fCodeVerifier};
    }

    /*
     ********************************************************************************
     ****************************** Auth::TokenResponse *****************************
     ********************************************************************************
     */
    inline Auth::TokenResponse::TokenResponse (const Stroika::Frameworks::Auth::OAuth::TokenResponse& tr)
        : access_token{tr.access_token}
        , expires_at{tr.expires_at}
        , scopes{tr.scope}
        , refresh_token{tr.refresh_token}
        , id_token{tr.id_token}
    {
    }

    /*
     ********************************************************************************
     *********************************** Auth::UserInfo *****************************
     ********************************************************************************
     */
    inline Auth::UserInfo::UserInfo (const Stroika::Frameworks::Auth::OAuth::UserInfo& ui)
        : fName{ui.name}
        , fEmail{ui.email}
        , fPersonImage{ui.picture}
    {
    }

}
