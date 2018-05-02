/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
            inline WhenTimeExceeded::WhenTimeExceeded (Time::DurationSecondsType callIfTakesLongerThan, const function<void(Time::DurationSecondsType)>& f)
                : fStartedAt_ (Time::GetTickCount ())
                , fCallIfTakesLongerThan_ (callIfTakesLongerThan)
                , fRunIfTakesTooLong (f)
            {
            }
            inline WhenTimeExceeded::~WhenTimeExceeded ()
            {
                Time::DurationSecondsType timeTaken = Time::GetTickCount () - fStartedAt_;
                if (timeTaken >= fCallIfTakesLongerThan_) {
                    IgnoreExceptionsForCall (fRunIfTakesTooLong (timeTaken));
                }
            }
        }
    }
}

#endif /*_Stroika_Foundation_Execution_WhenTimeExceeded_inl_*/
