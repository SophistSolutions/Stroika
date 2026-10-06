/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/IO/Network/ConnectionlessSocket.h"
#include "Stroika/Foundation/IO/Network/Interface.h"

#include "Stroika/Frameworks/UPnP/SSDP/Advertisement.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"

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

namespace {
    // each set of NOTIFYs - every advertisement, out of every interface - goes out this many times, this far apart, as UDP loses
    // packets: the UPnP Device Architecture (1.1, section 1.2.2) says more than once - "e.g. a few hundred milliseconds" apart - and
    // not more than three times. 100 ms (libupnp's default too) keeps short the pause this adds to starting and stopping a device
    constexpr unsigned int          kSetsSent_{2};
    constexpr Time::DurationSeconds kBetweenSets_{100ms};
}

/*
 ********************************************************************************
 ******************************** PeriodicNotifier ******************************
 ********************************************************************************
 */
PeriodicNotifier::PeriodicNotifier (const Iterable<Advertisement>& advertisements, const LocationProvider& location, const Options& options)
    : fNotifyingOn_{Memory::MakeSharedPtr<Execution::Synchronized<InterfacesByID>> ()}
{
    Require (options.fFrequencyInfo.fRepeatInterval < options.fFrequencyInfo.fMaxAge); // else listeners forget the device between NOTIFYs
    InterfaceFilter       interfaceFilter = options.fInterfaces;
    Time::DurationSeconds maxAge          = options.fFrequencyInfo.fMaxAge;

    if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
        advertisements.Apply ([] ([[maybe_unused]] const auto& a) { Require (not a.fTarget.empty ()); });
    }

    // Construction of notifier will fail if we cannot bind - instead of failing quietly inside the loop
    Collection<pair<ConnectionlessSocket::Ptr, SocketAddress>> sockets;
    {
        static constexpr Execution::Activity kActivity_{"SSDP Binding in PeriodNotifier"sv};
        Execution::DeclareActivity           da{&kActivity_};
        if (InternetProtocol::IP::SupportIPV4 (options.fIPVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            s.Bind (SocketAddress{Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()}, Socket::BindFlags{.fSO_REUSEADDR = true});
            s.SetMulticastTTL (options.fMulticastTTL);
            sockets += make_pair (s, UPnP::SSDP::V4::kSocketAddress);
        }
        if (InternetProtocol::IP::SupportIPV6 (options.fIPVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET6, Socket::DGRAM);
            s.Bind (SocketAddress{Network::V6::kAddrAny, UPnP::SSDP::V6::kSocketAddress.GetPort ()}, Socket::BindFlags{.fSO_REUSEADDR = true});
            s.SetMulticastTTL (options.fMulticastTTL);
            sockets += make_pair (s, UPnP::SSDP::V6::kSocketAddress);
        }
    }

    if constexpr (qStroika_Foundation_Debug_DefaultTracingOn) {
        Debug::TraceContextBumper ctx{"SSDP PeriodicNotifier - first time notifications"};
        for ([[maybe_unused]] const auto& a : advertisements) {
            DbgTrace ("(alive,usn={},...)"_f, a.fUSN);
        }
    }

    // NOTIFYs on socket s go out of interface i - whose own address, of s's family, is local
    auto goOutOf = [] (const ConnectionlessSocket::Ptr& s, const Interface& i, const InternetAddress& local) {
        if (local.GetAddressFamily () == InternetAddress::AddressFamily::V4) {
            s.SetMulticastInterface (local);
        }
        else {
            s.SetMulticastInterface (i);
        }
    };

    // the NOTIFYs go out on the IntervalTimer's thread, and - right after a network appears - the network-change thread's: one at a time
    shared_ptr<mutex>                                   sendingNotifies = Memory::MakeSharedPtr<mutex> ();
    shared_ptr<Execution::Synchronized<InterfacesByID>> notifyingOn     = fNotifyingOn_;
    Execution::IntervalTimer::TimerCallback             callback        = [=] () mutable {
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
        [[maybe_unused]] lock_guard critSec{*sendingNotifies}; // (through the sets' spacing too: only the other sender waits for it)
        InterfacesByID              sentOn;
        for (unsigned int nthSet = 0; nthSet < kSetsSent_; ++nthSet) {
            if (nthSet != 0) {
                Execution::Sleep (kBetweenSets_); // on the IntervalTimer's thread, which other timers share - so briefly
            }
            try {
                // out of each interface, with that interface's own LOCATION
                for (const Interface& i : SSDP::Private_::GetSSDPInterfaces (interfaceFilter)) {
                    for (const pair<ConnectionlessSocket::Ptr, SocketAddress>& s : sockets) {
                        try {
                            InternetAddress::AddressFamily family = s.second.GetInternetAddress ().GetAddressFamily ();
                            optional<InternetAddress>      local  = SSDP::Server::Private_::AdvertisableAddress (i, family);
                            if (not local) {
                                continue; // no address of this channel's family there
                            }
                            optional<URI> url = location (LocationContext{*local, nullopt});
                            if (not url) {
                                continue; // nothing to advertise there
                            }
                            goOutOf (s.first, i, *local);
                            for (Advertisement a : advertisements) {
                                a.fAlive    = true; // (and ssdp:byebye as this goes - ~PeriodicNotifier)
                                a.fLocation = *url;
                                a.fMaxAge   = maxAge;
                                s.first.SendTo (SSDP::Serialize ("NOTIFY * HTTP/1.1"sv, SearchOrNotify::Notify, a, s.second), s.second);
                            }
                            sentOn.Add (i);
                        }
                        catch (const Execution::Thread::AbortException&) {
                            Execution::ReThrow ();
                        }
                        catch (...) {
                            DbgTrace ("Ignoring inability to send SSDP notify packets on {}: {} (try again later)"_f, i.fInterfaceID,
                                      current_exception ());
                        }
                    }
                }
            }
            catch (const Execution::Thread::AbortException&) {
                Execution::ReThrow ();
            }
            catch (...) {
                DbgTrace ("Ignoring inability to send SSDP notify packets: {} (try again later)"_f, current_exception ());
            }
        }
        notifyingOn->store (sentOn);
    };
    // as this goes (~PeriodicNotifier): an ssdp:byebye for each advertisement, out of each interface the last ssdp:alive went out of
    fSayByebye_ = [sockets, advertisements, sendingNotifies, notifyingOn, goOutOf] () {
        [[maybe_unused]] lock_guard critSec{*sendingNotifies};
        // as many sets as of ssdp:alive: one byebye for each alive (UPnP Device Architecture 1.1, section 1.2.3)
        for (unsigned int nthSet = 0; nthSet < kSetsSent_; ++nthSet) {
            if (nthSet != 0) {
                Execution::Sleep (kBetweenSets_);
            }
            for (const Interface& i : notifyingOn->load ()) {
                for (const pair<ConnectionlessSocket::Ptr, SocketAddress>& s : sockets) {
                    try {
                        InternetAddress::AddressFamily family = s.second.GetInternetAddress ().GetAddressFamily ();
                        optional<InternetAddress>      local  = SSDP::Server::Private_::AdvertisableAddress (i, family);
                        if (not local) {
                            continue; // no address of this channel's family there
                        }
                        goOutOf (s.first, i, *local);
                        for (Advertisement a : advertisements) {
                            a.fAlive = false;
                            s.first.SendTo (SSDP::Serialize ("NOTIFY * HTTP/1.1"sv, SearchOrNotify::Notify, a, s.second), s.second);
                        }
                    }
                    catch (...) {
                        DbgTrace ("Ignoring inability to send SSDP byebye packets on {}: {}"_f, i.fInterfaceID, current_exception ());
                    }
                }
            }
        }
        notifyingOn->store (InterfacesByID{});
    };
    fIntervalTimerAdder_ = make_unique<Execution::IntervalTimer::Adder> (callback, Time::Duration{options.fFrequencyInfo.fRepeatInterval},
                                                                         Execution::IntervalTimer::Adder::eRunImmediately);
    if (options.fFollowNetworkChanges) {
        fNetworkChanges_ = SSDP::Private_::FollowNetworkChanges ([callback] () mutable { callback (); });
    }
}

PeriodicNotifier::~PeriodicNotifier ()
{
    Debug::TraceContextBumper ctx{"SSDP PeriodicNotifier - ssdp:byebye"};
    // no more ssdp:alive - with these gone, none is under way, nor to come - then ssdp:byebye, where they went
    fNetworkChanges_.reset ();
    fIntervalTimerAdder_.reset ();
    Execution::Thread::SuppressInterruptionInContext suppressInterruption; // a destructor: finish, even on a thread being aborted
    IgnoreExceptionsForCall (fSayByebye_ ());
}

InterfacesByID PeriodicNotifier::GetNetworkInterfaces () const
{
    return fNotifyingOn_->load ();
}
