/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Server_LocationProvider_h_
#define _Stroika_Frameworks_UPnP_SSDP_Server_LocationProvider_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <functional>
#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/IO/Network/Interface.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

/*
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Frameworks::UPnP::SSDP::Server {

    using Foundation::Characters::String;
    using Foundation::IO::Network::InternetAddress;
    using Foundation::IO::Network::SocketAddress;
    using Foundation::IO::Network::URI;

    /**
     *  What SSDP knows when it is about to advertise - for a LocationProvider to choose the LOCATION by.
     */
    struct LocationContext {
        /**
         *  This machine's address on the network the advertisement goes to: for a NOTIFY, an address of the interface it goes
         *  out of; for a search response, the address the asker reaches this machine at (@see IO::Network::GetLocalAddressToReach).
         *  Of the family (IPv4 or IPv6) of the SSDP channel the advertisement is on - and a link-local IPv6 address only if the
         *  interface has no other.
         */
        InternetAddress fLocalAddress;

        /**
         *  For a search response, who asked; for a NOTIFY, nullopt.
         */
        optional<SocketAddress> fAsker;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  \brief The URL of the device description to advertise in the given context - or nullopt: do not advertise there.
     *
     *  SSDP advertises a device on every network interface, and answers every asker, and the right LOCATION can differ for
     *  each - and only the application knows where its web server listens, or how it is reached (a port mapping, a reverse
     *  proxy, a host name). So the application supplies it - usually as one of LocationFromBindings, FixedLocation or
     *  LocationFillingInHost.
     *
     *  Called from SSDP's own threads, so must be safe to call from any thread (the ready-made ones are).
     */
    using LocationProvider = function<optional<URI> (const LocationContext& context)>;

    /**
     *  \brief For a web server listening on webServerBindings (e.g. WebServer::ConnectionManager::bindings ()): where it can be
     *         reached at context.fLocalAddress.
     *
     *  The port is that of the binding on context.fLocalAddress itself, or else of a wildcard binding (V4::kAddrAny or
     *  V6::kAddrAny) of its family; nullopt if there is neither - so a web server listening only on IPv6 is never advertised
     *  over IPv4, and one listening on one address only on that address's network.
     *
     *  Never for a link-local IPv6 address: its URL would need a zone (http://[fe80::1%25en0]/) naming an interface of THIS
     *  machine, meaningless to the receiver.
     *
     *  \par Example Usage
     *      \code
     *          WebServer::ConnectionManager deviceWS{SocketAddresses (InternetAddresses_Any (), 8080), routes};
     *          BasicServer                  ssdpServer{device, deviceDescription, LocationFromBindings (deviceWS.bindings (), "/device.xml"sv)};
     *          // deviceWS listens on every IPv4 and IPv6 address, port 8080 - so on each network this advertises this machine's
     *          // address there:
     *          //      on the IPv4 network 192.168.1.x                           http://192.168.1.5:8080/device.xml
     *          //      on an IPv6 network where this machine is 2001:db8::5      http://[2001:db8::5]:8080/device.xml
     *          //      on an IPv6 network where it has only a link-local address (fe80::...): nothing
     *      \endcode
     */
    LocationProvider LocationFromBindings (const Foundation::Traversal::Iterable<SocketAddress>& webServerBindings,
                                           const String& path = "/"sv, const URI::SchemeType& scheme = URI::SchemeType{"http"sv});

    /**
     *  \brief Always url - for when the device is reached at an address that is not this machine's (a port mapping, NAT, a
     *         reverse proxy), or by name.
     *
     *  \note The same url goes to every network the device is advertised on, and nothing here can check that each of them
     *        can reach it: that depends on the receiver's network (its routes, NAT, its DNS), which this machine cannot see.
     *        So use it only for a URL all of those networks can reach - a name they all resolve, an address routed to from all
     *        of them. To advertise on some networks only, write the LocationProvider: it is told this machine's address on
     *        each candidate network (LocationContext::fLocalAddress), and returns nullopt to skip one.
     *
     *  \par Example Usage
     *      \code
     *          // the device description is served by a reverse proxy, under a name
     *          BasicServer ssdpServer{device, deviceDescription, FixedLocation (URI{"http://media-server.example:8200/device.xml"sv})};
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // ... but advertised on the 192.168.1.x network only
     *          static const CIDR kHomeNetwork_{"192.168.1.0/24"sv};
     *          BasicServer       ssdpServer{device, deviceDescription, [] (const LocationContext& c) -> optional<URI> {
     *                                if (kHomeNetwork_.GetRange ().Contains (c.fLocalAddress)) {
     *                                    return URI{"http://media-server.example:8200/device.xml"sv};
     *                                }
     *                                return nullopt; // not this network
     *                            }};
     *      \endcode
     */
    LocationProvider FixedLocation (const URI& url);

    /**
     *  \brief location itself, if it names a host; else location with its host set to context.fLocalAddress - and then
     *         nullopt for a link-local IPv6 address (@see LocationFromBindings).
     *
     *  \note Before Stroika v3.0d25 this was BasicServer's only behavior (from Device::fLocation) - except that it filled in
     *        one address for every network, IO::Network::GetPrimaryInternetAddress (), and an IPv4 one even on the IPv6 channel.
     *
     *  \par Example Usage
     *      \code
     *          // some other web server, on port 8080 of every address this machine has
     *          URI         location{URI::SchemeType{"http"sv}, URI::Authority{nullopt, PortType{8080}}, "/device.xml"sv}; // no host
     *          BasicServer ssdpServer{device, deviceDescription, LocationFillingInHost (location)};
     *          // advertises http://192.168.1.5:8080/device.xml on the 192.168.1.x network, http://10.0.0.7:8080/device.xml on 10.0.0.x
     *      \endcode
     */
    LocationProvider LocationFillingInHost (const URI& location);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LocationProvider.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Server_LocationProvider_h_*/
