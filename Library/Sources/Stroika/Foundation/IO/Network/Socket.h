/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Socket_h_
#define _Stroika_Foundation_IO_Network_Socket_h_ 1

#include "../../StroikaPreComp.h"

#if qPlatform_POSIX
#include <sys/socket.h>
#endif

#if qPlatform_Linux
#include <netinet/in.h>
#include <netinet/ip.h>
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
                 * TODO:
                 *
                 *      @todo   In socket class, set CLOSE_ON_EXEC?
                 *
                 *      @todo   Document (or define new expcetion) thrown when operaiton done on CLOSED socket.
                 *              and acutally handle all the nullptr cases...
                 *
                 *      @todo   Document THREADSAFETY.
                 *
                 *      @todo   See about socket 'connected' state, and the 'connect' operation.
                 *              And see about send/recv() API - and docuemnt about only working when
                 *              connected.
                 */

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
                    class _Rep;

                public:
                    /**
                     */
                    enum class ProtocolFamily : int {
                        INET  = AF_INET,
                        INET6 = AF_INET6,
                    };

                public:
                    /**
                    * 'second arg' to ::socket() call - socket type
                    */
                    enum class SocketKind : int {
                        STREAM = SOCK_STREAM,
                        DGRAM  = SOCK_DGRAM,
                        RAW    = SOCK_RAW,
                    };

                public:
                    /**
                     *  Note - socket is CLOSED (filesystem close for now) in DTOR
                     *
                     *  \note copying a Socket just copies the 'smart pointer' to the underlying OS socket object.
                     *
                     *  TODO:
                     *          We will need an abstract Socket object, and maybe have  it refernce
                     *          counted so close can happen when last refernce goes
                     *          away!
                     */
                    Socket ();
                    Socket (ProtocolFamily family, SocketKind socketKind, const Optional<IPPROTO>& protocol = {});
                    Socket (SocketKind socketKind);
                    Socket (Socket&& s);
                    Socket (const Socket& s);

                protected:
                    explicit Socket (shared_ptr<_Rep>&& rep);
                    explicit Socket (const shared_ptr<_Rep>& rep);

                public:
                    ~Socket ();
                    nonvirtual Socket& operator= (Socket&& s);
                    nonvirtual Socket& operator= (const Socket& s);

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
                    static Socket Attach (PlatformNativeHandle sd);

                public:
                    /**
                     *  Marks this Socket (and and sockets copied from it, before or after). This can be used
                     *  to prevent the underlying native socket from being closed.
                     */
                    nonvirtual PlatformNativeHandle Detach ();

                public:
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
                     *  @see POSIX bind()
                     */
                    nonvirtual void Bind (const SocketAddress& sockAddr, BindFlags bindFlags = BindFlags ());

                public:
                    /**
                     */
                    nonvirtual Optional<IO::Network::SocketAddress> GetLocalAddress () const;

                public:
                    /**
                     *  If there is a socket connected to the other side, return that peer's socket address.
                     */
                    nonvirtual Optional<IO::Network::SocketAddress> GetPeerAddress () const;

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
                    nonvirtual uint8_t GetMulticastTTL ();

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
                     *  @see SetLinger()
                     */
                    nonvirtual Optional<int> GetLinger ();

                public:
                    /**
                     *
                     *  @see GetLinger()
                     */
                    nonvirtual void SetLinger (Optional<int> linger);

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
                     *
                     */
                    nonvirtual Socket Accept ();

                public:
                    /**
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     *
                     */
                    nonvirtual size_t Read (Byte* intoStart, Byte* intoEnd);

                public:
                    /**
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     *
                     */
                    nonvirtual void Write (const Byte* start, const Byte* end);

                public:
                    /**
                     *  @todo   Clarify distinctions between read/write and send/sendto/recv/recvfrom
                     *
                     */
                    nonvirtual void SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr);

                public:
                    /**
                     *  @todo   Clarify distinctions between read/write and send/sendto/recv/recvfrom
                     *
                     */
                    nonvirtual size_t ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress);

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
                     *
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
                    nonvirtual RESULT_TYPE getsockopt (int level, int optname);

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

                private:
                    shared_ptr<_Rep> fRep_;
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
                class Socket::_Rep {
                public:
                    virtual ~_Rep ()        = default;
                    virtual void   Close () = 0;
                    virtual size_t Read (Byte* intoStart, Byte* intoEnd)    = 0;
                    virtual void Write (const Byte* start, const Byte* end) = 0;
                    virtual void SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr) = 0;
                    virtual size_t ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress) = 0;
                    virtual void Listen (unsigned int backlog)                            = 0;
                    virtual Socket                               Accept ()                = 0;
                    virtual Optional<IO::Network::SocketAddress> GetLocalAddress () const = 0;
                    virtual Optional<IO::Network::SocketAddress> GetPeerAddress () const  = 0;
                    virtual void JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)  = 0;
                    virtual void LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) = 0;
                    virtual uint8_t GetMulticastTTL () const                              = 0;
                    virtual void SetMulticastTTL (uint8_t ttl)                            = 0;
                    virtual bool GetMulticastLoopMode () const                            = 0;
                    virtual void SetMulticastLoopMode (bool loopMode)                     = 0;
                    virtual KeepAliveOptions GetKeepAlives () const                       = 0;
                    virtual void SetKeepAlives (const KeepAliveOptions& keepAliveOptions) = 0;
                    virtual Optional<int> GetLinger ()                                    = 0;
                    virtual void SetLinger (Optional<int> linger)                         = 0;
                    virtual PlatformNativeHandle GetNativeSocket () const                 = 0;
                    virtual void getsockopt (int level, int optname, void* optval, socklen_t* optvallen) const = 0;
                    virtual void setsockopt (int level, int optname, void* optval, socklen_t optvallen) const  = 0;
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
