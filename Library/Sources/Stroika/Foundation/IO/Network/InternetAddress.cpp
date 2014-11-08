/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <winsock2.h>
#include    <ws2tcpip.h>
#endif

#include    "../../Characters/String_Constant.h"
#include    "../../Debug/Trace.h"
#include     "../../Execution/ErrNoException.h"
#if     qPlatform_Windows
#include "../../../Foundation/Execution/Platform/Windows/Exception.h"
#endif

#include    "InternetAddress.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;



#if     qCompilerAndStdLib_constexpr_union_variants_Buggy
const   InternetAddress V4::kAddrAny    =   InternetAddress (in_addr {});
const   InternetAddress V6::kAddrAny    =   InternetAddress (in6_addr {});
const   InternetAddress V4::kLocalhost  =   InternetAddress { 0x7f, 0x0, 0x0, 0x1 };
const   InternetAddress V6::kLocalhost  =   InternetAddress (in6_addr { { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } } } );
#endif



#if     qPlatform_Windows
// Not sure why this is necessary, but we get link errors sometimes without it... Maybe a windows makefile issue on regtest apps?
// -- LGP 2014-11-06
#pragma comment(lib, "Ws2_32.lib")
#endif



#if     qPlatform_Windows
namespace {
    void    CheckStarup_ ()
    {
        static  bool    sStartedUp_ = false;
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



#if     qPlatform_Windows && (NTDDI_VERSION < NTDDI_VISTA)
namespace {
    int inet_pton (int af, const char* src, void* dst)
    {
        CheckStarup_ ();
        struct sockaddr_storage ss {};
        int size = sizeof(ss);
        char src_copy[INET6_ADDRSTRLEN + 1];
        /* stupid non-const API */
        strncpy (src_copy, src, INET6_ADDRSTRLEN + 1);
        src_copy[INET6_ADDRSTRLEN] = 0;
        if (WSAStringToAddressA(src_copy, af, NULL, (struct sockaddr*)&ss, &size) == 0) {
            switch(af) {
                case AF_INET:
                    *(struct in_addr*)dst = ((struct sockaddr_in*)&ss)->sin_addr;
                    return 1;
                case AF_INET6:
                    *(struct in6_addr*)dst = ((struct sockaddr_in6*)&ss)->sin6_addr;
                    return 1;
            }
        }
        return 0;
    }
    const char* inet_ntop (int af, const void* src, char* dst, socklen_t size)
    {
        CheckStarup_ ();
        struct sockaddr_storage ss {};
        ss.ss_family = af;
        unsigned long s = size;
        switch(af) {
            case AF_INET:
                ((struct sockaddr_in*)&ss)->sin_addr = *(struct in_addr*)src;
                break;
            case AF_INET6:
                ((struct sockaddr_in6*)&ss)->sin6_addr = *(struct in6_addr*)src;
                break;
            default:
                return NULL;
        }
        /* cannot direclty use &size because of strict aliasing rules */
        return (WSAAddressToStringA((struct sockaddr*)&ss, sizeof(ss), NULL, dst, &s) == 0) ? dst : NULL;
    }

}
#endif




/*
 ********************************************************************************
 ********************* IO::Network::InternetAddress *****************************
 ********************************************************************************
 */
InternetAddress::InternetAddress (const string& s, AddressFamily af)
    : fAddressFamily_ (AddressFamily::UNKNOWN)
{
    if (not s.empty ()) {
        if (af == AddressFamily::UNKNOWN) {
            // guess format - based on '.' versus ':' in name
            if (s.find ('.') != string::npos) {
                af = AddressFamily::V4;
            }
            else if (s.find (':') != string::npos) {
                af = AddressFamily::V6;
            }
        }
        switch (af) {
            case AddressFamily::V4: {
                    Execution::ThrowErrNoIfNegative (inet_pton (AF_INET, s.c_str (), &fV4_));
                    fAddressFamily_ = af;
                }
                break;
            case AddressFamily::V6: {
                    Execution::ThrowErrNoIfNegative (inet_pton (AF_INET6, s.c_str (), &fV6_));
                    fAddressFamily_ = af;
                }
                break;
            default: {
                    // @todo need better exception
                    Execution::DoThrow (Execution::StringException (String_Constant (L"Unrecognized address family")));
                }
                break;
        }
    }
}

InternetAddress::InternetAddress (const String& s, AddressFamily af)
    : InternetAddress (s.AsUTF8 (), af)
{
}

namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                template    <>
                String  InternetAddress::As<String> () const
                {
                    switch (fAddressFamily_) {
                        case AddressFamily::UNKNOWN: {
                                return String ();
                            }
                            break;
                        case AddressFamily::V4: {
                                char    buf[INET_ADDRSTRLEN + 1];
                                const char*   result  =   ::inet_ntop (AF_INET, &fV4_, buf, sizeof (buf));
                                return result == nullptr ? String () : String::FromUTF8 (result);
                            }
                            break;
                        case AddressFamily::V6: {
                                char    buf[INET6_ADDRSTRLEN + 1];
                                const char*   result  =   ::inet_ntop (AF_INET6, &fV6_, buf, sizeof (buf));
                                return result == nullptr ? String () : String::FromUTF8 (result);
                            }
                            break;
                        default: {
                                RequireNotReached ();
                                return String ();
                            }
                            break;
                    }
                }
            }
        }
    }
}

bool    InternetAddress::IsLocalhostAddress () const
{
    Require (not empty ());
    switch (fAddressFamily_) {
        case AddressFamily::V4: {
                // 127.0.0.x
                IPv4AddressOctets   octets = As<IPv4AddressOctets> ();
                return get<0> (octets) == 0x7f and get<1> (octets) == 0x0 and get<2> (octets) == 0x0;
                //return (ntohl (fV4_.s_addr) & 0xffffff00) == 0x7f000000;
                //return (ntohl (fV4_.s_addr) & 0x00ffffff) == 0x00007f;
            }
            break;
        case AddressFamily::V6: {
                return
                    fV6_.s6_addr[0] == 0 and
                    fV6_.s6_addr[1] == 0 and
                    fV6_.s6_addr[2] == 0 and
                    fV6_.s6_addr[3] == 0 and
                    fV6_.s6_addr[4] == 0 and
                    fV6_.s6_addr[5] == 0 and
                    fV6_.s6_addr[6] == 0 and
                    fV6_.s6_addr[7] == 0 and
                    fV6_.s6_addr[8] == 0 and
                    fV6_.s6_addr[9] == 0 and
                    fV6_.s6_addr[10] == 0 and
                    fV6_.s6_addr[11] == 0 and
                    fV6_.s6_addr[12] == 0 and
                    fV6_.s6_addr[13] == 0 and
                    fV6_.s6_addr[14] == 0 and
                    fV6_.s6_addr[15] == 1
                    ;
            }
            break;
    }
    AssertNotReached ();
    return false;
}

bool    InternetAddress::IsLinkLocalAddress () const
{
    Require (not empty ());
    switch (fAddressFamily_) {
        case AddressFamily::V4: {
                static  const   InternetAddress kMinLinkLocal_  { 169, 254, 0, 1 };
                static  const   InternetAddress kMaxLinkLocal_  { 169, 254, 255, 254 };
                Assert (kMinLinkLocal_ < kMaxLinkLocal_);
                return kMinLinkLocal_ <= *this and * this <= kMaxLinkLocal_;
            }
            break;
        case AddressFamily::V6: {
                return
                    fV6_.s6_addr[0] == 0xfe and fV6_.s6_addr[1] == 0x80 and
                    fV6_.s6_addr[2] == 0x0 and fV6_.s6_addr[3] == 0x0 and
                    fV6_.s6_addr[4] == 0x0 and fV6_.s6_addr[5] == 0x0 and
                    fV6_.s6_addr[6] == 0x0 and fV6_.s6_addr[7] == 0x0
                    ;
            }
            break;
    }
    AssertNotReached ();
    return false;
}

bool    InternetAddress::IsPrivateAddress () const
{
#if     qDebug && defined (s_net)
    auto ipv4Checker = [](in_addr n) -> bool {
        if (n.s_net == 10)
        {
            return true;
        }
        else if (n.s_net == 172 and (16 <= n.s_host and n.s_host == 31))
        {
            return true;
        }
        else if (n.s_net == 192 and n.s_host == 168)
        {
            return true;
        }
        return false;
    };
#endif
#if     qDebug && defined (s6_words)
    auto ipv6Checker = [](in6_addr n) -> bool {
        return n.s6_words[0] == 0xfc00;
    };
#endif
    switch (fAddressFamily_) {
        case AddressFamily::V4: {
                /*
                 * http://www.faqs.org/rfcs/rfc1918.html
                 *
                 *  3. Private Address Space
                 *
                 *      The Internet Assigned Numbers Authority (IANA) has reserved the
                 *      following three blocks of the IP address space for private internets:
                 *
                 *      10.0.0.0        -   10.255.255.255  (10/8 prefix)
                 *      172.16.0.0      -   172.31.255.255  (172.16/12 prefix)
                 *      192.168.0.0     -   192.168.255.255 (192.168/16 prefix)
                 */
                IPv4AddressOctets   octets = As<IPv4AddressOctets> ();
                if (get<0> (octets) == 10) {
                    return true;
                }
                if (get<0> (octets) == 172 and (get<1> (octets) & 0xf0) == 16) {
                    return true;
                }
                if (get<0> (octets) == 192 and get<1> (octets) == 168) {
                    return true;
                }
                return false;
            }
            break;
        case AddressFamily::V6: {
                /*
                 *  From http://en.wikipedia.org/wiki/Private_network
                 *
                 *      The concept of private networks and special address reservation for such networks
                 *      has been carried over to the next generation of the Internet Protocol, IPv6.
                 *      The address block fc00:: / 7 has been reserved by IANA as described in RFC 4193.
                 *      These addresses are called Unique Local Addresses (ULA).They are defined as being
                 *      unicast in character and contain a 40 - bit random number in the routing prefix.
                 */
                bool result = (fV6_.s6_addr[0] == 0xfc or fV6_.s6_addr[0] == 0xfd) and fV6_.s6_addr[1] == 0x0;
#if     defined (s6_words)
                Assert (ipv6Checker (fV6_) == result);
#endif
                return result;
            }
            break;
    }
    AssertNotReached ();
    return false;
}

bool    InternetAddress::IsMulticastAddress () const
{
    Require (not empty ());
    switch (fAddressFamily_) {
        case AddressFamily::V4: {
                // Not sure - might have byte order backwards??? or totally wrong - a bit of a guess?
                return (ntohl (fV4_.s_addr) & 0xf0000000) == 0xe0000000;
            }
            break;
        case AddressFamily::V6: {
                return (fV6_.s6_addr[0] == 0xff);
            }
            break;
    }
    AssertNotReached ();
    return false;
}

int InternetAddress::Compare (const InternetAddress& rhs) const
{
    if (fAddressFamily_ != rhs.fAddressFamily_) {
        return fAddressFamily_ < rhs.fAddressFamily_ ? -1 : 1;
    }
    switch (fAddressFamily_) {
        case AddressFamily::UNKNOWN: {
                return 0;
            }
            break;
        case AddressFamily::V4: {
                // if not equal, compare by net/host before other things so we get sensible intuitive ordering
                if (memcmp (&fV4_, &rhs.fV4_, sizeof (fV4_)) == 0) {
                    return 0;
                }
#if     qPlatform_POSIX
                if (inet_netof (fV4_) != inet_netof (rhs.fV4_)) {
                    return static_cast<int> (inet_netof (fV4_)) - static_cast<int> (inet_netof (rhs.fV4_));
                }
                if (inet_lnaof (fV4_) != inet_lnaof (rhs.fV4_)) {
                    return static_cast<int> (inet_lnaof (fV4_)) - static_cast<int> (inet_lnaof (rhs.fV4_));
                }
#elif   qPlatform_Windows
                if (fV4_.s_net != rhs.fV4_.s_net) {
                    return static_cast<int> (fV4_.s_net) - static_cast<int> (rhs.fV4_.s_net);
                }
                if (fV4_.s_host != rhs.fV4_.s_host) {
                    return static_cast<int> (fV4_.s_host) - static_cast<int> (rhs.fV4_.s_host);
                }
                if (fV4_.s_lh != rhs.fV4_.s_lh) {
                    return static_cast<int> (fV4_.s_lh) - static_cast<int> (rhs.fV4_.s_lh);
                }
                if (fV4_.s_impno != rhs.fV4_.s_impno) {
                    return static_cast<int> (fV4_.s_impno) - static_cast<int> (rhs.fV4_.s_impno);
                }
#endif
                AssertNotReached ();
                return 0;
            }
            break;
        case AddressFamily::V6: {
                return memcmp (&fV6_, &fV6_, sizeof (fV6_));
            }
            break;
    }
    AssertNotReached ();
    return 0;
}
