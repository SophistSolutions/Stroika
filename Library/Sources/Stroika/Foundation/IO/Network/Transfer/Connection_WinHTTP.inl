/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Connection_WinHTTP_inl_
#define _Stroika_Foundation_IO_Network_Transfer_Connection_WinHTTP_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
CompileTimeFlagChecker_HEADER (Stroika::Foundation::IO::Network::Transfer, qHasFeature_WinHTTP, qHasFeature_WinHTTP);

namespace Stroika::Foundation::IO::Network::Transfer {

#if qHasFeature_WinHTTP
    namespace [[deprecated ("Since Stroika v3.0d4 use WinHTTP::Connection")]] Connection_WinHTTP {
        using namespace Stroika::Foundation::IO::Network::Transfer::WinHTTP::Connection;
    };
#endif

}

#endif /*_Stroika_Foundation_IO_Network_Transfer_Connection_WinHTTP_inl_*/
