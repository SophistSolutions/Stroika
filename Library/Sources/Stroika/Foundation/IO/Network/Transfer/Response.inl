/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::Transfer {

    /*
     ********************************************************************************
     ********************************* Transfer::Response ***************************
     ********************************************************************************
     */
    inline BLOB Response::GetData () const
    {
        return fData_;
    }
    inline HTTP::Headers Response::GetHeaders () const
    {
        return fHeaders_;
    }
    inline HTTP::Status Response::GetStatus () const
    {
        return fStatus_;
    }
    inline optional<Response::SSLResultInfo> Response::GetSSLResultInfo () const
    {
        return fServerEndpointSSLInfo_;
    }
    inline bool Response::GetSucceeded () const
    {
        return HTTP::IsOK (fStatus_);
    }
    inline bool Response::GetFailed () const
    {
        return 400 <= fStatus_ and fStatus_ <= 599;
    }
}

namespace Stroika::Foundation::IO::Network::Transfer {
    Common::StroikaNaming EnumNamingRule (Response::SSLResultInfo::ValidationStatus);
}
