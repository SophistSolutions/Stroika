/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
void WaitableEvent::WE_::WaitUntil (Time::TimePointSeconds timeoutAt)
{
    if (WaitUntilQuietly (timeoutAt) == WaitStatus::eTimeout) {
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

auto WaitableEvent::WE_::WaitUntilQuietly (Time::TimePointSeconds timeoutAt) -> WaitStatus
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{L"WaitableEvent::WE_::WaitUntilQuietly", L"timeout = %e", timeoutAt};
#endif
    Thread::CheckForInterruption ();
    unique_lock<mutex> lock{fConditionVariable.fMutex};
    if (fConditionVariable.wait_until (lock, Time::Pin2SafeSeconds (timeoutAt), [this] () { return fTriggered; })) [[likely]] {
        return WaitStatus::eTriggered;
    }
    else {
        Assert (timeoutAt <= Time::GetTickCount ()); // otherwise we couldn't have timed out
        return WaitStatus::eTimeout;
    }
}

/*
 ********************************************************************************
 ********************************** WaitableEvent *******************************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (4996);
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
constexpr WaitableEvent::ResetType WaitableEvent::eAutoReset;
constexpr WaitableEvent::ResetType WaitableEvent::eManualReset;
DISABLE_COMPILER_MSC_WARNING_END (4996);
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

#if qDebug || qStroika_FeatureSupported_Valgrind
WaitableEvent::~WaitableEvent ()
{
#if qExecution_WaitableEvent_SupportWaitForMultipleObjects
    Require (fExtraWaitableEvents_.empty ()); // Cannot kill a waitable event while its being waited on by others
#endif
}
#endif

void WaitableEvent::Set ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"WaitableEvent::Set"};
#endif
    fWE_.Set ();
#if qExecution_WaitableEvent_SupportWaitForMultipleObjects
    [[maybe_unused]] lock_guard critSec{sExtraWaitableEventsMutex_};
    for (const auto& i : fExtraWaitableEvents_) {
        i->Set ();
    }
#endif
}
