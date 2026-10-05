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
 */

namespace Stroika::Frameworks::UPnP::SSDP::Server {

    using Traversal::Iterable;

    /**
     *  A big part of SSDP server functionality is to send periodic notifications of the Device description.
     *  Most applications want BasicServer instead, which does this and also answers searches (@see SearchResponder).
     *
     *  Instantiating the class starts the (background) notifications automatically, and they
     *  continue until the PeriodicNotifier object is destroyed. (so a smart pointer to one of these is typically kept around
     *  for the life of the application).
     *
     *  \par Example Usage
     *      \code
     *          Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator; // once, near the start of main ()
     *          ...
     *          // announce this device, as a UPnP root device - with where deviceWS serves its description
     *          SSDP::Advertisement rootDevice;
     *          rootDevice.fServer = SSDP::MakeServerHeaderValue ("MyProduct/1.0"sv);
     *          rootDevice.fTarget = SSDP::kTarget_UPNPRootDevice;
     *          rootDevice.fUSN    = Characters::Format ("uuid:{}::{}"_f, deviceID, SSDP::kTarget_UPNPRootDevice);
     *          PeriodicNotifier notifier{Sequence<SSDP::Advertisement>{rootDevice}, LocationFromBindings (deviceWS.bindings ())};
     *      \endcode
     *
     *  \note requires Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator
     * 
     *  \note - this behavior differs from Stroika 2.1, where you had to explicitly call Run ()
     *  \note - requirement to instantiate IntervalTimer::Manager::Activator before this new in Stroika v3
     */
    class PeriodicNotifier {
    public:
        /**
         *  How often the NOTIFYs go out, and how long each says it is good for (its CACHE-CONTROL max-age) - a listener
         *  forgets a device whose announcement is not renewed by then. So fRepeatInterval must be less than fMaxAge: the UPnP
         *  Device Architecture recommends under half of it, and fMaxAge at least 1800 seconds.
         */
        struct FrequencyInfo {
            Time::DurationSeconds fRepeatInterval{3 * 60.0s};
            Time::DurationSeconds fMaxAge{kDefaultMaxAge};
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
             *  It costs a LinkMonitor (all share one thread, waiting on the OS's address-change notifications) - and, only while
             *  a burst of changes settles and is acted on, a thread of its own. Where the OS cannot tell (e.g. some containers),
             *  it is just not done.
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
        nonvirtual IO::Network::InterfacesByID GetNetworkInterfaces () const;

    private:
        shared_ptr<Execution::Synchronized<IO::Network::InterfacesByID>> fNotifyingOn_; // shared with the timer's callback
        unique_ptr<Execution::IntervalTimer::Adder>                      fIntervalTimerAdder_;
        optional<SSDP::Private_::NetworkChangeFollower> fNetworkChanges_; // last, so destroyed first: no NOTIFY while the rest goes away
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "PeriodicNotifier.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_*/
