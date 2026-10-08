/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_GENA_Subscriber_h_
#define _Stroika_Frameworks_UPnP_GENA_Subscriber_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdint>
#include <functional>
#include <memory>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/IO/Network/Port.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Stroika/Frameworks/WebServer/Message.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Frameworks::UPnP::GENA {

    using Foundation::Characters::String;
    using Foundation::Containers::Mapping;
    using Foundation::IO::Network::URI;

    /**
     *  \brief A control point's subscription to a UPnP service's events (UPnP Device Architecture 1.1, section 4): Start SUBSCRIBEs at
     *         the service's eventSubURL, it renews before its TIMEOUT runs out, and Stop (or destruction) UNSUBSCRIBEs - and it reports
     *         each event the service sends it, each NOTIFY, which the control point's own web server routes to HandleNotify.
     *
     *  The NOTIFYs come to the callback URL it gives the service, which must reach the control point's web server from the service:
     *  MakeCallbackURL makes one, with this machine's address on the network the service is on.
     *
     *  Each event has its SEQ: 0 the first, with every evented variable, then 1, 2 ... in order - a SEQ missed is an event the
     *  service could not deliver. That first one can come before the SUBSCRIBE's answer, with the SID it is to be known by: so
     *  until that answer, a NOTIFY is taken whatever its SID - the callback URL being this subscription's own. And so route the
     *  NOTIFYs to it before Start: the first can come while Start is still waiting for that answer.
     *
     *  If a renewal fails - the service forgot the subscription, say, restarting - it SUBSCRIBEs again, so has a new SID; and
     *  failing that, tries again a little later.
     *
     *  \note THREADS: onEvent is called on the web server's thread that took the NOTIFY. Renewal is on IntervalTimer's thread, so
     *        it needs an IntervalTimer::Manager::Activator, as SSDP's BasicServer and CachingListener do.
     *
     *  \par Example Usage
     *      \code
     *          // NOTIFYs to /light-events on port 8091 - routed to it by this web server - then subscribed
     *          GENA::Subscriber             lightEvents{eventSubURL, GENA::Subscriber::MakeCallbackURL (eventSubURL, 8091, "/light-events"sv),
     *                                                   [] (const GENA::Subscriber::Event& e) {
     *                                                       // e.fVariables: what changed - say, {Status: 1}
     *                                                   }};
     *          WebServer::ConnectionManager notifies{SocketAddresses (InternetAddresses_Any (), 8091),
     *                                                {Route{"NOTIFY"_RegEx, "light-events"_RegEx, [&] (Message& m) { lightEvents.HandleNotify (m); }}}};
     *          lightEvents.Start ();
     *      \endcode
     */
    class Subscriber {
    public:
        /**
         */
        struct Event {
            uint32_t fSEQ;
            /**
             *  The evented variables changed - in the first event, all of them - by name.
             */
            Mapping<String, String> fVariables;
        };

    public:
        /**
         */
        struct Options {
            /**
             *  The TIMEOUT asked of the service - which may grant less. It is renewed at half what it grants.
             */
            Foundation::Time::DurationSeconds fTimeout{1800.0};
        };
        static const Options kDefaultOptions;

    public:
        /**
         *  Not yet subscribed: Start does that.
         */
        Subscriber (const URI& eventSubURL, const URI& callbackURL, const function<void (const Event&)>& onEvent, const Options& options = kDefaultOptions);
        Subscriber (const Subscriber&) = delete;
        /**
         *  Stops - so unsubscribes.
         */
        ~Subscriber ();

    public:
        nonvirtual Subscriber& operator= (const Subscriber&) = delete;

    public:
        /**
         *  \brief Subscribe - throwing if the service does not take it - and keep it renewed. Started already: does nothing.
         */
        nonvirtual void Start ();

    public:
        /**
         *  \brief Unsubscribe - failing quietly, as the service then drops it at its TIMEOUT - so no more events are reported.
         *         Not started: does nothing.
         */
        nonvirtual void Stop ();

    public:
        /**
         *  The ID the service knows the subscription by - new if it was subscribed again; nullopt when not subscribed.
         */
        nonvirtual optional<String> GetSID () const;

    public:
        /**
         *  \brief A NOTIFY at the callback URL - answered as UPnP Device Architecture 1.1, section 4.3, says: 200, or 400 Bad Request
         *         for no NT or NTS, 412 Precondition Failed for an NT or NTS not GENA's, a SID not this subscription's, or one
         *         when not started.
         */
        nonvirtual void HandleNotify (WebServer::Message& m);

    public:
        /**
         *  \brief A callback URL for the service at eventSubURL: http, at this machine's address on the network the service is on
         *         - the near end of a TCP connection to it - and port and path. So it connects to the service: throws if it cannot.
         */
        static URI MakeCallbackURL (const URI& eventSubURL, Foundation::IO::Network::PortType port, const String& path);

    private:
        class Rep_;
        unique_ptr<Rep_> fRep_;
    };

    inline const Subscriber::Options Subscriber::kDefaultOptions;

}

#endif /*_Stroika_Frameworks_UPnP_GENA_Subscriber_h_*/
