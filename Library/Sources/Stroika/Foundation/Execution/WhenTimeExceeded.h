/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WhenTimeExceeded_h_
#define _Stroika_Foundation_Execution_WhenTimeExceeded_h_ 1

#include "../StroikaPreComp.h"

#include "../Time/Realtime.h"

/**
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /**
             *  \note callIfTakesLongerThan is compared <=, so you can pass in zero to always trigger.
             *
             *  \par Example Usage
             *      \code
             *      WhenTimeExceeded    whenTimeExceeded (1.0, [] (DurationSecondsType timeTaken) { Logger::Get ().Log (Logger::eWarning, "Took along time  to do 'x'"); });
             *      \endcode
             */
            struct WhenTimeExceeded {
#if qCompilerAndStdLib_noexcept_declarator_in_std_function_Buggy
                WhenTimeExceeded (Time::DurationSecondsType callIfTakesLongerThan, const function<void(Time::DurationSecondsType)>& f);
#else
                WhenTimeExceeded (Time::DurationSecondsType callIfTakesLongerThan, const function<void(Time::DurationSecondsType) noexcept>& f);
#endif
                ~WhenTimeExceeded ();

            private:
                Time::DurationSecondsType fStartedAt_;
                Time::DurationSecondsType fCallIfTakesLongerThan_;
#if qCompilerAndStdLib_noexcept_declarator_in_std_function_Buggy
                function<void(Time::DurationSecondsType)> fRunIfTakesTooLong;
#else
                function<void(Time::DurationSecondsType) noexcept> fRunIfTakesTooLong;
#endif
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WhenTimeExceeded.inl"

#endif /*_Stroika_Foundation_Execution_WhenTimeExceeded_h_*/
