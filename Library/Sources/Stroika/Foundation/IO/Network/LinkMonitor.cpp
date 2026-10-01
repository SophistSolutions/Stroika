/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

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
#include "Stroika/Foundation/Execution/Thread.h"
#if qStroika_Platform_Windows
#include "Platform/Windows/WinSock.h"
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include "Stroika/Foundation/IO/Network/DNS.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

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
    IP_ADAPTER_INFO adapterInfo[10];
    DWORD           dwBufLen = sizeof (adapterInfo);
    Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (::GetAdaptersInfo (adapterInfo, &dwBufLen));
    for (PIP_ADAPTER_INFO pi = adapterInfo; pi != nullptr; pi = pi->Next) {
        // check attributes - IF TEST to see if good adaptoer
        // @todo
        return printMacAddr (pi->Address);
    }
#else
    AssertNotImplemented ();
#endif
    return String{};
}

struct LinkMonitor::Rep_ {
    void AddCallback (const Callback& callback)
    {
        fCallbacks_.Add (callback);
        StartMonitorIfNeeded_ ();
    }
    void RemoveCallback (const Callback& callback)
    {
        fCallbacks_.Remove (callback);
        // @todo - add some such StopMonitorIfNeeded_();
    }
    Containers::Collection<Callback> fCallbacks_;
#if qStroika_Platform_POSIX
    Execution::Thread::Ptr fMonitorThread_;
#endif
#if qStroika_Platform_Windows
    HANDLE fMonitorHandler_ = INVALID_HANDLE_VALUE;
#endif

    void SendNotifies (LinkChange lc, const String& linkName, const String& ipAddr)
    {
        for (const auto& cb : fCallbacks_) {
            cb (lc, linkName, ipAddr);
        }
    }

#if qStroika_Platform_Windows
    // cannot use LAMBDA cuz we need WINAPI call convention
    static void WINAPI CB_ (void* callerContext, PMIB_UNICASTIPADDRESS_ROW Address, MIB_NOTIFICATION_TYPE NotificationType)
    {
        Rep_* rep = reinterpret_cast<Rep_*> (callerContext);
        if (Address != NULL) {
            char ipAddrBuf[1024];
            (void)snprintf (ipAddrBuf, std::size (ipAddrBuf), "%d.%d.%d.%d", Address->Address.Ipv4.sin_addr.s_net,
                            Address->Address.Ipv4.sin_addr.s_host, Address->Address.Ipv4.sin_addr.s_lh, Address->Address.Ipv4.sin_addr.s_impno);
            LinkChange lc = (NotificationType == MibDeleteInstance) ? LinkChange::eRemoved : LinkChange::eAdded;
            rep->SendNotifies (lc, String{}, String{ipAddrBuf});
        }
    }
#endif

    void StartMonitorIfNeeded_ ()
    {
#if qStroika_Platform_Linux
        if (fMonitorThread_ == nullptr) {
            // very slight race starting this but not worth worrying about
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

                //
                /// @todo - PROBABLY REDO USING Socket::Recv () - but we have none right now!!!
                //          -- LGP 2014-01-23
                //

                int              len;
                char             buffer[4096];
                struct nlmsghdr* nlh;
                nlh = (struct nlmsghdr*)buffer;
                while ((len = ::recv (sock.GetNativeSocket (), nlh, 4096, 0)) > 0) {
                    while ((NLMSG_OK (nlh, len)) and (nlh->nlmsg_type != NLMSG_DONE)) {
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
                                        SendNotifies (LinkChange::eAdded, String::FromNarrowSDKString (name), String{ipAddrBuf});
                                    }
                                }
                                rth = RTA_NEXT (rth, rtl);
                            }
                        }
                        nlh = NLMSG_NEXT (nlh, len);
                    }
                }
            });
            fMonitorThread_.SetThreadName ("Network LinkMonitor thread"sv);
            fMonitorThread_.Start ();
        }
#elif qStroika_Platform_Windows
        /*
        * @todo    Minor - but we maybe should be using NotifyIpInterfaceChange... - not sure we get stragiht up/down issues this
        *          way...
        */
        if (fMonitorHandler_ == INVALID_HANDLE_VALUE) {
            Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (::NotifyUnicastIpAddressChange (AF_INET, &CB_, this, FALSE, &fMonitorHandler_));
        }
#elif qStroika_Platform_MacOS
        if (fMonitorThread_ == nullptr) {
            // very slight race starting this but not worth worrying about
            fMonitorThread_ = Execution::Thread::New ([this] () {
                // As the Linux netlink loop above, via the BSD routing socket: report each IPv4 address added (RTM_NEWADDR).
                ConnectionlessSocket::Ptr sock = ConnectionlessSocket::New (static_cast<SocketAddress::FamilyType> (PF_ROUTE), Socket::RAW, AF_UNSPEC);
                byte    buffer[4096];
                ssize_t len;
                while ((len = ::recv (sock.GetNativeSocket (), buffer, sizeof (buffer), 0)) > 0) {
                    // Every routing message starts {u_short msglen; u_char version; u_char type}. Copied out, never cast in
                    // place: like SIOCGIFCONF's records, these are packed
                    for (size_t offset = 0; offset + sizeof (ifa_msghdr) <= static_cast<size_t> (len);) {
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
                                        SendNotifies (LinkChange::eAdded, String::FromNarrowSDKString (name),
                                                      InternetAddress{sin.sin_addr}.As<String> ());
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
        }
#else
        AssertNotImplemented ();
#endif
    }

    ~Rep_ ()
    {
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
