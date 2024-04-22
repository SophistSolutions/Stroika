/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedStreamSocket_h_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedStreamSocket_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Time/Duration.h"

#include "Socket.h"

namespace Stroika::Foundation::Memory {
    class BLOB;
}

namespace Stroika::Foundation::IO::Network {

    /**
     *  \brief ConnectionOrientedStreamSocket is typically a tcp stream, either setup with Connect, or ConnectionOrientedMasterSocket::Accept ()
     *
     *  \par Example Usage
     *      \code
     *          ConnectionOrientedStreamSocket::Ptr  s = ConnectionOrientedStreamSocket::New (SocketAddress::INET, Socket::STREAM);
     *          s.Connect (someSocketAddress);
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          ConnectionOrientedMasterSocket::Ptr ms ConnectionOrientedMasterSocket::New (SocketAddress::INET, Socket::STREAM);
     *          ms.Bind (addr);
     *          ms.Listen (backlog);
     *          ConnectionOrientedStreamSocket::Ptr      newConnection = ms.Accept ();
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter</a>
     */
    namespace ConnectionOrientedStreamSocket {
        using namespace Socket;

        class _IRep;

        /**
         */
        struct KeepAliveOptions {
            bool fEnabled{};
#if qPlatform_Linux or qPlatform_Windows
            optional<unsigned int>          fMaxProbesSentBeforeDrop;              // https://linux.die.net/man/7/tcp TCP_KEEPCNT
            optional<Time::DurationSeconds> fTimeIdleBeforeSendingKeepalives;      // https://linux.die.net/man/7/tcp TCP_KEEPIDLE
            optional<Time::DurationSeconds> fTimeBetweenIndividualKeepaliveProbes; // https://linux.die.net/man/7/tcp TCP_KEEPINTVL
#endif
            /**
             *  @see Characters::ToString ();
             */
            nonvirtual Characters::String ToString () const;
        };

        /**
         *  \par Example Usage
         *      \code
         *          ConnectionOrientedStreamSocket::Ptr      s  = ConnectionOrientedStreamSocket::New (SocketAddress::INET, Socket::STREAM);
         *          s.Connect (someSocketAddress);
         *          Sequence<ConnectionOrientedStreamSocket::Ptr> l;  // cannot do Sequence<ConnectionOrientedStreamSocket> cuz not copyable
         *          l.push_back (s);
         *      \endcode
         *
         *  \note Since ConnectionOrientedStreamSocket::Ptr is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
         *        the underlying thread object.
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter</a>
         *          Note that we make Read, ReadNonBlocking, Connect, and Write () all const, so that they can be each called at the same time from different threads as other methods
         *          like GetPeerAddress ().
         *
         *          It is \em NOT however, legal to call Read (or ReadNonBlocking) at the same time from two different threads, nor Write at the same time from two
         *          different threads, nor Connect while already connected, nor Read/ReadNonBlocking/Write while not Connected (and so therefore not legal to call
         *          Connect while calling Read/Write).
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
             *  @see Socket::Close () - plus optionally handles GetAutomaticTCPDisconnectOnClose
             *
             *  \note - you don't have to use this Close () - using the base class close does the same thing,
             *          but these docs are here just to make that more clear - the extra close functionality.
             *
             *  @see GetAutomaticTCPDisconnectOnClose - if set - Close automatically calls Shutdown () for connection-oriented sockets.
             */
            nonvirtual void Close () const;

        public:
            /**
             *  \brief Connects to the argument sockAddr;
             *
             *  If explicit timeout given, and the connection doesn't complete in time, TimeOutException will result.
             *  If no explicit timeout is given, the OS default setting for timeouts will be used.
             * 
             *  \note ***Cancelation Point***
             */
            nonvirtual void Connect (const SocketAddress& sockAddr) const;
            nonvirtual void Connect (const SocketAddress& sockAddr, const Time::Duration& timeout) const;

        public:
            /**
             *  @todo   Need timeout on this API? Or global (for instance) timeout?
             *
             *  \note Though Read () is a const method, can be done concurrently with most other const ConnectionOrientedStreamSocket methods,
             *        It is illegal (because useless and confusing) to do two reads (or ReadNonBlocking) at the same time. Read and Write maybe
             *        done simultaneously, from different threads.
             *
             *  \note ***Cancelation Point***
             */
            nonvirtual size_t Read (byte* intoStart, byte* intoEnd) const;

        public:
            /**
             *  \brief Non-blocking read: return {} if no data available, 0 on EOF.
             *
             *  \note Though Read () is a const method, can be done concurrently with most other const ConnectionOrientedStreamSocket methods,
             *        It is illegal (because useless and confusing) to do two reads (or ReadNonBlocking) at the same time. Read and Write maybe
             *        done simultaneously, from different threads.
             *
             *  \note if intoStart == nullptr, then don't actually read, but return the number of bytes available.
             */
            nonvirtual optional<size_t> ReadNonBlocking (byte* intoStart, byte* intoEnd) const;

        public:
            /**
             *  @todo   Need timeout on this API? Or global (for instance) timeout?
             */
            nonvirtual void Write (const byte* start, const byte* end) const;
            nonvirtual void Write (const Memory::BLOB& b) const;

        public:
            /**
             *  If there is a socket connected to the other side, return that peer's socket address.
             */
            nonvirtual optional<IO::Network::SocketAddress> GetPeerAddress () const;

        public:
            /**
             *  Automatically call Shutdown () when closing socket, and Wait this number of seconds to recieve the
             *  peer's close acknowledgment. If missing, don't automatically call Shutdown, nor do any waiting for the acknowledgment.
             *
             *  @see SetAutomaticTCPDisconnectOnClose ()
             */
            nonvirtual optional<Time::DurationSeconds> GetAutomaticTCPDisconnectOnClose () const;

        public:
            /**
             *  @see GetAutomaticTCPDisconnectOnClose ()
             */
            nonvirtual void SetAutomaticTCPDisconnectOnClose (const optional<Time::DurationSeconds>& waitFor) const;

        public:
            /**
             *  See http://man7.org/linux/man-pages/man7/socket.7.html - option SO_LINGER
             *
             *  If MISSING, then SO_LINGER is disabled - the default. This means a socket close
             *  will make best effort delivery, but close wont BLOCK until data delivered.
             *
             *  If PRESENT, then the value is the number of seconds close will wait to finish
             *  delivering data.
             *
             *  \note   If you see (netstat -an) lots of TIME_WAIT state sockets, you can generally
             *          eliminate them with SetLinger (0); However, this is generally not advisable,
             *          as close() will send a  RST (connection reset) which indicates an error condition.
             *          (see http://stackoverflow.com/questions/3757289/tcp-option-so-linger-zero-when-its-required)
             *
             *  @see SetLinger ()
             */
            nonvirtual optional<int> GetLinger () const;

        public:
            /**
             *  @see GetLinger ()
             */
            nonvirtual void SetLinger (const optional<int>& linger) const;

        public:
            /**
             *  \brief Is this socket configured to use TCP keepalives (SO_KEEPALIVE)
             *
             *  @see https://linux.die.net/man/3/setsockopt (SO_KEEPALIVE)
             */
            nonvirtual KeepAliveOptions GetKeepAlives () const;

        public:
            /**
             *  @see GetKeepAlives
             *  @see KeepAliveOptions
             */
            nonvirtual void SetKeepAlives (const KeepAliveOptions& keepalive) const;

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
        };

        /**
         */
        class _IRep : public Socket::_IRep {
        public:
            virtual ~_IRep () = default;
            // return true on success, and false on failure (if e != nullptr) - if e == nullptr - just throws as normal
            virtual void             Connect (const SocketAddress& sockAddr, const optional<Time::Duration>& timeout) const = 0;
            virtual size_t           Read (byte* intoStart, byte* intoEnd) const                                            = 0;
            virtual optional<size_t> ReadNonBlocking (byte* intoStart, byte* intoEnd) const                                 = 0;
            virtual void             Write (const byte* start, const byte* end) const                                       = 0;
            virtual optional<IO::Network::SocketAddress> GetPeerAddress () const                                            = 0;
            virtual optional<Time::DurationSeconds>      GetAutomaticTCPDisconnectOnClose () const                          = 0;
            virtual void             SetAutomaticTCPDisconnectOnClose (const optional<Time::DurationSeconds>& waitFor)      = 0;
            virtual KeepAliveOptions GetKeepAlives () const                                                                 = 0;
            virtual void             SetKeepAlives (const KeepAliveOptions& keepAliveOptions)                               = 0;
        };

        Ptr New (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol = {});

        /**
         *  \brief create a ConnectionOrientedStreamSocket::Ptr, and connect to the given address.
         *
         *  Shorthand for:
         *          ConnectionOrientedStreamSocket::Ptr  s = ConnectionOrientedStreamSocket::New (sockAddr.GetAddressFamily (), Socket::STREAM);
         *          s.Connect (someSocketAddress);
         *          return s;
         */
        Ptr NewConnection (const SocketAddress& sockAddr);

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
    struct WaitForIOReady_Traits<IO::Network::ConnectionOrientedStreamSocket::Ptr> {
        using HighLevelType = IO::Network::ConnectionOrientedStreamSocket::Ptr;
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
#include "ConnectionOrientedStreamSocket.inl"

#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedStreamSocket_h_*/
