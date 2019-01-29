/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <winsock2.h>

#include <ws2tcpip.h>
#endif

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/String_Constant.h"
#include "../../Containers/Collection.h"
#include "../../Debug/Trace.h"
#include "../../Execution/ErrNoException.h"
#if qPlatform_Windows
#include "../../../Foundation/Execution/Platform/Windows/Exception.h"
#include "Platform/Windows/WinSock.h"
#endif
#include "../../Memory/SmallStackBuffer.h"

#include "InternetAddress.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

#if qPlatform_Windows
// Not sure why this is necessary, but we get link errors sometimes without it... Maybe a windows makefile issue on regtest apps?
// -- LGP 2014-11-06
#pragma comment(lib, "Ws2_32.lib")
#endif

#if qPlatform_Windows && (NTDDI_VERSION < NTDDI_VISTA)
namespace {
    int inet_pton (int af, const char* src, void* dst)
    {
        IO::Network::Platform::Windows::WinSock::AssureStarted ();
        struct sockaddr_storage ss {
        };
        int     size = sizeof (ss);
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
            switch (af) {
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
        struct sockaddr_storage ss {
        };
        ss.ss_family = af;
        switch (af) {
            case AF_INET:
                ((struct sockaddr_in*)&ss)->sin_addr = *(struct in_addr*)src;
                break;
            case AF_INET6:
                ((struct sockaddr_in6*)&ss)->sin6_addr = *(struct in6_addr*)src;
                break;
            default:
                return NULL;
        }
        Memory::SmallStackBuffer<wchar_t> buf (size + 1);
        unsigned long                     s = size;
        DWORD                             d = WSAAddressToStringW ((struct sockaddr*)&ss, sizeof (ss), NULL, buf.begin (), &s);
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
        // queer API betware - inet_pton returns 0 (not negative) on error
        switch (af) {
            case AddressFamily::V4: {
                if (::inet_pton (AF_INET, s.c_str (), &fV4_) == 0) {
                    Execution::Throw (Execution::StringException (String_Constant (L"unable to parse string as IPv4 internet address")));
                }
                fAddressFamily_ = af;
            } break;
            case AddressFamily::V6: {
                if (::inet_pton (AF_INET6, s.c_str (), &fV6_) == 0) {
                    Execution::Throw (Execution::StringException (String_Constant (L"unable to parse string as IPv6 internet address")));
                }
                fAddressFamily_ = af;
            } break;
            default: {
                // @todo need better exception
                Execution::Throw (Execution::StringException (String_Constant (L"Unrecognized address family")));
            } break;
        }
    }
}

InternetAddress::InternetAddress (const String& s, AddressFamily af)
    : InternetAddress (s.AsUTF8 (), af)
{
}

namespace Stroika::Foundation::IO::Network {

    template <>
    vector<bool> InternetAddress::As<vector<bool>> () const
    {
        Require (GetAddressSize ().has_value ());
        size_t       sz = *GetAddressSize ();
        vector<bool> result;
        result.reserve (sz * 8);
        for (uint8_t b : As<vector<uint8_t>> ()) {
            // We could logically have numbered bits either way, but since this returns high order byte first only makes
            // sense to return high order bit first
            for (unsigned int i = 0; i < 8; ++i) {
                result.push_back (BitSubstring (b, 7 - i, 7 - i + 1));
            }
        }
        return result;
    }
    template <>
    String InternetAddress::As<String> () const
    {
        switch (fAddressFamily_) {
            case AddressFamily::UNKNOWN: {
                return String{};
            } break;
            case AddressFamily::V4: {
                char        buf[INET_ADDRSTRLEN + 1];
                const char* result = ::inet_ntop (AF_INET, &fV4_, buf, sizeof (buf));
                return result == nullptr ? String () : String::FromUTF8 (result);
            } break;
            case AddressFamily::V6: {
                char        buf[INET6_ADDRSTRLEN + 1];
                const char* result = ::inet_ntop (AF_INET6, &fV6_, buf, sizeof (buf));
                return result == nullptr ? String () : String::FromUTF8 (result);
            } break;
            default: {
                RequireNotReached ();
                return String ();
            } break;
        }
    }

}

bool InternetAddress::IsLocalhostAddress () const
{
    Require (not empty ());
    switch (fAddressFamily_) {
        case AddressFamily::V4: {
            // 127.0.0.x
            array<uint8_t, 4> octets = As<array<uint8_t, 4>> ();
            return octets[0] == 0x7f and octets[1] == 0x0 and octets[2] == 0x0;
        } break;
        case AddressFamily::V6: {
            return fV6_.s6_addr[0] == 0 and
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
                   fV6_.s6_addr[15] == 1;
        } break;
    }
    AssertNotReached ();
    return false;
}

bool InternetAddress::IsLinkLocalAddress () const
{
    Require (not empty ());
    switch (fAddressFamily_) {
        case AddressFamily::V4: {
            static constexpr InternetAddress kMinLinkLocal_{169, 254, 0, 1};
            static constexpr InternetAddress kMaxLinkLocal_{169, 254, 255, 254};
            Assert (kMinLinkLocal_ < kMaxLinkLocal_);
            return kMinLinkLocal_ <= *this and *this <= kMaxLinkLocal_;
        } break;
        case AddressFamily::V6: {
            return fV6_.s6_addr[0] == 0xfe and fV6_.s6_addr[1] == 0x80 and
                   fV6_.s6_addr[2] == 0x0 and fV6_.s6_addr[3] == 0x0 and
                   fV6_.s6_addr[4] == 0x0 and fV6_.s6_addr[5] == 0x0 and
                   fV6_.s6_addr[6] == 0x0 and fV6_.s6_addr[7] == 0x0;
        } break;
    }
    AssertNotReached ();
    return false;
}

bool InternetAddress::IsPrivateAddress () const
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
            array<uint8_t, 4> octets = As<array<uint8_t, 4>> ();
            if (octets[0] == 10) {
                return true;
            }
            if (octets[0] == 172 and (octets[1] & 0xf0) == 16) {
                return true;
            }
            if (octets[0] == 192 and octets[1] == 168) {
                return true;
            }
            return false;
        } break;
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
        } break;
    }
    AssertNotReached ();
    return false;
}

bool InternetAddress::IsMulticastAddress () const
{
    Require (not empty ());
    switch (fAddressFamily_) {
        case AddressFamily::V4: {
            // From http://en.wikipedia.org/wiki/Multicast_address :
            // The group includes the addresses from 224.0.0.0 to 239.255.255.255
            array<uint8_t, 4> octets = As<array<uint8_t, 4>> ();
            return 224 <= octets[0] and octets[0] <= 239;
        } break;
        case AddressFamily::V6: {
            return fV6_.s6_addr[0] == 0xff;
        } break;
    }
    AssertNotReached ();
    return false;
}

bool InternetAddress::Equals (const InternetAddress& rhs) const
{
    // @todo could optimize slightly since we don't care about order of element comparison
    return Compare (rhs) == 0;
}

optional<InternetAddress> InternetAddress::AsAddressFamily (AddressFamily family) const
{
    if (GetAddressFamily () == family) {
        return *this;
    }
    if (GetAddressFamily () == AddressFamily::V4 and family == AddressFamily::V6) {
        /*
         *   See  https://en.wikipedia.org/wiki/6to4
         */
        array<uint8_t, 4> octets = As<array<uint8_t, 4>> ();
        return InternetAddress{in6_addr{{{0x20, 0x02, octets[0], octets[1], octets[2], octets[3]}}}};
    }
    else if (GetAddressFamily () == AddressFamily::V6 and family == AddressFamily::V4) {
        /*
         *  See  https://en.wikipedia.org/wiki/6to4
         *
         *  Note: there are other mappings we should support
         */
        in6_addr tmp = As<in6_addr> ();
        if (tmp.s6_addr[0] == 0x20 and tmp.s6_addr[1] == 0x02) {
            return InternetAddress{tmp.s6_addr[2], tmp.s6_addr[3], tmp.s6_addr[4], tmp.s6_addr[5]};
        }
    }
    // @todo - other cases - can SOMETIMES be done!!!
    return nullopt;
}

String InternetAddress::ToString () const
{
    if (V4::kAddrAny == *this) {
        return L"INADDR_ANY"sv;
    }
    if (V6::kAddrAny == *this) {
        return L"in6addr_any"sv;
    }
    if (V4::kLocalhost == *this) {
        return L"localhost"sv;
    }
    if (V6::kLocalhost == *this) {
        return L"v6-localhost"sv; // no well-defined constant for this, but a good guess
    }
    if (V6::kV4MappedLocalhost == *this) {
        return L"v4-localhost-As-v6"sv; // no well-defined constant for this, but a good guess
    }
    return As<String> ();
}

int InternetAddress::Compare (const InternetAddress& rhs) const
{
    if (fAddressFamily_ != rhs.fAddressFamily_) {
        return fAddressFamily_ < rhs.fAddressFamily_ ? -1 : 1;
    }
    switch (fAddressFamily_) {
        case AddressFamily::UNKNOWN: {
            return 0;
        } break;
        case AddressFamily::V4: {
            // if not equal, compare by net/host before other things so we get sensible intuitive ordering
            if (memcmp (&fV4_, &rhs.fV4_, sizeof (fV4_)) == 0) {
                return 0;
            }
            IPv4AddressOctets lOctets = As<IPv4AddressOctets> ();
            IPv4AddressOctets rOctets = rhs.As<IPv4AddressOctets> ();
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
        } break;
        case AddressFamily::V6: {
            return memcmp (&fV6_, &rhs.fV6_, sizeof (fV6_));
        } break;
    }
    AssertNotReached ();
    return 0;
}

/*
 ********************************************************************************
 ***************** IO::Network::InternetAddresses_Any ***************************
 ********************************************************************************
 */
Traversal::Iterable<InternetAddress> Network::InternetAddresses_Any (InternetProtocol::IP::IPVersionSupport ipSupport)
{
    Containers::Collection<InternetAddress> result;
    if (InternetProtocol::IP::SupportIPV4 (ipSupport)) {
        result += V4::kAddrAny;
    }
    if (InternetProtocol::IP::SupportIPV6 (ipSupport)) {
        result += V6::kAddrAny;
    }
    return result;
}

/*
 ********************************************************************************
 ****************** IO::Network::InternetAddresses_Localhost ********************
 ********************************************************************************
 */
Traversal::Iterable<InternetAddress> Network::InternetAddresses_Localhost (InternetProtocol::IP::IPVersionSupport ipSupport)
{
    Containers::Collection<InternetAddress> result;
    if (InternetProtocol::IP::SupportIPV4 (ipSupport)) {
        result += V4::kLocalhost;
    }
    if (InternetProtocol::IP::SupportIPV6 (ipSupport)) {
        result += V6::kLocalhost;
    }
    return result;
}
