/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     *************************** IntervalTimer::TimerID *****************************
     ********************************************************************************
     */
    constexpr IntervalTimer::TimerID::TimerID (uint64_t id)
        : fID_{id}
    {
    }

    /*
     ********************************************************************************
     *************************** IntervalTimer::Manager *****************************
     ********************************************************************************
     */
    inline IntervalTimer::Manager::Manager (const shared_ptr<IRep>& rep)
        : fRep_{rep}
    {
    }
    inline auto IntervalTimer::Manager::AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when) -> TimerID
    {
        RequireNotNull (intervalTimer);
        Require (when >= 0s);
        RequireNotNull (fRep_); // If this fails, and its accessed through IntervalTimer::Manager::sThe, its probably because of lack of construction of IntervalTimer::Manager::Activator object.
        TimerID timer = NewTimerID_ ();
        fRep_->AddOneShot (timer, intervalTimer, when);
        return timer;
    }
    inline auto IntervalTimer::Manager ::AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval,
                                                       const optional<Time::Duration>& hysteresis) -> TimerID
    {
        RequireNotNull (intervalTimer);
        Require (repeatInterval >= 0s);
        Require (hysteresis == nullopt or hysteresis >= 0s);
        RequireNotNull (fRep_); // If this fails, and its accessed through IntervalTimer::Manager::sThe, its probably because of lack of construction of IntervalTimer::Manager::Activator object.
        TimerID timer = NewTimerID_ ();
        fRep_->AddRepeating (timer, intervalTimer, repeatInterval, hysteresis);
        return timer;
    }
    inline void IntervalTimer::Manager::RemoveRepeating (TimerID timer) noexcept
    {
        RequireNotNull (fRep_); // If this fails, and its accessed through IntervalTimer::Manager::sThe, its probably because of lack of construction of IntervalTimer::Manager::Activator object.
        Require (fRep_->GetAllRegisteredTasks ().Contains (timer));
        fRep_->RemoveRepeating (timer);
    }
    inline auto IntervalTimer::Manager::GetAllRegisteredTasks () const -> RegisteredTaskCollection
    {
        RequireNotNull (fRep_); // If this fails, and its accessed through IntervalTimer::Manager::sThe, its probably because of lack of construction of IntervalTimer::Manager::Activator object.
        return fRep_->GetAllRegisteredTasks ();
    }
    inline IntervalTimer::Manager IntervalTimer::Manager::sThe{nullptr};

    /*
     ********************************************************************************
     ***************************** IntervalTimer::Adder *****************************
     ********************************************************************************
     */
    inline IntervalTimer::Adder::Adder (const TimerCallback& f, const Time::Duration& repeatInterval, RunImmediatelyFlag runImmediately,
                                        const optional<Time::Duration>& hysteresis)
        : Adder{Manager::sThe, f, repeatInterval, runImmediately, hysteresis}
    {
    }
    inline IntervalTimer::Adder::Adder (IntervalTimer::Manager& manager, const TimerCallback& f, const Time::Duration& repeatInterval,
                                        const optional<Time::Duration>& hysteresis)
        : Adder{manager, f, repeatInterval, RunImmediatelyFlag::eDontRunImmediately, hysteresis}
    {
    }
    inline IntervalTimer::Adder::Adder (const TimerCallback& f, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis)
        : Adder{Manager::sThe, f, repeatInterval, RunImmediatelyFlag::eDontRunImmediately, hysteresis}
    {
    }
    inline IntervalTimer::Adder::Adder (Adder&& src) noexcept
        : fManager_{src.fManager_}
        , fFunction_{move (src.fFunction_)}
        , fTimer_{src.fTimer_}
    {
        src.fManager_ = nullptr; // so its DTOR does nothing (and a move adds nothing)
    }
    inline IntervalTimer::Adder::~Adder ()
    {
        if (fManager_ != nullptr) { // null if moved from
            fManager_->RemoveRepeating (fTimer_);
        }
    }
    inline IntervalTimer::Adder& IntervalTimer::Adder::operator= (Adder&& rhs) noexcept
    {
        if (this != &rhs) {
            if (fManager_ != nullptr) { // null if moved from
                fManager_->RemoveRepeating (fTimer_);
            }
            fManager_     = rhs.fManager_;
            fFunction_    = move (rhs.fFunction_);
            fTimer_       = rhs.fTimer_;
            rhs.fManager_ = nullptr; // so its DTOR does not remove the timer, now this one's
        }
        return *this;
    }
    inline auto IntervalTimer::Adder::GetCallback () const -> TimerCallback
    {
        return fFunction_;
    }

}
