/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Containers/Collection.h"
#include "../Time/Realtime.h"

#include "Synchronized.h"
#include "Thread.h"

#include "IntervalTimer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Time;

/*
 ********************************************************************************
 ******************** IntervalTimer::Manager::DefaultRep ************************
 ********************************************************************************
 */
struct IntervalTimer::Manager::DefaultRep ::Rep_ {
    Rep_ ()
    {
    }
    void AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when)
    {
        auto lk = fData_.rwget ();
        lk->Add (Elt_{intervalTimer, Time::GetTickCount () + when.As<DurationSecondsType> ()});
        DataChanged_ ();
    }
    void AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis)
    {
        auto lk = fData_.rwget ();
        lk->Add (Elt_{intervalTimer, Time::GetTickCount () + repeatInterval.As<DurationSecondsType> (), repeatInterval, hysteresis});
        DataChanged_ ();
    }
    void RemoveRepeating (const TimerCallback& intervalTimer) noexcept
    {
        auto lk = fData_.rwget ();
        Verify (lk->RemoveIf ([&] (const Elt_& i) { return i.fCallback == intervalTimer; }));
        DataChanged_ ();
    }

    struct Elt_ {
        Function<void (void)> fCallback;
        DurationSecondsType   fCallNextAt;
        optional<Duration>    fFrequency;
        optional<Duration>    fHisteresis;
    };
    // @todo - re-implement using priority q, with next time at top of q
    Synchronized<Collection<Elt_>>   fData_;
    Synchronized<Thread::CleanupPtr> sThread_{Execution::Thread::CleanupPtr::eAbortBeforeWaiting};
    WaitableEvent                    fDataChanged_{WaitableEvent::ResetType::eAutoReset};

    // this is where a priorityq would be better
    DurationSecondsType GetNextWakeupTime_ ()
    {
        auto dataLock = fData_.cget ();
        Assert (not dataLock->empty ());
        DurationSecondsType r = kInfinite;
        for (const Elt_& i : dataLock.cref ()) {
            r = min (r, i.fCallNextAt);
        }
        return r;
    }
    void RunnerLoop_ ()
    {
        // keep checking for timer events to run
        while (true) {
            DurationSecondsType wakeupAt = GetNextWakeupTime_ ();
            fDataChanged_.WaitUntilQuietly (wakeupAt);
            // now process any timer events that are ready (could easily be more than one).
            // if we had a priority q, we would do them in order, but for now, just do all that are ready
            // NOTE - to avoid holding a lock (in case these guys remove themselves or whatever) - lock/run through list twice
            DurationSecondsType now      = Time::GetTickCount ();
            Collection<Elt_>    elts2Run = fData_.cget ()->Where ([=] (const Elt_& i) { return i.fCallNextAt <= now; });
            for (Elt_ i : elts2Run) {
                IgnoreExceptionsExceptThreadAbortForCall (i.fCallback ());
            }
            // now reset the 'next' time for each run element
            auto rwDataLock = fData_.rwget ();
            now             = Time::GetTickCount ();
            for (Elt_ i : elts2Run) {
                if (i.fFrequency.has_value ()) {
                    Elt_ newE = i;
                    // @todo add hysteresis
                    newE.fCallNextAt = now + newE.fFrequency->As<DurationSecondsType> ();
                    auto updateI     = rwDataLock->Find ([&] (const Elt_& ii) { return ii.fCallback == i.fCallback; });
                    rwDataLock->Update (updateI, newE);
                }
                else {
                    Verify (rwDataLock->RemoveIf ([&] (const Elt_& ii) { return ii.fCallback == i.fCallback; }));
                }
            }
        }
    }
    void DataChanged_ ()
    {
        auto rwThreadLk = sThread_.rwget ();
        if (fData_.cget ()->empty ()) {
            rwThreadLk->operator= (nullptr);
        }
        else {
            auto lk = sThread_.rwget ();
            if (lk.cref () == nullptr) {
                lk->operator= (Thread::New ([this] () {
                    RunnerLoop_ ();
                },
                                            Thread::eAutoStart, L"Default-Interval-Timer"sv));
            }
        }
    }
};

IntervalTimer::Manager::DefaultRep::DefaultRep ()
    : fHiddenRep_{make_shared<Rep_> ()}
{
}

void IntervalTimer::Manager::DefaultRep::AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when)
{
    AssertNotNull (fHiddenRep_);
    fHiddenRep_->AddOneShot (intervalTimer, when);
}

void IntervalTimer::Manager::DefaultRep::AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis)
{
    AssertNotNull (fHiddenRep_);
    fHiddenRep_->AddRepeating (intervalTimer, repeatInterval, hysteresis);
}

void IntervalTimer::Manager::DefaultRep::RemoveRepeating (const TimerCallback& intervalTimer) noexcept
{
    AssertNotNull (fHiddenRep_);
    fHiddenRep_->RemoveRepeating (intervalTimer);
}

/*
 ********************************************************************************
 ********************* IntervalTimer::Manager::Activator ************************
 ********************************************************************************
 */
IntervalTimer::Manager::Activator::Activator ()
{
    Require (Manager::sThe.fRep_ == nullptr); // only one activator object allowed
    Manager::sThe = Manager{make_shared<IntervalTimer::Manager::DefaultRep> ()};
}

IntervalTimer::Manager::Activator::~Activator ()
{
    RequireNotNull (Manager::sThe.fRep_); // this is the only way to remove, and so must not be null here
    Manager::sThe.fRep_.reset ();
}
