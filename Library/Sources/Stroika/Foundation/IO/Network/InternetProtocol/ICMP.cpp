/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/Format.h"
#include "../../../Characters/String_Constant.h"
#include "../../../Streams/TextReader.h"
#include "../HTTP/Headers.h"

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
                return L"ICMP Destination Unreachable: Network unreachable error."_k;
            case 1:
                return L"ICMP Destination Unreachable: Host unreachable error."_k;
            case 2:
                return L"ICMP Destination Unreachable: Protocol unreachable error (the designated transport protocol is not supported)."_k;
            case 3:
                return L"ICMP Destination Unreachable: Destination port unreachable."_k;
            case 4:
                return L"ICMP Destination Unreachable: Fragmentation required, and DF flag set."_k;
            case 5:
                return L"ICMP Destination Unreachable: Source route failed."_k;
            case 6:
                return L"ICMP Destination Unreachable: Destination network unknown."_k;
            case 7:
                return L"ICMP Destination Unreachable: Destination host unknown."_k;
            case 8:
                return L"ICMP Destination Unreachable: Source host isolated."_k;
            case 9:
                return L"ICMP Destination Unreachable: Network administratively prohibited."_k;
            case 10:
                return L"ICMP Destination Unreachable: Host administratively prohibited."_k;
            case 11:
                return L"ICMP Destination Unreachable: Network unreachable for ToS."_k;
            case 12:
                return L"ICMP Destination Unreachable: Host unreachable for ToS."_k;
            case 13:
                return L"ICMP Destination Unreachable: Communication administratively prohibited."_k;
            case 14:
                return L"ICMP Destination Unreachable: Host Precedence Violation."_k;
            case 15:
                return L"ICMP Destination Unreachable: Precedence cutoff in effect."_k;
            default:
                return Characters::Format (L"ICMP Destination Unreachable: code %d.", code);
        }
    }
}

/*
 ********************************************************************************
 ***** InternetProtocol::ICMP::V4::DestinationUnreachableException **************
 ********************************************************************************
 */
InternetProtocol::ICMP::V4::DestinationUnreachableException::DestinationUnreachableException (uint8_t code, const InternetAddress& reachedIP)
    : inherited (mkMessage_ (code))
    , fCode_ (code)
    , fReachedIP_ (reachedIP)
{
}

/*
 ********************************************************************************
 **************** InternetProtocol::ICMP::V4::UnknownICMPPacket *****************
 ********************************************************************************
 */
InternetProtocol::ICMP::V4::UnknownICMPPacket::UnknownICMPPacket (ICMP_PacketTypes type)
    : inherited (Characters::Format (L"ICMP Unknown packet type: %d.", type))
    , fType_ (type)
{
}

/*
 ********************************************************************************
 ************* InternetProtocol::ICMP::V4::TTLExpiredException ******************
 ********************************************************************************
 */
InternetProtocol::ICMP::V4::TTLExpiredException::TTLExpiredException (const InternetAddress& reachedIP)
    : inherited (L"ICMP TTL Expired.")
    , fReachedIP_ (reachedIP)
{
}
