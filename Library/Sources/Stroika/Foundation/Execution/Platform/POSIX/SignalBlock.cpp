/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include <mutex>

#include "SignalBlock.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Platform::POSIX;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ************* Execution::ScopedBlockCurrentThreadSignal ************************
 ********************************************************************************
 */
ScopedBlockCurrentThreadSignal::ScopedBlockCurrentThreadSignal ()
    : fRestoreMask_ ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"ScopedBlockCurrentThreadSignal blocking signals all signals");
#endif
    sigset_t mySet;
    Verify (sigemptyset (&mySet) == 0);         // nb: cannot use :: cuz crapple uses macro --LGP 2016-12-31
    Verify (sigfillset (&mySet) == 0);          // ""
    Verify (sigemptyset (&fRestoreMask_) == 0); // Unclear if this emptyset call is needed?
    Verify (::pthread_sigmask (SIG_BLOCK, &mySet, &fRestoreMask_) == 0);
}

ScopedBlockCurrentThreadSignal::ScopedBlockCurrentThreadSignal (SignalID signal)
    : fRestoreMask_ ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"ScopedBlockCurrentThreadSignal blocking signals for %s", SignalToName (signal).c_str ());
#endif
    sigset_t mySet;
    Verify (sigemptyset (&mySet) == 0); // nb: cannot use :: cuz crapple uses macro --LGP 2016-12-31
    Verify (sigaddset (&mySet, signal) == 0);
    Verify (sigemptyset (&fRestoreMask_) == 0); // Unclear if this emptyset call is needed?
    Verify (::pthread_sigmask (SIG_BLOCK, &mySet, &fRestoreMask_) == 0);
}

ScopedBlockCurrentThreadSignal::~ScopedBlockCurrentThreadSignal ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"ScopedBlockCurrentThreadSignal restoring signals");
#endif
    Verify (::pthread_sigmask (SIG_SETMASK, &fRestoreMask_, nullptr) == 0);
}
