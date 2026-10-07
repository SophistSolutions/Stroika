/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <algorithm>
#include <memory>
#include <mutex>

#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/IntervalTimer.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "Stroika/Frameworks/UPnP/SSDP/Client/Search.h"

#include "CachingListener.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO::Network;

using Memory::MakeSharedPtr;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;
using namespace Stroika::Frameworks::UPnP::SSDP::Client;

/*
 ********************************************************************************
 *************************** CachingListener::Rep_ ******************************
 ********************************************************************************
 */
class CachingListener::Rep_ {
private:
    struct Entry_ {
        SSDP::Advertisement    fAdvertisement; // the last heard - fAlive true
        Time::TimePointSeconds fExpiresAt;
    };
    using Locations_ = Mapping<URI, Entry_>; // one USN's advertisements, by LOCATION

public:
    Rep_ (const Options& options)
        : fOptions_{options}
        , fListener_{[this] (const SSDP::Advertisement& a) { Heard_ (a, false); }, options.fListener}
    {
        if (options.fSearchFor) {
            fSearch_.emplace ([this] (const SSDP::Advertisement& a) { Heard_ (a, true); },
                              Search::Options{.fIPVersion            = options.fListener.fIPVersion,
                                              .fInterfaces           = options.fListener.fInterfaces,
                                              .fFollowNetworkChanges = options.fListener.fFollowNetworkChanges});
        }
    }
    ~Rep_ ()
    {
        IgnoreExceptionsForCall (Stop ()); // first: its expiry checks call Expire_, on this
    }
    CallbackID AddOnChangeCallback (const function<void (const SSDP::Advertisement& d)>& callOnChanges)
    {
        return fCallbacks_.Add (callOnChanges);
    }
    void RemoveOnChangeCallback (CallbackID callOnChanges)
    {
        fCallbacks_.Remove (callOnChanges);
    }
    InterfacesByID GetNetworkInterfaces () const
    {
        return fListener_.GetNetworkInterfaces ();
    }
    Collection<SSDP::Advertisement> GetAdvertisements () const
    {
        Collection<SSDP::Advertisement> result;
        for (const KeyValuePair<String, Locations_>& usn : fCache_.load ()) {
            for (const KeyValuePair<URI, Entry_>& i : usn.fValue) {
                result += i.fValue.fAdvertisement;
            }
        }
        return result;
    }
    void Start ()
    {
        [[maybe_unused]] lock_guard lifecycle{fLifecycleMutex_};
        {
            [[maybe_unused]] lock_guard noting{fNoting_};
            Require (not fStarted_); // not already started
            fStarted_ = true;
            CheckExpiriesWhenDue_ (); // what it kept from before a Stop
        }
        fListener_.Start ();
        if (fSearch_) {
            fSearch_->Start (*fOptions_.fSearchFor, fOptions_.fSearchRepeatInterval);
        }
    }
    void Stop ()
    {
        [[maybe_unused]] lock_guard lifecycle{fLifecycleMutex_};
        if (fSearch_) {
            fSearch_->Stop ();
        }
        fListener_.Stop ();
        // last, so after any Heard_: the expiry checks - removed not holding fNoting_, as removing one waits for its call under
        // way, an Expire_ that may be waiting for fNoting_ (then doing nothing, as stopped)
        Sequence<IntervalTimer::TimerID> checks;
        {
            [[maybe_unused]] lock_guard noting{fNoting_};
            fStarted_ = false;
            checks    = exchange (fChecks_, {});
            fCheckAt_ = nullopt;
        }
        for (IntervalTimer::TimerID c : checks) {
            IntervalTimer::Manager::sThe.Remove (c);
        }
    }

private:
    // on the listener's thread, or the searcher's: an ssdp:alive, or a search answer, adds its advertisement - or keeps it
    // longer; an ssdp:byebye removes its device's every advertisement, at every LOCATION
    void Heard_ (const SSDP::Advertisement& heard, bool searchAnswer)
    {
        const String& usn = heard.fUSN;
        if (usn.empty ()) {
            return; // nothing to know it by
        }
        [[maybe_unused]] lock_guard noting{fNoting_};
        if (searchAnswer or heard.fAlive == true) {
            SSDP::Advertisement a            = heard;
            a.fAlive                         = true; // as a search answer does not say
            Time::TimePointSeconds expiresAt = Time::GetTickCount () + a.fMaxAge.value_or (kDefaultMaxAge);
            fSoonestExpiry_                  = fSoonestExpiry_ ? min (*fSoonestExpiry_, expiresAt) : expiresAt;
            CheckExpiriesWhenDue_ ();
            bool added = false;
            {
                auto       cache     = fCache_.rwget ();
                Locations_ locations = cache->LookupValue (usn);
                added                = locations.Add (a.fLocation, Entry_{a, expiresAt});
                cache->Add (usn, locations);
            }
            if (added) {
                fCallbacks_.Call (a);
            }
        }
        else if (heard.fAlive == false) {
            // the whole device is gone: a device cannot withdraw one of its advertisements alone (UPnP Device Architecture 1.1,
            // sections 1.2.2 and 2), so whichever its ssdp:byebye names, it withdraws them all
            const String                  device = DeviceOf_ (usn);
            auto                          ofIt   = [&] (const KeyValuePair<String, Locations_>& i) { return DeviceOf_ (i.fKey) == device; };
            Sequence<SSDP::Advertisement> gone;
            {
                auto cache = fCache_.rwget ();
                for (const KeyValuePair<String, Locations_>& i : cache->Where (ofIt)) {
                    for (const KeyValuePair<URI, Entry_>& j : i.fValue) {
                        gone += Gone_ (j.fValue);
                    }
                }
                cache->RemoveAll (ofIt);
            }
            for (const SSDP::Advertisement& a : gone) {
                fCallbacks_.Call (a);
            }
        }
        // else an ssdp:update (UPnP 1.1), which neither adds nor removes
    }
    // on IntervalTimer's thread, as the soonest expiry is due (@see CheckExpiriesWhenDue_): removes what has expired
    void Expire_ ()
    {
        [[maybe_unused]] lock_guard noting{fNoting_};
        if (not fStarted_) {
            return; // stopping: Stop removes the checks
        }
        // this check is done, and any other to come is no longer needed - CheckExpiriesWhenDue_ adds the next: removed, which here,
        // on the timer thread, never waits
        for (IntervalTimer::TimerID c : fChecks_) {
            IntervalTimer::Manager::sThe.Remove (c);
        }
        fChecks_.RemoveAll ();
        fCheckAt_                         = nullopt;
        Time::TimePointSeconds        now = Time::GetTickCount ();
        Sequence<SSDP::Advertisement> expired;
        if (fSoonestExpiry_ and *fSoonestExpiry_ <= now) { // else nothing can have expired yet
            Mapping<String, Locations_> kept;
            fSoonestExpiry_ = nullopt; // what was soonest may have been heard again since: recompute
            for (const KeyValuePair<String, Locations_>& usn : fCache_.load ()) {
                Locations_ keptHere;
                for (const KeyValuePair<URI, Entry_>& i : usn.fValue) {
                    if (i.fValue.fExpiresAt <= now) {
                        expired += Gone_ (i.fValue);
                    }
                    else {
                        keptHere.Add (i.fKey, i.fValue);
                        fSoonestExpiry_ = fSoonestExpiry_ ? min (*fSoonestExpiry_, i.fValue.fExpiresAt) : i.fValue.fExpiresAt;
                    }
                }
                if (not keptHere.empty ()) {
                    kept.Add (usn.fKey, keptHere);
                }
            }
            if (not expired.empty ()) {
                fCache_.store (kept);
            }
        }
        CheckExpiriesWhenDue_ ();
        for (const SSDP::Advertisement& a : expired) {
            fCallbacks_.Call (a);
        }
    }
    // holding fNoting_: that Expire_ is called as the soonest expiry is due - adding a one-shot timer, unless one is due by then.
    // It never removes one: that waits for its call under way - an Expire_ that may be waiting for fNoting_ - so Expire_ removes
    // those no longer needed (on the timer thread, where that never waits), and Stop the rest.
    void CheckExpiriesWhenDue_ ()
    {
        if (fStarted_ and fSoonestExpiry_ and (not fCheckAt_ or *fSoonestExpiry_ < *fCheckAt_)) {
            fCheckAt_ = *fSoonestExpiry_;
            fChecks_ += IntervalTimer::Manager::sThe.AddOneShot ([this] () { Expire_ (); },
                                                                 max (*fSoonestExpiry_ - Time::GetTickCount (), Time::DurationSeconds{0}));
        }
    }
    // the device a USN names - its "uuid:device-UUID", before any "::" (UPnP Device Architecture 1.1, section 1.2.2) - so an
    // embedded device, with a UUID of its own, is a device of its own
    static String DeviceOf_ (const String& usn)
    {
        optional<size_t> i = usn.Find ("::"sv);
        return i ? usn.SubString (0, *i) : usn;
    }
    // what the callbacks are told of an advertisement removed: the last heard, fAlive false
    static SSDP::Advertisement Gone_ (const Entry_& e)
    {
        SSDP::Advertisement a = e.fAdvertisement;
        a.fAlive              = false;
        return a;
    }

private:
    const Options fOptions_;
    mutex         fLifecycleMutex_; // Start and Stop
    // one change at a time - made and told - on whichever thread sees it: so the callbacks are told of the changes in order
    mutex                                               fNoting_;
    CallbackRegistry<void (const SSDP::Advertisement&)> fCallbacks_;
    Synchronized<Mapping<String, Locations_>>           fCache_;         // by USN; changed only holding fNoting_
    optional<Time::TimePointSeconds>                    fSoonestExpiry_; // (fNoting_) no entry expires before it - nullopt: none to
    Sequence<IntervalTimer::TimerID> fChecks_;         // (fNoting_) the one-shot timers still to call Expire_ - Stop removes them
    optional<Time::TimePointSeconds> fCheckAt_;        // (fNoting_) when the soonest of them is due - nullopt: none
    bool                             fStarted_{false}; // (fNoting_)
    // last, so stopped first: they call Heard_, which uses the rest
    Listener         fListener_;
    optional<Search> fSearch_;
};

/*
 ********************************************************************************
 ******************************** CachingListener *******************************
 ********************************************************************************
 */
CachingListener::CachingListener (const Options& options)
    : fRep_{MakeSharedPtr<Rep_> (options)}
{
}

CachingListener::CachingListener (const Options& options, AutoStart)
    : CachingListener{options}
{
    Start ();
}

CachingListener::CachingListener (const function<void (const SSDP::Advertisement& d)>& callOnChanges, const Options& options)
    : CachingListener{options}
{
    AddOnChangeCallback (callOnChanges);
}

CachingListener::CachingListener (const function<void (const SSDP::Advertisement& d)>& callOnChanges, const Options& options, AutoStart)
    : CachingListener{callOnChanges, options}
{
    Start ();
}

CachingListener::CachingListener (const function<void (const SSDP::Advertisement& d)>& callOnChanges, AutoStart)
    : CachingListener{callOnChanges}
{
    Start ();
}

CachingListener::~CachingListener () = default; // Rep_ stops as it goes

auto CachingListener::AddOnChangeCallback (const function<void (const SSDP::Advertisement& d)>& callOnChanges) -> CallbackID
{
    return fRep_->AddOnChangeCallback (callOnChanges);
}

void CachingListener::RemoveOnChangeCallback (CallbackID callOnChanges)
{
    fRep_->RemoveOnChangeCallback (callOnChanges);
}

IO::Network::InterfacesByID CachingListener::GetNetworkInterfaces () const
{
    return fRep_->GetNetworkInterfaces ();
}

Collection<SSDP::Advertisement> CachingListener::GetAdvertisements () const
{
    return fRep_->GetAdvertisements ();
}

void CachingListener::Start ()
{
    fRep_->Start ();
}

void CachingListener::Stop ()
{
    fRep_->Stop ();
}
