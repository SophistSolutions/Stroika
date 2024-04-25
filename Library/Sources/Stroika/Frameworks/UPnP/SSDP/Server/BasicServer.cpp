/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/IO/Network/LinkMonitor.h"
#include "Stroika/Foundation/IO/Network/Socket.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"

#include "Stroika/Frameworks/UPnP/SSDP/Advertisement.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"

#include "BasicServer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;
using namespace Stroika::Frameworks::UPnP::SSDP::Server;

/*
********************************************************************************
******************************* BasicServer::Rep_ ******************************
********************************************************************************
*/
class BasicServer::Rep_ {
public:
    Sequence<Advertisement> fAdvertisements;
    FrequencyInfo           fFrequencyInfo;
    URI                     fLocation;
    Rep_ (const Device& d, const DeviceDescription& dd, const FrequencyInfo& fi, IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
        : fFrequencyInfo{fi}
        , fLocation{d.fLocation}
    {
        {
            SSDP::Advertisement dan;
            // dan.fLocation set in GetAdjustedAdvertisements_...
            dan.fServer = d.fServer;
            {
                dan.fTarget = kTarget_UPNPRootDevice;
                dan.fUSN    = Format ("uuid:{}::{}"_f, d.fDeviceID, kTarget_UPNPRootDevice);
                fAdvertisements.Append (dan);
            }
            {
                dan.fUSN    = Format ("uuid:{}"_f, d.fDeviceID);
                dan.fTarget = dan.fUSN;
                fAdvertisements.Append (dan);
            }
            if (not dd.fDeviceType.empty ()) {
                dan.fUSN    = Format ("uuid:{}::{}"_f, d.fDeviceID, dd.fDeviceType);
                dan.fTarget = dd.fDeviceType;
                fAdvertisements.Append (dan);
            }
        }

        // SEE https://stroika.atlassian.net/browse/STK-962 - make resilient to failures setting up watchers, to retry

        fNotifier_        = make_unique<PeriodicNotifier> (GetAdjustedAdvertisements_ (), fi, ipVersion);
        fSearchResponder_ = make_unique<SearchResponder> (GetAdjustedAdvertisements_ (), ipVersion);

        using LinkMonitor = IO::Network::LinkMonitor;
        LinkMonitor lm;
        lm.AddCallback ([this] (LinkMonitor::LinkChange lc, String netName, String ipNum) {
            Debug::TraceContextBumper ctx{"Basic SSDP server - LinkMonitor callback", "lc = {}, netName={}, ipNum={}"_f, lc, netName, ipNum};
            if (lc == LinkMonitor::LinkChange::eAdded) {
                this->Restart_ ();
            }
        });
        fLinkMonitor_ = optional<LinkMonitor>{move (lm)};
    }
    Sequence<Advertisement> GetAdjustedAdvertisements_ () const
    {
        /*
         *  As a convenience, automatically set the LOCATION advertised to be that of the primary IP address of this
         *  host.
         */
        if (fLocation.GetAuthority () and fLocation.GetAuthority ()->GetHost ()) {
            return fAdvertisements;
        }
        else {
            Sequence<Advertisement> revisedAdvertisements;
            URI                     useBaseURL   = fLocation;
            URI::Authority          useAuthority = useBaseURL.GetAuthority ().value_or (URI::Authority{});
            useAuthority.SetHost (IO::Network::GetPrimaryInternetAddress ());
            useBaseURL.SetAuthority (useAuthority);
            for (auto ai : fAdvertisements) {
                ai.fLocation = useBaseURL.Combine (ai.fLocation);
                revisedAdvertisements.Append (ai);
            }
            return revisedAdvertisements;
        }
    }
    void Restart_ ()
    {
        Debug::TraceContextBumper ctx{"Restarting Basic SSDP server threads"};
        fNotifier_.reset ();
        fSearchResponder_.reset ();
        fNotifier_        = make_unique<PeriodicNotifier> (GetAdjustedAdvertisements_ (), PeriodicNotifier::FrequencyInfo{});
        fSearchResponder_ = make_unique<SearchResponder> (GetAdjustedAdvertisements_ ());
    }
    unique_ptr<PeriodicNotifier>       fNotifier_;
    unique_ptr<SearchResponder>        fSearchResponder_;
    optional<IO::Network::LinkMonitor> fLinkMonitor_; // optional so we can delete it first on shutdown (so no restart while stopping stuff)
};

/*
********************************************************************************
********************************** BasicServer *********************************
********************************************************************************
*/
BasicServer::BasicServer (const Device& d, const DeviceDescription& dd, const FrequencyInfo& fi, IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
    : fRep_{make_shared<Rep_> (d, dd, fi, ipVersion)}
{
}
