/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Connection_WinHTTP_h_
#define _Stroika_Foundation_IO_Network_Transfer_Connection_WinHTTP_h_ 1

#include "../../../StroikaPreComp.h"

#include "Connection.h"

/**
 *
 */

namespace Stroika::Foundation::IO::Network::Transfer {

#if qHasFeature_WinHTTP
    /**
     *  'Connection_WinHTTP' is a quasi-namespace - just for the 'WinHTTP' based Connection factory
     */
    namespace Connection_WinHTTP {
        using namespace Stroika::Foundation::IO::Network::Transfer::Connection;
        /**
         */
        Connection::Ptr New (const Options& options = {});
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Connection_WinHTTP.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Connection_WinHTTP_h_*/
