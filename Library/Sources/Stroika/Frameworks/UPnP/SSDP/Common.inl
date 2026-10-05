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

        // Follows the OS's address changes, and calls onNetworkAppeared - on a thread of its own - once each burst of address
        // additions has gone quiet, so the caller can act on the new network once: a network coming up adds its addresses
        // one after another (IPv4, then each IPv6), and re-joining or re-searching or re-announcing for each would repeat it
        // several times over. That thread runs only from a burst's first address until the burst is acted on; the rest of the
        // time, following costs just a LinkMonitor (all of which share one thread). Being on its own thread, a slow
        // onNetworkAppeared delays nothing else. An exception from it is only logged. Once destroyed, onNetworkAppeared is not
        // running, and is not called again.
        class NetworkChangeFollower {
        public:
            NetworkChangeFollower (const function<void ()>& onNetworkAppeared); // throws where the OS cannot tell (e.g. some containers)
            NetworkChangeFollower (NetworkChangeFollower&&) noexcept;
            NetworkChangeFollower& operator= (NetworkChangeFollower&&) noexcept;
            ~NetworkChangeFollower ();

        private:
            struct Rep_;
            unique_ptr<Rep_> fRep_;
        };
        // a NetworkChangeFollower - or nullopt (and logged) if the OS cannot tell
        optional<NetworkChangeFollower> FollowNetworkChanges (const function<void ()>& onNetworkAppeared);
    }

}
