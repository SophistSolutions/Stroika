/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../../Foundation/Characters/Format.h"

#include "../../../../Foundation/Execution/Sleep.h"
#include "../../../../Foundation/Execution/Thread.h"
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
    void ParsePacketAndRespond_ (Streams::InputStream<Character>::Ptr in, const Iterable<Advertisement>& advertisements, ConnectionlessSocket::Ptr useSocket, SocketAddress sendTo)
    {
        String firstLine = in.ReadLine ().Trim ();

#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx ("Read SSDP Packet");
        DbgTrace (L"firstLine: %s", firstLine.c_str ());
#endif
        static const String kNOTIFY_LEAD = L"M-SEARCH "sv;
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
                if (kLabelComparer_ (label, L"ST"sv) == 0) {
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
                        Memory::BLOB data = SSDP::Serialize (L"HTTP/1.1 200 OK", SearchOrNotify::SearchResponse, a);
                        useSocket.SendTo (data.begin (), data.end (), sendTo);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        String msg;
                        msg += L"location=" sz + a.fLocation + L", ";
                        msg += L"TARGET(ST/NT)=" sz + a.fTarget + L", ";
                        msg += L"USN=" sz + a.fUSN;
                        DbgTrace (L"(%s)", msg.c_str ());
#endif
                    }
                }
            }
        }
    }
}

void SearchResponder::Run (const Iterable<Advertisement>& advertisements)
{
#if qDebug
    for (const auto& a : advertisements) {
        Require (not a.fTarget.empty ());
    }
#endif
    static const String kThreadName_{L"SSDP Search Responder"sv};
    fListenThread_ = Execution::Thread::New (
        [advertisements] () {
            Debug::TraceContextBumper ctx{"SSDP SearchResponder thread loop"};
            ConnectionlessSocket::Ptr s         = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            Socket::BindFlags         bindFlags = Socket::BindFlags ();
            bindFlags.fReUseAddr                = true;
            s.Bind (SocketAddress (Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()), bindFlags);
            //s.Bind (SocketAddress (Network::V6::kAddrAny, UPnP::SSDP::V6::kSocketAddress.GetPort ()), bindFlags);
            s.SetMulticastLoopMode (true); // probably should make this configurable
            constexpr unsigned int kMaxHops_ = 4;
            s.SetMulticastTTL (kMaxHops_);
            {
            Again:
                try {
                    s.JoinMulticastGroup (UPnP::SSDP::V4::kSocketAddress.GetInternetAddress ());
                }
                catch (const system_error& e) {
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
            while (1) {
                try {
                    byte          buf[4 * 1024]; // not sure of max packet size
                    SocketAddress from;
                    size_t        nBytesRead = s.ReceiveFrom (begin (buf), end (buf), 0, &from);
                    Assert (nBytesRead <= NEltsOf (buf));
                    using namespace Streams;
                    ParsePacketAndRespond_ (TextReader::New (ExternallyOwnedMemoryInputStream<byte>::New (begin (buf), begin (buf) + nBytesRead)), advertisements, s, from);
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
    fListenThread_.Join ();
}
