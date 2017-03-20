/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetProtocol_ICMP_h_
#define _Stroika_Foundation_IO_Network_InternetProtocol_ICMP_h_ 1

#include "../../../StroikaPreComp.h"

#include <cstdint>

#include "../../../Configuration/Common.h"
#include "../../../Configuration/Endian.h"
#include "../../../Memory/Common.h"

/**
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *
 * @see https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
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
                    namespace ICMP {

                        using Memory::Byte;

                        /**
                         * ICMP packet header (does not include IP header).
                         *
                         * @see https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
                         */
                        Stroika_Foundation_Configuration_STRUCT_PACKED (struct PacketHeader {
                            Byte     type; // ICMP packet type
                            Byte     code; // Type sub code
                            uint16_t checksum;
                            uint16_t id;
                            uint16_t seq;
                            uint32_t timestamp; // not part of ICMP, but we need it
                        });
                        static_assert (sizeof (PacketHeader) == 12, "Check Stroika_Foundation_Configuration_STRUCT_PACKED: ICMP::PacketHeader size wrong");

                        // ICMP packet types
                        constexpr Byte ICMP_ECHO_REPLY{0};
                        constexpr Byte ICMP_DEST_UNREACH{3};
                        constexpr Byte ICMP_ECHO_REQUEST{8};
                        constexpr Byte ICMP_TTL_EXPIRE{11};

                        // Minimum ICMP packet size, in bytes
                        constexpr size_t ICMP_MIN{8};
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
#include "ICMP.inl"

#endif /*_Stroika_Foundation_IO_Network_InternetProtocol_ICMP_h_*/
