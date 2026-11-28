/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"

#include "ICMP.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::InternetProtocol;

namespace {
    // what a destination unreachable message's code means: ICMP's (IPv4)
    // @see https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol - Destination unreachable message
    String mkMessageV4_ (unsigned int code)
    {
        switch (code) {
            case 0:
                return "ICMP Destination Unreachable: Network unreachable error."sv;
            case 1:
                return "ICMP Destination Unreachable: Host unreachable error."sv;
            case 2:
                return "ICMP Destination Unreachable: Protocol unreachable error (the designated transport protocol is not supported)."sv;
            case 3:
                return "ICMP Destination Unreachable: Destination port unreachable."sv;
            case 4:
                return "ICMP Destination Unreachable: Fragmentation required, and DF flag set."sv;
            case 5:
                return "ICMP Destination Unreachable: Source route failed."sv;
            case 6:
                return "ICMP Destination Unreachable: Destination network unknown."sv;
            case 7:
                return "ICMP Destination Unreachable: Destination host unknown."sv;
            case 8:
                return "ICMP Destination Unreachable: Source host isolated."sv;
            case 9:
                return "ICMP Destination Unreachable: Network administratively prohibited."sv;
            case 10:
                return "ICMP Destination Unreachable: Host administratively prohibited."sv;
            case 11:
                return "ICMP Destination Unreachable: Network unreachable for ToS."sv;
            case 12:
                return "ICMP Destination Unreachable: Host unreachable for ToS."sv;
            case 13:
                return "ICMP Destination Unreachable: Communication administratively prohibited."sv;
            case 14:
                return "ICMP Destination Unreachable: Host Precedence Violation."sv;
            case 15:
                return "ICMP Destination Unreachable: Precedence cutoff in effect."sv;
            default:
                return "ICMP Destination Unreachable: code {}."_f(code);
        }
    }
    // ... and ICMPv6's (RFC 4443, 3.1)
    String mkMessageV6_ (unsigned int code)
    {
        switch (code) {
            case 0:
                return "ICMPv6 Destination Unreachable: No route to destination."sv;
            case 1:
                return "ICMPv6 Destination Unreachable: Communication with destination administratively prohibited."sv;
            case 2:
                return "ICMPv6 Destination Unreachable: Beyond scope of source address."sv;
            case 3:
                return "ICMPv6 Destination Unreachable: Address unreachable."sv;
            case 4:
                return "ICMPv6 Destination Unreachable: Port unreachable."sv;
            case 5:
                return "ICMPv6 Destination Unreachable: Source address failed ingress/egress policy."sv;
            case 6:
                return "ICMPv6 Destination Unreachable: Reject route to destination."sv;
            default:
                return "ICMPv6 Destination Unreachable: code {}."_f(code);
        }
    }
    bool IsV6_ (const InternetAddress& a)
    {
        return a.GetAddressFamily () == InternetAddress::AddressFamily::V6;
    }
}

/*
 ********************************************************************************
 ******** InternetProtocol::ICMP::DestinationUnreachableException ***************
 ********************************************************************************
 */
InternetProtocol::ICMP::DestinationUnreachableException::DestinationUnreachableException (uint8_t code, const InternetAddress& unreachedIP)
    : inherited{IsV6_ (unreachedIP) ? mkMessageV6_ (code) : mkMessageV4_ (code)}
    , fCode_{code}
    , fUnreachedIP_{unreachedIP}
{
}

/*
 ********************************************************************************
 **************** InternetProtocol::ICMP::V4::UnknownICMPPacket *****************
 ********************************************************************************
 */
InternetProtocol::ICMP::V4::UnknownICMPPacket::UnknownICMPPacket (ICMP_PacketTypes type)
    : inherited{"ICMP Unknown packet type: {}."_f(type)}
    , fType_{type}
{
}

/*
 ********************************************************************************
 *************** InternetProtocol::ICMP::TTLExpiredException ********************
 ********************************************************************************
 */
InternetProtocol::ICMP::TTLExpiredException::TTLExpiredException (const InternetAddress& unreachedIP)
    : inherited{IsV6_ (unreachedIP) ? "ICMPv6 Hop Limit Exceeded."sv : "ICMP TTL Expired."sv}
    , fUnreachedIP_{unreachedIP}
{
}
