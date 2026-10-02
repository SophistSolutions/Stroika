/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Thread.h"

#include "Common.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP::SSDP;

namespace {
    constexpr char     SSDP_MULTICAST_[] = "239.255.255.250";
    constexpr uint16_t SSDP_PORT_        = 1900;
}

const SocketAddress UPnP::SSDP::V4::kSocketAddress = SocketAddress{InternetAddress{SSDP_MULTICAST_, InternetAddress::AddressFamily::V4}, SSDP_PORT_};
const SocketAddress UPnP::SSDP::V6::kSocketAddress = SocketAddress{InternetAddress{"FF02::C"sv, InternetAddress::AddressFamily::V6}, SSDP_PORT_};

String UPnP::SSDP::MakeServerHeaderValue (const String& useProductTokenWithVersion, const String& usePlatformTokenAndVersion, const String& useUPNPVersion)
{
    Require (not useProductTokenWithVersion.empty ());
    Require (not usePlatformTokenAndVersion.empty ());
    Require (not useUPNPVersion.empty ());
    static const String kSpace_{" "sv};
    return usePlatformTokenAndVersion + kSpace_ + useUPNPVersion + kSpace_ + useProductTokenWithVersion;
}

/*
 ********************************************************************************
 *************************** SSDP::DefaultInterfaceFilter ***********************
 ********************************************************************************
 */
bool UPnP::SSDP::DefaultInterfaceFilter (const Interface& i)
{
    return i.fType != Interface::Type::eLoopback and i.fStatus and i.fStatus->Contains (Interface::Status::eRunning);
}

/*
 ********************************************************************************
 ********************** SSDP::Private_::GetSSDPInterfaces ***********************
 ********************************************************************************
 */
InterfacesByID UPnP::SSDP::Private_::GetSSDPInterfaces (const InterfaceFilter& filter)
{
    return SystemInterfacesMgr{}.GetAll ().Where ([&] (const Interface& i) { return filter (i); });
}

/*
 ********************************************************************************
 ********************* SSDP::Private_::JoinOnEveryInterface *********************
 ********************************************************************************
 */
InterfacesByID UPnP::SSDP::Private_::JoinOnEveryInterface (const Traversal::Iterable<pair<ConnectionlessSocket::Ptr, InternetAddress>>& socketsAndGroups,
                                                           const InterfaceFilter& filter)
{
    InterfacesByID joinedOn;
    for (const Interface& i : GetSSDPInterfaces (filter)) {
        bool joined = false;
        for (const pair<ConnectionlessSocket::Ptr, InternetAddress>& sg : socketsAndGroups) {
            if (not i.fBindings.fAddresses.Any (
                    [&] (const InternetAddress& a) { return a.GetAddressFamily () == sg.second.GetAddressFamily (); })) {
                continue;
            }
            try {
                sg.first.JoinMulticastGroup (sg.second, i);
                joined = true;
            }
            catch (const Execution::Thread::AbortException&) {
                Execution::ReThrow ();
            }
            catch (...) {
                DbgTrace ("SSDP: could not join {} on {}: {}"_f, sg.second, i.fInterfaceID, current_exception ());
            }
        }
        if (joined) {
            joinedOn.Add (i);
        }
    }
    return joinedOn;
}

/*
 ********************************************************************************
 ********************* SSDP::Private_::FollowNetworkChanges *********************
 ********************************************************************************
 */
optional<LinkMonitor> UPnP::SSDP::Private_::FollowNetworkChanges (const function<void ()>& onNetworkAppeared)
{
    try {
        LinkMonitor lm;
        lm.AddCallback ([onNetworkAppeared] (LinkMonitor::LinkChange lc, const String& linkName, const String& ipAddr) {
            if (lc == LinkMonitor::LinkChange::eAdded) {
                Debug::TraceContextBumper ctx{"SSDP: a network appeared", "linkName={}, ipAddr={}"_f, linkName, ipAddr};
                try {
                    onNetworkAppeared ();
                }
                catch (const Execution::Thread::AbortException&) {
                    Execution::ReThrow ();
                }
                catch (...) {
                    DbgTrace ("SSDP: could not act on the network that appeared: {}"_f, current_exception ());
                }
            }
        });
        return optional<LinkMonitor>{move (lm)};
    }
    catch (const Execution::Thread::AbortException&) {
        Execution::ReThrow ();
    }
    catch (...) {
        DbgTrace ("SSDP: cannot follow network changes here: {}"_f, current_exception ());
        return nullopt;
    }
}
