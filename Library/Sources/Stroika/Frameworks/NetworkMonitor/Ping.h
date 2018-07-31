/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_NetworkMonitor_Ping_h_
#define _Stroika_Frameworks_NetworkMonitor_Ping_h_ 1

#include "../StroikaPreComp.h"

#include <limits>
#include <optional>
#include <random>

#include "../../Foundation/Characters/String.h"
#include "../../Foundation/IO/Network/ConnectionlessSocket.h"
#include "../../Foundation/IO/Network/InternetAddress.h"
#include "../../Foundation/IO/Network/InternetProtocol/ICMP.h"
#include "../../Foundation/IO/Network/InternetProtocol/IP.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"
#include "../../Foundation/Time/Duration.h"
#include "../../Foundation/Traversal/Range.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo
 */

namespace Stroika::Frameworks {
    namespace NetworkMonitor {
        namespace Ping {

            using namespace Stroika::Foundation;

            using IO::Network::InternetAddress;
            using Time::Duration;
            using Time::DurationSecondsType;

            /**
             */
            struct Options {
                /**
                 */
                static constexpr unsigned int kDefaultMaxHops = 64;

                /**
                 */
                optional<unsigned int> fMaxHops;

                /**
                 */
                static const Duration kDefaultTimeout;

                /**
                 *  time after a single ping is sent before we treat the ping as having timed out (so not total time if multiple samples taken).
                 */
                optional<Duration> fTimeout;

                /*
                 * No standard for this, but just what this library does.
                 */
                static constexpr size_t kDefaultPayloadSize = 32;

                /**
                 *  The range of supported payload (not including ICMP and IP packet headers)
                 *
                 *      @see http://stackoverflow.com/questions/9449837/maximum-legal-size-of-icmp-echo-packet
                 *
                 *      This does NOT include the IP header, nor the ICMP Header
                 */
                static constexpr Traversal::Range<size_t> kAllowedICMPPayloadSizeRange{0, numeric_limits<uint16_t>::max () - (sizeof (IO::Network::InternetProtocol::ICMP::V4::PacketHeader) + sizeof (IO::Network::InternetProtocol::IP::V4::PacketHeader)), Traversal::Openness::eClosed, Traversal::Openness::eClosed};

                /**
                 *  \not including ICMP nor IP header overhead.
                 */
                optional<size_t> fPacketPayloadSize;

                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual Characters::String ToString () const;
            };

            /**
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
             */
            class Pinger {
            public:
                Pinger ()              = delete;
                Pinger (const Pinger&) = delete;
                Pinger (const InternetAddress& addr, const Options& options = {});

            public:
                struct ResultType;

            public:
                /**
                 *  Can throw:
                 *      TimeoutException
                 *      InternetProtocol::ICMP::DestinationUnreachableException
                 *      InternetProtocol::ICMP::UnknownICMPPacket
                 *      InternetProtocol::ICMP::TTLExpiredException
                 *      others...
                 */
                nonvirtual ResultType RunOnce (const optional<unsigned int>& ttl = {});

            private:
                nonvirtual ResultType RunOnce_ICMP_ (unsigned int ttl);

            private:
                std::uniform_int_distribution<std::mt19937::result_type> fAllUInt16Distribution_{0, numeric_limits<uint16_t>::max ()};
                std::mt19937                                             fRng_{std::random_device () ()};
                InternetAddress                                          fDestination_;
                Options                                                  fOptions_;
                size_t                                                   fICMPPacketSize_;
                Memory::SmallStackBuffer<Memory::Byte>                   fSendPacket_;
                IO::Network::ConnectionlessSocket::Ptr                   fSocket_;
                uint16_t                                                 fNextSequenceNumber_;
                Time::DurationSecondsType                                fPingTimeout_;
            };

            /**
             */
            struct Pinger::ResultType {
                Duration     fPingTime;
                unsigned int fHopCount{};

                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual Characters::String ToString () const;
            };

            /**
             */
            struct SampleOptions {
                Duration     fInterval;
                unsigned int fSampleCount{};

                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual Characters::String ToString () const;
            };

            /**
             */
            struct SampleResults {
                optional<Duration>     fMedianPingTime; // excludes timeouts
                optional<unsigned int> fMedianHopCount;
                unsigned int           fExceptionCount{};

                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual Characters::String ToString () const;
            };

            /**
             *  \breif Send network packets to the argument internet address (govered by sample Options and optins) - and return the sampled results (e.g. averages)
             *
             *  'options' govern how the pinging is done.
             *
             *  'sampleOptions' govern how the results are statistically processed.
             *
             *  Sample () will summarize most exceptions in Results.
             */
            SampleResults Sample (const InternetAddress& addr, const SampleOptions& sampleOptions = {}, const Options& options = {});
        }
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Ping.inl"

#endif /*_Stroika_Frameworks_NetworkMonitor_Ping_h_*/
