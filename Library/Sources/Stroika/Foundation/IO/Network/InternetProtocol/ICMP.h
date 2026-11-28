/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetProtocol_ICMP_h_
#define _Stroika_Foundation_IO_Network_InternetProtocol_ICMP_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdint>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Endian.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/Memory/Common.h"

/**
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 * @see https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
 *
 */

namespace Stroika::Foundation::IO::Network::InternetProtocol::ICMP {

    /**
     *  A destination unreachable message's (ICMP or ICMPv6) - GetCode () is its code, which means what its version's says:
     *  ICMP's (IPv4), or ICMPv6's (RFC 4443), as GetUnreachedIP () - who sent it - is IPv4 or IPv6.
     *
     *  @see https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol - Destination unreachable message
     */
    class DestinationUnreachableException : public Execution::Exception<runtime_error> {
    private:
        using inherited = Execution::Exception<runtime_error>;

    public:
        /**
         */
        DestinationUnreachableException (uint8_t code, const InternetAddress& unreachedIP);

    public:
        /**
         */
        nonvirtual uint8_t GetCode () const;

    public:
        nonvirtual InternetAddress GetUnreachedIP () const;

    private:
        uint8_t         fCode_;
        InternetAddress fUnreachedIP_;
    };

    /**
     *  The hop count was not large enough to reach the destination: a time exceeded message's (ICMP's TTL, ICMPv6's hop limit).
     *
     *  'UnreachedIP' is the address the packet reached when its hop count ran out - the router that sent the message.
     *
     *  @see https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
     */
    class TTLExpiredException : public Execution::Exception<runtime_error> {
    private:
        using inherited = Execution::Exception<runtime_error>;

    public:
        /**
         */
        TTLExpiredException (const InternetAddress& unreachedIP);

    public:
        nonvirtual InternetAddress GetUnreachedIP () const;

    private:
        InternetAddress fUnreachedIP_;
    };

    namespace V4 {

        /*
         *  ICMP packet types - from https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol "Control messages" 
         */
        enum class ICMP_PacketTypes : uint8_t {
            ICMP_ECHO_REPLY   = 0,
            ICMP_DEST_UNREACH = 3,
            ICMP_ECHO_REQUEST = 8,
            ICMP_TTL_EXPIRE   = 11,
        };
        using ICMP_PacketTypes::ICMP_DEST_UNREACH;
        using ICMP_PacketTypes::ICMP_ECHO_REPLY;
        using ICMP_PacketTypes::ICMP_ECHO_REQUEST;
        using ICMP_PacketTypes::ICMP_TTL_EXPIRE;

        /**
         * ICMP packet header (does not include IP header).
         *
         * @see https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
         */
        Stroika_Foundation_Common_STRUCT_PACKED (struct PacketHeader {
            ICMP_PacketTypes type; // ICMP packet type
            uint8_t          code; // Type sub code
            uint16_t         checksum;
            uint16_t         id;
            uint16_t         seq;
            uint32_t         timestamp; // not part of ICMP: the first 4 bytes of an echo request's data, which its reply echoes back
        });
        static_assert (sizeof (PacketHeader) == 12, "Check Stroika_Foundation_Common_STRUCT_PACKED: ICMP::PacketHeader size wrong");

        // Minimum ICMP packet size, in bytes
        constexpr size_t ICMP_MIN{8};

        /**
         */
        class UnknownICMPPacket : public Execution::Exception<runtime_error> {
        private:
            using inherited = Execution::Exception<runtime_error>;

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
    }

    /**
     *  ICMPv6 - IPv6's ICMP (RFC 4443). On a raw socket, a message arrives without the IPv6 header before it - unlike IPv4's - so
     *  its hop limit comes as ConnectionlessSocket::PacketInfo::fHopLimit; and the OS computes the checksum of one sent (it
     *  covers the source address).
     */
    namespace V6 {

        /*
         *  ICMPv6 message types - those an echo (ping) uses, and the errors it can get back (RFC 4443)
         */
        enum class ICMP_PacketTypes : uint8_t {
            ICMP_DEST_UNREACH = 1,
            ICMP_TTL_EXPIRE   = 3, // time exceeded: its hop limit ran out
            ICMP_ECHO_REQUEST = 128,
            ICMP_ECHO_REPLY   = 129,
        };
        using ICMP_PacketTypes::ICMP_DEST_UNREACH;
        using ICMP_PacketTypes::ICMP_ECHO_REPLY;
        using ICMP_PacketTypes::ICMP_ECHO_REQUEST;
        using ICMP_PacketTypes::ICMP_TTL_EXPIRE;

        /**
         *  An ICMPv6 echo request's or reply's header (RFC 4443, 4.1) - the message's first bytes, its data after it.
         */
        Stroika_Foundation_Common_STRUCT_PACKED (struct PacketHeader {
            ICMP_PacketTypes type;
            uint8_t          code;
            uint16_t         checksum;
            uint16_t         id;
            uint16_t         seq;
        });
        static_assert (sizeof (PacketHeader) == 8, "Check Stroika_Foundation_Common_STRUCT_PACKED: ICMP::V6::PacketHeader size wrong");

        // An ICMPv6 message's header size, in bytes - an error message's too: type, code, checksum, and 4 more bytes
        constexpr size_t ICMP_MIN{8};

        // The IPv6 header's size, in bytes (RFC 8200) - an error message quotes it, from the packet it answers
        constexpr size_t kIPHeaderSize{40};
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ICMP.inl"

#endif /*_Stroika_Foundation_IO_Network_InternetProtocol_ICMP_h_*/
