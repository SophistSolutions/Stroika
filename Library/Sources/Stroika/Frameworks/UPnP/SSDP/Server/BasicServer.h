/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Server_BasicServer_h_
#define _Stroika_Frameworks_UPnP_SSDP_Server_BasicServer_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"

#include "Stroika/Frameworks/UPnP/Device.h"
#include "Stroika/Frameworks/UPnP/DeviceDescription.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/PeriodicNotifier.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/SearchResponder.h"

/*
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
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
     *  \brief SSDP Server Implementation: handle the multicast part of UPnP SSDP - listening for searches and sending periodic notifications
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
        /**
         *  Advertise the device on every network interface, each with the LOCATION location gives for it: where its device
         *  description is to be found, from that network (@see LocationProvider). For a Stroika web server, that is usually
         *  LocationFromBindings (theWebServer.bindings ()). d.fLocation is not used.
         *
         *  ipVersion is which SSDP channels to use (IPv4 239.255.255.250, IPv6 ff02::c); each LOCATION is of the same family as
         *  its channel.
         */
        BasicServer (const Device& d, const DeviceDescription& dd, const LocationProvider& location, const FrequencyInfo& fi = FrequencyInfo{},
                     IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion = IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT);
        BasicServer (const BasicServer&)                  = delete;
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
