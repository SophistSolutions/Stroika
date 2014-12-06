/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <cstdlib>
#include    <sys/types.h>

#if     qPlatform_Windows
#include    <winsock2.h>
#include    <ws2tcpip.h>
#include    <io.h>
#elif   qPlatform_POSIX
#include    <unistd.h>
#include    <sys/socket.h>
#include    <netdb.h>
#include    <sys/ioctl.h>
#endif

#include    "../../Characters/String_Constant.h"
#include    "../../Debug/Trace.h"
#include    "../../Execution/Sleep.h"
#include    "../../Execution/Thread.h"
#include    "../../Execution/ErrNoException.h"
#if     qPlatform_Windows
#include "../../../Foundation/Execution/Platform/Windows/Exception.h"
#include    "Platform/Windows/WinSock.h"
#endif
#include    "../../Memory/BlockAllocated.h"

#include    "Socket.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;



namespace   {
    /*
     *  Internally we use this -1 value to mean invalid socket, but keep that a private implementation
     *  detail, since I'm not sure it will be good for all socket implementations?
     */
#if     qPlatform_Windows
    constexpr   Socket::PlatformNativeHandle    kINVALID_NATIVE_HANDLE_  =   INVALID_SOCKET;
#elif   qPlatform_POSIX
    constexpr   Socket::PlatformNativeHandle    kINVALID_NATIVE_HANDLE_  =   -1; // right value??
#endif
}




namespace {
    template    <typename   T>
    void    BreakWriteIntoParts_ (const T* start, const T* end, size_t maxSendAtATime, const function<size_t(const T*, const T*)>& writeFunc)
    {
        ptrdiff_t   amountToSend = end - start;
        ptrdiff_t   amountRemainingToSend = amountToSend;
        const T*    remainingSendFrom = start;
        while (amountRemainingToSend > 0) {
            size_t  amountToSend    =   min<size_t> (maxSendAtATime, amountRemainingToSend);
            size_t  amountSent  =   writeFunc (remainingSendFrom, remainingSendFrom + amountToSend);
            Assert (amountSent <= amountToSend);
            Assert (static_cast<size_t> (amountRemainingToSend) >= amountSent);
            amountRemainingToSend -= amountSent;
            remainingSendFrom += amountSent;
        }
    }
}

#if     qPlatform_Windows
namespace {
    template    <typename N>
    inline  void    ThrowIf_Windows_SOCKET_ERROR_ (N returnCode)
    {
        if (returnCode == SOCKET_ERROR ) {
            Execution::Platform::Windows::Exception::DoThrow (::WSAGetLastError ());
        }
    }
}
#endif




namespace   {

    struct  REALSOCKET_ : public Socket {
        class   Rep_ : public Socket::_Rep {
        public:
            Socket::PlatformNativeHandle    fSD_;
        public:
            DECLARE_USE_BLOCK_ALLOCATION(Rep_);
        public:
            Rep_ (Socket::PlatformNativeHandle sd)
                : fSD_ (sd)
            {
            }
            ~Rep_ ()
            {
                if (fSD_ != kINVALID_NATIVE_HANDLE_) {
                    Close ();
                }
            }
            virtual void    Close () override
            {
                if (fSD_ != kINVALID_NATIVE_HANDLE_) {
#if     qPlatform_Windows
                    ::closesocket (fSD_);
#elif   qPlatform_POSIX
                    ::close (fSD_);
#else
                    AssertNotImplemented ();
#endif
                    fSD_ = kINVALID_NATIVE_HANDLE_;
                }
            }
            virtual size_t  Read (Byte* intoStart, Byte* intoEnd) override
            {
                // Must do erorr checking and throw exceptions!!!
#if     qPlatform_Windows
                ///tmpahcl - a good start
                //return ::_read (fSD_, intoStart, intoEnd - intoStart);
                int flags = 0;
                int nBytesToRead = static_cast<int> (min<size_t> ((intoEnd - intoStart), numeric_limits<int>::max ()));
                int r = ::recv (fSD_, reinterpret_cast<char*> (intoStart), nBytesToRead, flags);
                if (r < 0) {
                    Execution::DoThrow (StringException (String_Constant (L"fix error")));
                }
                return size_t (r);// rough attempt...
#elif   qPlatform_POSIX
                return Execution::Handle_ErrNoResultInteruption ([this, &intoStart, &intoEnd] () -> int { return ::read (fSD_, intoStart, intoEnd - intoStart); });
#else
                AssertNotImplemented ();
#endif
            }
            virtual void    Write (const Byte* start, const Byte* end) override
            {
                // Must do erorr checking and throw exceptions!!!
#if     qPlatform_Windows
                /*
                 *  Note sure what the best way is here, but with WinSock, you cannot use write() directly. Sockets are not
                 *  file descriptors in windows implemenation.
                 *      WONT WORK:
                 *          int       n   =   ::_write (fSD_, start, end - start);
                 */
                size_t maxSendAtATime = getsockopt<unsigned int> (SOL_SOCKET, SO_MAX_MSG_SIZE);
                BreakWriteIntoParts_<Byte> (
                    start,
                    end,
                    maxSendAtATime,
                [this, maxSendAtATime] (const Byte * start, const Byte * end) -> size_t {
                    Require (static_cast<size_t> (end - start) <= maxSendAtATime);
                    Assert ((end - start) < numeric_limits<int>::max ());
                    int len  = static_cast<int> (end - start);
                    int flags = 0;
                    int n = ::send (fSD_, reinterpret_cast<const char*> (start), len, flags);
                    ThrowIf_Windows_SOCKET_ERROR_ (n);
                    Assert (0 <= n and n <= (end - start));
                    return static_cast<size_t> (n);
                }
                );
#elif   qPlatform_POSIX
                int     n   =   Execution::Handle_ErrNoResultInteruption ([this, &start, &end] () -> int { return ::write (fSD_, start, end - start); });
#else
                AssertNotImplemented ();
#endif
            }
            virtual void  SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr) override
            {
                // Must do erorr checking and throw exceptions!!!
                sockaddr sa = sockAddr.As<sockaddr> ();
#if     qPlatform_Windows
                Require (end - start < numeric_limits<int>::max ());
                Execution::ThrowErrNoIfNegative (::sendto (fSD_, reinterpret_cast<const char*> (start), static_cast<int> (end - start), 0, reinterpret_cast<sockaddr*> (&sa), sizeof(sa)));
#elif   qPlatform_POSIX
                Execution::ThrowErrNoIfNegative (Execution::Handle_ErrNoResultInteruption ([this, &start, &end, &sa] () -> int { return ::sendto (fSD_, reinterpret_cast<const char*> (start), end - start, 0, reinterpret_cast<sockaddr*> (&sa), sizeof(sa)); }));
#else
                AssertNotImplemented ();
#endif
            }
            virtual size_t    ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress) override
            {
                RequireNotNull (fromAddress);
                // Must do erorr checking and throw exceptions!!!
                sockaddr    sa;
                socklen_t   salen   =   sizeof(sa);
#if     qPlatform_Windows
                Require (intoEnd - intoStart < numeric_limits<int>::max ());
                size_t result = static_cast<size_t> (Execution::ThrowErrNoIfNegative (::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), static_cast<int> (intoEnd - intoStart), flag, &sa, &salen)));
                *fromAddress = sa;
                return result;
#elif   qPlatform_POSIX
                size_t result = static_cast<size_t> (Execution::ThrowErrNoIfNegative (Execution::Handle_ErrNoResultInteruption ([this, &intoStart, &intoEnd, &flag, &sa, &salen] () -> int { return ::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), intoEnd - intoStart, flag, &sa, &salen); })));
                *fromAddress = sa;
                return result;
#else
                AssertNotImplemented ();
#endif
            }
            virtual void    Listen (unsigned int backlog) override
            {
                Execution::Handle_ErrNoResultInteruption ([this, &backlog] () -> int { return ::listen (fSD_, backlog); });
            }
            virtual Socket  Accept () override
            {
                // HACK - not right.... - Need to find a way to get interupted when abort is called
#if     qPlatform_POSIX && 0
                fd_set  fs;
                FD_ZERO (&fs);
                FD_SET (fSD_, &fs);
                timeval timeout;
                memset (&timeout, 0, sizeof (timeout));
                timeout.tv_sec  = 5;
                int maxSD   =   fSD_;
#endif

                sockaddr    peer;
                memset (&peer, 0, sizeof (peer));

AGAIN:
                Execution::CheckForThreadAborting ();
                socklen_t   sz = sizeof (peer);
                Socket::PlatformNativeHandle    r = ::accept (fSD_, &peer, &sz);
                // must update Socket object so CTOR also takes (optional) sockaddr (for the peer - mostly to answer  other quesiutona later)
                if (r < 0) {
                    // HACK - so we get interuptabilitiy.... MUST IMPROVE!!!
                    if (errno == EAGAIN or errno == EINTR/* or errno == EWOULDBLOCK*/) {
                        // DONT THINK SLEEP NEEDED ANYMORE - NOR WEOUDBLCOKExecution::Sleep(1.0);
                        goto AGAIN;
                    }
                }
#if     qPlatform_Windows
                return Socket::Attach (r);
#elif   qPlatform_POSIX
                return Socket::Attach (Execution::ThrowErrNoIfNegative (r));
#endif
            }
            virtual void    JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) override
            {
                DbgTrace (L"Joining multicast group for address %s on interface %s", iaddr.As<String> ().c_str (), onInterface.As<String> ().c_str ());
                ip_mreq m;
                memset (&m, 0, sizeof (m));
                Assert (iaddr.GetAddressFamily () == InternetAddress::AddressFamily::V4);   // simple change to support IPV6 but NYI
                m.imr_multiaddr = iaddr.As<in_addr> ();
                m.imr_interface = onInterface.As<in_addr> ();
                Execution::ThrowErrNoIfNegative (::setsockopt (fSD_, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<const char*> (&m), sizeof (m)));
            }
            virtual void    LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) override
            {
                DbgTrace (L"Leaving multicast group for address %s on interface %s", iaddr.As<String> ().c_str (), onInterface.As<String> ().c_str ());
                ip_mreq m;
                memset (&m, 0, sizeof (m));
                Assert (iaddr.GetAddressFamily () == InternetAddress::AddressFamily::V4);   // simple change to support IPV6 but NYI
                m.imr_multiaddr = iaddr.As<in_addr> ();
                m.imr_interface = onInterface.As<in_addr> ();
                Execution::ThrowErrNoIfNegative (::setsockopt (fSD_, IPPROTO_IP, IP_DROP_MEMBERSHIP, reinterpret_cast<const char*> (&m), sizeof (m)));
            }
            virtual uint8_t     GetMulticastTTL ()  const override
            {
                return getsockopt<uint8_t> (IPPROTO_IP, IP_MULTICAST_TTL);
            }
            virtual void        SetMulticastTTL (uint8_t ttl)  override
            {
                char useTTL =   ttl;
                Execution::ThrowErrNoIfNegative (::setsockopt(fSD_, IPPROTO_IP, IP_MULTICAST_TTL, &useTTL, sizeof (useTTL)));
            }
            virtual bool        GetMulticastLoopMode ()  const override
            {
                return !!getsockopt<char> (IPPROTO_IP, IP_MULTICAST_LOOP);
            }
            virtual void        SetMulticastLoopMode (bool loopMode)  override
            {
                char loop   =   loopMode;
                Execution::ThrowErrNoIfNegative (::setsockopt(fSD_, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof (loop)));
            }
            virtual Socket::PlatformNativeHandle    GetNativeSocket () const override
            {
                return fSD_;
            }
            virtual void                    getsockopt (int level, int optname, void* optval, socklen_t* optlen) const override
            {
                // According to http://linux.die.net/man/2/getsockopt cannot return EINTR, so no need to retry
                RequireNotNull (optval);
                Execution::ThrowErrNoIfNegative (::getsockopt (fSD_, level, optname, reinterpret_cast<char*> (optval), optlen));
            }
            template    <typename RESULT_TYPE>
            inline  RESULT_TYPE getsockopt (int level, int optname) const
            {
                RESULT_TYPE r {};
                socklen_t   roptlen = sizeof (r);
                this->getsockopt (level, optname, &r, &roptlen);
                return r;
            }
        };
    };
}









/*
 ********************************************************************************
 ****************************** Network::Socket::_Rep ***************************
 ********************************************************************************
 */
Socket::_Rep::~_Rep ()
{
}







/*
 ********************************************************************************
 ********************************** Network::Socket *****************************
 ********************************************************************************
 */
Socket::Socket (SocketKind socketKind)
    : fRep_ ()
{
#if     qPlatform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#endif

    Socket::PlatformNativeHandle    sfd;
#if     qPlatform_POSIX
    Execution::ThrowErrNoIfNegative (sfd = Execution::Handle_ErrNoResultInteruption ([&socketKind]() -> int { return socket (AF_INET, static_cast<int> (socketKind), 0); }));
#else
    Execution::ThrowErrNoIfNegative (sfd = ::socket (AF_INET, static_cast<int> (socketKind), 0));
#endif
    fRep_ = std::move (shared_ptr<_Rep> (new REALSOCKET_::Rep_ (sfd)));
}

Socket::Socket (const shared_ptr<_Rep>& rep)
    : fRep_ (rep)
{
#if     qPlatform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#endif
}

Socket::Socket (shared_ptr<_Rep>&&  rep)
    : fRep_ (std::move (rep))
{
#if     qPlatform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#endif
}

Socket  Socket::Attach (PlatformNativeHandle sd)
{
    return Socket (shared_ptr<_Rep> (new REALSOCKET_::Rep_ (sd)));
}

Socket::PlatformNativeHandle    Socket::Detach ()
{
    PlatformNativeHandle    h   =   kINVALID_NATIVE_HANDLE_;
    if (fRep_.get () != nullptr) {
        h = fRep_->GetNativeSocket ();
    }
    fRep_.reset ();
    return h;
}

void    Socket::Bind (const SocketAddress& sockAddr, BindFlags bindFlags)
{
    Require (fRep_.get () != nullptr);  // Construct with Socket::Kind::SOCKET_STREAM?
    PlatformNativeHandle    sfd =    fRep_->GetNativeSocket ();

    {
        // Indicates that the rules used in validating addresses supplied in a bind(2) call should allow
        // reuse of local addresses. For AF_INET sockets this means that a socket may bind, except when
        // there is an active listening socket bound to the address. When the listening socket is bound
        // to INADDR_ANY with a specific port then it is not possible to bind to this port for any local address.
        int    on = bindFlags.fReUseAddr ? 1 : 0;
        Execution::Handle_ErrNoResultInteruption ([&sfd, &on] () -> int { return ::setsockopt(sfd, SOL_SOCKET,  SO_REUSEADDR, (char*)&on, sizeof(on)); });
    }

    sockaddr                useSockAddr =   sockAddr.As<sockaddr> ();
    ThrowErrNoIfNegative (Handle_ErrNoResultInteruption ([&sfd, &useSockAddr] () -> int { return ::bind (sfd, (sockaddr*)&useSockAddr, sizeof (useSockAddr));}));
}


#if 0
struct  BindProperties {
    static  const   String          kANYHOST;
    static  const   int             kANYPORT                =   0;
    static  const   int             kDefaultListenBacklog   =   100;
    String          fHostName;
    int             fPort;
    unsigned int    fListenBacklog;
    unsigned int    fExtraBindFlags;        // eg. SO_REUSEADDR
};
// throws if socket already bound or valid - only legal on empty socket
nonvirtual  void    OLD_Bind (const BindProperties& bindProperties);
void    Socket::OLD_Bind (const BindProperties& bindProperties)
{
    // Should this throw if already has something bound - already non-null!???
    if (fRep_.get () != nullptr and fRep_->GetNativeSocket () != kINVALID_NATIVE_HANDLE_) {
        throw Execution::StringException (String_Constant (L"Cannot bind an already bound socket"));
    }

    addrinfo hints;
    addrinfo* res = nullptr;
    memset ((void*)&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    string  tmp =   bindProperties.fHostName.AsUTF8<string> (); // BAD - SB tstring - or??? not sure what...
    try {
        Execution::Handle_ErrNoResultInteruption ([&tmp, &hints, &res] () -> int { return getaddrinfo (tmp.c_str (), nullptr, &hints, &res);});
    }
    catch (...) {
        // MUST FIX THIS - BROKEN - BUT LEAVE IGNORING ERRORS FOR NOW...
    }

    sockaddr_in useAddr;
    memset (&useAddr, 0, sizeof (useAddr));
    useAddr.sin_family = AF_INET;
    useAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    useAddr.sin_port = htons((short)bindProperties.fPort);

    Socket::PlatformNativeHandle sd;
#if     qPlatform_POSIX
    sd = Execution::Handle_ErrNoResultInteruption ([]() -> int { return socket (AF_INET, SOCK_STREAM, 0); });
#else
    sd = 0;
    AssertNotImplemented ();
#endif


    {
        // Indicates that the rules used in validating addresses supplied in a bind(2) call should allow
        // reuse of local addresses. For AF_INET sockets this means that a socket may bind, except when
        // there is an active listening socket bound to the address. When the listening socket is bound
        // to INADDR_ANY with a specific port then it is not possible to bind to this port for any local address.
        int    on = 1;
        Execution::Handle_ErrNoResultInteruption ([&sd, &on] () -> int { return ::setsockopt(sd, SOL_SOCKET,  SO_REUSEADDR, (char*)&on, sizeof(on)); });
    }

    Execution::Handle_ErrNoResultInteruption ([&sd, &useAddr] () -> int { return ::bind (sd, (sockaddr*)&useAddr, sizeof (useAddr));});

    fRep_  = shared_ptr<_Rep> (new REALSOCKET_::Rep_ (sd));
}
#endif

void    Socket::Close ()
{
    // not importnat to null-out, but may as well...
    if (fRep_.get () != nullptr) {
        fRep_->Close ();
        fRep_.reset ();
    }
}

bool    Socket::IsOpen () const
{
    if (fRep_.get () != nullptr) {
        return fRep_->GetNativeSocket () != kINVALID_NATIVE_HANDLE_;
    }
    return false;
}












/*
 ********************************************************************************
 ******************** Execution::ThrowErrNoIfNegative ***************************
 ********************************************************************************
 */
#if     qPlatform_Windows
namespace Stroika {
    namespace Foundation {
        namespace Execution {
            // this specialization needed because the winsock type for SOCKET is UNSIGNED so < 0 test doesn't work
            template    <>
            inline  IO::Network::Socket::PlatformNativeHandle    ThrowErrNoIfNegative (IO::Network::Socket::PlatformNativeHandle returnCode)
            {
                if (returnCode == kINVALID_NATIVE_HANDLE_) {
                    Execution::Platform::Windows::Exception::DoThrow (::WSAGetLastError ());
                }
                return returnCode;
            }
        }
    }
}
#endif

