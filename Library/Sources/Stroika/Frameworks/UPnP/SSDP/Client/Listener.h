/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Client_Listener_h_
#define _Stroika_Frameworks_UPnP_SSDP_Client_Listener_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <functional>

#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/IO/Network/InternetProtocol/IP.h"

#include "Stroika/Frameworks/UPnP/Device.h"
#include "Stroika/Frameworks/UPnP/SSDP/Advertisement.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"

/**
*  \file
*
*  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
*
* TODO:
 *      @todo   Should probably add Network::NetlinkListener - to check for net up/down messages, and
 *              redo multicast (as we do for server).
 *
 *      @todo   Consider adding OnError callback?
 *
 *      @todo   Better docs on 'Firewall Note' - and maybe workaround suggestions.
 *
 *      @todo   Fix Result object do a better job summarizing original map of headers
 *              versus unused headers (now just all raw headers returned).
 */

namespace Stroika::Frameworks::UPnP::SSDP::Client {

    /**
     *  The SSDP Listener object will listen for SSDP 'multicast messages, and call any
     *  designated callbacks with the values in those SSDP multicast 'NOTIFY' messages.
     *
     *  Firewall Note:
     *      Firewalls can occasionally block SSDP multicast listening support. Perhaps
     *      because they are blocking the multicast group add? I've never seen an explicit
     *      error message, but often turning off firewalls, rebooting, and trying again
     *      makes the listen problem go away.
     *
     *  \note - this internally creates a thread to monitor network traffic, and to call the callback functions on.
     */
    class Listener {
    public:
        enum AutoStart {
            eAutoStart
        };

    public:
        /**
         *  \par Example Usage
         *      \code
         *          Listener listener{callOnFinds, Listener::Options{.fIPVersion = IPVersionSupport::eIPV4Only}, Listener::eAutoStart};
         *      \endcode
         */
        struct Options {
            /**
             *  Which SSDP channels to listen on: IPv4 (239.255.255.250) and/or IPv6 (ff02::c).
             */
            IO::Network::InternetProtocol::IP::IPVersionSupport fIPVersion{IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT};

            /**
             *  Which network interfaces to listen on (@see SSDP::InterfaceFilter).
             */
            InterfaceFilter fInterfaces{DefaultInterfaceFilter};

            /**
             *  When a network appears, listen there too (it re-joins the multicast group on every interface).
             *  It costs a LinkMonitor (all share one thread, waiting on the OS's address-change notifications). Where the OS cannot
             *  tell (e.g. some containers), it is just not done.
             */
            bool fFollowNetworkChanges{true};
        };
        static const Options kDefaultOptions;

    public:
        /**
         *  Listen for SSDP NOTIFY messages, calling callOnFinds - and any callback added with AddOnFoundCallback - with each one
         *  heard. Listening starts with Start (), or right away given eAutoStart.
         *
         *  \note THREADS: callOnFinds is called on the listener's own thread, not the caller's - so it must be thread-safe, and
         *        whatever it shares with other threads needs synchronizing (e.g. Execution::Synchronized).
         *
         *  \note NETWORKS: it listens on every network interface options.fInterfaces accepts (by default, every one running and
         *        not loopback) - and, given Options::fFollowNetworkChanges, on each that appears later. If there is none (e.g.
         *        no network yet), it hears nothing until there is.
         */
        Listener (const Options& options = kDefaultOptions);
        Listener (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const Options& options = kDefaultOptions); ///< \brief callOnFinds: called on the listener's own thread
        Listener (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const Options& options,
                  AutoStart); ///< \brief ... and Start (); callOnFinds: called on the listener's own thread
        Listener (const function<void (const SSDP::Advertisement& d)>& callOnFinds, AutoStart); ///< \brief ... and Start (); callOnFinds: called on the listener's own thread
        Listener (Listener&&)      = default;
        Listener (const Listener&) = delete;

    public:
        /**
         *  Its OK to destroy a listener while running. It will silently stop the running listener thread.
         */
        ~Listener ();

    public:
        nonvirtual const Listener& operator= (const Listener&) = delete;

    public:
        /**
         *  Using std::function, no way to compare for operator==, so no way to remove.
         *  @todo    RETHINK!
         *  This can be done after the listening has started.
         *
         *  \note THREADS: callOnFinds is called on the listener's own thread, not the caller's - so it must be thread-safe.
         */
        void AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds);

    public:
        /**
         *  \brief The network interfaces it is listening on now - as they were when it joined them (so with the addresses they
         *         had then). Safe to call from any thread.
         *
         *  \par Example Usage
         *      \code
         *          // Listener's constructor threw no_such_device before v3.0d25 when it could listen on no network; to fail as
         *          // it did - or more usefully, unless it is listening on a network you care about:
         *          Listener listener{callOnFinds, Listener::eAutoStart};
         *          if (not listener.GetNetworkInterfaces ().Any ([] (const Interface& i) {
         *                  return i.fType == Interface::Type::eWiredEthernet or i.fType == Interface::Type::eWIFI;
         *              })) {
         *              Execution::Throw (SystemErrorException{make_error_code (errc::no_such_device)});
         *          }
         *          // (though given Options::fFollowNetworkChanges, it starts listening on such a network as soon as one appears)
         *      \endcode
         */
        nonvirtual IO::Network::InterfacesByID GetNetworkInterfaces () const;

    public:
        /**
         *  Starts listener (probably starts a thread).
         *  \pre not already started.
         */
        nonvirtual void Start ();

    public:
        /**
         *  Stop an already running listener. Not an error to call if not already started
         *  (just does nothing). This will block until the listener is stopped.
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
#include "Listener.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Client_Listener_h_*/
