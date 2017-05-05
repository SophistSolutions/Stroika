/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WhenTimeExceeded_inl_
#define _Stroika_Foundation_Execution_WhenTimeExceeded_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

/*
             ********************************************************************************
             ************************ Execution::WhenTimeExceeded ***************************
             ********************************************************************************
             */
#if qCompilerAndStdLib_noexcept_declarator_in_std_function_Buggy
            inline WhenTimeExceeded::WhenTimeExceeded (Time::DurationSecondsType callIfTakesLongerThan, const function<void(Time::DurationSecondsType)>& f)
                : fStartedAt_ (Time::GetTickCount ())
                , fCallIfTakesLongerThan_ (callIfTakesLongerThan)
                , fRunIfTakesTooLong (f)
            {
            }
#else
            inline WhenTimeExceeded::WhenTimeExceeded (Time::DurationSecondsType callIfTakesLongerThan, const function<void(Time::DurationSecondsType) noexcept>& f)
                : fStartedAt_ (Time::GetTickCount ())
                , fCallIfTakesLongerThan_ (callIfTakesLongerThan)
                , fRunIfTakesTooLong (f)
            {
            }
#endif
            inline WhenTimeExceeded::~WhenTimeExceeded ()
            {
                Time::DurationSecondsType timeTaken = Time::GetTickCount () - fStartedAt_;
                if (timeTaken >= fCallIfTakesLongerThan_) {
                    fRunIfTakesTooLong (timeTaken);
                }
            }
        }
    }
}

#endif /*_Stroika_Foundation_Execution_WhenTimeExceeded_inl_*/
