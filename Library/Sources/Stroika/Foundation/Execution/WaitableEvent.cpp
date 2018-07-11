/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
    unique_lock<mutex> lock (fConditionVariable.fMutex);
    if (fConditionVariable.wait_until (lock, timeoutAt, [this]() { return fTriggered; })) {
        if (fResetType == eAutoReset) {
            // cannot call Reset () directly because we (may???) already have the lock mutex? Maybe not cuz of cond variable?
            fTriggered = false; // autoreset
        }
        return not kWaitQuietlyTimeoutResult;
    }
    else {
        return kWaitQuietlyTimeoutResult;
    }
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
    Require (fExtraWaitableEvents_.empty ()); // Cannot kill a waitable event while its being waited on by others
}
#endif

void WaitableEvent::Set ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("WaitableEvent::Set");
#endif
    fWE_.Set ();
#if qExecution_WaitableEvent_SupportWaitForMultipleObjects
    [[maybe_unused]] auto&& critSec = lock_guard{_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_};
    for (auto i : fExtraWaitableEvents_) {
        i->Set ();
    }
#endif
}
