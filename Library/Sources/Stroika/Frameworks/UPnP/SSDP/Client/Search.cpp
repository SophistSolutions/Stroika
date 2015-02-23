/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    <sstream>

#include    "../../../../Foundation/Characters/String_Constant.h"
#include    "../../../../Foundation/Debug/Trace.h"
#include    "../../../../Foundation/Execution/ErrNoException.h"
#include    "../../../../Foundation/Execution/Sleep.h"
#include    "../../../../Foundation/Execution/Thread.h"
#include    "../../../../Foundation/IO/Network/Socket.h"
#include    "../../../../Foundation/Streams/ExternallyOwnedMemoryBinaryInputStream.h"
#include    "../../../../Foundation/Streams/TextInputStreamBinaryAdapter.h"
#include    "../Common.h"

#include    "Search.h"


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




class   Search::Rep_ {
public:
    Rep_ ()
        : fCritSection_ ()
        , fFoundCallbacks_ ()
        , fSocket_ (Socket::SocketKind::DGRAM)
        , fThread_ ()
    {
    }
    ~Rep_ ()
    {
        // critical we wait for finish of thread cuz it has bare 'this' pointer captured
        Execution::Thread::SuppressInteruptionInContext  suppressInteruption;
        IgnoreExceptionsForCall (fThread_.AbortAndWaitForDone ());
    }
    void    AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds)
    {
        auto    critSec { make_unique_lock (fCritSection_) };
        fFoundCallbacks_.push_back (callOnFinds);
    }
    void    Start (const String& serviceType)
    {
        fThread_ = Execution::Thread ([this, serviceType] () { DoRun_ (serviceType); });
        fThread_.SetThreadName (L"SSDP Searcher");
        fThread_.Start ();
    }
    void    Stop ()
    {
        fThread_.AbortAndWaitForDone ();
    }
    void    DoRun_ (const String& serviceType)
    {
        fSocket_.SetMulticastLoopMode (true);       // probably should make this configurable

        /// MUST REDO TO SEND OUT MULTIPLE SENDS (a second or two apart)


        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx (SDKSTR ("Sending M-SEARCH"));
#endif
            string  request;
            {
                const   unsigned int kMaxHops_   =   3;
                stringstream    requestBuf;
                requestBuf << "M-SEARCH * HTTP/1.1\r\n";
                requestBuf << "Host: " << SSDP::V4::kSocketAddress.GetInternetAddress ().As<String> ().AsUTF8 () << ":" << SSDP::V4::kSocketAddress.GetPort () << "\r\n";
                requestBuf << "Man: \"ssdp:discover\"\r\n";
                requestBuf << "ST: " << serviceType.AsUTF8 ().c_str () << "\r\n";
                requestBuf << "MX: " << kMaxHops_ << "\r\n";
                requestBuf << "\r\n";
                request = requestBuf.str ();
                fSocket_.SetMulticastTTL (kMaxHops_);
            }
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("DETAILS: %s", request.c_str ());
#endif
            fSocket_.SendTo (reinterpret_cast<const Byte*> (request.c_str ()), reinterpret_cast<const Byte*> (request.c_str () + request.length ()), SSDP::V4::kSocketAddress);
        }

        // only stopped by thread abort (which we PROBALY SHOULD FIX - ONLY SEARCH FOR CONFIRABLE TIMEOUT???)
        while (1) {
            try {
                Byte    buf[3 * 1024];  // not sure of max packet size
                SocketAddress   from;
                size_t nBytesRead = fSocket_.ReceiveFrom (std::begin (buf), std::end (buf), 0, &from);
                Assert (nBytesRead <= NEltsOf (buf));
                using   namespace   Streams;
                ReadPacketAndNotifyCallbacks_ (TextInputStreamBinaryAdapter (ExternallyOwnedMemoryBinaryInputStream (std::begin (buf), std::begin (buf) + nBytesRead)));
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
    void    ReadPacketAndNotifyCallbacks_ (Streams::TextInputStream in)
    {
        String firstLine    =   in.ReadLine ().Trim ();

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (SDKSTR ("Read Reply"));
        DbgTrace (L"(firstLine: %s)", firstLine.c_str ());
#endif

        const   String  kOKRESPONSELEAD_    =   String_Constant (L"HTTP/1.1 200");
        if (firstLine.length () >= kOKRESPONSELEAD_.length () and firstLine.SubString (0, kOKRESPONSELEAD_.length ()) == kOKRESPONSELEAD_) {
            SSDP::Advertisement d;
            while (true) {
                String line =   in.ReadLine ().Trim ();
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"reply-line: %s", line.c_str ());
#endif
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
                if (label.Compare (L"Location", Characters::CompareOptions::eCaseInsensitive) == 0) {
                    d.fLocation = value;
                }
                else if (label.Compare (L"ST", Characters::CompareOptions::eCaseInsensitive) == 0) {
                    d.fTarget = value;
                }
                else if (label.Compare (L"USN", Characters::CompareOptions::eCaseInsensitive) == 0) {
                    d.fUSN = value;
                }
                else if (label.Compare (L"Server", Characters::CompareOptions::eCaseInsensitive) == 0) {
                    d.fServer = value;
                }
            }
            {
                // bad practice to keep mutex lock here - DEADLOCK CITY - find nice CLEAN way todo this...
                auto    critSec { make_unique_lock (fCritSection_) };
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
 *************************************** Search *********************************
 ********************************************************************************
 */
Search::Search ()
    : fRep_ (new Rep_ ())
{
}

Search::~Search ()
{
    IgnoreExceptionsForCall (fRep_->Stop ());
}

void    Search::AddOnFoundCallback (const std::function<void (const SSDP::Advertisement& d)>& callOnFinds)
{
    fRep_->AddOnFoundCallback (callOnFinds);
}

void    Search::Start (const String& serviceType)
{
    fRep_->Start (serviceType);
}

void    Search::Stop ()
{
    fRep_->Stop ();
}

