/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../../Foundation/Characters/Format.h"
#include    "../../../../Foundation/Containers/Sequence.h"
#include    "../../../../Foundation/Execution/Sleep.h"
#include    "../../../../Foundation/Execution/Thread.h"
#include    "../../../../Foundation/IO/Network/Socket.h"
#include    "../../../../Foundation/Streams/BasicBinaryOutputStream.h"
#include    "../../../../Foundation/Streams/TextOutputStreamBinaryAdapter.h"

#include    "../Common.h"
#include    "../Advertisement.h"
#include    "BasicServer.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;
using   namespace   Stroika::Frameworks::UPnP::SSDP;
using   namespace   Stroika::Frameworks::UPnP::SSDP::Server;






/*
********************************************************************************
******************************* BasicServer::Rep_ ******************************
********************************************************************************
*/
class   BasicServer::Rep_ {
public:
    Sequence<Advertisement> fAdvertisements;

    Rep_ (const Device& d, const FrequencyInfo& fi)
        : fAdvertisements ()
    {
        {
            SSDP::Advertisement  dan;
            dan.fLocation = d.fLocation;
            dan.fServer = d.fServer;
            {
                dan.fST = L"upnp:rootdevice";
                dan.fUSN = Format (L"uuid:%s::upnp:rootdevice", d.fDeviceID.c_str ());
                fAdvertisements.Append (dan);
            }
            {
                dan.fUSN = Format (L"uuid:%s", d.fDeviceID.c_str ());
                dan.fST = dan.fUSN;
                fAdvertisements.Append (dan);
            }
        }

        fNotifierThread_ = Thread ([this, fi]() {
            PeriodicNotifier l;
            l.Run (fAdvertisements, PeriodicNotifier::FrequencyInfo ());
        });
        fNotifierThread_.Start ();
        fSearchResponderThread_ = Thread ([this, fi]() {
            SearchResponder sr;
            sr.Run (fAdvertisements);
        });
        fSearchResponderThread_.Start ();
    }
    ~Rep_ ()
    {
        fNotifierThread_.AbortAndWaitForDone ();
        fSearchResponderThread_.AbortAndWaitForDone ();
    }
    Execution::Thread     fNotifierThread_;
    Execution::Thread     fSearchResponderThread_;
};



/*
********************************************************************************
********************************** BasicServer *********************************
********************************************************************************
*/
BasicServer::BasicServer (const Device& d, const FrequencyInfo& fi)
    : fRep_ (new Rep_ (d, fi))
{
}
