/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
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
    Sequence<Advertisement> fAdvertisements; // without fLocation: the notifier and responder fill that in
    LocationProvider        fLocation;
    Options                 fOptions;
    Rep_ (const Device& d, const DeviceDescription& dd, const LocationProvider& location, const Options& options)
        : fLocation{location}
        , fOptions{options}
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

        // the notifier and responder each follow network changes themselves (Options::fFollowNetworkChanges)
        fNotifier_ = make_unique<PeriodicNotifier> (fAdvertisements, fLocation,
                                                    PeriodicNotifier::Options{.fFrequencyInfo        = fOptions.fFrequencyInfo,
                                                                              .fIPVersion            = fOptions.fIPVersion,
                                                                              .fInterfaces           = fOptions.fInterfaces,
                                                                              .fFollowNetworkChanges = fOptions.fFollowNetworkChanges});
        fSearchResponder_ = make_unique<SearchResponder> (fAdvertisements, fLocation,
                                                          SearchResponder::Options{.fIPVersion  = fOptions.fIPVersion,
                                                                                   .fInterfaces = fOptions.fInterfaces,
                                                                                   .fFollowNetworkChanges = fOptions.fFollowNetworkChanges});
    }
    unique_ptr<PeriodicNotifier> fNotifier_;
    unique_ptr<SearchResponder>  fSearchResponder_;
};

/*
********************************************************************************
********************************** BasicServer *********************************
********************************************************************************
*/
BasicServer::BasicServer (const Device& d, const DeviceDescription& dd, const LocationProvider& location, const Options& options)
    : fRep_{MakeSharedPtr<Rep_> (d, dd, location, options)}
{
}

Traversal::Iterable<Interface> BasicServer::GetNetworkInterfaces () const
{
    Containers::Sequence<Interface> both;
    both.AppendAll (fRep_->fNotifier_->GetNetworkInterfaces ());
    both.AppendAll (fRep_->fSearchResponder_->GetNetworkInterfaces ());
    // each from its own listing: an interface on both is reported once - matched by fInternalInterfaceID
    return both.Distinct ([] (const Interface& a, const Interface& b) { return a.fInternalInterfaceID == b.fInternalInterfaceID; });
}
