/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"
#include "../Characters/StringBuilder.h"
#include "../Characters/ToString.h"
#include "../Execution/Finally.h"
#include "../Memory/BlockAllocated.h"

#include "Common.h"
#include "Sleep.h"
#include "TimeOutException.h"

#include "ThreadPool.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

using Characters::String;
using Characters::StringBuilder;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

class ThreadPool::MyRunnable_ {
public:
    MyRunnable_ (ThreadPool& threadPool)
        : fCurTaskUpdateCritSection_ ()
        , fThreadPool_ (threadPool)
        , fCurTask_ ()
        , fNextTask_ ()
    {
    }

public:
    ThreadPool::TaskType GetCurrentTask () const
    {
        auto critSec{make_unique_lock (fCurTaskUpdateCritSection_)};
        // THIS CODE IS TOO SUBTLE - BUT BECAUSE OF HOW THIS IS CALLED - fNextTask_ will NEVER be in the middle of being updated during this code - so this test is OK
        // Caller is never in the middle of doing a WaitForNextTask - and because we have this lock - we aren't updateing fCurTask_ or fNextTask_ either
        Assert (fCurTask_ == nullptr or fNextTask_ == nullptr); // one or both must be null
        return fCurTask_ == nullptr ? fNextTask_ : fCurTask_;
    }

public:
    nonvirtual void Run ()
    {
        // For NOW - allow ThreadAbort to just kill this thread. In the future - we may want to implement some sort of restartability

        // Keep grabbing new tasks, and running them
        while (true) {
            {
                fThreadPool_.WaitForNextTask_ (&fNextTask_); // This will block INDEFINITELY until ThreadAbort throws out or we have a new task to run
                auto critSec{make_unique_lock (fCurTaskUpdateCritSection_)};
                Assert (fNextTask_ != nullptr);
                Assert (fCurTask_ == nullptr);
                fCurTask_  = fNextTask_;
                fNextTask_ = nullptr;
                Assert (fCurTask_ != nullptr);
                Assert (fNextTask_ == nullptr);
            }
            auto&& cleanup = Execution::Finally ([this]() {
                auto critSec{make_unique_lock (fCurTaskUpdateCritSection_)};
                fCurTask_ = nullptr;
            });
            try {
                // Use lock to access fCurTask_, but dont hold the lock during run, so others can call getcurrenttask
                ThreadPool::TaskType task2Run;
                {
                    auto critSec{make_unique_lock (fCurTaskUpdateCritSection_)};
                    task2Run = fCurTask_;
                }
                task2Run ();
            }
            catch (const Thread::AbortException&) {
                throw; // cancel this thread
            }
            catch (...) {
                // other exceptions WARNING WITH DEBUG MESSAGE - but otherwise - EAT/IGNORE
            }
        }
    }

private:
    mutable recursive_mutex fCurTaskUpdateCritSection_;
    ThreadPool&             fThreadPool_;
    ThreadPool::TaskType    fCurTask_;
    ThreadPool::TaskType    fNextTask_;
};

/*
 ********************************************************************************
 ****************************** Execution::ThreadPool ***************************
 ********************************************************************************
 */
ThreadPool::ThreadPool (unsigned int nThreads, const Memory::Optional<String>& threadPoolName)
    : fThreadPoolName_ (threadPoolName)
{
    SetPoolSize (nThreads);
}

ThreadPool::~ThreadPool ()
{
    Thread::SuppressInterruptionInContext suppressCtx; // cuz we must shutdown owned threads
    try {
        AbortAndWaitForDoneUntil_ (Time::kInfinite);
    }
    catch (...) {
        DbgTrace (L"serious bug - make AbortAndWaitForDoneUntil_ noexcept after we lose deprecated variants");
        AssertNotReached (); // this should never happen due to the SuppressInterruptionInContext...
    }
}

unsigned int ThreadPool::GetPoolSize () const
{
    auto critSec{make_unique_lock (fCriticalSection_)};
    return static_cast<unsigned int> (fThreads_.size ());
}

void ThreadPool::SetPoolSize (unsigned int poolSize)
{
    Debug::TraceContextBumper ctx (L"ThreadPool::SetPoolSize", L"poolSize=%d", poolSize);
    Require (not fAborted_);
    auto critSec{make_unique_lock (fCriticalSection_)};
    DbgTrace (L"fThreads_.size ()=%d", fThreads_.size ());
    while (poolSize > fThreads_.size ()) {
        fThreads_.Add (mkThread_ ());
    }

    // Still quite weak implementation of REMOVAL
    while (poolSize < fThreads_.size ()) {
        // iterate over threads if any not busy, remove that one
        bool anyFoundToKill = false;
        for (Iterator<TPInfo_> i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            shared_ptr<MyRunnable_> tr{i->fRunnable};
            TaskType                ct{tr->GetCurrentTask ()};
            if (ct == nullptr) {
                // since we have fCriticalSection_ - we can safely remove this thread
                fThreads_.Remove (i);
                anyFoundToKill = true;
                break;
            }
        }
        if (not anyFoundToKill) {
            // @todo - fix this better/eventually
            DbgTrace ("Failed to lower the loop size - cuz all threads busy - giving up");
            return;
        }
    }
}

ThreadPool::TaskType ThreadPool::AddTask (const TaskType& task)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"ThreadPool::AddTask"};
#endif
    Require (not fAborted_);
    {
        auto critSec{make_unique_lock (fCriticalSection_)};
        fPendingTasks_.push_back (task);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"fPendingTasks.size () now = %d", (int)fPendingTasks_.size ());
#endif
    }

    // Notify any waiting threads to wakeup and claim the next task
    fTasksMaybeAdded_.Set ();
    // this would be a race - if aborting and adding tasks at the same time
    Require (not fAborted_);
    return task;
}

void ThreadPool::AbortTask (const TaskType& task, Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper ctx ("ThreadPool::AbortTask");
    {
        // First see if its in the Q
        auto critSec{make_unique_lock (fCriticalSection_)};
        for (auto i = fPendingTasks_.begin (); i != fPendingTasks_.end (); ++i) {
            if (*i == task) {
                fPendingTasks_.erase (i);
                return;
            }
        }
    }

    // If we got here - its NOT in the task Q, so maybe its already running.
    //
    //

    // TODO:
    //      We walk the list of existing threads and ask each one if its (indirected - running task) is the given one and abort that task.
    //      But that requires we can RESTART an ABORTED thread (or that we remove it from the list - maybe thats better). THat COULD be OK
    //      actually since it involves on API changes and makes sense. The only slight issue is a peformace one but probably for something
    //      quite rare.
    //
    //      Anyhow SB OK for now to just not allow aborting a task which has already started....
    Thread::Ptr thread2Kill;
    {
        auto critSec{make_unique_lock (fCriticalSection_)};
        for (Iterator<TPInfo_> i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            shared_ptr<MyRunnable_> tr{i->fRunnable};
            TaskType                ct{tr->GetCurrentTask ()};
            if (task == ct) {
                thread2Kill = i->fThread;
                fThreads_.Update (i, mkThread_ ());
                break;
            }
        }
    }
    thread2Kill.AbortAndWaitForDone (timeout);
}

void ThreadPool::AbortTasks (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper ctx ("ThreadPool::AbortTasks");
    auto                      tps = GetPoolSize ();
    {
        auto critSec{make_unique_lock (fCriticalSection_)};
        fPendingTasks_.clear ();
    }
    {
        auto critSec{make_unique_lock (fCriticalSection_)};
        for (TPInfo_ ti : fThreads_) {
            ti.fThread.Abort ();
        }
    }
    {
        auto critSec{make_unique_lock (fCriticalSection_)};
        for (TPInfo_ ti : fThreads_) {
            // @todo fix wrong timeout value here
            ti.fThread.AbortAndWaitForDone (timeout);
        }
        fThreads_.RemoveAll ();
    }
    // hack - not a good design or impl!!! - waste to recreate if not needed!
    SetPoolSize (tps);
}

bool ThreadPool::IsPresent (const TaskType& task) const
{
    Require (task != nullptr);
    {
        // First see if its in the Q
        auto critSec{make_unique_lock (fCriticalSection_)};
        for (auto i = fPendingTasks_.begin (); i != fPendingTasks_.end (); ++i) {
            if (*i == task) {
                return true;
            }
        }
    }
    return IsRunning (task);
}

bool ThreadPool::IsRunning (const TaskType& task) const
{
    Require (task != nullptr);
    {
        auto critSec{make_unique_lock (fCriticalSection_)};
        for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            shared_ptr<MyRunnable_> tr{i->fRunnable};
            TaskType                rTask{tr->GetCurrentTask ()};
            if (task == rTask) {
                return true;
            }
        }
    }
    return false;
}

void ThreadPool::WaitForTask (const TaskType& task, Time::DurationSecondsType timeout) const
{
    Debug::TraceContextBumper ctx ("ThreadPool::WaitForTask");
    // Inefficient / VERY SLOPPY impl
    using Time::DurationSecondsType;
    DurationSecondsType timeoutAt = timeout + Time::GetTickCount ();
    while (true) {
        if (not IsPresent (task)) {
            return;
        }
        DurationSecondsType remaining = timeoutAt - Time::GetTickCount ();
        Execution::ThrowTimeoutExceptionAfter (timeoutAt);
        Execution::Sleep (min<DurationSecondsType> (remaining, 1.0));
    }
}

Collection<ThreadPool::TaskType> ThreadPool::GetTasks () const
{
    Collection<ThreadPool::TaskType> result;
    {
        auto critSec{make_unique_lock (fCriticalSection_)};
        result.AddAll (fPendingTasks_.begin (), fPendingTasks_.end ()); // copy pending tasks
        for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            shared_ptr<MyRunnable_> tr{i->fRunnable};
            TaskType                task{tr->GetCurrentTask ()};
            if (task != nullptr) {
                result.Add (task);
            }
        }
    }
    return result;
}

Collection<ThreadPool::TaskType> ThreadPool::GetRunningTasks () const
{
    Collection<ThreadPool::TaskType> result;
    {
        auto critSec{make_unique_lock (fCriticalSection_)};
        for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            shared_ptr<MyRunnable_> tr{i->fRunnable};
            TaskType                task{tr->GetCurrentTask ()};
            if (task != nullptr) {
                result.Add (task);
            }
        }
    }
    return result;
}

size_t ThreadPool::GetTasksCount () const
{
    size_t count = 0;
    {
        // First see if its in the Q
        auto critSec{make_unique_lock (fCriticalSection_)};
        count += fPendingTasks_.size ();
        for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            shared_ptr<MyRunnable_> tr{i->fRunnable};
            TaskType                task{tr->GetCurrentTask ()};
            if (task != nullptr) {
                count++;
            }
        }
    }
    return count;
}

size_t ThreadPool::GetPendingTasksCount () const
{
    size_t count = 0;
    {
        auto critSec{make_unique_lock (fCriticalSection_)};
        count += fPendingTasks_.size ();
    }
    return count;
}

void ThreadPool::WaitForDoneUntil (Time::DurationSecondsType timeoutAt) const
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ThreadPool::WaitForDoneUntil", L"*this=%s, timeoutAt=%f", ToString ().c_str (), timeoutAt)};
    Require (fAborted_);
    {
        Collection<Thread::Ptr> threadsToShutdown; // cannot keep critical section while waiting on subthreads since they may need to acquire the critsection for whatever they are doing...
        {
            auto critSec{make_unique_lock (fCriticalSection_)};
            fThreads_.Apply ([&](const TPInfo_& i) { threadsToShutdown.Add (i.fThread); });
        }
        Thread::WaitForDoneUntil (threadsToShutdown, timeoutAt);
    }
}

void ThreadPool::Abort ()
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ThreadPool::Abort", L"*this=%s", ToString ().c_str ())};
    Abort_ ();
}

void ThreadPool::Abort_ ()
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ThreadPool::Abort_", L"*this=%s", ToString ().c_str ())};
    CheckForThreadInterruption ();
    Thread::SuppressInterruptionInContext suppressCtx;            // must cleanly shut down each of our subthreads - even if our thread is aborting... dont be half-way aborted
    Stroika_Foundation_Debug_ValgrindDisableHelgrind (fAborted_); // disable cuz - see below
    fAborted_ = true;                                             // No race, because fAborted never 'unset'
                                                                  // no need to set fTasksMaybeAdded_, since aborting each thread should be sufficient
    {
        // Clear the task Q and then abort each thread
        auto critSec{make_unique_lock (fCriticalSection_)};
        fPendingTasks_.clear ();
        for (TPInfo_&& ti : fThreads_) {
            ti.fThread.Abort ();
        }
    }
}

void ThreadPool::AbortAndWaitForDone (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ThreadPool::AbortAndWaitForDone", L"*this=%s, timeout=%f", ToString ().c_str (), timeout)};
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    AbortAndWaitForDoneUntil (timeout + Time::GetTickCount ());
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
}

void ThreadPool::AbortAndWaitForDoneUntil_ (Time::DurationSecondsType timeoutAt)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ThreadPool::AbortAndWaitForDoneUntil_", L"*this=%s, timeoutAt=%f", ToString ().c_str (), timeoutAt)};
#endif
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    CheckForThreadInterruption ();
    Thread::SuppressInterruptionInContext suppressCtx; // must cleanly shut down each of our subthreads - even if our thread is aborting... dont be half-way aborted
    Abort_ ();                                         // to get the rest of the threadpool abort stuff triggered - flag saying aborting
    Collection<Thread::Ptr> threadsToShutdown;
    {
        auto critSec{make_unique_lock (fCriticalSection_)};
        fThreads_.Apply ([&](const TPInfo_& i) { threadsToShutdown.Add (i.fThread); });
    }
    Thread::AbortAndWaitForDoneUntil (threadsToShutdown, timeoutAt);
    DISABLE_COMPILER_MSC_WARNING_END (4996)
}

void ThreadPool::AbortAndWaitForDoneUntil (Time::DurationSecondsType timeoutAt)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ThreadPool::AbortAndWaitForDoneUntil", L"*this=%s, timeoutAt=%f", ToString ().c_str (), timeoutAt)};
    AbortAndWaitForDoneUntil_ (timeoutAt);
}

String ThreadPool::ToString () const
{
    auto          critSec{make_unique_lock (fCriticalSection_)};
    StringBuilder sb;
    sb += L"{";
    if (fThreadPoolName_) {
        sb += Characters::Format (L"pool-name: '%s'", fThreadPoolName_->c_str ()) + L", ";
    }
    sb += Characters::Format (L"pending-task-count: %d", GetPendingTasksCount ()) + L", ";
    sb += Characters::Format (L"running-task-count: %d", GetRunningTasks ().size ()) + L", ";
    sb += Characters::Format (L"pool-thread-count: %d", fThreads_.size ()) + L", ";
    sb += L"}";
    return sb.str ();
}

// THIS is called NOT from 'this' - but from the context of an OWNED thread of the pool
void ThreadPool::WaitForNextTask_ (TaskType* result)
{
    RequireNotNull (result);
    if (fAborted_) {
        Execution::Throw (Thread::AbortException::kThe);
    }

    while (true) {
        {
            auto critSec{make_unique_lock (fCriticalSection_)};
            if (not fPendingTasks_.empty ()) {
                *result = fPendingTasks_.front ();
                fPendingTasks_.pop_front ();
                DbgTrace ("ThreadPool::WaitForNextTask_ () pulled a new task from 'pending-tasks' to run on this thread, leaving pending-task-list-size = %d", fPendingTasks_.size ());
                return;
            }
        }

        // Prevent spinwaiting... This event is SET when any new item arrives
        //DbgTrace ("ThreadPool::WaitForNextTask_ () - about to wait for added tasks");
        fTasksMaybeAdded_.Wait ();
        //DbgTrace ("ThreadPool::WaitForNextTask_ () - completed wait for added tasks");
    }
}

ThreadPool::TPInfo_ ThreadPool::mkThread_ ()
{
    shared_ptr<MyRunnable_> r{make_shared<ThreadPool::MyRunnable_> (*this)};
    String                  entryName = Characters::Format (L"TPE #%d", fNextThreadEntryNumber_++); // make name so short cuz unix only shows first 15 chars - http://man7.org/linux/man-pages/man3/pthread_setname_np.3.html
    entryName += L" {" + fThreadPoolName_.Value (L"anonymous-thread-pool") + L"}";
    Thread::Ptr t = Thread::New ([r]() { r->Run (); }, Thread::eAutoStart, entryName); // race condition for updating this number, but who cares - its purely cosmetic...
    return TPInfo_{t, r};
}
