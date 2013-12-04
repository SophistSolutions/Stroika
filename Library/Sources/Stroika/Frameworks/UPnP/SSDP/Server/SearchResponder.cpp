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

#include    "../Common.h"
#include    "SearchResponder.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;
using   namespace   Stroika::Frameworks::UPnP::SSDP;
using   namespace   Stroika::Frameworks::UPnP::SSDP::Server;


#if 0
/*
********************************************************************************
******************************** PeriodicNotifier ******************************
********************************************************************************
*/
PeriodicNotifier::PeriodicNotifier ()
{
}

void    PeriodicNotifier::Run (const Device& d, const FrequencyInfo& fi)
{
    Execution::Thread t ([d, fi]() {
        Socket s (Socket::SocketKind::DGRAM);
        while (true) {
            Memory::BLOB    data;
            {
                Streams::BasicBinaryOutputStream    out;
                Streams::TextOutputStreamBinaryAdapter  textOut (out);
                //// SUPER ROUGH FIRST DRAFT
                textOut.Write (Format (L"NOTIFY * HTTP/1.1\r\n"));
                textOut.Write (Format (L"Host: 239.255.255.250:reservedSSDPport\r\n"));
                if (not d.fST.empty ()) {
                    textOut.Write (Format (L"NT: %s\r\n", d.fST.c_str ()));
                }
                textOut.Write (Format (L"NTS: ssdp:alive\r\n"));
                if (not d.fUSN.empty ()) {
                    textOut.Write (Format (L"USN: %s\r\n", d.fUSN.c_str ()));
                }
                if (not d.fLocation.empty ()) {
                    textOut.Write (Format (L"Location: %s\r\n", d.fLocation.c_str ()));
                }
                textOut.Write (Format (L"Cache-Control: max-age = 7393\r\n"));
#if 0
                NOTIFY* HTTP / 1.1
Host: 239.255.255.250 :
                reservedSSDPport
NT :
blenderassociation :
                blender
NTS :
ssdp :
                alive
USN :
someunique :
                idscheme3
AL :
<blender : ixl> < http : //foo/bar>
Cache - Control :
                max - age = 7393
#endif

                            ///need fluush API on  OUTSTREAM
                            data = out.As<Memory::BLOB> ();
            }
            s.SendTo (data.begin (), data.end (), UPnP::SSDP::V4::kSocketAddress);
            Execution::Sleep (30.0);
        }
    });
    t.Start ();
    t.WaitForDone ();
}
#endif
