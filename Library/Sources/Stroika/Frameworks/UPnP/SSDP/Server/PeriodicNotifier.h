/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_
#define _Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../../Foundation/Characters/String.h"
#include "../../../../Foundation/Configuration/Common.h"
#include "../../../../Foundation/Execution/IntervalTimer.h"
#include "../../../../Foundation/Traversal/Iterable.h"

#include "../../Device.h"
#include "../Advertisement.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Look at http://brisa.garage.maemo.org/doc/html/upnp/ssdp.html for example server API
 */

namespace Stroika::Frameworks::UPnP::SSDP::Server {

    using Traversal::Iterable;

    /**
     *  A big part of SSDP server functinality is to send periodic notifications of the Device description
     *
     *  Instantiating the class starts the (background) notifications automatically, and they
     *  continue until the PeriodicNotifier object is destroyed.
     * 
     *  \note requires Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator
     * 
     *  \note - this behavior differs from Stroika 2.1, where you had to explicitly call Run ()
     *  \note - requirement to instantiate IntervalTimer::Manager::Activator before this new in Stroika v3
     */
    class PeriodicNotifier {
    public:
        // Very primitive definition - should refine - read details on spec on this...
        struct FrequencyInfo {
            Time::DurationSecondsType fRepeatInterval = 3 * 60.0;
        };

    public:
        /**
         *  Note this binds to the sockets on construction, and then keeps notifying through IntervalTimer wakeups, until
         *  this object is destroyed.
         * 
         *  Errors doing sends are just logged with DbgTrace()
         */
        PeriodicNotifier (const Iterable<Advertisement>& advertisements, const FrequencyInfo& fi,
                          IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion = IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT);
        PeriodicNotifier (const PeriodicNotifier&) = delete;

    public:
        const PeriodicNotifier operator= (const PeriodicNotifier&) = delete;

    public:
        ~PeriodicNotifier () = default;

#if 0
        //...
        //Get/Set supported DeviceEntries ();

        //Get/Set Refresh/MaxAge (default is autocompute refresh pace based on maxage)

        // smart ptr to one of these - caller keeps it around, it runs in its own
        // thread as needed, does responses etc.
#endif
    private:
        unique_ptr<Execution::IntervalTimer::Adder> fIntervalTimerAdder_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "PeriodicNotifier.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_*/
