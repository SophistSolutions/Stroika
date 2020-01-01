/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Connection_libcurl_inl_
#define _Stroika_Foundation_IO_Network_Transfer_Connection_libcurl_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
CompileTimeFlagChecker_HEADER (Stroika::Foundation::IO::Network::Transfer, qHasFeature_LibCurl, qHasFeature_LibCurl);

namespace Stroika::Foundation::IO::Network::Transfer {

#if qHasFeature_LibCurl

    inline void ThrowIfError (CURLcode status)
    {
        if (status != CURLE_OK)
            [[UNLIKELY_ATTR]]
            {
                Execution::Throw (Execution::SystemErrorException<> (status, LibCurl_error_category ()));
            }
    }
#endif

}

#endif /*_Stroika_Foundation_IO_Network_Transfer_Connection_libcurl_inl_*/
