/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../../Foundation/Characters/Format.h"
#include "../../../../Foundation/Characters/ToString.h"
#include "../../../../Foundation/Execution/Sleep.h"
#include "../../../../Foundation/Execution/Thread.h"
#include "../../../../Foundation/IO/Network/ConnectionlessSocket.h"

#include "../Advertisement.h"
#include "../Common.h"
#include "PeriodicNotifier.h"

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
 ******************************** PeriodicNotifier ******************************
 ********************************************************************************
 */
PeriodicNotifier::PeriodicNotifier (const Iterable<Advertisement>& advertisements, const FrequencyInfo& fi,
                                    IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
{
    if constexpr (qDebug) {
        advertisements.Apply ([] ([[maybe_unused]] const auto& a) { Require (not a.fTarget.empty ()); });
    }

    // Construction of notifier will fail if we cannot bind - instead of failing quietly inside the loop
    Collection<pair<ConnectionlessSocket::Ptr, SocketAddress>> sockets;
    {
        static constexpr Execution::Activity kActivity_{"SSDP Binding in PeriodNotifier"sv};
        Execution::DeclareActivity           da{&kActivity_};
        if (InternetProtocol::IP::SupportIPV4 (ipVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            s.Bind (SocketAddress{Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()}, Socket::BindFlags{.fSO_REUSEADDR = true});
            sockets += make_pair (s, UPnP::SSDP::V4::kSocketAddress);
        }
        if (InternetProtocol::IP::SupportIPV6 (ipVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET6, Socket::DGRAM);
            s.Bind (SocketAddress{Network::V6::kAddrAny, UPnP::SSDP::V6::kSocketAddress.GetPort ()}, Socket::BindFlags{.fSO_REUSEADDR = true});
            sockets += make_pair (s, UPnP::SSDP::V6::kSocketAddress);
        }
    }

    if constexpr (qStroika_Foundation_Debug_Trace_DefaultTracingOn) {
        Debug::TraceContextBumper ctx{"SSDP PeriodicNotifier - first time notifications"};
        for ([[maybe_unused]] const auto& a : advertisements) {
            DbgTrace ("(alive,loc={},usn={},...)"_f, Characters::ToString (a.fLocation), a.fUSN);
        }
    }

    Execution::IntervalTimer::TimerCallback callback = [=] () mutable {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"SSDP PeriodicNotifier - notifications"};
        for ([[maybe_unused]] const auto& a : advertisements) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            String msg;
            msg += "alive," sz;
            msg += "location=" sz + a.fLocation + ", " sz;
            msg += "ST=" sz + a.fST + ", " sz;
            msg += "USN=" sz + a.fUSN;
            DbgTrace (L"(%s)", msg.c_str ());
#endif
        }
#endif
        try {
            for (auto a : advertisements) {
                a.fAlive          = true; // periodic notifier must announce alive (we don't support 'going down' yet)
                Memory::BLOB data = SSDP::Serialize ("NOTIFY * HTTP/1.1"sv, SearchOrNotify::Notify, a);
                for (pair<ConnectionlessSocket::Ptr, SocketAddress> s : sockets) {
                    s.first.SendTo (data.begin (), data.end (), s.second);
                }
            }
        }
        catch (const Execution::Thread::AbortException&) {
            Execution::ReThrow ();
        }
        catch (...) {
            DbgTrace ("Ignoring inability to send SSDP notify packets: {} (try again later)"_f, Characters::ToString (current_exception ()));
        }
    };
    fIntervalTimerAdder_ = make_unique<Execution::IntervalTimer::Adder> (callback, Time::Duration{fi.fRepeatInterval},
                                                                         Execution::IntervalTimer::Adder::eRunImmediately);
}
