/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Sequence.h"

#include "LocationProvider.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks::UPnP::SSDP;
using namespace Stroika::Frameworks::UPnP::SSDP::Server;

namespace {
    // a URL naming a link-local IPv6 address needs a zone naming an interface of THIS machine - meaningless to the receiver
    bool Advertisable_ (const InternetAddress& a)
    {
        return not(a.GetAddressFamily () == InternetAddress::AddressFamily::V6 and a.IsLinkLocalAddress ());
    }
}

/*
 ********************************************************************************
 *************************** Server::LocationContext ****************************
 ********************************************************************************
 */
String LocationContext::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "LocalAddress: "sv << Characters::ToString (fLocalAddress);
    if (fAsker) {
        sb << ", Asker: "sv << Characters::ToString (*fAsker);
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************ Server::LocationFromBindings **************************
 ********************************************************************************
 */
LocationProvider Server::LocationFromBindings (const Traversal::Iterable<SocketAddress>& webServerBindings, const String& path, const URI::SchemeType& scheme)
{
    Containers::Sequence<SocketAddress> bindings{webServerBindings}; // a copy, independent of where they came from
    return [bindings, path, scheme] (const LocationContext& context) -> optional<URI> {
        const InternetAddress& thisMachineAddressOnCandidateNetwork = context.fLocalAddress;
        if (not Advertisable_ (thisMachineAddressOnCandidateNetwork)) {
            return nullopt;
        }
        const InternetAddress anyOfFamily =
            thisMachineAddressOnCandidateNetwork.GetAddressFamily () == InternetAddress::AddressFamily::V4 ? V4::kAddrAny : V6::kAddrAny;
        optional<SocketAddress> b = bindings.First ([&] (const SocketAddress& sa) {
            return sa.IsInternetAddress () and sa.GetInternetAddress () == thisMachineAddressOnCandidateNetwork;
        });
        if (not b) {
            b = bindings.First ([&] (const SocketAddress& sa) { return sa.IsInternetAddress () and sa.GetInternetAddress () == anyOfFamily; });
        }
        if (not b) {
            return nullopt; // the web server isn't reachable on that network
        }
        return URI{scheme, URI::Authority{thisMachineAddressOnCandidateNetwork, b->GetPort ()}, path};
    };
}

/*
 ********************************************************************************
 **************************** Server::FixedLocation *****************************
 ********************************************************************************
 */
LocationProvider Server::FixedLocation (const URI& url)
{
    return [url] ([[maybe_unused]] const LocationContext& context) -> optional<URI> { return url; };
}

/*
 ********************************************************************************
 ************************ Server::LocationFillingInHost *************************
 ********************************************************************************
 */
LocationProvider Server::LocationFillingInHost (const URI& location)
{
    return [location] (const LocationContext& context) -> optional<URI> {
        optional<URI::Authority> authority = location.GetAuthority ();
        if (authority and authority->GetHost ()) {
            return location; // it names its own host
        }
        if (not Advertisable_ (context.fLocalAddress)) {
            return nullopt;
        }
        URI::Authority useAuthority = authority.value_or (URI::Authority{});
        useAuthority.SetHost (context.fLocalAddress);
        URI result = location;
        result.SetAuthority (useAuthority);
        return result;
    };
}

/*
 ********************************************************************************
 ******************** Server::Private_::AdvertisableAddress *********************
 ********************************************************************************
 */
optional<InternetAddress> Server::Private_::AdvertisableAddress (const Interface& i, InternetAddress::AddressFamily f)
{
    optional<InternetAddress> linkLocal;
    for (const InternetAddress& a : i.fBindings.fAddresses) {
        if (a.GetAddressFamily () == f) {
            if (not a.IsLinkLocalAddress ()) {
                return a;
            }
            if (not linkLocal) {
                linkLocal = a;
            }
        }
    }
    return linkLocal;
}
