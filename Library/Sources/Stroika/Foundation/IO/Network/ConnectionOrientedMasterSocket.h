/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_h_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/IO/Network/ConnectionOrientedStreamSocket.h"

namespace Stroika::Foundation::IO::Network {

    /**
     *  This class is to be used with ConnectionOrientedStreamSocket. You create a ConnectionOrientedMasterSocket, and
     *  Bind () it, and Listen () on it, and the resulting sockets (from Accept()) are of type ConnectionOrientedStreamSocket.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter</a>
     */
    namespace ConnectionOrientedMasterSocket {

        using namespace Socket;

        class _IRep;

        /**
         *  \brief How Ptr::Bind binds a listening (TCP) socket: what this socket consents to, when the address is not free
         *
         *  An address can be busy two ways - connections from an earlier socket, now closed, are still closing (TCP's
         *  TIME_WAIT, a minute or so); or a LIVE socket holds it (another server, or another copy of this one). Each flag is
         *  this socket's side of one of those:
         *
         *      Already on the address      Bind succeeds when
         *      ----------------------      ------------------
         *      nothing                     always
         *      old connections closing     fBindDespiteOldConnections
         *      a live socket               BOTH sockets set fShareWithCooperatingSockets
         *
         *  Whether this socket (or process) bound the address before does not matter. A combination the OS cannot honor
         *  makes Bind throw (errc::operation_not_supported) - see each flag.
         */
        struct BindFlags {
            /**
             *  Bind succeeds though connections from an earlier socket on this address - ours or another process's, since
             *  closed - are still closing: so a server restarts at once, rather than failing for a minute or so.
             *
             *  POSIX: SO_REUSEADDR - which Linux honors only if the earlier socket set it too (every Stroika listener does),
             *  while macOS needs only this one's. Windows: always so - false is not supported there.
             *
             *  \note POSIX binds despite old connections with fShareWithCooperatingSockets too (SO_REUSEPORT allows it) - so
             *        false is not supported with it.
             */
            bool fBindDespiteOldConnections{true};

            /**
             *  Share the address with other live listening sockets that ALSO set this - to spread connections across
             *  processes, or to start a new server before the old one stops, so there is never a moment when nothing
             *  listens. Sharing takes BOTH sockets' consent: one that does not set this can neither join a shared address,
             *  nor be joined.
             *
             *  Which of the sharing sockets gets each new connection depends on the OS (Linux 7.0, macOS 26, Windows 11):
             *      o   Linux: spread across them; and they must belong to the same user (SO_REUSEPORT)
             *      o   macOS: the one bound last - so a new server takes over at once (SO_REUSEPORT)
             *      o   Windows: the one bound first - so a new server takes over once the old one closes (SO_REUSEADDR)
             *
             *  Not set: no other socket may bind the address while this one holds it - POSIX: always so, for a TCP listener;
             *  Windows: SO_EXCLUSIVEADDRUSE (it does not stop a restart while old connections close).
             */
            bool fShareWithCooperatingSockets{false};
        };

        /**
         *  \par Example Usage
         *      \code
         *          ConnectionOrientedMasterSocket::Ptr ms  = ConnectionOrientedMasterSocket::New (SocketAddress::INET, Socket::STREAM);
         *          ms.Bind (addr);
         *          ms.Listen (backlog);
         *          Sequence<ConnectionOrientedMasterSocket::Ptr>   l;  // cannot do Sequence<ConnectionOrientedMasterSocket> cuz not copyable
         *          l.push_back (ms);
         *      \endcode
         *
         *  \note Since ConnectionOrientedMasterSocket::Ptr is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
         *        the underlying thread object.
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter</a>
         */
        class Ptr : public Socket::Ptr {
        private:
            using inherited = Socket::Ptr;

        public:
            /**
             */
            Ptr () = delete;
            Ptr (nullptr_t);
            Ptr (const Ptr& src) = default;
            Ptr (Ptr&& src)      = default;
            Ptr (shared_ptr<_IRep>&& rep);
            Ptr (const shared_ptr<_IRep>& rep);

        public:
            /**
             */
            nonvirtual Ptr& operator= (const Ptr& rhs) = default;
            nonvirtual Ptr& operator= (Ptr&& rhs)      = default;

        public:
            /**
             *  Associate this socket with the given local address - before Listen ().
             *
             *  @todo   SB an overload taking just a port, and defaults to INADDRANY with that port.
             *          Port is only thing really needed, but InternetAddress part provided as arg
             *          too in case you want to bind to a particular interface.
             *
             *  @see POSIX bind()
             */
            nonvirtual void Bind (const SocketAddress& sockAddr, BindFlags bindFlags = BindFlags{});

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
             * \pre fRep_ != nullptr
             */
            nonvirtual _IRep& _ref () const;

        protected:
            /**
             * \pre fRep_ != nullptr
             */
            nonvirtual const _IRep& _cref () const;
        };

        /**
         */
        class _IRep : public Socket::_IRep {
        public:
            virtual ~_IRep ()                                                         = default;
            virtual void                                Listen (unsigned int backlog) = 0;
            virtual ConnectionOrientedStreamSocket::Ptr Accept ()                     = 0;
        };

        /**
         *  \par Example Usage
         *      \code
         *          ConnectionOrientedMasterSocket::Ptr ms = ConnectionOrientedMasterSocket::New (SocketAddress::INET, Socket::STREAM);
         *          ms.Bind (addr);
         *          ms.Listen (backlog);
         *      \endcode
         *
         *  \note unless you call @Detach() - socket is CLOSED in DTOR of rep, so when final reference goes away
         *
         *  \note ConnectionOrientedMasterSocket is not copyable, but it can be copied into a ConnectionOrientedMasterSocket::Ptr or
         *        Socket::Ptr.  This is critical to save them in a container, for example.
         */
        Ptr New (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol = {});

        /**
         *  This function associates a Platform native socket handle with a Stroika wrapper object.
         *
         *  Once a PlatformNativeHandle is attached to Socket object, it will be automatically closed
         *  when the last reference to the socket disappears (or when someone calls close).
         *
         *  To prevent that behavior, you can Detach the PlatformNativeHandle before destroying
         *  the associated Socket object.
         */
        Ptr Attach (PlatformNativeHandle sd);

    };

}

namespace Stroika::Foundation::Execution::WaitForIOReady_Support {

    // Specialize to override GetSDKPollable
    template <typename T>
    struct WaitForIOReady_Traits;
    template <>
    struct WaitForIOReady_Traits<IO::Network::ConnectionOrientedMasterSocket::Ptr> {
        using HighLevelType = IO::Network::ConnectionOrientedMasterSocket::Ptr;
        static inline auto GetSDKPollable (const HighLevelType& t)
        {
            return t.GetNativeSocket ();
        }
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConnectionOrientedMasterSocket.inl"

#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_h_*/
