/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Listener_h_
#define _Stroika_Foundation_IO_Network_Listener_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/IO/Network/ConnectionOrientedMasterSocket.h"
#include "Stroika/Foundation/IO/Network/ConnectionOrientedStreamSocket.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"

namespace Stroika::Foundation::IO::Network {

    /**
     *  \brief Listens on one or more addresses, and hands each connection that arrives to a callback, on a thread of its own
     *
     *  Listener binds each address in the calling thread, so it fails to construct if a Bind () fails.
     *
     *  It accepts connections on a thread of its own, and calls newConnectionAcceptor ON THAT THREAD: no other connection is
     *  accepted until it returns, so it should do little more than hand the connection to another thread (as
     *  WebServer::ConnectionManager does, to its thread pool). An exception from it is traced and ignored.
     *
     *  A failed accept is retried after a pause, which grows while the failures continue (to half a second), so a lasting
     *  cause - the process out of file descriptors - costs no CPU, and the connections it held up are taken once it goes
     *  (where the OS keeps them queued, as Linux does; macOS's accept drops each one).
     *
     *  Destroying the Listener aborts its thread and waits for it to finish - so a newConnectionAcceptor still running then
     *  gets Thread::AbortException at its next interruption point.
     */
    class Listener {
    public:
        /**
         *  \brief How many connections the OS queues, waiting to be accepted, unless told otherwise: the OS's own maximum
         *
         *  SOMAXCONN, which each OS caps at its own configured limit (Linux: net.core.somaxconn).
         *
         *  @see ConnectionOrientedMasterSocket::Ptr::Listen
         */
        static const unsigned int kDefaultBacklog;

    public:
        /**
         *  backlog: how many connections the OS queues, waiting to be accepted (@see kDefaultBacklog)
         *
         *  \note   DesignNote
         *          The reason the accept callback doesn't tell you which sockaddr returned is because you can find that with
         *          sock.GetLocalAddress ()
         */
        Listener (const SocketAddress& addr, const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor,
                  unsigned int backlog = kDefaultBacklog);
        Listener (const SocketAddress& addr, const ConnectionOrientedMasterSocket::BindFlags& bindFlags,
                  const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor,
                  unsigned int backlog = kDefaultBacklog); ///< \brief one address, with BindFlags
        Listener (const Traversal::Iterable<SocketAddress>&                                        addrs,
                  const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor,
                  unsigned int backlog = kDefaultBacklog); ///< \brief several addresses, one thread for all
        Listener (const Traversal::Iterable<SocketAddress>& addrs, const ConnectionOrientedMasterSocket::BindFlags& bindFlags,
                  const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor,
                  unsigned int backlog = kDefaultBacklog); ///< \brief several addresses, with BindFlags
        Listener (Listener&& rhs) noexcept;                ///< \brief takes over rhs's thread
        Listener (const Listener&) = delete;

    public:
        nonvirtual Listener& operator= (Listener&&) noexcept = default;
        nonvirtual Listener& operator= (const Listener&)     = delete;

    private:
        struct Rep_;
        shared_ptr<Rep_> fRep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Listener.inl"

#endif /*_Stroika_Foundation_IO_Network_Listener_h_*/
