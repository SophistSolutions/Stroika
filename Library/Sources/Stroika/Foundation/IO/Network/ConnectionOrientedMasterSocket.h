/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_h_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_h_ 1

#include "../../StroikaPreComp.h"

#include "ConnectionOrientedStreamSocket.h"

namespace Stroika::Foundation::IO::Network {

    /**
     *  This class is to be used with ConnectionOrientedStreamSocket. You create a ConnectionOrientedMasterSocket, and
     *  Bind () it, and Listen () on it, and the resulting sockets (from Accept()) are of type ConnectionOrientedStreamSocket.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
     */
    class ConnectionOrientedMasterSocket : public Socket {
    private:
        using inherited = Socket;

    protected:
        class _IRep;

    public:
        class Ptr;

    public:
        /**
         */
        ConnectionOrientedMasterSocket ()                                        = delete;
        ConnectionOrientedMasterSocket (ConnectionOrientedMasterSocket&& s)      = delete;
        ConnectionOrientedMasterSocket (const ConnectionOrientedMasterSocket& s) = delete;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          ConnectionOrientedMasterSocket::Ptr ms = ConnectionOrientedMasterSocket::New (Socket::INET, Socket::STREAM);
         *          ms.Bind (addr);
         *          ms.Listen (backlog);
         *      \endcode
         *
         *  \note unless you call @Detatch() - socket is CLOSED in DTOR of rep, so when final reference goes away
         *
         *  \note ConnectionOrientedMasterSocket is not copyable, but it can be copied into a ConnectionOrientedMasterSocket::Ptr or
         *        Socket::Ptr.  This is critical to save them in a container, for example.
         */
        static Ptr New (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol = {});

    public:
        /**
         *  For copyability, use ConnectionOrientedMasterSocket::Ptr for assigned-to type.
         */
        nonvirtual ConnectionOrientedMasterSocket& operator= (ConnectionOrientedMasterSocket&& s) = delete;
        nonvirtual ConnectionOrientedMasterSocket& operator= (const ConnectionOrientedMasterSocket& s) = delete;

    public:
        /**
         *  This function associates a Platform native socket handle with a Stroika wrapper object.
         *
         *  Once a PlatformNativeHandle is attached to Socket object, it will be automatically closed
         *  when the last reference to the socket disappears (or when someone calls close).
         *
         *  To prevent that behavior, you can Detatch the PlatformNativeHandle before destroying
         *  the associated Socket object.
         */
        static Ptr Attach (PlatformNativeHandle sd);
    };

    /**
     *  \par Example Usage
     *      \code
     *          ConnectionOrientedMasterSocket::Ptr ms  = ConnectionOrientedMasterSocket::New (Socket::INET, Socket::STREAM);
     *          ms.Bind (addr);
     *          ms.Listen (backlog);
     *          Sequence<ConnectionOrientedMasterSocket::Ptr>   l;  // cannot do Sequence<ConnectionOrientedMasterSocket> cuz not copyable
     *          l.push_back (ms);
     *      \endcode
     *
     *  \note Since ConnectionOrientedMasterSocket::Ptr is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
     *        the underlying thread object.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
     */
    class ConnectionOrientedMasterSocket::Ptr : public Socket::Ptr {
    private:
        using inherited = Socket::Ptr;

    public:
        /**
         *  defaults to null (empty ())
         */
        Ptr () = default;
        Ptr (nullptr_t);
        Ptr (const Ptr& src) = default;
        Ptr (Ptr&& src)      = default;

    protected:
        Ptr (shared_ptr<_IRep>&& rep);
        Ptr (const shared_ptr<_IRep>& rep);

    public:
        /**
         */
        nonvirtual Ptr& operator= (const Ptr& rhs) = default;
        nonvirtual Ptr& operator= (Ptr&& rhs) = default;

    public:
        /**
         *  @todo   Need timeout on this API? Or global (for instance) timeout?
         *
         *   throws on error, and otherwise means should call accept
         */
        nonvirtual void Listen (unsigned int backlog) const;

    public:
        /**
         *  After Listen() on a connected socket returns (not throws) - you can call Accept() on tha same
         *  socket to allocate a NEW socket with the new connection stream.
         *
         *  @todo   Need timeout on this API? Or global (for instance) timeout?
         *
         *  \note ***Cancelation Point***
         */
        nonvirtual ConnectionOrientedStreamSocket::Ptr Accept () const;

    protected:
        /**
         */
        nonvirtual shared_ptr<_IRep> _GetSharedRep () const;

    protected:
        /**
         * \req fRep_ != nullptr
         */
        nonvirtual _IRep& _ref () const;

    protected:
        /**
         * \req fRep_ != nullptr
         */
        nonvirtual const _IRep& _cref () const;

    private:
        friend class ConnectionOrientedMasterSocket;
    };

    /**
     */
    class ConnectionOrientedMasterSocket::_IRep : public Socket::_IRep {
    public:
        virtual ~_IRep ()                                                         = default;
        virtual void                                Listen (unsigned int backlog) = 0;
        virtual ConnectionOrientedStreamSocket::Ptr Accept ()                     = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConnectionOrientedMasterSocket.inl"

#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_h_*/
