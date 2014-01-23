/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../../Foundation/Characters/Format.h"
#include    "../../../../Foundation/Containers/Sequence.h"
#include    "../../../../Foundation/Execution/Sleep.h"
#include    "../../../../Foundation/Execution/Thread.h"
#include    "../../../../Foundation/IO/Network/LinkMonitor.h"
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
    FrequencyInfo           fFrequencyInfo;
    Rep_ (const Device& d, const FrequencyInfo& fi)
        : fAdvertisements ()
        , fFrequencyInfo (fi)
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

        StartNotifier_ ();
        StartResponder_ ();

        fLinkMonitor_ = Optional<IO::Network::LinkMonitor> (move (IO::Network::LinkMonitor ()));
        fLinkMonitor_->AddCallback ([this] (IO::Network::LinkMonitor::LinkChange lc, String netName, String ipNum) {
            if (lc == IO::Network::LinkMonitor::LinkChange::eAdded) {
                this->Restart_ ();
            }
        });
    }
    ~Rep_ ()
    {
        fLinkMonitor_.clear ();
        Thread::SuppressAbortInContext  suppressAbort;  // critical to wait til done cuz captures this
        fNotifierThread_.AbortAndWaitForDone ();
        fSearchResponderThread_.AbortAndWaitForDone ();
    }
    void    StartNotifier_ ()
    {
        fNotifierThread_ = Thread ([this]() {
            PeriodicNotifier l;
            l.Run (fAdvertisements, PeriodicNotifier::FrequencyInfo ());
        });
        fNotifierThread_.Start ();
    }
    void    StartResponder_ ()
    {
        fSearchResponderThread_ = Thread ([this]() {
            SearchResponder sr;
            sr.Run (fAdvertisements);
        });
        fSearchResponderThread_.Start ();
    }
    void    Restart_ ()
    {
        {
            Thread::SuppressAbortInContext  suppressAbort;  // critical to wait til done cuz captures this
            fNotifierThread_.AbortAndWaitForDone ();
            fSearchResponderThread_.AbortAndWaitForDone ();
        }
        StartNotifier_ ();
        StartResponder_ ();
    }
    Execution::Thread                   fNotifierThread_;
    Execution::Thread                   fSearchResponderThread_;
    Optional<IO::Network::LinkMonitor>  fLinkMonitor_;          // optional so we can delete it first on shutdown (so no restart while stopping stuff)
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
