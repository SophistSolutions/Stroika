/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
 *      @todo   Add an auto-timeout feature, so connections disappear when not used for a certain
 *              period of time (to save memory, and networking resources).
 */

namespace Stroika::Foundation::IO::Network::Transfer {

    /**
     * Simple connection factory object. If you don't care what backend to use for remote connections, use this API
     * to construct an unconnected object.
     *
     *  \par Example Usage
     *      \code
     *          ConnectionPool  connectionPool {ConnectionPool::Options{3}};
     *          auto&&          connection = connectionPool.New (URI{L"http://myexternalip.com/});
     *          auto&&          response = connection.GET (L"http://myexternalip.com/raw");
     *          nw.fExternalIPAddress = IO::Network::InternetAddress{response.GetDataTextInputStream ().ReadAll ()};
     *      \endcode
     */
    class ConnectionPool {
    public:
        /**
         */
        struct Options {
            Options (const optional<unsigned int>& maxConnections = nullopt, const function<Connection::Ptr ()>& connectionFactory = (Connection::Ptr (*) ()) & Connection::New);

            /**
             *  Default options for each connection
             */
            optional<unsigned int> fMaxConnections;

            /**
             *  Factory to use for underlying connectionpool connections
             *
             *  \todo figure out why cast needed on New on g++ and clang++
             */
            function<Connection::Ptr ()> fConnectionFactory;
        };

    public:
        /**
         *  A ConnectionPool is a fixed, not copyable/movable, object, containing a bunch of other
         *  (typically http)IO::Transfer::Connection objects. The idea is that its cheaper to
         *  re-use these objects if you happen to want to connect to an HTTP endpoint that is already connected.
         */
        ConnectionPool (const Options& options = {});
        ConnectionPool (const ConnectionPool&) = delete;
        ~ConnectionPool ();

    public:
        nonvirtual ConnectionPool& operator= (const ConnectionPool&) = delete;

    public:
        /**
         * Only schemeAndAuthority is looked at from (optional) hint.
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

    private:
        class Rep_;
        unique_ptr<Rep_> fRep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConnectionPool.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_ConnectionPool_h_*/
