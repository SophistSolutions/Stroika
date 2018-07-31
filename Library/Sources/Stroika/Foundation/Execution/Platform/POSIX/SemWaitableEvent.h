/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_POSIX_SemWaitableEvent_h_
#define _Stroika_Foundation_Execution_Platform_POSIX_SemWaitableEvent_h_ 1

#include "../../../StroikaPreComp.h"

#include <semaphore.h>

#if !qPlatform_POSIX
#error "ONLY INCLUDE FOR POSIX"
#endif

/**
 *
 * TODO:
 */

namespace Stroika::Foundation::Execution::Platform::POSIX {

    /**
     *  Very simplistic wrapper on sem_init/sem_wait/sem_post POSIX semaphore APIs.
     *
     *  Useful because they work from signal handlers, when most other APIs don't.
     */
    class SemWaitableEvent {
    public:
        SemWaitableEvent ();
        SemWaitableEvent (const SemWaitableEvent&) = delete;

    public:
        ~SemWaitableEvent ();

    public:
        /**
         *  @see http://pubs.opengroup.org/onlinepubs/7908799/xsh/sem_wait.html
         */
        nonvirtual void Wait ();

    public:
        /**
         *  @see http://pubs.opengroup.org/onlinepubs/7908799/xsh/sem_post.html
         */
        nonvirtual void Set ();

    private:
#if qCompilerAndStdLib_unnamed_semaphores_Buggy
        sem_t* fSem_;
#else
        sem_t fSem_;
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SemWaitableEvent.inl"

#endif /*_Stroika_Foundation_Execution_Platform_POSIX_SemWaitableEvent_h_*/
