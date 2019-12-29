/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Client_Listener_h_
#define _Stroika_Frameworks_UPnP_SSDP_Client_Listener_h_ 1

#include "../../../StroikaPreComp.h"

#include <functional>

#include "../../../../Foundation/Containers/Mapping.h"
#include "../../../../Foundation/IO/Network/InternetProtocol/IP.h"

#include "../../Device.h"
#include "../Advertisement.h"

/**
*  \file
*
*  \version    <a href="Code-Status.md#Beta">Beta</a>
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
 *              verus unused headers (now just all raw headers returned).
 */

namespace Stroika::Frameworks::UPnP::SSDP::Client {

    /**
     *  The SSDP Listener object will listen for SSDP 'muticast' messages, and call any
     *  designated callbacks with the values in those SSDP muticast 'NOTIFY' messages.
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
        enum AutoStart { eAutoStart };

    public:
        /**
         */
        Listener (IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion = IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT);
        Listener (const function<void (const SSDP::Advertisement& d)>& callOnFinds, IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion = IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT);
        Listener (const function<void (const SSDP::Advertisement& d)>& callOnFinds, IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion, AutoStart);
        Listener (const function<void (const SSDP::Advertisement& d)>& callOnFinds, AutoStart);
        Listener (Listener&&)      = default;
        Listener (const Listener&) = delete;

    public:
        /**
         *  Its OK to destroy a listener while running. It will silently stop the running listner thread.
         */
        ~Listener ();

    public:
        nonvirtual const Listener& operator= (const Listener&) = delete;

    public:
        /**
         *  Using std::function, no way to compare for operator==, so no way to remove.
         *  @todo    RETHINK!
         *  Note - the callback will be called on an arbitrary thread, so the callback must be threadsafe.
         *  This can be done after the listening has started.
         */
        void AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds);

    public:
        /**
         *  Starts listener (probably starts a thread).
         *  \req not already started.
         */
        nonvirtual void Start ();

    public:
        /**
         *  Stop an already running listener. Not an error to call if not already started
         *  (just does nothing). This will block until the listner is stopped.
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
