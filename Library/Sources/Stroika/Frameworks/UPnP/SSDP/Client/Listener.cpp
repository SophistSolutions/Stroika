/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    <vector>

#include    "../../../../Foundation/Execution/ErrNoException.h"
#include    "../../../../Foundation/Execution/Sleep.h"
#include    "../../../../Foundation/Execution/Thread.h"
#include    "../../../../Foundation/IO/Network/Socket.h"
#include    "../../../../Foundation/Streams/ExternallyOwnedMemoryBinaryInputStream.h"
#include    "../../../../Foundation/Streams/TextInputStreamBinaryAdapter.h"

#include    "Listener.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;
using   namespace   Stroika::Frameworks::UPnP::SSDP;
using   namespace   Stroika::Frameworks::UPnP::SSDP::Client;



/*
 *  See http://quimby.gnus.org/internet-drafts/draft-cai-ssdp-v1-03.txt
 *  for details on the SSDP specification.
 *
 *  And http://www.upnp-hacks.org/upnp.html for more hints.
 */


namespace {
    constexpr   char    SSDP_MULTICAST[]    =      "239.255.255.250";
    constexpr   int     SSDP_PORT           =       1900;
}



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
        , fThread_ () {
        Socket::BindFlags   bindFlags   =   Socket::BindFlags ();
        bindFlags.fReUseAddr = true;
        fSocket_.Bind (SocketAddress (V4::kAddrAny, SSDP_PORT), bindFlags);
        fSocket_.JoinMulticastGroup (InternetAddress (SSDP_MULTICAST));
    }
    void    AddOnFoundCallback (const std::function<void(const Result& d)>& callOnFinds) {
        lock_guard<recursive_mutex> critSection (fCritSection_);
        fFoundCallbacks_.push_back (callOnFinds);
    }
    void    Start () {
        fThread_ = Execution::Thread ([this] () { DoRun_ (); });
        fThread_.Start ();
    }
    void    Stop () {
        fThread_.AbortAndWaitForDone ();
    }
    void    DoRun_ () {
        // only stopped by thread abort
        while (1) {
            try {
                Byte    buf[3 * 1024];  // not sure of max packet size
                SocketAddress   from;
                size_t nBytesRead = fSocket_.ReceiveFrom (StartOfArray (buf), EndOfArray (buf), 0, &from);
                Assert (nBytesRead <= NEltsOf (buf));
                {
                    Streams::ExternallyOwnedMemoryBinaryInputStream readDataAsBinStream (StartOfArray (buf), StartOfArray (buf) + nBytesRead);
                    ParsePacketAndNotifyCallbacks_ (Streams::TextInputStreamBinaryAdapter (readDataAsBinStream));
                }
            }
            catch (const Execution::ThreadAbortException&) {
                Execution::DoReThrow ();
            }
            catch (...) {
                // ignore errors - and keep on trucking
                // but avoid wasting too much time if we get into an error storm
                Execution::Sleep (1.0);
            }
        }
    }
    void    ParsePacketAndNotifyCallbacks_ (Streams::TextInputStream in) {
        String firstLine    =   in.ReadLine ().Trim ();
        const   String  kNOTIFY_LEAD    =   L"NOTIFY ";
        if (firstLine.length () > kNOTIFY_LEAD.length () and firstLine.SubString (0, kNOTIFY_LEAD.length ()) == kNOTIFY_LEAD) {
            Result d;
            while (true) {
                String line =   in.ReadLine ().Trim ();
                if (line.empty ()) {
                    break;
                }

                // Need to simplify this code (stroika string util)
                String  label;
                String  value;
                {
                    size_t n = line.IndexOf (':');
                    if (n != Characters::kBadStringIndex) {
                        label = line.SubString (0, n);
                        value = line.SubString (n + 1).Trim ();
                    }
                }
                if (label == L"Location") {
                    d.fLocation = value;
                }
                else if (label == L"NT") {
                    d.fST = value;
                }
                else if (label == L"USN") {
                    d.fUSN = value;
                }
                else if (label == L"Server") {
                    d.fServer = value;
                }
            }

            {
                lock_guard<recursive_mutex> critSection (fCritSection_);
for (auto i : fFoundCallbacks_) {
                    i (d);
                }
            }
        }
    }
private:
    recursive_mutex                                 fCritSection_;
    vector<std::function<void(const Result& d)>>    fFoundCallbacks_;
    Socket                                          fSocket_;
    Execution::Thread                               fThread_;
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

void    Listener::AddOnFoundCallback (const std::function<void(const Result& d)>& callOnFinds)
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

