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
         *  \par Example Usage
         *      \code
         *          BasicServer server{device, deviceDescription, LocationFromBindings (deviceWS.bindings ()),
         *                             BasicServer::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
         *      \endcode
         */
        struct Options {
            /**
             *  How often to send the NOTIFYs, and how long they - and the answers to searches - say they are good for (as
             *  PeriodicNotifier::FrequencyInfo describes).
             */
            FrequencyInfo fFrequencyInfo{};

            /**
             *  Which SSDP channels to use: IPv4 (239.255.255.250) and/or IPv6 (ff02::c). Each LOCATION is of the same family as
             *  its channel.
             */
            IO::Network::InternetProtocol::IP::IPVersionSupport fIPVersion{IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT};

            /**
             *  Which network interfaces to advertise, and answer searches, on - asked each time SSDP lists them (@see
             *  SSDP::InterfaceFilter). To advertise there with no LOCATION, or some other one, @see LocationProvider.
             */
            InterfaceFilter fInterfaces{DefaultInterfaceFilter};

            /**
             *  When a network appears, advertise there right away, and listen for searches there too.
             *  It costs a LinkMonitor (all share one thread, waiting on the OS's address-change notifications) - and, only while
             *  a burst of changes settles and is acted on, a thread of its own. Where the OS cannot tell (e.g. some containers),
             *  it is just not done. (Two of each: one for the NOTIFYs, one for the answers.)
             */
            bool fFollowNetworkChanges{true};
        };
        static const Options kDefaultOptions;

    public:
        /**
         *  Advertise the device on every network interface, each with the LOCATION location gives for it: where its device
         *  description is to be found, from that network (@see LocationProvider). For a Stroika web server, that is usually
         *  LocationFromBindings (theWebServer.bindings ()). d.fLocation is not used.
         */
        BasicServer (const Device& d, const DeviceDescription& dd, const LocationProvider& location, const Options& options = kDefaultOptions);
        BasicServer (const BasicServer&)                  = delete;
        const BasicServer& operator= (const BasicServer&) = delete;

    public:
        /**
         *  \brief The network interfaces it is advertising on now: those its NOTIFYs last went out of, and those it listens for
         *         searches on. One on both is listed once (matched by fInterfaceID). Safe to call from any thread.
         */
        nonvirtual IO::Network::InterfacesByID GetNetworkInterfaces () const;

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
