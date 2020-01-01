/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
    class Connection_WinHTTP : public Connection {
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
#include "Connection_WinHTTP.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Connection_WinHTTP_h_*/
