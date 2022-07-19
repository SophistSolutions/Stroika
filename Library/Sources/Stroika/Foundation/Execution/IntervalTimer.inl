/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
        RequireNotNull (rep);
    }
    inline void IntervalTimer::Manager::AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when)
    {
        fRep_->AddOneShot (intervalTimer, when);
    }
    inline void IntervalTimer::Manager ::AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis)
    {
        fRep_->AddRepeating (intervalTimer, repeatInterval, hysteresis);
    }
    inline void IntervalTimer::Manager::RemoveRepeating (const TimerCallback& intervalTimer) noexcept
    {
        fRep_->RemoveRepeating (intervalTimer);
    }

    /*
     ********************************************************************************
     ***************************** IntervalTimer::Adder *****************************
     ********************************************************************************
     */
    inline IntervalTimer::Adder::Adder (IntervalTimer::Manager& manager, const Function<void (void)>& f, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis)
        : fRepeatInterval_{repeatInterval}
        , fHysteresis_{hysteresis}
        , fManager_{&manager}
        , fFunction_{f}
    {
        Manager::sThe.AddRepeating (fFunction_, repeatInterval, hysteresis);
    }
    inline IntervalTimer::Adder::Adder (const Function<void (void)>& f, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis)
        : Adder{Manager::sThe, f, repeatInterval, hysteresis}
    {
    }
    inline IntervalTimer::Adder::Adder (Adder&& src)
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
    inline IntervalTimer::Adder& IntervalTimer::Adder::operator= (Adder&& rhs)
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

}

#endif /*_Stroika_Foundation_Execution_IntervalTimer_inl_*/
