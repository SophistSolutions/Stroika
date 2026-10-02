/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::UPnP::SSDP {

    namespace Private_ {
        // the network interfaces SSDP is to talk on: those, of this machine's now, that filter accepts
        Foundation::IO::Network::InterfacesByID GetSSDPInterfaces (const InterfaceFilter& filter);

        // for each of those (listed once), join each socket's group where the interface has an address of the group's family -
        // so multicasts arriving on any of them are heard; returns the interfaces joined on (each once, from that one listing;
        // a failure is only logged)
        Foundation::IO::Network::InterfacesByID JoinOnEveryInterface (
            const Foundation::Traversal::Iterable<pair<Foundation::IO::Network::ConnectionlessSocket::Ptr, Foundation::IO::Network::InternetAddress>>& socketsAndGroups,
            const InterfaceFilter& filter);

        // a LinkMonitor calling onNetworkAppeared (on its own thread) whenever an address is added - so the caller can act on
        // the new network; nullopt (and logged) if the OS cannot tell. An exception from onNetworkAppeared is only logged.
        optional<Foundation::IO::Network::LinkMonitor> FollowNetworkChanges (const function<void ()>& onNetworkAppeared);
    }

}
