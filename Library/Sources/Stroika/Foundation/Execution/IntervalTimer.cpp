/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <condition_variable>
#include <mutex>
#include <random>

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Debug/Main.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "IntervalTimer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Time;

using Memory::MakeSharedPtr;

namespace {
    thread_local const void* tRunnerOf_{}; // on a timer thread, the manager (DefaultRep::Rep_) it runs timers for
}

/*
 ********************************************************************************
 *********************** IntervalTimer::RegisteredTask **************************
 ********************************************************************************
 */
Characters::String IntervalTimer::RegisteredTask::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "id: "sv << fID;
    sb << ", callNextAt: "sv << fCallNextAt;
    sb << ", frequency: "sv << fFrequency;
    sb << ", hysteresis: "sv << fHysteresis;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ******************** IntervalTimer::Manager::DefaultRep ************************
 ********************************************************************************
 */
struct IntervalTimer::Manager::DefaultRep ::Rep_ {
    Rep_ ()          = default;
    virtual ~Rep_ () = default;
    void AddOneShot (TimerID timer, const TimerCallback& intervalTimer, const Time::Duration& when)
    {
        Debug::TraceContextBumper ctx{"IntervalTimer::Manager: default implementation: AddOneShot"};
        fData_.rwget ()->Add (RegisteredTask{timer, intervalTimer, Time::GetTickCount () + when});
        DataChanged_ ();
    }
    void AddRepeating (TimerID timer, const TimerCallback& intervalTimer, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis)
    {
        Debug::TraceContextBumper ctx{"IntervalTimer::Manager: default implementation: AddRepeating"};
        fData_.rwget ()->Add ({timer, intervalTimer, Time::GetTickCount () + repeatInterval, repeatInterval, hysteresis});
        DataChanged_ ();
    }
    bool Remove (TimerID timer) noexcept
    {
        Debug::TraceContextBumper ctx{"IntervalTimer::Manager: default implementation: Remove"};
        bool                      wasThere;
        {
            unique_lock runningLock{fRunningMutex_};
            // not there: a one-shot already called (the timer thread removes it once its call ends), or removed already
            wasThere = fData_.rwget ()->RemoveIf (timer);
            // if it is running now, wait for that call to finish: once removed, it is not running, and not called again. But not
            // on the timer thread - the callback removing itself - which would wait for itself, forever
            if (tRunnerOf_ != this) {
                fRunningChanged_.wait (runningLock, [&] () { return fRunning_ != timer; });
            }
        }
        DataChanged_ ();
        return wasThere;
    }
    RegisteredTaskCollection GetAllRegisteredTasks () const
    {
        Debug::TraceContextBumper ctx{"IntervalTimer::Manager: default implementation: GetAllRegisteredTasks"};
        return fData_.load ();
    }

    // the timer whose callback the timer thread is running now, if any - so Remove can wait for it to finish. A
    // std::condition_variable, not a Stroika ConditionVariable: Remove is noexcept, so its wait must not be interruptible
    mutex              fRunningMutex_; // taken before fData_'s
    condition_variable fRunningChanged_;
    optional<TimerID>  fRunning_;

    // @todo - re-implement using priority q, with next time at top of q
    Synchronized<RegisteredTaskCollection>       fData_;
    WaitableEvent                                fDataChanged_{};
    Synchronized<shared_ptr<Thread::CleanupPtr>> fThread_{}; // last, so destroyed first: the thread is stopped before what it uses goes

    // this is where a priorityq would be better
    TimePointSeconds GetNextWakeupTime_ ()
    {
        TimePointSeconds funResult =
            fData_.cget ()->Map<Iterable<TimePointSeconds>> ([] (const RegisteredTask& i) { return i.fCallNextAt; }).MinValue (TimePointSeconds{kInfinity});
#if qStroika_Foundation_Debug_AssertionsChecked
        auto dataLock = fData_.cget ();
        // note: usually (not dataLock->empty ()), but it can be empty temporarily as we are shutting down this process
        // from one thread, while checking this simultaneously from another
        TimePointSeconds r = TimePointSeconds{kInfinity};
        for (const RegisteredTask& i : dataLock.cref ()) {
            r = min (r, i.fCallNextAt);
        }
        Assert (r == funResult);
#endif
        return funResult;
    }
    void RunnerLoop_ ()
    {
        tRunnerOf_ = this; // so a callback removing a timer is known to be on this thread
        // keep checking for timer events to run
        random_device rd;
        mt19937       gen{rd ()};
        while (true) {
            Require (Debug::AppearsDuringMainLifetime ());
            fDataChanged_.WaitUntilQuietly (GetNextWakeupTime_ ());
            fDataChanged_.Reset (); // SUBTLE why this is not a race. Yes, another thread could post new data, but we haven't yet looked through the data so OK; and only one reader (us)
            // now process any timer events that are ready (could easily be more than one).
            // if we had a priority q, we would do them in order, but for now, just do all that are ready
            // NOTE - to avoid holding a lock (in case these guys remove themselves or whatever) - run them from a copy of the list;
            // so each is looked up again before it is run, and again after - it may have been removed meanwhile
            TimePointSeconds           now      = Time::GetTickCount ();
            Collection<RegisteredTask> elts2Run = fData_.cget ()->Where ([=] (const RegisteredTask& i) { return i.fCallNextAt <= now; });
            // note - this could EASILY be empty, for example, if fDataChanged_ wakes too early due to a change/Signal/Set
            for (const RegisteredTask& i : elts2Run) {
                {
                    lock_guard runningLock{fRunningMutex_};
                    if (not fData_.cget ()->Contains (i.fID)) {
                        continue; // removed since the copy was taken
                    }
                    fRunning_ = i.fID;
                }
                [[maybe_unused]] auto&& doneRunning = Finally ([this] () noexcept {
                    {
                        lock_guard runningLock{fRunningMutex_};
                        fRunning_ = nullopt;
                    }
                    fRunningChanged_.notify_all (); // for a Remove waiting for it to finish
                });
                IgnoreExceptionsExceptThreadAbortForCall (i.fCallback ());
                // set its next time - from when it finished - unless it was removed while running: adding it then would bring it back
                auto rwDataLock = fData_.rwget ();
                if (optional<RegisteredTask> current = rwDataLock->Lookup (i.fID)) {
                    if (current->fFrequency.has_value ()) {
                        current->fCallNextAt = Time::GetTickCount () + *current->fFrequency;
                        if (current->fHysteresis) {
                            uniform_real_distribution<> dis{-current->fHysteresis->count (), current->fHysteresis->count ()};
                            current->fCallNextAt += Time::DurationSeconds{dis (gen)}; // can use fCallNextAt to be called immediately again... or even be < now
                        }
                        rwDataLock->Add (*current); // replaces it
                    }
                    else {
                        rwDataLock->Remove (i.fID); // a one-shot: done
                    }
                }
            }
        }
    }
    // The timer thread starts with the first timer, and stays - idle while there are none - until this goes. Stopping it as the
    // last timer went (before v3.0d25) waited for it holding fThread_'s lock: a deadlock, with a callback adding or removing a
    // timer just then, which needs that lock too.
    void DataChanged_ ()
    {
        auto lk = fThread_.rwget ();
        if (lk.cref () == nullptr) {
            if (not fData_.cget ()->empty ()) {
                using namespace Execution;
                lk.store (MakeSharedPtr<Thread::CleanupPtr> (Thread::CleanupPtr::eAbortBeforeWaiting,
                                                             Thread::New ([this] () { RunnerLoop_ (); }, Thread::eAutoStart, "Default-Interval-Timer"sv)));
            }
        }
        else {
            fDataChanged_.Set (); // it may be sleeping too long - or with no timer to wait for - so wake it up
        }
    }
};

IntervalTimer::Manager::DefaultRep::DefaultRep ()
    : fHiddenRep_{MakeSharedPtr<Rep_> ()}
{
}

void IntervalTimer::Manager::DefaultRep::AddOneShot (TimerID timer, const TimerCallback& intervalTimer, const Time::Duration& when)
{
    AssertNotNull (fHiddenRep_);
    fHiddenRep_->AddOneShot (timer, intervalTimer, when);
}

void IntervalTimer::Manager::DefaultRep::AddRepeating (TimerID timer, const TimerCallback& intervalTimer,
                                                       const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis)
{
    AssertNotNull (fHiddenRep_);
    fHiddenRep_->AddRepeating (timer, intervalTimer, repeatInterval, hysteresis);
}

bool IntervalTimer::Manager::DefaultRep::Remove (TimerID timer) noexcept
{
    AssertNotNull (fHiddenRep_);
    return fHiddenRep_->Remove (timer);
}

auto IntervalTimer::Manager::DefaultRep::GetAllRegisteredTasks () const -> RegisteredTaskCollection
{
    AssertNotNull (fHiddenRep_);
    return fHiddenRep_->GetAllRegisteredTasks ();
}

/*
 ********************************************************************************
 ********************* IntervalTimer::Manager::Activator ************************
 ********************************************************************************
 */
IntervalTimer::Manager::Activator::Activator ()
{
    Debug::TraceContextBumper ctx{"IntervalTimer::Manager::Activator::Activator"};
    Require (Manager::sThe.fRep_ == nullptr); // only one activator object allowed
    Require (Debug::AppearsDuringMainLifetime ());
    Manager::sThe = Manager{MakeSharedPtr<IntervalTimer::Manager::DefaultRep> ()};
}

IntervalTimer::Manager::Activator::~Activator ()
{
    Debug::TraceContextBumper ctx{"IntervalTimer::Manager::Activator::~Activator"};
    RequireNotNull (Manager::sThe.fRep_); // this is the only way to remove, and so must not be null here
    Require (Debug::AppearsDuringMainLifetime ());
    Manager::sThe.fRep_.reset ();
}

/*
 ********************************************************************************
 ***************************** IntervalTimer::Adder *****************************
 ********************************************************************************
 */
IntervalTimer::Adder::Adder (IntervalTimer::Manager& manager, const TimerCallback& f, const Time::Duration& repeatInterval,
                             RunImmediatelyFlag runImmediately, const optional<Time::Duration>& hysteresis)
    : fManager_{&manager}
    , fFunction_{f}
    , fTimer_{manager.AddRepeating (f, repeatInterval, hysteresis)}
{
    if (runImmediately == RunImmediatelyFlag::eRunImmediately) {
        IgnoreExceptionsExceptThreadAbortForCall (fFunction_ ());
    }
}
