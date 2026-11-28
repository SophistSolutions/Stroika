/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/IO/Network/ConnectionlessSocket.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"

#include "IP.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::InternetProtocol;

/*
 ********************************************************************************
 ********** IO::Network::InternetProtocol::IP::GetHostIPVersionSupport **********
 ********************************************************************************
 */
IP::IPVersionSupport InternetProtocol::IP::GetHostIPVersionSupport ()
{
    // only the OS saying it has no such address family counts as no - a socket failing for another reason (too many open
    // files, say) leaves it to whatever uses that family to meet that failure, and report it
    auto makesSockets = [] (SocketAddress::FamilyType family) {
        try {
            ConnectionlessSocket::New (family, Socket::DGRAM);
            return true;
        }
        catch (const system_error& e) {
            return not Execution::IsA (e, errc::address_family_not_supported);
        }
    };
    static const IPVersionSupport sHostSupport_ = [&] () {
        bool v4 = makesSockets (SocketAddress::INET);
        bool v6 = makesSockets (SocketAddress::INET6);
        if (v4 != v6) {
            return v4 ? IPVersionSupport::eIPV4Only : IPVersionSupport::eIPV6Only;
        }
        return IPVersionSupport::eIPV4AndIPV6;
    }();
    return sHostSupport_;
}

/*
 ********************************************************************************
 ***************** IO::Network::InternetProtocol::ip_checksum *******************
 ********************************************************************************
 */
uint16_t InternetProtocol::IP::ip_checksum (const void* packetStart, const void* packetEnd)
{
    const uint16_t* buffer = reinterpret_cast<const uint16_t*> (packetStart);
    ptrdiff_t       size   = reinterpret_cast<const uint8_t*> (packetEnd) - reinterpret_cast<const uint8_t*> (packetStart);
    unsigned long   cksum  = 0;

    // Sum all the words together, adding the final byte if size is odd
    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof (uint16_t);
    }
    if (size) {
        cksum += *reinterpret_cast<const uint8_t*> (buffer);
    }

    // Do a little shuffling
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);

    // Return the bitwise complement of the resulting mishmash
    return (uint16_t)(~cksum);
}
