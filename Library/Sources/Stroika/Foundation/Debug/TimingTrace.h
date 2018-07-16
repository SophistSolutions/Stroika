/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_TimingTrace_h_
#define _Stroika_Foundation_Debug_TimingTrace_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Execution/WhenTimeExceeded.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

namespace Stroika::Foundation::Debug {

    /**
     *  Simple utility to print debug trace timing values for procedures (or contexts).
     *
     *  \par Example Usage
     *      \code
     *      {
     *          TimingTrace ttrc;
     *          do_some_call();
     *      }
     *      \endcode
     *      \code
     *      OUTPUT IN TRACELOG:
     *          (timeTaken=3.4 seconds)
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *      {
     *          TraceContextBumper ctx ("do_some_call");
     *          TimingTrace         ttrc;
     *          do_some_call();
     *      }
     *      \endcode
     *      \code
     *      OUTPUT IN TRACELOG:
     *          <do_some_call>
     *              (timeTaken=3.4 seconds)
     *          </do_some_call>
     *      \endcode
     *
     * \note   Aliases Performance Trace, PerforamnceTrace
     */
    class TimingTrace : private Execution::WhenTimeExceeded {
    public:
        TimingTrace (Time::DurationSecondsType warnIfLongerThan = 0.0);
        TimingTrace (const char* label, Time::DurationSecondsType warnIfLongerThan = 0.0);
        TimingTrace (const wchar_t* label, Time::DurationSecondsType warnIfLongerThan = 0.0);
        TimingTrace (const Characters::String& label, Time::DurationSecondsType warnIfLongerThan = 0.0);
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimingTrace.inl"

#endif /*_Stroika_Foundation_Debug_TimingTrace_h_*/
