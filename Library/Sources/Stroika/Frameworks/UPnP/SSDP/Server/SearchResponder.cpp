/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/WaitForIOReady.h"
#include "Stroika/Foundation/IO/Network/ConnectionlessSocket.h"
#include "Stroika/Foundation/IO/Network/Interface.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/ExternallyOwnedSpanInputStream.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"

#include "Stroika/Frameworks/UPnP/SSDP/Advertisement.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"

#include "SearchResponder.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;
using namespace Stroika::Frameworks::UPnP::SSDP::Server;

// Comment this in to turn on tracing in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
********************************************************************************
******************************** SearchResponder *******************************
********************************************************************************
*/
namespace {
    // the LOCATION to answer asker with: location's, given this machine's address as asker reaches it
    optional<URI> LocationFor_ (const LocationProvider& location, const SocketAddress& asker)
    {
        optional<InternetAddress> local = GetLocalAddressToReach (asker);
        if (not local) {
            return nullopt; // no route back
        }
        if (local->GetAddressFamily () == InternetAddress::AddressFamily::V6 and local->IsLinkLocalAddress ()) {
            // asked from a link-local address: offer the interface's other IPv6 address, if it has one
            if (optional<Interface> i =
                    SystemInterfacesMgr{}.GetAll ().First ([&] (const Interface& ii) { return ii.fBindings.fAddresses.Contains (*local); })) {
                local = SSDP::Server::Private_::AdvertisableAddress (*i, InternetAddress::AddressFamily::V6);
            }
        }
        return location (LocationContext{*local, asker});
    }

    void ParsePacketAndRespond_ (span<const byte> packet, const Iterable<Advertisement>& advertisements, const LocationProvider& location,
                                 ConnectionlessSocket::Ptr useSocket, SocketAddress sendTo)
    {
        String              headLine;
        SSDP::Advertisement da;
        SSDP::DeSerialize (Memory::BLOB{packet}, &headLine, &da);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"Read SSDP Packet"};
        DbgTrace ("headLine: {}"_f, headLine);
#endif
        if (headLine.StartsWith ("M-SEARCH "sv)) {
            auto targetEqComparer = String::EqualsComparer{eCaseInsensitive};
            bool matches          = false;
            if (targetEqComparer (da.fTarget, kTarget_UPNPRootDevice)) {
                matches = true;
            }
            else if (targetEqComparer (da.fTarget, kTarget_SSDPAll)) {
                matches = true;
            }
            else {
                for (const auto& a : advertisements) {
                    if (targetEqComparer (a.fTarget, da.fTarget)) {
                        matches = true;
                        break;
                    }
                }
            }
            optional<URI> url = matches ? LocationFor_ (location, sendTo) : nullopt;
            if (url) {
// if any match, I think we are supposed to send all
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"sending search responder advertisements...");
#endif
                for (auto a : advertisements) {
                    a.fAlive    = nullopt; // in responder we don't set alive flag
                    a.fLocation = *url;

                    bool includeThisAdvertisement = false;
                    if (targetEqComparer (da.fTarget, kTarget_SSDPAll)) {
                        includeThisAdvertisement = true;
                    }
                    else {
                        includeThisAdvertisement = targetEqComparer (a.fTarget, da.fTarget);
                    }

                    if (includeThisAdvertisement) {
                        Memory::BLOB data = SSDP::Serialize ("HTTP/1.1 200 OK"sv, SearchOrNotify::SearchResponse, a);
                        useSocket.SendTo (data, sendTo);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace ("(location={},TARGET(ST/NT)={},USN={})"_f, a.fLocation, a.fTarget, a.fUSN);
#endif
                    }
                }
            }
        }
    }
}

SearchResponder::SearchResponder (const Iterable<Advertisement>& advertisements, const LocationProvider& location,
                                  ::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
{
    if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
        advertisements.Apply ([] ([[maybe_unused]] const auto& a) { Require (not a.fTarget.empty ()); });
    }

    // Construction of search responder will fail if we cannot bind - instead of failing quietly inside the loop
    Collection<pair<ConnectionlessSocket::Ptr, SocketAddress>> sockets;
    {
        static constexpr Activity kActivity_{"SSDP Binding in SearchResponder"sv};
        DeclareActivity           da{&kActivity_};
        constexpr unsigned int    kMaxHops_ = 4;
        if (InternetProtocol::IP::SupportIPV4 (ipVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            s.Bind (SocketAddress{Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()}, Socket::BindFlags{.fSO_REUSEADDR = true});
            s.SetMulticastLoopMode (true); // probably should make this configurable
            s.SetMulticastTTL (kMaxHops_);
            sockets += make_pair (s, UPnP::SSDP::V4::kSocketAddress);
        }
        if (InternetProtocol::IP::SupportIPV6 (ipVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET6, Socket::DGRAM);
            s.Bind (SocketAddress{Network::V6::kAddrAny, UPnP::SSDP::V6::kSocketAddress.GetPort ()}, Socket::BindFlags{.fSO_REUSEADDR = true});
            s.SetMulticastLoopMode (true); // probably should make this configurable
            s.SetMulticastTTL (kMaxHops_);
            sockets += make_pair (s, UPnP::SSDP::V6::kSocketAddress);
        }
    }

    // Use a thread to wait on a set of sockets we are listening for requests on
    static const String kThreadName_{"SSDP Search Responder"sv};
    fListenThread_ = Thread::New (
        [advertisements, location, sockets] () {
            Debug::TraceContextBumper ctx{"SSDP SearchResponder thread loop"};
            // join the group on every interface (running, not loopback) with an address of its family - so searches arriving
            // on any of them are heard; until at least one join works (e.g. started before there was a network), keep trying
            for (Time::DurationSeconds wait = 1s;; wait = min<Time::DurationSeconds> (wait * 2, 60s)) {
                unsigned int nJoined = 0;
                for (const Interface& i : SystemInterfacesMgr{}.GetAll ()) {
                    if (i.fType == Interface::Type::eLoopback or not(i.fStatus and i.fStatus->Contains (Interface::Status::eRunning))) {
                        continue;
                    }
                    for (const pair<ConnectionlessSocket::Ptr, SocketAddress>& s : sockets) {
                        InternetAddress group = s.second.GetInternetAddress ();
                        if (not i.fBindings.fAddresses.Any (
                                [&] (const InternetAddress& a) { return a.GetAddressFamily () == group.GetAddressFamily (); })) {
                            continue;
                        }
                        try {
                            s.first.JoinMulticastGroup (group, i);
                            ++nJoined;
                        }
                        catch (const Thread::AbortException&) {
                            ReThrow ();
                        }
                        catch (...) {
                            DbgTrace ("SSDP SearchResponder: could not join {} on {}: {}"_f, group, i.fInternalInterfaceID, current_exception ());
                        }
                    }
                }
                if (nJoined != 0) {
                    break;
                }
                // SEE https://github.com/SophistSolutions/Stroika/issues/1094 (STK-962) - BasicServer also restarts this when a network appears
                DbgTrace ("SSDP SearchResponder: could join on no interface, so try again in {}"_f, wait);
                Sleep (wait);
            }

            // only stopped by thread abort
            auto inUseSockets = sockets.Map<Iterable<ConnectionlessSocket::Ptr>> ([] (auto i) { return i.first; });
            while (true) {
                try {
                    for (ConnectionlessSocket::Ptr s : WaitForIOReady{inUseSockets}.WaitQuietly ()) {
                        SocketAddress from;
                        byte          buf[4 * 1024]; // not sure of max packet size
                        size_t        nBytesRead = s.ReceiveFrom (buf, 0, &from).size ();
                        Assert (nBytesRead <= std::size (buf));
                        ParsePacketAndRespond_ (span{buf, nBytesRead}, advertisements, location, s, from);
                    }
                }
                catch (const Thread::AbortException&) {
                    ReThrow ();
                }
                catch (...) {
                    // ignore errors - and keep on trucking
                    // but avoid wasting too much time if we get into an error storm
                    Sleep (1.0s);
                }
            }
        },
        Thread::eAutoStart, kThreadName_);
}
