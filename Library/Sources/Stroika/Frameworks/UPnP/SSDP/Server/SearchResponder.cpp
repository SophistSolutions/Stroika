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
    // the LOCATION to answer asker with: location's, given this machine's address as asker reaches it - on asker's own network,
    // found by its subnet among listeningOn (the interfaces the search can have come in on); else, asker further off, the
    // routing table's. Not the routing table's first: a VPN's route covering a LAN makes that the VPN's address, which the LAN
    // cannot reach. (Exactly, which interface the search came in on, only IP_PKTINFO could say -
    // https://github.com/SophistSolutions/Stroika/issues/1202)
    optional<URI> LocationFor_ (const LocationProvider& location, const SocketAddress& asker, const InterfacesByID& listeningOn)
    {
        optional<InternetAddress> local =
            asker.IsInternetAddress () ? SSDP::Server::Private_::LocalAddressOnAskersNetwork (asker.GetInternetAddress (), listeningOn) : nullopt;
        if (not local) {
            local = GetLocalAddressToReach (asker);
            if (not local) {
                return nullopt; // no route back
            }
            if (local->GetAddressFamily () == InternetAddress::AddressFamily::V6 and local->IsLinkLocalAddress ()) {
                // asked from a link-local address: offer the interface's other IPv6 address, if it has one
                if (optional<Interface> i = SystemInterfacesMgr{}.GetAll ().First (
                        [&] (const Interface& ii) { return ii.fBindings.fAddresses.Contains (*local); })) {
                    local = SSDP::Server::Private_::AdvertisableAddress (*i, InternetAddress::AddressFamily::V6);
                }
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
    bool IsMulticast_ (const SSDP::Advertisement& search)
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

    // a device or service type - urn:domain:device|service:type:version - as the type, and its version
    optional<pair<String, unsigned int>> TypeAndVersion_ (const String& target)
    {
        optional<size_t> lastColon = target.RFind (':');
        if (not target.StartsWith ("urn:"sv, eCaseInsensitive) or not lastColon) {
            return nullopt;
        }
        String version = target.SubString (*lastColon + 1);
        if (version.empty () or version.size () > 9 or not version.All ([] (Character c) { return c.IsDigit (); })) {
            return nullopt;
        }
        return make_pair (target.SubString (0, *lastColon), Characters::String2Int<unsigned int> (version));
    }

    // advertisement a, as it answers a search for searchTarget - or nullopt if it does not. Every advertisement answers ssdp:all;
    // and one for a device or service type answers a search for an older version of it too, as that version, in its ST and its USN
    // - each version being compatible with those before it (UPnP Device Architecture 1.1, sections 1.3.2 and 1.3.3)
    optional<Advertisement> AnswerTo_ (const String& searchTarget, const Advertisement& a)
    {
        String::EqualsComparer equals{eCaseInsensitive};
        if (equals (searchTarget, kTarget_SSDPAll) or equals (searchTarget, a.fTarget)) {
            return a;
        }
        optional<pair<String, unsigned int>> searchedFor = TypeAndVersion_ (searchTarget);
        optional<pair<String, unsigned int>> advertised  = TypeAndVersion_ (a.fTarget);
        if (searchedFor and advertised and equals (searchedFor->first, advertised->first) and 1 <= searchedFor->second and
            searchedFor->second < advertised->second) {
            Advertisement answer = a;
            answer.fTarget       = searchTarget;
            if (a.fUSN.EndsWith ("::"sv + a.fTarget, eCaseInsensitive)) {
                answer.fUSN = a.fUSN.SubString (0, a.fUSN.size () - a.fTarget.size ()) + searchTarget;
            }
            return answer;
        }
        return nullopt;
    }

    // the answers to packet, if an M-SEARCH for something advertised. Each of a multicast M-SEARCH's is due after a random wait of
    // up to its MX, so devices, and a device's several answers, do not all come at once - and one with no MX is ignored (1.1,
    // section 1.3.3); a unicast one's, at once
    Sequence<Answer_> Answers_ (span<const byte> packet, const Iterable<Advertisement>& advertisements, const LocationProvider& location,
                                ConnectionlessSocket::Ptr useSocket, SocketAddress sendTo, const InterfacesByID& listeningOn)
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
            if (IsMulticast_ (da)) {
                waitAtMost = MX_ (da);
                if (not waitAtMost) {
                    return result;
                }
            }
            Sequence<Advertisement> answering; // each advertisement that answers it, as it does
            for (const Advertisement& a : advertisements) {
                if (optional<Advertisement> answer = AnswerTo_ (da.fTarget, a)) {
                    answering += *answer;
                }
            }
            optional<URI> url = answering.empty () ? nullopt : LocationFor_ (location, sendTo, listeningOn);
            if (url) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("sending search responder advertisements..."_f);
#endif
                for (Advertisement a : answering) {
                    a.fAlive    = nullopt; // in responder we don't set alive flag
                    a.fLocation = *url;

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
        return result;
    }
}

SearchResponder::SearchResponder (const Iterable<Advertisement>& advertisements, const LocationProvider& location, const Options& options)
{
    if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
        advertisements.Apply ([] ([[maybe_unused]] const auto& a) { Require (not a.fTarget.empty ()); });
    }
    // Construction of search responder will fail if we cannot bind - instead of failing quietly inside the loop
    Sequence<pair<ConnectionlessSocket::Ptr, InternetAddress>> socketsAndGroups; // each socket, and the SSDP group it joins
    {
        static constexpr Activity kActivity_{"SSDP Binding in SearchResponder"sv};
        DeclareActivity           da{&kActivity_};
        // (no multicast TTL to set: these sockets send only answers, which are unicast and need no TTL limit - UPnP Device
        // Architecture 1.1, section 1.3.3)
        if (InternetProtocol::IP::SupportIPV4 (options.fIPVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            s.Bind (SocketAddress{Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()}, Socket::BindFlags{.fSO_REUSEADDR = true});
            s.SetMulticastLoopMode (true); // probably should make this configurable
            socketsAndGroups += make_pair (s, UPnP::SSDP::V4::kSocketAddress.GetInternetAddress ());
        }
        if (InternetProtocol::IP::SupportIPV6 (options.fIPVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET6, Socket::DGRAM);
            s.Bind (SocketAddress{Network::V6::kAddrAny, UPnP::SSDP::V6::kSocketAddress.GetPort ()}, Socket::BindFlags{.fSO_REUSEADDR = true});
            s.SetMulticastLoopMode (true); // probably should make this configurable
            socketsAndGroups += make_pair (s, UPnP::SSDP::V6::kSocketAddress.GetInternetAddress ());
        }
    }

    // Use a thread to wait on a set of sockets we are listening for requests on
    static const String kThreadName_{"SSDP Search Responder"sv};
    fListenThread_ = Thread::New (
        [this, advertisements = Sequence<Advertisement>{advertisements}, location, interfaceFilter = options.fInterfaces, socketsAndGroups] () {
            Debug::TraceContextBumper ctx{"SSDP SearchResponder thread loop"};
            auto inUseSockets = socketsAndGroups.Map<Iterable<ConnectionlessSocket::Ptr>> ([] (auto i) { return i.first; });
            WaitForIOReady<ConnectionlessSocket::Ptr> readyChecker{inUseSockets, WaitForIOReady<ConnectionlessSocket::Ptr>::kDefaultTypeOfMonitor,
                                                                   fJoinAgain_->GetWaitInfo ()};
            // the group is joined on every interface (running, not loopback) with an address of its family - so searches arriving
            // on any of them are heard: at first, again when a network appears (on these sockets, so the searches waiting in them,
            // and the answers waiting to go, are kept), and - until at least one join works (e.g. started before there was a
            // network) - every so often
            optional<Time::TimePointSeconds> joinAt = Time::GetTickCount ();
            Time::DurationSeconds            retryJoinAfter{1s};
            InterfacesByID                   listeningOn; // as joined last (fListeningOn_, for this thread)
            // only stopped by thread abort - which drops the answers still waiting
            Sequence<Answer_> waiting; // not yet due: this thread goes on answering other searches meanwhile, as 1.3.3 requires
            while (true) {
                try {
                    if (fJoinAgain_->IsSet () or (joinAt and *joinAt <= Time::GetTickCount ())) {
                        fJoinAgain_->Clear (); // first: a network appearing as this joins wakes the wait below at once
                        listeningOn = SSDP::Private_::JoinOnEveryInterface (socketsAndGroups, interfaceFilter);
                        fListeningOn_.store (listeningOn);
                        joinAt = nullopt;
                        if (listeningOn.empty ()) {
                            // SEE https://github.com/SophistSolutions/Stroika/issues/1094 (STK-962)
                            DbgTrace ("SSDP SearchResponder: could join on no interface, so try again in {}"_f, retryJoinAfter);
                            joinAt         = Time::GetTickCount () + retryJoinAfter;
                            retryJoinAfter = min<Time::DurationSeconds> (retryJoinAfter * 2, 60s);
                        }
                    }
                    Time::TimePointSeconds nextDue = joinAt.value_or (Time::TimePointSeconds{Time::kInfinity});
                    for (const Answer_& a : waiting) {
                        nextDue = min (nextDue, a.fDueAt);
                    }
                    for (ConnectionlessSocket::Ptr s : readyChecker.WaitQuietlyUntil (nextDue)) {
                        SocketAddress from;
                        byte          buf[4 * 1024]; // not sure of max packet size
                        size_t        nBytesRead = s.ReceiveFrom (buf, 0, &from).size ();
                        Assert (nBytesRead <= std::size (buf));
                        for (const Answer_& a : Answers_ (span{buf, nBytesRead}, advertisements, location, s, from, listeningOn)) {
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
    if (options.fFollowNetworkChanges) {
        // a network appeared: the listening thread joins it there too - woken to, on the sockets it has (each used by one thread
        // at a time)
        fNetworkChanges_ = SSDP::Private_::FollowNetworkChanges ([this] () { fJoinAgain_->Set (); });
    }
}

InterfacesByID SearchResponder::GetNetworkInterfaces () const
{
    return fListeningOn_.load ();
}
