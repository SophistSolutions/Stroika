/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetProtocol_ICMP_h_
#define _Stroika_Foundation_IO_Network_InternetProtocol_ICMP_h_ 1

#include "../../../StroikaPreComp.h"

#include <cstdint>

#if qPlatform_Linux
#include <linux/ip.h>
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
                    namespace ICMP {

                        using Memory::Byte;

                        /**
                         * ICMP header
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
#ifndef ICMP_ECHO_REPLY
                        constexpr Byte ICMP_ECHO_REPLY{0};
#endif
#ifndef ICMP_DEST_UNREACH
                        constexpr Byte ICMP_DEST_UNREACH{3};
#endif
#ifndef ICMP_TTL_EXPIRE
                        constexpr Byte ICMP_TTL_EXPIRE{11};
#endif
                        // Minimum ICMP packet size, in bytes
                        constexpr size_t ICMP_MIN{8};
#ifndef ICMP_ECHO_REQUEST
                        constexpr Byte ICMP_ECHO_REQUEST{8};
#endif
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
