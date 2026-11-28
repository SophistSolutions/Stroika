/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Timeout.h"
#include "Stroika/Foundation/IO/Network/InternetProtocol/ICMP.h"
#include "Stroika/Foundation/IO/Network/InternetProtocol/IP.h"
#include "Stroika/Foundation/IO/Network/Socket.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"

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
    sb << "{"sv;
    sb << "maxHops: "sv << fMaxHops;
    sb << ", timeout: "sv << fTimeout;
    sb << ", packetPayloadSize: "sv << fPacketPayloadSize;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ********************** NetworkMonitor::Ping::Pinger::ResultType ****************
 ********************************************************************************
 */
String Pinger::ResultType::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "pingTime: "sv << fPingTime;
    sb << ", hopCount: "sv << fHopCount;
    sb << "}"sv;
    return sb;
}

namespace {
    bool IsV6_ (const InternetAddress& a)
    {
        return a.GetAddressFamily () == InternetAddress::AddressFamily::V6;
    }

    // the size of the echo request header Pinger sends: IPv4's (V4::PacketHeader - ICMP's, and the first 4 bytes of its data), or
    // ICMPv6's
    size_t EchoHeaderSize_ (const InternetAddress& destination)
    {
        return IsV6_ (destination) ? sizeof (ICMP::V6::PacketHeader) : sizeof (ICMP::V4::PacketHeader);
    }

    // An ICMP message a ping's socket received, as far as Pinger cares - read from IPv4's raw packet or an ICMPv6 message
    struct Received_ {
        enum Kind_ {
            eEchoReply,
            eTTLExpired,
            eDestinationUnreachable,
            eOther, // not an answer to any echo request - another host's echo request, or the one sent (pinging this host)
        };
        Kind_              fKind{eOther};
        optional<uint16_t> fEchoedID; // the id of the echo request it answers: an echo reply's own, or the one an error quotes
        uint8_t            fCode{};
        optional<uint8_t>  fHopLimit; // what it arrived with - IPv4's TTL, IPv6's hop limit
        InternetAddress    fFrom;
    };

    // the length, in bytes, of the IPv4 header packet starts with (its ihl) - nullopt if packet is too short for that header,
    // or for an ICMP header after it
    optional<size_t> IPHeaderLengthBeforeICMP_ (span<const byte> packet)
    {
        if (packet.size () < sizeof (IP::V4::PacketHeader)) {
            return nullopt;
        }
        size_t length = reinterpret_cast<const IP::V4::PacketHeader*> (packet.data ())->ihl * 4;
        if (length < sizeof (IP::V4::PacketHeader) or packet.size () < length + ICMP::V4::ICMP_MIN) {
            return nullopt;
        }
        return length;
    }
    // The id of the echo request an ICMP error message (time exceeded, destination unreachable) quotes: after its own 8-byte
    // ICMP header comes the request's IP header (with any options) and the first 8 bytes of its data - the request's ICMP
    // header (RFC 792). nullopt if the message is too short to quote it.
    optional<uint16_t> QuotedRequestIDV4_ (span<const byte> icmpMessage)
    {
        span<const byte> quoted = icmpMessage.subspan (ICMP::V4::ICMP_MIN);
        if (optional<size_t> ipHeaderLength = IPHeaderLengthBeforeICMP_ (quoted)) {
            return reinterpret_cast<const ICMP::V4::PacketHeader*> (quoted.data () + *ipHeaderLength)->id;
        }
        return nullopt;
    }
    // what an IPv4 raw socket received: the packet, its IP header first - nullopt if too short for what its headers say
    optional<Received_> ReadV4_ (span<const byte> packet)
    {
        optional<size_t> ipHeaderLength = IPHeaderLengthBeforeICMP_ (packet);
        if (not ipHeaderLength) {
            return nullopt;
        }
        const IP::V4::PacketHeader*   ipHeader    = reinterpret_cast<const IP::V4::PacketHeader*> (packet.data ());
        span<const byte>              icmpMessage = packet.subspan (*ipHeaderLength);
        const ICMP::V4::PacketHeader* icmpHeader  = reinterpret_cast<const ICMP::V4::PacketHeader*> (icmpMessage.data ());
        Received_ result{.fCode = icmpHeader->code, .fHopLimit = ipHeader->ttl, .fFrom = InternetAddress{ipHeader->saddr}};
        switch (icmpHeader->type) {
            case ICMP::V4::ICMP_ECHO_REPLY:
                result.fKind     = Received_::eEchoReply;
                result.fEchoedID = icmpHeader->id;
                break;
            case ICMP::V4::ICMP_TTL_EXPIRE:
                result.fKind     = Received_::eTTLExpired;
                result.fEchoedID = QuotedRequestIDV4_ (icmpMessage);
                break;
            case ICMP::V4::ICMP_DEST_UNREACH:
                result.fKind     = Received_::eDestinationUnreachable;
                result.fEchoedID = QuotedRequestIDV4_ (icmpMessage);
                break;
        }
        return result;
    }
    // what an ICMPv6 raw socket received: the ICMPv6 message alone - no IPv6 header before it, so its hop limit comes as packet
    // information, and its sender as the socket's from address. nullopt if too short for an ICMPv6 header.
    optional<Received_> ReadV6_ (span<const byte> icmpMessage, const SocketAddress& from, const optional<ConnectionlessSocket::PacketInfo>& packetInfo)
    {
        if (icmpMessage.size () < ICMP::V6::ICMP_MIN) {
            return nullopt;
        }
        const ICMP::V6::PacketHeader* icmpHeader = reinterpret_cast<const ICMP::V6::PacketHeader*> (icmpMessage.data ());
        Received_ result{.fCode = icmpHeader->code, .fHopLimit = packetInfo ? packetInfo->fHopLimit : nullopt, .fFrom = from.GetInternetAddress ()};
        // An ICMPv6 error message quotes as much of the packet it answers as fits (RFC 4443, 3.1, 3.3): after its own 8-byte
        // header, the request's IPv6 header, then its ICMPv6 header
        auto quotedRequestID = [&] () -> optional<uint16_t> {
            constexpr size_t kQuotedICMPAt = ICMP::V6::ICMP_MIN + ICMP::V6::kIPHeaderSize;
            if (icmpMessage.size () < kQuotedICMPAt + sizeof (ICMP::V6::PacketHeader)) {
                return nullopt;
            }
            return reinterpret_cast<const ICMP::V6::PacketHeader*> (icmpMessage.data () + kQuotedICMPAt)->id;
        };
        switch (icmpHeader->type) {
            case ICMP::V6::ICMP_ECHO_REPLY:
                result.fKind     = Received_::eEchoReply;
                result.fEchoedID = icmpHeader->id;
                break;
            case ICMP::V6::ICMP_TTL_EXPIRE:
                result.fKind     = Received_::eTTLExpired;
                result.fEchoedID = quotedRequestID ();
                break;
            case ICMP::V6::ICMP_DEST_UNREACH:
                result.fKind     = Received_::eDestinationUnreachable;
                result.fEchoedID = quotedRequestID ();
                break;
        }
        return result;
    }

    // How many hops an echo reply crossed, from the hop limit it arrived with: what its sender set, less one per hop. Which
    // starting value the sender used is a guess - the usual ones are 64, 128 and 255 - the nearest above. 0 if unknown.
    unsigned int HopCount_ (optional<uint8_t> hopLimit)
    {
        if (not hopLimit) {
            return 0;
        }
        if (*hopLimit > 128) {
            return 257 - *hopLimit;
        }
        else if (*hopLimit > 64) {
            return 129 - *hopLimit;
        }
        return 65 - *hopLimit;
    }
}

/*
 ********************************************************************************
 *************************** NetworkMonitor::Ping::Pinger ***********************
 ********************************************************************************
 */
Pinger::Pinger (const InternetAddress& addr, const Options& options)
    : fDestination_{addr}
    , fOptions_{options}
    , fICMPPacketSize_{Options::kAllowedICMPPayloadSizeRange.Clamp (options.fPacketPayloadSize) + EchoHeaderSize_ (addr)}
    , fSendPacket_{fICMPPacketSize_}
    , fSocket_{IsV6_ (addr) ? IO::Network::ConnectionlessSocket::New (SocketAddress::INET6, Socket::RAW, IPPROTO_ICMPV6)
                            : IO::Network::ConnectionlessSocket::New (SocketAddress::INET, Socket::RAW, IPPROTO_ICMP)}
    , fNextSequenceNumber_{static_cast<uint16_t> (fAllUInt16Distribution_ (fRng_))}
    , fPingTimeout_{options.fTimeout}
{
    Debug::TraceContextBumper ctx{"Frameworks::NetworkMonitor::Ping::Pinger::CTOR", "addr={}, options={}"_f, fDestination_, fOptions_};
    Require (addr.GetAddressFamily () == InternetAddress::AddressFamily::V4 or IsV6_ (addr));
    if (IsV6_ (addr)) {
        fSocket_.SetReceivePacketInfo (true); // for a reply's hop limit - the IPv6 header with it does not come
    }
    // use random data as a payload
    for (byte* p = (byte*)fSendPacket_.begin () + EchoHeaderSize_ (addr); p < fSendPacket_.end (); ++p) {
        uniform_int_distribution<mt19937::result_type> anyByteDistribution (0, numeric_limits<uint8_t>::max ());
        *p = static_cast<byte> (anyByteDistribution (fRng_));
    }
}

Pinger::ResultType Pinger::RunOnce (const optional<unsigned int>& ttl)
{
    Debug::TraceContextBumper ctx{"Frameworks::NetworkMonitor::Ping::Pinger::RunOnce", "ttl={}"_f, ttl};
    return RunOnce_ICMP_ (ttl.value_or (fOptions_.fMaxHops));
}

Pinger::ResultType Pinger::RunOnce_ICMP_ (unsigned int ttl)
{
    Debug::TraceContextBumper ctx{
        Stroika_Foundation_Debug_OptionalizeTraceArgs ("Frameworks::NetworkMonitor::Ping::Pinger::RunOnce_ICMP_", "ttl={}"_f, ttl)};
    const bool     v6 = IsV6_ (fDestination_);
    const uint16_t id = static_cast<uint16_t> (fAllUInt16Distribution_ (fRng_));
    if (v6) {
        fSocket_.setsockopt (IPPROTO_IPV6, IPV6_UNICAST_HOPS, static_cast<int> (ttl)); // max # of hops
        ICMP::V6::PacketHeader pingRequest{};
        pingRequest.type = ICMP::V6::ICMP_ECHO_REQUEST;
        pingRequest.id   = id;
        pingRequest.seq  = ++fNextSequenceNumber_;
        // no checksum: the OS computes it - it covers the source address the OS picks (RFC 4443, 2.3)
        (void)::memcpy (fSendPacket_.begin (), &pingRequest, sizeof (pingRequest));
    }
    else {
        fSocket_.setsockopt (IPPROTO_IP, IP_TTL, ttl); // max # of hops
        ICMP::V4::PacketHeader pingRequest{};
        pingRequest.type = ICMP::V4::ICMP_ECHO_REQUEST;
        pingRequest.id   = id;
        pingRequest.seq  = ++fNextSequenceNumber_;
        (void)::memcpy (fSendPacket_.begin (), &pingRequest, sizeof (pingRequest));
        reinterpret_cast<ICMP::V4::PacketHeader*> (fSendPacket_.begin ())->checksum =
            IP::ip_checksum (fSendPacket_.begin (), fSendPacket_.begin () + fICMPPacketSize_);
    }
    // timed here, not from a timestamp sent in the request: one that fits there is too coarse, and wraps
    Time::TimePointSeconds sentAt = Time::GetTickCount ();
    fSocket_.SendTo (fSendPacket_, SocketAddress{fDestination_, 0});

    // Find first packet responding (some packets could be bogus/ignored)
    Time::TimePointSeconds pingTimeoutAfter = sentAt + fPingTimeout_;
    while (true) {
        ThrowTimeoutExceptionAfter (pingTimeoutAfter);
        SocketAddress                              fromAddress;
        optional<ConnectionlessSocket::PacketInfo> packetInfo;
        constexpr size_t                           kExtraSluff_{100}; // Leave a little extra room in case some packets return extra
        // fICMPPacketSize_ is ONE ICMP header and the payload, but an error message (TTL exceeded) quoting the request has 2 ICMP
        // headers, and an IP header or two (IPv4: its own, and the quoted one; IPv6: the quoted one)
        StackBuffer<byte> recv_buf{Memory::eUninitialized, fICMPPacketSize_ + sizeof (ICMP::V4::PacketHeader) +
                                                               2 * sizeof (IP::V4::PacketHeader) + ICMP::V6::kIPHeaderSize + kExtraSluff_};
        span<const byte>  packet =
            fSocket_.ReceiveFrom (recv_buf, 0, &fromAddress, v6 ? &packetInfo : nullptr, pingTimeoutAfter - Time::GetTickCount ());
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("got back packet from {}"_f, fromAddress);
#endif
        // The raw socket gets every ICMP packet this host does, from anyone: one too short for what its headers say is skipped,
        // as is one that answers no request of this pinger's (below) - the wait goes on, to the timeout
        optional<Received_> received = v6 ? ReadV6_ (packet, fromAddress, packetInfo) : ReadV4_ (packet);
        if (not received) {
            DbgTrace ("ignoring a packet too short to be an ICMP message ({} bytes) from {}"_f, packet.size (), fromAddress);
            continue;
        }
        if (received->fKind == Received_::eOther) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("ignoring an ICMP packet answering no echo request, from {}"_f, fromAddress);
#endif
            continue;
        }
        if (received->fEchoedID != id) {
            // for another pinger running locally (likely quite rare) - or an error message too short to say which request
            DbgTrace ("echoedID ({}) != pingRequest.id (0x{:x}), so ignoring this reply"_f, received->fEchoedID, static_cast<int> (id));
            continue;
        }
        switch (received->fKind) {
            case Received_::eEchoReply: {
                WeakAssert (received->fHopLimit.has_value ()); // IPv6: the OSes Stroika supports all report it, asked (SetReceivePacketInfo)
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("reply hop limit = {}, nHops = {}"_f, received->fHopLimit, HopCount_ (received->fHopLimit));
#endif
                return ResultType{Duration{Time::GetTickCount () - sentAt}, HopCount_ (received->fHopLimit)};
            }
            case Received_::eTTLExpired: {
                Execution::Throw (ICMP::TTLExpiredException{received->fFrom});
            }
            case Received_::eDestinationUnreachable: {
                Execution::Throw (ICMP::DestinationUnreachableException{received->fCode, received->fFrom});
            }
            default: {
                AssertNotReached (); // skipped above
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
    sb << "{"sv;
    sb << "interval: "sv << fInterval;
    sb << ", count: "sv << fSampleCount;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ******************* NetworkMonitor::Ping::SampleResults ************************
 ********************************************************************************
 */
String SampleResults::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (fMedianPingTime) {
        sb << "medianPingTime: "sv << *fMedianPingTime;
    }
    if (fMedianHopCount) {
        sb << ", medianHopCount: "sv << fMedianHopCount;
    }
    if (fExceptionCount != 0) {
        sb << ", exceptionCount: "sv << fExceptionCount; // to see exceptions - run with sample-count = 1
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************* NetworkMonitor::Ping::Sample *************************
 ********************************************************************************
 */
SampleResults NetworkMonitor::Ping::Sample (const InternetAddress& addr, const SampleOptions& sampleOptions, const Options& options)
{
    Debug::TraceContextBumper ctx{"Frameworks::NetworkMonitor::Ping::Sample", "addr={}, sampleOptions={}, options={}"_f, addr, sampleOptions, options};
    Pinger                            pinger{addr, options};
    Collection<Time::DurationSeconds> sampleTimes;
    Collection<unsigned int>          sampleHopCounts;
    unsigned int                      samplesTaken{};
    while (samplesTaken < sampleOptions.fSampleCount) {
        if (samplesTaken != 0) {
            Execution::Sleep (sampleOptions.fInterval);
        }
        try {
            Pinger::ResultType tmp = pinger.RunOnce ();
            sampleTimes += tmp.fPingTime;
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
