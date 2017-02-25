/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <cstdlib>
#include <sys/types.h>

#if qPlatform_Windows
#include <winsock2.h>

#include <io.h>
#include <ws2tcpip.h>
#elif qPlatform_POSIX
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "../../Characters/String_Constant.h"
#include "../../Characters/ToString.h"
#include "../../Debug/Trace.h"
#include "../../Execution/ErrNoException.h"
#include "../../Execution/Sleep.h"
#include "../../Execution/Thread.h"
#if qPlatform_Windows
#include "../../../Foundation/Execution/Platform/Windows/Exception.h"
#include "Platform/Windows/WinSock.h"
#endif
#include "../../IO/FileAccessException.h"
#include "../../Memory/BlockAllocated.h"

#include "Socket.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

namespace {
/*
     *  Internally we use this -1 value to mean invalid socket, but keep that a private implementation
     *  detail, since I'm not sure it will be good for all socket implementations?
     */
#if qPlatform_POSIX
    constexpr Socket::PlatformNativeHandle kINVALID_NATIVE_HANDLE_ = -1; // right value??
#elif qPlatform_Windows
    constexpr Socket::PlatformNativeHandle kINVALID_NATIVE_HANDLE_ = INVALID_SOCKET;
#endif
}

namespace {
    template <typename T>
    void BreakWriteIntoParts_ (const T* start, const T* end, size_t maxSendAtATime, const function<size_t (const T*, const T*)>& writeFunc)
    {
        ptrdiff_t amountToSend          = end - start;
        ptrdiff_t amountRemainingToSend = amountToSend;
        const T*  remainingSendFrom     = start;
        while (amountRemainingToSend > 0) {
            size_t amountToSend = min<size_t> (maxSendAtATime, amountRemainingToSend);
            size_t amountSent   = writeFunc (remainingSendFrom, remainingSendFrom + amountToSend);
            Assert (amountSent <= amountToSend);
            Assert (static_cast<size_t> (amountRemainingToSend) >= amountSent);
            amountRemainingToSend -= amountSent;
            remainingSendFrom += amountSent;
        }
    }
}

namespace {
    struct REALSOCKET_ : public Socket {
        class Rep_ : public Socket::_Rep {
        public:
            Socket::PlatformNativeHandle fSD_;

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
            virtual void Close () override
            {
                if (fSD_ != kINVALID_NATIVE_HANDLE_) {
#if qPlatform_POSIX
                    ::close (fSD_);
#elif qPlatform_Windows
                    ::closesocket (fSD_);
#else
                    AssertNotImplemented ();
#endif
                    fSD_ = kINVALID_NATIVE_HANDLE_;
                }
            }
            virtual size_t Read (Byte* intoStart, Byte* intoEnd) override
            {
#if qPlatform_POSIX
                return ThrowErrNoIfNegative (Handle_ErrNoResultInterruption ([this, &intoStart, &intoEnd]() -> int { return ::read (fSD_, intoStart, intoEnd - intoStart); }));
#elif qPlatform_Windows
                ///tmpahcl - a good start
                //return ::_read (fSD_, intoStart, intoEnd - intoStart);
                int flags        = 0;
                int nBytesToRead = static_cast<int> (min<size_t> ((intoEnd - intoStart), numeric_limits<int>::max ()));
                return static_cast<size_t> (ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::recv (fSD_, reinterpret_cast<char*> (intoStart), nBytesToRead, flags))); // rough attempt...
#else
                AssertNotImplemented ();
#endif
            }
            virtual void Write (const Byte* start, const Byte* end) override
            {
#if qPlatform_POSIX
                // @todo - maybe check n bytes written and write more - see API docs! But this is VERY BAD -- LGP 2015-10-18
                int n = Handle_ErrNoResultInterruption ([this, &start, &end]() -> int { return ::write (fSD_, start, end - start); });
                ThrowErrNoIfNegative (n);
#elif qPlatform_Windows
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
                    [this, maxSendAtATime](const Byte* start, const Byte* end) -> size_t {
                        Require (static_cast<size_t> (end - start) <= maxSendAtATime);
                        Assert ((end - start) < numeric_limits<int>::max ());
                        int len   = static_cast<int> (end - start);
                        int flags = 0;
                        int n     = ::send (fSD_, reinterpret_cast<const char*> (start), len, flags);
                        ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (n);
                        Assert (0 <= n and n <= (end - start));
                        return static_cast<size_t> (n);
                    });
#else
                AssertNotImplemented ();
#endif
            }
            virtual void SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr) override
            {
                sockaddr sa = sockAddr.As<sockaddr> ();
#if qPlatform_POSIX
                ThrowErrNoIfNegative (Handle_ErrNoResultInterruption ([this, &start, &end, &sa]() -> int { return ::sendto (fSD_, reinterpret_cast<const char*> (start), end - start, 0, reinterpret_cast<sockaddr*> (&sa), sizeof (sa)); }));
#elif qPlatform_Windows
                Require (end - start < numeric_limits<int>::max ());
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::sendto (fSD_, reinterpret_cast<const char*> (start), static_cast<int> (end - start), 0, reinterpret_cast<sockaddr*> (&sa), sizeof (sa)));
#else
                AssertNotImplemented ();
#endif
            }
            virtual size_t ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress) override
            {
                RequireNotNull (fromAddress);
                sockaddr  sa;
                socklen_t salen = sizeof (sa);
#if qPlatform_POSIX
                size_t result = static_cast<size_t> (ThrowErrNoIfNegative (Handle_ErrNoResultInterruption ([this, &intoStart, &intoEnd, &flag, &sa, &salen]() -> int { return ::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), intoEnd - intoStart, flag, &sa, &salen); })));
                *fromAddress  = sa;
                return result;
#elif qPlatform_Windows
                Require (intoEnd - intoStart < numeric_limits<int>::max ());
                size_t result = static_cast<size_t> (ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), static_cast<int> (intoEnd - intoStart), flag, &sa, &salen)));
                *fromAddress  = sa;
                return result;
#else
                AssertNotImplemented ();
#endif
            }
            virtual void Listen (unsigned int backlog) override
            {
#if qPlatform_POSIX
                ThrowErrNoIfNegative (Handle_ErrNoResultInterruption ([this, &backlog]() -> int { return ::listen (fSD_, backlog); }));
#elif qPlatform_Windows
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::listen (fSD_, backlog));
#else
                AssertNotImplemented ();
#endif
            }
            virtual Socket Accept () override
            {
                sockaddr  peer{};
                socklen_t sz = sizeof (peer);
#if qPlatform_POSIX
                return Socket::Attach (ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (Handle_ErrNoResultInterruption ([&]() -> int { return ::accept (fSD_, &peer, &sz); })));
#elif qPlatform_Windows
                return Socket::Attach (ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::accept (fSD_, &peer, &sz)));
#else
                AssertNotImplemented ();
#endif
            }
            virtual Optional<IO::Network::SocketAddress> GetLocalAddress () const override
            {
                struct sockaddr_storage radr;
                socklen_t               len = sizeof (radr);
                if (::getsockname (static_cast<int> (fSD_), (struct sockaddr*)&radr, &len) == 0) {
                    IO::Network::SocketAddress sa{radr};
                    return sa;
                }
                return Optional<IO::Network::SocketAddress>{};
            }
            virtual Optional<IO::Network::SocketAddress> GetPeerAddress () const override
            {
                struct sockaddr_storage radr;
                socklen_t               len = sizeof (radr);
                if (::getpeername (static_cast<int> (fSD_), (struct sockaddr*)&radr, &len) == 0) {
                    IO::Network::SocketAddress sa{radr};
                    return sa;
                }
                return Optional<IO::Network::SocketAddress>{};
            }
            virtual void JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) override
            {
                DbgTrace (L"Joining multicast group for address %s on interface %s", Characters::ToString (iaddr).c_str (), Characters::ToString (onInterface).c_str ());
                ip_mreq m{};
                Assert (iaddr.GetAddressFamily () == InternetAddress::AddressFamily::V4); // simple change to support IPV6 but NYI
                m.imr_multiaddr = iaddr.As<in_addr> ();
                m.imr_interface = onInterface.As<in_addr> ();
                setsockopt (IPPROTO_IP, IP_ADD_MEMBERSHIP, m);
            }
            virtual void LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) override
            {
                DbgTrace (L"Leaving multicast group for address %s on interface %s", Characters::ToString (iaddr).c_str (), Characters::ToString (onInterface).c_str ());
                ip_mreq m{};
                Assert (iaddr.GetAddressFamily () == InternetAddress::AddressFamily::V4); // simple change to support IPV6 but NYI
                m.imr_multiaddr = iaddr.As<in_addr> ();
                m.imr_interface = onInterface.As<in_addr> ();
                setsockopt (IPPROTO_IP, IP_DROP_MEMBERSHIP, m);
            }
            virtual uint8_t GetMulticastTTL () const override
            {
                return getsockopt<uint8_t> (IPPROTO_IP, IP_MULTICAST_TTL);
            }
            virtual void SetMulticastTTL (uint8_t ttl) override
            {
                setsockopt<uint8_t> (IPPROTO_IP, IP_MULTICAST_TTL, ttl);
            }
            virtual bool GetMulticastLoopMode () const override
            {
                return !!getsockopt<char> (IPPROTO_IP, IP_MULTICAST_LOOP);
            }
            virtual void SetMulticastLoopMode (bool loopMode) override
            {
                setsockopt<char> (IPPROTO_IP, IP_MULTICAST_LOOP, loopMode);
            }
            virtual KeepAliveOptions GetKeepAlives () const override
            {
                KeepAliveOptions result;
                result.fEnabled = !!getsockopt<int> (SOL_SOCKET, SO_KEEPALIVE);
#if qPlatform_Linux
                // Only available if linux >= 2.4
                result.fMaxProbesSentBeforeDrop              = !!getsockopt<int> (IPPROTO_IP, TCP_KEEPCNT);
                result.fTimeIdleBeforeSendingKeepalives      = !!getsockopt<int> (IPPROTO_IP, TCP_KEEPIDLE);
                result.fTimeBetweenIndividualKeepaliveProbes = !!getsockopt<int> (IPPROTO_IP, TCP_KEEPINTVL);
#endif
                return result;
            }
            virtual void SetKeepAlives (const KeepAliveOptions& keepAliveOptions) override
            {
                setsockopt<int> (SOL_SOCKET, SO_KEEPALIVE, keepAliveOptions.fEnabled);
#if qPlatform_Linux
                // Only available if linux >= 2.4
                if (keepAliveOptions.fMaxProbesSentBeforeDrop) {
                    setsockopt<int> (IPPROTO_IP, TCP_KEEPCNT, *keepAliveOptions.fMaxProbesSentBeforeDrop);
                }
                if (keepAliveOptions.fTimeIdleBeforeSendingKeepalives) {
                    setsockopt<int> (IPPROTO_IP, TCP_KEEPIDLE, *keepAliveOptions.fTimeIdleBeforeSendingKeepalives);
                }
                if (keepAliveOptions.fTimeBetweenIndividualKeepaliveProbes) {
                    setsockopt<int> (IPPROTO_IP, TCP_KEEPINTVL, *keepAliveOptions.fTimeBetweenIndividualKeepaliveProbes);
                }
#endif
            }
            virtual Optional<int> GetLinger () override
            {
                linger lr = getsockopt<linger> (SOL_SOCKET, SO_LINGER);
                return lr.l_onoff ? lr.l_linger : Optional<int>{};
            }
            inline void SetLinger (Optional<int> linger) override
            {
                ::linger so_linger{};
                if (linger) {
                    so_linger.l_onoff  = true;
                    so_linger.l_linger = *linger;
                }
                setsockopt<::linger> (SOL_SOCKET, SO_LINGER, so_linger);
            }
            virtual Socket::PlatformNativeHandle GetNativeSocket () const override
            {
                return fSD_;
            }
            virtual void getsockopt (int level, int optname, void* optval, socklen_t* optvallen) const override
            {
                // According to http://linux.die.net/man/2/getsockopt cannot return EINTR, so no need to retry
                RequireNotNull (optval);
#if qPlatform_POSIX
                ThrowErrNoIfNegative (::getsockopt (fSD_, level, optname, reinterpret_cast<char*> (optval), optvallen));
#elif qPlatform_Windows
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::getsockopt (fSD_, level, optname, reinterpret_cast<char*> (optval), optvallen));
#else
                AssertNotImplemented ();
#endif
            }
            template <typename RESULT_TYPE>
            inline RESULT_TYPE getsockopt (int level, int optname) const
            {
                RESULT_TYPE r{};
                socklen_t   roptlen = sizeof (r);
                this->getsockopt (level, optname, &r, &roptlen);
                return r;
            }
            virtual void setsockopt (int level, int optname, void* optval, socklen_t optvallen) const override
            {
                // According to http://linux.die.net/man/2/setsockopt cannot return EINTR, so no need to retry
                RequireNotNull (optval);
#if qPlatform_POSIX
                ThrowErrNoIfNegative (::setsockopt (fSD_, level, optname, reinterpret_cast<char*> (optval), optvallen));
#elif qPlatform_Windows
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::setsockopt (fSD_, level, optname, reinterpret_cast<char*> (optval), optvallen));
#else
                AssertNotImplemented ();
#endif
            }
            template <typename ARG_TYPE>
            inline void setsockopt (int level, int optname, ARG_TYPE arg) const
            {
                socklen_t optvallen = sizeof (arg);
                this->setsockopt (level, optname, &arg, optvallen);
            }
        };
    };
}

/*
********************************************************************************
*********************** Network::Socket::KeepAliveOptions **********************
********************************************************************************
*/
Characters::String Network::Socket::KeepAliveOptions::ToString () const
{
    Characters::StringBuilder sb;
    sb += L"{";
    sb += L"Enabled: " + Characters::ToString (fEnabled) + L",";
#if qPlatform_Linux
    if (fMaxProbesSentBeforeDrop) {
        sb += L"MaxProbesSentBeforeDrop: " + Characters::ToString (fMaxProbesSentBeforeDrop) + L",";
    }
    if (fTimeIdleBeforeSendingKeepalives) {
        sb += L"TimeIdleBeforeSendingKeepalives: " + Characters::ToString (fTimeIdleBeforeSendingKeepalives) + L",";
    }
    if (fTimeBetweenIndividualKeepaliveProbes) {
        sb += L"TimeBetweenIndividualKeepaliveProbes: " + Characters::ToString (fTimeBetweenIndividualKeepaliveProbes) + L",";
    }
#endif
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ********************************** Network::Socket *****************************
 ********************************************************************************
 */
Socket::Socket (SocketKind socketKind)
    : fRep_ ()
{
#if qPlatform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#endif
    Socket::PlatformNativeHandle sfd;
#if qPlatform_POSIX
    ThrowErrNoIfNegative (sfd = Handle_ErrNoResultInterruption ([&socketKind]() -> int { return socket (AF_INET, static_cast<int> (socketKind), 0); }));
#elif qPlatform_Windows
    ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (sfd = ::socket (AF_INET, static_cast<int> (socketKind), 0));
#else
    AssertNotImplemented ();
#endif
    fRep_ = make_shared<REALSOCKET_::Rep_> (sfd);
}

Socket::Socket (const shared_ptr<_Rep>& rep)
    : fRep_ (rep)
{
#if qPlatform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#endif
}

Socket::Socket (shared_ptr<_Rep>&& rep)
    : fRep_ (std::move (rep))
{
#if qPlatform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#endif
}

Socket Socket::Attach (PlatformNativeHandle sd)
{
    return Socket (make_shared<REALSOCKET_::Rep_> (sd));
}

Socket::PlatformNativeHandle Socket::Detach ()
{
    PlatformNativeHandle h = kINVALID_NATIVE_HANDLE_;
    if (fRep_.get () != nullptr) {
        h = fRep_->GetNativeSocket ();
    }
    fRep_.reset ();
    return h;
}

void Socket::Bind (const SocketAddress& sockAddr, BindFlags bindFlags)
{
    Require (fRep_ != nullptr); // Construct with Socket::Kind::SOCKET_STREAM?

    // Indicates that the rules used in validating addresses supplied in a bind(2) call should allow
    // reuse of local addresses. For AF_INET sockets this means that a socket may bind, except when
    // there is an active listening socket bound to the address. When the listening socket is bound
    // to INADDR_ANY with a specific port then it is not possible to bind to this port for any local address.
    setsockopt<int> (SOL_SOCKET, SO_REUSEADDR, bindFlags.fReUseAddr ? 1 : 0);

    sockaddr             useSockAddr = sockAddr.As<sockaddr> ();
    PlatformNativeHandle sfd         = fRep_->GetNativeSocket ();
#if qPlatform_Windows
    ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::bind (sfd, (sockaddr*)&useSockAddr, sizeof (useSockAddr)));
#else
    // EACCESS reproted as FileAccessException - which is crazy confusing.
    // @todo - find a better way, but for now remap this...
    try {
        ThrowErrNoIfNegative (Handle_ErrNoResultInterruption ([&sfd, &useSockAddr]() -> int { return ::bind (sfd, (sockaddr*)&useSockAddr, sizeof (useSockAddr)); }));
    }
    catch (const IO::FileAccessException&) {
        Throw (StringException (L"Cannot Bind to port"));
    }
#endif
}

void Socket::Close ()
{
    // not important to null-out, but may as well...
    if (fRep_.get () != nullptr) {
        fRep_->Close ();
        fRep_.reset ();
    }
}

bool Socket::IsOpen () const
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
#if qPlatform_Windows
namespace Stroika {
    namespace Foundation {
        namespace Execution {
            // this specialization needed because the winsock type for SOCKET is UNSIGNED so < 0 test doesn't work
            template <>
            IO::Network::Socket::PlatformNativeHandle ThrowErrNoIfNegative (IO::Network::Socket::PlatformNativeHandle returnCode)
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
