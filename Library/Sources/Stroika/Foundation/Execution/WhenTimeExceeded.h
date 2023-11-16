/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WhenTimeExceeded_h_
#define _Stroika_Foundation_Execution_WhenTimeExceeded_h_ 1

#include "../StroikaPreComp.h"

#include "../Time/Realtime.h"

/**
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Execution {

    /**
     *  \note callIfTakesLongerThan is compared <=, so you can pass in zero to always trigger.
     *
     *  \par Example Usage
     *      \code
     *          WhenTimeExceeded    whenTimeExceeded{1.0s, [] (DurationSeconds timeTaken) { Logger::sThe.Log (Logger::eWarning, "Took along time  to do 'x'"); }};
     *      \endcode
     * 
     *  \note this can be hopefully, mostly, optimized away if passed a nullptr_t constructor argument, or perhaps even if passed a null
     *        value of f
     */
    struct WhenTimeExceeded {
        /**
         *  note - the called function f can be nullptr, in which case this does nothing.
         */
        WhenTimeExceeded (Time::DurationSeconds callIfTakesLongerThan, nullptr_t f);
        WhenTimeExceeded (Time::DurationSeconds callIfTakesLongerThan, const function<void (Time::DurationSeconds)>& f);
        ~WhenTimeExceeded ();

    private:
        Time::DurationSecondsTimePoint         fStartedAt_;
        Time::DurationSeconds                  fCallIfTakesLongerThan_;
        function<void (Time::DurationSeconds)> fRunIfTakesTooLong_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WhenTimeExceeded.inl"

#endif /*_Stroika_Foundation_Execution_WhenTimeExceeded_h_*/
