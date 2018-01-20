/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include <unistd.h>

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
#if qCompilerAndStdLib_unnamed_semaphores_Buggy
    char nameBuf[1000] = "/tmp/semaphore-XXXXXX";
    ::close (::mkstemp (nameBuf)); // create file as side-effect so no race - file name still reserved by this CTOR
    Verify ((fSem_ = ::sem_open (nameBuf, O_CREAT, S_IRWXU | S_IRWXG, defaultValue)) != SEM_FAILED);
    Verify (::sem_unlink (nameBuf) == 0);
#else
    Verify (::sem_init (&fSem_, kpshared, defaultValue) == 0);
#endif
}

SemWaitableEvent::~SemWaitableEvent ()
{
#if qCompilerAndStdLib_unnamed_semaphores_Buggy
    Verify (::sem_close (fSem_) == 0);
#else
    Verify (::sem_destroy (&fSem_) == 0);
#endif
}

void SemWaitableEvent::Wait ()
{
#if qCompilerAndStdLib_unnamed_semaphores_Buggy
    sem_t* pSem = fSem_;
#else
    sem_t* pSem = &fSem_;
#endif
    int s{};
    while ((s = ::sem_wait (pSem)) == -1 && errno == EINTR) {
        continue; /* Restart if interrupted by handler */
    }
    Execution::ThrowErrNoIfNegative (s);
}

void SemWaitableEvent::Set ()
{
#if qCompilerAndStdLib_unnamed_semaphores_Buggy
    sem_t* pSem = fSem_;
#else
    sem_t* pSem = &fSem_;
#endif
    Verify (::sem_post (pSem) == 0);
}
