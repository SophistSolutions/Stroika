/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Client_h_
#define _Stroika_Foundation_IO_Network_Transfer_Client_h_ 1

#include "../../../StroikaPreComp.h"

#include "Connection.h"

/**
 *
 *
 */

namespace Stroika::Foundation::IO::Network::Transfer {

    [[deprecated ("Since v2.1d27, use Connection::New")]] inline Connection::Ptr CreateConnection (const Connection::Options& options = {})
    {
        return Connection::New (options);
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_IO_Network_Transfer_Client_h_*/
