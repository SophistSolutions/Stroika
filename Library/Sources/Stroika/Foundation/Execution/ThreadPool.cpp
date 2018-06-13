/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
        [[maybe_unused]] auto&& critSec = lock_guard{fCurTaskUpdateCritSection_};
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
                [[maybe_unused]] auto&& critSec = lock_guard{fCurTaskUpdateCritSection_};
                Assert (fNextTask_ != nullptr);
                Assert (fCurTask_ == nullptr);
                fCurTask_  = fNextTask_;
                fNextTask_ = nullptr;
                Assert (fCurTask_ != nullptr);
                Assert (fNextTask_ == nullptr);
            }
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([this]() {
                [[maybe_unused]] auto&& critSec = lock_guard{fCurTaskUpdateCritSection_};
                fCurTask_                       = nullptr;
            });
            try {
                // Use lock to access fCurTask_, but dont hold the lock during run, so others can call getcurrenttask
                ThreadPool::TaskType task2Run;
                {
                    [[maybe_unused]] auto&& critSec = lock_guard{fCurTaskUpdateCritSection_};
                    task2Run                        = fCurTask_;
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
    AbortAndWaitForDone_ ();
}

unsigned int ThreadPool::GetPoolSize () const
{
    [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
    return static_cast<unsigned int> (fThreads_.size ());
}

void ThreadPool::SetPoolSize (unsigned int poolSize)
{
    Debug::TraceContextBumper ctx (L"ThreadPool::SetPoolSize", L"poolSize=%d", poolSize);
    Require (not fAborted_);
    [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
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
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
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
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
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
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
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
    if (thread2Kill != nullptr) {
        thread2Kill.AbortAndWaitForDone (timeout);
    }
}

void ThreadPool::AbortTasks (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper ctx ("ThreadPool::AbortTasks");
    auto                      tps = GetPoolSize ();
    {
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
        fPendingTasks_.clear ();
    }
    {
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
        for (TPInfo_ ti : fThreads_) {
            ti.fThread.Abort ();
        }
    }
    {
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
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
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
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
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
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
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
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
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
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
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
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
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
        count += fPendingTasks_.size ();
    }
    return count;
}

void ThreadPool::WaitForTasksDoneUntil (const Iterable<TaskType>& tasks, Time::DurationSecondsType timeoutAt) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ThreadPool::WaitForTasksDoneUntil", L"*this=%s, tasks=%s, timeoutAt=%f", ToString ().c_str (), ToString (tasks).c_str (), timeoutAt)};
#endif
    CheckForThreadInterruption ();
    for (auto&& task : tasks) {
        auto now = Time::GetTickCount ();
        ThrowTimeoutExceptionAfter (timeoutAt);
        this->WaitForTask (task, timeoutAt - now);
    }
}

void ThreadPool::WaitForTasksDoneUntil (Time::DurationSecondsType timeoutAt) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ThreadPool::WaitForTasksDoneUntil", L"*this=%s, timeoutAt=%f", ToString ().c_str (), timeoutAt)};
#endif
    CheckForThreadInterruption ();
    // @todo - use waitableevent - this is a horribly implementation
    while (GetTasksCount () != 0) {
        ThrowTimeoutExceptionAfter (timeoutAt);
        Execution::Sleep (100ms);
    }
}

void ThreadPool::Abort_ () noexcept
{
    Thread::SuppressInterruptionInContext suppressCtx; // must cleanly shut down each of our subthreads - even if our thread is aborting... dont be half-way aborted
    Debug::TraceContextBumper             ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ThreadPool::Abort_", L"*this=%s", ToString ().c_str ())};
    Stroika_Foundation_Debug_ValgrindDisableHelgrind (fAborted_); // Valgrind warns updated with no lock, but my design (see below) - and since using std::atomic, will be published to other threads
    fAborted_ = true;                                             // No race, because fAborted never 'unset'
                                                                  // no need to set fTasksMaybeAdded_, since aborting each thread should be sufficient
    {
        // Clear the task Q and then abort each thread
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
        fPendingTasks_.clear ();
        for (TPInfo_&& ti : fThreads_) {
            ti.fThread.Abort ();
        }
    }
}

void ThreadPool::AbortAndWaitForDone_ () noexcept
{
    Thread::SuppressInterruptionInContext suppressCtx; // cuz we must shutdown owned threads
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ThreadPool::AbortAndWaitForDone_", L"*this=%s, timeoutAt=%f", ToString ().c_str (), timeoutAt)};
#endif
    try {
        Abort_ (); // to get the rest of the threadpool abort stuff triggered - flag saying aborting
        Collection<Thread::Ptr> threadsToShutdown;
        {
            [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
            fThreads_.Apply ([&](const TPInfo_& i) { threadsToShutdown.Add (i.fThread); });
        }
        Thread::AbortAndWaitForDone (threadsToShutdown);
    }
    catch (...) {
        DbgTrace (L"serious bug");
        AssertNotReached (); // this should never happen due to the SuppressInterruptionInContext...
    }

    DISABLE_COMPILER_MSC_WARNING_START (4996)
    DISABLE_COMPILER_MSC_WARNING_END (4996)
}

String ThreadPool::ToString () const
{
    [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
    StringBuilder           sb;
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
    while (true) {
        if (fAborted_) {
            Execution::Throw (Thread::AbortException::kThe);
        }

        {
            [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
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
