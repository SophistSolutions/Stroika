/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <vector>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Bijection.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/WaitForIOReady.h"
#include "Stroika/Foundation/IO/Network/ConnectionlessSocket.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/ExternallyOwnedSpanInputStream.h"

#include "Stroika/Frameworks/UPnP/SSDP/Advertisement.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"

#include "Listener.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;
using namespace Stroika::Frameworks::UPnP::SSDP::Client;

// Comment this in to turn on tracing in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ****************************** Listener::Rep_ **********************************
 ********************************************************************************
 */
class Listener::Rep_ {
public:
    Rep_ (const Options& options)
        : fOptions_{options}
    {
        static constexpr Activity kConstructingSSDPListener_{"constructing SSDP Listener"sv};
        DeclareActivity           activity{&kConstructingSSDPListener_};
        MakeSockets_ ();
        if (options.fFollowNetworkChanges) {
            fNetworkChanges_ = SSDP::Private_::FollowNetworkChanges ([this] () { Rejoin_ (); });
        }
    }
    ~Rep_ () = default;
    CallbackID AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds)
    {
        return fFoundCallbacks_.Add (callOnFinds);
    }
    void RemoveOnFoundCallback (CallbackID callOnFinds)
    {
        fFoundCallbacks_.Remove (callOnFinds);
    }
    void Start ()
    {
        [[maybe_unused]] lock_guard lifecycle{fLifecycleMutex_};
        StartThread_ ();
    }
    void Stop ()
    {
        [[maybe_unused]] lock_guard lifecycle{fLifecycleMutex_};
        StopThread_ ();
    }
    InterfacesByID GetNetworkInterfaces () const
    {
        return fListeningOn_.load ();
    }
    // bound, and joined on every interface fOptions_.fInterfaces accepts - so notifications arriving on any of them are heard
    void MakeSockets_ ()
    {
        Socket::BindFlags bindFlags = Socket::BindFlags{};
        bindFlags.fSO_REUSEADDR     = true;
        if (InternetProtocol::IP::SupportIPV4 (fOptions_.fIPVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            s.SetReceivePacketInfo (true); // for Advertisement::fReceivedOn
            s.Bind (SocketAddress{Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()}, bindFlags);
            fSocketsAndGroups_ += make_pair (s, UPnP::SSDP::V4::kSocketAddress.GetInternetAddress ());
            fSockets_.Add (s);
        }
        if (InternetProtocol::IP::SupportIPV6 (fOptions_.fIPVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET6, Socket::DGRAM);
            s.SetReceivePacketInfo (true); // for Advertisement::fReceivedOn
            s.Bind (SocketAddress{Network::V6::kAddrAny, UPnP::SSDP::V6::kSocketAddress.GetPort ()}, bindFlags);
            fSocketsAndGroups_ += make_pair (s, UPnP::SSDP::V6::kSocketAddress.GetInternetAddress ());
            fSockets_.Add (s);
        }
        Join_ ();
    }
    // join SSDP's groups on every interface fOptions_.fInterfaces accepts - those joined already stay as they are
    void Join_ ()
    {
        fListeningOn_.store (SSDP::Private_::JoinOnEveryInterface (fSocketsAndGroups_, fOptions_.fInterfaces));
    }
    void StartThread_ ()
    {
        static const String kThreadName_ = "SSDP Listener"sv;

        fThread_ = Thread::New ([this] () { DoRun_ (); }, Thread::eAutoStart, kThreadName_);
    }
    void StopThread_ ()
    {
        if (fThread_ != nullptr) {
            fThread_.AbortAndWaitForDone ();
            fThread_ = nullptr;
        }
    }
    // a network appeared: listen there too - joined on the sockets already listening, so nothing waiting in them is lost or read
    // twice. By the listening thread - woken from its wait for packets (each socket used by one thread at a time), though not
    // from a slow callOnFinds - or here, if none is running
    void Rejoin_ ()
    {
        [[maybe_unused]] lock_guard lifecycle{fLifecycleMutex_};
        if (fThread_ != nullptr) {
            fJoinAgain_->Set ();
        }
        else {
            Join_ ();
        }
    }
    void DoRun_ ()
    {
        // only stopped by thread abort
        WaitForIOReady<ConnectionlessSocket::Ptr> readyChecker{fSockets_, WaitForIOReady<ConnectionlessSocket::Ptr>::kDefaultTypeOfMonitor,
                                                               fJoinAgain_->GetWaitInfo ()};
        while (true) {
            if (fJoinAgain_->IsSet ()) {
                fJoinAgain_->Clear (); // first: a network appearing as this joins wakes the wait below at once
                Join_ ();
            }
            for (const ConnectionlessSocket::Ptr& s : readyChecker.WaitQuietly ()) {
                try {
                    byte                                       buf[8 * 1024]; // not sure of max packet size
                    SocketAddress                              from;
                    optional<ConnectionlessSocket::PacketInfo> arrived;
                    size_t                                     nBytesRead = s.ReceiveFrom (buf, 0, &from, &arrived).size ();
                    Assert (nBytesRead <= std::size (buf));
                    ParsePacketAndNotifyCallbacks_ (span{buf, nBytesRead}, SSDP::Private_::ReceivedOn (arrived, fListeningOn_.load ()));
                }
                catch (const Thread::AbortException&) {
                    ReThrow ();
                }
                catch (...) {
                    // ignore errors - and keep on trucking
                    // but avoid wasting too much time if we get into an error storm
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("Caught/ignored exception for SSDP advertisement packet: {}"_f, current_exception ());
#endif
                    Execution::Sleep (1s);
                }
            }
        }
    }
    void ParsePacketAndNotifyCallbacks_ (span<const byte> packet, const optional<Interface::SystemIDType>& receivedOn)
    {
        String              headLine;
        SSDP::Advertisement d;
        SSDP::DeSerialize (Memory::BLOB{packet}, &headLine, &d);
        d.fReceivedOn = receivedOn;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"Read SSDP Packet"};
        DbgTrace ("headLine: {}"_f, headLine);
#endif
        if (headLine.StartsWith ("NOTIFY "sv)) {
            fFoundCallbacks_.Call (d);
        }
    }

private:
    const Options fOptions_;
    mutex         fLifecycleMutex_; // Start, Stop and Rejoin_ (called on its network-change thread)
    Execution::CallbackRegistry<void (const SSDP::Advertisement&)> fFoundCallbacks_;
    Containers::Sequence<pair<ConnectionlessSocket::Ptr, InternetAddress>> fSocketsAndGroups_; // each socket, and the SSDP group it joins - all set as constructed
    Collection<ConnectionlessSocket::Ptr> fSockets_;                                           // the same sockets
    Synchronized<InterfacesByID>          fListeningOn_;                                       // what fSockets_ are joined on
    unique_ptr<WaitForIOReady_Support::EventFD> fJoinAgain_{WaitForIOReady_Support::mkEventFD ()}; // set by Rejoin_, waking the listening thread (so declared before it)
    Thread::CleanupPtr                              fThread_{Thread::CleanupPtr::eAbortBeforeWaiting};
    optional<SSDP::Private_::NetworkChangeFollower> fNetworkChanges_; // last, so destroyed first: no Rejoin_ while the rest goes away
};

/*
 ********************************************************************************
 ************************************* Listener *********************************
 ********************************************************************************
 */
Listener::Listener (const Options& options)
    : fRep_{Memory::MakeSharedPtr<Rep_> (options)}
{
}

Listener::Listener (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const Options& options)
    : Listener{options}
{
    AddOnFoundCallback (callOnFinds);
}

Listener::Listener (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const Options& options, AutoStart)
    : Listener{callOnFinds, options}
{
    Start ();
}

Listener::Listener (const function<void (const SSDP::Advertisement& d)>& callOnFinds, AutoStart)
    : Listener{callOnFinds}
{
    Start ();
}

InterfacesByID Listener::GetNetworkInterfaces () const
{
    return fRep_->GetNetworkInterfaces ();
}

Listener::~Listener ()
{
    if (fRep_ != nullptr) { // (moved from, it has none)
        IgnoreExceptionsForCall (fRep_->Stop ());
    }
}

auto Listener::AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds) -> CallbackID
{
    return fRep_->AddOnFoundCallback (callOnFinds);
}

void Listener::RemoveOnFoundCallback (CallbackID callOnFinds)
{
    fRep_->RemoveOnFoundCallback (callOnFinds);
}

void Listener::Start ()
{
    fRep_->Start ();
}

void Listener::Stop ()
{
    fRep_->Stop ();
}
