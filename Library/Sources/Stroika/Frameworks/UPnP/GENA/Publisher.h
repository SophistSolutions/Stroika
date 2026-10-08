/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_GENA_Publisher_h_
#define _Stroika_Frameworks_UPnP_GENA_Publisher_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <functional>
#include <memory>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Mapping.h"
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

    /**
     *  \brief The eventing of one UPnP service (UPnP Device Architecture 1.1, section 4): control points SUBSCRIBE at the service's
     *         eventSubURL, and are sent a NOTIFY with each change of its evented state variables (those its description marks
     *         sendEvents="yes") - the first, as each subscribes, with all of them.
     *
     *  The service's web server routes SUBSCRIBE and UNSUBSCRIBE at its eventSubURL to HandleRequest; the service calls Notify with
     *  each change. A subscription lasts its TIMEOUT - renewed by the subscriber, with a SUBSCRIBE giving its SID - or until it
     *  UNSUBSCRIBEs.
     *
     *  Each subscription's events are numbered (SEQ): 0 its first, with every evented variable, then 1, 2 ... - wrapping to 1, never
     *  0 again. That first one is sent once the SUBSCRIBE's answer has been, as the subscriber needs that answer's SID to know it -
     *  and with the variables' values as it is sent, so a change in between is in it.
     *
     *  \note THREADS: HandleRequest and Notify may be called on any threads. The NOTIFYs are sent on a thread of the Publisher's own,
     *        one at a time, in the order they were made - each subscription's in SEQ order - to the first of the subscription's
     *        CALLBACK URLs that takes it. A subscriber that takes none stays subscribed (till its TIMEOUT); its next event still
     *        goes, numbered after the lost one, which is how a subscriber can tell an event was lost. currentState (the
     *        constructor's) is called on that thread too, holding no lock of the Publisher's.
     *
     *  \par Example Usage
     *      \code
     *          atomic<bool>    lightOn{false};
     *          GENA::Publisher events{[&] () { return Mapping<String, String>{{"Status"sv, lightOn ? "1"sv : "0"sv}}; }};
     *          // in its web server's routes:
     *          Route{"SUBSCRIBE|UNSUBSCRIBE"_RegEx, "SwitchPower/event"_RegEx, [&] (Message& m) { events.HandleRequest (m); }},
     *          ...
     *          lightOn = true;
     *          events.Notify ({{"Status"sv, "1"sv}});
     *      \endcode
     */
    class Publisher {
    public:
        /**
         */
        struct Options {
            /**
             *  The TIMEOUT a subscription gets when its SUBSCRIBE asks none - and the most one gets, asking more. The UPnP Device
             *  Architecture says SHOULD be at least 1800 seconds.
             */
            Foundation::Time::DurationSeconds fTimeout{1800.0};
        };
        static const Options kDefaultOptions;

    public:
        /**
         *  currentState: the service's evented state variables - all of them, by name, with their values now - for each new
         *  subscription's first event.
         */
        Publisher (const function<Mapping<String, String> ()>& currentState, const Options& options = kDefaultOptions);
        Publisher (const Publisher&) = delete;
        ~Publisher ();

    public:
        nonvirtual Publisher& operator= (const Publisher&) = delete;

    public:
        /**
         *  \brief A SUBSCRIBE - a new subscription, or the renewal of one by its SID - or UNSUBSCRIBE, at the service's eventSubURL;
         *         answered as UPnP Device Architecture 1.1, section 4.1, says: 200 with the SID and TIMEOUT, 400 Bad Request for a
         *         SID given with an NT or a CALLBACK, 412 Precondition Failed for an NT other than upnp:event, no CALLBACK URL,
         *         or a SID it does not know.
         */
        nonvirtual void HandleRequest (WebServer::Message& m);

    public:
        /**
         *  \brief Send every subscriber these state variables' new values, by name.
         */
        nonvirtual void Notify (const Mapping<String, String>& changed);

    private:
        class Rep_;
        unique_ptr<Rep_> fRep_;
    };

    inline const Publisher::Options Publisher::kDefaultOptions;

}

#endif /*_Stroika_Frameworks_UPnP_GENA_Publisher_h_*/
