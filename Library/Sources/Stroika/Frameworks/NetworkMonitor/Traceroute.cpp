/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <random>

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/IO/Network/Listener.h"
#include "Stroika/Foundation/IO/Network/Socket.h"

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
    sb << "Interval: "sv << fInterval << ", "sv;
    sb << "Count: "sv << fSampleCount;
    sb << "}"sv;
    return sb;
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
        sb << "Max-Hops: "sv << *fMaxHops << ", "sv;
    }
    if (fTimeout) {
        sb << "Timeout: "sv << *fTimeout << ", "sv;
    }
    if (fPacketPayloadSize) {
        sb << "Packet-Payload-Size: "sv << *fPacketPayloadSize << ", "sv;
    }
    if (fSampleInfo) {
        sb << "Sample: "sv << *fSampleInfo;
    }
    sb << "}"sv;
    return sb;
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
    sb << "Time: "sv << fTime << ", "sv;
    sb << "Address: "sv << fAddress;
    sb << "}"sv;
    return sb;
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
    Debug::TraceContextBumper ctx{"Frameworks::NetworkMonitor::Traceroute::Run", "addr={}, options={}"_f, addr, options};
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
            DbgTrace ("exception {} - ipaddr = {}"_f, ttlExpiredException, ttlExpiredException.GetReachedIP ());
#endif
            // totally normal - this is how we find out the hops
            perHopCallback (Hop{Duration{Time::GetTickCount () - startOfPingRequest}, ttlExpiredException.GetUnreachedIP ()});
        }
        catch (const ICMP::V4::DestinationUnreachableException& destinationUnreachableException) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("exception {} - ipaddr = {}"_f, destinationUnreachableException, destinationUnreachableException.GetReachedIP ());
#endif
            // Not sure how normal this is? @todo - research - maybe abandon ping when this happens... -- LGP 2017-03-27
            perHopCallback (Hop{Duration{Time::GetTickCount () - startOfPingRequest}, destinationUnreachableException.GetUnreachedIP ()});
        }
        catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"exception {}"_f, current_exception ());
#endif
            perHopCallback (Hop{});
        }
    }
}