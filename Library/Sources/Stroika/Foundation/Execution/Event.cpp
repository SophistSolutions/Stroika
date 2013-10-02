/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Time/Duration.h"

#include    "Event.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;


using   Stroika::Foundation::Time::Duration;


/*
 * Design notes:
 *
 *      o   The use of condition variables is non-obvious. I haven't found good documentation, but
 *          the best I've found would be
 *              https://computing.llnl.gov/tutorials/pthreads/#ConVarOverview
 *
 *          In particular, on the surface, it looks like the mutex locks in Wait() and signal should prevent things
 *          form working (deadlock). But they apparently do not cause a deadlock because
 *              "pthread_cond_wait() blocks the calling thread until the specified condition is signalled.
 *               This routine should be called while mutex is locked, and it will automatically release the
 *               mutex while it waits. After signal is received and thread is awakened, mutex will be
 *               automatically locked for use by the thread. The programmer is then responsible for
 *               unlocking mutex when the thread is finished with it."
 *
 */


/*
 * TODO:
 *          o   POSIX/C++ code below on wait is a bit of a KLUGE. Unclear if it was a red-herring or
 *              osmething like that needed. Review once threading stuff stable.
 *                  -- LGP 2011-10-27
 *              NB: its been working and stable for quite a while. It could use cleanup/docs (working on that).
 *              and the timeout part maybe wrong (esp with signals). But it appears to mostly be
 *              correct.
 */





/*
 ********************************************************************************
 ************************************** Event ***********************************
 ********************************************************************************
 */
#if     qTrack_ThreadUtils_HandleCounts
uint32_t    Event::sCurAllocatedHandleCount     =   0;
#endif
void    Event::Wait (Time::DurationSecondsType timeout)
{
    //Debug::TraceContextBumper ctx (SDKSTR ("Event::Wait"));
    //DbgTrace ("(timeout = %.2f)", timeout);
    CheckForThreadAborting ();
    if (timeout < 0) {
        DoThrow (WaitTimedOutException ());
    }
#if         qUseThreads_WindowsNative
    AssertNotNull (fEventHandle);
    // must be careful about rounding errors in int->DurationSecondsType->int
Again:
    DWORD   result  =   ::WaitForSingleObjectEx (fEventHandle, Platform::Windows::Duration2Milliseconds (timeout), true);
    switch (result) {
        case    WAIT_TIMEOUT:
            DoThrow (WaitTimedOutException ());
        case    WAIT_ABANDONED:
            DoThrow (WaitAbandonedException ());
        case    WAIT_IO_COMPLETION:
            CheckForThreadAborting ();
            goto Again;  // roughly right to goto again - should decrement timeout- APC other than for abort - we should just keep waiting
    }
    Verify (result == WAIT_OBJECT_0);
#elif       qUseThreads_StdCPlusPlus
    std::unique_lock<std::mutex> lock (fMutex_);
    Time::DurationSecondsType   until   =   Time::GetTickCount () + timeout;
    Assert (until >= timeout);  // so no funny overflow issues...
    /*
     * The reason for the loop is that fConditionVariable_.wait_for() can return for things like errno==EINTR,
     * but must keep waiting. wait_for () returns no_timeout if for a real reason (notify called) OR spurrious.
     */
    while (not fTriggered_) {
        CheckForThreadAborting ();
        Time::DurationSecondsType   remaining   =   until - Time::GetTickCount ();
        if (remaining < 0) {
            DoThrow (WaitTimedOutException ());
        }
        // avoid roundoff issues - and not  a big deal to wakeup and wait again once a day ;-)
        const Time::DurationSecondsType k1Day   =   Time::DurationSecondsType (60 * 60 * 24);
        if (remaining >= k1Day) {
            remaining = k1Day;
        }
#if     qEVENT_GCCTHREADS_LINUX_WAITBUG
        remaining = min (remaining, 5.0);       // hack to force quick wakeup shutting down threadpool (and other threads waiting on a semaphore)
#endif

        if (fConditionVariable_.wait_for (lock, Time::Duration (remaining).As<std::chrono::milliseconds> ()) == std::cv_status::timeout) {
            // No need for this, since could be caught next time through the loop...
            // And it interferes with the bounding of the 'remaining' count used to avoid overflows
            // DoThrow (WaitTimedOutException ());
        }

    }
    fTriggered_ = false ;   // autoreset
#else
    AssertNotImplemented ();
#endif
}
