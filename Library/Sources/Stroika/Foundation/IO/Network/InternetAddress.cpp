/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/String_Constant.h"
#include     "../../Execution/ErrNoException.h"

#include    "InternetAddress.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;



#define     qSupportPTONAndPTON_ (qPlatform_POSIX || (qPlatformWindows && (NTDDI_VERSION >= NTDDI_VISTA)))

#if 0
namespace {
    constexpr   in_addr     kV4AddrAny_ =   { 0 };
    constexpr   in6_addr    kV6AddrAny_ =   { { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } } };
}


const   InternetAddress V4::kAddrAny    =   InternetAddress (kV4AddrAny_);
const   InternetAddress V6::kAddrAny    =   InternetAddress (kV6AddrAny_);
#endif


namespace {
    inline  const   in_addr kV4Localhost_ ()
    {
        // @todo - check if this localhost is right? May have byte order backwards - net or host byteorder???
        in_addr p;
        p.s_addr = INADDR_LOOPBACK;
        return p;
    }
    constexpr   in6_addr    kV6Localhost_   =   { { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } } };
}
const   InternetAddress V4::kLocalhost  =   InternetAddress (kV4Localhost_ ());
const   InternetAddress V6::kLocalhost  =   InternetAddress (kV6Localhost_);





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
#if     qSupportPTONAndPTON_
                    Execution::ThrowErrNoIfNegative (inet_pton (AF_INET, s.c_str (), &fV4_));
#elif   qPlatform_Windows
                    DISABLE_COMPILER_MSC_WARNING_START(4996)    // msft doesnt have this on old platforms but still warns!
                    fV4_.s_addr = ::inet_addr (s.c_str ());
                    DISABLE_COMPILER_MSC_WARNING_END(4996)
#else
                    AssertNotImplemented ();
#endif
                    fAddressFamily_ = af;
                }
                break;
            case AddressFamily::V6: {
#if     qSupportPTONAndPTON_
                    Execution::ThrowErrNoIfNegative (inet_pton (AF_INET6, s.c_str (), &fV6_));
#else
                    AssertNotImplemented ();
#endif
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
#if     qSupportPTONAndPTON_
                                char    buf[INET_ADDRSTRLEN + 1];
                                const char*   result  =   ::inet_ntop (AF_INET, &fV4_, buf, sizeof (buf));
                                return result == nullptr ? String () : String::FromUTF8 (result);
#else
                                DISABLE_COMPILER_MSC_WARNING_START(4996)    // msft doesnt have this on old platforms but still warns!
                                return String::FromUTF8 (::inet_ntoa (fV4_));
                                DISABLE_COMPILER_MSC_WARNING_END(4996)
#endif
                            }
                            break;
                        case AddressFamily::V6: {
#if     qSupportPTONAndPTON_
                                char    buf[INET6_ADDRSTRLEN + 1];
                                const char*   result  =   ::inet_ntop (AF_INET6, &fV6_, buf, sizeof (buf));
                                return result == nullptr ? String () : String::FromUTF8 (result);
#else
                                AssertNotImplemented ();
                                return String ();
#endif
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
                return (fV4_.s_addr & 0xff000000) == 0x7f000000;
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
                    fV6_.s6_addr[14] == 1 and
                    fV6_.s6_addr[15] == 0
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
                uint8_t net = (fV4_.s_addr >> 24);
                uint8_t host = ((fV4_.s_addr >> 16) & 0xff);
                if (net == 10) {
#if     defined (s_net)
                    Assert (ipv4Checker (fV4_));
#endif
                    return true;
                }
                else if (net == 172 and (16 <= host and host == 31)) {
#if     defined (s_net)
                    Assert (ipv4Checker (fV4_));
#endif
                    return true;
                }
                else if (net == 192 and host == 168) {
#if     defined (s_net)
                    Assert (ipv4Checker (fV4_));
#endif
                    return true;
                }
#if     defined (s_net)
                Assert (not ipv4Checker (fV4_));
#endif
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
                bool result = fV6_.s6_addr[0] == 0xfc and fV6_.s6_addr[1] == 0x0;
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
                return (fV4_.s_addr & 0xf0000000) == 0xe0000000;
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
        case AddressFamily::V4: {
                return memcmp (&fV4_, &rhs.fV4_, sizeof (fV4_));
            }
            break;
        case AddressFamily::V6: {
                return memcmp (&fV6_, &fV6_, sizeof (fV6_));
            }
            break;
    }
    AssertNotReached ();
    return false;
}
