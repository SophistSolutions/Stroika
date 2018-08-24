/*
* Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
*/
#ifndef _Stroika_Foundation_IO_Network_InternetProtocol_IP_h_
#define _Stroika_Foundation_IO_Network_InternetProtocol_IP_h_ 1

#include "../../../StroikaPreComp.h"

#if qPlatform_Linux
#include <netinet/ip.h>
#endif

#include "../../../Configuration/Common.h"
#include "../../../Configuration/Endian.h"
#include "../../../Memory/Common.h"

/**
*
*  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
*
*
* TODO:
*
*      @see https://en.wikipedia.org/w/index.php?title=IPv4
*
*/

namespace Stroika::Foundation::IO::Network::InternetProtocol::IP {

    /**
     * This type is frequently used to select what versions of IP protocol to use in higher level services.
     *
     *  @see SupportIPV4
     *  @see SupportIPV6
     */
    enum class IPVersionSupport {
        eIPV4Only,
        eIPV6Only,
        eIPV4AndIPV6,

        eDEFAULT = eIPV4AndIPV6,

        Stroika_Define_Enum_Bounds (eIPV4Only, eIPV4AndIPV6)
    };

    /**
     *  Trivial helper so you can do:
     *
     *  \par Example Usage
     *      \code
     *          if (InternetProtocol::IP::SupportIPV4 (ipVersion)) {
     *              fSocket_.Bind (SocketAddress (Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()), bindFlags);
     *          }
     *      \endcode
     */
    bool SupportIPV4 (IPVersionSupport flag);

    /**
     *  Trivial helper so you can do:
     *
     *  \par Example Usage
     *      \code
     *          if (InternetProtocol::IP::SupportIPV6 (ipVersion)) {
     *              fSocket_.Bind (SocketAddress (Network::V6::kAddrAny, UPnP::SSDP::V6::kSocketAddress.GetPort ()), bindFlags);
     *          }
     *      \endcode
     */
    bool SupportIPV6 (IPVersionSupport flag);

    namespace V4 {

        /**
     * The IPv4 packet header
     *
     * @see https://tools.ietf.org/html/rfc760
     *
     * copied field names to match http://lxr.free-electrons.com/source/include/uapi/linux/ip.h
     */
#if qPlatform_Linux
        using PacketHeader = ::iphdr;
#else
        Stroika_Foundation_Configuration_STRUCT_PACKED (struct iphdr_le_ {
            uint8_t ihl : 4,   // Length of the header in dwords
                version : 4;   // Version of IP
            uint8_t  tos;      // Type of service
            uint16_t tot_len;  // Length of the packet in dwords
            uint16_t id;       // unique identifier
            uint16_t frag_off; // Flags and Frament Offset
            uint8_t  ttl;      // Time to live
            uint8_t  protocol; // Protocol number (TCP, UDP etc)
            uint16_t check;    // IP checksum
            uint32_t saddr;
            uint32_t daddr;
        });
        Stroika_Foundation_Configuration_STRUCT_PACKED (struct iphdr_be_ {
            uint8_t version : 4, // Version of IP
                ihl : 4;         // Length of the header in dwords
            uint8_t  tos;        // Type of service
            uint16_t tot_len;    // Length of the packet in dwords
            uint16_t id;         // unique identifier
            uint16_t frag_off;   // Flags and Frament Offset
            uint8_t  ttl;        // Time to live
            uint8_t  protocol;   // Protocol number (TCP, UDP etc)
            uint16_t check;      // IP checksum
            uint32_t saddr;
            uint32_t daddr;
        });
        using PacketHeader = conditional_t<Configuration::GetEndianness () == Configuration::Endian::eBig, iphdr_be_, iphdr_le_>;
#endif
        static_assert (sizeof (PacketHeader) == 20, "Check Stroika_Foundation_Configuration_STRUCT_PACKED, or builtin definition of iphdr: iphdr size wrong");
    }

    /**
     */
    uint16_t ip_checksum (const void* packetStart, const void* packetEnd);

}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "IP.inl"

#endif /*_Stroika_Foundation_IO_Network_InternetProtocol_IP_h_*/
