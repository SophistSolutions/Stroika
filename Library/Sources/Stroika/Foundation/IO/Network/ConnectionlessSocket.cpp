/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#include "Stroika/Foundation/Execution/WaitForIOReady.h"

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

using Debug::AssertExternallySynchronizedMutex;

namespace {
    struct Rep_ : BackSocketImpl_<ConnectionlessSocket::_IRep> {
        using inherited = BackSocketImpl_<ConnectionlessSocket::_IRep>;
        Rep_ (Socket::PlatformNativeHandle sd)
            : inherited{sd}
        {
        }
        virtual void SendTo (const byte* start, const byte* end, const SocketAddress& sockAddr) override
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"IO::Network::Socket...rep...::SendTo", "end-start={}, sockAddr={}"_f,
                                          static_cast<long long> (end - start), sockAddr};
#endif
            AssertExternallySynchronizedMutex::WriteContext declareContext{this->fThisAssertExternallySynchronized};
            sockaddr_storage                                sa = sockAddr.As<sockaddr_storage> ();
#if qPlatform_POSIX
            Handle_ErrNoResultInterruption ([this, &start, &end, &sa, &sockAddr] () -> int {
                return ::sendto (fSD_, reinterpret_cast<const char*> (start), end - start, 0, reinterpret_cast<sockaddr*> (&sa),
                                 sockAddr.GetRequiredSize ());
            });
#elif qPlatform_Windows
            Require (end - start < numeric_limits<int>::max ());
            ThrowWSASystemErrorIfSOCKET_ERROR (::sendto (fSD_, reinterpret_cast<const char*> (start), static_cast<int> (end - start), 0,
                                                         reinterpret_cast<sockaddr*> (&sa), static_cast<int> (sockAddr.GetRequiredSize ())));
#else
            AssertNotImplemented ();
#endif
        }
        virtual size_t ReceiveFrom (byte* intoStart, byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSeconds timeout) override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized};

            if constexpr (qPlatform_Windows) {
                // TMPHACK for - https://stroika.atlassian.net/browse/STK-964
                auto s = Execution::WaitForIOReady{fSD_}.WaitQuietly (timeout);
                Execution::Thread::CheckForInterruption ();
                if (s.empty ()) {
                    Execution::Throw (Execution::TimeOutException::kThe);
                }
            }

            // Note - COULD have implemented timeout with SO_RCVTIMEO, but that would risk statefulness, and confusion setting/resetting the parameter. Could be done, but this seems
            // cleaner...
            constexpr Time::DurationSeconds kMaxPolltime_{numeric_limits<int>::max () / 1000.0};
            if (timeout < kMaxPolltime_) {
                int    timeout_millisecs = Math::Round<int> (timeout.count () * 1000);
                pollfd pollData{};
                pollData.fd     = fSD_;
                pollData.events = POLLIN;
#if qPlatform_Windows
                int nresults;
                if ((nresults = ::WSAPoll (&pollData, 1, timeout_millisecs)) == SOCKET_ERROR) {
                    Execution::ThrowSystemErrNo (::WSAGetLastError ());
                }
#else
                int nresults = Handle_ErrNoResultInterruption ([&] () { return ::poll (&pollData, 1, timeout_millisecs); });
#endif
                if (nresults == 0) [[unlikely]] {
                    Execution::Throw (Execution::TimeOutException::kThe);
                }
            }

            struct sockaddr_storage sa;
            socklen_t               salen = sizeof (sa);
#if qPlatform_POSIX
            size_t result = static_cast<size_t> (Handle_ErrNoResultInterruption ([&] () -> int {
                return ::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), intoEnd - intoStart, flag,
                                   fromAddress == nullptr ? nullptr : reinterpret_cast<sockaddr*> (&sa), fromAddress == nullptr ? nullptr : &salen);
            }));
            if (fromAddress != nullptr) {
                *fromAddress = sa;
            }
            return result;
#elif qPlatform_Windows
            Require (intoEnd - intoStart < numeric_limits<int>::max ());
            size_t result = static_cast<size_t> (ThrowWSASystemErrorIfSOCKET_ERROR (
                ::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), static_cast<int> (intoEnd - intoStart), flag,
                            fromAddress == nullptr ? nullptr : reinterpret_cast<sockaddr*> (&sa), fromAddress == nullptr ? nullptr : &salen)));
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
            Debug::TraceContextBumper                       ctx{"IO::Network::Socket::JoinMulticastGroup",
                                          Stroika_Foundation_Debug_OptionalizeTraceArgs ("iaddr={} onInterface={}"_f, iaddr, onInterface)};
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized};
            Assert (iaddr.GetAddressFamily () == InternetAddress::AddressFamily::V4 or iaddr.GetAddressFamily () == InternetAddress::AddressFamily::V6);
            auto                       activity = Execution::LazyEvalActivity{[&] () -> Characters::String {
                return "joining multicast group "sv + Characters::ToString (iaddr) + " on interface "sv + Characters::ToString (onInterface);
            }};
            Execution::DeclareActivity activityDeclare{&activity};
            switch (iaddr.GetAddressFamily ()) {
                case InternetAddress::AddressFamily::V4: {
                    ::ip_mreq m{};
                    m.imr_multiaddr = iaddr.As<in_addr> ();
                    m.imr_interface = onInterface.As<in_addr> ();
                    setsockopt (IPPROTO_IP, IP_ADD_MEMBERSHIP, m);
                } break;
                case InternetAddress::AddressFamily::V6: {
                    ::ipv6_mreq m{};
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
            Debug::TraceContextBumper ctx{"IO::Network::Socket::LeaveMulticastGroup", "iaddr={} onInterface={}"_f, iaddr, onInterface};
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized};
            switch (iaddr.GetAddressFamily ()) {
                case InternetAddress::AddressFamily::V4: {
                    ::ip_mreq m{};
                    m.imr_multiaddr = iaddr.As<in_addr> ();
                    m.imr_interface = onInterface.As<in_addr> ();
                    setsockopt (IPPROTO_IP, IP_DROP_MEMBERSHIP, m);
                } break;
                case InternetAddress::AddressFamily::V6: {
                    ::ipv6_mreq m{};
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
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
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
            static constexpr Execution::Activity            kSettingMulticastTTL{"setting multicast TTL"sv};
            Execution::DeclareActivity                      activityDeclare{&kSettingMulticastTTL};
            AssertExternallySynchronizedMutex::WriteContext declareContext{this->fThisAssertExternallySynchronized};
            switch (GetAddressFamily ()) {
                case SocketAddress::INET: {
                    setsockopt<uint8_t> (IPPROTO_IP, IP_MULTICAST_TTL, ttl);
                    break;
                }
                case SocketAddress::INET6: {
                    constexpr bool kIPV6LoophackMulticastTTLLinuxBug_{qPlatform_Linux}; // https://stroika.atlassian.net/browse/STK-578
                    if (kIPV6LoophackMulticastTTLLinuxBug_) {
                        try {
                            setsockopt<char> (IPPROTO_IPV6, IPV6_MULTICAST_HOPS, ttl);
                        }
                        catch (const std::system_error& e) {
                            // I've dug into this, and have no idea why its failing - with EINVAL
                            if (e.code () == errc::invalid_argument) {
                                DbgTrace ("IPV6_MULTICAST_HOPS: For now ignoring what is probably a very small, minor bug, but one "
                                          "where I have no idea why this is happening - but I saw reliably on Ubuntu/Linux"_f);
                            }
                            // @todo - fix this code - almost certainly wrong...
                        }
                    }
                    else {
                        setsockopt<char> (IPPROTO_IPV6, IPV6_MULTICAST_HOPS, ttl);
                    }
                    break;
                }
                default:
                    RequireNotReached (); // only legal for IP sockets
            }
        }
        virtual bool GetMulticastLoopMode () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
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
            static constexpr Execution::Activity            kSettingMulticastLoopMode{"setting multicast loop mode"sv};
            Execution::DeclareActivity                      activityDeclare{&kSettingMulticastLoopMode};
            AssertExternallySynchronizedMutex::WriteContext declareContext{this->fThisAssertExternallySynchronized};
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
                        catch (const std::system_error& e) {
                            // I've dug into this, and have no idea why its failing - with EINVAL
                            if (e.code () == errc::invalid_argument) {
                                DbgTrace ("IPV6_MULTICAST_LOOP: For now ignoring what is probably a very small, minor bug, but one "
                                          "where I have no idea why this is happening - but I saw reliably on Ubuntu/Linux"_f);
                            }
                            // @todo - fix this code - almost certainly wrong...
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
}

/*
 ********************************************************************************
 ************************** ConnectionlessSocket ********************************
 ********************************************************************************
 */
ConnectionlessSocket::Ptr ConnectionlessSocket::New (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol)
{
    Require (socketKind != Type::STREAM); // use ConnectionOrientedStreamSocket or ConnectionOrientedMasterSocket
    return Ptr{make_shared<Rep_> (_Protected::mkLowLevelSocket_ (family, socketKind, protocol))};
}

ConnectionlessSocket::Ptr ConnectionlessSocket::Attach (PlatformNativeHandle sd)
{
    return Ptr{make_shared<Rep_> (sd)};
}
