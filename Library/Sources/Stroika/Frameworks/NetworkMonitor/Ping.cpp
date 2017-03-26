/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <random>

#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Collection.h"
#include "../../Foundation/Execution/Sleep.h"
#include "../../Foundation/Execution/TimeOutException.h"
#include "../../Foundation/IO/Network/InternetProtocol/ICMP.h"
#include "../../Foundation/IO/Network/InternetProtocol/IP.h"
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
using namespace Stroika::Foundation::IO::Network::InternetProtocol;
#if 0
using namespace Stroika::Foundation::IO::Network::InternetProtocol::ICMP;
using namespace Stroika::Foundation::IO::Network::InternetProtocol::ICMP::V4;
using namespace Stroika::Foundation::IO::Network::InternetProtocol::IP;
#endif

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::NetworkMonitor;
using namespace Stroika::Frameworks::NetworkMonitor::Ping;

using Memory::Byte;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    const Options::SampleInfo kDefaultSampleInfo_{Duration (0), 1};
}

// Originally based on
// file:///C:/Sandbox/Stroika/DevRoot/Winsock%20Programmer%E2%80%99s%20FAQ_%20Ping_%20Raw%20Sockets%20Method.html

/*
 ********************************************************************************
 ************** NetworkMonitor::Ping::Options::SampleInfo ***********************
 ********************************************************************************
 */
Characters::String Options::SampleInfo::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Interval: " + Characters::ToString (fInterval) + L", ";
    sb += L"Count: " + Characters::Format (L"%d", fSampleCount) + L", ";
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ********************** NetworkMonitor::Ping::Options ***************************
 ********************************************************************************
 */
constexpr Traversal::Range<size_t> Ping::Options::kAllowedICMPPayloadSizeRange;

const Duration Ping::Options::kDefaultTimeout{1.0};

String Ping::Options::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    if (fMaxHops) {
        sb += L"Max-Hops: " + Characters::Format (L"%d", *fMaxHops) + L", ";
    }
    if (fTimeout) {
        sb += L"Timeout: " + Characters::ToString (*fTimeout) + L", ";
    }
    if (fPacketPayloadSize) {
        sb += L"Packet-Payload-Size: " + Characters::Format (L"%d", *fPacketPayloadSize) + L", ";
    }
    if (fSampleInfo) {
        sb += L"Sample: " + Characters::ToString (*fSampleInfo) + L", ";
    }
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ********************** NetworkMonitor::Ping::Results ***************************
 ********************************************************************************
 */
String Results::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    if (fMedianPingTime) {
        sb += L"Median-Ping-Time: " + Characters::ToString (*fMedianPingTime) + L", ";
    }
    if (fMedianHopCount) {
        sb += L"Median-Hop-Count: " + Characters::Format (L"%d", *fMedianHopCount) + L", ";
    }
    if (fExceptionCount != 0) {
        sb += L"Exception-Count: " + Characters::Format (L"%d", fExceptionCount) + L", "; // to see exceptions - run with sample-count = 1
    }
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 *************************** NetworkMonitor::Ping::Run **************************
 ********************************************************************************
 */
Results NetworkMonitor::Ping::Run (const InternetAddress& addr, const Options& options)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Frameworks::NetworkMonitor::Ping::Run", L"addr=%s, options=%s", Characters::ToString (addr).c_str (), Characters::ToString (options).c_str ())};
    size_t                    icmpPacketSize = Options::kAllowedICMPPayloadSizeRange.Pin (options.fPacketPayloadSize.Value (Options::kDefaultPayloadSize)) + sizeof (ICMP::V4::PacketHeader);
    unsigned int              ttl            = options.fMaxHops.Value (Options::kDefaultMaxHops);

    Time::DurationSecondsType pingTimeout = options.fTimeout.Value (Options::kDefaultTimeout).As<Time::DurationSecondsType> ();

    Options::SampleInfo sampleInfo = options.fSampleInfo.Value (kDefaultSampleInfo_);
    Require (sampleInfo.fSampleCount >= 1);

    static std::mt19937    rng{std::random_device () ()};
    SmallStackBuffer<Byte> sendPacket (icmpPacketSize); // does not include IP header
    // use random data as a payload
    static std::uniform_int_distribution<std::mt19937::result_type> distByte (0, numeric_limits<Byte>::max ());
    for (Byte* p = (Byte*)sendPacket.begin () + sizeof (ICMP::V4::PacketHeader); p < sendPacket.end (); ++p) {
        static std::uniform_int_distribution<std::mt19937::result_type> distribution (0, numeric_limits<Byte>::max ());
        *p = distribution (rng);
    }

    /*
     *  General theorey of operation:
     *      >   Use ICMP (low level IP raw packets) to send a packet with a time-to-live (max # of hops) to the target address.
     *      >   That target address - if ICMP enabled properly - will send an ECHO reply back.
     *      >   we can look at how long this took, and report back the difference.
     *
     *  Also, allow for sampling differntly sized packets, etc.
     */

    Socket s{Socket::ProtocolFamily::INET, Socket::SocketKind::RAW, IPPROTO_ICMP};
    s.setsockopt (IPPROTO_IP, IP_TTL, ttl); // max # of hops

    Collection<DurationSecondsType> sampleTimes;
    Collection<unsigned int>        sampleHopCounts;
    unsigned int                    samplesTaken{};
    while (samplesTaken < sampleInfo.fSampleCount) {
        if (sampleInfo.fSampleCount != 0) {
            Execution::Sleep (sampleInfo.fInterval);
        }
        try {
            ICMP::V4::PacketHeader pingRequest = [&]() {
                static std::uniform_int_distribution<std::mt19937::result_type> distribution (0, numeric_limits<uint16_t>::max ());
                static uint16_t                                                 seq_no = distribution (rng);
                ICMP::V4::PacketHeader                                          tmp{};
                tmp.type      = ICMP::V4::ICMP_ECHO_REQUEST;
                tmp.id        = distribution (rng);
                tmp.seq       = seq_no++;
                tmp.timestamp = static_cast<uint32_t> (Time::GetTickCount () * 1000);
                return tmp;
            }();
            memcpy (sendPacket.begin (), &pingRequest, sizeof (pingRequest));
            reinterpret_cast<ICMP::V4::PacketHeader*> (sendPacket.begin ())->checksum = IP::ip_checksum (sendPacket.begin (), sendPacket.begin () + icmpPacketSize);
            s.SendTo (sendPacket.begin (), sendPacket.end (), SocketAddress{addr, 0});

            // Find first packet responding
            while (true) {
                SocketAddress          fromAddress;
                constexpr size_t       kExtraSluff_{100};                                                                                              // Leave a little extra room
                SmallStackBuffer<Byte> recv_buf (icmpPacketSize + sizeof (ICMP::V4::PacketHeader) + 2 * sizeof (IP::V4::PacketHeader) + kExtraSluff_); // icmpPacketSize includes ONE ICMP header and payload, but we get 2 IP and 2 ICMP headers in TTL Exceeded response
                size_t                 n = s.ReceiveFrom (begin (recv_buf), end (recv_buf), 0, &fromAddress, pingTimeout);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"got back packet from %s", Characters::ToString (fromAddress).c_str ());
#endif
                const IP::V4::PacketHeader* replyIPHeader = reinterpret_cast<const IP::V4::PacketHeader*> (recv_buf.begin ());

                {
                    // Skip ahead to the ICMP header within the IP packet
                    unsigned short                header_len      = replyIPHeader->ihl * 4;
                    const ICMP::V4::PacketHeader* replyICMPHeader = (const ICMP::V4::PacketHeader*)((const Byte*)replyIPHeader + header_len);

                    // Make sure the reply is sane
                    if (n < header_len + ICMP::V4::ICMP_MIN) {
                        Execution::Throw (Execution::StringException (L"too few bytes from " + Characters::ToString (fromAddress))); // draft @todo fix
                    }

                    Optional<uint16_t> echoedID;
                    switch (replyICMPHeader->type) {
                        case ICMP::V4::ICMP_ECHO_REPLY: {
                            echoedID = replyICMPHeader->id;
                        } break;
                        case ICMP::V4::ICMP_TTL_EXPIRE: {
                            // According to https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol#Time_exceeded - we also can find the first 8 bytes of original datagram's data
                            const ICMP::V4::PacketHeader* echoedICMPHeader = (const ICMP::V4::PacketHeader*)((const Byte*)replyICMPHeader + 8 + sizeof (IP::V4::PacketHeader));
                            echoedID                                       = echoedICMPHeader->id;
                        } break;
                        case ICMP::V4::ICMP_DEST_UNREACH: {
                            // According to https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol#Destination_unreachable - we also can find the first 8 bytes of original datagram's data
                            const ICMP::V4::PacketHeader* echoedICMPHeader = (const ICMP::V4::PacketHeader*)((const Byte*)replyICMPHeader + 8 + sizeof (IP::V4::PacketHeader));
                            echoedID                                       = echoedICMPHeader->id;
                        } break;
                    }
                    // If we got a response id, compare it with the request we sent to make sure we're reading a resposne to the request we sent
                    if (echoedID and echoedID != pingRequest.id) {
                        DbgTrace (L"echoedID (%s != pingRequest.id (%x) so ignoring this reply", Characters::ToString (echoedID).c_str (), pingRequest.id);
                        // Must be a reply for another pinger running locally, so just
                        // ignore it.
                        continue;
                    }

                    switch (replyICMPHeader->type) {
                        case ICMP::V4::ICMP_ECHO_REPLY: {
                            // Different operating systems use different starting values for TTL. TTL here is the original number used,
                            // less the number of hops. So we are left with making an educated guess. Need refrence and would be nice to find better
                            // way, but this seems to work pretty often.
                            unsigned int nHops{};
                            if (replyIPHeader->ttl > 128) {
                                nHops = 257 - replyIPHeader->ttl;
                            }
                            else if (replyIPHeader->ttl > 64) {
                                nHops = 129 - replyIPHeader->ttl;
                            }
                            else {
                                nHops = 65 - replyIPHeader->ttl;
                            }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace (L"reply->ttl = %d, nHops = %d", reply->ttl, nHops);
#endif

                            sampleTimes += (Time::GetTickCount () * 1000 - replyICMPHeader->timestamp) / 1000;
                            sampleHopCounts += nHops;
                            samplesTaken++;
                            goto nextSample;
                        }
                        case ICMP::V4::ICMP_TTL_EXPIRE: {
                            Execution::Throw (ICMP::V4::TTLExpiredException (InternetAddress{replyIPHeader->saddr}));
                        }
                        case ICMP::V4::ICMP_DEST_UNREACH: {
                            Execution::Throw (ICMP::V4::DestinationUnreachableException (replyICMPHeader->code, InternetAddress{replyIPHeader->saddr}));
                        };
                        default: {
                            Execution::Throw (ICMP::V4::UnknownICMPPacket (replyICMPHeader->type));
                        }
                    }
                }
            }
        }
        catch (...) {
            bool suppressThrows = sampleInfo.fSampleCount > 1;
            if (suppressThrows) {
                DbgTrace (L"ignore exception %s", Characters::ToString (current_exception ()).c_str ());
            }
            else {
                Execution::ReThrow ();
            }
            samplesTaken++;
        }
    nextSample:;
    }

    Assert (sampleTimes.empty () == sampleHopCounts.empty ());
    if (sampleTimes.empty ()) {
        return Results{{}, {}, samplesTaken};
    }
    else {
        return Results{Duration (*sampleTimes.Median ()), *sampleHopCounts.Median (), static_cast<unsigned int> (samplesTaken - sampleTimes.size ())};
    }
}
