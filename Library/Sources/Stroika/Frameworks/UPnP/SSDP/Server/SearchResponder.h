/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Server_SearchResponder_h_
#define _Stroika_Frameworks_UPnP_SSDP_Server_SearchResponder_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <mutex>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
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
     *  Answers SSDP searches (M-SEARCH) that the given advertisements match, with where to find the device's description.
     *  Most applications want BasicServer instead, which does this and also announces the device (@see PeriodicNotifier).
     *
     *  As the UPnP Device Architecture asks (1.1, section 1.3.3), each answer to a multicast M-SEARCH is sent after a random wait
     *  of up to its MX seconds (5 at most) - so devices, and a device's several answers, do not all come at once - while it goes
     *  on answering others; and a multicast M-SEARCH without an MX is ignored. A unicast one (its HOST not a multicast address)
     *  is answered at once.
     *
     *  Instantiating the class starts the (background) listening for searches automatically, and it continues
     *  until the SearchResponder object is destroyed. (so a smart pointer to one of these is typically kept around for the life of the application).
     *
     *  \par Example Usage
     *      \code
     *          // answer searches for this device, as a UPnP root device - with where deviceWS serves its description
     *          SSDP::Advertisement rootDevice;
     *          rootDevice.fServer = SSDP::MakeServerHeaderValue ("MyProduct/1.0"sv);
     *          rootDevice.fTarget = SSDP::kTarget_UPNPRootDevice;
     *          rootDevice.fUSN    = Characters::Format ("uuid:{}::{}"_f, deviceID, SSDP::kTarget_UPNPRootDevice);
     *          SearchResponder responder{Sequence<SSDP::Advertisement>{rootDevice}, LocationFromBindings (deviceWS.bindings ())};
     *      \endcode
     *
     *  \note - this behavior differs from Stroika 2.1, where you had to explicitly call Run ()
     * 
     *  \note this uses its own thread, rather than using IntervalTimer, because it waits on input
     *        from the network, and must always be running/waiting
     */
    class SearchResponder {
    public:
        /**
         */
        struct Options {
            IO::Network::InternetProtocol::IP::IPVersionSupport fIPVersion{IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT}; ///< which SSDP channels: IPv4 (239.255.255.250) and/or IPv6 (ff02::c)
            InterfaceFilter fInterfaces{DefaultInterfaceFilter}; ///< which network interfaces to listen for searches on (@see SSDP::InterfaceFilter)

            /**
             *  When a network appears, listen for searches there too (it re-joins the multicast group on every interface).
             *  It costs a LinkMonitor (all share one thread, waiting on the OS's address-change notifications) - and, only while
             *  a burst of changes settles and is acted on, a thread of its own. Where the OS cannot tell (e.g. some containers),
             *  it is just not done.
             */
            bool fFollowNetworkChanges{true};
        };
        static const Options kDefaultOptions;

    public:
        /**
         *  Listens for searches on every network interface options.fInterfaces accepts (and, given fFollowNetworkChanges, on
         *  each that appears later), and answers
         *  each search the advertisements match with the LOCATION location gives for the asker: the context's fLocalAddress is
         *  this machine's address as the asker reaches it. The advertisements' own fLocation is not used.
         */
        SearchResponder (const Iterable<Advertisement>& advertisements, const LocationProvider& location, const Options& options = kDefaultOptions);
        SearchResponder (const SearchResponder&) = delete;

    public:
        const SearchResponder operator= (const SearchResponder&) = delete;

    public:
        ~SearchResponder () = default;

    public:
        /**
         *  \brief The network interfaces it is listening for searches on now - as they were when it joined them (so with the
         *         addresses they had then). Safe to call from any thread.
         */
        nonvirtual IO::Network::InterfacesByID GetNetworkInterfaces () const;

    private:
        nonvirtual void StartListening_ (const Iterable<Advertisement>& advertisements, const LocationProvider& location, const Options& options);

    private:
        mutex fLifecycleMutex_; // (re)starting the listening - also done on the network-change thread
        Execution::Synchronized<IO::Network::InterfacesByID> fListeningOn_; // set by the listening thread (so declared before it, outliving it)
        Execution::Thread::CleanupPtr                   fListenThread_{Execution::Thread::CleanupPtr::eAbortBeforeWaiting};
        optional<SSDP::Private_::NetworkChangeFollower> fNetworkChanges_; // last, so destroyed first: no restart while the rest goes away
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SearchResponder.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_*/
