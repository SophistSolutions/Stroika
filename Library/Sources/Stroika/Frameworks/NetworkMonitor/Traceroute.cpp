/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <random>

#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Collection.h"
#include "../../Foundation/Debug/Trace.h"
#include "../../Foundation/Execution/Sleep.h"
#include "../../Foundation/IO/Network/Listener.h"
#include "../../Foundation/IO/Network/Socket.h"

#include "Ping.h"

#include "Traceroute.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::InternetProtocol;
//using namespace Stroika::Foundation::IO::Network::InternetProtocol::ICMP;
//using namespace Stroika::Foundation::IO::Network::InternetProtocol::IP;
using namespace Stroika::Foundation::Traversal;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::NetworkMonitor;
using namespace Stroika::Frameworks::NetworkMonitor::Traceroute;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    const Options::SampleInfo kDefaultSampleInfo_{Duration (0), 1};
}

/*
 ********************************************************************************
 ************ NetworkMonitor::Traceroute::Options::SampleInfo *******************
 ********************************************************************************
 */
Characters::String Traceroute::Options::SampleInfo::ToString () const
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
 ***************** NetworkMonitor::Traceroute::Options **************************
 ********************************************************************************
 */
constexpr Traversal::Range<size_t> Traceroute::Options::kAllowedICMPPayloadSizeRange;

const Duration Traceroute::Options::kDefaultTimeout{1.0};

String Traceroute::Options::ToString () const
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
 ******************** NetworkMonitor::Traceroute::Hop ***************************
 ********************************************************************************
 */
String Hop::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Time: " + Characters::ToString (fTime) + L", ";
    sb += L"Address: " + Characters::ToString (fAddress) + L", ";
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 *********************** NetworkMonitor::Traceroute *****************************
 ********************************************************************************
 */
Sequence<Hop> NetworkMonitor::Traceroute::Run (const InternetAddress& addr, const Options& options)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Frameworks::NetworkMonitor::Traceroute::Run", L"addr=%s, options=%s", Characters::ToString (addr).c_str (), Characters::ToString (options).c_str ())};
    Sequence<Hop>             results;
    unsigned int              maxTTL = options.fMaxHops.Value (Options::kDefaultMaxHops);
    for (unsigned int ttl = 1; ttl <= maxTTL; ++ttl) {
        Ping::Options pingOptions{};
        pingOptions.fMaxHops           = ttl;
        pingOptions.fPacketPayloadSize = options.fPacketPayloadSize;
#if 0
        if (options.fSampleInfo) {
            pingOptions.fSampleInfo = Ping::Options::SampleInfo{options.fSampleInfo->fInterval, options.fSampleInfo->fSampleCount};
        }
#endif
        // for traceroute, we must do accum, not ping, or we must have way other than exception to handle TTL exceeded (its own rollup/summary)
        pingOptions.fSampleInfo = Ping::Options::SampleInfo{Duration (0), 1}; //tmphack
        try {
            Ping::Results r = Ping::Run (addr, pingOptions);
            results += Hop{
                r.fMedianPingTime.Value (),
                addr};
            break;
        }
        catch (const ICMP::V4::TTLExpiredException& ttlExpiredException) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"exception %s - ipaddr = %s", Characters::ToString (ttlExpiredException).c_str (), Characters::ToString (ttlExpiredException.GetReachedIP ()).c_str ());
#endif
            // totally normal - this is how we find out the hops
            results += Hop{
                Duration (1),
                ttlExpiredException.GetReachedIP ()};
        }
        catch (const ICMP::V4::DestinationUnreachableException& destinationUnreachableException) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"exception %s - ipaddr = %s", Characters::ToString (destinationUnreachableException).c_str (), Characters::ToString (destinationUnreachableException.GetReachedIP ()).c_str ());
#endif
            // totally normal - this is how we find out the hops
            results += Hop{
                Duration (1),
                destinationUnreachableException.GetReachedIP ()};
        }
        catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"exception %s ", Characters::ToString (current_exception ()).c_str ());
#endif
            results += Hop{};
            ///
        }
    }
    return results;
}
