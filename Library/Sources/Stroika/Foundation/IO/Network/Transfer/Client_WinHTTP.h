/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Client_WinHTTP_h_
#define _Stroika_Foundation_IO_Network_Transfer_Client_WinHTTP_h_ 1

#include "../../../StroikaPreComp.h"

#include "Client.h"

/**
 *
 */

namespace Stroika::Foundation::IO::Network::Transfer {

#if qHasFeature_WinHTTP
    /**
     *  'Connection_WinHTTP' is a quasi-namespace - just for the 'WinHTTP' based Connection factory
     */
    class Connection_WinHTTP : public Connection {
    public:
        // when we lose this CTOR, then Connection_WinHTTP inherits from Connection
        [[deprecated ("in Stroika v2.1d27 - use Connection::Ptr conn = Connection_WinHTTP::New () instead")]] Connection_WinHTTP (const Options& options = Options ());

    public:
        /**
         */
        static Connection::Ptr New (const Options& options = {});

    private:
        class Rep_;
    };

#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Client_WinHTTP.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Client_WinHTTP_h_*/
