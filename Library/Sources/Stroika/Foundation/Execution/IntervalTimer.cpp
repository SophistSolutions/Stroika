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
    void AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval, const optional<Time::Duration>& histeresis)
    {
        auto lk = fData_.rwget ();
        lk->Add (Elt_{intervalTimer, Time::GetTickCount () + repeatInterval.As<DurationSecondsType> (), repeatInterval, histeresis});
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
        for (Elt_ i : dataLock.cref ()) {
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
            for (Iterator<Elt_> i = rwDataLock->begin (); i != rwDataLock->end (); ++i) {
                if (i->fFrequency.has_value ()) {
                    Elt_ newE = *i;
                    // @todo add histeresis
                    newE.fCallNextAt = now + i->fFrequency->As<DurationSecondsType> ();
                    rwDataLock->Update (i, newE, &i);
                }
                else {
                    rwDataLock->Remove (i, &i);
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
            sThread_.rwget ()->operator= (Thread::New ([this] () {
                RunnerLoop_ ();
            },
                                                       Thread::eAutoStart, L"Default-Idle-Timer"));
        }
    }
};

IntervalTimer::Manager::DefaultRep::DefaultRep ()
    : fHiddenRep_{make_shared<Rep_> ()}
{
}
void IntervalTimer::Manager::DefaultRep::AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when)
{
    fHiddenRep_->AddOneShot (intervalTimer, when);
}

void IntervalTimer::Manager::DefaultRep::AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval, const optional<Time::Duration>& histeresis)
{
    fHiddenRep_->AddRepeating (intervalTimer, repeatInterval, histeresis);
}

void IntervalTimer::Manager::DefaultRep::RemoveRepeating (const TimerCallback& intervalTimer) noexcept
{
    fHiddenRep_->RemoveRepeating (intervalTimer);
}
