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
     *          Connection  c   =   IO::Network::Transfer::CreateConnection ();
     *          c.SetURL (URI::Parse (L"http://www.google.com"));
     *          Response    r   =   c.GET ();
     *          Assert (r.GetSucceeded ());
     *          String result = r.GetDataTextInputStream ().ReadAll ();
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          auto&&             connection = IO::Network::Transfer::CreateConnection ();
     *          connection.SetURL (IO::Network::URI::Parse (L"http://myexternalip.com/raw"));
     *          auto&&             response = connection.GET ();
     *          nw.fExternalIPAddress = IO::Network::InternetAddress{response.GetDataTextInputStream ().ReadAll ()};
     *      \endcode
     */

    class ConnectionPool {
    public:
        ConnectionPool (size_t maxConnections);
        ConnectionPool (const ConnectionPool&) = delete;

    public:
        /**
         * Only ‘schemeAndAuthority looked at from hint
         * its optional (empty OK)
         * if timeout, throw as usual (if no connections become available in time
         * If AllocateGloballyIfTimeout given, and timeout, then instead of allocating from the throwing, allocate a global connection object (caller cannot really tell for the most part, unless they call get, but if they just say what they want, they cannot tell).
         */
        nonvirtual Connection::Ptr New (URI hint);
        nonvirtual Connection::Ptr New (URI hint, const Time::Duration& timeout);
        enum AllocateGloballyIfTimeout { eAllocateGloballyIfTimeout };
        nonvirtual Connection::Ptr New (URI hint, const Time::Duration& timeout, AllocateGloballyIfTimeout);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConnectionPool.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_ConnectionPool_h_*/
