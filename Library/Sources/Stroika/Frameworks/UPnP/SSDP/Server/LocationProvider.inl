/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::UPnP::SSDP::Server {

    namespace Private_ {
        // the address to advertise for interface i on an SSDP channel of family f: one not link-local, if it has one
        optional<InternetAddress> AdvertisableAddress (const Foundation::IO::Network::Interface& i, InternetAddress::AddressFamily f);
    }

}
