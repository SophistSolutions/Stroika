/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ThreadPool_h_
#define _Stroika_Foundation_Execution_ThreadPool_h_ 1

#include "../StroikaPreComp.h"

#include <list>
#include <mutex>

#include "../Containers/Collection.h"

#include "Function.h"
#include "Thread.h"
#include "WaitableEvent.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   ThreadPool::WaitForTask () is a very sloppy inefficient implementation.
 *
 *      @todo   CONSIDER USE OF blcoking q - I htink it will help. Or firgure out
 *              how these tie together. Or rewrite using condition_variable.
 *
 *      @todo   Current approach to aborting a running task is to abort the thread. But the current
 *              thread code doesn't support restarting a thread once its been aborted. We PROBABLY
 *              should correct that at some point - and allow a thread to undo its abort-in-progress.
 *              However - no need immediately. Instead - the current ThreadPool implementation simply
 *              drops that thread and builds a new one. Performacne overhead yes - but only for the
 *              likely rare case of aborting a running task.
 *
 *      @todo   Consider adding the idea of TaskGroups - which are properties shared by all tasks (or some tasks)
 *              added to a threadpool. If multiple tasks are added to the threadpool with the same
 *              TaskGroup, then they respect that taskgroup's contraints. One example constraint would be
 *              mutual run exclusion. This would allow you to create lockless threaded procedures, because
 *              they would be guaranteed to not be run all at the same time, and yet could STILL leverage
 *              the benefits of thread pooling.
 *
 *              For example, if you had 3 threads in the pool, and 5 thread groups, then typically one or
 *              more thread groups would be idle. The thread groups give you lockless execution, and
 *              the threadpool lets the 5 groups run 'at the same time' on only 3 threads.
 */

namespace Stroika::Foundation::Execution {

    /**
     *  The ThreadPool class creates a small fixed number of Thread objects, and lets you use them
     *  as if there were many more. You submit a task (representable as a comparable std::function - @see Function) -
     *  and it gets eventually executed.
     *
     *  If as Task in the thread pool raises an exception - this will be IGNORED (except for the
     *  special case of Thread::AbortException which  is used internally to end the threadpool or
     *  remove some threads). Because of this, your submitted runnables should take care of their own
     *  error handling internally.
     *
     *  ThreadPool mainly useful for servicing lost-cost calls/threads, where the overhead
     *  of constructing the thread is significant compared to the cost of performing the action,
     *  and where the priority & stacksize can all be predeterimed and 'shared'.
     *  Also - where you want to CONTROL the level of thread creation (possibly to avoid
     *  DOS attacks or just accidental overloading).
     *
     *  \req     Debug::AppearsDuringMainLifetime (); during the lifetime of the ThreadPool
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *          all methods can be freely used from any thread, and they will block internally as needed.
     */
    class [[nodiscard]] ThreadPool {
    public:
        /**
         *  \par Example Usage
         *      \code
         *          ThreadPool p{3};
         *          p.AddTask ([&q, &counter] () {
         *              ..dostuff..
         *          });
         *          // when goes out of scope automatically blocks waiting for threads to complete...
         *          // or call p.WaitForTasksDoneUntil ()
         *      \endcode
         */
        ThreadPool (unsigned int nThreads = thread::hardware_concurrency (), const optional<Characters::String>& threadPoolName = nullopt);
        ThreadPool (ThreadPool&&)      = delete;
        ThreadPool (const ThreadPool&) = delete;

    public:
        nonvirtual ThreadPool& operator= (ThreadPool&&)      = delete;
        nonvirtual ThreadPool& operator= (const ThreadPool&) = delete;

    public:
        /**
         *  Destroying a threadpool implicitly calls AbortAndWaitForDone () and eats any errors (cannot rethrow).
         *
         *  \note - ThreadPool used to have explicit abort methods, but there was no point. When aborting, you must wait for them all to
         *          shut down to destroy the object. And since you cannot restart the object, there is no point in ever aborting without destorying.
         *          so KISS - just destory the ThreadPool object.
         */
        ~ThreadPool ();

    public:
        /**
         *  \note   It is imporant (required) that all tasks added to a ThreadPool respond in a timely manner to Thread Abort.
         *          ThreadPool counts on that for clean shutdown.
         * 
         *          This means periodically calling CheckForInterruption () and that any waits respect thread cancelation (stop_token).
         * 
         *          Tasks may exit via exception, but nothing will be done with that exception (beyond DbgTrace loggging). So generally
         *          not a good idea, except for ThreadAbort handling.
         */
        using TaskType = Function<void ()>;

    public:
        /**
         *  This returns the number of threads in the pool (not the number of tasks). Note 0 is a legal size.
         */
        nonvirtual unsigned int GetPoolSize () const;

    public:
        /**
         *  SetPoolSize () is advisory. It attempts to add or remove entries as requested. Note - 0 is a legal size.
         *
         *  But under some circumstances, it will fail. For example, if tasks are busy
         *  running on all threads, the number of threads in the pool cannot be decreased.
         *
         *  @todo - WE CAN do better than this - at least marking the thread as to be removed when the
         *      task finsihes - but NYI
         */
        nonvirtual void SetPoolSize (unsigned int poolSize);

    public:
        /**
         *  Push the given task into the queue.
         *
         *  \par Example Usage
         *      \code
         *          ThreadPool p;
         *          p.AddTask ([] () {doIt ();});
         *      \endcode
         *
         *  \note   Design Note:
         *      The reason this returns as TaskType is that its easy to convert a lambda or whatever into a TaskType, but if you do
         *      it multiple times you get different (!=) values. So to make the auto conversion work easier without needing
         *      to first create a variable, and then do the add task, you can just do them together. And it avoids mistakes like:
         *          function<void()> f = ...;
         *          p.AddTask(f);
         *          p.RemoveTask (p);   // fails cuz different 'TaskType' added - f converted to TaskType twice!
         */
        nonvirtual TaskType AddTask (const TaskType& task, const optional<Characters::String>& name = nullopt);

    public:
        /**
         *  It is NOT an error to call this with a task that is not in the Queue
         *  (since it would be a race to try to find out if it was already executed.
         *
         *  It can cancel a task if it has not yet been started, or EVEN if its already in
         *  progress (see Thread::Abort - it sends abort signal)
         * 
         *  The function doesn't return until the task has been successfully cancelled, or it throws if timeout.
         */
        nonvirtual void AbortTask (const TaskType& task, Time::DurationSecondsType timeout = Time::kInfinite);

    public:
        /**
         *  See AbortTask () - it aborts all tasks - if any.
         */
        nonvirtual void AbortTasks (Time::DurationSecondsType timeout = Time::kInfinite);

    public:
        /**
         *  returns true if queued OR actively running.
         *
         *  \req task != nullptr
         */
        nonvirtual bool IsPresent (const TaskType& task) const;

    public:
        /**
         *  returns true actively running
         *
         *  \req task != nullptr
         */
        nonvirtual bool IsRunning (const TaskType& task) const;

    public:
        /**
         *  throws if timeout. Returns when task has completed (or if not in task q)
         *
         *  \req task != nullptr
         */
        nonvirtual void WaitForTask (const TaskType& task, Time::DurationSecondsType timeout = Time::kInfinite) const;

    public:
        /**
         */
        struct TaskInfo {
            TaskType                            fTask;
            optional<Characters::String>        fName;
            optional<Time::DurationSecondsType> fRunningSince; // if missing, cuz not running

            nonvirtual bool IsRunning () const;
        };

    public:
        /**
         *  \brief returns GetPendingTasks () + GetRunningTasks () - but also some extra information about each task
         */
        nonvirtual Containers::Collection<TaskInfo> GetTasks () const;

    public:
        /**
         *  return all tasks which are queued, but haven't yet been assigned to a thread.
         */
        nonvirtual Containers::Collection<TaskType> GetPendingTasks () const;

    public:
        /**
         *  return all tasks which are currently running (assigned to some thread  in the thread pool).
         *  \note - this is a snapshot in time of something which is often rapidly changing, so by the time
         *  you look at it, it may have changed (but since we use shared_ptr's, its always safe to look at).
         */
        nonvirtual Containers::Collection<TaskType> GetRunningTasks () const;

    public:
        /**
         *  \brief return total number of tasks, either pending, or currently running.
         * 
         *  This is GetRunningTasks().size () + GetPendingTasks ().size (), or alternatively GetTasks.size (), but more efficient.
         */
        nonvirtual size_t GetTasksCount () const;

    public:
        /**
         *  \brief return GetPendingTasks ().size (), except probably much more efficient
         */
        nonvirtual size_t GetPendingTasksCount () const;

    public:
        /**
        *  Wait for the given amount of time for all (either given argument or all tasks in this thread pool) to be done.
        *
        *   When called with a specific set of tasks, this proceedure waits for exactly those tasks. When called with no task
        *   argument, it waits until GetTaskCount () == 0.
        *
        *   \note For the all-tasks overload, if new tasks are added to the thread pool, those are waited for too.
        *         But - its perfectly legal to add new tasks after this returns, so the task count could increase (if tasks were added) after
        *         this returns without exception (obviously there are likely more tasks if it returns with a timeout exception).
        *
        *  \note ***Cancelation Point***
        */
        nonvirtual void WaitForTasksDone (const Traversal::Iterable<TaskType>& tasks, Time::DurationSecondsType timeout = Time::kInfinite) const;
        nonvirtual void WaitForTasksDone (Time::DurationSecondsType timeout = Time::kInfinite) const;

    public:
        /**
        *  Wait for the given amount of time for all (either given argument or all tasks in this thread pool) to be done.
        *
        *   When called with a specific set of tasks, this proceedure waits for exactly those tasks. When called with no task
        *   argument, it waits until GetTaskCount () == 0.
        *
        *   \note For the all-tasks overload, if new tasks are added to the thread pool, those are waited for too.
        *         But - its perfectly legal to add new tasks after this returns, so the task count could increase (if tasks were added) after
        *         this returns without exception (obviously there are likely more tasks if it returns with a timeout exception).
        *
        *  \note ***Cancelation Point***
        */
        nonvirtual void WaitForTasksDoneUntil (const Traversal::Iterable<TaskType>& tasks, Time::DurationSecondsType timeoutAt) const;
        nonvirtual void WaitForTasksDoneUntil (Time::DurationSecondsType timeoutAt) const;

    private:
        nonvirtual void Abort_ () noexcept;

    private:
        nonvirtual void AbortAndWaitForDone_ () noexcept;

    public:
        /**
         *  a helpful debug dump of the ThreadPool status
         */
        nonvirtual Characters::String ToString () const;

    private:
        class MyRunnable_;

    private:
        struct TPInfo_ {
            Thread::Ptr             fThread;
            shared_ptr<MyRunnable_> fRunnable;
        };

    private:
        // Called internally from threadpool tasks - to wait until there is a new task to run.
        // This will not return UNTIL it has a new task to proceed with (except via exception like Thread::AbortException)
        nonvirtual void    WaitForNextTask_ (TaskType* result, optional<Characters::String>* resultName);
        nonvirtual TPInfo_ mkThread_ ();

    private:
        struct PendingTaskInfo_ {
            TaskType                     fTask;
            optional<Characters::String> fName;
        };
        /*
         *  fCriticalSection_ protectes fThreads_ and fPendingTasks_
         */
        mutable recursive_mutex         fCriticalSection_;
        atomic<bool>                    fAborted_{false};
        Containers::Collection<TPInfo_> fThreads_; // all threads, and a data member for thread object, and one for running task, if any
        list<PendingTaskInfo_>          fPendingTasks_;      // tasks not yet running - somewhat like a queue, but allow remove from middle
        WaitableEvent                   fTasksMaybeAdded_{}; // recheck for new tasks (or other events - wakeup waiters on fTasks);
        atomic<unsigned int>            fNextThreadEntryNumber_{1};
        optional<Characters::String>    fThreadPoolName_;

    private:
        friend class MyRunnable_; // So MyRunnable_ can call WaitForNextTask_()
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ThreadPool.inl"

#endif /*_Stroika_Foundation_Execution_ThreadPool_h_*/
