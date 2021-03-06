/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include <sstream>

#include "../../../../Foundation/Containers/Collection.h"
#include "../../../../Foundation/Debug/Trace.h"
#include "../../../../Foundation/Execution/Activity.h"
#include "../../../../Foundation/Execution/Exceptions.h"
#include "../../../../Foundation/Execution/Sleep.h"
#include "../../../../Foundation/Execution/Thread.h"
#include "../../../../Foundation/Execution/WaitForIOReady.h"
#include "../../../../Foundation/IO/Network/ConnectionlessSocket.h"
#include "../../../../Foundation/Streams/ExternallyOwnedMemoryInputStream.h"
#include "../../../../Foundation/Streams/TextReader.h"
#include "../Common.h"

#include "Search.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;
using namespace Stroika::Frameworks::UPnP::SSDP::Client;

// Comment this in to turn on tracing in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

class Search::Rep_ {
public:
    Rep_ (IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
    {
        static constexpr Execution::Activity kConstructingSSDPSearcher_{L"constucting SSDP searcher"sv};
        Execution::DeclareActivity           activity{&kConstructingSSDPSearcher_};
        if (InternetProtocol::IP::SupportIPV4 (ipVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            fSockets_.Add (s);
        }
        if (InternetProtocol::IP::SupportIPV6 (ipVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET6, Socket::DGRAM);
            fSockets_.Add (s);
        }
        for (ConnectionlessSocket::Ptr cs : fSockets_) {
            cs.SetMulticastLoopMode (true); // possible should make this configurable
        }
    }
    ~Rep_ () = default;
    void AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds)
    {
        [[maybe_unused]] auto&& critSec = lock_guard{fCritSection_};
        fFoundCallbacks_.push_back (callOnFinds);
    }
    void Start (const String& serviceType, const optional<Time::Duration>& autoRetryInterval)
    {
        if (fThread_ != nullptr) {
            fThread_.AbortAndWaitForDone ();
        }
        fThread_ = Execution::Thread::New ([this, serviceType, autoRetryInterval] () { DoRun_ (serviceType, autoRetryInterval); }, Execution::Thread::eAutoStart, L"SSDP Searcher"sv);
    }
    void Stop ()
    {
        if (fThread_ != nullptr) {
            fThread_.AbortAndWaitForDone ();
        }
    }
    void DoRun_ (const String& serviceType, const optional<Time::Duration>& autoRetryInterval)
    {
        bool didFirstRetry = false; // because search unreliable/UDP, recomended to send two search requests
                                    // a little bit apart even if addition to longer retry interval
                                    // http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf
    Retry:
        optional<Time::DurationSecondsType> retrySendAt;
        if (not didFirstRetry) {
            retrySendAt   = Time::GetTickCount () + 2;
            didFirstRetry = true;
        }
        else if (autoRetryInterval.has_value ()) {
            retrySendAt = Time::GetTickCount () + autoRetryInterval->As<Time::DurationSecondsType> ();
        }
        for (ConnectionlessSocket::Ptr s : fSockets_) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Sending M-SEARCH");
#endif
            SocketAddress useSocketAddress = s.GetAddressFamily () == SocketAddress::INET ? SSDP::V4::kSocketAddress : SSDP::V6::kSocketAddress;
            string        request;
            {
                /*
                 *  From http://www.upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.0-20080424.pdf:
                 *      To limit network congestion, the time-to-live (TTL) of each IP packet for each multicast
                 *      message should default to 4 and should be configurable. 
                 */
                const unsigned int kMaxHops_ = 4;
                stringstream       requestBuf;
                requestBuf << "M-SEARCH * HTTP/1.1\r\n";
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
                requestBuf << "Host: " << hostAuthority.As<String> ().AsUTF8 () << "\r\n";
                requestBuf << "Man: \"ssdp:discover\"\r\n";
                requestBuf << "ST: " << serviceType.AsUTF8 ().c_str () << "\r\n";
                requestBuf << "MX: " << kMaxHops_ << "\r\n";
                requestBuf << "\r\n";
                request = requestBuf.str ();
                s.SetMulticastTTL (kMaxHops_);
            }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("DETAILS: %s", request.c_str ());
#endif
            s.SendTo (reinterpret_cast<const byte*> (request.c_str ()), reinterpret_cast<const byte*> (request.c_str () + request.length ()), useSocketAddress);
        }

        // only stopped by thread abort (which we PROBALY SHOULD FIX - ONLY SEARCH FOR CONFIRABLE TIMEOUT???)
        Execution::WaitForIOReady<ConnectionlessSocket::Ptr> readyChecker{fSockets_};
        while (1) {
            for (ConnectionlessSocket::Ptr s : readyChecker.WaitQuietlyUntil (retrySendAt.value_or (Time::kInfinite))) {
                try {
                    byte          buf[8 * 1024]; // not sure of max packet size
                    SocketAddress from;
                    size_t        nBytesRead = s.ReceiveFrom (std::begin (buf), std::end (buf), 0, &from);
                    Assert (nBytesRead <= NEltsOf (buf));
                    using namespace Streams;
                    ReadPacketAndNotifyCallbacks_ (TextReader::New (ExternallyOwnedMemoryInputStream<byte>::New (std::begin (buf), std::begin (buf) + nBytesRead)));
                }
                catch (const Execution::Thread::AbortException&) {
                    Execution::ReThrow ();
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
    void ReadPacketAndNotifyCallbacks_ (const Streams::InputStream<Character>::Ptr& in)
    {
        String firstLine = in.ReadLine ().Trim ();

#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx ("Read Reply");
        DbgTrace (L"firstLine: %s", firstLine.c_str ());
#endif

        static const String kOKRESPONSELEAD_ = L"HTTP/1.1 200"sv;
        if (firstLine.length () >= kOKRESPONSELEAD_.length () and firstLine.SubString (0, kOKRESPONSELEAD_.length ()) == kOKRESPONSELEAD_) {
            SSDP::Advertisement d;
            while (true) {
                String line = in.ReadLine ().Trim ();
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"reply-line: %s", line.c_str ());
#endif
                if (line.empty ()) {
                    break;
                }

                // Need to simplify this code (stroika string util)
                if (optional<size_t> n = line.Find (':')) {
                    String label = line.SubString (0, *n);
                    String value = line.SubString (*n + 1).Trim ();
                    if (String::ThreeWayComparer{Characters::CompareOptions::eCaseInsensitive}(label, L"Location") == 0) {
                        d.fLocation = IO::Network::URI{value};
                    }
                    else if (String::ThreeWayComparer{Characters::CompareOptions::eCaseInsensitive}(label, L"ST") == 0) {
                        d.fTarget = value;
                    }
                    else if (String::ThreeWayComparer{Characters::CompareOptions::eCaseInsensitive}(label, L"USN") == 0) {
                        d.fUSN = value;
                    }
                    else if (String::ThreeWayComparer{Characters::CompareOptions::eCaseInsensitive}(label, L"Server") == 0) {
                        d.fServer = value;
                    }
                }
            }
            {
                // bad practice to keep mutex lock here - DEADLOCK CITY - find nice CLEAN way todo this...
                [[maybe_unused]] auto&& critSec = lock_guard{fCritSection_};
                for (const auto& i : fFoundCallbacks_) {
                    i (d);
                }
            }
        }
    }

private:
    recursive_mutex                                       fCritSection_;
    vector<function<void (const SSDP::Advertisement& d)>> fFoundCallbacks_;
    Collection<ConnectionlessSocket::Ptr>                 fSockets_;
    Execution::Thread::CleanupPtr                         fThread_{Execution::Thread::CleanupPtr::eAbortBeforeWaiting};
};

/*
 ********************************************************************************
 ********************************** Search **************************************
 ********************************************************************************
 */
const String Search::kSSDPAny    = L"ssdp:any"sv;
const String Search::kRootDevice = L"upnp:rootdevice"sv;

Search::Search (IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
    : fRep_ (make_shared<Rep_> (ipVersion))
{
}

Search::Search (const function<void (const SSDP::Advertisement& d)>& callOnFinds, IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
    : Search (ipVersion)
{
    AddOnFoundCallback (callOnFinds);
}

Search::Search (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const String& initialSearch, IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
    : Search (callOnFinds, ipVersion)
{
    Start (initialSearch);
}

Search::Search (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const String& initialSearch, const optional<Time::Duration>& autoRetryInterval, IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
    : Search (callOnFinds, ipVersion)
{
    Start (initialSearch, autoRetryInterval);
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
