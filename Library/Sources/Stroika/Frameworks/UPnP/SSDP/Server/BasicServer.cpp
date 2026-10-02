/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/IO/Network/LinkMonitor.h"
#include "Stroika/Foundation/IO/Network/Socket.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
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

using Memory::MakeSharedPtr;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;
using namespace Stroika::Frameworks::UPnP::SSDP::Server;

/*
 ********************************************************************************
 ******************************* BasicServer::Rep_ ******************************
 ********************************************************************************
 */
class BasicServer::Rep_ final {
public:
    Sequence<Advertisement>                             fAdvertisements; // without fLocation: the notifier and responder fill that in
    LocationProvider                                    fLocation;
    FrequencyInfo                                       fFrequencyInfo;
    IO::Network::InternetProtocol::IP::IPVersionSupport fIPVersion;
    Rep_ (const Device& d, const DeviceDescription& dd, const LocationProvider& location, const FrequencyInfo& fi,
          IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
        : fLocation{location}
        , fFrequencyInfo{fi}
        , fIPVersion{ipVersion}
    {
        {
            SSDP::Advertisement dan;
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

        // SEE https://github.com/SophistSolutions/Stroika/issues/1094 (STK-962) - make resilient to failures setting up watchers, to retry

        Start_ ();

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
    void Start_ ()
    {
        fNotifier_        = make_unique<PeriodicNotifier> (fAdvertisements, fLocation, fFrequencyInfo, fIPVersion);
        fSearchResponder_ = make_unique<SearchResponder> (fAdvertisements, fLocation, fIPVersion);
    }
    void Restart_ ()
    {
        Debug::TraceContextBumper ctx{"Restarting Basic SSDP server threads"};
        fNotifier_.reset ();
        fSearchResponder_.reset ();
        Start_ (); // joining the group on any new interfaces
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
BasicServer::BasicServer (const Device& d, const DeviceDescription& dd, const LocationProvider& location, const FrequencyInfo& fi,
                          IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion)
    : fRep_{MakeSharedPtr<Rep_> (d, dd, location, fi, ipVersion)}
{
}
