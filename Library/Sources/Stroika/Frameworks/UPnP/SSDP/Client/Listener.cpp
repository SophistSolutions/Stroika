/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <vector>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Bijection.h"
#include "Stroika/Foundation/Containers/Collection.h"
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
        fSockets_ = MakeSockets_ ();
        if (options.fFollowNetworkChanges) {
            fLinkMonitor_ = SSDP::Private_::FollowNetworkChanges ([this] () { Rejoin_ (); });
        }
    }
    ~Rep_ () = default;
    void AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds)
    {
        [[maybe_unused]] lock_guard critSec{fCritSection_};
        fFoundCallbacks_.push_back (callOnFinds);
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
    Collection<ConnectionlessSocket::Ptr> MakeSockets_ ()
    {
        Socket::BindFlags bindFlags = Socket::BindFlags{};
        bindFlags.fSO_REUSEADDR     = true;
        Collection<ConnectionlessSocket::Ptr>                                  sockets;
        Containers::Sequence<pair<ConnectionlessSocket::Ptr, InternetAddress>> toJoin;
        if (InternetProtocol::IP::SupportIPV4 (fOptions_.fIPVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            s.Bind (SocketAddress{Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()}, bindFlags);
            toJoin += make_pair (s, UPnP::SSDP::V4::kSocketAddress.GetInternetAddress ());
            sockets.Add (s);
        }
        if (InternetProtocol::IP::SupportIPV6 (fOptions_.fIPVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET6, Socket::DGRAM);
            s.Bind (SocketAddress{Network::V6::kAddrAny, UPnP::SSDP::V6::kSocketAddress.GetPort ()}, bindFlags);
            toJoin += make_pair (s, UPnP::SSDP::V6::kSocketAddress.GetInternetAddress ());
            sockets.Add (s);
        }
        fListeningOn_.store (SSDP::Private_::JoinOnEveryInterface (toJoin, fOptions_.fInterfaces));
        return sockets;
    }
    void StartThread_ ()
    {
        static const String kThreadName_ = "SSDP Listener"sv;
        fThread_                         = Thread::New ([this] () { DoRun_ (); }, Thread::eAutoStart, kThreadName_);
    }
    void StopThread_ ()
    {
        if (fThread_ != nullptr) {
            fThread_.AbortAndWaitForDone ();
            fThread_ = nullptr;
        }
    }
    // a network appeared: listen there too - on new sockets, joined afresh (the listening thread uses the sockets, so it stops
    // meanwhile)
    void Rejoin_ ()
    {
        [[maybe_unused]] lock_guard lifecycle{fLifecycleMutex_};
        bool                        wasRunning = fThread_ != nullptr;
        StopThread_ ();
        fSockets_ = MakeSockets_ ();
        if (wasRunning) {
            StartThread_ ();
        }
    }
    void DoRun_ ()
    {
        // only stopped by thread abort
        WaitForIOReady<ConnectionlessSocket::Ptr> readyChecker{fSockets_};
        while (true) {
            for (const ConnectionlessSocket::Ptr& s : readyChecker.Wait ()) {
                try {
                    byte          buf[8 * 1024]; // not sure of max packet size
                    SocketAddress from;
                    size_t        nBytesRead = s.ReceiveFrom (buf, 0, &from).size ();
                    Assert (nBytesRead <= std::size (buf));
                    ParsePacketAndNotifyCallbacks_ (span{buf, nBytesRead});
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
    void ParsePacketAndNotifyCallbacks_ (span<const byte> packet)
    {
        String              headLine;
        SSDP::Advertisement d;
        SSDP::DeSerialize (Memory::BLOB{packet}, &headLine, &d);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"Read SSDP Packet"};
        DbgTrace ("headLine: {}"_f, headLine);
#endif
        if (headLine.StartsWith ("NOTIFY "sv)) {
            [[maybe_unused]] lock_guard critSec{fCritSection_};
            for (const auto& i : fFoundCallbacks_) {
                i (d);
            }
        }
    }

private:
    const Options                                         fOptions_;
    mutex                                                 fLifecycleMutex_; // Start, Stop and Rejoin_ (called on the LinkMonitor's thread)
    recursive_mutex                                       fCritSection_;
    vector<function<void (const SSDP::Advertisement& d)>> fFoundCallbacks_;
    Collection<ConnectionlessSocket::Ptr>                 fSockets_;
    Synchronized<InterfacesByID>                          fListeningOn_; // what fSockets_ are joined on
    Thread::CleanupPtr                                    fThread_{Thread::CleanupPtr::eAbortBeforeWaiting};
    optional<IO::Network::LinkMonitor>                    fLinkMonitor_; // last, so destroyed first: no Rejoin_ while the rest goes away
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
    IgnoreExceptionsForCall (fRep_->Stop ());
}

void Listener::AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds)
{
    fRep_->AddOnFoundCallback (callOnFinds);
}

void Listener::Start ()
{
    fRep_->Start ();
}

void Listener::Stop ()
{
    fRep_->Stop ();
}
