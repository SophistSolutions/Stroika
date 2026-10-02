/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <sstream>

#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/WaitForIOReady.h"
#include "Stroika/Foundation/IO/Network/ConnectionlessSocket.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/ExternallyOwnedSpanInputStream.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"

#include "Search.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using Memory::MakeSharedPtr;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;
using namespace Stroika::Frameworks::UPnP::SSDP::Client;

// Comment this in to turn on tracing in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

class Search::Rep_ final {
public:
    Rep_ (const Options& options)
        : fInterfaceFilter_{options.fInterfaces}
    {
        static constexpr Activity kConstructingSSDPSearcher_{"constructing SSDP searcher"sv};
        DeclareActivity           activity{&kConstructingSSDPSearcher_};
        if (InternetProtocol::IP::SupportIPV4 (options.fIPVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            fSockets_.Add (s);
        }
        if (InternetProtocol::IP::SupportIPV6 (options.fIPVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET6, Socket::DGRAM);
            fSockets_.Add (s);
        }
        for (ConnectionlessSocket::Ptr cs : fSockets_) {
            cs.SetMulticastLoopMode (true); // possible should make this configurable
        }
        if (options.fFollowNetworkChanges) {
            fLinkMonitor_ = SSDP::Private_::FollowNetworkChanges ([this] () { SearchAgain_ (); });
        }
    }
    ~Rep_ () = default;
    void AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds)
    {
        [[maybe_unused]] lock_guard critSec{fCritSection_};
        fFoundCallbacks_.push_back (callOnFinds);
    }
    void Start (const String& serviceType, const optional<Time::Duration>& autoRetryInterval)
    {
        [[maybe_unused]] lock_guard lifecycle{fLifecycleMutex_};
        StartThread_ (serviceType, autoRetryInterval);
    }
    InterfacesByID GetNetworkInterfaces () const
    {
        return fSearchingOn_.load ();
    }
    void Stop ()
    {
        [[maybe_unused]] lock_guard lifecycle{fLifecycleMutex_};
        if (fThread_ != nullptr) {
            fThread_.AbortAndWaitForDone ();
        }
        fSearching_ = nullopt;
    }
    void StartThread_ (const String& serviceType, const optional<Time::Duration>& autoRetryInterval)
    {
        if (fThread_ != nullptr) {
            fThread_.AbortAndWaitForDone ();
        }
        fSearching_ = make_pair (serviceType, autoRetryInterval);
        fThread_ = Thread::New ([this, serviceType, autoRetryInterval] () { DoRun_ (serviceType, autoRetryInterval); }, Thread::eAutoStart, "SSDP Searcher"sv);
    }
    // a network appeared: search there right away, rather than at the next retry - by starting the search over (it sends out
    // of every interface as it starts)
    void SearchAgain_ ()
    {
        [[maybe_unused]] lock_guard lifecycle{fLifecycleMutex_};
        if (fSearching_) {
            StartThread_ (fSearching_->first, fSearching_->second);
        }
    }
    void DoRun_ (const String& serviceType, const optional<Time::Duration>& autoRetryInterval)
    {
        bool didFirstRetry = false; // because search unreliable/UDP, recommended to send two search requests
                                    // a little bit apart even if addition to longer retry interval
                                    // http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf
    Retry:
        optional<Time::TimePointSeconds> retrySendAt;
        if (not didFirstRetry) {
            retrySendAt   = Time::GetTickCount () + 2s;
            didFirstRetry = true;
        }
        else if (autoRetryInterval.has_value ()) {
            retrySendAt = Time::GetTickCount () + *autoRetryInterval;
        }
        const InterfacesByID candidates = SSDP::Private_::GetSSDPInterfaces (fInterfaceFilter_); // listed once for the round
        InterfacesByID       searchedOn;
        for (ConnectionlessSocket::Ptr s : fSockets_) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"Sending M-SEARCH"sv};
#endif
            SocketAddress useSocketAddress = s.GetAddressFamily () == SocketAddress::INET ? SSDP::V4::kSocketAddress : SSDP::V6::kSocketAddress;
            string request;
            {
                /*
                 *  From http://www.upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.0-20080424.pdf:
                 *      To limit network congestion, the time-to-live (TTL) of each IP packet for each multicast
                 *      message should default to 4 and should be configurable. 
                 */
                const unsigned int kMaxHops_ = 4;
                stringstream       requestBuf;
                requestBuf << "M-SEARCH * HTTP/1.1\r\n"sv;
                UniformResourceIdentification::Authority hostAuthority = [&] () -> UniformResourceIdentification::Authority {
                    switch (s.GetAddressFamily ()) {
                        case SocketAddress::FamilyType::INET: {
                            return UniformResourceIdentification::Authority{SSDP::V4::kSocketAddress.GetInternetAddress (), useSocketAddress.GetPort ()};
                        } break;
                        case SocketAddress::FamilyType::INET6: {
                            return UniformResourceIdentification::Authority{SSDP::V6::kSocketAddress.GetInternetAddress (), useSocketAddress.GetPort ()};
                        } break;
                        default:
                            AssertNotReached ();
                            return UniformResourceIdentification::Authority{};
                    }
                }();
                requestBuf << "Host: "sv << hostAuthority.As<String> ().AsUTF8<string> () << "\r\n";
                requestBuf << "Man: \"ssdp:discover\"\r\n"sv;
                requestBuf << "ST: "sv << serviceType.AsUTF8<string> ().c_str () << "\r\n";
                requestBuf << "MX: "sv << kMaxHops_ << "\r\n";
                requestBuf << "\r\n"sv;
                request = requestBuf.str ();
                s.SetMulticastTTL (kMaxHops_);
            }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("DETAILS: {}"_f, request);
#endif
            // out of each interface with an address of this socket's family - so devices on every network hear it (the answers
            // all come back to this one socket)
            const span<const byte>         data{reinterpret_cast<const byte*> (request.c_str ()), request.length ()};
            InternetAddress::AddressFamily family = useSocketAddress.GetInternetAddress ().GetAddressFamily ();
            for (const Interface& i : candidates) {
                if (not i.fBindings.fAddresses.Any ([&] (const InternetAddress& a) { return a.GetAddressFamily () == family; })) {
                    continue;
                }
                try {
                    s.SetMulticastInterface (i);
                    s.SendTo (data, useSocketAddress);
                    searchedOn.Add (i);
                }
                catch (const Thread::AbortException&) {
                    ReThrow ();
                }
                catch (...) {
                    DbgTrace ("SSDP Search: could not send M-SEARCH on {}: {}"_f, i.fInterfaceID, current_exception ());
                }
            }
        }

        fSearchingOn_.store (searchedOn);

        // only stopped by thread abort (which we PROBALY SHOULD FIX - ONLY SEARCH FOR CONFIRABLE TIMEOUT???)
        WaitForIOReady<ConnectionlessSocket::Ptr> readyChecker{fSockets_};
        while (1) {
            for (ConnectionlessSocket::Ptr s : readyChecker.WaitQuietlyUntil (retrySendAt.value_or (Time::TimePointSeconds{Time::kInfinity}))) {
                try {
                    byte          buf[8 * 1024]; // not sure of max packet size
                    SocketAddress from;
                    size_t        nBytesRead = s.ReceiveFrom (buf, 0, &from).size ();
                    Assert (nBytesRead <= std::size (buf));
                    ReadPacketAndNotifyCallbacks_ (span{buf, nBytesRead});
                }
                catch (const Thread::AbortException&) {
                    ReThrow ();
                }
                catch (...) {
                    // ignore errors - and keep on trucking
                    // but avoid wasting too much time if we get into an error storm
                    Execution::Sleep (1s);
                }
            }
            if (retrySendAt and *retrySendAt < Time::GetTickCount ()) {
                goto Retry;
            }
        }
    }
    void ReadPacketAndNotifyCallbacks_ (span<const byte> packet)
    {
        String              headLine;
        SSDP::Advertisement d;
        SSDP::DeSerialize (Memory::BLOB{packet}, &headLine, &d);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"Read Reply"};
        DbgTrace ("headLine: {}"_f, headLine);
#endif
        if (headLine.StartsWith ("HTTP/1.1 200"sv)) {
            // bad practice to keep mutex lock here - DEADLOCK CITY - find nice CLEAN way todo this...
            [[maybe_unused]] lock_guard critSec{fCritSection_};
            for (const auto& i : fFoundCallbacks_) {
                i (d);
            }
        }
    }

private:
    recursive_mutex                                       fCritSection_;
    vector<function<void (const SSDP::Advertisement& d)>> fFoundCallbacks_;
    InterfaceFilter                                       fInterfaceFilter_;
    mutex                                            fLifecycleMutex_; // Start, Stop and SearchAgain_ (called on the LinkMonitor's thread)
    optional<pair<String, optional<Time::Duration>>> fSearching_; // the search started, and not stopped: serviceType, autoRetryInterval
    Collection<ConnectionlessSocket::Ptr>            fSockets_;
    Synchronized<InterfacesByID>                     fSearchingOn_; // what the last M-SEARCH went out of
    Thread::CleanupPtr                               fThread_{Thread::CleanupPtr::eAbortBeforeWaiting};
    optional<IO::Network::LinkMonitor>               fLinkMonitor_; // last, so destroyed first: no SearchAgain_ while the rest goes away
};

/*
 ********************************************************************************
 ********************************** Search **************************************
 ********************************************************************************
 */
const String Search::kSSDPAny    = SSDP::kTarget_SSDPAll;
const String Search::kRootDevice = "upnp:rootdevice"sv;

Search::Search (const Options& options)
    : fRep_{MakeSharedPtr<Rep_> (options)}
{
}

Search::Search (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const Options& options)
    : Search{options}
{
    AddOnFoundCallback (callOnFinds);
}

Search::Search (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const String& initialSearch, const Options& options)
    : Search{callOnFinds, options}
{
    Start (initialSearch);
}

Search::Search (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const String& initialSearch,
                const optional<Time::Duration>& autoRetryInterval, const Options& options)
    : Search{callOnFinds, options}
{
    Start (initialSearch, autoRetryInterval);
}

InterfacesByID Search::GetNetworkInterfaces () const
{
    return fRep_->GetNetworkInterfaces ();
}

Search::~Search ()
{
    IgnoreExceptionsForCall (fRep_->Stop ());
}

void Search::AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds)
{
    fRep_->AddOnFoundCallback (callOnFinds);
}

void Search::Start (const String& serviceType, const optional<Time::Duration>& autoRetryInterval)
{
    fRep_->Start (serviceType, autoRetryInterval);
}

void Search::Stop ()
{
    fRep_->Stop ();
}
