/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if qPlatform_POSIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include     "../../Execution/ErrNoException.h"
#if     qPlatform_Windows
#include "../../../Foundation/Execution/Platform/Windows/Exception.h"
#endif

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

#if qPlatform_Windows

// /SEE THIS COD EOFR WINDOWS
//http ://support.microsoft.com/default.aspx?scid=http://support.microsoft.com:80/support/kb/articles/Q129/3/15.asp&NoWebContent=1


#include <winsock.h>
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


#if qPlatform_Windows
    CheckStarup_ ();

#if 0
    DWORD TEST = GetComputerNameEx((COMPUTER_NAME_FORMAT)cnf, buffer, &dwSize))
#endif

#if 0

    WSAData wsaData;
    if (WSAStartup (MAKEWORD (1, 1), &wsaData) != 0) {
//      return 255;
    }
#endif

    char ac[1024];
    if (gethostname (ac, sizeof(ac)) == SOCKET_ERROR) {


        return InternetAddress ();
    }
    struct hostent* phe = gethostbyname (ac);
    if (phe == 0) {
    return InternetAddress ();
    }
    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy (&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        //cout << "Address " << i << ": " << inet_ntoa (addr) << endl;
        return InternetAddress (addr);
    }

    return 0;

#elif   qPlatform_POSIX

    //// HORRIBLY IMPL - NOT THREADSAFE
    char ac[1024];
    if (gethostname (ac, sizeof(ac)) < 0) {


        return InternetAddress ();
    }
    struct hostent* phe = gethostbyname (ac);
    if (phe == 0) {
        return InternetAddress ();
    }
    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy (&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        //cout << "Address " << i << ": " << inet_ntoa (addr) << endl;
        return InternetAddress (addr);
    }

    //static struct ifreq ifreqs[32];
    return InternetAddress ();
#endif
}
