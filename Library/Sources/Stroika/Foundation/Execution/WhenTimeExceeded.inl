/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
    inline WhenTimeExceeded::WhenTimeExceeded ([[maybe_unused]] Time::DurationSecondsType callIfTakesLongerThan, [[maybe_unused]] nullptr_t f)
        : fRunIfTakesTooLong_{nullptr}
    {
    }
    inline WhenTimeExceeded::WhenTimeExceeded (Time::DurationSecondsType callIfTakesLongerThan, const function<void (Time::DurationSecondsType)>& f)
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
            Time::DurationSecondsType timeTaken = Time::GetTickCount () - fStartedAt_;
            if (timeTaken >= fCallIfTakesLongerThan_) {
                IgnoreExceptionsForCall (fRunIfTakesTooLong_ (timeTaken));
            }
        }
    }

}

#endif /*_Stroika_Foundation_Execution_WhenTimeExceeded_inl_*/
