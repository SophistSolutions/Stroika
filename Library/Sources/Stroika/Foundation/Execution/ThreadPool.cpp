/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Characters/CString/Utilities.h"
#include    "../Memory/BlockAllocated.h"

#include    "Common.h"
#include    "Sleep.h"
#include    "TimeOutException.h"

#include    "ThreadPool.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Execution;



class   ThreadPool::MyRunnable_  {
public:
    MyRunnable_ (ThreadPool& threadPool)
        : fCurTaskUpdateCritSection_ ()
        , fThreadPool_ (threadPool)
        , fCurTask_ ()
        , fNextTask_ ()
    {
    }

public:
    ThreadPool::TaskType    GetCurrentTask () const
    {
        auto    critSec { make_unique_lock (fCurTaskUpdateCritSection_) };
        // THIS CODE IS TOO SUBTLE - BUT BECAUSE OF HOW THIS IS CALLED - fNextTask_ will NEVER be in the middle of being updated during this code - so this test is OK
        // Caller is never in the middle of doing a WaitForNextTask - and because we have this lock - we aren't updateing fCurTask_ or fNextTask_ either
        Assert (fCurTask_ == nullptr or fNextTask_ == nullptr);   // one or both must be null
        return fCurTask_ == nullptr ? fNextTask_ : fCurTask_;
    }

public:
    nonvirtual void    Run ()
    {
        // For NOW - allow ThreadAbort to just kill this thread. In the future - we may want to implement some sort of restartability

        // Keep grabbing new tasks, and running them
        while (true) {
            {
                fThreadPool_.WaitForNextTask_ (&fNextTask_);            // This will block INDEFINITELY until ThreadAbort throws out or we have a new task to run
                auto    critSec { make_unique_lock (fCurTaskUpdateCritSection_) };
                Assert (fNextTask_ != nullptr);
                Assert (fCurTask_ == nullptr);
                fCurTask_ = fNextTask_;
                fNextTask_ = nullptr;
                Assert (fCurTask_ != nullptr);
                Assert (fNextTask_ == nullptr);
            }
            try {
                fCurTask_ ();
                fCurTask_ = nullptr;
            }
            catch (const Thread::AbortException&) {
                auto    critSec { make_unique_lock (fCurTaskUpdateCritSection_) };
                fCurTask_ = nullptr;
                throw;  // cancel this thread
            }
            catch (...) {
                auto    critSec { make_unique_lock (fCurTaskUpdateCritSection_) };
                fCurTask_ = nullptr;
                // other exceptions WARNING WITH DEBUG MESSAGE - but otehrwise - EAT/IGNORE
            }
        }
    }

private:
    mutable recursive_mutex     fCurTaskUpdateCritSection_;
    ThreadPool&                 fThreadPool_;
    ThreadPool::TaskType        fCurTask_;
    ThreadPool::TaskType        fNextTask_;

public:
    DECLARE_USE_BLOCK_ALLOCATION(MyRunnable_);
};











/*
 ********************************************************************************
 ****************************** Execution::ThreadPool ***************************
 ********************************************************************************
 */
ThreadPool::ThreadPool (unsigned int nThreads)
    : fCriticalSection_ ()
    , fAborted_ (false)
    , fThreads_ ()
    , fTasks_ ()
    , fTasksAdded_ (WaitableEvent::eAutoReset)
{
    SetPoolSize (nThreads);
}

ThreadPool::~ThreadPool ()
{
    try {
        this->AbortAndWaitForDone ();
    }
    catch (...) {
        DbgTrace ("Ignore exception in destroying thread pool ** probably bad thing...");
    }
}

unsigned int    ThreadPool::GetPoolSize () const
{
    return static_cast<unsigned int> (fThreads_.size ());
}

void    ThreadPool::SetPoolSize (unsigned int poolSize)
{
    Debug::TraceContextBumper ctx ("ThreadPool::SetPoolSize");
    Require (not fAborted_);
    auto    critSec { make_unique_lock (fCriticalSection_) };
    while (poolSize > fThreads_.size ()) {
        fThreads_.Add (mkThread_ ());
    }

    // Still quite weak implementation of REMOVAL
    while (poolSize < fThreads_.size ()) {
        // iterate over threads if any not busy, remove that one
        bool anyFoundToKill = false;
        for (Iterator<TPInfo_> i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            shared_ptr<MyRunnable_>     tr      { i->fRunnable };
            TaskType                    ct      { tr->GetCurrentTask () };
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

ThreadPool::TaskType    ThreadPool::AddTask (const TaskType& task)
{
    //Debug::TraceContextBumper ctx ("ThreadPool::AddTask");
    Require (not fAborted_);
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        fTasks_.push_back (task);
    }

    // Notify any waiting threads to wakeup and claim the next task
    fTasksAdded_.Set ();
    // this would be a race - if aborting and adding tasks at the same time
    Require (not fAborted_);
    return task;
}

void    ThreadPool::AbortTask (const TaskType& task, Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper ctx ("ThreadPool::AbortTask");
    {
        // First see if its in the Q
        auto    critSec { make_unique_lock (fCriticalSection_) };
        for (auto i = fTasks_.begin (); i != fTasks_.end (); ++i) {
            if (*i == task) {
                fTasks_.erase (i);
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
    Thread  thread2Kill;
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        for (Iterator<TPInfo_> i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            shared_ptr<MyRunnable_>     tr      { i->fRunnable };
            TaskType                    ct      { tr->GetCurrentTask () };
            if (task == ct) {
                thread2Kill =   i->fThread;
                fThreads_.Update (i, mkThread_ ());
                break;
            }
        }
    }
    if (thread2Kill.GetStatus () != Thread::Status::eNull) {
        thread2Kill.AbortAndWaitForDone (timeout);
    }
}

void    ThreadPool::AbortTasks (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper ctx ("ThreadPool::AbortTasks");
    auto tps = GetPoolSize ();
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        fTasks_.clear ();
    }
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        for (TPInfo_ ti : fThreads_) {
            ti.fThread.Abort ();
        }
    }
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        for (TPInfo_ ti : fThreads_) {
            // @todo fix wrong timeout value here
            ti.fThread.AbortAndWaitForDone (timeout);
        }
        fThreads_.RemoveAll ();
    }
    // hack - not a good design or impl!!! - waste to recreate if not needed!
    SetPoolSize (tps);
}

bool    ThreadPool::IsPresent (const TaskType& task) const
{
    Require (task != nullptr);
    {
        // First see if its in the Q
        auto    critSec { make_unique_lock (fCriticalSection_) };
        for (auto i = fTasks_.begin (); i != fTasks_.end (); ++i) {
            if (*i == task) {
                return true;
            }
        }
    }
    return IsRunning (task);
}

bool    ThreadPool::IsRunning (const TaskType& task) const
{
    Require (task != nullptr);
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            shared_ptr<MyRunnable_>     tr      { i->fRunnable };
            TaskType                    rTask   { tr->GetCurrentTask () };
            if (task == rTask) {
                return true;
            }
        }
    }
    return false;
}

void    ThreadPool::WaitForTask (const TaskType& task, Time::DurationSecondsType timeout) const
{
    Debug::TraceContextBumper ctx ("ThreadPool::WaitForTask");
    // Inefficient / VERY SLOPPY impl
    using   Time::DurationSecondsType;
    DurationSecondsType   timeoutAt   =   timeout + Time::GetTickCount ();
    while (true) {
        if (not IsPresent (task)) {
            return;
        }
        DurationSecondsType   remaining   =   timeoutAt - Time::GetTickCount ();
        Execution::ThrowTimeoutExceptionAfter (timeoutAt);
        Execution::Sleep (min<DurationSecondsType> (remaining, 1.0));
    }
}

Collection<ThreadPool::TaskType>    ThreadPool::GetTasks () const
{
    Collection<ThreadPool::TaskType>    result;
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        result.AddAll (fTasks_.begin (), fTasks_.end ());          // copy pending tasks
        for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            shared_ptr<MyRunnable_>     tr      { i->fRunnable };
            TaskType                    task    { tr->GetCurrentTask () };
            if (task != nullptr) {
                result.Add (task);
            }
        }
    }
    return result;
}

Collection<ThreadPool::TaskType>    ThreadPool::GetRunningTasks () const
{
    Collection<ThreadPool::TaskType>    result;
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            shared_ptr<MyRunnable_>     tr      { i->fRunnable };
            TaskType                    task    { tr->GetCurrentTask () };
            if (task != nullptr) {
                result.Add (task);
            }
        }
    }
    return result;
}

size_t  ThreadPool::GetTasksCount () const
{
    size_t  count   =   0;
    {
        // First see if its in the Q
        auto    critSec { make_unique_lock (fCriticalSection_) };
        count += fTasks_.size ();
        for (auto i = fThreads_.begin (); i != fThreads_.end (); ++i) {
            shared_ptr<MyRunnable_>     tr      { i->fRunnable };
            TaskType                    task    { tr->GetCurrentTask () };
            if (task != nullptr) {
                count++;
            }
        }
    }
    return count;
}

size_t   ThreadPool::GetPendingTasksCount () const
{
    size_t  count   =   0;
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
        count += fTasks_.size ();
    }
    return count;
}

void    ThreadPool::WaitForDoneUntil (Time::DurationSecondsType timeoutAt) const
{
    Debug::TraceContextBumper ctx ("ThreadPool::WaitForDoneUntil");
    Require (fAborted_);
    {
        Collection<Thread>  threadsToShutdown;  // cannot keep critical section while waiting on subthreads since they may need to acquire the critsection for whatever they are doing...
        {
            auto    critSec { make_unique_lock (fCriticalSection_) };
            for (auto ti : fThreads_) {
                threadsToShutdown.Add (ti.fThread);
            }
        }
        for (auto t : threadsToShutdown) {
            t.WaitForDoneUntil (timeoutAt);
        }
    }
}

void    ThreadPool::Abort ()
{
    Debug::TraceContextBumper ctx ("ThreadPool::Abort");
    fAborted_ = true;
    {
        // Clear the task Q and then abort each thread
        auto    critSec { make_unique_lock (fCriticalSection_) };
        fTasks_.clear ();
        for (auto ti : fThreads_) {
            ti.fThread.Abort ();
        }
    }
}

void    ThreadPool::AbortAndWaitForDone (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx ("ThreadPool::AbortAndWaitForDone");
    Thread::SuppressInterruptionInContext ctx; // must cleanly shut down each of our subthreads - even if our thread is aborting...
    Abort ();
    WaitForDone (timeout);
}

// THIS is called NOT from 'this' - but from the context of an OWNED thread of the pool
void    ThreadPool::WaitForNextTask_ (TaskType* result)
{
    RequireNotNull (result);
    if (fAborted_) {
        Execution::DoThrow (Thread::AbortException ());
    }

    while (true) {
        {
            auto    critSec { make_unique_lock (fCriticalSection_) };
            if (not fTasks_.empty ()) {
                *result =   fTasks_.front ();
                fTasks_.pop_front ();
                DbgTrace ("ThreadPool::WaitForNextTask_ () is starting a new task");
                return;
            }
        }

        // Prevent spinwaiting... This event is SET when any new item arrives
        //DbgTrace ("ThreadPool::WaitForNextTask_ () - about to wait for added tasks");
        fTasksAdded_.Wait ();
        //DbgTrace ("ThreadPool::WaitForNextTask_ () - completed wait for added tasks");
    }
}

ThreadPool::TPInfo_      ThreadPool::mkThread_ ()
{
    shared_ptr<MyRunnable_> r   { new ThreadPool::MyRunnable_ (*this) };
    Thread  t   =   Thread ([r] () { r->Run (); });
    static  int sThreadNum_ =   1;  // race condition for updating this number, but who cares - its purely cosmetic...
    t.SetThreadName (Characters::CString::Format (L"Thread Pool Entry %d", sThreadNum_++));
    t.Start ();
    return TPInfo_ { t, r };
}
