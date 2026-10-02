/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Common_h_
#define _Stroika_Frameworks_UPnP_SSDP_Common_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/SystemConfiguration.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/IO/Network/ConnectionlessSocket.h"
#include "Stroika/Foundation/IO/Network/Interface.h"
#include "Stroika/Foundation/IO/Network/LinkMonitor.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

/**
 *  \file
 *
 *  See http://quimby.gnus.org/internet-drafts/draft-cai-ssdp-v1-03.txt
 *  for details on the SSDP specification.
 *
 *  And http://www.upnp-hacks.org/upnp.html for more hints.
 *
 *  Also - http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.0.pdf
 *  Also - http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf
 */

namespace Stroika::Frameworks::UPnP::SSDP {

    namespace V4 {
        extern const Foundation::IO::Network::SocketAddress kSocketAddress;
    }
    namespace V6 {
        extern const Foundation::IO::Network::SocketAddress kSocketAddress;
    }

    using Foundation::Characters::String;

    /**
     *  MakeServerHeaderValue
     */
    String
    MakeServerHeaderValue (const String& useProductTokenWithVersion,
                           const String& usePlatformTokenAndVersion = Foundation::Common::GetSystemConfiguration_ActualOperatingSystem ().fRFC1945CompatProductTokenWithVersion,
                           const String& useUPNPVersion = "UPnP/1.0"sv);

    /**
     *  \brief Which network interfaces SSDP is to talk on - asked of each interface every time SSDP lists them, so it applies to
     *         networks that come and go, too.
     *
     *  The clients (Client::Search, Client::Listener) and servers (Server::BasicServer ...) take one in their Options, defaulting
     *  to DefaultInterfaceFilter.
     *
     *  \par Example Usage
     *      \code
     *          // search as usual, but not over a tunnel (a VPN)
     *          Client::Search search{callOnFinds, Client::Search::kRootDevice, Client::Search::Options{.fInterfaces = [] (const Interface& i) {
     *                                    return DefaultInterfaceFilter (i) and i.fType != Interface::Type::eTunnel;
     *                                }}};
     *      \endcode
     */
    using InterfaceFilter = function<bool (const Foundation::IO::Network::Interface& i)>;

    /**
     *  \brief SSDP's default InterfaceFilter: an interface that is running, and not loopback (whose multicast support varies by OS).
     */
    bool DefaultInterfaceFilter (const Foundation::IO::Network::Interface& i);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Common_h_*/
