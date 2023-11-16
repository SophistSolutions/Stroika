/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WhenTimeExceeded_inl_
#define _Stroika_Foundation_Execution_WhenTimeExceeded_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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
            fStartedAt_ = Time::New_GetTickCount ();
        }
    }
    inline WhenTimeExceeded::~WhenTimeExceeded ()
    {
        if (fRunIfTakesTooLong_ != nullptr) {
            Time::DurationSeconds timeTaken = Time::New_GetTickCount () - fStartedAt_;
            if (timeTaken >= fCallIfTakesLongerThan_) {
                IgnoreExceptionsForCall (fRunIfTakesTooLong_ (timeTaken));
            }
        }
    }

}

#endif /*_Stroika_Foundation_Execution_WhenTimeExceeded_inl_*/
