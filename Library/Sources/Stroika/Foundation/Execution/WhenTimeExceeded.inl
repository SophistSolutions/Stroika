/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ************************ Execution::WhenTimeExceeded ***************************
     ********************************************************************************
     */
    inline WhenTimeExceeded::WhenTimeExceeded ([[maybe_unused]] Time::DurationSeconds callIfTakesLongerThan, [[maybe_unused]] nullptr_t f)
        : fRunIfTakesTooLong_{nullptr}
    {
    }
    inline WhenTimeExceeded::WhenTimeExceeded (Time::DurationSeconds callIfTakesLongerThan, const function<void (Time::DurationSeconds)>& f)
        : fCallIfTakesLongerThan_{callIfTakesLongerThan}
        , fRunIfTakesTooLong_{f}
    {
        if (f != nullptr) {
            fStartedAt_ = Time::GetTickCount ();
        }
    }
    inline WhenTimeExceeded::~WhenTimeExceeded ()
    {
        if (fRunIfTakesTooLong_ != nullptr) {
            Time::DurationSeconds timeTaken = Time::GetTickCount () - fStartedAt_;
            if (timeTaken >= fCallIfTakesLongerThan_) {
                IgnoreExceptionsForCall (fRunIfTakesTooLong_ (timeTaken));
            }
        }
    }

}
