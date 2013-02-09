/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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

#include    "../../Execution/Sleep.h"
#include    "../../Execution/Thread.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Memory/BlockAllocated.h"


#include    "Socket.h"




using   namespace   Stroika::Foundation;
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






namespace   {

    struct  REALSOCKET_ : public Socket {
        class   Rep_ : public Socket::_Rep {
        public:
            Socket::PlatformNativeHandle    fSD_;
        public:
            DECLARE_USE_BLOCK_ALLOCATION(Rep_);
        public:
            Rep_ (Socket::PlatformNativeHandle sd)
                : fSD_ (sd) {
            }
            ~Rep_ () {
                if (fSD_ != kINVALID_NATIVE_HANDLE_) {
                    Close ();
                }
            }
            virtual void    Close () override {
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
            virtual size_t  Read (Byte* intoStart, Byte* intoEnd) override {
                // Must do erorr checking and throw exceptions!!!
#if     qPlatform_Windows
                AssertNotImplemented ();
                return 0;
                //return ::_read (fSD_, intoStart, intoEnd - intoStart);
#elif   qPlatform_POSIX
                return Execution::Handle_ErrNoResultInteruption ([this, &intoStart, &intoEnd] () -> int { return ::read (fSD_, intoStart, intoEnd - intoStart); });
#else
                AssertNotImplemented ();
#endif
            }
            virtual void    Write (const Byte* start, const Byte* end) override {
                // Must do erorr checking and throw exceptions!!!
#if     qPlatform_Windows
                AssertNotImplemented ();
                //int       n   =   ::_write (fSD_, start, end - start);
#elif   qPlatform_POSIX
                int     n   =   Execution::Handle_ErrNoResultInteruption ([this, &start, &end] () -> int { return ::write (fSD_, start, end - start); });
#else
                AssertNotImplemented ();
#endif
            }
            virtual void  SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr) override {
                // Must do erorr checking and throw exceptions!!!
                sockaddr sa = sockAddr.As<sockaddr> ();
#if     qPlatform_Windows
                Execution::ThrowErrNoIfNegative (::sendto (fSD_, reinterpret_cast<const char*> (start), end - start, 0, reinterpret_cast<sockaddr*> (&sa), sizeof(sa)));
#elif   qPlatform_POSIX
                Execution::ThrowErrNoIfNegative (Execution::Handle_ErrNoResultInteruption ([this, &start, &end, &sa] () -> int { return ::sendto (fSD_, reinterpret_cast<const char*> (start), end - start, 0, reinterpret_cast<sockaddr*> (&sa), sizeof(sa)); }));
#else
                AssertNotImplemented ();
#endif
            }
            virtual size_t    ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress) override {
                // Must do erorr checking and throw exceptions!!!
                sockaddr    sa;
                socklen_t   salen   =   sizeof(sa);
                int         result  =   0;
#if     qPlatform_Windows
                Execution::ThrowErrNoIfNegative (result = ::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), intoEnd - intoStart, flag, &sa, &salen));
#elif   qPlatform_POSIX
                Execution::ThrowErrNoIfNegative (result = Execution::Handle_ErrNoResultInteruption ([this, &intoStart, &intoEnd, &flag, &sa, &salen] () -> int { return ::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), intoEnd - intoStart, flag, &sa, &salen); }));
#else
                AssertNotImplemented ();
#endif
                return static_cast<size_t> (result);
            }
            virtual void    Listen (unsigned int backlog) override {
                Execution::Handle_ErrNoResultInteruption ([this, &backlog] () -> int { return ::listen (fSD_, backlog); });
            }
            virtual Socket  Accept () override {
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
                socklen_t   sz  =   sizeof (peer);
                Socket::PlatformNativeHandle    r = ::accept (fSD_, &peer, &sz);
                // must update Socket object so CTOR also takes (optional) sockaddr (for the peer - mostly to answer  other quesiutona later)
                if (r < 0) {
                    // HACK - so we get interuptabilitiy.... MUST IMPROVE!!!
                    if (errno == EAGAIN or errno == EINTR/* or errno == EWOULDBLOCK*/) {
                        // DONT THINK SLEEP NEEDED ANYMORE - NOR WEOUDBLCOKExecution::Sleep(1.0);
                        Execution::CheckForThreadAborting();
                        goto AGAIN;
                    }
                }
#if     qPlatform_Windows
                AssertNotImplemented ();
#elif   qPlatform_POSIX
                Execution::ThrowErrNoIfNegative (r);
#endif
                return Socket::Attach (r);
            }
            virtual void    JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) override {
                ip_mreq m;
                memset (&m, 0, sizeof (m));
                Assert (iaddr.GetAddressFamily () == InternetAddress::AddressFamily::V4);   // simple change to support IPV6 but NYI
                m.imr_multiaddr = iaddr.As<in_addr> ();
                m.imr_interface = onInterface.As<in_addr> ();
                Execution::ThrowErrNoIfNegative (::setsockopt (fSD_, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<const char*> (&m), sizeof (m)));
            }
            void    LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) override {
                ip_mreq m;
                memset (&m, 0, sizeof (m));
                Assert (iaddr.GetAddressFamily () == InternetAddress::AddressFamily::V4);   // simple change to support IPV6 but NYI
                m.imr_multiaddr = iaddr.As<in_addr> ();
                m.imr_interface = onInterface.As<in_addr> ();
                Execution::ThrowErrNoIfNegative (::setsockopt (fSD_, IPPROTO_IP, IP_DROP_MEMBERSHIP, reinterpret_cast<const char*> (&m), sizeof (m)));
            }
            virtual Socket::PlatformNativeHandle    GetNativeSocket () const override {
                return fSD_;
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
    Socket::PlatformNativeHandle    sfd;
#if     qPlatform_POSIX
    Execution::ThrowErrNoIfNegative (sfd = Execution::Handle_ErrNoResultInteruption ([&socketKind]() -> int { return socket (AF_INET, static_cast<int> (socketKind), 0); }));
#else
    Execution::ThrowErrNoIfNegative (sfd = ::socket (AF_INET, static_cast<int> (socketKind), 0));
#endif
    fRep_ = std::move (shared_ptr<_Rep> (DEBUG_NEW REALSOCKET_::Rep_ (sfd)));
}

Socket  Socket::Attach (PlatformNativeHandle sd)
{
    return Socket (shared_ptr<_Rep> (DEBUG_NEW REALSOCKET_::Rep_ (sd)));
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
        throw Execution::StringException (L"Cannot bind an already bound socket");
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

    fRep_  = shared_ptr<_Rep> (DEBUG_NEW REALSOCKET_::Rep_ (sd));
}
#endif

void    Socket::Listen (unsigned int backlog)
{
    fRep_->Listen (backlog);
}

Socket  Socket::Accept ()
{
    return fRep_->Accept ();
}

void    Socket::JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)
{
    fRep_->JoinMulticastGroup (iaddr, onInterface);
}

void    Socket::LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)
{
    fRep_->LeaveMulticastGroup (iaddr, onInterface);
}

size_t  Socket::Read (Byte* intoStart, Byte* intoEnd)
{
    return fRep_->Read (intoStart, intoEnd);
}

void    Socket::Write (const Byte* start, const Byte* end)
{
    fRep_->Write (start, end);
}

void    Socket::SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr)
{
    fRep_->SendTo (start, end, sockAddr);
}

size_t    Socket::ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress)
{
    return fRep_->ReceiveFrom (intoStart, intoEnd, flag, fromAddress);
}

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
