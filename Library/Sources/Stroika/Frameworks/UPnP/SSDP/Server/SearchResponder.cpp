/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Execution/WaitForIOReady.h"
#include "Stroika/Foundation/IO/Network/ConnectionlessSocket.h"
#include "Stroika/Foundation/IO/Network/Interface.h"
#include "Stroika/Foundation/IO/Network/UniformResourceIdentification.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/ExternallyOwnedSpanInputStream.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Time/Realtime.h"

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
        // @todo the address of the interface the search arrived on, not the route lookup's - where the routing table sends the
        //       LAN elsewhere (a VPN's route covering it), the asker cannot reach that one. Exactly, that needs IP_PKTINFO -
        //       https://github.com/SophistSolutions/Stroika/issues/1202 - or by the asker's subnet, UPnP-only (TODO.md)
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

    // an answer to a search, to send at fDueAt
    struct Answer_ {
        Time::TimePointSeconds    fDueAt;
        ConnectionlessSocket::Ptr fSocket;
        SocketAddress             fTo;
        Memory::BLOB              fPacket;
    };

    // most answers waiting to be sent at once - so a flood of M-SEARCHes cannot make them pile up without bound
    constexpr size_t kMaxWaitingAnswers_{1000};

    // a header's value, by its name in any case
    optional<String> Header_ (const SSDP::Advertisement& a, const String& name)
    {
        for (const KeyValuePair<String, String>& h : a.fRawHeaders) {
            if (String::EqualsComparer{eCaseInsensitive}(h.fKey, name)) {
                return h.fValue;
            }
        }
        return nullopt;
    }

    // how long an M-SEARCH lets its answers wait, if it says: its MX - 5 seconds at most, as the UPnP Device Architecture
    // (1.1, section 1.3.3) has a device assume for a larger one
    optional<Time::DurationSeconds> MX_ (const SSDP::Advertisement& search)
    {
        optional<String> mx = Header_ (search, "MX"sv);
        if (not mx or mx->empty () or not mx->All ([] (Character c) { return c.IsDigit (); })) {
            return nullopt;
        }
        return Time::DurationSeconds{mx->size () > 9 ? 5 : min (Characters::String2Int<int> (*mx), 5)};
    }

    // whether an M-SEARCH was multicast - unless its HOST names a unicast address. (Which address it actually came to, only
    // IP_PKTINFO could say - https://github.com/SophistSolutions/Stroika/issues/1202)
    bool Multicast_ (const SSDP::Advertisement& search)
    {
        if (optional<String> host = Header_ (search, "HOST"sv)) {
            if (optional<UniformResourceIdentification::Authority> a = UniformResourceIdentification::Authority::Parse (*host)) {
                if (optional<InternetAddress> ia = a->GetHost () ? a->GetHost ()->AsInternetAddress () : nullopt) {
                    return ia->IsMulticastAddress ();
                }
            }
        }
        return true;
    }

    // the answers to packet, if an M-SEARCH for something advertised. Each of a multicast M-SEARCH's is due after a random wait of
    // up to its MX, so devices, and a device's several answers, do not all come at once - and one with no MX is ignored (1.1,
    // section 1.3.3); a unicast one's, at once
    Sequence<Answer_> Answers_ (span<const byte> packet, const Iterable<Advertisement>& advertisements, const LocationProvider& location,
                                ConnectionlessSocket::Ptr useSocket, SocketAddress sendTo)
    {
        String              headLine;
        SSDP::Advertisement da;
        SSDP::DeSerialize (Memory::BLOB{packet}, &headLine, &da);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"Read SSDP Packet"};
        DbgTrace ("headLine: {}"_f, headLine);
#endif
        Sequence<Answer_> result;
        if (headLine.StartsWith ("M-SEARCH "sv)) {
            optional<Time::DurationSeconds> waitAtMost; // nullopt: answer at once
            if (Multicast_ (da)) {
                waitAtMost = MX_ (da);
                if (not waitAtMost) {
                    return result;
                }
            }
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
                        Time::TimePointSeconds dueAt = Time::GetTickCount ();
                        if (waitAtMost) {
                            dueAt += SSDP::Private_::RandomDuration (*waitAtMost);
                        }
                        result += Answer_{dueAt, useSocket, sendTo, SSDP::Serialize ("HTTP/1.1 200 OK"sv, SearchOrNotify::SearchResponse, a)};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace ("(location={},TARGET(ST/NT)={},USN={})"_f, a.fLocation, a.fTarget, a.fUSN);
#endif
                    }
                }
            }
        }
        return result;
    }
}

SearchResponder::SearchResponder (const Iterable<Advertisement>& advertisements, const LocationProvider& location, const Options& options)
{
    if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
        advertisements.Apply ([] ([[maybe_unused]] const auto& a) { Require (not a.fTarget.empty ()); });
    }
    StartListening_ (advertisements, location, options); // here, so construction fails if it cannot bind
    if (options.fFollowNetworkChanges) {
        fNetworkChanges_ =
            SSDP::Private_::FollowNetworkChanges ([this, advertisements = Sequence<Advertisement>{advertisements}, location, options] () {
                // a network appeared: listen there too - on new sockets, joined afresh (the old listening thread uses the old
                // ones, so it stops first)
                [[maybe_unused]] lock_guard lifecycle{fLifecycleMutex_};
                fListenThread_.AbortAndWaitForDone ();
                StartListening_ (advertisements, location, options);
            });
    }
}

void SearchResponder::StartListening_ (const Iterable<Advertisement>& advertisements, const LocationProvider& location, const Options& options)
{
    InterfaceFilter interfaceFilter = options.fInterfaces;

    // Construction of search responder will fail if we cannot bind - instead of failing quietly inside the loop
    Collection<pair<ConnectionlessSocket::Ptr, SocketAddress>> sockets;
    {
        static constexpr Activity kActivity_{"SSDP Binding in SearchResponder"sv};
        DeclareActivity           da{&kActivity_};
        // (no multicast TTL to set: these sockets send only answers, which are unicast and need no TTL limit - UPnP Device
        // Architecture 1.1, section 1.3.3)
        if (InternetProtocol::IP::SupportIPV4 (options.fIPVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            s.Bind (SocketAddress{Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()}, Socket::BindFlags{.fSO_REUSEADDR = true});
            s.SetMulticastLoopMode (true); // probably should make this configurable
            sockets += make_pair (s, UPnP::SSDP::V4::kSocketAddress);
        }
        if (InternetProtocol::IP::SupportIPV6 (options.fIPVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET6, Socket::DGRAM);
            s.Bind (SocketAddress{Network::V6::kAddrAny, UPnP::SSDP::V6::kSocketAddress.GetPort ()}, Socket::BindFlags{.fSO_REUSEADDR = true});
            s.SetMulticastLoopMode (true); // probably should make this configurable
            sockets += make_pair (s, UPnP::SSDP::V6::kSocketAddress);
        }
    }

    // Use a thread to wait on a set of sockets we are listening for requests on
    static const String kThreadName_{"SSDP Search Responder"sv};
    fListenThread_ = Thread::New (
        [this, advertisements, location, interfaceFilter, sockets] () {
            Debug::TraceContextBumper ctx{"SSDP SearchResponder thread loop"};
            // join the group on every interface (running, not loopback) with an address of its family - so searches arriving
            // on any of them are heard; until at least one join works (e.g. started before there was a network), keep trying
            for (Time::DurationSeconds wait = 1s;; wait = min<Time::DurationSeconds> (wait * 2, 60s)) {
                Containers::Sequence<pair<ConnectionlessSocket::Ptr, InternetAddress>> toJoin;
                for (const pair<ConnectionlessSocket::Ptr, SocketAddress>& s : sockets) {
                    toJoin += make_pair (s.first, s.second.GetInternetAddress ());
                }
                InterfacesByID listeningOn = SSDP::Private_::JoinOnEveryInterface (toJoin, interfaceFilter);
                fListeningOn_.store (listeningOn);
                if (not listeningOn.empty ()) {
                    break;
                }
                // SEE https://github.com/SophistSolutions/Stroika/issues/1094 (STK-962) - BasicServer also restarts this when a network appears
                DbgTrace ("SSDP SearchResponder: could join on no interface, so try again in {}"_f, wait);
                Sleep (wait);
            }

            // only stopped by thread abort - which drops the answers still waiting
            auto              inUseSockets = sockets.Map<Iterable<ConnectionlessSocket::Ptr>> ([] (auto i) { return i.first; });
            Sequence<Answer_> waiting; // not yet due: this thread goes on answering other searches meanwhile, as 1.3.3 requires
            while (true) {
                try {
                    Time::TimePointSeconds nextDue{Time::kInfinity};
                    for (const Answer_& a : waiting) {
                        nextDue = min (nextDue, a.fDueAt);
                    }
                    for (ConnectionlessSocket::Ptr s : WaitForIOReady{inUseSockets}.WaitQuietlyUntil (nextDue)) {
                        SocketAddress from;
                        byte          buf[4 * 1024]; // not sure of max packet size
                        size_t        nBytesRead = s.ReceiveFrom (buf, 0, &from).size ();
                        Assert (nBytesRead <= std::size (buf));
                        for (const Answer_& a : Answers_ (span{buf, nBytesRead}, advertisements, location, s, from)) {
                            if (waiting.size () < kMaxWaitingAnswers_) {
                                waiting += a;
                            }
                        }
                    }
                    Time::TimePointSeconds now = Time::GetTickCount ();
                    Sequence<Answer_>      notYet;
                    for (const Answer_& a : waiting) {
                        if (a.fDueAt <= now) {
                            IgnoreExceptionsExceptThreadAbortForCall (a.fSocket.SendTo (a.fPacket, a.fTo));
                        }
                        else {
                            notYet += a;
                        }
                    }
                    waiting = notYet;
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

InterfacesByID SearchResponder::GetNetworkInterfaces () const
{
    return fListeningOn_.load ();
}
