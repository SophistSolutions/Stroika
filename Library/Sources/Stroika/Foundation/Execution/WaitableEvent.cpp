/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Time/Duration.h"

#include    "WaitTimedOutException.h"

#include    "WaitableEvent.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;


using   Stroika::Foundation::Time::Duration;


// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1



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
 *      @todo   POSIX/C++ code below on wait is a bit of a KLUGE. Unclear if it was a red-herring or
 *              something like that needed. Review once threading stuff stable.
 *                  -- LGP 2011-10-27
 *              NB: its been working and stable for quite a while. It could use cleanup/docs (working on that).
 *              and the timeout part maybe wrong (esp with signals). But it appears to mostly be
 *              correct.
 */



/*
 ********************************************************************************
 ******************************** WaitableEvent *********************************
 ********************************************************************************
 */
void    WaitableEvent::WaitUntil (Time::DurationSecondsType timeoutAt)
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (SDKSTR ("WaitableEvent::WaitUntil"));
    DbgTrace ("(timeout = %.2f)", timeoutAt);
#endif
    CheckForThreadAborting ();
    if (timeoutAt <= Time::GetTickCount ()) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        // only thing DoThrow() helper does is DbgTrace ()- and that can make traces hard to read unless you are debugging a timeout /event issue
        DoThrow (WaitTimedOutException ());
#else
        throw (WaitTimedOutException ());
#endif
    }

    /*
     *  Note - this unique_lock<> looks like a bug, but is not. Internally, fConditionVariable_.wait_for does an
     *  unlock.
     */
    std::unique_lock<mutex>     lock (fMutex_);
    /*
     * The reason for the loop is that fConditionVariable_.wait_for() can return for things like errno==EINTR,
     * but must keep waiting. wait_for () returns no_timeout if for a real reason (notify called) OR spurrious.
     */
    while (not fTriggered_) {
        CheckForThreadAborting ();
        Time::DurationSecondsType   remaining   =   timeoutAt - Time::GetTickCount ();
        if (remaining < 0) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            // only thing DoThrow() helper does is DbgTrace ()- and that can make traces hard to read unless you are debugging a timeout /event issue
            DoThrow (WaitTimedOutException ());
#else
            throw (WaitTimedOutException ());
#endif
        }

        /*
         *  See WaitableEvent::SetThreadAbortCheckFrequency ();
         */
        remaining = min (remaining, fThreadAbortCheckFrequency_);

        if (fConditionVariable_.wait_for (lock, Time::Duration (remaining).As<std::chrono::milliseconds> ()) == std::cv_status::timeout) {
            /*
             *  Cannot throw here because we trim time to wait so we can re-check for thread aborting. No need to pay attention to
             *  this timeout value (or any return code) - cuz we re-examine fTriggered and tickcount.
             *
             *      DoThrow (WaitTimedOutException ());
             */
        }
    }
    fTriggered_ = false ;   // autoreset
}
