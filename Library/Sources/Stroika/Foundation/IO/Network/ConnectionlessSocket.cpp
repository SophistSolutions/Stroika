/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/ToString.h"
#include "../../Execution/TimeOutException.h"

#include "Socket-Private_.h"

#include "ConnectionlessSocket.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1
using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Foundation::IO::Network::PRIVATE_;

namespace {
    struct ConnectionlessSocket_IMPL_ : ConnectionlessSocket {
        struct Rep_ : BackSocketImpl_<ConnectionlessSocket>::Rep_ {
            using inherited = BackSocketImpl_<ConnectionlessSocket>::Rep_;
            Rep_ (Socket::PlatformNativeHandle sd)
                : inherited (sd)
            {
            }
            virtual void SendTo (const byte* start, const byte* end, const SocketAddress& sockAddr) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"IO::Network::Socket...rep...::SendTo", L"end-start=%lld, sockAddr=%s", static_cast<long long> (end - start), Characters::ToString (sockAddr).c_str ())};
#endif
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                sockaddr_storage                                   sa = sockAddr.As<sockaddr_storage> ();
#if qPlatform_POSIX
                ThrowErrNoIfNegative (Handle_ErrNoResultInterruption ([this, &start, &end, &sa]() -> int { return ::sendto (fSD_, reinterpret_cast<const char*> (start), end - start, 0, reinterpret_cast<sockaddr*> (&sa), sizeof (sa)); }));
#elif qPlatform_Windows
                Require (end - start < numeric_limits<int>::max ());
                ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::sendto (fSD_, reinterpret_cast<const char*> (start), static_cast<int> (end - start), 0, reinterpret_cast<sockaddr*> (&sa), sizeof (sa)));
#else
                AssertNotImplemented ();
#endif
            }
            virtual size_t ReceiveFrom (byte* intoStart, byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSecondsType timeout) override
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                // Note - COULD have implemented timeout with SO_RCVTIMEO, but that would risk statefulness, and confusion setting/resetting the parameter. Could be done, but this seems
                // cleaner...
                constexpr Time::DurationSecondsType kMaxPolltime_{numeric_limits<int>::max () / 1000.0};
                if (timeout < kMaxPolltime_) {
                    int    timeout_msecs = Math::Round<int> (timeout * 1000);
                    pollfd pollData{};
                    pollData.fd     = fSD_;
                    pollData.events = POLLIN;
#if qPlatform_Windows
                    int nresults;
                    if ((nresults = ::WSAPoll (&pollData, 1, timeout_msecs)) == SOCKET_ERROR) {
                        Execution::Platform::Windows::Exception::Throw (::WSAGetLastError ());
                    }
#else
                    int nresults = Handle_ErrNoResultInterruption ([&]() { return ::poll (&pollData, 1, timeout_msecs); });
#endif
                    if (nresults == 0)
                        [[UNLIKELY_ATTR]]
                        {
                            Execution::Throw (Execution::TimeOutException ());
                        }
                }

                struct sockaddr_storage sa;
                socklen_t               salen = sizeof (sa);
#if qPlatform_POSIX
                size_t result = static_cast<size_t> (ThrowErrNoIfNegative (Handle_ErrNoResultInterruption ([&]() -> int { return ::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), intoEnd - intoStart, flag, fromAddress == nullptr ? nullptr : reinterpret_cast<sockaddr*> (&sa), fromAddress == nullptr ? nullptr : &salen); })));
                if (fromAddress != nullptr) {
                    *fromAddress = sa;
                }
                return result;
#elif qPlatform_Windows
                Require (intoEnd - intoStart < numeric_limits<int>::max ());
                size_t result = static_cast<size_t> (ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), static_cast<int> (intoEnd - intoStart), flag, fromAddress == nullptr ? nullptr : reinterpret_cast<sockaddr*> (&sa), fromAddress == nullptr ? nullptr : &salen)));
                if (fromAddress != nullptr) {
                    *fromAddress = sa;
                }
                return result;
#else
                AssertNotImplemented ();
#endif
            }
            virtual void JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) override
            {
                Debug::TraceContextBumper                          ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"IO::Network::Socket::JoinMulticastGroup", L"iaddr=%s onInterface=%s", Characters::ToString (iaddr).c_str (), Characters::ToString (onInterface).c_str ())};
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                Assert (iaddr.GetAddressFamily () == InternetAddress::AddressFamily::V4 or iaddr.GetAddressFamily () == InternetAddress::AddressFamily::V6);
                switch (iaddr.GetAddressFamily ()) {
                    case InternetAddress::AddressFamily::V4: {
                        ip_mreq m{};
                        m.imr_multiaddr = iaddr.As<in_addr> ();
                        m.imr_interface = onInterface.As<in_addr> ();
                        setsockopt (IPPROTO_IP, IP_ADD_MEMBERSHIP, m);
                    } break;
                    case InternetAddress::AddressFamily::V6: {
                        ipv6_mreq m{};
                        m.ipv6mr_multiaddr = iaddr.As<in6_addr> ();
                        m.ipv6mr_interface = 0; //??? seems to mean any
                        setsockopt (IPPROTO_IPV6, IPV6_JOIN_GROUP, m);
                    } break;
                    default:
                        RequireNotReached ();
                }
            }
            virtual void LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) override
            {
                Debug::TraceContextBumper                          ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"IO::Network::Socket::LeaveMulticastGroup", L"iaddr=%s onInterface=%s", Characters::ToString (iaddr).c_str (), Characters::ToString (onInterface).c_str ())};
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                switch (iaddr.GetAddressFamily ()) {
                    case InternetAddress::AddressFamily::V4: {
                        ip_mreq m{};
                        m.imr_multiaddr = iaddr.As<in_addr> ();
                        m.imr_interface = onInterface.As<in_addr> ();
                        setsockopt (IPPROTO_IP, IP_DROP_MEMBERSHIP, m);
                    } break;
                    case InternetAddress::AddressFamily::V6: {
                        ipv6_mreq m{};
                        m.ipv6mr_multiaddr = iaddr.As<in6_addr> ();
                        m.ipv6mr_interface = 0; ///??? seems to mean any
                        setsockopt (IPPROTO_IPV6, IPV6_LEAVE_GROUP, m);
                    } break;
                    default:
                        RequireNotReached ();
                }
            }
            virtual uint8_t GetMulticastTTL () const override
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                switch (GetAddressFamily ()) {
                    case SocketAddress::INET: {
                        return getsockopt<uint8_t> (IPPROTO_IP, IP_MULTICAST_TTL);
                    }
                    case SocketAddress::INET6: {
                        return getsockopt<uint8_t> (IPPROTO_IPV6, IPV6_MULTICAST_HOPS);
                    }
                    default:
                        RequireNotReached (); // only legal for IP sockets
                        return 0;
                }
            }
            virtual void SetMulticastTTL (uint8_t ttl) override
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                switch (GetAddressFamily ()) {
                    case SocketAddress::INET: {
                        setsockopt<uint8_t> (IPPROTO_IP, IP_MULTICAST_TTL, ttl);
                        break;
                    }
                    case SocketAddress::INET6: {
                        setsockopt<uint8_t> (IPPROTO_IPV6, IPV6_MULTICAST_HOPS, ttl);
                        break;
                    }
                    default:
                        RequireNotReached (); // only legal for IP sockets
                }
            }
            virtual bool GetMulticastLoopMode () const override
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                switch (GetAddressFamily ()) {
                    case SocketAddress::INET: {
                        return !!getsockopt<char> (IPPROTO_IP, IP_MULTICAST_LOOP);
                    }
                    case SocketAddress::INET6: {
                        return !!getsockopt<char> (IPPROTO_IPV6, IP_MULTICAST_LOOP);
                    }
                    default:
                        RequireNotReached (); // only legal for IP sockets
                        return false;
                }
            }
            virtual void SetMulticastLoopMode (bool loopMode) override
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                switch (GetAddressFamily ()) {
                    case SocketAddress::INET: {
                        setsockopt<char> (IPPROTO_IP, IP_MULTICAST_LOOP, loopMode);
                        break;
                    }
                    case SocketAddress::INET6: {
                        constexpr bool kIPV6LoophackMulticastModeLinuxBug_{qPlatform_Linux}; // https://stroika.atlassian.net/browse/STK-578
                        if (kIPV6LoophackMulticastModeLinuxBug_) {
                            try {
                                setsockopt<char> (IPPROTO_IPV6, IPV6_MULTICAST_LOOP, loopMode);
                            }
                            catch (const errno_ErrorException& e) {
                                // I've dug into this, and have no idea why its failing - with EINVAL
                                if (e == EINVAL) {
                                    DbgTrace (L"IPV6_MULTICAST_LOOP: For now ignoring what is probbaly a very small, minor bug, but one where I have no idea why this is happening - but I saw reliably on Ubuntu/Linux");
                                }
                            }
                        }
                        else {
                            setsockopt<char> (IPPROTO_IPV6, IPV6_MULTICAST_LOOP, loopMode);
                        }
                        break;
                    }
                    default:
                        RequireNotReached (); // only legal for IP sockets
                }
            }
        };
    };
}

/*
 ********************************************************************************
 ************************** ConnectionlessSocket ********************************
 ********************************************************************************
 */
ConnectionlessSocket::Ptr ConnectionlessSocket::New (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol)
{
    Require (socketKind != Type::STREAM); // use ConnectionOrientedStreamSocket or ConnectionOrientedMasterSocket
    return Ptr{make_shared<ConnectionlessSocket_IMPL_::Rep_> (mkLowLevelSocket_ (family, socketKind, protocol))};
}

ConnectionlessSocket::Ptr ConnectionlessSocket::Attach (PlatformNativeHandle sd)
{
    return Ptr{make_shared<ConnectionlessSocket_IMPL_::Rep_> (sd)};
}
