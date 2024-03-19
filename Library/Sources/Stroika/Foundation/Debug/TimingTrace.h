/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_TimingTrace_h_
#define _Stroika_Foundation_Debug_TimingTrace_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Execution/WhenTimeExceeded.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Debug {

    /**
     *  Simple utility to print debug trace timing values for procedures (or contexts).
     *
     *  \par Example Usage
     *      \code
     *          {
     *              TimingTrace ttrc;
     *              do_some_call();
     *          }
     *      \endcode
     *      \code
     *          OUTPUT IN TRACELOG:
     *              (timeTaken=3.4 seconds)
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          {
     *              TraceContextBumper ctx{"do_some_call"};
     *              TimingTrace         ttrc;
     *              do_some_call();
     *          }
     *      \endcode
     *      \code
     *          OUTPUT IN TRACELOG:
     *              <do_some_call>
     *                  (timeTaken=3.4 seconds)
     *              </do_some_call>
     *      \endcode
     *
     * \note   This implementation should mostly disappear when qStroika_Foundation_Debug_Trace_DefaultTracingOn is disabled (and so the DbgTrace() calls wouldn't do anything)
     *         To do something like this when when qStroika_Foundation_Debug_Trace_DefaultTracingOn is off, see the base class WhenTimeExceeded
     *
     * \note   Aliases Performance Trace, PerformanceTrace
     */
    class TimingTrace : private Execution::WhenTimeExceeded {
    public:
        TimingTrace (Time::DurationSeconds warnIfLongerThan = 0s);
        TimingTrace (const char* label, Time::DurationSeconds warnIfLongerThan = 0s);
        TimingTrace (const wchar_t* label, Time::DurationSeconds warnIfLongerThan = 0s);
        TimingTrace (const Characters::String& label, Time::DurationSeconds warnIfLongerThan = 0s);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimingTrace.inl"

#endif /*_Stroika_Foundation_Debug_TimingTrace_h_*/
