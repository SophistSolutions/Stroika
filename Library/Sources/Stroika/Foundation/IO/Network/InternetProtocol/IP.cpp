/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/String_Constant.h"
#include "../../../Streams/TextReader.h"
#include "../HTTP/Headers.h"

#include "IP.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::InternetProtocol;

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
    return (uint16_t) (~cksum);
}
