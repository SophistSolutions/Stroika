/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
                return String_Constant{L"ICMP Destination Unreachable: Network unreachable error."};
            case 1:
                return String_Constant{L"ICMP Destination Unreachable: Host unreachable error."};
            case 2:
                return String_Constant{L"ICMP Destination Unreachable: Protocol unreachable error (the designated transport protocol is not supported)."};
            case 3:
                return String_Constant{L"ICMP Destination Unreachable: Destination port unreachable."};
            case 4:
                return String_Constant{L"ICMP Destination Unreachable: Fragmentation required, and DF flag set."};
            case 5:
                return String_Constant{L"ICMP Destination Unreachable: Source route failed."};
            case 6:
                return String_Constant{L"ICMP Destination Unreachable: Destination network unknown."};
            case 7:
                return String_Constant{L"ICMP Destination Unreachable: Destination host unknown."};
            case 8:
                return String_Constant{L"ICMP Destination Unreachable: Source host isolated."};
            case 9:
                return String_Constant{L"ICMP Destination Unreachable: Network administratively prohibited."};
            case 10:
                return String_Constant{L"ICMP Destination Unreachable: Host administratively prohibited."};
            case 11:
                return String_Constant{L"ICMP Destination Unreachable: Network unreachable for ToS."};
            case 12:
                return String_Constant{L"ICMP Destination Unreachable: Host unreachable for ToS."};
            case 13:
                return String_Constant{L"ICMP Destination Unreachable: Communication administratively prohibited."};
            case 14:
                return String_Constant{L"ICMP Destination Unreachable: Host Precedence Violation."};
            case 15:
                return String_Constant{L"ICMP Destination Unreachable: Precedence cutoff in effect."};
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
