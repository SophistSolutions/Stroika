/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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
#if     qPlatform_POSIX
    constexpr   Socket::PlatformNativeHandle    kINVALID_NATIVE_HANDLE_  =   -1; // right value??
#elif   qPlatform_Windows
    constexpr   Socket::PlatformNativeHandle    kINVALID_NATIVE_HANDLE_  =   INVALID_SOCKET;
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




namespace   {

    struct  REALSOCKET_ : public Socket {
        class   Rep_ : public Socket::_Rep {
        public:
            Socket::PlatformNativeHandle    fSD_;
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
#if     qPlatform_POSIX
                    ::close (fSD_);
#elif   qPlatform_Windows
                    ::closesocket (fSD_);
#else
                    AssertNotImplemented ();
#endif
                    fSD_ = kINVALID_NATIVE_HANDLE_;
                }
            }
            virtual size_t  Read (Byte* intoStart, Byte* intoEnd) override
            {
#if     qPlatform_POSIX
                return ThrowErrNoIfNegative (Handle_ErrNoResultInteruption ([this, &intoStart, &intoEnd] () -> int { return ::read (fSD_, intoStart, intoEnd - intoStart); }));
#elif   qPlatform_Windows
                ///tmpahcl - a good start
                //return ::_read (fSD_, intoStart, intoEnd - intoStart);
                int flags = 0;
                int nBytesToRead = static_cast<int> (min<size_t> ((intoEnd - intoStart), numeric_limits<int>::max ()));
                return static_cast<size_t> (ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::recv (fSD_, reinterpret_cast<char*> (intoStart), nBytesToRead, flags)));// rough attempt...
#else
                AssertNotImplemented ();
#endif
            }
            virtual void    Write (const Byte* start, const Byte* end) override
            {
#if     qPlatform_POSIX
                // @todo - maybe check n bytes written and write more - see API docs! But this is VERY BAD -- LGP 2015-10-18
                int     n   =   Handle_ErrNoResultInteruption ([this, &start, &end] () -> int { return ::write (fSD_, start, end - start); });
                ThrowErrNoIfNegative (n);
#elif   qPlatform_Windows
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
                    ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (n);
                    Assert (0 <= n and n <= (end - start));
                    return static_cast<size_t> (n);
                }
                );
#else
                AssertNotImplemented ();
#endif
            }
            virtual void  SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr) override
            {
                sockaddr sa = sockAddr.As<sockaddr> ();
#if     qPlatform_POSIX
                ThrowErrNoIfNegative (Handle_ErrNoResultInteruption ([this, &start, &end, &sa] () -> int { return ::sendto (fSD_, reinterpret_cast<const char*> (start), end - start, 0, reinterpret_cast<sockaddr*> (&sa), sizeof(sa)); }));
#elif   qPlatform_Windows
                Require (end - start < numeric_limits<int>::max ());
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::sendto (fSD_, reinterpret_cast<const char*> (start), static_cast<int> (end - start), 0, reinterpret_cast<sockaddr*> (&sa), sizeof(sa)));
#else
                AssertNotImplemented ();
#endif
            }
            virtual size_t    ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress) override
            {
                RequireNotNull (fromAddress);
                sockaddr    sa;
                socklen_t   salen   =   sizeof(sa);
#if     qPlatform_POSIX
#if     qCompilerAndStdLib_AIX_GCC_TOC_Inline_Buggy
                size_t result = static_cast<size_t> (ThrowErrNoIfNegative (Handle_ErrNoResultInteruption ([this, &intoStart, &intoEnd, &flag, &sa, &salen] () -> int { return ::nrecvfrom (fSD_, reinterpret_cast<char*> (intoStart), intoEnd - intoStart, flag, &sa, &salen); })));
#else
                size_t result = static_cast<size_t> (ThrowErrNoIfNegative (Handle_ErrNoResultInteruption ([this, &intoStart, &intoEnd, &flag, &sa, &salen] () -> int { return ::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), intoEnd - intoStart, flag, &sa, &salen); })));
#endif
                *fromAddress = sa;
                return result;
#elif   qPlatform_Windows
                Require (intoEnd - intoStart < numeric_limits<int>::max ());
                size_t result = static_cast<size_t> (ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), static_cast<int> (intoEnd - intoStart), flag, &sa, &salen)));
                *fromAddress = sa;
                return result;
#else
                AssertNotImplemented ();
#endif
            }
            virtual void    Listen (unsigned int backlog) override
            {
#if     qPlatform_POSIX
                ThrowErrNoIfNegative (Handle_ErrNoResultInteruption ([this, &backlog] () -> int { return ::listen (fSD_, backlog); }));
#elif   qPlatform_Windows
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::listen (fSD_, backlog));
#else
                AssertNotImplemented ();
#endif

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

                sockaddr    peer {};

AGAIN:
                CheckForThreadInterruption ();
                socklen_t   sz = sizeof (peer);
#if     qCompilerAndStdLib_AIX_GCC_TOC_Inline_Buggy
                Socket::PlatformNativeHandle    r = ::naccept (fSD_, &peer, &sz);
#else
                Socket::PlatformNativeHandle    r = ::accept (fSD_, &peer, &sz);
#endif
                // must update Socket object so CTOR also takes (optional) sockaddr (for the peer - mostly to answer  other quesiutona later)
                if (r < 0) {
                    // HACK - so we get interuptabilitiy.... MUST IMPROVE!!!
                    if (errno == EAGAIN or errno == EINTR/* or errno == EWOULDBLOCK*/) {
                        // DONT THINK SLEEP NEEDED ANYMORE - NOR WEOUDBLCOKExecution::Sleep(1.0);
                        goto AGAIN;
                    }
                }
#if     qPlatform_POSIX
                return Socket::Attach (ThrowErrNoIfNegative (r));
#elif       qPlatform_Windows
                return Socket::Attach (r);
#endif
            }
            virtual void    JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) override
            {
                DbgTrace (L"Joining multicast group for address %s on interface %s", iaddr.As<String> ().c_str (), onInterface.As<String> ().c_str ());
                ip_mreq m {};
                Assert (iaddr.GetAddressFamily () == InternetAddress::AddressFamily::V4);   // simple change to support IPV6 but NYI
                m.imr_multiaddr = iaddr.As<in_addr> ();
                m.imr_interface = onInterface.As<in_addr> ();
#if     qPlatform_POSIX
                ThrowErrNoIfNegative (::setsockopt (fSD_, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<const char*> (&m), sizeof (m)));
#elif   qPlatform_Windows
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::setsockopt (fSD_, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<const char*> (&m), sizeof (m)));
#else
                AssertNotImplemented ();
#endif
            }
            virtual void    LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) override
            {
                DbgTrace (L"Leaving multicast group for address %s on interface %s", iaddr.As<String> ().c_str (), onInterface.As<String> ().c_str ());
                ip_mreq m {};
                Assert (iaddr.GetAddressFamily () == InternetAddress::AddressFamily::V4);   // simple change to support IPV6 but NYI
                m.imr_multiaddr = iaddr.As<in_addr> ();
                m.imr_interface = onInterface.As<in_addr> ();
#if     qPlatform_POSIX
                ThrowErrNoIfNegative (::setsockopt (fSD_, IPPROTO_IP, IP_DROP_MEMBERSHIP, reinterpret_cast<const char*> (&m), sizeof (m)));
#elif   qPlatform_Windows
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::setsockopt (fSD_, IPPROTO_IP, IP_DROP_MEMBERSHIP, reinterpret_cast<const char*> (&m), sizeof (m)));
#else
                AssertNotImplemented ();
#endif
            }
            virtual uint8_t     GetMulticastTTL ()  const override
            {
                return getsockopt<uint8_t> (IPPROTO_IP, IP_MULTICAST_TTL);
            }
            virtual void        SetMulticastTTL (uint8_t ttl)  override
            {
                char useTTL =   ttl;
#if     qPlatform_POSIX
                ThrowErrNoIfNegative (::setsockopt (fSD_, IPPROTO_IP, IP_MULTICAST_TTL, &useTTL, sizeof (useTTL)));
#elif   qPlatform_Windows
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::setsockopt (fSD_, IPPROTO_IP, IP_MULTICAST_TTL, &useTTL, sizeof (useTTL)));
#else
                AssertNotImplemented ();
#endif
            }
            virtual bool        GetMulticastLoopMode ()  const override
            {
                return !!getsockopt<char> (IPPROTO_IP, IP_MULTICAST_LOOP);
            }
            virtual void        SetMulticastLoopMode (bool loopMode)  override
            {
                char loop   =   loopMode;
#if     qPlatform_POSIX
                ThrowErrNoIfNegative (::setsockopt (fSD_, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof (loop)));
#elif   qPlatform_Windows
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::setsockopt (fSD_, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof (loop)));
#else
                AssertNotImplemented ();
#endif
            }
            virtual  Optional<int>        GetLinger () override
            {
                linger lr = getsockopt<linger> (SOL_SOCKET, SO_LINGER);
                return lr.l_onoff ? lr.l_linger :  Optional<int> {};
            }
            inline  void        SetLinger (Optional<int> linger) override
            {
                struct linger so_linger {};
                if (linger) {
                    so_linger.l_onoff = true;
                    so_linger.l_linger = *linger;
                }
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::setsockopt (fSD_, SOL_SOCKET, SO_LINGER, reinterpret_cast<const char*> (&so_linger), sizeof (so_linger)));
            }
            virtual Socket::PlatformNativeHandle    GetNativeSocket () const override
            {
                return fSD_;
            }
            virtual void                    getsockopt (int level, int optname, void* optval, socklen_t* optlen) const override
            {
                // According to http://linux.die.net/man/2/getsockopt cannot return EINTR, so no need to retry
                RequireNotNull (optval);
#if     qPlatform_POSIX
                ThrowErrNoIfNegative (::getsockopt (fSD_, level, optname, reinterpret_cast<char*> (optval), optlen));
#elif   qPlatform_Windows
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::getsockopt (fSD_, level, optname, reinterpret_cast<char*> (optval), optlen));
#else
                AssertNotImplemented ();
#endif
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
    ThrowErrNoIfNegative (sfd = Handle_ErrNoResultInteruption ([&socketKind]() -> int { return socket (AF_INET, static_cast<int> (socketKind), 0); }));
#elif   qPlatform_Windows
    ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (sfd = ::socket (AF_INET, static_cast<int> (socketKind), 0));
#else
    AssertNotImplemented ();
#endif
    fRep_ = std::move (make_shared<REALSOCKET_::Rep_> (sfd));
}

Socket::Socket (const shared_ptr<_Rep>& rep)
    : fRep_ (rep)
{
#if     qPlatform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#endif
}

Socket::Socket (shared_ptr<_Rep>&& rep)
    : fRep_ (std::move (rep))
{
#if     qPlatform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#endif
}

Socket  Socket::Attach (PlatformNativeHandle sd)
{
    return Socket (make_shared<REALSOCKET_::Rep_> (sd));
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
        Handle_ErrNoResultInteruption ([&sfd, &on] () -> int { return ::setsockopt(sfd, SOL_SOCKET,  SO_REUSEADDR, (char*)&on, sizeof(on)); });
    }

    sockaddr                useSockAddr =   sockAddr.As<sockaddr> ();
#if     qPlatform_Windows
    ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::bind (sfd, (sockaddr*)&useSockAddr, sizeof (useSockAddr)));
#else
    // EACCESS reproted as FileAccessException - which is crazy confusing.
    // @todo - find a better way, but for now remap this...
    try {
        ThrowErrNoIfNegative (Handle_ErrNoResultInteruption ([&sfd, &useSockAddr] () -> int { return ::bind (sfd, (sockaddr*)&useSockAddr, sizeof (useSockAddr));}));
    }
    catch (const IO::FileAccessException&) {
        Throw (StringException (L"Cannot Bind to port"));
    }
#endif
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

    addrinfo hints {};
    addrinfo* res = nullptr;
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
    useAddr.sin_addr.s_addr = htonl (INADDR_ANY);               //NB no ':' cuz some systems use macro
    useAddr.sin_port = htons ((short)bindProperties.fPort);     //NB no ':' cuz some systems use macro

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

    fRep_  = make_shared<REALSOCKET_::Rep_> (sd);
}
#endif

void    Socket::Close ()
{
    // not important to null-out, but may as well...
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
                    Execution::Platform::Windows::Exception::Throw (::WSAGetLastError ());
                }
                return returnCode;
            }
        }
    }
}
#endif

