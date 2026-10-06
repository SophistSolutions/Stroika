/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstddef>
#include <cstdio>
#include <cstring>

#if qStroika_Platform_POSIX
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#if qStroika_Platform_Linux
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#elif qStroika_Platform_MacOS
#include <net/if_dl.h>
#include <net/route.h>
#endif
#elif qStroika_Platform_Windows
#include <WinSock2.h>

#include <WS2tcpip.h>

#include <Iphlpapi.h>
#include <netioapi.h>
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/WaitForIOReady.h"
#if qStroika_Platform_Windows
#include "Platform/Windows/WinSock.h"
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include "Stroika/Foundation/IO/Network/DNS.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

#include "ConnectionlessSocket.h"

#include "LinkMonitor.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

#if defined(_MSC_VER)
// support use of Iphlpapi - but better to reference here than in lib entry of project file cuz
// easiser to see/modularize (and only pulled in if this module is referenced)
#pragma comment(lib, "Iphlpapi.lib")
#endif

#if 0
// FOR POSIX DO SOMETHING LIKE THIS:
#include <arpa/inet.h>
#include <assert.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>


static const char* flags (int sd, const char* name)
{
    static char buf[1024];

    static struct ifreq ifreq;
    strcpy (ifreq.ifr_name, name);

    int r = ioctl (sd, SIOCGIFFLAGS, (char*)&ifreq);
    assert (r == 0);

    int l = 0;
#define FLAG(b)                                                                                                                            \
    if (ifreq.ifr_flags & b)                                                                                                               \
    l += snprintf (buf + l, sizeof (buf) - l, #b " ")
    FLAG (IFF_UP);
    FLAG (IFF_BROADCAST);
    FLAG (IFF_DEBUG);
    FLAG (IFF_LOOPBACK);
    FLAG (IFF_POINTOPOINT);
    FLAG (IFF_RUNNING);
    FLAG (IFF_NOARP);
    FLAG (IFF_PROMISC);
    FLAG (IFF_NOTRAILERS);
    FLAG (IFF_ALLMULTI);
    FLAG (IFF_MASTER);
    FLAG (IFF_SLAVE);
    FLAG (IFF_MULTICAST);
    FLAG (IFF_PORTSEL);
    FLAG (IFF_AUTOMEDIA);
    FLAG (IFF_DYNAMIC);
#undef FLAG

    return buf;
}

int main (void)
{
    static struct ifreq ifreqs[32] {};
    struct ifconf ifconf {};
    ifconf.ifc_req = ifreqs;
    ifconf.ifc_len = sizeof(ifreqs);

    int sd = ::socket (PF_INET, SOCK_STREAM, 0);
    assert (sd >= 0);

    int r = ioctl (sd, SIOCGIFCONF, (char*)&ifconf);
    assert (r == 0);

    for (int i = 0; i < ifconf.ifc_len / sizeof(struct ifreq); ++i) {
        printf ("%s: %s\n", ifreqs[i].ifr_name, inet_ntoa (((struct sockaddr_in*)&ifreqs[i].ifr_addr)->sin_addr));
        printf (" flags: %s\n", flags (sd, ifreqs[i].ifr_name));
    }

    close (sd);

    return 0;
}
#endif

#if qStroika_Platform_Windows
// /SEE THIS CODE FOR WINDOWS
//http ://support.microsoft.com/default.aspx?scid=http://support.microsoft.com:80/support/kb/articles/Q129/3/15.asp&NoWebContent=1
#endif

InternetAddress Network::GetPrimaryInternetAddress ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"IO::Network::GetPrimaryInternetAddress"};
#endif
/// HORRIBLY KLUDGY BAD IMPL!!!
#if qStroika_Platform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#if 0
    DWORD TEST = GetComputerNameEx((COMPUTER_NAME_FORMAT)cnf, buffer, &dwSize))
#endif
    char ac[1024];
    if (::gethostname (ac, sizeof (ac)) == SOCKET_ERROR) {
        DbgTrace ("gethostname: err={}"_f, WSAGetLastError ());
        return InternetAddress{};
    }
    Sequence<InternetAddress> allAddrs    = DNS::kThe.GetHostAddresses (String::FromNarrowSDKString (ac));
    Sequence<InternetAddress> allNotLocal = allAddrs.Where ([] (const InternetAddress& ia) { return not ia.IsLinkLocalAddress (); });
    if (auto f = allNotLocal.First ()) {
        return *f;
    }
    if (auto f = allAddrs.First ()) {
        return *f;
    }
    return InternetAddress{};
#elif qStroika_Platform_POSIX
    // the first IPv4 address of an interface that is up, running and not loopback (getifaddrs, not SIOCGIFCONF - whose
    // records macOS packs to varying lengths, starting with AF_LINK ones, so they cannot be indexed as an array)
    ifaddrs* ifa = nullptr;
    if (::getifaddrs (&ifa) != 0) {
        Execution::ThrowPOSIXErrNo ();
    }
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([ifa] () noexcept { ::freeifaddrs (ifa); });
    for (const ifaddrs* p = ifa; p != nullptr; p = p->ifa_next) {
        if (p->ifa_addr != nullptr and p->ifa_addr->sa_family == AF_INET and (p->ifa_flags & IFF_UP) and (p->ifa_flags & IFF_RUNNING) and
            not(p->ifa_flags & IFF_LOOPBACK)) {
            sockaddr_in a{};
            ::memcpy (&a, p->ifa_addr, sizeof (a));
            return InternetAddress{a.sin_addr};
        }
    }
    return InternetAddress{};
#endif
}

String Network::GetPrimaryNetworkDeviceMacAddress ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"IO::Network::GetPrimaryNetworkDeviceMacAddress"};
#endif
    [[maybe_unused]] auto printMacAddr = [] (const uint8_t macaddrBytes[6]) -> String {
        char buf[100]{};
        (void)std::snprintf (buf, sizeof (buf), "%02x:%02x:%02x:%02x:%02x:%02x", macaddrBytes[0], macaddrBytes[1], macaddrBytes[2],
                             macaddrBytes[3], macaddrBytes[4], macaddrBytes[5]);
        return String{buf};
    };
#if qStroika_Platform_Linux
    // This counts on SIOCGIFHWADDR, which appears to be Linux specific
    for (SocketAddress::FamilyType family : {SocketAddress::INET, SocketAddress::INET6}) {
        ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (family, Socket::DGRAM);

        char   buf[10 * 1024];
        ifconf ifc;
        ifc.ifc_len = sizeof (buf);
        ifc.ifc_buf = buf;
        Execution::ThrowPOSIXErrNoIfNegative (::ioctl (s.GetNativeSocket (), SIOCGIFCONF, &ifc));

        const struct ifreq* const end = ifc.ifc_req + (ifc.ifc_len / sizeof (struct ifreq));
        for (const ifreq* it = ifc.ifc_req; it != end; ++it) {
            struct ifreq ifr{};
            Characters::CString::Copy (ifr.ifr_name, std::size (ifr.ifr_name), it->ifr_name);
            if (::ioctl (s.GetNativeSocket (), SIOCGIFFLAGS, &ifr) == 0) {
                if (!(ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
                    if (::ioctl (s.GetNativeSocket (), SIOCGIFHWADDR, &ifr) == 0) {
                        return printMacAddr (reinterpret_cast<const uint8_t*> (ifr.ifr_hwaddr.sa_data));
                    }
                }
            }
        }
    }
#elif qStroika_Platform_Windows
    // GetAdaptersInfo () fills a caller-sized buffer, and says how big it must be when it overflows (a fixed array of 10 threw
    // ERROR_BUFFER_OVERFLOW on any machine with more adapters, which Hyper-V, WSL and VPNs make common). So grow to that and
    // retry - as GetInterfaces_Windows_ does for GetAdaptersAddresses () - which allocates only if it outgrows the stack buffer.
    Memory::StackBuffer<IP_ADAPTER_INFO> adapterInfo;
Again:
    ULONG bufLen = static_cast<ULONG> (adapterInfo.GetSize () * sizeof (IP_ADAPTER_INFO));
    DWORD r      = ::GetAdaptersInfo (adapterInfo.begin (), &bufLen);
    if (r == ERROR_BUFFER_OVERFLOW) {
        adapterInfo.GrowToSize_uninitialized (bufLen / sizeof (IP_ADAPTER_INFO) + 1);
        goto Again;
    }
    if (r == ERROR_NO_DATA) {
        return String{}; // no adapters
    }
    Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (r);
    for (PIP_ADAPTER_INFO pi = adapterInfo.begin (); pi != nullptr; pi = pi->Next) {
        if (pi->AddressLength == 6) {
            return printMacAddr (pi->Address);
        }
    }
#elif qStroika_Platform_MacOS
    // As the Linux code above: the hardware address of the first non-loopback interface with an IPv4 address. macOS has no
    // SIOCGIFHWADDR - each interface's link-layer address is its getifaddrs () AF_LINK entry (a sockaddr_dl, often longer than
    // the struct, so copied by sa_len)
    ifaddrs* ifa = nullptr;
    if (::getifaddrs (&ifa) != 0) {
        Execution::ThrowPOSIXErrNo ();
    }
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([ifa] () noexcept { ::freeifaddrs (ifa); });
    for (const ifaddrs* p = ifa; p != nullptr; p = p->ifa_next) {
        if (p->ifa_addr != nullptr and p->ifa_addr->sa_family == AF_INET and not(p->ifa_flags & IFF_LOOPBACK)) {
            for (const ifaddrs* q = ifa; q != nullptr; q = q->ifa_next) {
                if (q->ifa_addr != nullptr and q->ifa_addr->sa_family == AF_LINK and ::strcmp (q->ifa_name, p->ifa_name) == 0) {
                    alignas (sockaddr_dl) uint8_t buf[sizeof (sockaddr_storage)]{};
                    ::memcpy (buf, q->ifa_addr, min<size_t> (q->ifa_addr->sa_len, sizeof (buf)));
                    const sockaddr_dl* sdl = reinterpret_cast<const sockaddr_dl*> (buf);
                    if (sdl->sdl_alen == 6 and offsetof (sockaddr_dl, sdl_data) + sdl->sdl_nlen + 6 <= sizeof (buf)) {
                        return printMacAddr (reinterpret_cast<const uint8_t*> (LLADDR (sdl)));
                    }
                }
            }
        }
    }
#else
    AssertNotImplemented ();
#endif
    return String{};
}

namespace {
    using Event      = LinkMonitor::Event;
    using LinkChange = LinkMonitor::LinkChange;

#if qStroika_Platform_Windows
    // a GUID as IP_ADAPTER_ADDRESSES::AdapterName spells it - so as Interface::fInterfaceID: upper case, in braces
    String AdapterNameOf_ (const ::GUID& g)
    {
        char buf[64];
        (void)::snprintf (buf, std::size (buf), "{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", g.Data1, g.Data2, g.Data3,
                          g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
        return String{buf};
    }
#endif

    // an address on an interface - the interface by its index, as the OS's messages name it
    using IndexAndAddress_ = pair<unsigned int, InternetAddress>;

#if qStroika_Platform_Windows
    InternetAddress AddressOf_ (const SOCKADDR_INET& a)
    {
        return a.si_family == AF_INET6 ? InternetAddress{a.Ipv6.sin6_addr} : InternetAddress{a.Ipv4.sin_addr};
    }
#endif

#if qStroika_Platform_POSIX

    // BSD's kernel puts the interface's index in bytes 2-3 of a link-local IPv6 address it hands out (KAME's 'embedded scope');
    // the address itself has 0 there. (As of macOS 26.5 getifaddrs () clears them itself; a routing message may not.)
    in6_addr WithoutEmbeddedScope_ (in6_addr a)
    {
        if (a.s6_addr[0] == 0xfe and (a.s6_addr[1] & 0xc0) == 0x80) {
            a.s6_addr[2] = 0;
            a.s6_addr[3] = 0;
        }
        return a;
    }

    // the addresses on the interfaces now - so one reported again (its lifetime renewed, say) is not taken for one added
    Containers::Set<IndexAndAddress_> CurrentAddresses_ ()
    {
        Containers::Set<IndexAndAddress_> result;
        ifaddrs*                          ifa = nullptr;
        Execution::ThrowPOSIXErrNoIfNegative (::getifaddrs (&ifa));
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([ifa] () noexcept { ::freeifaddrs (ifa); });
        for (const ifaddrs* p = ifa; p != nullptr; p = p->ifa_next) {
            if (p->ifa_addr == nullptr) {
                continue;
            }
            string device{p->ifa_name};
            if (size_t colon = device.find (':'); colon != string::npos) {
                device.erase (colon); // a Linux address label (eth0:1): the address is its device's
            }
            unsigned int index = ::if_nametoindex (device.c_str ());
            if (index == 0) {
                continue;
            }
            if (p->ifa_addr->sa_family == AF_INET) {
                sockaddr_in sin;
                ::memcpy (&sin, p->ifa_addr, sizeof (sin));
                result.Add (IndexAndAddress_{index, InternetAddress{sin.sin_addr}});
            }
            else if (p->ifa_addr->sa_family == AF_INET6) {
                sockaddr_in6 sin6;
                ::memcpy (&sin6, p->ifa_addr, sizeof (sin6));
                result.Add (IndexAndAddress_{index, InternetAddress{WithoutEmbeddedScope_ (sin6.sin6_addr)}});
            }
        }
        return result;
    }
#endif

    // what one LinkMonitor has registered
    struct Subscriber_ {
        // held while its callbacks are called, and only then - so they run one at a time, though on Windows the OS's
        // notifications may come on more than one of its threads (nothing else takes it, so it cannot deadlock with a callback)
        mutex                                            fCalling;
        Execution::CallbackRegistry<void (const Event&)> fCallbacks;
    };

    // The one watcher of the OS's address changes, shared by every LinkMonitor with a callback: on Linux and macOS a thread
    // (reading a netlink or routing socket), on Windows a NotifyUnicastIpAddressChange registration. Started by the first
    // LinkMonitor to add a callback, and stopped when the last such goes - so it never outlives them (as no Stroika thread may
    // outlive main).
    struct Backend_ {
        static shared_ptr<Backend_> Get ()
        {
            [[maybe_unused]] lock_guard critSec{sMutex_};
            if (shared_ptr<Backend_> b = sCurrent_.lock ()) {
                return b;
            }
            shared_ptr<Backend_> b = Memory::MakeSharedPtr<Backend_> ();
            b->Start_ ();
            sCurrent_ = b;
            return b;
        }

        Backend_ ()                = default;
        Backend_ (const Backend_&) = delete;
        ~Backend_ ()
        {
            // the last LinkMonitor destroyed from within one of its callbacks: this cannot stop (and wait for) what is calling it
            Require (not tNotifying_);
#if qStroika_Platform_POSIX
            Execution::Thread::SuppressInterruptionInContext suppressInterruption; // critical to wait til done cuz captures this
            if (fMonitorThread_ != nullptr) {
                fMonitorThread_.AbortAndWaitForDone ();
            }
#elif qStroika_Platform_Windows
            if (fMonitorHandler_ != INVALID_HANDLE_VALUE) {
                // @todo should check error result, but then do what?
                // also - does this blcok until pending notifies done?
                // assuming so!!!
                ::CancelMibChangeNotify2 (fMonitorHandler_);
            }
#endif
        }

        void Add (const shared_ptr<Subscriber_>& s)
        {
            fSubscribers_.rwget ().rwref ().Add (s);
        }
        void Remove (const shared_ptr<Subscriber_>& s)
        {
            fSubscribers_.rwget ().rwref ().Remove (s);
        }

    private:
#if qStroika_Platform_POSIX
        // on the monitor thread: an address added or removed - reported only if it really was (known: the addresses there)
        void Changed_ (Containers::Set<IndexAndAddress_>* known, LinkChange c, unsigned int index, const InternetAddress& a)
        {
            if (c == LinkChange::eAdded ? known->AddIf (IndexAndAddress_{index, a}) : known->RemoveIf (IndexAndAddress_{index, a})) {
                char name[IF_NAMESIZE]{};
                Notify_ (Event{.fChange      = c,
                               .fInterfaceID = ::if_indextoname (index, name) == nullptr ? String{} : String::FromNarrowSDKString (name),
                               .fAddress     = a});
            }
        }
#endif

        void Notify_ (const Event& e)
        {
            tNotifying_                     = true;
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([] () noexcept { tNotifying_ = false; });
            // from a copy - a callback may add or remove LinkMonitors (and its CallbackRegistry lets it add or remove callbacks,
            // calls none removed meanwhile, and logs and ignores a callback's exception, which would stop the notifications)
            for (const shared_ptr<Subscriber_>& sub : fSubscribers_.load ()) {
                [[maybe_unused]] lock_guard critSec{sub->fCalling};
                sub->fCallbacks.Call (e);
            }
        }

#if qStroika_Platform_Windows
        // cannot use LAMBDA cuz we need WINAPI call convention
        static void WINAPI CB_ (void* callerContext, PMIB_UNICASTIPADDRESS_ROW Address, MIB_NOTIFICATION_TYPE NotificationType)
        {
            if (Address == NULL) {
                return; // MibInitialNotification (not asked for)
            }
            try {
                Backend_* b = reinterpret_cast<Backend_*> (callerContext);
                // Whether the address is usable (its DAD state Preferred) is what counts - not its being added or deleted:
                // Windows keeps an address whose network goes (Wi-Fi off, within its DHCP lease), marking it Deprecated, and
                // Preferred again when the network is back, telling of each only as a MibParameterNotification. (And it adds an
                // address Tentative, while it checks it is no duplicate.) The row given holds only the keys, so its state is
                // looked up.
                MIB_UNICASTIPADDRESS_ROW now{};
                now.Address       = Address->Address;
                now.InterfaceLuid = Address->InterfaceLuid;
                bool usable = NotificationType != MibDeleteInstance and ::GetUnicastIpAddressEntry (&now) == NO_ERROR and now.DadState == IpDadStatePreferred;
                InternetAddress a = AddressOf_ (Address->Address);
                bool            changed;
                {
                    [[maybe_unused]] lock_guard critSec{b->fUsableMutex_};
                    changed = usable ? b->fUsable_.AddIf (IndexAndAddress_{Address->InterfaceIndex, a})
                                     : b->fUsable_.RemoveIf (IndexAndAddress_{Address->InterfaceIndex, a});
                }
                if (changed) {
                    ::GUID guid{};
                    b->Notify_ (Event{.fChange = usable ? LinkChange::eAdded : LinkChange::eRemoved,
                                      .fInterfaceID = (::ConvertInterfaceLuidToGuid (&Address->InterfaceLuid, &guid) == NO_ERROR) ? AdapterNameOf_ (guid)
                                                                                                                                  : String{},
                                      .fAddress = a});
                }
            }
            catch (...) {
                // nothing may escape to the OS's thread
                DbgTrace ("LinkMonitor: {}"_f, current_exception ());
            }
        }
#endif

        void Start_ ()
        {
#if qStroika_Platform_Linux
            // the socket made (and bound) here, not on the thread: so a change from when AddCallback () returns is seen, and an
            // OS that cannot tell (some containers) makes AddCallback () throw - rather than the thread silently ending
            ConnectionlessSocket::Ptr sock = ConnectionlessSocket::New (static_cast<SocketAddress::FamilyType> (PF_NETLINK), Socket::RAW, NETLINK_ROUTE);
            {
                sockaddr_nl addr{};
                addr.nl_family = AF_NETLINK;
                addr.nl_groups = RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR;
                Execution::ThrowPOSIXErrNoIfNegative (::bind (sock.GetNativeSocket (), (struct sockaddr*)&addr, sizeof (addr)));
            }
            // (listed after the socket is bound, so no change is missed between)
            fMonitorThread_ = Execution::Thread::New ([this, sock, known = CurrentAddresses_ ()] () mutable {
                // wait here, not in ReceiveFrom: with no timeout, that can miss an Abort () that comes just before it blocks
                Execution::WaitForIOReady ready{sock.GetNativeSocket ()};
                alignas (nlmsghdr) byte   buffer[4096];
                while (true) {
                    (void)ready.Wait ();
                    int len = static_cast<int> (sock.ReceiveFrom (span{buffer}, 0, nullptr).size ());
                    if (len <= 0) {
                        break;
                    }
                    for (nlmsghdr* nlh = reinterpret_cast<nlmsghdr*> (buffer); NLMSG_OK (nlh, len) and nlh->nlmsg_type != NLMSG_DONE;
                         nlh           = NLMSG_NEXT (nlh, len)) {
                        if (nlh->nlmsg_type == RTM_NEWADDR or nlh->nlmsg_type == RTM_DELADDR) {
                            struct ifaddrmsg* ifa = (struct ifaddrmsg*)NLMSG_DATA (nlh);
                            // one still checking it is no duplicate (IPv6) is not usable yet: reported when that is done
                            if (nlh->nlmsg_type == RTM_NEWADDR and (ifa->ifa_flags & (IFA_F_TENTATIVE | IFA_F_DADFAILED))) {
                                continue;
                            }
                            // IFA_LOCAL is this end's address where there is a peer (IFA_ADDRESS then being the peer's); IPv6
                            // gives only IFA_ADDRESS
                            optional<InternetAddress> local;
                            optional<InternetAddress> address;
                            struct rtattr*            rth = IFA_RTA (ifa);
                            int                       rtl = IFA_PAYLOAD (nlh);
                            for (; RTA_OK (rth, rtl); rth = RTA_NEXT (rth, rtl)) {
                                if (rth->rta_type == IFA_LOCAL or rth->rta_type == IFA_ADDRESS) {
                                    optional<InternetAddress>& into = rth->rta_type == IFA_LOCAL ? local : address;
                                    if (ifa->ifa_family == AF_INET and RTA_PAYLOAD (rth) >= sizeof (in_addr)) {
                                        in_addr a;
                                        ::memcpy (&a, RTA_DATA (rth), sizeof (a));
                                        into = InternetAddress{a};
                                    }
                                    else if (ifa->ifa_family == AF_INET6 and RTA_PAYLOAD (rth) >= sizeof (in6_addr)) {
                                        in6_addr a;
                                        ::memcpy (&a, RTA_DATA (rth), sizeof (a));
                                        into = InternetAddress{a};
                                    }
                                }
                            }
                            if (optional<InternetAddress> a = local ? local : address) {
                                Changed_ (&known, nlh->nlmsg_type == RTM_NEWADDR ? LinkChange::eAdded : LinkChange::eRemoved, ifa->ifa_index, *a);
                            }
                        }
                    }
                }
            });
            fMonitorThread_.SetThreadName ("Network LinkMonitor thread"sv);
            fMonitorThread_.Start ();
#elif qStroika_Platform_Windows
            /*
             * @todo    Minor - but we maybe should be using NotifyIpInterfaceChange... - not sure we get stragiht up/down issues this
             *          way...
             */
            Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (::NotifyUnicastIpAddressChange (AF_UNSPEC, &CB_, this, FALSE, &fMonitorHandler_));
            // the addresses usable now (listed after registering, so no change is missed between) - so only a change is reported
            PMIB_UNICASTIPADDRESS_TABLE table = nullptr;
            if (::GetUnicastIpAddressTable (AF_UNSPEC, &table) == NO_ERROR) {
                [[maybe_unused]] auto&&     cleanup = Execution::Finally ([table] () noexcept { ::FreeMibTable (table); });
                [[maybe_unused]] lock_guard critSec{fUsableMutex_};
                for (ULONG i = 0; i < table->NumEntries; ++i) {
                    if (table->Table[i].DadState == IpDadStatePreferred) {
                        fUsable_.Add (IndexAndAddress_{table->Table[i].InterfaceIndex, AddressOf_ (table->Table[i].Address)});
                    }
                }
            }
#elif qStroika_Platform_MacOS
            // as the Linux netlink loop above (the socket made, and the addresses listed, here too), via the BSD routing socket:
            // each address added or removed (RTM_NEWADDR, RTM_DELADDR)
            ConnectionlessSocket::Ptr sock = ConnectionlessSocket::New (static_cast<SocketAddress::FamilyType> (PF_ROUTE), Socket::RAW, AF_UNSPEC);
            fMonitorThread_ = Execution::Thread::New ([this, sock, known = CurrentAddresses_ ()] () mutable {
                // wait here, not in ReceiveFrom: with no timeout, that can miss an Abort () that comes just before it blocks
                Execution::WaitForIOReady ready{sock.GetNativeSocket ()};
                byte                      buffer[4096];
                while (true) {
                    (void)ready.Wait ();
                    size_t len = sock.ReceiveFrom (span{buffer}, 0, nullptr).size ();
                    if (len == 0) {
                        break;
                    }
                    // Every routing message starts {u_short msglen; u_char version; u_char type}. Copied out, never cast in
                    // place: like SIOCGIFCONF's records, these are packed
                    for (size_t offset = 0; offset + sizeof (ifa_msghdr) <= len;) {
                        ifa_msghdr ifam;
                        ::memcpy (&ifam, buffer + offset, sizeof (ifam));
                        if (ifam.ifam_msglen == 0) {
                            break;
                        }
                        size_t end = min<size_t> (offset + ifam.ifam_msglen, len);
                        if (ifam.ifam_type == RTM_NEWADDR or ifam.ifam_type == RTM_DELADDR) {
                            // its addresses follow the header: one per bit set in ifam_addrs, in RTAX_ order, each padded to 4 bytes
                            size_t a = offset + sizeof (ifam);
                            for (int i = 0; i < RTAX_MAX and a < end; ++i) {
                                if (not(ifam.ifam_addrs & (1 << i))) {
                                    continue;
                                }
                                uint8_t saLen = static_cast<uint8_t> (buffer[a]);
                                if (i == RTAX_IFA and a + 2 <= end) {
                                    LinkChange c      = ifam.ifam_type == RTM_NEWADDR ? LinkChange::eAdded : LinkChange::eRemoved;
                                    uint8_t    family = static_cast<uint8_t> (buffer[a + 1]); // a BSD sockaddr: {sa_len, sa_family, ...}
                                    if (family == AF_INET and saLen >= sizeof (sockaddr_in) and a + sizeof (sockaddr_in) <= end) {
                                        sockaddr_in sin;
                                        ::memcpy (&sin, buffer + a, sizeof (sin));
                                        Changed_ (&known, c, ifam.ifam_index, InternetAddress{sin.sin_addr});
                                    }
                                    else if (family == AF_INET6 and saLen >= sizeof (sockaddr_in6) and a + sizeof (sockaddr_in6) <= end) {
                                        sockaddr_in6 sin6;
                                        ::memcpy (&sin6, buffer + a, sizeof (sin6));
                                        Changed_ (&known, c, ifam.ifam_index, InternetAddress{WithoutEmbeddedScope_ (sin6.sin6_addr)});
                                    }
                                }
                                a += saLen > 0 ? (1 + ((saLen - 1) | (sizeof (uint32_t) - 1))) : sizeof (uint32_t);
                            }
                        }
                        offset += ifam.ifam_msglen;
                    }
                }
            });
            fMonitorThread_.SetThreadName ("Network LinkMonitor thread"sv);
            fMonitorThread_.Start ();
#else
            AssertNotImplemented ();
#endif
        }

        Execution::Synchronized<Containers::Collection<shared_ptr<Subscriber_>>> fSubscribers_;
#if qStroika_Platform_POSIX
        Execution::Thread::Ptr fMonitorThread_;
#elif qStroika_Platform_Windows
        HANDLE                            fMonitorHandler_ = INVALID_HANDLE_VALUE;
        mutex                             fUsableMutex_; // the OS's callbacks may come on more than one of its threads
        Containers::Set<IndexAndAddress_> fUsable_;      // the addresses usable now (DAD state Preferred)
#endif
        static inline mutex              sMutex_; // guards sCurrent_
        static inline weak_ptr<Backend_> sCurrent_;
        static inline thread_local bool  tNotifying_{false};
    };
}

struct LinkMonitor::Rep_ {
    ~Rep_ ()
    {
        if (fBackend_ != nullptr) {
            fBackend_->Remove (fSubscriber_); // no more notifications to it
            // and none still running - unless here, a callback destroying its own LinkMonitor: then it calls none of the rest
            fSubscriber_->fCallbacks.RemoveAll ();
        }
        // then fBackend_ goes - so it stops, if this was the last LinkMonitor using it
    }
    CallbackID AddCallback (const Callback& callback)
    {
        {
            [[maybe_unused]] lock_guard critSec{fBackendMutex_};
            if (fBackend_ == nullptr) {
                fBackend_ = Backend_::Get (); // (throws if the OS cannot tell: so nothing is added)
                fBackend_->Add (fSubscriber_);
            }
        }
        return fSubscriber_->fCallbacks.Add (callback);
    }
    void RemoveCallback (CallbackID callback)
    {
        fSubscriber_->fCallbacks.Remove (callback); // (waits for it, if running on another thread)
    }
    const shared_ptr<Subscriber_> fSubscriber_{Memory::MakeSharedPtr<Subscriber_> ()};
    mutex                         fBackendMutex_;
    shared_ptr<Backend_>          fBackend_; // from the first AddCallback; guarded by fBackendMutex_
};

/*
 ********************************************************************************
 ************************* IO::Network::LinkMonitor *****************************
 ********************************************************************************
 */
String LinkMonitor::Event::ToString () const
{
    Characters::StringBuilder sb;
    sb << "{"sv;
    sb << "change: "sv << fChange;
    sb << ", interfaceID: "sv << fInterfaceID;
    sb << ", address: "sv << fAddress;
    sb << "}"sv;
    return sb;
}

LinkMonitor::LinkMonitor ()
    : fRep_{Memory::MakeSharedPtr<Rep_> ()}
{
}

auto LinkMonitor::AddCallback (const Callback& callback) -> CallbackID
{
    return fRep_->AddCallback (callback);
}

void LinkMonitor::RemoveCallback (CallbackID callback)
{
    fRep_->RemoveCallback (callback);
}
