/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../../Foundation/Characters/Format.h"
#include    "../../../../Foundation/Execution/Sleep.h"
#include    "../../../../Foundation/Execution/Thread.h"
#include    "../../../../Foundation/IO/Network/Socket.h"
#include    "../../../../Foundation/Streams/BasicBinaryOutputStream.h"
#include    "../../../../Foundation/Streams/TextOutputStreamBinaryAdapter.h"

#include    "../Advertisement.h"
#include    "../Common.h"
#include    "PeriodicNotifier.h"


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
******************************** PeriodicNotifier ******************************
********************************************************************************
*/
PeriodicNotifier::PeriodicNotifier ()
{
}

namespace {
    void    DoSend_ (SSDP::Advertisement deviceAnnouncement, Socket s)
    {
        Memory::BLOB    data;
        {
            Streams::BasicBinaryOutputStream    out;
            Streams::TextOutputStreamBinaryAdapter  textOut (out, Streams::TextOutputStreamBinaryAdapter::Format::eUTF8WithoutBOM);
            //// SUPER ROUGH FIRST DRAFT
            textOut.Write (Format (L"NOTIFY * HTTP/1.1\r\n"));
            textOut.Write (Format (L"Host: %s:%d\r\n", SSDP::V4::kSocketAddress.GetInternetAddress ().As<String> ().c_str (), SSDP::V4::kSocketAddress.GetPort ()));
            textOut.Write (Format (L"Cache-Control: max-age=60\r\n"));    // @todo fix
            if (not deviceAnnouncement.fLocation.empty ()) {
                textOut.Write (Format (L"Location: %s\r\n", deviceAnnouncement.fLocation.c_str ()));
            }
            textOut.Write (Format (L"NTS: ssdp:alive\r\n"));
            if (not deviceAnnouncement.fServer.empty ()) {
                textOut.Write (Format (L"Server: %s\r\n", deviceAnnouncement.fServer.c_str ()));
            }
            textOut.Write (Format (L"NT: %s\r\n", deviceAnnouncement.fST.c_str ()));
            textOut.Write (Format (L"USN: %s\r\n", deviceAnnouncement.fUSN.c_str ()));
            ///need fluush API on  OUTSTREAM
            data = out.As<Memory::BLOB> ();
        }
        s.SendTo (data.begin (), data.end (), UPnP::SSDP::V4::kSocketAddress);
    };
}

void    PeriodicNotifier::Run (const Iterable<Advertisement>& advertisements, const FrequencyInfo& fi)
{
    Execution::Thread t ([advertisements, fi]() {
        Socket s (Socket::SocketKind::DGRAM);
        Socket::BindFlags   bindFlags = Socket::BindFlags ();
        bindFlags.fReUseAddr = true;
        s.Bind (SocketAddress (Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()), bindFlags);
        while (true) {
            for (auto a : advertisements) {
                DoSend_ (a, s);
            }
            Execution::Sleep (30.0);
        }
    });
    t.Start ();
    t.WaitForDone ();
}
