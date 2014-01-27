/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../../Foundation/Characters/Format.h"
#include    "../../../../Foundation/Execution/Sleep.h"
#include    "../../../../Foundation/Execution/Thread.h"
#include    "../../../../Foundation/IO/Network/Socket.h"
#include    "../../../../Foundation/Streams/BasicBinaryInputStream.h"
#include    "../../../../Foundation/Streams/BasicBinaryOutputStream.h"
#include    "../../../../Foundation/Streams/ExternallyOwnedMemoryBinaryInputStream.h"
#include    "../../../../Foundation/Streams/TextInputStreamBinaryAdapter.h"
#include    "../../../../Foundation/Streams/TextOutputStreamBinaryAdapter.h"

#include    "../Advertisement.h"
#include    "../Common.h"
#include    "SearchResponder.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;
using   namespace   Stroika::Frameworks::UPnP::SSDP;
using   namespace   Stroika::Frameworks::UPnP::SSDP::Server;



// Comment this in to turn on tracing in this module
//#define   USE_TRACE_IN_THIS_MODULE_       1



/*
********************************************************************************
******************************** SearchResponder *******************************
********************************************************************************
*/
SearchResponder::SearchResponder ()
    : fListenThread_ ()
{
}

SearchResponder::~SearchResponder ()
{
    // Even though no this pointer captured, we must shutdown any running threads before this object terminated else it would run
    // after main exists...
    Execution::Thread::SuppressAbortInContext  suppressAbort;
    fListenThread_.AbortAndWaitForDone ();
}

namespace {
    void    ParsePacketAndRespond_ (Streams::TextInputStream in, const Iterable<Advertisement>& advertisements, Socket useSocket, SocketAddress sendTo)
    {
        String firstLine = in.ReadLine ().Trim ();

#if     USE_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (SDKSTR ("Read SSDP Packet"));
        DbgTrace (L"(firstLine: %s)", firstLine.c_str ());
#endif
        const   String  kNOTIFY_LEAD = L"M-SEARCH ";
        if (firstLine.length () > kNOTIFY_LEAD.length () and firstLine.SubString (0, kNOTIFY_LEAD.length ()) == kNOTIFY_LEAD) {
            SSDP::Advertisement da;
            while (true) {
                String line = in.ReadLine ().Trim ();
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
                    da.fRawHeaders.Add (label, value);
                }
                if (label.Compare (L"ST", Characters::CompareOptions::eCaseInsensitive) == 0) {
                    da.fST = value;
                }
                else {
                    int hreab = 1;
                }
            }

            bool    matches = false;
            if (da.fST == L"upnp:rootdevice") {
                matches = true;
            }
            else if (da.fST == L"ssdp:all") {
                matches = true;
            }
            else if (da.fST.StartsWith (String (L"uuid:"))) {
                for (auto a : advertisements) {
                    // @todo - not quite right... well - maybe right - look more closely
                    if (a.fUSN == da.fST) {
                        matches = true;
                    }
                }
            }
            if (matches) {
                // if any match, I think we are supposed to send all
#if     USE_TRACE_IN_THIS_MODULE_
                DbgTrace (L"sending advertisements...");
#endif
                for (auto a : advertisements) {
                    a.fAlive.clear (); // in responder we dont set alive flag
                    Memory::BLOB    data = SSDP::Serialize (L"HTTP/1.1 200 OK", a);
                    useSocket.SendTo (data.begin (), data.end (), sendTo);
#if     USE_TRACE_IN_THIS_MODULE_
                    String msg;
                    msg += String (L"location=") + a.fLocation + L", ";
                    msg += String (L"ST=") + a.fST + L", ";
                    msg += String (L"USN=") + a.fUSN;
                    DbgTrace (L"(%s)", msg.c_str ());
#endif
                }
            }
        }
    }

}

void    SearchResponder::Run (const Iterable<Advertisement>& advertisements)
{
    fListenThread_ = Execution::Thread ([advertisements]() {
        Debug::TraceContextBumper ctx (SDKSTR ("SSDP SearchResponder thread loop"));
        Socket s (Socket::SocketKind::DGRAM);
        Socket::BindFlags   bindFlags = Socket::BindFlags ();
        bindFlags.fReUseAddr = true;
        s.Bind (SocketAddress (Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()), bindFlags);
        s.SetMulticastLoopMode (true);       // probably should make this configurable
        const   unsigned int kMaxHops_   =   3;
        s.SetMulticastTTL (kMaxHops_);
        {
Again:
            try {
                s.JoinMulticastGroup (UPnP::SSDP::V4::kSocketAddress.GetInternetAddress ());
            }
            catch (const Execution::errno_ErrorException& e) {
                if (e == ENODEV) {
                    // This can happen on Linux when you start before you have a network connection - no problem - just keep trying
                    DbgTrace ("Got exception (errno: ENODEV) - while joining multicast group, so try again");
                    Execution::Sleep (1);
                    goto Again;
                }
                else {
                    Execution::DoReThrow ();
                }
            }
        }

        // only stopped by thread abort
        while (1) {
            try {
                Byte    buf[4 * 1024];  // not sure of max packet size
                SocketAddress   from;
                size_t nBytesRead = s.ReceiveFrom (std::begin (buf), std::end (buf), 0, &from);
                Assert (nBytesRead <= NEltsOf (buf));
                using   namespace   Streams;
                ParsePacketAndRespond_ (TextInputStreamBinaryAdapter (ExternallyOwnedMemoryBinaryInputStream (std::begin (buf), std::begin (buf) + nBytesRead)), advertisements, s, from);
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
    });
    fListenThread_.SetThreadName (L"SSDP Search Responder thread");
    fListenThread_.Start ();
    fListenThread_.WaitForDone ();
}
