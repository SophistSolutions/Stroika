/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../../Foundation/Characters/Format.h"
#include    "../../../../Foundation/Characters/String_Constant.h"
#include    "../../../../Foundation/Containers/Sequence.h"
#include    "../../../../Foundation/Debug/Trace.h"
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
    URL                     fLocation;
    Rep_ (const Device& d, const FrequencyInfo& fi)
        : fAdvertisements ()
        , fFrequencyInfo (fi)
        , fLocation (d.fLocation)
    {
        {
            SSDP::Advertisement  dan;
            dan.fLocation = d.fLocation.GetFullURL ();
            dan.fServer = d.fServer;
            {
                dan.fTarget = kTarget_UPNPRootDevice;
                dan.fUSN = Format (L"uuid:%s::%s", d.fDeviceID.c_str (), kTarget_UPNPRootDevice.c_str ());
                fAdvertisements.Append (dan);
            }
            {
                dan.fUSN = Format (L"uuid:%s", d.fDeviceID.c_str ());
                dan.fTarget = dan.fUSN;
                fAdvertisements.Append (dan);
            }
        }

        StartNotifier_ ();
        StartResponder_ ();

        fLinkMonitor_ = Optional<IO::Network::LinkMonitor> (move (IO::Network::LinkMonitor ()));
        fLinkMonitor_->AddCallback ([this] (IO::Network::LinkMonitor::LinkChange lc, String netName, String ipNum) {
            Debug::TraceContextBumper ctx (SDKSTR ("Basic SSDP server - LinkMonitor callback"));
            DbgTrace (L"(lc = %d, netName=%s, ipNum=%s)", lc, netName.c_str (), ipNum.c_str ());
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
    Sequence<Advertisement> GetAdjustedAdvertisements_ () const
    {
        if (fLocation.GetHost ().empty ()) {
            Sequence<Advertisement> revisedAdvertisements;
            URL useURL = fLocation;
            useURL.SetHost (IO::Network::GetPrimaryInternetAddress ().As<String> ());
            for (auto ai : fAdvertisements) {
                ai.fLocation = useURL.GetFullURL ();
                revisedAdvertisements.Append (ai);
            }
            return revisedAdvertisements;
        }
        else {
            return fAdvertisements;
        }
    }
    void    StartNotifier_ ()
    {
        fNotifierThread_ = Thread ([this]() {
            PeriodicNotifier l;
            l.Run (GetAdjustedAdvertisements_ (), PeriodicNotifier::FrequencyInfo ());
        });
        fNotifierThread_.SetThreadName (L"SSDP Notifier Thread");
        fNotifierThread_.Start ();
    }
    void    StartResponder_ ()
    {
        fSearchResponderThread_ = Thread ([this]() {
            SearchResponder sr;
            sr.Run (GetAdjustedAdvertisements_ ());
        });
        fNotifierThread_.SetThreadName (L"SSDP Search Responder Thread");
        fSearchResponderThread_.Start ();
    }
    void    Restart_ ()
    {
        Debug::TraceContextBumper ctx (SDKSTR ("Restarting Basic SSDP server threads"));
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
