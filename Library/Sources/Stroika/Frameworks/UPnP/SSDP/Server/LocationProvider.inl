/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::UPnP::SSDP::Server {

    namespace Private_ {
        // the address to advertise for interface i on an SSDP channel of family f: one not link-local, if it has one
        optional<InternetAddress> AdvertisableAddress (const Foundation::IO::Network::Interface& i, InternetAddress::AddressFamily f);

        // this machine's address on asker's own network - one of interfaces' addresses in a subnet of it that asker is in - or
        // nullopt: asker further off, or link-local (in every interface's subnet), so the routing table must say
        optional<InternetAddress> LocalAddressOnAskersNetwork (const InternetAddress& asker,
                                                               const Foundation::Traversal::Iterable<Foundation::IO::Network::Interface>& interfaces);
    }

}
