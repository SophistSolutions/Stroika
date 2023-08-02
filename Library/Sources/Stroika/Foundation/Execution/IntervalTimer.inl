/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_IntervalTimer_inl_
#define _Stroika_Foundation_Execution_IntervalTimer_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     *************************** IntervalTimer::Manager *****************************
     ********************************************************************************
     */
    inline IntervalTimer::Manager::Manager (const shared_ptr<IRep>& rep)
        : fRep_{rep}
    {
    }
    inline void IntervalTimer::Manager::AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when)
    {
        RequireNotNull (intervalTimer);
        Require (when >= 0s);
        RequireNotNull (fRep_); // If this fails, and its accessed through IntervalTimer::Manager::sThe, its probably because of lack of construction of IntervalTimer::Manager::Activator object.
        Require (not fRep_->GetAllRegisteredTasks ().Contains (intervalTimer));
        fRep_->AddOneShot (intervalTimer, when);
    }
    inline void IntervalTimer::Manager ::AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval,
                                                       const optional<Time::Duration>& hysteresis)
    {
        RequireNotNull (intervalTimer);
        Require (repeatInterval >= 0s);
        Require (hysteresis == nullopt or hysteresis >= 0s);
        RequireNotNull (fRep_); // If this fails, and its accessed through IntervalTimer::Manager::sThe, its probably because of lack of construction of IntervalTimer::Manager::Activator object.
        Require (not fRep_->GetAllRegisteredTasks ().Contains (intervalTimer));
        fRep_->AddRepeating (intervalTimer, repeatInterval, hysteresis);
    }
    inline void IntervalTimer::Manager::RemoveRepeating (const TimerCallback& intervalTimer) noexcept
    {
        RequireNotNull (intervalTimer);
        RequireNotNull (fRep_); // If this fails, and its accessed through IntervalTimer::Manager::sThe, its probably because of lack of construction of IntervalTimer::Manager::Activator object.
        Require (fRep_->GetAllRegisteredTasks ().Contains (intervalTimer));
        fRep_->RemoveRepeating (intervalTimer);
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
    inline IntervalTimer::Adder::Adder (const Function<void (void)>& f, const Time::Duration& repeatInterval,
                                        RunImmediatelyFlag runImmediately, const optional<Time::Duration>& hysteresis)
        : Adder{Manager::sThe, f, repeatInterval, runImmediately, hysteresis}
    {
    }
    inline IntervalTimer::Adder::Adder (IntervalTimer::Manager& manager, const Function<void (void)>& f,
                                        const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis)
        : Adder{manager, f, repeatInterval, RunImmediatelyFlag::eDontRunImmediately, hysteresis}
    {
    }
    inline IntervalTimer::Adder::Adder (const Function<void (void)>& f, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis)
        : Adder{Manager::sThe, f, repeatInterval, RunImmediatelyFlag::eDontRunImmediately, hysteresis}
    {
    }
    inline IntervalTimer::Adder::Adder (Adder&& src) noexcept
        : fRepeatInterval_{move (src.fRepeatInterval_)}
        , fHysteresis_{move (src.fHysteresis_)}
        , fManager_{src.fManager_}
        , fFunction_{move (src.fFunction_)}
    {
        // Move does not trigger re-add to manager
        src.fManager_ = nullptr; // so its DTOR does nothing
    }
    inline IntervalTimer::Adder::~Adder ()
    {
        if (fManager_ != nullptr) { // null check cuz Adder can be moved
            fManager_->RemoveRepeating (fFunction_);
        }
    }
    inline IntervalTimer::Adder& IntervalTimer::Adder::operator= (Adder&& rhs) noexcept
    {
        if (this != &rhs) {
            if (fManager_ != nullptr) { // null check cuz Adder can be moved
                fManager_->RemoveRepeating (fFunction_);
            }
            fManager_     = rhs.fManager_;
            rhs.fManager_ = nullptr; // so its DTOR doesnt remove
            fFunction_    = move (rhs.fFunction_);
            Manager::sThe.AddRepeating (fFunction_, fRepeatInterval_, fHysteresis_);
        }
        return *this;
    }
    inline Function<void (void)> IntervalTimer::Adder::GetCallback () const
    {
        return fFunction_;
    }

}

#endif /*_Stroika_Foundation_Execution_IntervalTimer_inl_*/
