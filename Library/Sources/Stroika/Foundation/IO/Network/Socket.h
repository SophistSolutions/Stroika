/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Socket_h_
#define _Stroika_Foundation_IO_Network_Socket_h_ 1

#include "../../StroikaPreComp.h"

#if qPlatform_POSIX
#include <sys/socket.h>
#endif

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Execution/ErrNoException.h"
#if qPlatform_Windows
#include "Platform/Windows/WinSock.h"
#endif
#include "../../Memory/Optional.h"

#include "SocketAddress.h"

#if qPlatform_Linux or qPlatform_MacOS
using IPPROTO = int;
#endif

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                using Characters::String;
                using Memory::Byte;
                using Memory::Optional;

                /**
                 *  Note that Socket acts a bit like a smart_ptr<> - to an underlying operating system object.
                 *  They can be assigned to one another, and those assigned copies all refer to the same
                 *  underlying object.
                 *
                 *  Closing one, closes them all (though overwriting one just has the effect of detatching
                 *  from the underlying socket.
                 *
                 *  When the last reference to an underlying socket represenation is lost, the native socket
                 *  is automatically closed (unless manually Detached first).
                 *
                 *  \note   select: Socket has no select method: instead use Execution::WaitForIOReady which
                 *          works transparently with sockets, sets of sockets, or other waitable objects.
                 *
                 *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
                 *
                 *
                 * TODO:
                 *      @todo   DOCUMENT 'SMARTPOINTER' class
                 *
                 *      @todo   In socket class, set CLOSE_ON_EXEC?
                 *
                 *      @todo   Document (or define new expcetion) thrown when operaiton done on CLOSED socket.
                 *              and acutally handle all the nullptr cases...
                 *
                 *      @todo   Document THREADSAFETY. ((@@todo - use AssertExternallySynchronized - both for this class and underlying rep)
                 *
                 *      @todo   See about socket 'connected' state, and the 'connect' operation.
                 *              And see about send/recv() API - and docuemnt about only working when
                 *              connected.
                 */
                class Socket {
                public:
/**
                     *  Platform Socket descriptor - file descriptor on unix (something like this on windoze)
                     */
#if qPlatform_Windows
                    using PlatformNativeHandle = SOCKET;
#else
                    using PlatformNativeHandle = int;
#endif
                protected:
                    class _IRep;

                public:
                    /**
                     */
                    enum class ProtocolFamily : int {
                        INET  = AF_INET,
                        INET6 = AF_INET6,
                    };
                    static constexpr ProtocolFamily INET  = ProtocolFamily::INET;
                    static constexpr ProtocolFamily INET6 = ProtocolFamily::INET6;

                public:
                    /**
                     * 'second arg' to ::socket() call - socket type
                     */
                    enum class Type : int {
                        STREAM = SOCK_STREAM,
                        DGRAM  = SOCK_DGRAM,
                        RAW    = SOCK_RAW,
                    };
                    static constexpr Type STREAM = Type::STREAM;
                    static constexpr Type DGRAM  = Type::DGRAM;
                    static constexpr Type RAW    = Type::RAW;

                public:
                    // Deprecated - v2.0a206
                    _Deprecated_ ("USE Type") typedef Type SocketKind;

                protected:
                    /**
                     *  Socket is now an abstract class. Use an explicit subclass like
                     *      o   ConnectionlessSocket
                     *      o   ConnectionOrientedSocket
                     *      o   ConnectionOrientedMasterSocket
                     */
                public:
                    Socket (const Socket& s) = default;
                    Socket (Socket&& s);

                protected:
                    Socket () = default;
                    Socket (shared_ptr<_IRep>&& rep);
                    Socket (const shared_ptr<_IRep>& rep);

                public:
                    ~Socket () = default;

                public:
                    /**
                     */
                    nonvirtual Socket& operator= (Socket&& s);
                    nonvirtual Socket& operator= (const Socket& s);

                public:
                    /**
                     *  Marks this Socket (and and sockets copied from it, before or after). This can be used
                     *  to prevent the underlying native socket from being closed.
                     */
                    nonvirtual PlatformNativeHandle Detach ();

                public:
                    /**
                     *  Return the second argument to the ::socket() call (type) or the result of getsockopt (SOL_SOCKET, SO_TYPE)
                     */
                    nonvirtual Type GetType () const;

                public:
                    /**
                     */
                    struct BindFlags {
                        bool fReUseAddr : 1; // SO_REUSEADDR
                        BindFlags (bool reUseAddr = false);
                    };

                public:
                    /**
                     *  Associate this socket object with the given address.
                     *
                     *  @todo   SB an overload taking just a port, and defaults to INADDRANY with that port.
                     *          Port is only thing really needed, but InternetAddress part provided as arg
                     *          too in case you want to bind to a particular interface.
                     *
                     *  @todo   CLARIFY if a socket can be bound to more than one address (and what about unbind)?
                     *
                     *  \note   This is usually just used on a ConnectionOrientedMasterSocket but may also be used
                     *          by a ConnectionlessSocket (e.g. with SSDP).
                     *
                     *  @see POSIX bind()
                     */
                    nonvirtual void Bind (const SocketAddress& sockAddr, BindFlags bindFlags = BindFlags ());

                public:
                    /**
                     * if bound (@see Bind ()) - to what local endpoint? Remember a computer can be multi-homed, and can be bound to ADDR_ANY, or a specific address (plus the port).
                     */
                    nonvirtual Optional<IO::Network::SocketAddress> GetLocalAddress () const;

                public:
                    enum class ShutdownTarget {
                        eReads,
                        eWrites,
                        eBoth,

                        eDEFAULT = eBoth,

                        Stroika_Define_Enum_Bounds (eReads, eBoth)
                    };
                    static constexpr ShutdownTarget eReads  = ShutdownTarget::eReads;
                    static constexpr ShutdownTarget eWrites = ShutdownTarget::eWrites;
                    static constexpr ShutdownTarget eBoth   = ShutdownTarget::eBoth;

                public:
                    /** 
                     *  @see GetAutomaticTCPDisconnectOnClose - if set - Close automatically calls Shutdown () for connection-oriented sockets.
                     *
                     *  @see https://msdn.microsoft.com/en-us/library/system.net.sockets.socket.shutdown(v=vs.110).aspx
                     *      When using a connection-oriented Socket, always call the Shutdown method before closing the Socket. 
                     *      This ensures that all data is sent and received on the connected socket before it is closed
                     *
                     *  @see http://pubs.opengroup.org/onlinepubs/9699919799/
                     *      The shutdown() function shall cause all or part of a full-duplex connection on the socket
                     *      associated with the file descriptor socket to be shut down.
                     *  
                     *      The shutdown() function disables subsequent send and/or receive operations on a socket, 
                     *      depending on the value of the how argument.
                     *
                     *  @see https://msdn.microsoft.com/en-us/library/windows/desktop/ms740481(v=vs.85).aspx
                     *      If the how parameter is SD_RECEIVE, subsequent calls to the recv function on the socket 
                     *      will be disallowed. This has no effect on the lower protocol layers. For TCP sockets, 
                     *      if there is still data queued on the socket waiting to be received, or data arrives subsequently,
                     *      the connection is reset, since the data cannot be delivered to the user. For UDP sockets,
                     *      incoming datagrams are accepted and queued. In no case will an ICMP error packet be generated.
                     *
                     *      If the how parameter is SD_SEND, subsequent calls to the send function are disallowed. 
                     *      For TCP sockets, a FIN will be sent after all data is sent and acknowledged by the receiver.
                     */
                    nonvirtual void Shutdown (ShutdownTarget shutdownTarget = ShutdownTarget::eDEFAULT);

                public:
                    /**
                     *  Note that Socket is an envelope class, and there could be multiple references to
                     *  the same underlying platform socket. But this closes ALL of them. It also removes the reference
                     *  to the underlying rep (meaning that some Socket envelopes COULD have a rep with an
                     *  underlying closed socket).
                     */
                    nonvirtual void Close ();

                public:
                    /**
                     *  A socket can be open or closed. Open is roughly analagous to non-null. A socket once closed
                     *  can never be 'Opened' - but you can assign a new Open socket to the Socket object.
                     *
                     *  @see Close
                     */
                    nonvirtual bool IsOpen () const;

                public:
                    /**
                     *  Return true iff the sOCKETS are the same.
                     *
                     *  This is like Compare() == 0.
                     *
                     *  \note   Two sockets compare equal iff their underlying native sockets are equal (@see GetNativeSocket)
                     *          This means you can have two Socket objects which compare equal by use of Attach().
                     */
                    nonvirtual bool Equals (const Socket& rhs) const;

                public:
                    /**
                     *
                     *  \note   Sockets are compared by their underlying native sockets (@see GetNativeSocket).
                     *          This means you can have two Socket objects which compare equal by use of Attach().
                     */
                    nonvirtual int Compare (const Socket& rhs) const;

                public:
                    /**
                     *  Return the native platform handle object associated with this socket
                     *  (typically an integer file descriptor)
                     */
                    nonvirtual PlatformNativeHandle GetNativeSocket () const;

                public:
                    /**
                     *  Usually the return value is an int, but the caller must specify the right type. This is a simple,
                     *  low level, wrapper on 'man 2 getsockopt'.
                     *
                     *  \note   this is usually unnecessary, somewhat dangerous (easy to mismatch types), and most common socket options
                     *          can be accessed via other methods (e.g. GetMulticastTTL ())
                     *
                     *  @see setsockopt
                     */
                    template <typename RESULT_TYPE>
                    nonvirtual RESULT_TYPE getsockopt (int level, int optname) const;

                public:
                    /**
                     *  This is a simple, low level, wrapper on 'man 2 setsockopt'.
                     *
                     *  \note   this is usually unnecessary, somewhat dangerous (easy to mismatch types), and most common socket options
                     *          can be accessed via other methods (e.g. SetMulticastTTL ())
                     *
                     *  \note   It maybe advisable to be explicit about the ARG_TYPE in the template, as constants like '3' may get misinterpretted,
                     *          and you must use the exactly correct type for the low level setsockopt API.
                     *
                     *  @see getsockopt
                     */
                    template <typename ARG_TYPE>
                    nonvirtual void setsockopt (int level, int optname, ARG_TYPE arg);

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual _IRep& _ref ();

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual const _IRep& _cref () const;

                private:
                    shared_ptr<_IRep> fRep_;
                };

                /**
                 *  operator indirects to Socket::Compare()
                 */
                bool operator< (const Socket& lhs, const Socket& rhs);

                /**
                 *  operator indirects to Socket::Compare()
                 */
                bool operator<= (const Socket& lhs, const Socket& rhs);

                /**
                 *  operator indirects to Socket::Equals()
                 */
                bool operator== (const Socket& lhs, const Socket& rhs);

                /**
                 *  operator indirects to Socket::Equals()
                 */
                bool operator!= (const Socket& lhs, const Socket& rhs);

                /**
                 *  operator indirects to Socket::Compare()
                 */
                bool operator>= (const Socket& lhs, const Socket& rhs);

                /**
                 *  operator indirects to Socket::Compare()
                 */
                bool operator> (const Socket& lhs, const Socket& rhs);

                /**
                 */
                class Socket::_IRep {
                public:
                    virtual ~_IRep ()                                                     = default;
                    virtual void Shutdown (ShutdownTarget shutdownTarget)                 = 0;
                    virtual void                                 Close ()                 = 0;
                    virtual Optional<IO::Network::SocketAddress> GetLocalAddress () const = 0;
                    virtual PlatformNativeHandle                 GetNativeSocket () const = 0;
                    virtual void getsockopt (int level, int optname, void* optval, socklen_t* optvallen) const = 0;
                    virtual void setsockopt (int level, int optname, const void* optval, socklen_t optvallen)  = 0;
                };

                /**
                 */
                class ConnectionlessSocket : public Socket {
                private:
                    using inherited = Socket;

                protected:
                    class _IRep;

                public:
                    /**
                     *  \par Example Usage
                     *      \code
                     *          ConnectionlessSocket      s  { Socket::INET, Socket::DGRAM };
                     *      \endcode
                     *
                     *  \req socketKind != SOCK_STREAM
                     */
                    ConnectionlessSocket () = default;
                    ConnectionlessSocket (ProtocolFamily family, Type socketKind, const Optional<IPPROTO>& protocol = {});
                    ConnectionlessSocket (ConnectionlessSocket&& s)      = default;
                    ConnectionlessSocket (const ConnectionlessSocket& s) = default;

                private:
                    ConnectionlessSocket (const shared_ptr<_IRep>& rep);

                public:
                    nonvirtual ConnectionlessSocket& operator= (ConnectionlessSocket&& s);
                    nonvirtual ConnectionlessSocket& operator= (const ConnectionlessSocket& s) = default;

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
                    static ConnectionlessSocket Attach (PlatformNativeHandle sd);

                public:
                    /**
                     */
                    nonvirtual void JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface = V4::kAddrAny);

                public:
                    /**
                     */
                    nonvirtual void LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface = V4::kAddrAny);

                public:
                    /**
                     *  This specifies the number of networks to traverse in sending the multicast message.
                     *  It defaults to 1.
                     */
                    nonvirtual uint8_t GetMulticastTTL () const;

                public:
                    /**
                     *  This specifies the number of networks to traverse in sending the multicast message.
                     *  It defaults to 1.
                     */
                    nonvirtual void SetMulticastTTL (uint8_t ttl);

                public:
                    /**
                     *  This determines whether the data sent will be looped back to sender host or not.
                     */
                    nonvirtual bool GetMulticastLoopMode ();

                public:
                    /**
                     *  This determines whether the data sent will be looped back to sender host or not.
                     */
                    nonvirtual void SetMulticastLoopMode (bool loopMode);

                public:
                    /**
                     *  @todo   Clarify distinctions between read/write and send/sendto/recv/recvfrom
                     */
                    nonvirtual void SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr);

                public:
                    /**
                     *  @todo   Clarify distinctions between read/write and send/sendto/recv/recvfrom
                     */
                    nonvirtual size_t ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSecondsType timeout = Time::kInfinite);

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual _IRep& _ref ();

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual const _IRep& _cref () const;
                };
                class ConnectionlessSocket::_IRep : public Socket::_IRep {
                public:
                    virtual ~_IRep () = default;

                    virtual void SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr) = 0;
                    virtual size_t ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSecondsType timeout) = 0;
                    virtual void JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)  = 0;
                    virtual void LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) = 0;
                    virtual uint8_t GetMulticastTTL () const          = 0;
                    virtual void SetMulticastTTL (uint8_t ttl)        = 0;
                    virtual bool GetMulticastLoopMode () const        = 0;
                    virtual void SetMulticastLoopMode (bool loopMode) = 0;
                };

                /**
                 *  \par Example Usage
                 *      \code
                 *          ConnectionOrientedSocket      s (Socket::INET, Socket::STREAM);
                 *          s.Connect (someSocketAddress);
                 *      \endcode
                 *
                 *  \par Example Usage
                 *      \code
                 *          ConnectionOrientedMasterSocket ms (Socket::INET, Socket::STREAM);
                 *          ms.Bind (addr);
                 *          ms.Listen (backlog);
                 *          ConnectionOrientedSocket      newConnection = ms.Accept ();
                 *      \endcode
                 */
                class ConnectionOrientedSocket : public Socket {
                private:
                    using inherited = Socket;

                protected:
                    class _IRep;

                public:
                    /**
                     *  \note - use ConnectionOrientedSocket::Attach () instead of a normal constructor to emphasize that
                     *          The newly created object takes ownership of the socket.
                     */
                    ConnectionOrientedSocket (ConnectionOrientedSocket&& s) = default;
                    ConnectionOrientedSocket (ProtocolFamily family, Type socketKind, const Optional<IPPROTO>& protocol = {});
                    ConnectionOrientedSocket (const ConnectionOrientedSocket& s) = default;

                private:
                    ConnectionOrientedSocket (const shared_ptr<_IRep>& rep);

                public:
                    nonvirtual ConnectionOrientedSocket& operator= (ConnectionOrientedSocket&& s);
                    nonvirtual ConnectionOrientedSocket& operator= (const ConnectionOrientedSocket& s) = default;

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
                    static ConnectionOrientedSocket Attach (PlatformNativeHandle sd);

                public:
                    /**
                     *  @see Socket::Close () - plus optionally handles GetAutomaticTCPDisconnectOnClose
                     *
                     *  \note - you dont have to use this Close () - using the base class close does the same thing,
                     *          but these docs are here just to make that more clear - the extra close functionality.
                     *
                     *  @see GetAutomaticTCPDisconnectOnClose - if set - Close automatically calls Shutdown () for connection-oriented sockets.
                     */
                    nonvirtual void Close ();

                public:
                    /**
                     *  Throws on failure to connect. Else leaves the socket in a connected state.
                     */
                    nonvirtual void Connect (const SocketAddress& sockAddr);

                public:
                    /**
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     */
                    nonvirtual size_t Read (Byte* intoStart, Byte* intoEnd);

                public:
                    /**
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     */
                    nonvirtual void Write (const Byte* start, const Byte* end);

                public:
                    /**
                     *  If there is a socket connected to the other side, return that peer's socket address.
                     */
                    nonvirtual Optional<IO::Network::SocketAddress> GetPeerAddress () const;

                public:
                    /**
                     *  Automatically call Shutdown () when closing socket, and Wait this number of seconds to recieve the
                     *  peers close acknowledgment. If missing, don't automatically call Shutdown, nor do any waiting.
                     *
                     *  @see SetAutomaticTCPDisconnectOnClose ()
                     */
                    nonvirtual Optional<Time::DurationSecondsType> GetAutomaticTCPDisconnectOnClose () const;

                public:
                    /**
                     *  @see GetAutomaticTCPDisconnectOnClose ()
                     */
                    nonvirtual void SetAutomaticTCPDisconnectOnClose (const Optional<Time::DurationSecondsType>& waitFor);

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
                    nonvirtual void SetLinger (const Optional<int>& linger);

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

                public:
                    /**
                     *  \brief Is this socket configured to use TCP keepalives (SO_KEEPALIVE)
                     *
                     *  @see https://linux.die.net/man/3/setsockopt (SO_KEEPALIVE)
                     */
                    nonvirtual KeepAliveOptions GetKeepAlives ();

                public:
                    /**
                     *  @see GetKeepAlives
                     *  @see KeepAliveOptions
                     */
                    nonvirtual void SetKeepAlives (const KeepAliveOptions& keepalive);

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual _IRep& _ref ();

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual const _IRep& _cref () const;
                };
                class ConnectionOrientedSocket::_IRep : public Socket::_IRep {
                public:
                    virtual ~_IRep ()                                    = default;
                    virtual void Connect (const SocketAddress& sockAddr) = 0;
                    virtual size_t Read (Byte* intoStart, Byte* intoEnd)    = 0;
                    virtual void Write (const Byte* start, const Byte* end) = 0;
                    virtual Optional<IO::Network::SocketAddress> GetPeerAddress () const                               = 0;
                    virtual Optional<Time::DurationSecondsType>  GetAutomaticTCPDisconnectOnClose () const             = 0;
                    virtual void SetAutomaticTCPDisconnectOnClose (const Optional<Time::DurationSecondsType>& waitFor) = 0;
                    virtual KeepAliveOptions GetKeepAlives () const                                                    = 0;
                    virtual void SetKeepAlives (const KeepAliveOptions& keepAliveOptions)                              = 0;
                };

                /**
                 *  This class is to be used with ConnectionOrientedSocket. You create a ConnectionOrientedMasterSocket, and
                 *  Bind () it, and Listen () on it, and the resulting sockets (from Accept()) are of type ConnectionOrientedSocket.
                 */
                class ConnectionOrientedMasterSocket : public Socket {
                private:
                    using inherited = Socket;

                protected:
                    class _IRep;

                public:
                    /**
                     *  \par Example Usage
                     *      \code
                     *          ConnectionOrientedMasterSocket ms (Socket::INET, Socket::STREAM);
                     *          ms.Bind (addr);
                     *          ms.Listen (backlog);
                     *      \endcode
                     */
                    ConnectionOrientedMasterSocket () = default;
                    ConnectionOrientedMasterSocket (ProtocolFamily family, Type socketKind, const Optional<IPPROTO>& protocol = {});
                    ConnectionOrientedMasterSocket (ConnectionOrientedMasterSocket&& s)      = default;
                    ConnectionOrientedMasterSocket (const ConnectionOrientedMasterSocket& s) = default;

                private:
                    ConnectionOrientedMasterSocket (const shared_ptr<_IRep>& rep);

                public:
                    nonvirtual ConnectionOrientedMasterSocket& operator= (ConnectionOrientedMasterSocket&& s);
                    nonvirtual ConnectionOrientedMasterSocket& operator= (const ConnectionOrientedMasterSocket& s) = default;

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
                    static ConnectionOrientedMasterSocket Attach (PlatformNativeHandle sd);

                public:
                    /**
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     *
                     *   throws on error, and otherwise means should call accept
                     */
                    nonvirtual void Listen (unsigned int backlog);

                public:
                    /**
                     *  After Listen() on a connected socket returns (not throws) - you can call Accept() on tha same
                     *  socket to allocate a NEW socket with the new connection stream.
                     *
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     */
                    nonvirtual ConnectionOrientedSocket Accept ();

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual _IRep& _ref ();

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual const _IRep& _cref () const;
                };
                class ConnectionOrientedMasterSocket::_IRep : public Socket::_IRep {
                public:
                    virtual ~_IRep ()                          = default;
                    virtual void Listen (unsigned int backlog) = 0;
                    virtual ConnectionOrientedSocket Accept () = 0;
                };
            }
        }
    }
}

#if qPlatform_Windows
namespace Stroika {
    namespace Foundation {
        namespace Execution {
            template <>
            IO::Network::Socket::PlatformNativeHandle ThrowErrNoIfNegative (IO::Network::Socket::PlatformNativeHandle returnCode);
        }
    }
}
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Socket.inl"

#endif /*_Stroika_Foundation_IO_Network_Socket_h_*/
