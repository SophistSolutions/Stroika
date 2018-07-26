/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Client_libcurl_inl_
#define _Stroika_Foundation_IO_Network_Transfer_Client_libcurl_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::Network::Transfer {

#if qHasFeature_LibCurl
    // class LibCurlException
    inline LibCurlException::CURLcode LibCurlException::GetCode () const
    {
        return fCurlCode_;
    }
#endif

}
#endif /*_Stroika_Foundation_IO_Network_Transfer_Client_libcurl_inl_*/
