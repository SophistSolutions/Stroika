/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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




/*
********************************************************************************
******************************** SearchResponder *******************************
********************************************************************************
*/
SearchResponder::SearchResponder ()
{
}

namespace {
    void    DoSend_ (SSDP::Advertisement deviceAnnouncement, Socket s, SocketAddress sendTo)
    {
        deviceAnnouncement.fAlive.clear (); // in responder we dont set alive flag

#if 1
        Memory::BLOB    data = SSDP::Serialize (L"HTTP/1.1 200 OK", deviceAnnouncement);
#else
        Memory::BLOB    data;
        {
            Streams::BasicBinaryOutputStream    out;
            Streams::TextOutputStreamBinaryAdapter  textOut (out, Streams::TextOutputStreamBinaryAdapter::Format::eUTF8WithoutBOM);
            //// SUPER ROUGH FIRST DRAFT
            textOut.Write (Format (L"HTTP/1.1 200 OK\r\n"));
            textOut.Write (Format (L"Host: %s:%d\r\n", SSDP::V4::kSocketAddress.GetInternetAddress ().As<String> ().c_str (), SSDP::V4::kSocketAddress.GetPort ()));
            textOut.Write (Format (L"Cache-Control: max-age=60\r\n"));    // @todo fix
            if (not deviceAnnouncement.fLocation.empty ()) {
                textOut.Write (Format (L"Location: %s\r\n", deviceAnnouncement.fLocation.c_str ()));
            }
            if (not deviceAnnouncement.fServer.empty ()) {
                textOut.Write (Format (L"Server: %s\r\n", deviceAnnouncement.fServer.c_str ()));
            }
            textOut.Write (Format (L"ST: %s\r\n", deviceAnnouncement.fST.c_str ()));
            textOut.Write (Format (L"USN: %s\r\n", deviceAnnouncement.fUSN.c_str ()));
            ///need fluush API on  OUTSTREAM
            data = out.As<Memory::BLOB> ();
        }
#endif
        s.SendTo (data.begin (), data.end (), sendTo);
    };
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

#if 1
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
                for (auto a : advertisements) {
                    DoSend_ (a, useSocket, sendTo);
                }
            }

#else
            if (da.fST == L"upnp:rootdevice") {
                da.fServer = d.fServer;
                da.fLocation = d.fLocation;
                da.fUSN = Format (L"uuid:%s", d.fDeviceID.c_str ());
                DoSend_ (da, useSocket, sendTo);    //

                {
                    da.fServer = d.fServer;
                    da.fLocation = d.fLocation;
                    da.fUSN = d.fDeviceID;
                    da.fUSN = Format (L"uuid:%s", d.fDeviceID.c_str ());
                    da.fST = da.fUSN;
                    DoSend_ (da, useSocket, sendTo);    //
                }
            }
            else if (da.fST.StartsWith (String (L"uuid:") + d.fDeviceID)) {
                da.fServer = d.fServer;
                da.fLocation = d.fLocation;
                da.fUSN = d.fDeviceID;
                da.fUSN = Format (L"uuid:%s", d.fDeviceID.c_str ());
                da.fST = da.fUSN;
                DoSend_ (da, useSocket, sendTo);    //
            }
            else if (da.fST == L"ssdp:all") {
                da.fServer = d.fServer;
                da.fLocation = d.fLocation;
                da.fUSN = Format (L"uuid:%s", d.fDeviceID.c_str ());
                da.fST = da.fUSN;
                DoSend_ (da, useSocket, sendTo);    //
                {
                    da.fServer = d.fServer;
                    da.fLocation = d.fLocation;
                    da.fUSN = d.fDeviceID;
                    da.fUSN = Format (L"uuid:%s", d.fDeviceID.c_str ());
                    da.fST = da.fUSN;
                    DoSend_ (da, useSocket, sendTo);    //
                }
            }
            else {
                int breakere = 1;
                // for now ignore other searches but we should match on device, and owned services, and I'm sure more...
            }
#endif

        }
    }

}

void    SearchResponder::Run (const Iterable<Advertisement>& advertisements)
{
    Execution::Thread t ([this, advertisements]() {
        Socket s (Socket::SocketKind::DGRAM);
        Socket::BindFlags   bindFlags = Socket::BindFlags ();
        bindFlags.fReUseAddr = true;
        s.Bind (SocketAddress (Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()), bindFlags);
        s.JoinMulticastGroup (UPnP::SSDP::V4::kSocketAddress.GetInternetAddress ());

        // only stopped by thread abort
        while (1) {
            try {
                Byte    buf[3 * 1024];  // not sure of max packet size
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
    t.Start ();
    t.WaitForDone ();
}
