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
        : fManager_{manager}
        , fFunction_{f}
    {
        Manager::sThe.AddRepeating (fFunction_, repeatInterval, hysteresis);
    }
    inline IntervalTimer::Adder::Adder (const Function<void (void)>& f, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis)
        : Adder{Manager::sThe, f, repeatInterval, hysteresis}
    {
    }
    inline IntervalTimer::Adder::~Adder ()
    {
        Manager::sThe.RemoveRepeating (fFunction_);
    }

}

#endif /*_Stroika_Foundation_Execution_IntervalTimer_inl_*/
