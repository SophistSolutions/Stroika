/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <random>

#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Execution/TimeOutException.h"
#include "../../Foundation/IO/Network/Socket.h"
#include "../../Foundation/IO/Network/SocketAddress.h"
#include "../../Foundation/Traversal/DiscreteRange.h"

#include "Ping.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::NetworkMontior;

using Memory::Byte;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********************** NetworkMontior::PingOptions *****************************
 ********************************************************************************
 */
constexpr Traversal::Range<size_t> PingOptions::kAllowedICMPPayloadSizeRange;

String NetworkMontior::PingOptions::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    if (fMaxHops) {
        sb += L"Max-Hops: " + Characters::Format (L"%d", *fMaxHops);
    }
    if (fPacketPayloadSize) {
        sb += L"Packet-Payload-Size: " + Characters::Format (L"%d", *fPacketPayloadSize);
    }
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ***************************** NetworkMontior::Ping *****************************
 ********************************************************************************
 */
namespace {
/*
 * The IP header
 *      @see https://en.wikipedia.org/w/index.php?title=IPv4
 */
#if !qPlatform_Linux
    struct iphdr {
#if defined(__LITTLE_ENDIAN_BITFIELD) or qPlatform_Windows
        alignas (1) Byte ihl : 4, // Length of the header in dwords
            version : 4;          // Version of IP
#elif defined(__BIG_ENDIAN_BITFIELD)
        alignas (1) Byte version : 4,
            ihl : 4;
#else
#error "Please fix <asm/byteorder.h>"
#endif
        alignas (1) Byte tos;           // Type of service
        alignas (2) uint16_t total_len; // Length of the packet in dwords
        alignas (2) uint16_t ident;     // unique identifier
        alignas (2) uint16_t flags;     // Flags
        alignas (1) Byte ttl;           // Time to live
        alignas (1) Byte proto;         // Protocol number (TCP, UDP etc)
        alignas (2) uint16_t checksum;  // IP checksum
        alignas (4) uint32_t source_ip;
        alignas (4) uint32_t dest_ip;
    };
#endif
    static_assert (sizeof (iphdr) == 20);

    // ICMP header
    struct ICMPHeader {
        alignas (1) Byte type; // ICMP packet type
        alignas (1) Byte code; // Type sub code
        alignas (2) uint16_t checksum;
        alignas (2) uint16_t id;
        alignas (2) uint16_t seq;
        alignas (4) uint32_t timestamp; // not part of ICMP, but we need it
    };
    static_assert (sizeof (ICMPHeader) == 12);
    static_assert (sizeof (ICMPHeader) == kICMPPacketHeaderSize);

// ICMP packet types
#define ICMP_ECHO_REPLY 0
#define ICMP_DEST_UNREACH 3
#define ICMP_TTL_EXPIRE 11
#define ICMP_ECHO_REQUEST 8

    uint16_t ip_checksum (uint16_t* buffer, int size)
    {
        unsigned long cksum = 0;

        // Sum all the words together, adding the final byte if size is odd
        while (size > 1) {
            cksum += *buffer++;
            size -= sizeof (uint16_t);
        }
        if (size) {
            cksum += *(Byte*)buffer;
        }

        // Do a little shuffling
        cksum = (cksum >> 16) + (cksum & 0xffff);
        cksum += (cksum >> 16);

        // Return the bitwise complement of the resulting mishmash
        return (uint16_t) (~cksum);
    }
}

namespace {
    // Minimum ICMP packet size, in bytes
    constexpr size_t       ICMP_MIN{8};
    constexpr unsigned int DEFAULT_TTL        = 30;
    constexpr size_t       MAX_PING_DATA_SIZE = 1024;
    constexpr size_t       MAX_PING_PACKET_SIZE{MAX_PING_DATA_SIZE + sizeof (iphdr)};
#define ICMP_ECHO_REQUEST 8
}

Duration NetworkMontior::Ping (const InternetAddress& addr, const PingOptions& options)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Frameworks::NetworkMontior::Ping", L"addr=%s, options=%s", Characters::ToString (addr).c_str (), Characters::ToString (options).c_str ())};
    // file:///C:/Sandbox/Stroika/DevRoot/Winsock%20Programmer%E2%80%99s%20FAQ_%20Ping_%20Raw%20Sockets%20Method.html
    size_t icmpPacketSize = PingOptions::kAllowedICMPPayloadSizeRange.Pin (options.fPacketPayloadSize.Value (PingOptions::kDefaulPayloadSize)) + sizeof (ICMPHeader);
    int    ttl            = DEFAULT_TTL;

    static std::mt19937 rng{std::random_device () ()};
    ICMPHeader          pingRequest = [&]() {
        static std::uniform_int_distribution<std::mt19937::result_type> distribution (0, numeric_limits<uint16_t>::max ());
        static uint16_t                                                 seq_no = distribution (rng);
        ICMPHeader                                                      tmp{};
        tmp.type      = ICMP_ECHO_REQUEST;
        tmp.id        = distribution (rng);
        tmp.seq       = seq_no++;
        tmp.timestamp = static_cast<uint32_t> (Time::GetTickCount () * 1000);
        return tmp;
    }();
    SmallStackBuffer<Byte> sendPacket (icmpPacketSize);
    memcpy (sendPacket.begin (), &pingRequest, sizeof (pingRequest));
    // use random data as a payload
    static std::uniform_int_distribution<std::mt19937::result_type> distByte (0, numeric_limits<Byte>::max ());
    for (Byte* p = (Byte*)sendPacket.begin () + sizeof (ICMPHeader); p < sendPacket.end (); ++p) {
        static std::uniform_int_distribution<std::mt19937::result_type> distribution (0, numeric_limits<Byte>::max ());
        *p = distribution (rng);
    }
    reinterpret_cast<ICMPHeader*> (sendPacket.begin ())->checksum = ip_checksum ((uint16_t*)sendPacket.begin (), icmpPacketSize);

    Socket s{Socket::ProtocolFamily::INET, Socket::SocketKind::RAW, IPPROTO_ICMP};
    s.setsockopt (IPPROTO_IP, IP_TTL, ttl);

    s.SendTo (sendPacket.begin (), sendPacket.end (), SocketAddress{addr, 0});

    while (true) {
        SocketAddress fromAddress;

        SmallStackBuffer<Byte> recv_buf (icmpPacketSize + sizeof (iphdr));
        size_t                 n = s.ReceiveFrom (begin (recv_buf), end (recv_buf), 0, &fromAddress);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"got back packet from %s", Characters::ToString (fromAddress).c_str ());
#endif
        iphdr* reply = reinterpret_cast<iphdr*> (recv_buf.begin ());

        {
            // Skip ahead to the ICMP header within the IP packet
            unsigned short header_len = reply->ihl * 4;
            ICMPHeader*    icmphdr    = (ICMPHeader*)((char*)reply + header_len);

            // Make sure the reply is sane
            if (n < header_len + ICMP_MIN) {
                Execution::Throw (Execution::StringException (L"too few bytes from " + Characters::ToString (fromAddress))); // draft @todo fix
            }
            else if (icmphdr->type != ICMP_ECHO_REPLY) {
                if (icmphdr->type != ICMP_TTL_EXPIRE) {
                    if (icmphdr->type == ICMP_DEST_UNREACH) {
                        Execution::Throw (Execution::StringException (L"Destination unreachable")); // draft @todo fix
                    }
                    else {
                        Execution::Throw (Execution::StringException (L"Unknown ICMP packet type")); // draft @todo fix - int (icmphdr->type)
                    }
                }
                // If "TTL expired", fall through.  Next test will fail if we
                // try it, so we need a way past it.
            }
            else if (icmphdr->id != pingRequest.id) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"icmphdr->id != pingRequest.id so ignoring this reply");
#endif
                // Must be a reply for another pinger running locally, so just
                // ignore it.
                //              return -2;
            }

            // Figure out how far the packet travelled
            int nHops = int(256 - reply->ttl);
            if (nHops == 192) {
                // TTL came back 64, so ping was probably to a host on the
                // LAN -- call it a single hop.
                nHops = 1;
            }
            else if (nHops == 128) {
                // Probably localhost
                nHops = 0;
            }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"nHops = %d", nHops);
#endif

            if (icmphdr->type == ICMP_TTL_EXPIRE) {
                Execution::Throw (Execution::StringException (L"TTL expired")); // draft @todo fix
            }
            return Duration ((Time::GetTickCount () * 1000 - icmphdr->timestamp) / 1000);
        }
    }
    Execution::Throw (Execution::TimeOutException (L"No response"));
}
