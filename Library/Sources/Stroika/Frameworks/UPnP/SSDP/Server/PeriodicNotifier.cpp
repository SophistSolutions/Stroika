/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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

void    PeriodicNotifier::Run (const Iterable<Advertisement>& advertisements, const FrequencyInfo& fi)
{
    Execution::Thread t ([advertisements, fi]() {
        Socket s (Socket::SocketKind::DGRAM);
        Socket::BindFlags   bindFlags = Socket::BindFlags ();
        bindFlags.fReUseAddr = true;
        s.Bind (SocketAddress (Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()), bindFlags);
        while (true) {
            for (auto a : advertisements) {
                a.fAlive = true;   // periodic notifier must announce alive (we dont support 'going down' yet
                Memory::BLOB    data = SSDP::Serialize (L"NOTIFY * HTTP/1.1", a);
                s.SendTo (data.begin (), data.end (), UPnP::SSDP::V4::kSocketAddress);
            }
            Execution::Sleep (30.0);
        }
    });
    t.Start ();
    t.WaitForDone ();
}
