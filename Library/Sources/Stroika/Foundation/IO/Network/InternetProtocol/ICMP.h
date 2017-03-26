/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetProtocol_ICMP_h_
#define _Stroika_Foundation_IO_Network_InternetProtocol_ICMP_h_ 1

#include "../../../StroikaPreComp.h"

#include <cstdint>

#include "../../../Configuration/Common.h"
#include "../../../Configuration/Endian.h"
#include "../../../Execution/StringException.h"
#include "../../../Memory/Common.h"

#include "../InternetAddress.h"

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

                        namespace V4 {

                            /*
                         *  ICMP packet types - from https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol "Control messages" 
                         */
                            enum class ICMP_PacketTypes : Byte {
                                ICMP_ECHO_REPLY   = 0,
                                ICMP_DEST_UNREACH = 3,
                                ICMP_ECHO_REQUEST = 8,
                                ICMP_TTL_EXPIRE   = 11,
                            };
                            static constexpr ICMP_PacketTypes ICMP_ECHO_REPLY   = ICMP_PacketTypes::ICMP_ECHO_REPLY;
                            static constexpr ICMP_PacketTypes ICMP_DEST_UNREACH = ICMP_PacketTypes::ICMP_DEST_UNREACH;
                            static constexpr ICMP_PacketTypes ICMP_ECHO_REQUEST = ICMP_PacketTypes::ICMP_ECHO_REQUEST;
                            static constexpr ICMP_PacketTypes ICMP_TTL_EXPIRE   = ICMP_PacketTypes::ICMP_TTL_EXPIRE;

                            /**
                         * ICMP packet header (does not include IP header).
                         *
                         * @see https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
                         */
                            Stroika_Foundation_Configuration_STRUCT_PACKED (struct PacketHeader {
                                ICMP_PacketTypes type; // ICMP packet type
                                Byte             code; // Type sub code
                                uint16_t         checksum;
                                uint16_t         id;
                                uint16_t         seq;
                                uint32_t         timestamp; // not part of ICMP, but we need it
                            });
                            static_assert (sizeof (PacketHeader) == 12, "Check Stroika_Foundation_Configuration_STRUCT_PACKED: ICMP::PacketHeader size wrong");

                            // Minimum ICMP packet size, in bytes
                            constexpr size_t ICMP_MIN{8};

                            /**
                         * @see https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol - Destination unreachable message
                         */
                            class DestinationUnreachableException : public Execution::StringException {
                            private:
                                using inherited = Execution::StringException;

                            public:
                                /**
                             */
                                DestinationUnreachableException (unsigned short code, const InternetAddress& reachedIP);

                            public:
                                /**
                             */
                                nonvirtual unsigned int GetCode () const;

                            public:
                                nonvirtual InternetAddress GetReachedIP () const;

                            private:
                                unsigned int    fCode_;
                                InternetAddress fReachedIP_;
                            };

                            /**
                         */
                            class UnknownICMPPacket : public Execution::StringException {
                            private:
                                using inherited = Execution::StringException;

                            public:
                                /**
                             */
                                UnknownICMPPacket (ICMP_PacketTypes type);

                            public:
                                /**
                             */
                                nonvirtual ICMP_PacketTypes GetType () const;

                            private:
                                ICMP_PacketTypes fType_;
                            };

                            /**
                         *  @see https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol - means hop count (TTL) not large
                         *  enough to reach destination
                         *
                         *  'ReachedIP' is the address reached by the packet when the TTL expired.
                         */
                            class TTLExpiredException : public Execution::StringException {
                            private:
                                using inherited = Execution::StringException;

                            public:
                                /**
                             */
                                TTLExpiredException (const InternetAddress& reachedIP);

                            public:
                                nonvirtual InternetAddress GetReachedIP () const;

                            private:
                                InternetAddress fReachedIP_;
                            };
                        }
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
