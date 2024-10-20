/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "ICMP.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::InternetProtocol;

namespace {
    String mkMessage_ (unsigned short code)
    {
        // from  @see https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol - Destination unreachable message
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
                return Characters::Format ("ICMP Destination Unreachable: code {}."_f, code);
        }
    }
}

/*
 ********************************************************************************
 ***** InternetProtocol::ICMP::V4::DestinationUnreachableException **************
 ********************************************************************************
 */
InternetProtocol::ICMP::V4::DestinationUnreachableException::DestinationUnreachableException (uint8_t code, const InternetAddress& unreachedIP)
    : inherited{mkMessage_ (code)}
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
    : inherited{Characters::Format ("ICMP Unknown packet type: {}."_f, type)}
    , fType_{type}
{
}

/*
 ********************************************************************************
 ************* InternetProtocol::ICMP::V4::TTLExpiredException ******************
 ********************************************************************************
 */
InternetProtocol::ICMP::V4::TTLExpiredException::TTLExpiredException (const InternetAddress& unreachedIP)
    : inherited{"ICMP TTL Expired."sv}
    , fUnreachedIP_{unreachedIP}
{
}
