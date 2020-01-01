/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include <vector>

#include "../../../../Foundation/Characters/String_Constant.h"
#include "../../../../Foundation/Characters/ToString.h"
#include "../../../../Foundation/Containers/Bijection.h"
#include "../../../../Foundation/Containers/Collection.h"
#include "../../../../Foundation/Debug/Trace.h"
#include "../../../../Foundation/Execution/Exceptions.h"
#include "../../../../Foundation/Execution/Sleep.h"
#include "../../../../Foundation/Execution/Thread.h"
#include "../../../../Foundation/Execution/WaitForIOReady.h"
#include "../../../../Foundation/IO/Network/ConnectionlessSocket.h"
#include "../../../../Foundation/IO/Network/WaitForSocketIOReady.h"
#include "../../../../Foundation/Streams/ExternallyOwnedMemoryInputStream.h"
#include "../../../../Foundation/Streams/TextReader.h"
#include "../Advertisement.h"
#include "../Common.h"

#include "Listener.h"

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

/*
 ********************************************************************************
 ****************************** Listener::Rep_ **********************************
 ********************************************************************************
 */
class Listener::Rep_ {
public:
    Rep_ (IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
    {
        static constexpr Execution::Activity kConstructingSSDPListener_{L"constucting SSDP Listener"sv};
        Execution::DeclareActivity           activity{&kConstructingSSDPListener_};
        Socket::BindFlags                    bindFlags = Socket::BindFlags ();
        bindFlags.fReUseAddr                           = true;
        if (InternetProtocol::IP::SupportIPV4 (ipVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            s.Bind (SocketAddress (Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()), bindFlags);
            s.JoinMulticastGroup (UPnP::SSDP::V4::kSocketAddress.GetInternetAddress ());
            fSockets_.Add (s);
        }
        if (InternetProtocol::IP::SupportIPV6 (ipVersion)) {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET6, Socket::DGRAM);
            s.Bind (SocketAddress (Network::V6::kAddrAny, UPnP::SSDP::V6::kSocketAddress.GetPort ()), bindFlags);
            s.JoinMulticastGroup (UPnP::SSDP::V6::kSocketAddress.GetInternetAddress ());
            fSockets_.Add (s);
        }
    }
    ~Rep_ () = default;
    void AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds)
    {
        [[maybe_unused]] auto&& critSec = lock_guard{fCritSection_};
        fFoundCallbacks_.push_back (callOnFinds);
    }
    void Start ()
    {
        static const String kThreadName_ = L"SSDP Listener"sv;
        fThread_                         = Execution::Thread::New (
            [this] () { DoRun_ (); }, Execution::Thread::eAutoStart, kThreadName_);
    }
    void Stop ()
    {
        if (fThread_ != nullptr) {
            fThread_.AbortAndWaitForDone ();
            fThread_ = nullptr;
        }
    }
    void DoRun_ ()
    {
        // only stopped by thread abort
        WaitForSocketIOReady<ConnectionlessSocket::Ptr> readyChecker{fSockets_};
        while (true) {
            for (ConnectionlessSocket::Ptr s : readyChecker.Wait ()) {
                try {
                    byte          buf[8 * 1024]; // not sure of max packet size
                    SocketAddress from;
                    size_t        nBytesRead = s.ReceiveFrom (std::begin (buf), std::end (buf), 0, &from);
                    Assert (nBytesRead <= NEltsOf (buf));
                    using namespace Streams;
                    ParsePacketAndNotifyCallbacks_ (TextReader::New (ExternallyOwnedMemoryInputStream<byte>::New (std::begin (buf), std::begin (buf) + nBytesRead)));
                }
                catch (const Execution::Thread::AbortException&) {
                    Execution::ReThrow ();
                }
                catch (...) {
// ignore errors - and keep on trucking
// but avoid wasting too much time if we get into an error storm
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"Caught/ignored exception for SSDP advertisement packet: %s", Characters::ToString (current_exception ()).c_str ());
#endif
                    Execution::Sleep (1s);
                }
            }
        }
    }
    void ParsePacketAndNotifyCallbacks_ (Streams::InputStream<Character>::Ptr in)
    {
        String firstLine = in.ReadLine ().Trim ();

#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx ("Read SSDP Packet");
        DbgTrace (L"firstLine: %s", firstLine.c_str ());
#endif
        const String kNOTIFY_LEAD = L"NOTIFY "sv;
        if (firstLine.length () > kNOTIFY_LEAD.length () and firstLine.SubString (0, kNOTIFY_LEAD.length ()) == kNOTIFY_LEAD) {
            SSDP::Advertisement d;
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
                    d.fRawHeaders.Add (label, value);
                }
                constexpr auto kLabelComparer_ = String::ThreeWayComparer{Characters::CompareOptions::eCaseInsensitive};
                if (kLabelComparer_ (label, L"Location") == 0) {
                    d.fLocation = IO::Network::URI{value};
                }
                else if (kLabelComparer_ (label, L"NT") == 0) {
                    d.fTarget = value;
                }
                else if (kLabelComparer_ (label, L"USN") == 0) {
                    d.fUSN = value;
                }
                else if (kLabelComparer_ (label, L"Server") == 0) {
                    d.fServer = value;
                }
                else if (kLabelComparer_ (label, L"NTS") == 0) {
                    constexpr auto kValueComparer_ = String::ThreeWayComparer{Characters::CompareOptions::eCaseInsensitive};
                    if (kValueComparer_ (value, L"ssdp:alive") == 0) {
                        d.fAlive = true;
                    }
                    else if (kValueComparer_ (value, L"ssdp:byebye") == 0) {
                        d.fAlive = false;
                    }
                }
            }

            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCritSection_};
                for (auto i : fFoundCallbacks_) {
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
 ************************************* Listener *********************************
 ********************************************************************************
 */
Listener::Listener (IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
    : fRep_ (make_shared<Rep_> (ipVersion))
{
}

Listener::Listener (const function<void (const SSDP::Advertisement& d)>& callOnFinds, IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
    : Listener (ipVersion)
{
    AddOnFoundCallback (callOnFinds);
}

Listener::Listener (const function<void (const SSDP::Advertisement& d)>& callOnFinds, IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion, AutoStart)
    : Listener (callOnFinds, ipVersion)
{
    Start ();
}

Listener::Listener (const function<void (const SSDP::Advertisement& d)>& callOnFinds, AutoStart)
    : Listener (callOnFinds)
{
    Start ();
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
