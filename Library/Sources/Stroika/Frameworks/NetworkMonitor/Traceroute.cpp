/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
using namespace Stroika::Foundation::Traversal;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::NetworkMonitor;
using namespace Stroika::Frameworks::NetworkMonitor::Traceroute;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ************ NetworkMonitor::Traceroute::Options::SampleInfo *******************
 ********************************************************************************
 */
Characters::String Traceroute::Options::SampleInfo::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Interval: "sv << Characters::ToString (fInterval) << ", "sv;
    sb << "Count: "sv << Characters::Format (L"%d", fSampleCount);
    sb << "}"sv;
    return sb.str ();
}

/*
 ********************************************************************************
 ***************** NetworkMonitor::Traceroute::Options **************************
 ********************************************************************************
 */
String Traceroute::Options::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (fMaxHops) {
        sb << "Max-Hops: "sv << Characters::Format (L"%d", *fMaxHops) << ", "sv;
    }
    if (fTimeout) {
        sb << "Timeout: "sv << Characters::ToString (*fTimeout) << ", "sv;
    }
    if (fPacketPayloadSize) {
        sb << "Packet-Payload-Size: "sv << Characters::Format (L"%d", *fPacketPayloadSize) << ", "sv;
    }
    if (fSampleInfo) {
        sb << "Sample: "sv << Characters::ToString (*fSampleInfo);
    }
    sb << "}"sv;
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
    sb << "{"sv;
    sb << "Time: "sv << Characters::ToString (fTime) << ", "sv;
    sb << "Address: "sv << Characters::ToString (fAddress);
    sb << "}"sv;
    return sb.str ();
}

/*
 ********************************************************************************
 *********************** NetworkMonitor::Traceroute *****************************
 ********************************************************************************
 */
Sequence<Hop> NetworkMonitor::Traceroute::Run (const InternetAddress& addr, const Options& options)
{
    Sequence<Hop> results;
    Run (
        addr, [&results] (const Hop& h) { results += h; }, options);
    return results;
}

void NetworkMonitor::Traceroute::Run (const InternetAddress& addr, function<void (Hop)> perHopCallback, const Options& options)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
        "Frameworks::NetworkMonitor::Traceroute::Run", L"addr={}, options={}"_f, Characters::ToString (addr), Characters::ToString (options))};
    unsigned int              maxTTL = options.fMaxHops.value_or (Options::kDefaultMaxHops);

    Ping::Options pingOptions{};
    pingOptions.fPacketPayloadSize = options.fPacketPayloadSize;
    Ping::Pinger pinger{addr, pingOptions};

    for (unsigned int ttl = 1; ttl <= maxTTL; ++ttl) {
#if 0
        if (options.fSampleInfo) {
            pingOptions.fSampleInfo = Ping::Options::SampleInfo{options.fSampleInfo->fInterval, options.fSampleInfo->fSampleCount};
        }
#endif
        Time::TimePointSeconds startOfPingRequest = Time::GetTickCount ();
        try {
            Ping::Pinger::ResultType r = pinger.RunOnce (ttl);
            perHopCallback (Hop{r.fPingTime, addr});
            break;
        }
        catch (const ICMP::V4::TTLExpiredException& ttlExpiredException) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"exception %s - ipaddr = %s", Characters::ToString (ttlExpiredException).c_str (),
                      Characters::ToString (ttlExpiredException.GetReachedIP ()).c_str ());
#endif
            // totally normal - this is how we find out the hops
            perHopCallback (Hop{Duration{Time::GetTickCount () - startOfPingRequest}, ttlExpiredException.GetUnreachedIP ()});
        }
        catch (const ICMP::V4::DestinationUnreachableException& destinationUnreachableException) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"exception %s - ipaddr = %s", Characters::ToString (destinationUnreachableException).c_str (),
                      Characters::ToString (destinationUnreachableException.GetReachedIP ()).c_str ());
#endif
            // Not sure how normal this is? @todo - research - maybe abandon ping when this happens... -- LGP 2017-03-27
            perHopCallback (Hop{Duration{Time::GetTickCount () - startOfPingRequest}, destinationUnreachableException.GetUnreachedIP ()});
        }
        catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"exception %s ", Characters::ToString (current_exception ()).c_str ());
#endif
            perHopCallback (Hop{});
        }
    }
}