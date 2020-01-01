/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Response_inl_
#define _Stroika_Foundation_IO_Network_Transfer_Response_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../HTTP/Exception.h"

namespace Stroika::Foundation::IO::Network::Transfer {

    /*
     ********************************************************************************
     ********************************* Transfer::Response ***************************
     ********************************************************************************
     */
    inline Response::Response (const BLOB& data, HTTP::Status status, const Mapping<String, String>& headers, const optional<SSLResultInfo>& sslInfo)
        : fData_ (data)
        , fHeaders_ (headers)
        , fStatus_ (status)
        , fServerEndpointSSLInfo_ (sslInfo)
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

}

#endif /*_Stroika_Foundation_IO_Network_Transfer_Response_inl_*/
