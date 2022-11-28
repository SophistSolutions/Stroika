/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <random>

#include "../Characters/StringBuilder.h"
#include "../Characters/ToString.h"
#include "../Containers/Collection.h"
#include "../Debug/Main.h"
#include "../Debug/Trace.h"
#include "../Time/Realtime.h"

#include "Synchronized.h"
#include "Thread.h"

#include "IntervalTimer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Time;

/*
 ********************************************************************************
 *********************** IntervalTimer::RegisteredTask **************************
 ********************************************************************************
 */
Characters::String IntervalTimer::RegisteredTask::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Callback: " + Characters::ToString (fCallback) + L", ";
    sb += L"CallNextAt: " + Characters::ToString (fCallNextAt) + L", ";
    sb += L"Frequency: " + Characters::ToString (fFrequency) + L", ";
    sb += L"Hysteresis: " + Characters::ToString (fHysteresis) + L", ";
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ******************** IntervalTimer::Manager::DefaultRep ************************
 ********************************************************************************
 */
struct IntervalTimer::Manager::DefaultRep ::Rep_ {
    Rep_ ()          = default;
    virtual ~Rep_ () = default;
    void AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when)
    {
        Debug::TraceContextBumper ctx{L"IntervalTimer::Manager: default implementation: AddOneShot"};
        auto                      lk = fData_.rwget ();
        lk->Add (RegisteredTask{intervalTimer, Time::GetTickCount () + when.As<DurationSecondsType> ()});
        DataChanged_ ();
    }
    void AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis)
    {
        Debug::TraceContextBumper ctx{L"IntervalTimer::Manager: default implementation: AddRepeating"};
        auto                      lk = fData_.rwget ();
        lk->Add ({intervalTimer, Time::GetTickCount () + repeatInterval.As<DurationSecondsType> (), repeatInterval, hysteresis});
        DataChanged_ ();
    }
    void RemoveRepeating (const TimerCallback& intervalTimer) noexcept
    {
        Debug::TraceContextBumper ctx{L"IntervalTimer::Manager: default implementation: RemoveRepeating"};
        auto                      lk = fData_.rwget ();
        RegisteredTaskCollection  x  = lk.cref ();
        lk->Remove (intervalTimer);
        DataChanged_ ();
    }
    RegisteredTaskCollection GetAllRegisteredTasks () const
    {
        Debug::TraceContextBumper ctx{L"IntervalTimer::Manager: default implementation: GetAllRegisteredTasks"};
        return fData_.load ();
    }

    // @todo - re-implement using priority q, with next time at top of q
    Synchronized<RegisteredTaskCollection>       fData_;
    Synchronized<shared_ptr<Thread::CleanupPtr>> fThread_{};
    WaitableEvent                                fDataChanged_{WaitableEvent::ResetType::eAutoReset};

    // this is where a priorityq would be better
    DurationSecondsType GetNextWakeupTime_ ()
    {
        DurationSecondsType funResult = fData_.cget ()->Map<DurationSecondsType> ([] (const RegisteredTask& i) { return i.fCallNextAt; }).MinValue (kInfinite);
#if qDebug
        auto dataLock = fData_.cget ();
        // note: usually (not dataLock->empty ()), but it can be empty temporarily as we are shutting down this process
        // from one thread, while checking this simultaneously from another
        DurationSecondsType r = kInfinite;
        for (const RegisteredTask& i : dataLock.cref ()) {
            r = min (r, i.fCallNextAt);
        }
        Assert (r == funResult);
#endif
        return funResult;
    }
    void RunnerLoop_ ()
    {
        // keep checking for timer events to run
        random_device rd;
        mt19937       gen{rd ()};
        while (true) {
            Require (Debug::AppearsDuringMainLifetime ());
            DurationSecondsType wakeupAt = GetNextWakeupTime_ ();
            WeakAssert (wakeupAt > Time::GetTickCount ()); // could be violated if unlucky, but noteworthy, and most likely a bug/issue
            fDataChanged_.WaitUntilQuietly (wakeupAt);
            // now process any timer events that are ready (could easily be more than one).
            // if we had a priority q, we would do them in order, but for now, just do all that are ready
            // NOTE - to avoid holding a lock (in case these guys remove themselves or whatever) - lock/run through list twice
            DurationSecondsType now      = Time::GetTickCount ();
            Collection<RegisteredTask> elts2Run = fData_.cget ()->Where ([=] (const RegisteredTask& i) { return i.fCallNextAt <= now; });
            // note - this could EASILY be empty, for example, if fDataChanged_ wakes too early due to a change/Signal/Set
            for (const RegisteredTask& i : elts2Run) {
                IgnoreExceptionsExceptThreadAbortForCall (i.fCallback ());
            }
            // now reset the 'next' time for each run element
            auto rwDataLock = fData_.rwget ();
            now             = Time::GetTickCount ();
            for (const RegisteredTask& i : elts2Run) {
                if (i.fFrequency.has_value ()) {
                    RegisteredTask newE = i;
                    newE.fCallNextAt = now + i.fFrequency->As<DurationSecondsType> ();
                    if (i.fHysteresis) {
                        uniform_real_distribution<> dis{-i.fHysteresis->As<DurationSecondsType> (), i.fHysteresis->As<DurationSecondsType> ()};
                        newE.fCallNextAt += dis (gen); // can use fCallNextAt to be called immediately again... or even be < now
                    }
                    rwDataLock->Add (newE); // just replaces
                }
                else {
                    rwDataLock->Remove (i);
                }
            }
        }
    }
    void DataChanged_ ()
    {
        auto rwThreadLk = fThread_.rwget ();
        if (fData_.cget ()->empty ()) {
            rwThreadLk.store (nullptr); // destroy thread
        }
        else {
            auto lk = fThread_.rwget ();
            if (lk.cref () == nullptr) {
                using namespace Execution;
                lk.store (make_shared<Thread::CleanupPtr> (Thread::CleanupPtr::eAbortBeforeWaiting,
                                                           Thread::New ([this] () {
                                                               RunnerLoop_ ();
                                                           },
                                                                        Thread::eAutoStart, L"Default-Interval-Timer"sv)));
            }
            else {
                fDataChanged_.Set (); // if there was and still is a thread, it maybe sleeping too long, so wake it up
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
    Debug::TraceContextBumper ctx{L"IntervalTimer::Manager::Activator::Activator"};
    Require (Manager::sThe.fRep_ == nullptr); // only one activator object allowed
    Require (Debug::AppearsDuringMainLifetime ());
    Manager::sThe = Manager{make_shared<IntervalTimer::Manager::DefaultRep> ()};
}

IntervalTimer::Manager::Activator::~Activator ()
{
    Debug::TraceContextBumper ctx{L"IntervalTimer::Manager::Activator::~Activator"};
    RequireNotNull (Manager::sThe.fRep_); // this is the only way to remove, and so must not be null here
    Require (Debug::AppearsDuringMainLifetime ());
    Manager::sThe.fRep_.reset ();
}

/*
 ********************************************************************************
 ***************************** IntervalTimer::Adder *****************************
 ********************************************************************************
 */
IntervalTimer::Adder::Adder (IntervalTimer::Manager& manager, const Function<void (void)>& f, const Time::Duration& repeatInterval, RunImmediatelyFlag runImmediately, const optional<Time::Duration>& hysteresis)
    : fRepeatInterval_{repeatInterval}
    , fHysteresis_{hysteresis}
    , fManager_{&manager}
    , fFunction_{f}
{
    Manager::sThe.AddRepeating (fFunction_, repeatInterval, hysteresis);
    if (runImmediately == RunImmediatelyFlag::eRunImmediately) {
        IgnoreExceptionsExceptThreadAbortForCall (fFunction_ ());
    }
}
