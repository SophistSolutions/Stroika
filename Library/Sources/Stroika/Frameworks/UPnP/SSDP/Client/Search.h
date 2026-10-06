/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Client_Search_h_
#define _Stroika_Frameworks_UPnP_SSDP_Client_Search_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <functional>

#include "Stroika/Foundation/IO/Network/InternetProtocol/IP.h"

#include "Stroika/Frameworks/UPnP/Device.h"
#include "Stroika/Frameworks/UPnP/SSDP/Advertisement.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Frameworks::UPnP::SSDP::Client {

    /**
     */
    class Search {
    public:
        /**
         *  \par Example Usage
         *      \code
         *          Search search{callOnFinds, Search::kRootDevice, Search::Options{.fIPVersion = IPVersionSupport::eIPV4Only}};
         *      \endcode
         */
        struct Options {
            /**
             *  Which SSDP channels to search on: IPv4 (239.255.255.250) and/or IPv6 (ff02::c).
             */
            IO::Network::InternetProtocol::IP::IPVersionSupport fIPVersion{IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT};

            /**
             *  Which network interfaces to search on - asked of each one, for each send (@see SSDP::InterfaceFilter).
             */
            InterfaceFilter fInterfaces{DefaultInterfaceFilter};

            /**
             *  When a network appears, search there right away, rather than at the next retry (it starts the search over).
             *  It costs a LinkMonitor (all share one thread, waiting on the OS's address-change notifications) - and, only while
             *  a burst of changes settles and is acted on, a thread of its own. Where the OS cannot tell (e.g. some containers),
             *  it is just not done.
             */
            bool fFollowNetworkChanges{true};
        };
        static const Options kDefaultOptions;

    public:
        /**
         *  Search for SSDP devices, calling callOnFinds - and any callback added with AddOnFoundCallback - with each answer. The
         *  search starts given initialSearch, else with Start (); @see Start () for possible values for initialSearch and
         *  autoRetryInterval.
         *
         *  \note THREADS: callOnFinds is called on the searcher's own thread, not the caller's - so it must be thread-safe, and
         *        whatever it shares with other threads needs synchronizing (e.g. Execution::Synchronized).
         *
         *  \note Keep callOnFinds quick: Stop () and destruction wait for one running. (Searching a network that appears does
         *        not - given Options::fFollowNetworkChanges, the search starts over once it returns.)
         *        Hand slow work - fetching the device description, say - to another thread, or give it a timeout: an HTTP fetch
         *        from a device that has just gone can take minutes (IO::Network::Transfer::Connection has no timeout of its own).
         *
         *  \note NETWORKS: each M-SEARCH goes out of every network interface options.fInterfaces accepts (by default, every one
         *        running and not loopback) - listed afresh for each send, so networks that come and go are picked up (a new
         *        one right away, given Options::fFollowNetworkChanges). If it accepts none (or there is no network yet),
         *        nothing is sent until there is one.
         */
        Search (const Options& options = kDefaultOptions);
        Search (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const Options& options = kDefaultOptions); ///< \brief callOnFinds: called on the searcher's own thread
        Search (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const String& initialSearch,
                const Options& options = kDefaultOptions); ///< \brief ... and Start (initialSearch); callOnFinds: called on the searcher's own thread
        Search (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const String& initialSearch,
                const optional<Time::Duration>& autoRetryInterval,
                const Options& options = kDefaultOptions); ///< \brief ... and Start (initialSearch, autoRetryInterval); callOnFinds: called on the searcher's own thread
        Search (Search&&)      = default;
        Search (const Search&) = delete;

    public:
        /**
         *  Its OK to destroy a searcher while running. It will silently stop the running searcher thread.
         */
        ~Search ();

    public:
        nonvirtual const Search& operator= (const Search&) = delete;

    public:
        /**
         *  Callbacks can be added after the search has started - but not, yet, removed.
         *
         *  \note THREADS: callOnFinds is called on the searcher's own thread, not the caller's - so it must be thread-safe.
         */
        nonvirtual void AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds);

    public:
        /**
         *  \brief The network interfaces its last M-SEARCH went out of - as they were then (so with the addresses they had then).
         *         Safe to call from any thread.
         */
        nonvirtual IO::Network::InterfacesByID GetNetworkInterfaces () const;

    public:
        /**
         *  ssdp:all - possible argument for search string
         */
        static const String kSSDPAny;

    public:
        /**
         *  upnp:rootdevice - possible argument for search string
         */
        static const String kRootDevice;

    public:
        /**
         *  Starts searcher (probably starts a thread).
         *  args - ST, strings, uuid etc.
         *
         *  If already running, this automatically stops an existing search, and restarts it with
         *  the given serviceType parameters.
         *
         *  ssdp:all: Search for all devices and services.
         *  \par Example Usage
         *      \code
         *          Start ("ssdp:all");                                                 // Search for all devices and services
         *          Start (kSSDPAny);                                                   // ...
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Start ("upnp:rootdevice");                                          // Search for all root devices
         *          Start (kRootDevice);                                                // ...
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Start ("urn:schemas-wifialliance-org:service:WFAWLANConfig:1");    // Search for all devices of this type
         *      \endcode
         *
         *
         *  \par Example Usage
         *      \code
         *          Start ("uuid:9cd09dd4-fd8d-5737-abc3-2faa8c11cbdb");               // Search specific device
         *      \endcode
         *
         */
        nonvirtual void Start (const String& serviceType, const optional<Time::Duration>& autoRetryInterval = nullopt);

    public:
        /**
         *  Stop an already running search. Not an error to call if not already started (just does nothing).
         *  This will block until the searcher has stopped (typically milliseconds).
         */
        nonvirtual void Stop ();

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
#include "Search.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Client_Search_h_*/
