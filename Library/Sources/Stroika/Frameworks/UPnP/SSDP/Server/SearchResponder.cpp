/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../../Foundation/Characters/Format.h"

#include "../../../../Foundation/Execution/Sleep.h"
#include "../../../../Foundation/Execution/Thread.h"
#include "../../../../Foundation/Execution/WaitForIOReady.h"
#include "../../../../Foundation/IO/Network/ConnectionlessSocket.h"
#include "../../../../Foundation/Streams/ExternallyOwnedMemoryInputStream.h"
#include "../../../../Foundation/Streams/MemoryStream.h"
#include "../../../../Foundation/Streams/TextReader.h"

#include "../Advertisement.h"
#include "../Common.h"
#include "SearchResponder.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

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
    void ParsePacketAndRespond_ (Streams::InputStream<Character>::Ptr in, const Iterable<Advertisement>& advertisements,
                                 ConnectionlessSocket::Ptr useSocket, SocketAddress sendTo)
    {
        String firstLine = in.ReadLine ().Trim ();

#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"Read SSDP Packet"};
        DbgTrace (L"firstLine: %s", firstLine.c_str ());
#endif
        static const String kNOTIFY_LEAD = "M-SEARCH "sv;
        if (firstLine.length () > kNOTIFY_LEAD.length () and firstLine.SubString (0, kNOTIFY_LEAD.length ()) == kNOTIFY_LEAD) {
            SSDP::Advertisement da;
            while (true) {
                String line = in.ReadLine ().Trim ();
                if (line.empty ()) {
                    break;
                }

                // Need to simplify this code (stroika string util)
                String label;
                String value;
                if (optional<size_t> n = line.Find (':')) {
                    label = line.SubString (0, *n);
                    value = line.SubString (*n + 1).Trim ();
                }
                if (not label.empty ()) {
                    da.fRawHeaders.Add (label, value);
                }
                constexpr auto kLabelComparer_ = String::ThreeWayComparer{Characters::CompareOptions::eCaseInsensitive};
                if (kLabelComparer_ (label, "ST"sv) == 0) {
                    da.fTarget = value;
                }
            }

            bool matches          = false;
            auto targetEqComparer = String::EqualsComparer{CompareOptions::eCaseInsensitive};
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
            if (matches) {
// if any match, I think we are supposed to send all
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"sending search responder advertisements...");
#endif
                for (auto a : advertisements) {
                    a.fAlive = nullopt; // in responder we don't set alive flag

                    bool includeThisAdvertisement = false;
                    if (targetEqComparer (da.fTarget, kTarget_SSDPAll)) {
                        includeThisAdvertisement = true;
                    }
                    else {
                        includeThisAdvertisement = targetEqComparer (a.fTarget, da.fTarget);
                    }

                    if (includeThisAdvertisement) {
                        Memory::BLOB data = SSDP::Serialize ("HTTP/1.1 200 OK"sv, SearchOrNotify::SearchResponse, a);
                        useSocket.SendTo (data.begin (), data.end (), sendTo);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        String msg;
                        msg += "location=" sz + a.fLocation + ", "sv;
                        msg += "TARGET(ST/NT)=" sz + a.fTarget + ", "sv;
                        msg += "USN=" sz + a.fUSN;
                        DbgTrace (L"(%s)", msg.c_str ());
#endif
                    }
                }
            }
        }
    }
}

SearchResponder::SearchResponder (const Iterable<Advertisement>& advertisements, ::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
{
    if constexpr (qDebug) {
        advertisements.Apply ([] ([[maybe_unused]] const auto& a) { Require (not a.fTarget.empty ()); });
    }

    // Construction of search responder will fail if we cannot bind - instead of failing quietly inside the loop
    Collection<pair<ConnectionlessSocket::Ptr, SocketAddress>> sockets;
    {
        static constexpr Execution::Activity kActivity_{"SSDP Binding in SearchResponder"sv};
        Execution::DeclareActivity           da{&kActivity_};
        constexpr unsigned int               kMaxHops_ = 4;
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
    fListenThread_ = Execution::Thread::New (
        [advertisements, sockets] () {
            Debug::TraceContextBumper ctx{"SSDP SearchResponder thread loop"};
            {
            Again:
                try {
                    for (pair<ConnectionlessSocket::Ptr, SocketAddress> s : sockets) {
                        s.first.JoinMulticastGroup (s.second.GetInternetAddress ());
                    }
                }
                catch (const system_error& e) {
                    // SEE https://stroika.atlassian.net/browse/STK-962 -
                    // If I migrate the retry logic to BasicServer, this stuff can go away, I believe... LOW PRIORITY - since this does work - last time I tried

                    if (e.code () == errc::no_such_device) {
                        // This can happen on Linux when you start before you have a network connection - no problem - just keep trying
                        DbgTrace ("Got exception (errno: ENODEV) - while joining multicast group, so try again");
                        Execution::Sleep (1s);
                        goto Again;
                    }
                    else {
                        Execution::ReThrow ();
                    }
                }
            }

            // only stopped by thread abort
            auto inUseSockets = sockets.Map<ConnectionlessSocket::Ptr> ([] (auto i) { return i.first; });
            while (true) {
                try {
                    for (ConnectionlessSocket::Ptr s : Execution::WaitForIOReady{inUseSockets}.WaitQuietly ()) {
                        SocketAddress from;
                        byte          buf[4 * 1024]; // not sure of max packet size
                        size_t        nBytesRead = s.ReceiveFrom (begin (buf), end (buf), 0, &from);
                        Assert (nBytesRead <= Memory::NEltsOf (buf));
                        using namespace Streams;
                        ParsePacketAndRespond_ (TextReader::New (ExternallyOwnedMemoryInputStream<byte>::New (begin (buf), begin (buf) + nBytesRead)),
                                                advertisements, s, from);
                    }
                }
                catch (const Execution::Thread::AbortException&) {
                    Execution::ReThrow ();
                }
                catch (...) {
                    // ignore errors - and keep on trucking
                    // but avoid wasting too much time if we get into an error storm
                    Execution::Sleep (1.0s);
                }
            }
        },
        Execution::Thread::eAutoStart, kThreadName_);
}
