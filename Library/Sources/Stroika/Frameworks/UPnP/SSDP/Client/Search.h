/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Client_Search_h_
#define _Stroika_Frameworks_UPnP_SSDP_Client_Search_h_ 1

#include "../../../StroikaPreComp.h"

#include <functional>

#include "../../../../Foundation/IO/Network/InternetProtocol/IP.h"

#include "../../Device.h"
#include "../Advertisement.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   must re-read ssdp spec for exact format of messages!
 *
 *      @todo   Must have some API for how often to send searches, and how long
 *              to listen for responses (maybe forever on responses?)
 *
 *      @todo   Consider adding OnError callback?
 *
 *      @todo   Fix Result object to return other interesting fields
 *
 *      @todo   Consider some synchonous API, so it sends a certian number of times, and
 *              then returns all the answers.
 *
 */

namespace Stroika::Frameworks::UPnP::SSDP::Client {

    /**
     */
    class Search {
    public:
        /**
         * see @see Start () for possible values for initialSearch and autoRetryInterval
         */
        Search (IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion = IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT);
        Search (const function<void (const SSDP::Advertisement& d)>& callOnFinds, IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion = IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT);
        Search (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const String& initialSearch, IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion = IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT);
        Search (const function<void (const SSDP::Advertisement& d)>& callOnFinds, const String& initialSearch, const optional<Time::Duration>& autoRetryInterval, IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion = IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT);
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
         *  Using std::function, no way to compare for operator==, so no way to remove.
         *  @todo    RETHINK!
         *  Note - the callback will be called on an arbitrary thread, so the callback must be threadsafe.
         *  This can be done after the listening has started.
         */
        void AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds);

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
         *          Start (L"ssdp:all");                                                // Search for all devices and services
         *          Start (kSSDPAny);                                                   // ...
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Start (L"upnp:rootdevice");                                         // Search for all root devices
         *          Start (kRootDevice);                                                // ...
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Start (L"urn:schemas-wifialliance-org:service:WFAWLANConfig:1");    // Search for all devices of this type
         *      \endcode
         *
         *
         *  \par Example Usage
         *      \code
         *          Start (L"uuid:9cd09dd4-fd8d-5737-abc3-2faa8c11cbdb");               // Search specific device
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
