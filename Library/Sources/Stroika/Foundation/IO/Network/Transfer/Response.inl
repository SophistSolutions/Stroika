/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::Transfer {

    /*
     ********************************************************************************
     ********************************* Transfer::Response ***************************
     ********************************************************************************
     */
    inline Response::Response (const BLOB& data, HTTP::Status status, const Mapping<String, String>& headers, const optional<SSLResultInfo>& sslInfo)
        : fData_{data}
        , fHeaders_{headers}
        , fStatus_{status}
        , fServerEndpointSSLInfo_{sslInfo}
    {
    }
    inline BLOB Response::GetData () const
    {
        return fData_;
    }
    inline Mapping<String, String> Response::GetHeaders () const
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
        return HTTP::Exception::IsHTTPStatusOK (fStatus_);
    }
    inline bool Response::GetFailed () const
    {
        return 400 <= fStatus_ and fStatus_ <= 599;
    }
}

/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {

    template <>
    constexpr EnumNames<Foundation::IO::Network::Transfer::Response::SSLResultInfo::ValidationStatus>
        DefaultNames<Foundation::IO::Network::Transfer::Response::SSLResultInfo::ValidationStatus>::k{{{
            {Foundation::IO::Network::Transfer::Response::SSLResultInfo::ValidationStatus::eNoSSL, L"No-SSL"},
            {Foundation::IO::Network::Transfer::Response::SSLResultInfo::ValidationStatus::eSSLOK, L"SSL-OK"},
            {Foundation::IO::Network::Transfer::Response::SSLResultInfo::ValidationStatus::eCertNotYetValid, L"Cert-Not-Yet-Valid"},
            {Foundation::IO::Network::Transfer::Response::SSLResultInfo::ValidationStatus::eCertExpired, L"Cert-Expired"},
            {Foundation::IO::Network::Transfer::Response::SSLResultInfo::ValidationStatus::eHostnameMismatch, L"Hostname-Mismatch"},
            {Foundation::IO::Network::Transfer::Response::SSLResultInfo::ValidationStatus::eSSLFailure, L"SSL-Failure"},
        }}};

}
