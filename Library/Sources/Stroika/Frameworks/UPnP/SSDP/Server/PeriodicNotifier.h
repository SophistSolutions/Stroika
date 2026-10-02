/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_
#define _Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Execution/IntervalTimer.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

#include "Stroika/Frameworks/UPnP/Device.h"
#include "Stroika/Frameworks/UPnP/SSDP/Advertisement.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/LocationProvider.h"

/*
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
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
            Time::DurationSeconds fRepeatInterval{3 * 60.0s};
        };

    public:
        /**
         */
        struct Options {
            FrequencyInfo fFrequencyInfo{}; ///< how often to send the NOTIFYs
            IO::Network::InternetProtocol::IP::IPVersionSupport fIPVersion{IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT}; ///< which SSDP channels: IPv4 (239.255.255.250) and/or IPv6 (ff02::c)
            InterfaceFilter fInterfaces{DefaultInterfaceFilter}; ///< which network interfaces to send them out of - asked each time (@see SSDP::InterfaceFilter)

            /**
             *  When a network appears, send the NOTIFYs right away, rather than at the next cycle.
             *  It costs a LinkMonitor: a thread waiting on the OS's address-change notifications. Where the OS cannot
             *  tell (e.g. some containers), it is just not done. As of v3.0d25 LinkMonitor reports IPv4 addresses only, so a
             *  network that gains only an IPv6 address is not noticed.
             */
            bool fFollowNetworkChanges{true};
        };
        static const Options kDefaultOptions;

    public:
        /**
         *  Binds the sockets on construction, then sends the advertisements - out of every network interface
         *  options.fInterfaces accepts, each with the LOCATION location gives for that interface - right away and every
         *  options.fFrequencyInfo.fRepeatInterval after, until this object is destroyed. The interfaces are listed afresh each time, so networks that come and go
         *  are picked up. The advertisements' own fLocation is not used.
         * 
         *  Errors doing sends are just logged with DbgTrace()
         */
        PeriodicNotifier (const Iterable<Advertisement>& advertisements, const LocationProvider& location, const Options& options = kDefaultOptions);
        PeriodicNotifier (const PeriodicNotifier&) = delete;

    public:
        const PeriodicNotifier operator= (const PeriodicNotifier&) = delete;

    public:
        ~PeriodicNotifier () = default;

    public:
        /**
         *  \brief The network interfaces its last NOTIFYs went out of - as they were then (so with the addresses they had then).
         *         Safe to call from any thread.
         */
        nonvirtual Traversal::Iterable<IO::Network::Interface> GetNetworkInterfaces () const;

#if 0
        //...
        //Get/Set supported DeviceEntries ();

        //Get/Set Refresh/MaxAge (default is autocompute refresh pace based on maxage)

        // smart ptr to one of these - caller keeps it around, it runs in its own
        // thread as needed, does responses etc.
#endif
    private:
        shared_ptr<Execution::Synchronized<Containers::Sequence<IO::Network::Interface>>> fNotifyingOn_; // shared with the timer's callback
        unique_ptr<Execution::IntervalTimer::Adder>                                       fIntervalTimerAdder_;
        optional<IO::Network::LinkMonitor> fLinkMonitor_; // last, so destroyed first: no NOTIFY while the rest goes away
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "PeriodicNotifier.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_*/
