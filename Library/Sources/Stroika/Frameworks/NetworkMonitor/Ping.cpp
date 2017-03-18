/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

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
using namespace Stroika::Foundation::Traversal;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::NetworkMontior;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ***************************** NetworkMontior::Ping *****************************
 ********************************************************************************
 */
namespace {
    using Memory::Byte;

    // The IP header
    struct IPHeader {
        alignas (1) Byte h_len : 4;     // Length of the header in dwords
        alignas (1) Byte version : 4;   // Version of IP
        alignas (1) Byte tos;           // Type of service
        alignas (1) uint16_t total_len; // Length of the packet in dwords
        alignas (1) uint16_t ident;     // unique identifier
        alignas (1) uint16_t flags;     // Flags
        alignas (1) Byte ttl;           // Time to live
        alignas (1) Byte proto;         // Protocol number (TCP, UDP etc)
        alignas (1) uint16_t checksum;  // IP checksum
        alignas (1) uint32_t source_ip;
        alignas (1) uint32_t dest_ip;
    };
    static_assert (sizeof (IPHeader) == 20);

    // ICMP header
    struct ICMPHeader {
        alignas (1) Byte type; // ICMP packet type
        alignas (1) Byte code; // Type sub code
        alignas (1) uint16_t checksum;
        alignas (1) uint16_t id;
        alignas (1) uint16_t seq;
        alignas (1) uint32_t timestamp; // not part of ICMP, but we need it
    };
    static_assert (sizeof (ICMPHeader) == 12);

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
#define ICMP_MIN 8
    constexpr size_t       DEFAULT_PACKET_SIZE = 32;
    constexpr unsigned int DEFAULT_TTL         = 30;
    constexpr size_t       MAX_PING_DATA_SIZE  = 1024;
    constexpr size_t       MAX_PING_PACKET_SIZE{MAX_PING_DATA_SIZE + sizeof (IPHeader)};
#define ICMP_ECHO_REQUEST 8
}

Duration NetworkMontior::Ping (const InternetAddress& addr, const PingOptions& options)
{
    // file:///C:/Sandbox/Stroika/DevRoot/Winsock%20Programmer%E2%80%99s%20FAQ_%20Ping_%20Raw%20Sockets%20Method.html
    int packet_size        = DEFAULT_PACKET_SIZE;
    int ttl                = DEFAULT_TTL;
    packet_size            = max (sizeof (ICMPHeader), min (MAX_PING_DATA_SIZE, (unsigned int)packet_size));
    ICMPHeader pingRequest = []() {
        static int seq_no;
        ICMPHeader tmp{};
        tmp.type      = ICMP_ECHO_REQUEST;
        tmp.id        = (USHORT)GetCurrentProcessId ();
        tmp.seq       = seq_no++;
        tmp.timestamp = static_cast<uint32_t> (Time::GetTickCount () * 1000);
        return tmp;
    }();
    SmallStackBuffer<Byte> sendPacket (packet_size);
    memcpy (sendPacket.begin (), &pingRequest, sizeof (pingRequest));
    {
        const unsigned long int deadmeat   = 0xDEADBEEF;
        char*                   datapart   = (char*)sendPacket.begin () + sizeof (ICMPHeader);
        int                     bytes_left = packet_size - sizeof (ICMPHeader);
        while (bytes_left > 0) {
            memcpy (datapart, &deadmeat, min (int(sizeof (deadmeat)), bytes_left));
            bytes_left -= sizeof (deadmeat);
            datapart += sizeof (deadmeat);
        }
    }
    reinterpret_cast<ICMPHeader*> (sendPacket.begin ())->checksum = ip_checksum ((uint16_t*)sendPacket.begin (), packet_size);

    Socket s{Socket::ProtocolFamily::INET, Socket::SocketKind::RAW, IPPROTO_ICMP};
    s.setsockopt (IPPROTO_IP, IP_TTL, ttl);

    s.SendTo (sendPacket.begin (), sendPacket.end (), SocketAddress{addr, 0});

    while (true) {
        SocketAddress fromAddress;

        SmallStackBuffer<Byte> recv_buf (MAX_PING_PACKET_SIZE);
        size_t                 n     = s.ReceiveFrom (begin (recv_buf), end (recv_buf), 0, &fromAddress);
        IPHeader*              reply = reinterpret_cast<IPHeader*> (recv_buf.begin ());

        {
            // Skip ahead to the ICMP header within the IP packet
            unsigned short header_len = reply->h_len * 4;
            ICMPHeader*    icmphdr    = (ICMPHeader*)((char*)reply + header_len);

            // Make sure the reply is sane
            if (n < header_len + ICMP_MIN) {
#if 0
                cerr << "too few bytes from " << inet_ntoa (from->sin_addr) <<
                    endl;
                return -1;
#endif
                Execution::Throw (Execution::StringException (L"too few bytes from")); // draft @todo fix
            }
            else if (icmphdr->type != ICMP_ECHO_REPLY) {
                if (icmphdr->type != ICMP_TTL_EXPIRE) {
                    if (icmphdr->type == ICMP_DEST_UNREACH) {
                        Execution::Throw (Execution::StringException (L"Destination unreachable")); // draft @todo fix
                    }
                    else {
                        Execution::Throw (Execution::StringException (L"Unknown ICMP packet type")); // draft @todo fix
                        //  cerr << "Unknown ICMP packet type " << int (icmphdr->type) << " received" << endl;
                    }
                }
                // If "TTL expired", fall through.  Next test will fail if we
                // try it, so we need a way past it.
            }
            else if (icmphdr->id != (USHORT)GetCurrentProcessId ()) {
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

            if (icmphdr->type == ICMP_TTL_EXPIRE) {
                Execution::Throw (Execution::StringException (L"TTL expired")); // draft @todo fix
            }
            return Duration ((Time::GetTickCount () * 1000 - icmphdr->timestamp) / 1000);
        }
    }

    return Duration (1.0);
}
