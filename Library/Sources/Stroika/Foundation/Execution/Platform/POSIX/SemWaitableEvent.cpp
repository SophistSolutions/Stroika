/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../ErrNoException.h"

#include "SemWaitableEvent.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Platform::POSIX;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ************* Execution::Platform::POSIX::SemWaitableEvent *********************
 ********************************************************************************
 */
SemWaitableEvent::SemWaitableEvent ()
{
    constexpr int kpshared     = 0; // shared between threads of a process
    int           defaultValue = 0;
    Verify (::sem_init (&fSem_, kpshared, defaultValue) == 0);
}

SemWaitableEvent::~SemWaitableEvent ()
{
    Verify (::sem_destroy (&fSem_) == 0);
}

void SemWaitableEvent::Wait ()
{
    int s{};
    while ((s = ::sem_wait (&fSem_)) == -1 && errno == EINTR)
        continue; /* Restart if interrupted by handler */
    Execution::ThrowErrNoIfNegative (s);
}

void SemWaitableEvent::Set ()
{
    Verify (::sem_post (&fSem_) == 0);
}
