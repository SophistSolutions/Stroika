/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Platform_POSIX
#include <net/if.h>
#elif qStroika_Platform_Windows
#include <WinSock2.h>

#include <Iphlpapi.h>
#include <netioapi.h>
#endif

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/WaitForIOReady.h"
#include "Stroika/Foundation/IO/Network/Interface.h"
#include "Stroika/Foundation/IO/Network/Socket-Private_.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

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

using Debug::AssertExternallySynchronizedChecker;

#if defined(_MSC_VER)
#pragma comment(lib, "Iphlpapi.lib") // ConvertInterfaceGuidToLuid ()
#endif

namespace {
    // the index of the interface with this address (an exact match - not merely on its subnet); kAddrAny: kAnyIndex
    unsigned int InterfaceIndexOf_ (const InternetAddress& interfaceAddress)
    {
        if (interfaceAddress == V4::kAddrAny or interfaceAddress == V6::kAddrAny) {
            return Interface::kAnyIndex;
        }
        for (const Interface& i : SystemInterfacesMgr{}.GetAll ()) {
            if (i.fIndex and i.fBindings.fAddresses.Contains (interfaceAddress)) {
                return *i.fIndex;
            }
        }
        Execution::Throw (SystemErrorException{make_error_code (errc::no_such_device)}); // what IP_ADD_MEMBERSHIP reported for this (on Linux)
    }
    // i's index now - looked up by i.fInterfaceID, not i.fIndex, which may be stale (@see Interface::fIndex) - or ENODEV if it is
    // gone (as IP_ADD_MEMBERSHIP reports for a missing interface, on Linux)
    unsigned int CurrentIndexOf_ (const Interface& i)
    {
#if qStroika_Platform_POSIX
        // fInterfaceID is the device's name
        if (unsigned int index = ::if_nametoindex (i.fInterfaceID.AsNarrowSDKString ().c_str ()); index != Interface::kAnyIndex) {
            return index;
        }
#elif qStroika_Platform_Windows
        // fInterfaceID is the adapter's GUID (IP_ADAPTER_ADDRESSES::AdapterName) - so one that is not a GUID is no adapter's
        optional<::GUID> guid;
        try {
            Common::GUID g{i.fInterfaceID};
            static_assert (sizeof (::GUID) == sizeof (g));
            guid.emplace ();
            ::memcpy (&*guid, &g, sizeof (::GUID));
        }
        catch (const DataExchange::BadFormatException&) {
        }
        NET_LUID    luid{};
        NET_IFINDEX index{};
        if (guid and ::ConvertInterfaceGuidToLuid (&*guid, &luid) == NO_ERROR and
            ::ConvertInterfaceLuidToIndex (&luid, &index) == NO_ERROR and index != Interface::kAnyIndex) {
            return index;
        }
#else
        AssertNotImplemented ();
#endif
        Execution::Throw (SystemErrorException{make_error_code (errc::no_such_device)});
    }
    // the interface as IPv6 names it: by index (an address is looked up)
    unsigned int InterfaceIndex_ (const variant<InternetAddress, unsigned int>& onInterface)
    {
        if (const unsigned int* index = get_if<unsigned int> (&onInterface)) {
            return *index;
        }
        return InterfaceIndexOf_ (get<InternetAddress> (onInterface));
    }
}

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
            AssertExternallySynchronizedChecker::WriteContext declareContext{this->fThisAssertExternallySynchronized};
            sockaddr_storage                                  sa = sockAddr.As<sockaddr_storage> ();
#if qStroika_Platform_POSIX
            Handle_ErrNoResultInterruption ([this, &start, &end, &sa, &sockAddr] () -> int {
                return ::sendto (fSD_, reinterpret_cast<const char*> (start), end - start, 0, reinterpret_cast<sockaddr*> (&sa),
                                 sockAddr.GetRequiredSize ());
            });
#elif qStroika_Platform_Windows
            Require (end - start < numeric_limits<int>::max ());
            ThrowWSASystemErrorIfSOCKET_ERROR (::sendto (fSD_, reinterpret_cast<const char*> (start), static_cast<int> (end - start), 0,
                                                         reinterpret_cast<sockaddr*> (&sa), static_cast<int> (sockAddr.GetRequiredSize ())));
#else
            AssertNotImplemented ();
#endif
        }
        virtual size_t ReceiveFrom (byte* intoStart, byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSeconds timeout) override
        {
            AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized};

            if constexpr (qStroika_Platform_Windows) {
                // TMPHACK for - https://github.com/SophistSolutions/Stroika/issues/1096 (STK-964)
                auto s = Execution::WaitForIOReady{fSD_}.WaitQuietly (timeout);
                Execution::Thread::CheckForInterruption ();
                if (s.empty ()) {
                    Execution::ThrowError (errc::timed_out);
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
#if qStroika_Platform_Windows
                int nresults;
                if ((nresults = ::WSAPoll (&pollData, 1, timeout_millisecs)) == SOCKET_ERROR) {
                    Execution::ThrowSystemErrNo (::WSAGetLastError ());
                }
#else
                int nresults = Handle_ErrNoResultInterruption ([&] () { return ::poll (&pollData, 1, timeout_millisecs); });
#endif
                if (nresults == 0) [[unlikely]] {
                    Execution::ThrowError (errc::timed_out);
                }
            }

            struct sockaddr_storage sa;
            socklen_t               salen = sizeof (sa);
#if qStroika_Platform_POSIX
            size_t result = static_cast<size_t> (Handle_ErrNoResultInterruption ([&] () -> int {
                return ::recvfrom (fSD_, reinterpret_cast<char*> (intoStart), intoEnd - intoStart, flag,
                                   fromAddress == nullptr ? nullptr : reinterpret_cast<sockaddr*> (&sa), fromAddress == nullptr ? nullptr : &salen);
            }));
            if (fromAddress != nullptr) {
                *fromAddress = sa;
            }
            return result;
#elif qStroika_Platform_Windows
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
        virtual void JoinMulticastGroup (const InternetAddress& iaddr, const variant<InternetAddress, unsigned int>& onInterface) override
        {
            Debug::TraceContextBumper ctx{
                "IO::Network::Socket::JoinMulticastGroup",
                Stroika_Foundation_Debug_OptionalizeTraceArgs ("iaddr={} onInterface={}"_f, iaddr, Characters::ToString (onInterface))};
            AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized};
            auto                                              activity = Execution::LazyEvalActivity{[&] () -> Characters::String {
                return "joining multicast group "sv + Characters::ToString (iaddr) + " on interface "sv + Characters::ToString (onInterface);
            }};
            Execution::DeclareActivity                        activityDeclare{&activity};
            SetMembership_ (iaddr, onInterface, true);
        }
        virtual void LeaveMulticastGroup (const InternetAddress& iaddr, const variant<InternetAddress, unsigned int>& onInterface) override
        {
            Debug::TraceContextBumper ctx{"IO::Network::Socket::LeaveMulticastGroup", "iaddr={} onInterface={}"_f, iaddr,
                                          Characters::ToString (onInterface)};
            AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized};
            SetMembership_ (iaddr, onInterface, false);
        }
        /*
         *  Each case names the interface as the OS's own call does, so the kernel resolves it: IPv4's classic join takes an
         *  interface ADDRESS (INADDR_ANY: the OS picks one), IPv6's an index (kAnyIndex: the OS picks one). An IPv4 interface
         *  given by index uses RFC 3678's MCAST_JOIN_GROUP - but BSD/macOS refuse kAnyIndex there (EADDRNOTAVAIL), so that is
         *  INADDR_ANY.
         *  IPv6 has no join by address, so an address is looked up.
         */
        void SetMembership_ (const InternetAddress& group, const variant<InternetAddress, unsigned int>& onInterface, bool join)
        {
            switch (group.GetAddressFamily ()) {
                case InternetAddress::AddressFamily::V4: {
                    if (const unsigned int* index = get_if<unsigned int> (&onInterface); index != nullptr and *index != Interface::kAnyIndex) {
                        // RFC 3678's protocol-independent form: the group as a sockaddr, the interface by index.
                        // The kernel reads all of r, so copy only the GetRequiredSize () bytes of sockaddr_storage a SocketAddress
                        // sets - not all of it (else valgrind: setsockopt "points to uninitialised byte(s)").
                        ::group_req r{};
                        r.gr_interface = *index;
                        const SocketAddress    sa{group};
                        const sockaddr_storage ss = sa.As<sockaddr_storage> ();
                        ::memcpy (&r.gr_group, &ss, sa.GetRequiredSize ());
#if qStroika_Platform_MacOS
                        // a BSD sockaddr carries its own length - which bind () and sendto () take from their argument instead,
                        // but which MCAST_JOIN_GROUP checks (EINVAL)
                        r.gr_group.ss_len = static_cast<uint8_t> (sa.GetRequiredSize ());
#endif
                        setsockopt (IPPROTO_IP, join ? MCAST_JOIN_GROUP : MCAST_LEAVE_GROUP, &r, static_cast<socklen_t> (sizeof (r)));
                    }
                    else {
                        ::ip_mreq m{};
                        m.imr_multiaddr = group.As<in_addr> ();
                        if (const InternetAddress* address = get_if<InternetAddress> (&onInterface); address != nullptr and *address != V6::kAddrAny) {
                            m.imr_interface = address->As<in_addr> (); // else INADDR_ANY: the OS picks
                        }
                        setsockopt (IPPROTO_IP, join ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP, m);
                    }
                } break;
                case InternetAddress::AddressFamily::V6: {
                    ::ipv6_mreq m{};
                    m.ipv6mr_multiaddr = group.As<in6_addr> ();
                    m.ipv6mr_interface = InterfaceIndex_ (onInterface);
                    setsockopt (IPPROTO_IPV6, join ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP, m);
                } break;
                default:
                    RequireNotReached ();
            }
        }
        virtual void SetMulticastInterface (const variant<InternetAddress, unsigned int>& onInterface) override
        {
            AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized};
            switch (GetAddressFamily ()) {
                case SocketAddress::INET: {
                    // by one of its addresses - there is no portable way to choose an IPv4 interface by index
                    const InternetAddress* address = get_if<InternetAddress> (&onInterface);
                    Require (address != nullptr and address->GetAddressFamily () == InternetAddress::AddressFamily::V4);
                    setsockopt (IPPROTO_IP, IP_MULTICAST_IF, address->As<in_addr> ());
                } break;
                case SocketAddress::INET6: {
                    setsockopt<unsigned int> (IPPROTO_IPV6, IPV6_MULTICAST_IF, InterfaceIndex_ (onInterface)); // an unsigned int (a DWORD on Windows)
                } break;
                default:
                    RequireNotReached ();
            }
        }
        virtual uint8_t GetMulticastTTL () const override
        {
            AssertExternallySynchronizedChecker::ReadContext declareContext{this->fThisAssertExternallySynchronized};
            switch (GetAddressFamily ()) {
                case SocketAddress::INET: {
                    return getsockopt<uint8_t> (IPPROTO_IP, IP_MULTICAST_TTL);
                }
                case SocketAddress::INET6: {
                    // IPv6's multicast options are an int (hops) and an unsigned int (loop) - RFC 3493 5.2 - and Linux and macOS
                    // reject anything shorter, with EINVAL. (IPv4's traditionally take a byte, which all accept.)
                    return static_cast<uint8_t> (getsockopt<int> (IPPROTO_IPV6, IPV6_MULTICAST_HOPS));
                }
                default:
                    RequireNotReached (); // only legal for IP sockets
                    return 0;
            }
        }
        virtual void SetMulticastTTL (uint8_t ttl) override
        {
            static constexpr Execution::Activity              kSettingMulticastTTL{"setting multicast TTL"sv};
            Execution::DeclareActivity                        activityDeclare{&kSettingMulticastTTL};
            AssertExternallySynchronizedChecker::WriteContext declareContext{this->fThisAssertExternallySynchronized};
            switch (GetAddressFamily ()) {
                case SocketAddress::INET: {
                    setsockopt<uint8_t> (IPPROTO_IP, IP_MULTICAST_TTL, ttl);
                    break;
                }
                case SocketAddress::INET6: {
                    setsockopt<int> (IPPROTO_IPV6, IPV6_MULTICAST_HOPS, ttl); // an int: @see GetMulticastTTL
                    break;
                }
                default:
                    RequireNotReached (); // only legal for IP sockets
            }
        }
        virtual bool GetMulticastLoopMode () const override
        {
            AssertExternallySynchronizedChecker::ReadContext declareContext{this->fThisAssertExternallySynchronized};
            switch (GetAddressFamily ()) {
                case SocketAddress::INET: {
                    return !!getsockopt<char> (IPPROTO_IP, IP_MULTICAST_LOOP);
                }
                case SocketAddress::INET6: {
                    return !!getsockopt<unsigned int> (IPPROTO_IPV6, IPV6_MULTICAST_LOOP); // an unsigned int: @see GetMulticastTTL
                }
                default:
                    RequireNotReached (); // only legal for IP sockets
                    return false;
            }
        }
        virtual void SetMulticastLoopMode (bool loopMode) override
        {
            static constexpr Execution::Activity              kSettingMulticastLoopMode{"setting multicast loop mode"sv};
            Execution::DeclareActivity                        activityDeclare{&kSettingMulticastLoopMode};
            AssertExternallySynchronizedChecker::WriteContext declareContext{this->fThisAssertExternallySynchronized};
            switch (GetAddressFamily ()) {
                case SocketAddress::INET: {
                    setsockopt<char> (IPPROTO_IP, IP_MULTICAST_LOOP, loopMode);
                    break;
                }
                case SocketAddress::INET6: {
                    setsockopt<unsigned int> (IPPROTO_IPV6, IPV6_MULTICAST_LOOP, loopMode); // an unsigned int: @see GetMulticastTTL
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
 ************************** ConnectionlessSocket::Ptr ***************************
 ********************************************************************************
 */
void ConnectionlessSocket::Ptr::JoinMulticastGroup (const InternetAddress& iaddr, const Interface& onInterface) const
{
    JoinMulticastGroup (iaddr, CurrentIndexOf_ (onInterface));
}

void ConnectionlessSocket::Ptr::LeaveMulticastGroup (const InternetAddress& iaddr, const Interface& onInterface) const
{
    LeaveMulticastGroup (iaddr, CurrentIndexOf_ (onInterface));
}

void ConnectionlessSocket::Ptr::SetMulticastInterface (const Interface& i) const
{
    if (GetAddressFamily () == SocketAddress::INET) {
        // by one of its addresses - there is no portable way to choose an IPv4 interface by index
        optional<InternetAddress> a = i.fBindings.fAddresses.First (
            [] (const InternetAddress& ia) { return ia.GetAddressFamily () == InternetAddress::AddressFamily::V4; });
        if (not a) {
            Execution::Throw (SystemErrorException{make_error_code (errc::address_not_available)}); // no IPv4 on that interface
        }
        SetMulticastInterface (*a);
    }
    else {
        SetMulticastInterface (CurrentIndexOf_ (i));
    }
}

/*
 ********************************************************************************
 ************************** ConnectionlessSocket ********************************
 ********************************************************************************
 */
ConnectionlessSocket::Ptr ConnectionlessSocket::New (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol)
{
    Require (socketKind != Type::STREAM); // use ConnectionOrientedStreamSocket or ConnectionOrientedMasterSocket
    return Ptr{Memory::MakeSharedPtr<Rep_> (_Protected::mkLowLevelSocket_ (family, socketKind, protocol))};
}

ConnectionlessSocket::Ptr ConnectionlessSocket::Attach (PlatformNativeHandle sd)
{
    return Ptr{Memory::MakeSharedPtr<Rep_> (sd)};
}
