/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Server_BasicServer_h_
#define _Stroika_Frameworks_UPnP_SSDP_Server_BasicServer_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../../Foundation/Characters/String.h"
#include "../../../../Foundation/Configuration/Common.h"

#include "../../Device.h"
#include "../../DeviceDescription.h"
#include "PeriodicNotifier.h"
#include "SearchResponder.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Support SSDP::bye - sending going down notificaiton!!!
 *
 *      @todo   Add serviceList support
 *
 *      @todo   Look at http://brisa.garage.maemo.org/doc/html/upnp/ssdp.html for example server API
 */

namespace Stroika::Frameworks::UPnP::SSDP::Server {

    /**
     *  \brief handle the multicast part of UPnP SSDP - listening for searches and sending periodic notifications
     *
     *  When this object is instantiated, it fires off threads to notify and respond to
     *  searches. When it is destroyed, it stops doing that.
     * 
     *  \note   Caller must separately handle the HTTP requests for device discovery (see SSDP server sample)
     * 
     *  \note  (since Stroika v3)requires Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator
     */
    class BasicServer {
    public:
        using FrequencyInfo = PeriodicNotifier::FrequencyInfo;

    public:
        /*
         *  if the HOST part of d.fLocation is empty, it will dynamically be populated with the hosts primary IP address
         *  see IO::Network::GetPrimaryInternetAddress () - when the server broadcasts it
         */
        BasicServer (const BasicServer&) = delete;
        BasicServer (const Device& d, const DeviceDescription& dd, const FrequencyInfo& fi = FrequencyInfo{});
        const BasicServer& operator= (const BasicServer&) = delete;

    private:
        class Rep_;
        shared_ptr<Rep_> fRep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BasicServer.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Server_BasicServer_h_*/
