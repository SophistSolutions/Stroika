/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Client_CachingListener_h_
#define _Stroika_Frameworks_UPnP_SSDP_Client_CachingListener_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <functional>
#include <optional>

#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Stroika/Frameworks/UPnP/SSDP/Advertisement.h"
#include "Stroika/Frameworks/UPnP/SSDP/Client/Listener.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Frameworks::UPnP::SSDP::Client {

    /**
     *  \brief A Listener that remembers: the SSDP advertisements in force around it - each kept from its ssdp:alive (or answer to
     *         its search) until its max-age runs out, or an ssdp:byebye withdraws its device.
     *
     *  Its callbacks are told of each change to what it holds - not of each advertisement heard, as a Listener's are - with the
     *  Advertisement changed: fAlive true when it is added - heard for the first time, or the first since it was removed; false
     *  when it is removed - by an ssdp:byebye, or its max-age running out (so also for a device that went without saying so) -
     *  as last heard, LOCATION and all (a byebye has none). One it holds, heard again, calls nothing: it is only kept longer.
     *
     *  So it says what is around in two ways - callbacks, and GetAdvertisements () - for different uses:
     *      o   callbacks alone, to keep a model of one's own: fetching each device's description once, say, as it is added.
     *      o   GetAdvertisements () alone, to ask what is there when wanted: after a search (the second example), or to show a
     *          list now and then.
     *      o   both, for a part attaching once it is running - what is there now, then each change (the third example) - or for
     *          a callback that needs the whole picture, not just the change ("was that the last device of its type?"): from
     *          within a callback, GetAdvertisements () has the change it is told of made.
     *
     *  It listens (a Listener) and - given Options::fSearchFor - searches (a Search), so what is already there is found at once,
     *  not just at its next NOTIFY: a device need re-announce only within half its max-age, which the UPnP Device Architecture
     *  says SHOULD be at least 1800 seconds.
     *
     *  An advertisement is a USN at a LOCATION: a device on several networks advertises each USN with each network's own
     *  LOCATION (as BasicServer does), and each of those is added, and expires, on its own. But an ssdp:byebye removes its whole
     *  device - every USN with its "uuid:device-UUID" (not an embedded device's: that has a UUID of its own), at every LOCATION -
     *  whichever USN it names: a device cannot withdraw one of its advertisements alone (UPnP Device Architecture 1.1, sections
     *  1.2.2 and 2). Every advertisement heard is kept, whatever Options::fSearchFor - filter in the callback, or what
     *  GetAdvertisements () returns.
     *
     *  \note THREADS: callbacks run on whichever thread sees the change - the listener's, the searcher's, or IntervalTimer's for
     *        an expiry - one at a time, in the order of the changes they report. A callback may call GetAdvertisements (),
     *        AddOnChangeCallback () and RemoveOnChangeCallback (). An exception from one is logged and ignored. Keep them quick:
     *        while one runs, what is heard waits, and so do Stop () and destruction - and IntervalTimer's thread is shared with
     *        every other timer. Hand slow work - fetching the device description, say - to another thread.
     *
     *  \note Needs an IntervalTimer::Manager::Activator, as BasicServer does: while started, it looks for what has expired
     *        each second, on IntervalTimer's thread.
     *
     *  \note A search answer and an ssdp:byebye come in on different threads - the Search's and the Listener's - so are not
     *        ordered: an answer read late (that thread behind, on a busy network) can add back what a byebye removed, until its
     *        max-age runs out.
     *
     *  \par Example Usage
     *      \code
     *          CachingListener devices{[] (const SSDP::Advertisement& a) {
     *                                      // a.fAlive: added (true) or removed (false) - a.fUSN, a.fLocation ... say what
     *                                  },
     *                                  CachingListener::eAutoStart};
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // a search that returns what it found: the M-SEARCH's MX is 4, so devices answer within 4 seconds
     *          CachingListener found{CachingListener::Options{.fSearchFor = SSDP::kTarget_UPNPRootDevice}, CachingListener::eAutoStart};
     *          Execution::Sleep (5s);
     *          for (const SSDP::Advertisement& a : found.GetAdvertisements ()) {
     *              ...
     *          }
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // attaching once it is running: what is there now, then each change. In this order: the other way round, a change
     *          // between the two is missed. And holding the lock the callback takes, so it applies each change after what
     *          // GetAdvertisements () returned - else that older list could add back what a removal just removed. A change may
     *          // repeat that list - one added that it has, or removed that it lacks - so Apply_ must make a repeat change nothing:
     *          // by USN and LOCATION, add or replace, or remove if there.
     *          lock_guard l{fMutex_};
     *          fCallbackID = cache.AddOnChangeCallback ([this] (const SSDP::Advertisement& a) {
     *              lock_guard l{fMutex_};
     *              Apply_ (a);
     *          });
     *          for (const SSDP::Advertisement& a : cache.GetAdvertisements ()) {
     *              Apply_ (a);
     *          }
     *          ...
     *          cache.RemoveOnChangeCallback (fCallbackID); // later - not holding fMutex_: this waits for a running callback
     *      \endcode
     */
    class CachingListener {
    public:
        using AutoStart = Listener::AutoStart;
        static constexpr AutoStart eAutoStart{Listener::eAutoStart};

    public:
        /**
         *  \par Example Usage
         *      \code
         *          CachingListener cache{callOnChanges, CachingListener::Options{.fListener = {.fIPVersion = IPVersionSupport::eIPV4Only}, .fSearchFor = nullopt}, CachingListener::eAutoStart};
         *      \endcode
         */
        struct Options {
            /**
             *  How it listens - and searches, given fSearchFor: the same SSDP channels, network interfaces, and following of
             *  network changes.
             */
            Listener::Options fListener;

            /**
             *  What to search for, as it starts (and when a network appears): by default ssdp:all, which devices answer with each
             *  advertisement they have - so what is cached soon matches what their NOTIFYs would give. nullopt: only listen.
             */
            optional<String> fSearchFor{"ssdp:all"sv};

            /**
             *  Search again this often, too (@see Search::Start's autoRetryInterval). nullopt (the default): search only as it
             *  starts - twice, 2 seconds apart, as UDP can lose one - and when a network appears. An interval under a device's
             *  max-age keeps one that sends no NOTIFYs (some only answer searches) cached.
             */
            optional<Foundation::Time::Duration> fSearchRepeatInterval;
        };
        static const Options kDefaultOptions;

    public:
        /**
         *  Listen - and search, given options.fSearchFor - calling callOnChanges, and any callback added with AddOnChangeCallback,
         *  with each change. Listening starts with Start (), or right away given eAutoStart.
         *
         *  \note THREADS: callOnChanges is called on another thread, not the caller's (@see CachingListener).
         */
        CachingListener (const Options& options = kDefaultOptions);
        CachingListener (const Options& options, AutoStart); ///< \brief ... and Start () - for GetAdvertisements (), with no callback
        CachingListener (const function<void (const SSDP::Advertisement& d)>& callOnChanges, const Options& options = kDefaultOptions); ///< \brief callOnChanges: called on another thread
        CachingListener (const function<void (const SSDP::Advertisement& d)>& callOnChanges, const Options& options,
                         AutoStart); ///< \brief ... and Start (); callOnChanges: called on another thread
        CachingListener (const function<void (const SSDP::Advertisement& d)>& callOnChanges, AutoStart); ///< \brief ... and Start (); callOnChanges: called on another thread
        CachingListener (CachingListener&&)      = default;
        CachingListener (const CachingListener&) = delete;

    public:
        /**
         *  Its OK to destroy it while running: it stops - waiting for a callback running.
         *
         *  \pre not from within a callback (that would wait for itself)
         */
        ~CachingListener ();

    public:
        nonvirtual CachingListener& operator= (const CachingListener&) = delete;

    public:
        /**
         *  \brief Names a callback AddOnChangeCallback added, for RemoveOnChangeCallback - Listener's.
         */
        using CallbackID = Listener::CallbackID;

    public:
        /**
         *  \brief Call callOnChanges too, with each change from now on - what it holds already, GetAdvertisements () returns (both,
         *         missing nothing: @see CachingListener's third example). From any thread, also once started - and from within a
         *         callback; it never waits for a running one.
         *
         *  \note THREADS: callOnChanges is called on another thread, not the caller's (@see CachingListener) - so it must be thread-safe.
         */
        nonvirtual CallbackID AddOnChangeCallback (const function<void (const SSDP::Advertisement& d)>& callOnChanges);

    public:
        /**
         *  \brief Once this returns, that callback is not running, and is never called again - except that, called from within it,
         *         the call already under way finishes. So it waits for that callback, running on another thread: do not call it
         *         holding a lock that callback takes. Removing one not added (or already removed) does nothing.
         */
        nonvirtual void RemoveOnChangeCallback (CallbackID callOnChanges);

    public:
        /**
         *  \brief The network interfaces it is listening on now (@see Listener::GetNetworkInterfaces). Safe to call from any thread.
         */
        nonvirtual IO::Network::InterfacesByID GetNetworkInterfaces () const;

    public:
        /**
         *  \brief The advertisements in force: each added, and not yet removed - fAlive true. Safe to call from any thread.
         */
        nonvirtual Containers::Collection<SSDP::Advertisement> GetAdvertisements () const;

    public:
        /**
         *  Starts listening (and searching) - on the listener's thread (and the searcher's) - and looking, each second on
         *  IntervalTimer's thread, for what has expired.
         *
         *  \pre not already started.
         *  \pre an IntervalTimer::Manager::Activator exists
         */
        nonvirtual void Start ();

    public:
        /**
         *  Stops listening, searching, and looking for what has expired. What it has kept, it keeps - and removes, if it has
         *  expired by then, once started again.
         *  Not an error to call if not started (just does nothing). This waits for a callback running.
         *
         *  \pre not from within a callback (that would wait for itself)
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
#include "CachingListener.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Client_CachingListener_h_*/
