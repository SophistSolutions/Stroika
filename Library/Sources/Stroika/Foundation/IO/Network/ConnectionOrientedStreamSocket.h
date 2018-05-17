/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedStreamSocket_h_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedStreamSocket_h_ 1

#include "../../StroikaPreComp.h"

#include "Socket.h"

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                /**
                 *  \brief ConnectionOrientedStreamSocket is typically a tcp stream, either setup with Connect, or ConnectionOrientedMasterSocket::Accept ()
                 *
                 *  \par Example Usage
                 *      \code
                 *          ConnectionOrientedStreamSocket::Ptr  s = ConnectionOrientedStreamSocket { Socket::INET, Socket::STREAM };
                 *          s.Connect (someSocketAddress);
                 *      \endcode
                 *
                 *  \par Example Usage
                 *      \code
                 *          ConnectionOrientedMasterSocket::Ptr ms ConnectionOrientedMasterSocket { Socket::INET, Socket::STREAM };
                 *          ms.Bind (addr);
                 *          ms.Listen (backlog);
                 *          ConnectionOrientedStreamSocket::Ptr      newConnection = ms.Accept ();
                 *      \endcode
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
                 */
                class ConnectionOrientedStreamSocket : public Socket {
                private:
                    using inherited = Socket;

                protected:
                    class _IRep;

                public:
                    class Ptr;

                public:
                    /**
                     *  \note - use ConnectionOrientedStreamSocket::Attach () instead of a normal constructor to emphasize that
                     *          The newly created object takes ownership of the socket.
                     *
                     *  \note unless you call @Detatch() - socket is CLOSED in DTOR of rep, so when final reference goes away
                     *
                     *  \note ConnectionOrientedStreamSocket is not copyable, but it can be copied into a ConnectionOrientedStreamSocket::Ptr or
                     *        Socket::Ptr. This is critical to save them in a container, for example.
                     *
                     *  TODO:
                     *      @todo - maybe - ConnectionOrientedStreamSocket should take OVERLOAD with CTOR having socket-address-to-connect-to
                     */
                    ConnectionOrientedStreamSocket ()                                        = delete;
                    ConnectionOrientedStreamSocket (const ConnectionOrientedStreamSocket& s) = delete;
                    ConnectionOrientedStreamSocket (ConnectionOrientedStreamSocket&& s)      = delete;

                public:
                    static Ptr New (SocketAddress::FamilyType family, Type socketKind, const Optional<IPPROTO>& protocol = {});

                public:
                    /**
                     *  For copyability, use ConnectionOrientedStreamSocket::Ptr for assigned-to type.
                     */
                    nonvirtual ConnectionOrientedStreamSocket& operator= (ConnectionOrientedStreamSocket&& s) = delete;
                    nonvirtual ConnectionOrientedStreamSocket& operator= (const ConnectionOrientedStreamSocket& s) = delete;

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

                public:
                    /**
                     */
                    struct KeepAliveOptions {
                        bool fEnabled{};
#if qPlatform_Linux or qPlatform_Windows
                        Optional<unsigned int>              fMaxProbesSentBeforeDrop;              // https://linux.die.net/man/7/tcp TCP_KEEPCNT
                        Optional<Time::DurationSecondsType> fTimeIdleBeforeSendingKeepalives;      // https://linux.die.net/man/7/tcp TCP_KEEPIDLE
                        Optional<Time::DurationSecondsType> fTimeBetweenIndividualKeepaliveProbes; // https://linux.die.net/man/7/tcp TCP_KEEPINTVL
#endif
                        /**
                         *  @see Characters::ToString ();
                         */
                        nonvirtual Characters::String ToString () const;
                    };
                };

                /**
                 *  \par Example Usage
                 *      \code
                 *          ConnectionOrientedStreamSocket::Ptr      s  = ConnectionOrientedStreamSocket (Socket::INET, Socket::STREAM);
                 *          s.Connect (someSocketAddress);
                 *          Sequence<ConnectionOrientedStreamSocket::Ptr> l;  // cannot do Sequence<ConnectionOrientedStreamSocket> cuz not copyable
                 *          l.push_back (s);
                 *      \endcode
                 *
                 *  \note Since ConnectionOrientedStreamSocket::Ptr is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
                 *        the underlying thread object.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
                 *          Note that we make Read, ReadNonBlocking, Connect, and Write () all const, so that they can be each called at the same time from different threads as other methods
                 *          like GetPeerAddress ().
                 *
                 *          It is \em NOT however, legal to call Read (or ReadNonBlocking) at the same time from two different threads, nor Write at the same time from two
                 *          different threads, nor Connect while already connected, nor Read/ReadNonBlocking/Write while not Connected (and so therefore not legal to call
                 *          Connect while calling Read/Write).
                 */
                class ConnectionOrientedStreamSocket::Ptr : public Socket::Ptr {
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
                     *  @see Socket::Close () - plus optionally handles GetAutomaticTCPDisconnectOnClose
                     *
                     *  \note - you dont have to use this Close () - using the base class close does the same thing,
                     *          but these docs are here just to make that more clear - the extra close functionality.
                     *
                     *  @see GetAutomaticTCPDisconnectOnClose - if set - Close automatically calls Shutdown () for connection-oriented sockets.
                     */
                    nonvirtual void Close () const;

                public:
                    /**
                     *  Throws on failure to connect. Else leaves the socket in a connected state.
                     *
                     *  \note ***Cancelation Point***
                     */
                    nonvirtual void Connect (const SocketAddress& sockAddr) const;

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
                    nonvirtual size_t Read (Byte* intoStart, Byte* intoEnd) const;

                public:
                    /**
                     *  \brief Non-blocking read: return {} if no data available, 0 on EOF.
                     *
                     *  \note Though Read () is a const method, can be done concurrently with most other const ConnectionOrientedStreamSocket methods,
                     *        It is illegal (because useless and confusing) to do two reads (or ReadNonBlocking) at the same time. Read and Write maybe
                     *        done simultaneously, from different threads.
                     *
                     *  \note if intoStart == nullptr, then dont actually read, but return the number of bytes available.
                     */
                    nonvirtual Memory::Optional<size_t> ReadNonBlocking (Byte* intoStart, Byte* intoEnd) const;

                public:
                    /**
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     */
                    nonvirtual void Write (const Byte* start, const Byte* end) const;

                public:
                    /**
                     *  If there is a socket connected to the other side, return that peer's socket address.
                     */
                    nonvirtual Optional<IO::Network::SocketAddress> GetPeerAddress () const;

                public:
                    /**
                     *  Automatically call Shutdown () when closing socket, and Wait this number of seconds to recieve the
                     *  peer's close acknowledgment. If missing, don't automatically call Shutdown, nor do any waiting for the acknowledgment.
                     *
                     *  @see SetAutomaticTCPDisconnectOnClose ()
                     */
                    nonvirtual Optional<Time::DurationSecondsType> GetAutomaticTCPDisconnectOnClose () const;

                public:
                    /**
                     *  @see GetAutomaticTCPDisconnectOnClose ()
                     */
                    nonvirtual void SetAutomaticTCPDisconnectOnClose (const Optional<Time::DurationSecondsType>& waitFor) const;

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
                    nonvirtual Optional<int> GetLinger () const;

                public:
                    /**
                     *  @see GetLinger ()
                     */
                    nonvirtual void SetLinger (const Optional<int>& linger) const;

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

                private:
                    friend class ConnectionOrientedStreamSocket;
                };

                /**
                 */
                class ConnectionOrientedStreamSocket::_IRep : public Socket::_IRep {
                public:
                    virtual ~_IRep ()                                                                                                                  = default;
                    virtual void                                 Connect (const SocketAddress& sockAddr) const                                         = 0;
                    virtual size_t                               Read (Byte* intoStart, Byte* intoEnd) const                                           = 0;
                    virtual Memory::Optional<size_t>             ReadNonBlocking (Byte* intoStart, Byte* intoEnd) const                                = 0;
                    virtual void                                 Write (const Byte* start, const Byte* end) const                                      = 0;
                    virtual Optional<IO::Network::SocketAddress> GetPeerAddress () const                                                               = 0;
                    virtual Optional<Time::DurationSecondsType>  GetAutomaticTCPDisconnectOnClose () const                                             = 0;
                    virtual void                                 SetAutomaticTCPDisconnectOnClose (const Optional<Time::DurationSecondsType>& waitFor) = 0;
                    virtual KeepAliveOptions                     GetKeepAlives () const                                                                = 0;
                    virtual void                                 SetKeepAlives (const KeepAliveOptions& keepAliveOptions)                              = 0;
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConnectionOrientedStreamSocket.inl"

#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedStreamSocket_h_*/
