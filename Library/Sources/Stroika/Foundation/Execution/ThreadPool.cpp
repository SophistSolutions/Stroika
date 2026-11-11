/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Main.h"
#include "Stroika/Foundation/Debug/TimingTrace.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Timeout.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

#include "ThreadPool.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

using Memory::MakeSharedPtr;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    constexpr bool kEmitDbgTraceOnThreadPoolEntryExceptions_ = qStroika_Foundation_Debug_AssertionsChecked;
}

class ThreadPool::MyRunnable_ {
public:
    MyRunnable_ (ThreadPool& threadPool)
        : fThreadPool{threadPool}
    {
    }

public:
    // whether this thread is running a task - and if so, the task (assumes the caller holds fThreadPool.fCriticalSection_)
    bool IsRunning () const
    {
        return fCurTask.fTask != nullptr;
    }
    bool IsRunning (TaskID task) const
    {
        return IsRunning () and fCurTask.fID == task;
    }

public:
    nonvirtual void Run ()
    {
        // Keep grabbing new tasks, and running them - until a Thread::AbortException ends this thread (see AbortTask)
        while (true) {
            {
                if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
                    [[maybe_unused]] lock_guard critSec{fThreadPool.fCriticalSection_.fMutex};
                    Assert (not IsRunning ());
                }
                // Subtle point, but we must copy directly into fCurTask (WaitForNextTask_ call) so its filled in under lock
                // while being moved so task moves from pending to in-use without ever temporarily disappearing from known tasks lists
                fThreadPool.WaitForNextTask_ (*this); // This will block INDEFINITELY until ThreadAbort throws out or we have a new task to run
                if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
                    [[maybe_unused]] lock_guard critSec{fThreadPool.fCriticalSection_.fMutex};
                    Assert (IsRunning ());
                }
            }
            [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept {
                [[maybe_unused]] lock_guard critSec{fThreadPool.fCriticalSection_.fMutex};
                fCurTask = PendingTaskInfo_{};
                if (fThreadPool.fCollectStatistics_) {
                    ++fThreadPool.fCollectedTaskStats_.fNumberOfTasksCompleted;
                    ++fThreadPool.fCollectedTaskStats_.fNumberOfTasksReporting;
                    fThreadPool.fCollectedTaskStats_.fTotalTimeConsumed += Time::GetTickCount () - fCurTaskStartedAt;
                }
                fThreadPool.NotifyProgress_ (); // so the statistics are current when a WaitForTask for this task returns
            });
            try {
                // Use lock to access fCurTask, but don't hold the lock during run, so others can call getcurrenttask
                ThreadPool::TaskType task2Run;
                {
                    [[maybe_unused]] lock_guard critSec{fThreadPool.fCriticalSection_.fMutex};
                    task2Run          = fCurTask.fTask;
                    fCurTaskStartedAt = Time::GetTickCount ();
                }
                task2Run ();
            }
            catch (const Thread::AbortException&) {
                throw; // cancel this thread
            }
            catch (...) {
                // other exceptions WARNING WITH DEBUG MESSAGE - but otherwise - EAT/IGNORE
                if constexpr (kEmitDbgTraceOnThreadPoolEntryExceptions_ and qStroika_Foundation_Debug_DefaultTracingOn) {
                    DbgTrace ("in threadpool, ignoring exception running task: {}"_f, current_exception ());
                }
            }
        }
    }

public:
    ThreadPool& fThreadPool;
    // fThreadPool.fCriticalSection_ protect access to fCurTask/fCurTaskStartedAt/fRetire - very short duration
    PendingTaskInfo_       fCurTask;              // the task running, its ID and name; fTask null if none
    Time::TimePointSeconds fCurTaskStartedAt{0s}; // meaningless if not IsRunning ()
    bool                   fRetire{false};        // chosen to go (SetPoolSize): takes no further task, and is not counted in GetPoolSize
};

/*
 ********************************************************************************
 ************************ Execution::ThreadPool::Statistics *********************
 ********************************************************************************
 */
Characters::String ThreadPool::Statistics::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "numberOfTasksAdded: "sv << fNumberOfTasksAdded;
    sb << ", numberOfTasksCompleted: "sv << fNumberOfTasksCompleted;
    sb << ", totalTimeConsumed: "sv << fTotalTimeConsumed;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ****************************** Execution::ThreadPool ***************************
 ********************************************************************************
 */
ThreadPool::ThreadPool (const Options& options)
    : fCollectStatistics_{options.fCollectStatistics}
    , fDefaultQMax_{options.fQMax}
    , fThreadPoolName_{options.fThreadPoolName}
{
    Require (Debug::AppearsDuringMainLifetime ());
    SetPoolSize (options.fThreadCount);
}

ThreadPool::~ThreadPool ()
{
    Require (Debug::AppearsDuringMainLifetime ());
    AbortAndWaitForDone_ ();
}

unsigned int ThreadPool::GetPoolSize () const
{
    [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
    return static_cast<unsigned int> (fThreads_.Where ([] (const TPInfo_& ti) { return not ti.fRunnable->fRetire; }).size ());
}

void ThreadPool::SetPoolSize (unsigned int poolSize)
{
    Debug::TraceContextBumper ctx{"ThreadPool::SetPoolSize", "newPoolSize={}, *this={}"_f, poolSize, ToString ()};
    Require (not fAborted_);
    Collection<Thread::Ptr> removed; // aborted under the lock, but waited for after it, as AbortTasks does
    {
        ConditionVariable<>::LockType lock{fCriticalSection_.fMutex};
        size_t                        staying = fThreads_.Where ([] (const TPInfo_& ti) { return not ti.fRunnable->fRetire; }).size ();
        DbgTrace ("threads staying={}"_f, staying);
        for (; staying < poolSize; ++staying) {
            fThreads_.Add (mkThread_ ());
        }
        // The threads to go: idle ones first, then busy ones - each marked fRetire, so it takes no further task (WaitForNextTask_).
        // Any marked already, by a SetPoolSize aborted while it waited, go too
        for (bool idle : {true, false}) {
            for (const TPInfo_& ti : fThreads_) {
                if (staying > poolSize and not ti.fRunnable->fRetire and ti.fRunnable->IsRunning () != idle) {
                    ti.fRunnable->fRetire = true;
                    --staying;
                }
            }
        }
        Collection<TPInfo_> retiring{fThreads_.Where ([] (const TPInfo_& ti) { return ti.fRunnable->fRetire; })};
        if (not retiring.empty ()) {
            ++fProgressWaiters_;
            [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept { --fProgressWaiters_; }); // still under the lock, even if this throws
            (void)fCriticalSection_.wait_until (lock, Time::GetTickCount () + Time::kInfinity, [&] () {
                return retiring.All ([] (const TPInfo_& ti) { return not ti.fRunnable->IsRunning (); });
            });
            // each now idle, so aborting it costs no task
            for (const TPInfo_& ti : retiring) {
                ti.fThread.Abort ();
                removed.Add (ti.fThread);
            }
            fThreads_.RemoveAll ([] (const TPInfo_& ti) { return ti.fRunnable->fRetire; });
        }
    }
    for (const Thread::Ptr& t : removed) {
        t.AbortAndWaitForDone ();
    }
}

auto ThreadPool::AddTask (const TaskType& task, QMax qmax, const optional<Characters::String>& name) -> TaskID
{
    return AddTask_ (task, qmax, name);
}

auto ThreadPool::AddTask_ (const TaskType& task, const optional<QMax>& qmax, const optional<Characters::String>& name) -> TaskID
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"ThreadPool::AddTask_"};
#endif
    Require (not fAborted_);
    const TaskID                  id = TaskID::New ();
    ConditionVariable<>::LockType lock{fCriticalSection_.fMutex};
    if (qmax and fPendingTasks_.size () >= qmax->fLength) [[unlikely]] {
        DbgTrace ("Blocking inside ThreadPool::AddTask due to excessive pending task count"_f);
        ++fProgressWaiters_;
        [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept { --fProgressWaiters_; }); // still under the lock, even if this throws
        if (not fCriticalSection_.wait_until (lock, Time::GetTickCount () + qmax->fAddBlockTimeout,
                                              [&] () { return fPendingTasks_.size () < qmax->fLength; })) {
            ThrowError (errc::timed_out);
        }
    }
    fPendingTasks_.push_back (PendingTaskInfo_{.fID = id, .fTask = task, .fName = name});
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("fPendingTasks.size () now = {}"_f, (int)fPendingTasks_.size ());
#endif
    ++fCollectedTaskStats_.fNumberOfTasksAdded;
    fCriticalSection_.release_and_notify_all (lock); // so an idle pool thread takes it
    return id;
}

void ThreadPool::AbortTask (TaskID task, Time::DurationSeconds timeout)
{
    Debug::TraceContextBumper ctx{"ThreadPool::AbortTask"};
    {
        // First see if its in the Q
        [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
        for (auto i = fPendingTasks_.begin (); i != fPendingTasks_.end (); ++i) {
            if (i->fID == task) {
                fPendingTasks_.erase (i);
                NotifyProgress_ ();
                return;
            }
        }
    }

    // Not pending, so perhaps running: abort its thread, at once replaced by a new one (see AbortTask's docs)
    Thread::Ptr thread2Kill;
    {
        [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
        for (Iterator<TPInfo_> i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            if (i->fRunnable->IsRunning (task)) {
                thread2Kill = i->fThread;
                if (i->fRunnable->fRetire) {
                    fThreads_.Remove (i); // going anyway (SetPoolSize), so not replaced
                }
                else {
                    fThreads_.Update (i, mkThread_ ());
                }
                NotifyProgress_ ();
                break;
            }
        }
    }
    if (thread2Kill != nullptr) {
        thread2Kill.AbortAndWaitForDone (timeout);
    }
}

void ThreadPool::AbortTasks (Time::DurationSeconds timeout)
{
    Debug::TraceContextBumper ctx{"ThreadPool::AbortTasks"};
    Time::TimePointSeconds    timeoutAt = Time::GetTickCount () + timeout;
    Collection<Thread::Ptr>   aborted;
    {
        // each thread is aborted, and replaced by a new one, under the lock - but waited for after it: an aborted thread's cleanup
        // (MyRunnable_::Run) takes fCriticalSection_
        [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
        fPendingTasks_.clear ();
        Collection<TPInfo_> replacements;
        for (const TPInfo_& ti : fThreads_) {
            ti.fThread.Abort ();
            aborted.Add (ti.fThread);
            if (not ti.fRunnable->fRetire) { // one going anyway (SetPoolSize) is not replaced
                replacements.Add (mkThread_ ());
            }
        }
        fThreads_ = replacements;
        NotifyProgress_ ();
    }
    for (const Thread::Ptr& t : aborted) {
        t.AbortAndWaitForDoneUntil (timeoutAt);
    }
}

bool ThreadPool::IsPresent (TaskID task) const
{
    [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
    return IsPresent_ (task);
}

bool ThreadPool::IsPresent_ (TaskID task) const
{
    // First see if its in the Q
    for (auto i = fPendingTasks_.begin (); i != fPendingTasks_.end (); ++i) {
        if (i->fID == task) {
            return true;
        }
    }
    // then check if running
    for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
        if (i->fRunnable->IsRunning (task)) {
            return true;
        }
    }
    return false;
}

bool ThreadPool::IsRunning (TaskID task) const
{
    [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
    for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
        if (i->fRunnable->IsRunning (task)) {
            return true;
        }
    }
    return false;
}

void ThreadPool::WaitForTask (TaskID task, Time::DurationSeconds timeout) const
{
    Debug::TraceContextBumper     ctx{"ThreadPool::WaitForTask"};
    Time::TimePointSeconds        timeoutAt = Time::GetTickCount () + timeout;
    ConditionVariable<>::LockType lock{fCriticalSection_.fMutex};
    ++fProgressWaiters_;
    [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept { --fProgressWaiters_; }); // still under the lock, even if this throws
    if (not fCriticalSection_.wait_until (lock, timeoutAt, [&] () { return not IsPresent_ (task); })) {
        ThrowError (errc::timed_out);
    }
}

auto ThreadPool::GetTasks () const -> Collection<TaskInfo>
{
    Collection<TaskInfo>        result;
    [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
    for (const auto& ti : fPendingTasks_) {
        result.Add (TaskInfo{.fID = ti.fID, .fTask = ti.fTask, .fName = ti.fName});
    }
    for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
        if (i->fRunnable->IsRunning ()) {
            const PendingTaskInfo_& cur = i->fRunnable->fCurTask;
            result.Add (TaskInfo{.fID = cur.fID, .fTask = cur.fTask, .fName = cur.fName, .fRunningSince = i->fRunnable->fCurTaskStartedAt});
        }
    }
    return result;
}

auto ThreadPool::GetTaskInfo (TaskID task) const -> optional<TaskInfo>
{
    [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
    for (const auto& ti : fPendingTasks_) {
        if (ti.fID == task) {
            return TaskInfo{.fID = ti.fID, .fTask = ti.fTask, .fName = ti.fName};
        }
    }
    for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
        if (i->fRunnable->IsRunning (task)) {
            const PendingTaskInfo_& cur = i->fRunnable->fCurTask;
            return TaskInfo{.fID = cur.fID, .fTask = cur.fTask, .fName = cur.fName, .fRunningSince = i->fRunnable->fCurTaskStartedAt};
        }
    }
    return nullopt;
}

auto ThreadPool::GetRunningTasks () const -> Collection<TaskID>
{
    Collection<TaskID>          result;
    [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
    for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
        if (i->fRunnable->IsRunning ()) {
            result.Add (i->fRunnable->fCurTask.fID);
        }
    }
    return result;
}

size_t ThreadPool::GetTasksCount () const
{
    [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
    return GetTasksCount_ ();
}

size_t ThreadPool::GetTasksCount_ () const
{
    size_t count = fPendingTasks_.size ();
    for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
        AssertNotNull (i->fRunnable);
        if (i->fRunnable->IsRunning ()) {
            ++count;
        }
    }
    return count;
}

auto ThreadPool::GetPendingTasks () const -> Collection<TaskID>
{
    Collection<TaskID>          result;
    [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
    for (const auto& i : fPendingTasks_) {
        result.Add (i.fID);
    }
    return result;
}

size_t ThreadPool::GetPendingTasksCount () const
{
    [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
    return fPendingTasks_.size ();
}

void ThreadPool::WaitForTasksDoneUntil (const Iterable<TaskID>& tasks, Time::TimePointSeconds timeoutAt) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
        "ThreadPool::WaitForTasksDoneUntil", "*this={}, tasks={}, timeoutAt={}"_f, ToString (), ToString (tasks), timeoutAt)};
#endif
    Thread::CheckForInterruption ();
    for (const auto& task : tasks) {
        auto now = Time::GetTickCount ();
        ThrowTimeoutExceptionAfter (timeoutAt);
        WaitForTask (task, timeoutAt - now);
    }
}

void ThreadPool::WaitForTasksDoneUntil (Time::TimePointSeconds timeoutAt) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("ThreadPool::WaitForTasksDoneUntil",
                                                                                 "*this={}, timeoutAt={}"_f, ToString (), timeoutAt)};
#endif
    ConditionVariable<>::LockType lock{fCriticalSection_.fMutex};
    ++fProgressWaiters_;
    [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept { --fProgressWaiters_; }); // still under the lock, even if this throws
    if (not fCriticalSection_.wait_until (lock, timeoutAt, [this] () { return GetTasksCount_ () == 0; })) {
        ThrowError (errc::timed_out);
    }
}

void ThreadPool::ResetStatistics ()
{
    [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
    Require (fCollectStatistics_);
    fCollectedTaskStats_ = {};
}

auto ThreadPool::GetCurrentStatistics () const -> Statistics
{
    [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
    Require (fCollectStatistics_);
    return fCollectedTaskStats_;
}

void ThreadPool::Abort_ () noexcept
{
    Thread::SuppressInterruptionInContext suppressCtx; // must cleanly shut down each of our sub-threads - even if our thread is aborting... don't be half-way aborted
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("ThreadPool::Abort_", "*this={}"_f, ToString ())};
    Debug::TimingTrace        tt{1.0s};
    fAborted_ = true; // No race, because fAborted never 'unset'
    {
        // Clear the task Q and then abort each thread - which wakes it, if waiting for a task
        [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
        fPendingTasks_.clear ();
        for (const TPInfo_& ti : fThreads_) {
            ti.fThread.Abort ();
        }
        NotifyProgress_ ();
    }
}

void ThreadPool::AbortAndWaitForDone_ () noexcept
{
    Thread::SuppressInterruptionInContext suppressCtx; // cuz we must shutdown owned threads
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("ThreadPool::AbortAndWaitForDone_", "*this={}"_f, ToString ())};
    Debug::TimingTrace tt{1.0s};
#endif
    try {
        Abort_ (); // to get the rest of the threadpool abort stuff triggered - flag saying aborting
        Collection<Thread::Ptr> threadsToShutdown;
        {
            [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
            fThreads_.Apply ([&] (const TPInfo_& i) { threadsToShutdown.Add (i.fThread); });
        }
        Thread::AbortAndWaitForDone (threadsToShutdown);
        {
            [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex}; // they should all be shutdown now
            fThreads_.Apply ([&] (const TPInfo_& i) { Assert (i.fThread.IsDone ()); });
            fThreads_.RemoveAll ();
        }
    }
    catch (...) {
        DbgTrace ("ThreadPool::AbortAndWaitForDone_: serious bug/exception"_f);
        AssertNotReached (); // this should never happen due to the SuppressInterruptionInContext...
    }
    Ensure (fThreads_.empty ());
}

String ThreadPool::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    {
        [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
        if (fThreadPoolName_) {
            sb << "pool-name: '{}'"_f(*fThreadPoolName_);
        }
    }
    if (fThreadPoolName_) {
        sb << ", "sv;
    }
    sb << "pending-task-count: {}"_f(GetPendingTasksCount ());
    sb << ", running-task-count: {}"_f(GetRunningTasks ().size ());
    {
        [[maybe_unused]] lock_guard critSec{fCriticalSection_.fMutex};
        sb << ", pool-thread-count: {}"_f(fThreads_.size ());
    }
    sb << "}"sv;
    return sb;
}

// THIS is called NOT from 'this' - but from the context of an OWNED thread of the pool
void ThreadPool::WaitForNextTask_ (MyRunnable_& runnable)
{
    Require (runnable.fCurTask.fTask == nullptr);
    ConditionVariable<>::LockType lock{fCriticalSection_.fMutex};
    // An aborted thread throws from wait_until, under the lock, even with a task pending - so a thread the pool aborted (Abort_,
    // AbortTasks, SetPoolSize - each under this lock) takes no task. Nor does one chosen to go: it waits for SetPoolSize to abort it
    (void)fCriticalSection_.wait_until (lock, Time::GetTickCount () + Time::kInfinity,
                                        [&] () { return not runnable.fRetire and not fPendingTasks_.empty (); });
    runnable.fCurTask = fPendingTasks_.front ();
    fPendingTasks_.pop_front ();
    DbgTrace ("ThreadPool::WaitForNextTask_ () pulled a new task from 'pending-tasks' to run on this thread, leaving pending-task-list-size = {}"_f,
              fPendingTasks_.size ());
    Ensure (runnable.fCurTask.fTask != nullptr);
    NotifyProgress_ (); // room in the queue, for an AddTask with a QMax
}

void ThreadPool::NotifyProgress_ () const
{
    if (fProgressWaiters_ > 0) {
        fCriticalSection_.notify_all ();
    }
}

ThreadPool::TPInfo_ ThreadPool::mkThread_ ()
{
    shared_ptr<MyRunnable_> r{MakeSharedPtr<ThreadPool::MyRunnable_> (*this)};
    StringBuilder entryName = "TPE #{}"_f(fNextThreadEntryNumber_++); // make name so short cuz unix only shows first 15 chars - http://man7.org/linux/man-pages/man3/pthread_setname_np.3.html
    entryName += " {"sv + fThreadPoolName_.value_or ("anonymous-thread-pool"sv) + "}"sv;
    Thread::Ptr t = Thread::New ([r] () { r->Run (); }, Thread::eAutoStart, entryName); // race condition for updating this number, but who cares - its purely cosmetic...
    return TPInfo_{t, r};
}
