/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_POSIX_SignalBlock_h_
#define _Stroika_Foundation_Execution_Platform_POSIX_SignalBlock_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <csignal>
#include <mutex>

#if !qStroika_Foundation_Common_Platform_POSIX
#error "ONLY INCLUDE FOR POSIX"
#endif

#include "Stroika/Foundation/Execution/Signals.h"

/**
 *
 * TODO:
 */

namespace Stroika::Foundation::Execution::Platform::POSIX {

    /**
     *  For the lifetime of this object - save the initial signal block state for thread signals,
     *  and then block the given signal (or all for no arg CTOR). Restore on destructor.
     *
     *  This is similar to sigblock/sigprocmask to resture (in DTOR), but only applying to the current thread,
     *  and it takes a signal as argument, not a set/mask.
     */
    class ScopedBlockCurrentThreadSignal {
    public:
        ScopedBlockCurrentThreadSignal ();
        ScopedBlockCurrentThreadSignal (SignalID signal);
        ScopedBlockCurrentThreadSignal (const ScopedBlockCurrentThreadSignal&) = delete;
        ~ScopedBlockCurrentThreadSignal ();

    public:
        nonvirtual ScopedBlockCurrentThreadSignal& operator= (const ScopedBlockCurrentThreadSignal&) = delete;

    private:
        sigset_t fRestoreMask_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SignalBlock.inl"

#endif /*_Stroika_Foundation_Execution_Platform_POSIX_SignalBlock_h_*/
