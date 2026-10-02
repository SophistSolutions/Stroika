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
    using Callback   = LinkMonitor::Callback;
    using LinkChange = LinkMonitor::LinkChange;

    // what one LinkMonitor has registered. fMutex is held while its callbacks run - so taking it waits for any running on
    // another thread; recursive, so a callback can add or remove callbacks itself
    struct Subscriber_ {
        recursive_mutex                  fMutex;
        Containers::Collection<Callback> fCallbacks; // guarded by fMutex
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
        void Notify_ (LinkChange lc, const String& linkName, const String& ipAddr)
        {
            tNotifying_                     = true;
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([] () noexcept { tNotifying_ = false; });
            // each from a copy - a callback may add or remove callbacks, or LinkMonitors - but one removed meanwhile is not called
            for (const shared_ptr<Subscriber_>& sub : fSubscribers_.load ()) {
                [[maybe_unused]] lock_guard critSec{sub->fMutex};
                for (const Callback& cb : Containers::Collection<Callback>{sub->fCallbacks}) {
                    if (sub->fCallbacks.Contains (cb)) {
                        try {
                            cb (lc, linkName, ipAddr);
                        }
                        catch (const Execution::Thread::AbortException&) {
                            Execution::ReThrow ();
                        }
                        catch (...) {
                            // not let out: it would stop the notifications to every LinkMonitor
                            DbgTrace ("LinkMonitor: ignoring exception from a callback: {}"_f, current_exception ());
                        }
                    }
                }
            }
        }

#if qStroika_Platform_Windows
        // cannot use LAMBDA cuz we need WINAPI call convention
        static void WINAPI CB_ (void* callerContext, PMIB_UNICASTIPADDRESS_ROW Address, MIB_NOTIFICATION_TYPE NotificationType)
        {
            if (Address != NULL) {
                char ipAddrBuf[1024];
                (void)snprintf (ipAddrBuf, std::size (ipAddrBuf), "%d.%d.%d.%d", Address->Address.Ipv4.sin_addr.s_net,
                                Address->Address.Ipv4.sin_addr.s_host, Address->Address.Ipv4.sin_addr.s_lh, Address->Address.Ipv4.sin_addr.s_impno);
                LinkChange lc = (NotificationType == MibDeleteInstance) ? LinkChange::eRemoved : LinkChange::eAdded;
                try {
                    reinterpret_cast<Backend_*> (callerContext)->Notify_ (lc, String{}, String{ipAddrBuf});
                }
                catch (...) {
                    // nothing may escape to the OS's thread
                    DbgTrace ("LinkMonitor: {}"_f, current_exception ());
                }
            }
        }
#endif

        void Start_ ()
        {
#if qStroika_Platform_Linux
            fMonitorThread_ = Execution::Thread::New ([this] () {
                // for now - only handle adds, but removes SB easy too...

                ConnectionlessSocket::Ptr sock =
                    ConnectionlessSocket::New (static_cast<SocketAddress::FamilyType> (PF_NETLINK), Socket::RAW, NETLINK_ROUTE);

                {
                    sockaddr_nl addr{};
                    addr.nl_family = AF_NETLINK;
                    addr.nl_groups = RTMGRP_IPV4_IFADDR;
                    Execution::ThrowPOSIXErrNoIfNegative (::bind (sock.GetNativeSocket (), (struct sockaddr*)&addr, sizeof (addr)));
                }

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
                        if (nlh->nlmsg_type == RTM_NEWADDR) {
                            struct ifaddrmsg* ifa = (struct ifaddrmsg*)NLMSG_DATA (nlh);
                            struct rtattr*    rth = IFA_RTA (ifa);
                            int               rtl = IFA_PAYLOAD (nlh);
                            while (rtl and RTA_OK (rth, rtl)) {
                                if (rth->rta_type == IFA_LOCAL) {
                                    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated\""); // macro uses 'register' - htons not deprecated
                                    uint32_t ipaddr = htonl (*((uint32_t*)RTA_DATA (rth))); //NB no '::' cuz some systems use macro
                                    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated\""); // macro uses 'register' - htons not deprecated
                                    char name[IFNAMSIZ];
                                    ::if_indextoname (ifa->ifa_index, name);
                                    {
                                        char ipAddrBuf[1024];
                                        ::snprintf (ipAddrBuf, std::size (ipAddrBuf), "%d.%d.%d.%d", (ipaddr >> 24) & 0xff,
                                                    (ipaddr >> 16) & 0xff, (ipaddr >> 8) & 0xff, ipaddr & 0xff);
                                        Notify_ (LinkChange::eAdded, String::FromNarrowSDKString (name), String{ipAddrBuf});
                                    }
                                }
                                rth = RTA_NEXT (rth, rtl);
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
            Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (::NotifyUnicastIpAddressChange (AF_INET, &CB_, this, FALSE, &fMonitorHandler_));
#elif qStroika_Platform_MacOS
            fMonitorThread_ = Execution::Thread::New ([this] () {
                // As the Linux netlink loop above, via the BSD routing socket: report each IPv4 address added (RTM_NEWADDR).
                ConnectionlessSocket::Ptr sock = ConnectionlessSocket::New (static_cast<SocketAddress::FamilyType> (PF_ROUTE), Socket::RAW, AF_UNSPEC);
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
                        if (ifam.ifam_type == RTM_NEWADDR) {
                            // its addresses follow the header: one per bit set in ifam_addrs, in RTAX_ order, each padded to 4 bytes
                            size_t a = offset + sizeof (ifam);
                            for (int i = 0; i < RTAX_MAX and a < end; ++i) {
                                if (not(ifam.ifam_addrs & (1 << i))) {
                                    continue;
                                }
                                uint8_t saLen = static_cast<uint8_t> (buffer[a]);
                                if (i == RTAX_IFA and saLen >= sizeof (sockaddr_in) and a + sizeof (sockaddr_in) <= end) {
                                    sockaddr_in sin;
                                    ::memcpy (&sin, buffer + a, sizeof (sin));
                                    char name[IF_NAMESIZE]{};
                                    if (sin.sin_family == AF_INET and ::if_indextoname (ifam.ifam_index, name) != nullptr) {
                                        Notify_ (LinkChange::eAdded, String::FromNarrowSDKString (name), InternetAddress{sin.sin_addr}.As<String> ());
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
        HANDLE fMonitorHandler_ = INVALID_HANDLE_VALUE;
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
            [[maybe_unused]] lock_guard critSec{fSubscriber_->fMutex};
            fSubscriber_->fCallbacks.clear ();
        }
        // then fBackend_ goes - so it stops, if this was the last LinkMonitor using it
    }
    void AddCallback (const Callback& callback)
    {
        [[maybe_unused]] lock_guard critSec{fSubscriber_->fMutex};
        if (fBackend_ == nullptr) {
            fBackend_ = Backend_::Get ();
            fBackend_->Add (fSubscriber_);
        }
        fSubscriber_->fCallbacks.Add (callback);
    }
    void RemoveCallback (const Callback& callback)
    {
        [[maybe_unused]] lock_guard critSec{fSubscriber_->fMutex}; // waits for it, if running on another thread
        fSubscriber_->fCallbacks.Remove (callback);
    }
    const shared_ptr<Subscriber_> fSubscriber_{Memory::MakeSharedPtr<Subscriber_> ()};
    shared_ptr<Backend_>          fBackend_; // from the first AddCallback; guarded by fSubscriber_->fMutex
};

/*
 ********************************************************************************
 ************************* IO::Network::LinkMonitor *****************************
 ********************************************************************************
 */
LinkMonitor::LinkMonitor ()
    : fRep_{Memory::MakeSharedPtr<Rep_> ()}
{
}

void LinkMonitor::AddCallback (const Callback& callback)
{
    fRep_->AddCallback (callback);
}

void LinkMonitor::RemoveCallback (const Callback& callback)
{
    fRep_->RemoveCallback (callback);
}
