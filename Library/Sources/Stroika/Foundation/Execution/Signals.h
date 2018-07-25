/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Signals_h_
#define _Stroika_Foundation_Execution_Signals_h_ 1

#include "../StroikaPreComp.h"

#include <csignal>
#include <thread>

#include "../Characters/String.h"
#include "../Configuration/Common.h"

/**
 * Description:
 *
 *      This module defines support for POSIX (and std c++ defined) Signals (not to be confused
 *  with the 'Signals and slots' design pattern which is largely unrelated).
 *
 *
 * TODO:
 *
 */

namespace Stroika::Foundation::Execution {

#if !qCompilerAndStdLib_Supports_errno_t
    using errno_t = int;
#endif

    /**
     */
    using SignalID = int;

    /**
     */
    Characters::String SignalToName (SignalID signal);

    /**
     * Send the given signal to a specific thread (within this process)
     *
     *  @todo EXPLAIN HOW THIS INTERACTS WITH SignalHandledr::Tyep::eSafe!!!! - use thius with eDIRECT!
     *
     *  This function returns an errno error number.
     *      o   if the error number is ESRCH, this just menans the target thread doesn't
     *          exist anymore, whcih can easily happen (race) - like if you are sending an abort
     *          to a thread but it exits first.
     *
     *      o   other error numbres - for now - generate an assertion error. The function returns
     *          zero if no error
     */
    errno_t SendSignal (thread::native_handle_type target, SignalID signal);
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Signals.inl"

#endif /*_Stroika_Foundation_Execution_Signals_h_*/
