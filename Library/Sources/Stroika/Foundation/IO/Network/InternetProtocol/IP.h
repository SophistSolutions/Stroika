/*
* Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
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
*  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
*
*
* TODO:
*
*
*/

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {
                namespace InternetProtocol {
                    namespace IP {

                        using Memory::Byte;

/*
                     * The IP header
                     *      @see https://en.wikipedia.org/w/index.php?title=IPv4
                     */
#if qPlatform_Linux
                        using iphdr = ::iphdr;
#else
                        Stroika_Foundation_Configuration_STRUCT_PACKED (struct iphdr_le_ {
                            Byte ihl : 4,       // Length of the header in dwords
                                version : 4;    // Version of IP
                            Byte     tos;       // Type of service
                            uint16_t total_len; // Length of the packet in dwords
                            uint16_t ident;     // unique identifier
                            uint16_t flags;     // Flags
                            Byte     ttl;       // Time to live
                            Byte     protocol;  // Protocol number (TCP, UDP etc)
                            uint16_t checksum;  // IP checksum
                            uint32_t source_ip;
                            uint32_t dest_ip;
                        });
                        Stroika_Foundation_Configuration_STRUCT_PACKED (struct iphdr_be_ {
                            Byte version : 4,   // Version of IP
                                ihl : 4;        // Length of the header in dwords
                            Byte     tos;       // Type of service
                            uint16_t total_len; // Length of the packet in dwords
                            uint16_t ident;     // unique identifier
                            uint16_t flags;     // Flags
                            Byte     ttl;       // Time to live
                            Byte     protocol;  // Protocol number (TCP, UDP etc)
                            uint16_t checksum;  // IP checksum
                            uint32_t source_ip;
                            uint32_t dest_ip;
                        });
                        using iphdr = conditional<Configuration::GetEndianness () == Configuration::Endian::eBig, iphdr_be_, iphdr_le_>::type;
#endif
                        static_assert (sizeof (iphdr) == 20, "Check Stroika_Foundation_Configuration_STRUCT_PACKED, or builtin definition of iphdr: iphdr size wrong");

                        /**
                     */
                        uint16_t ip_checksum (const void* packetStart, const void* packetEnd);
                    }
                }
            }
        }
    }
}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "IP.inl"

#endif /*_Stroika_Foundation_IO_Network_InternetProtocol_IP_h_*/
