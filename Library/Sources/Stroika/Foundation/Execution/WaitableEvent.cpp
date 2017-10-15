/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Time/Duration.h"

#include "Common.h"
#include "TimeOutException.h"

#include "WaitableEvent.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using Stroika::Foundation::Time::Duration;

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
 ********************************************************************************
 ****************************** WaitableEvent::WE_ ******************************
 ********************************************************************************
 */
void WaitableEvent::WE_::WaitUntil (Time::DurationSecondsType timeoutAt)
{
    if (WaitUntilQuietly (timeoutAt) == kWaitQuietlyTimeoutResult) {
// note - safe use of TimeOutException::kThe because you cannot really wait except when threads are running, so
// inside 'main' lifetime
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        // only thing Throw() helper does is DbgTrace ()- and that can make traces hard to read unless you are debugging a timeout /event issue
        Throw (TimeOutException::kThe);
#else
        throw (TimeOutException::kThe);
#endif
    }
}

bool WaitableEvent::WE_::WaitUntilQuietly (Time::DurationSecondsType timeoutAt)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"WaitableEvent::WE_::WaitUntil", "timeout = %e", timeoutAt);
#endif
    CheckForThreadInterruption ();
    if (timeoutAt <= Time::GetTickCount ()) {
        return kWaitQuietlyTimeoutResult;
    }

    /*
     *  Note - this unique_lock<> looks like a bug, but is not. Internally, fConditionVariable_.wait_for does an
     *  unlock.
     */
    std::unique_lock<mutex> lock (fConditionVariable.fMutex);
    /*
     * The reason for the loop is that fConditionVariable_.wait_for() can return for things like errno==EINTR,
     * but must keep waiting. wait_for () returns no_timeout if for a real reason (notify called) OR spurious.
     */
    while (not fTriggered) {
        CheckForThreadInterruption ();
        Time::DurationSecondsType remaining = timeoutAt - Time::GetTickCount ();
        if (remaining < 0) {
            return kWaitQuietlyTimeoutResult;
        }

        /*
         *  See WaitableEvent::SetThreadAbortCheckFrequency ();
         */
        remaining = min (remaining, fConditionVariable.fThreadAbortCheckFrequency);

        if (fConditionVariable.fConditionVariable.wait_for (lock, Time::Duration (remaining).As<std::chrono::milliseconds> ()) == std::cv_status::timeout) {
            /*
             *  Cannot throw here because we trim time to wait so we can re-check for thread aborting. No need to pay attention to
             *  this timeout value (or any return code) - cuz we re-examine fTriggered and tickcount.
             *
             *      Throw (TimeOutException::kThe);
             */
        }
    }
    if (fResetType == eAutoReset) {
        // cannot call Reset () directly because we (may???) already have the lock mutex? Maybe not cuz of cond variable?
        fTriggered = false; // autoreset
    }
    return not kWaitQuietlyTimeoutResult;
}

/*
 ********************************************************************************
 ********************************** WaitableEvent *******************************
 ********************************************************************************
 */
constexpr WaitableEvent::ResetType WaitableEvent::eAutoReset;
constexpr WaitableEvent::ResetType WaitableEvent::eManualReset;

#if qDebug || qStroika_FeatureSupported_Valgrind
WaitableEvent::~WaitableEvent ()
{
    Require (fExtraWaitableEvents_.empty ());                                                      // Cannot kill a waitable event while its being waited on by others
    Stroika_Foundation_Debug_ValgrindDisableHelgrind (fWE_.fConditionVariable.fConditionVariable); // avoid sporadic (about 1/3 time) probably spurrious helgrind failure - for test Foundation::Execution::Threads -https://stroika.atlassian.net/browse/STK-484
}
#endif

void WaitableEvent::Set ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("WaitableEvent::Set");
#endif
    fWE_.Set ();
#if qExecution_WaitableEvent_SupportWaitForMultipleObjects
    auto critSec{make_unique_lock (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_)};
    for (auto i : fExtraWaitableEvents_) {
        i->Set ();
    }
#endif
}
