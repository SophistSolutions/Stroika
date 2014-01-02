/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <cstdio>

#if     qPlatform_POSIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#elif   qPlatform_Windows
#include    <WinSock2.h>
#include    <WS2tcpip.h>
#include    <Iphlpapi.h>
#endif

#include    "../../Characters/CString/Utilities.h"
#include    "../../Execution/ErrNoException.h"
#if     qPlatform_Windows
#include    "../../../Foundation/Execution/Platform/Windows/Exception.h"
#endif

#include    "Socket.h"

#include    "NetworkInterfaces.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;


#if 0
// FOR POSIX DO SOMETHING LIKE THIS:
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>


static const char* flags (int sd, const char* name)
{
    static char buf[1024];

    static struct ifreq ifreq;
    strcpy (ifreq.ifr_name, name);

    int r = ioctl (sd, SIOCGIFFLAGS, (char*)&ifreq);
    assert (r == 0);

    int l = 0;
#define FLAG(b) if(ifreq.ifr_flags & b) l += snprintf(buf + l, sizeof(buf) - l, #b " ")
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
    static struct ifreq ifreqs[32];
    struct ifconf ifconf;
    memset (&ifconf, 0, sizeof(ifconf));
    ifconf.ifc_req = ifreqs;
    ifconf.ifc_len = sizeof(ifreqs);

    int sd = socket (PF_INET, SOCK_STREAM, 0);
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



#if     qPlatform_Windows
// /SEE THIS COD EOFR WINDOWS
//http ://support.microsoft.com/default.aspx?scid=http://support.microsoft.com:80/support/kb/articles/Q129/3/15.asp&NoWebContent=1
#endif



#if     qPlatform_Windows
namespace {
    bool    sStartedUp_ = false;
    bool    sAutoSetup_ =   true;

    void    CheckStarup_ ()
    {
        if (not sStartedUp_) {
            WSADATA wsaData;        // Initialize Winsock
            int iResult = WSAStartup (MAKEWORD (2, 2), &wsaData);
            if (iResult != 0) {
                Execution::Platform::Windows::Exception::DoThrow (::WSAGetLastError ());
            }
            sStartedUp_ = true;
        }
    }
}
#endif


InternetAddress Network::GetPrimaryInternetAddress ()
{
    /// HORRIBLY KLUDGY BAD IMPL!!!
#if     qPlatform_Windows
    CheckStarup_ ();
#if 0
    DWORD TEST = GetComputerNameEx((COMPUTER_NAME_FORMAT)cnf, buffer, &dwSize))
#endif
    char ac[1024];
    if (gethostname (ac, sizeof(ac)) == SOCKET_ERROR) {
        return InternetAddress ();
    }
    struct hostent* phe = gethostbyname (ac);
    if (phe == nullptr) {
    return InternetAddress ();
    }
    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy (&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        return InternetAddress (addr);
    }
    return InternetAddress ();
#elif   qPlatform_POSIX
    auto getFlags = [] (int sd, const char* name) {
        char buf[1024];

        struct ifreq ifreq;
        memset(&ifreq, 0, sizeof (ifreq));
        strcpy (ifreq.ifr_name, name);

        int r = ioctl (sd, SIOCGIFFLAGS, (char*)&ifreq);
        Assert (r == 0);
        return (ifreq.ifr_flags);
    };

    struct ifreq ifreqs[32];
    struct ifconf ifconf;
    memset (&ifconf, 0, sizeof(ifconf));
    ifconf.ifc_req = ifreqs;
    ifconf.ifc_len = sizeof(ifreqs);

    int sd = socket (PF_INET, SOCK_STREAM, 0);
    Assert (sd >= 0);

    int r = ioctl (sd, SIOCGIFCONF, (char*)&ifconf);
    Assert (r == 0);

    InternetAddress result;
    for (int i = 0; i < ifconf.ifc_len / sizeof(struct ifreq); ++i) {
        int flags = getFlags (sd, ifreqs[i].ifr_name);
        if ((flags & IFF_UP)
                and (not (flags & IFF_LOOPBACK))
                and (flags & IFF_RUNNING)
           ) {
            result = InternetAddress (((struct sockaddr_in*)&ifreqs[i].ifr_addr)->sin_addr);
            break;
        }
        //printf ("%s: %s\n", ifreqs[i].ifr_name, inet_ntoa (((struct sockaddr_in*)&ifreqs[i].ifr_addr)->sin_addr));
        //printf (" flags: %s\n", flags (sd, ifreqs[i].ifr_name));
    }
    close (sd);
    return result;
#endif
}


String  Network::GetPrimaryNetworkDeviceMacAddress ()
{
    auto printMacAddr = [](const uint8_t macaddrBytes[6]) -> String {
        char     buf[100];
        (void)::memset (buf, 0, sizeof (buf));
        (void)snprintf (buf, sizeof (buf), "%02x-%02x-%02x-%02x-%02x-%02x",
        macaddrBytes[0], macaddrBytes[1],
        macaddrBytes[2], macaddrBytes[3],
        macaddrBytes[4], macaddrBytes[5]
                       );
        return String::FromAscii (buf);
    };
#if     qPlatform_Windows
    IP_ADAPTER_INFO adapterInfo[10];
    DWORD dwBufLen = sizeof(adapterInfo);
    Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (::GetAdaptersInfo (adapterInfo, &dwBufLen));
    for (PIP_ADAPTER_INFO pi = adapterInfo; pi != nullptr; pi = pi->Next) {
        // check attributes - IF TEST to see if good adaptoer
        // @todo
        return printMacAddr (pi->Address);
    }
#elif qPlatform_POSIX
    Socket s = Socket (Socket::SocketKind::DGRAM);

    ifconf   ifc;
    char buf[10 * 1024];
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    Execution::ThrowErrNoIfNegative (::ioctl (s.GetNativeSocket (), SIOCGIFCONF, &ifc));

    const struct ifreq* const end = ifc.ifc_req + (ifc.ifc_len / sizeof(struct ifreq));
    for (const ifreq* it = ifc.ifc_req ; it != end; ++it) {
        struct ifreq    ifr;
        memset (&ifr, 0, sizeof (ifr));
        Characters::CString::Copy (ifr.ifr_name, NEltsOf (ifr.ifr_name), it->ifr_name);
        if (::ioctl (s.GetNativeSocket (), SIOCGIFFLAGS, &ifr) == 0) {
            if (!(ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
                if (::ioctl (s.GetNativeSocket (), SIOCGIFHWADDR, &ifr) == 0) {
                    return printMacAddr (reinterpret_cast<const uint8_t*> (ifr.ifr_hwaddr.sa_data));
                }
            }
        }
    }
#endif
    return String ();
}


