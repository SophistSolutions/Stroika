/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

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

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::InternetProtocol;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::NetworkMonitor;
using namespace Stroika::Frameworks::NetworkMonitor::Ping;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********************** NetworkMonitor::Ping::Options ***************************
 ********************************************************************************
 */
String Ping::Options::ToString () const
{
    StringBuilder sb;
    sb += "{";
    if (fMaxHops) {
        sb += "Max-Hops: " + Characters::Format (L"%d", *fMaxHops) + ", ";
    }
    if (fTimeout) {
        sb += "Timeout: " + Characters::ToString (*fTimeout) + ", ";
    }
    if (fPacketPayloadSize) {
        sb += "Packet-Payload-Size: " + Characters::Format (L"%d", *fPacketPayloadSize) + ", ";
    }
    sb += "}";
    return sb.str ();
}

/*
 ********************************************************************************
 ********************** NetworkMonitor::Ping::Pinger::ResultType ****************
 ********************************************************************************
 */
String Pinger::ResultType::ToString () const
{
    StringBuilder sb;
    sb += "{"sv;
    sb += "Ping-Time: "sv + Characters::ToString (fPingTime) + ", ";
    sb += "Hop-Count: "sv + Characters::Format (L"%d", fHopCount) + ", ";
    sb += "}"sv;
    return sb.str ();
}

/*
 ********************************************************************************
 *************************** NetworkMonitor::Ping::Pinger ***********************
 ********************************************************************************
 */
Pinger::Pinger (const InternetAddress& addr, const Options& options)
    : fDestination_{addr}
    , fOptions_{options}
    , fICMPPacketSize_{Options::kAllowedICMPPayloadSizeRange.Pin (options.fPacketPayloadSize.value_or (Options::kDefaultPayloadSize)) +
                       sizeof (ICMP::V4::PacketHeader)}
    , fSendPacket_{fICMPPacketSize_}
    , fSocket_{IO::Network::ConnectionlessSocket::New (SocketAddress::INET, Socket::RAW, IPPROTO_ICMP)}
    , fNextSequenceNumber_{static_cast<uint16_t> (fAllUInt16Distribution_ (fRng_))}
    , fPingTimeout_{options.fTimeout.value_or (Options::kDefaultTimeout).As<Time::DurationSecondsType> ()}
{
    DbgTrace (L"Frameworks::NetworkMonitor::Ping::Pinger::CTOR", L"addr=%s, options=%s", Characters::ToString (fDestination_).c_str (),
              Characters::ToString (fOptions_).c_str ());
    // use random data as a payload
    for (byte* p = (byte*)fSendPacket_.begin () + sizeof (ICMP::V4::PacketHeader); p < fSendPacket_.end (); ++p) {
        uniform_int_distribution<mt19937::result_type> anyByteDistribution (0, numeric_limits<uint8_t>::max ());
        *p = static_cast<byte> (anyByteDistribution (fRng_));
    }
}

Pinger::ResultType Pinger::RunOnce (const optional<unsigned int>& ttl)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Frameworks::NetworkMonitor::Ping::Pinger::RunOnce",
                                                                                 L"ttl=%s", Characters::ToString (ttl).c_str ())};
    return RunOnce_ICMP_ (ttl.value_or (fOptions_.fMaxHops.value_or (Options::kDefaultMaxHops)));
}

Pinger::ResultType Pinger::RunOnce_ICMP_ (unsigned int ttl)
{
    Debug::TraceContextBumper ctx{
        Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Frameworks::NetworkMonitor::Ping::Pinger::RunOnce_ICMP_", L"ttl=%d", ttl)};
    fSocket_.setsockopt (IPPROTO_IP, IP_TTL, ttl); // max # of hops

    ICMP::V4::PacketHeader pingRequest = [&] () {
        ICMP::V4::PacketHeader tmp{};
        tmp.type      = ICMP::V4::ICMP_ECHO_REQUEST;
        tmp.id        = static_cast<uint16_t> (fAllUInt16Distribution_ (fRng_));
        tmp.seq       = ++fNextSequenceNumber_;
        tmp.timestamp = static_cast<uint32_t> (Time::GetTickCount () * 1000);
        return tmp;
    }();
    (void)::memcpy (fSendPacket_.begin (), &pingRequest, sizeof (pingRequest));
    reinterpret_cast<ICMP::V4::PacketHeader*> (fSendPacket_.begin ())->checksum =
        IP::ip_checksum (fSendPacket_.begin (), fSendPacket_.begin () + fICMPPacketSize_);
    fSocket_.SendTo (fSendPacket_.begin (), fSendPacket_.end (), SocketAddress{fDestination_, 0});

    // Find first packet responding (some packets could be bogus/ignored)
    Time::DurationSecondsType pingTimeoutAfter = Time::GetTickCount () + fPingTimeout_;
    while (true) {
        ThrowTimeoutExceptionAfter (pingTimeoutAfter);
        SocketAddress    fromAddress;
        constexpr size_t kExtraSluff_{100}; // Leave a little extra room in case some packets return extra
        StackBuffer<byte> recv_buf{Memory::eUninitialized, fICMPPacketSize_ + sizeof (ICMP::V4::PacketHeader) + 2 * sizeof (IP::V4::PacketHeader) +
                                                               kExtraSluff_}; // icmpPacketSize includes ONE ICMP header and payload, but we get 2 IP and 2 ICMP headers in TTL Exceeded response
        size_t n = fSocket_.ReceiveFrom (begin (recv_buf), end (recv_buf), 0, &fromAddress, fPingTimeout_);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"got back packet from %s", Characters::ToString (fromAddress).c_str ());
#endif
        const IP::V4::PacketHeader* replyIPHeader = reinterpret_cast<const IP::V4::PacketHeader*> (recv_buf.begin ());

        // Skip ahead to the ICMP header within the IP packet
        unsigned short                header_len      = replyIPHeader->ihl * 4;
        const ICMP::V4::PacketHeader* replyICMPHeader = (const ICMP::V4::PacketHeader*)((const byte*)replyIPHeader + header_len);

        // Make sure the reply is sane
        if (n < header_len + ICMP::V4::ICMP_MIN) {
            Execution::Throw (Execution::Exception{"too few bytes from "sv + Characters::ToString (fromAddress)}); // draft @todo fix
        }

        /*
         * If we got a response id, compare it with the request we sent to make sure we're reading a resposne to the request we sent
         */
        optional<uint16_t> echoedID;
        switch (replyICMPHeader->type) {
            case ICMP::V4::ICMP_ECHO_REPLY: {
                echoedID = replyICMPHeader->id;
            } break;
            case ICMP::V4::ICMP_TTL_EXPIRE: {
                // According to https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol#Time_exceeded - we also can find the first 8 bytes of original datagram's data
                const ICMP::V4::PacketHeader* echoedICMPHeader =
                    (const ICMP::V4::PacketHeader*)((const byte*)replyICMPHeader + 8 + sizeof (IP::V4::PacketHeader));
                echoedID = echoedICMPHeader->id;
            } break;
            case ICMP::V4::ICMP_DEST_UNREACH: {
                // According to https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol#Destination_unreachable - we also can find the first 8 bytes of original datagram's data
                const ICMP::V4::PacketHeader* echoedICMPHeader =
                    (const ICMP::V4::PacketHeader*)((const byte*)replyICMPHeader + 8 + sizeof (IP::V4::PacketHeader));
                echoedID = echoedICMPHeader->id;
            } break;
        }
        if (echoedID and echoedID != pingRequest.id) {
            DbgTrace (L"echoedID (%s != pingRequest.id (%x) so ignoring this reply", Characters::ToString (echoedID).c_str (), pingRequest.id);
            // Must be a reply for another pinger running locally, so just
            // ignore it.
            continue;
        }

        /*
         * Handle different response messages differently - but looking for ICMP_ECHO_REPLY, or ICMP_TTL_EXPIRE (for traceroute)
         */
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
                DbgTrace (L"reply->ttl = %d, nHops = %d", replyIPHeader->ttl, nHops);
#endif
                return ResultType{Duration{Time::GetTickCount () - replyICMPHeader->timestamp / 1000.0}, nHops};
            }
            case ICMP::V4::ICMP_TTL_EXPIRE: {
                Execution::Throw (ICMP::V4::TTLExpiredException{InternetAddress{replyIPHeader->saddr}});
            }
            case ICMP::V4::ICMP_DEST_UNREACH: {
                Execution::Throw (ICMP::V4::DestinationUnreachableException{replyICMPHeader->code, InternetAddress{replyIPHeader->saddr}});
            };
            default: {
                Execution::Throw (ICMP::V4::UnknownICMPPacket{replyICMPHeader->type});
            }
        }
    }
    AssertNotReached ();
    return ResultType{};
}

/*
 ********************************************************************************
 ********************** NetworkMonitor::Ping::SampleOptions *********************
 ********************************************************************************
 */
Characters::String SampleOptions::ToString () const
{
    StringBuilder sb;
    sb += "{";
    sb += "Interval: " + Characters::ToString (fInterval) + ", ";
    sb += "Count: " + Characters::Format (L"%d", fSampleCount) + ", ";
    sb += "}";
    return sb.str ();
}

/*
 ********************************************************************************
 ******************* NetworkMonitor::Ping::SampleResults ************************
 ********************************************************************************
 */
String SampleResults::ToString () const
{
    StringBuilder sb;
    sb += "{";
    if (fMedianPingTime) {
        sb += "Median-Ping-Time: " + Characters::ToString (*fMedianPingTime) + ", ";
    }
    if (fMedianHopCount) {
        sb += "Median-Hop-Count: " + Characters::Format (L"%d", *fMedianHopCount) + ", ";
    }
    if (fExceptionCount != 0) {
        sb += "Exception-Count: " + Characters::Format (L"%d", fExceptionCount) + ", "; // to see exceptions - run with sample-count = 1
    }
    sb += "}";
    return sb.str ();
}

/*
 ********************************************************************************
 ************************* NetworkMonitor::Ping::Sample *************************
 ********************************************************************************
 */
SampleResults NetworkMonitor::Ping::Sample (const InternetAddress& addr, const SampleOptions& sampleOptions, const Options& options)
{
    Debug::TraceContextBumper       ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
        L"Frameworks::NetworkMonitor::Ping::Sample", L"addr=%s, sampleOptions=%s, options=%s", Characters::ToString (addr).c_str (),
        Characters::ToString (sampleOptions).c_str (), Characters::ToString (options).c_str ())};
    Pinger                          pinger{addr, options};
    Collection<DurationSecondsType> sampleTimes;
    Collection<unsigned int>        sampleHopCounts;
    unsigned int                    samplesTaken{};
    while (samplesTaken < sampleOptions.fSampleCount) {
        if (samplesTaken != 0) {
            Execution::Sleep (sampleOptions.fInterval);
        }
        try {
            Pinger::ResultType tmp = pinger.RunOnce ();
            sampleTimes += tmp.fPingTime.As<DurationSecondsType> ();
            sampleHopCounts += tmp.fHopCount;
            ++samplesTaken;
        }
        catch (...) {
            ++samplesTaken;
        }
    }
    return sampleTimes.empty () ? SampleResults{{}, {}, samplesTaken}
                                : SampleResults{Duration{*sampleTimes.Median ()}, *sampleHopCounts.Median (),
                                                static_cast<unsigned int> (samplesTaken - sampleTimes.size ())};
}
