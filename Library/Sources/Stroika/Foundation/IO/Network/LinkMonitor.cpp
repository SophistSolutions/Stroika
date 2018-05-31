/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <cstdio>

#if qPlatform_POSIX
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#if qPlatform_Linux
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif
#elif qPlatform_Windows
#include <WinSock2.h>

#include <WS2tcpip.h>

#include <Iphlpapi.h>
#include <netioapi.h>
#endif

#include "../../Characters/CString/Utilities.h"
#include "../../Containers/Collection.h"
#include "../../Execution/ErrNoException.h"
#include "../../Execution/Thread.h"
#if qPlatform_Windows
#include "../../../Foundation/Execution/Platform/Windows/Exception.h"
#include "Platform/Windows/WinSock.h"
#endif
#include "../../IO/Network/DNS.h"

#include "ConnectionlessSocket.h"

#include "LinkMonitor.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using namespace Stroika::Foundation;
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
#define FLAG(b)              \
    if (ifreq.ifr_flags & b) \
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

#if qPlatform_Windows
// /SEE THIS CODE FOR WINDOWS
//http ://support.microsoft.com/default.aspx?scid=http://support.microsoft.com:80/support/kb/articles/Q129/3/15.asp&NoWebContent=1
#endif

InternetAddress Network::GetPrimaryInternetAddress ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"IO::Network::GetPrimaryInternetAddress"};
#endif
/// HORRIBLY KLUDGY BAD IMPL!!!
#if qPlatform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#if 0
    DWORD TEST = GetComputerNameEx((COMPUTER_NAME_FORMAT)cnf, buffer, &dwSize))
#endif
    char ac[1024];
    if (::gethostname (ac, sizeof (ac)) == SOCKET_ERROR) {
        return InternetAddress ();
    }
#if 1
    // WAG at charset - whole thing not well done!
    for (InternetAddress i : DNS::Default ().GetHostAddresses (String::FromUTF8 (ac))) {
        return i;
    }
#else
    struct hostent* phe = gethostbyname (ac);
    if (phe == nullptr) {
        return InternetAddress ();
    }
    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        (void)::memcpy (&addr, phe->h_addr_list[i], sizeof (struct in_addr));
        return InternetAddress (addr);
    }
#endif
    return InternetAddress ();
#elif qPlatform_POSIX
    auto getFlags = [](int sd, const char* name) -> int {
        struct ifreq ifreq {
        };
        Characters::CString::Copy (ifreq.ifr_name, NEltsOf (ifreq.ifr_name), name);
        int r = ::ioctl (sd, SIOCGIFFLAGS, (char*)&ifreq);
        // Since this is used only to filter the list of addresses, if we get an error, dont throw but
        // return 0
        if (r < 0) {
            DbgTrace ("ioctl on getFlags returned %d, errno=%d", r, errno);
            return 0;
        }
        Assert (r == 0);
        return ifreq.ifr_flags;
    };

    struct ifreq  ifreqs[32]{};
    struct ifconf ifconf {
    };
    ifconf.ifc_req = ifreqs;
    ifconf.ifc_len = sizeof (ifreqs);

    int sd = ::socket (PF_INET, SOCK_STREAM, 0);
    Assert (sd >= 0);

    int r = ::ioctl (sd, SIOCGIFCONF, (char*)&ifconf);
    Assert (r == 0);

    InternetAddress result;
    for (int i = 0; i < ifconf.ifc_len / sizeof (struct ifreq); ++i) {
        int flags = getFlags (sd, ifreqs[i].ifr_name);
        if ((flags & IFF_UP) and (not(flags & IFF_LOOPBACK)) and (flags & IFF_RUNNING)) {
            result = InternetAddress (((struct sockaddr_in*)&ifreqs[i].ifr_addr)->sin_addr);
            break;
        }
        //printf ("%s: %s\n", ifreqs[i].ifr_name, inet_ntoa (((struct sockaddr_in*)&ifreqs[i].ifr_addr)->sin_addr));
        //printf (" flags: %s\n", flags (sd, ifreqs[i].ifr_name));
    }
    ::close (sd);
    return result;
#endif
}

String Network::GetPrimaryNetworkDeviceMacAddress ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"IO::Network::GetPrimaryNetworkDeviceMacAddress"};
#endif
    auto printMacAddr = [](const uint8_t macaddrBytes[6]) -> String {
        char buf[100]{};
        (void)std::snprintf (buf, sizeof (buf), "%02x:%02x:%02x:%02x:%02x:%02x",
                             macaddrBytes[0], macaddrBytes[1],
                             macaddrBytes[2], macaddrBytes[3],
                             macaddrBytes[4], macaddrBytes[5]);
        return String::FromASCII (buf);
    };
#if qPlatform_Linux
    // This counts on SIOCGIFHWADDR, which appears to be Linux specific
    for (SocketAddress::FamilyType family : initializer_list<SocketAddress::FamilyType>{SocketAddress::INET, SocketAddress::INET6}) {
        ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (family, Socket::DGRAM);

        char   buf[10 * 1024];
        ifconf ifc;
        ifc.ifc_len = sizeof (buf);
        ifc.ifc_buf = buf;
        Execution::ThrowErrNoIfNegative (::ioctl (s.GetNativeSocket (), SIOCGIFCONF, &ifc));

        const struct ifreq* const end = ifc.ifc_req + (ifc.ifc_len / sizeof (struct ifreq));
        for (const ifreq* it = ifc.ifc_req; it != end; ++it) {
            struct ifreq ifr {
            };
            Characters::CString::Copy (ifr.ifr_name, NEltsOf (ifr.ifr_name), it->ifr_name);
            if (::ioctl (s.GetNativeSocket (), SIOCGIFFLAGS, &ifr) == 0) {
                if (!(ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
                    if (::ioctl (s.GetNativeSocket (), SIOCGIFHWADDR, &ifr) == 0) {
                        return printMacAddr (reinterpret_cast<const uint8_t*> (ifr.ifr_hwaddr.sa_data));
                    }
                }
            }
        }
    }
#elif qPlatform_Windows
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
    return String ();
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
#if qPlatform_POSIX
    Execution::Thread::Ptr fMonitorThread_;
#endif
#if qPlatform_Windows
    HANDLE fMonitorHandler_ = INVALID_HANDLE_VALUE;
#endif

    void SendNotifies (LinkChange lc, const String& linkName, const String& ipAddr)
    {
        for (auto cb : fCallbacks_) {
            cb (lc, linkName, ipAddr);
        }
    }

#if qPlatform_Windows
    // cannot use LAMBDA cuz we need WINAPI call convention
    static void WINAPI CB_ (void* callerContext, PMIB_UNICASTIPADDRESS_ROW Address, MIB_NOTIFICATION_TYPE NotificationType)
    {
        Rep_* rep = reinterpret_cast<Rep_*> (callerContext);
        if (Address != NULL) {
            char ipAddrBuf[1024];
            (void)std::snprintf (ipAddrBuf, NEltsOf (ipAddrBuf), "%d.%d.%d.%d", Address->Address.Ipv4.sin_addr.s_net,
                                 Address->Address.Ipv4.sin_addr.s_host,
                                 Address->Address.Ipv4.sin_addr.s_lh,
                                 Address->Address.Ipv4.sin_addr.s_impno);
            LinkChange lc = (NotificationType == MibDeleteInstance) ? LinkChange::eRemoved : LinkChange::eAdded;
            rep->SendNotifies (lc, String (), String::FromASCII (ipAddrBuf));
        }
    }
#endif

    void StartMonitorIfNeeded_ ()
    {
#if qPlatform_Linux
        if (fMonitorThread_ == nullptr) {
            // very slight race starting this but not worth worrying about
            fMonitorThread_ = Execution::Thread::New ([this]() {
                // for now - only handle adds, but removes SB easy too...

                ConnectionlessSocket::Ptr sock = ConnectionlessSocket::New (static_cast<SocketAddress::FamilyType> (PF_NETLINK), Socket::RAW, NETLINK_ROUTE);

                {
                    sockaddr_nl addr;
                    memset (&addr, 0, sizeof (addr));
                    addr.nl_family = AF_NETLINK;
                    addr.nl_groups = RTMGRP_IPV4_IFADDR;
                    Execution::ThrowErrNoIfNegative (::bind (sock.GetNativeSocket (), (struct sockaddr*)&addr, sizeof (addr)));
                }

                //
                /// @todo - PROBABLY REDO USING Socket::Recv () - but we have none right now!!!
                //          -- LGP 2014-01-23
                //

                int              len;
                char             buffer[4096];
                struct nlmsghdr* nlh;
                nlh = (struct nlmsghdr*)buffer;
                while ((len = recv (sock.GetNativeSocket (), nlh, 4096, 0)) > 0) {
                    while ((NLMSG_OK (nlh, len)) && (nlh->nlmsg_type != NLMSG_DONE)) {
                        if (nlh->nlmsg_type == RTM_NEWADDR) {
                            struct ifaddrmsg* ifa = (struct ifaddrmsg*)NLMSG_DATA (nlh);
                            struct rtattr*    rth = IFA_RTA (ifa);
                            int               rtl = IFA_PAYLOAD (nlh);
                            while (rtl && RTA_OK (rth, rtl)) {
                                if (rth->rta_type == IFA_LOCAL) {
                                    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated\""); // macro uses 'register' - htons not deprecated
                                    uint32_t ipaddr = htonl (*((uint32_t*)RTA_DATA (rth)));                             //NB no '::' cuz some systems use macro
                                    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated\"");   // macro uses 'register' - htons not deprecated
                                    char name[IFNAMSIZ];
                                    if_indextoname (ifa->ifa_index, name);
                                    {
                                        char ipAddrBuf[1024];
                                        std::snprintf (ipAddrBuf, NEltsOf (ipAddrBuf), "%d.%d.%d.%d", (ipaddr >> 24) & 0xff, (ipaddr >> 16) & 0xff, (ipaddr >> 8) & 0xff, ipaddr & 0xff);
                                        SendNotifies (LinkChange::eAdded, String::FromNarrowSDKString (name), String::FromASCII (ipAddrBuf));
                                    }
                                }
                                rth = RTA_NEXT (rth, rtl);
                            }
                        }
                        nlh = NLMSG_NEXT (nlh, len);
                    }
                }
            });
            fMonitorThread_.SetThreadName (L"Network LinkMonitor thread");
            fMonitorThread_.Start ();
        }
#elif qPlatform_Windows
        /*
        * @todo    Minor - but we maybe should be using NotifyIpInterfaceChange... - not sure we get stragiht up/down issues this
        *          way...
        */
        if (fMonitorHandler_ == INVALID_HANDLE_VALUE) {
            Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (::NotifyUnicastIpAddressChange (AF_INET, &CB_, this, FALSE, &fMonitorHandler_));
        }
#else
        AssertNotImplemented ();
#endif
    }

    ~Rep_ ()
    {
#if qPlatform_Windows
        if (fMonitorHandler_ != INVALID_HANDLE_VALUE) {
            // @todo should check error result, but then do what?
            // also - does this blcok until pending notifies done?
            // assuming so!!!
            ::CancelMibChangeNotify2 (fMonitorHandler_);
        }
#elif qPlatform_POSIX
        Execution::Thread::SuppressInterruptionInContext suppressInterruption; // critical to wait til done cuz captures this
        if (fMonitorThread_ != nullptr) {
            fMonitorThread_.AbortAndWaitForDone ();
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
    : fRep_ (make_shared<Rep_> ())
{
}

LinkMonitor::LinkMonitor (const LinkMonitor&& rhs)
    : fRep_ (move (rhs.fRep_))
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
