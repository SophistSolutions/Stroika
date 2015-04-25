/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    <vector>

#include    "../../../../Foundation/Characters/String_Constant.h"
#include    "../../../../Foundation/Debug/Trace.h"
#include    "../../../../Foundation/Execution/ErrNoException.h"
#include    "../../../../Foundation/Execution/Sleep.h"
#include    "../../../../Foundation/Execution/Thread.h"
#include    "../../../../Foundation/IO/Network/Socket.h"
#include    "../../../../Foundation/Streams/ExternallyOwnedMemoryBinaryInputStream.h"
#include    "../../../../Foundation/Streams/TextInputStreamBinaryAdapter.h"
#include    "../Advertisement.h"
#include    "../Common.h"

#include    "Listener.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;
using   namespace   Stroika::Frameworks::UPnP::SSDP;
using   namespace   Stroika::Frameworks::UPnP::SSDP::Client;

using   Execution::make_unique_lock;




// Comment this in to turn on tracing in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1



/*
 ********************************************************************************
 ********************************** Listener::Rep_ ******************************
 ********************************************************************************
 */
class   Listener::Rep_ {
public:
    Rep_ ()
        : fCritSection_ ()
        , fFoundCallbacks_ ()
        , fSocket_ (Socket::SocketKind::DGRAM)
        , fThread_ ()
    {
        Socket::BindFlags   bindFlags   =   Socket::BindFlags ();
        bindFlags.fReUseAddr = true;
        fSocket_.Bind (SocketAddress (Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()), bindFlags);
        fSocket_.JoinMulticastGroup (UPnP::SSDP::V4::kSocketAddress.GetInternetAddress ());
    }
    ~Rep_ ()
    {
        // critical we wait for finish of thread cuz it has bare 'this' pointer captured
        Execution::Thread::SuppressInterruptionInContext  suppressInterruption;
        fThread_.AbortAndWaitForDone ();
    }
    void    AddOnFoundCallback (const function<void(const SSDP::Advertisement& d)>& callOnFinds)
    {
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
        auto    critSec { make_unique_lock (fCritSection_) };
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");
        fFoundCallbacks_.push_back (callOnFinds);
    }
    void    Start ()
    {
        fThread_ = Execution::Thread ([this] () { DoRun_ (); });
        fThread_.SetThreadName (L"SSDP Listener");
        fThread_.Start ();
    }
    void    Stop ()
    {
        fThread_.AbortAndWaitForDone ();
    }
    void    DoRun_ ()
    {
        // only stopped by thread abort
        while (1) {
            try {
                Byte    buf[3 * 1024];  // not sure of max packet size
                SocketAddress   from;
                size_t nBytesRead = fSocket_.ReceiveFrom (std::begin (buf), std::end (buf), 0, &from);
                Assert (nBytesRead <= NEltsOf (buf));
                using   namespace   Streams;
                ParsePacketAndNotifyCallbacks_ (TextInputStreamBinaryAdapter (ExternallyOwnedMemoryBinaryInputStream (std::begin (buf), std::begin (buf) + nBytesRead)));
            }
            catch (const Execution::Thread::AbortException&) {
                Execution::DoReThrow ();
            }
            catch (...) {
                // ignore errors - and keep on trucking
                // but avoid wasting too much time if we get into an error storm
                Execution::Sleep (1.0);
            }
        }
    }
    void    ParsePacketAndNotifyCallbacks_ (Streams::TextInputStream in)
    {
        String firstLine    =   in.ReadLine ().Trim ();

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx ("Read SSDP Packet");
        DbgTrace (L"(firstLine: %s)", firstLine.c_str ());
#endif
        const   String  kNOTIFY_LEAD    =   String_Constant (L"NOTIFY ");
        if (firstLine.length () > kNOTIFY_LEAD.length () and firstLine.SubString (0, kNOTIFY_LEAD.length ()) == kNOTIFY_LEAD) {
            SSDP::Advertisement d;
            while (true) {
                String line =   in.ReadLine ().Trim ();
                if (line.empty ()) {
                    break;
                }

                // Need to simplify this code (stroika string util)
                String  label;
                String  value;
                {
                    size_t n = line.Find (':');
                    if (n != Characters::String::kBadIndex) {
                        label = line.SubString (0, n);
                        value = line.SubString (n + 1).Trim ();
                    }
                }
                if (not label.empty ()) {
                    d.fRawHeaders.Add (label, value);
                }
                if (label.Compare (L"Location", Characters::CompareOptions::eCaseInsensitive) == 0) {
                    d.fLocation = value;
                }
                else if (label.Compare (L"NT", Characters::CompareOptions::eCaseInsensitive) == 0) {
                    d.fTarget = value;
                }
                else if (label.Compare (L"USN", Characters::CompareOptions::eCaseInsensitive) == 0) {
                    d.fUSN = value;
                }
                else if (label.Compare (L"Server", Characters::CompareOptions::eCaseInsensitive) == 0) {
                    d.fServer = value;
                }
                else if (label.Compare (L"NTS", Characters::CompareOptions::eCaseInsensitive) == 0) {
                    if (value.Compare (L"ssdp:alive", Characters::CompareOptions::eCaseInsensitive) == 0) {
                        d.fAlive = true;
                    }
                    else if (value.Compare (L"ssdp:bye", Characters::CompareOptions::eCaseInsensitive) == 0) {
                        d.fAlive = false;
                    }
                }
                else {
                    int hreab = 1;
                }
            }

            {
                DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
                auto    critSec { make_unique_lock (fCritSection_) };
                DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");
                for (auto i : fFoundCallbacks_) {
                    i (d);
                }
            }
        }
    }
private:
    recursive_mutex                                             fCritSection_;
    vector<function<void (const SSDP::Advertisement& d)>>       fFoundCallbacks_;
    Socket                                                      fSocket_;
    Execution::Thread                                           fThread_;
};






/*
 ********************************************************************************
 ************************************* Listener *********************************
 ********************************************************************************
 */
Listener::Listener ()
    : fRep_ (new Rep_ ())
{
}

Listener::~Listener ()
{
    IgnoreExceptionsForCall (fRep_->Stop ());
}

void    Listener::AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds)
{
    fRep_->AddOnFoundCallback (callOnFinds);
}

void    Listener::Start ()
{
    fRep_->Start ();
}

void    Listener::Stop ()
{
    fRep_->Stop ();
}

