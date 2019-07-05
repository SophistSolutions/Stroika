/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_ConnectionPool_h_
#define _Stroika_Foundation_IO_Network_Transfer_ConnectionPool_h_ 1

#include "../../../StroikaPreComp.h"

#include "Connection.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *
 * TODO:
 */

namespace Stroika::Foundation::IO::Network::Transfer {

    /**
     * Simple connection factory object. If you don't care what backend to use for remote connections, use this API
     * to construct an unconnected object.
     *
     *  \par Example Usage
     *      \code
     *          ConnectionPool  connectionPool {3};
     *          auto&&             connection = connectionPool.New (URI{L"http://myexternalip.com/});
     *          auto&&             response = connection.GET (L"http://myexternalip.com/raw");
     *          nw.fExternalIPAddress = IO::Network::InternetAddress{response.GetDataTextInputStream ().ReadAll ()};
     *      \endcode
     */
    class ConnectionPool {
    public:
        ConnectionPool (size_t maxConnections);
        ConnectionPool (const ConnectionPool&) = delete;

    public:
        /**
         * Only ‘schemeAndAuthority looked at from (optional) hint.
         *
         * If timeout allocating connection (because all busy/in use), 
         *      throw TimeoutException
         * UNLESS
         *      If AllocateGloballyIfTimeout given argument, then instead of throwing, allocate a global connection object (Connection::New ())
         *      Caller cannot really tell for the most part, unless they call get, but if they just say what they want, they cannot tell.
         */
        nonvirtual Connection::Ptr New (URI hint = {});
        nonvirtual Connection::Ptr New (const Time::Duration& timeout, URI hint = {});
        enum AllocateGloballyIfTimeout { eAllocateGloballyIfTimeout };
        nonvirtual Connection::Ptr New (AllocateGloballyIfTimeout, const Time::Duration& timeout, URI hint = {});
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConnectionPool.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_ConnectionPool_h_*/
