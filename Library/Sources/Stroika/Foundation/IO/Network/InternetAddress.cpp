/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <winsock2.h>
#include    <ws2tcpip.h>
#endif

#include    "../../Characters/String_Constant.h"
#include    "../../Characters/CString/Utilities.h"
#include    "../../Debug/Trace.h"
#include     "../../Execution/ErrNoException.h"
#if     qPlatform_Windows
#include    "../../../Foundation/Execution/Platform/Windows/Exception.h"
#include    "Platform/Windows/WinSock.h"
#endif
#include     "../../Memory/SmallStackBuffer.h"

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







#if     qPlatform_Windows && (NTDDI_VERSION < NTDDI_VISTA)
namespace {
    int     inet_pton (int af, const char* src, void* dst)
    {
        IO::Network::Platform::Windows::WinSock::AssureStarted ();
        struct sockaddr_storage ss {};
        int size = sizeof(ss);
        wchar_t src_copy[INET6_ADDRSTRLEN + 1]; // stupid non-const API
        {
            const char* si = src;
            for (; *si != '\0'; ++si) {
                src_copy[si - src] = *si;
                if (si - src >= INET6_ADDRSTRLEN) {
                    break;
                }
            }
            src_copy[si - src] = '\0';
        }
        if (::WSAStringToAddressW (src_copy, af, NULL, (struct sockaddr*)&ss, &size) == 0) {
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
        IO::Network::Platform::Windows::WinSock::AssureStarted ();
        struct sockaddr_storage ss {};
        ss.ss_family = af;
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
        Memory::SmallStackBuffer<wchar_t>   buf (size + 1);
        unsigned long s = size;
        DWORD   d = WSAAddressToStringW ((struct sockaddr*)&ss, sizeof(ss), NULL, buf.begin (), &s);
        if (d == 0) {
            const wchar_t* si = buf.begin ();
            Assert (s <= size_t (size));
            for (; si < buf.begin () + s; ++si) {
                dst[si - buf.begin ()] = static_cast<char> (*si);
                if (si - buf.begin () >= size) {
                    break;
                }
            }
            dst[si - buf.begin ()] = '\0';
            return dst;
        }
        else {
            return nullptr;
        }
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
#if     qCompilerAndStdLib_constexpr_union_variants_Buggy
                static  const   InternetAddress kMinLinkLocal_  { 169, 254, 0, 1 };
                static  const   InternetAddress kMaxLinkLocal_  { 169, 254, 255, 254 };
#else
                static  constexpr   InternetAddress kMinLinkLocal_  { 169, 254, 0, 1 };
                static  constexpr   InternetAddress kMaxLinkLocal_  { 169, 254, 255, 254 };
#endif
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
                // From http://en.wikipedia.org/wiki/Multicast_address :
                // The group includes the addresses from 224.0.0.0 to 239.255.255.255
                IPv4AddressOctets   octets = As<IPv4AddressOctets> ();
                return 224 <= get<0> (octets) and get<0> (octets) <= 239;
            }
            break;
        case AddressFamily::V6: {
                return fV6_.s6_addr[0] == 0xff;
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
                IPv4AddressOctets   lOctets = As<IPv4AddressOctets> ();
                IPv4AddressOctets   rOctets = rhs.As<IPv4AddressOctets> ();
                if (get<0> (lOctets) != get<0> (rOctets)) {
                    return static_cast<int> (get<0> (lOctets)) - static_cast<int> (get<0> (rOctets));
                }
                if (get<1> (lOctets) != get<1> (rOctets)) {
                    return static_cast<int> (get<1> (lOctets)) - static_cast<int> (get<1> (rOctets));
                }
                if (get<2> (lOctets) != get<2> (rOctets)) {
                    return static_cast<int> (get<2> (lOctets)) - static_cast<int> (get<2> (rOctets));
                }
                if (get<3> (lOctets) != get<3> (rOctets)) {
                    return static_cast<int> (get<3> (lOctets)) - static_cast<int> (get<3> (rOctets));
                }
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
