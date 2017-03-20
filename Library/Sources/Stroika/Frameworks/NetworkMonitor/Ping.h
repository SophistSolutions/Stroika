/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Frameworks_NetworkMontior_Ping_h_
#define _Stroika_Frameworks_NetworkMontior_Ping_h_ 1

#include "../StroikaPreComp.h"

#include <limits>

#include "../../Foundation/Characters/String.h"
#include "../../Foundation/IO/Network/InternetAddress.h"
#include "../../Foundation/IO/Network/InternetProtocol/ICMP.h"
#include "../../Foundation/IO/Network/InternetProtocol/IP.h"
#include "../../Foundation/Memory/Optional.h"
#include "../../Foundation/Time/Duration.h"
#include "../../Foundation/Traversal/Range.h"

/**
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 *      @todo
 */

namespace Stroika {
    namespace Frameworks {
        namespace NetworkMontior {

            using namespace Stroika::Foundation;

            using IO::Network::InternetAddress;
            using Memory::Optional;
            using Time::Duration;
            using Time::DurationSecondsType;

            struct PingOptions {
                /**
                 */
                static constexpr unsigned int kDefaultMaxHops = 64;

                /**
                 */
                Optional<unsigned int> fMaxHops;

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
                static constexpr Traversal::Range<size_t> kAllowedICMPPayloadSizeRange{0, numeric_limits<uint16_t>::max () - (sizeof (IO::Network::InternetProtocol::ICMP::PacketHeader) + sizeof (IO::Network::InternetProtocol::IP::iphdr)), Traversal::Openness::eClosed, Traversal::Openness::eClosed};

                /**
                 *  \not including ICMP nor IP header overhead.
                 */
                Optional<size_t> fPacketPayloadSize;

                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual Characters::String ToString () const;
            };

            /**
             *  @todo - document/define exceptions
             */
            Duration Ping (const InternetAddress& addr, const PingOptions& options = {});
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Ping.inl"

#endif /*_Stroika_Frameworks_NetworkMontior_Ping_h_*/
