/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ThreadPool_h_
#define _Stroika_Foundation_Execution_ThreadPool_h_ 1

#include "../StroikaPreComp.h"

#include <list>
#include <mutex>

#include "../Containers/Collection.h"
#include "../Containers/Queue.h"

#include "Function.h"
#include "Thread.h"
#include "WaitableEvent.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 *      @todo   ThreadPool::WaitForTask () is a very sloppy inefficient implementation.
 *
 *      @todo   See if I can simplify use of critical sections with Synchronized!!!
 *
 *      @todo   Just got rid of some fCriticalSection_ use - review - maybe can get rid of ALL of it!
 *
 *      @todo   REDO USING Stroika Q - CONSIDER USE OF blcoking q - I htink it will help. Or firgure out
 *              how these tie together. Issue with using Queue<> is that we nee dto be able to remove
 *              something from the middle of it, which isnt reasonable for a queue. Or is it?
 *
 *              Not sure if fix is to use Stroika sequnece here, or to change Queue to allow Remove(iterator);
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

namespace Stroika {
    namespace Foundation {
        namespace Execution {

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
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
             *          all methods can be freely used from any thread, and they will block internally as needed.
             */
            class ThreadPool {
            public:
                ThreadPool (unsigned int nThreads = 0, const Memory::Optional<Characters::String>& threadPoolName = {});
                ThreadPool (ThreadPool&&)      = default;
                ThreadPool (const ThreadPool&) = delete;

            public:
                nonvirtual ThreadPool& operator= (ThreadPool&&) = default;
                nonvirtual ThreadPool& operator= (const ThreadPool&) = delete;

            public:
                /*
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
                 */
                using TaskType = Function<void()>;

            public:
                /**
                 */
                nonvirtual unsigned int GetPoolSize () const;

            public:
                /**
                 *  SetPoolSize () is advisory. It attempts to add or remove entries as requested.
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
                 *      ThreadPool p;
                 *      p.AddTask ([] () {doIt ();});
                 *      \endcode
                 *
                 *  \note   Design Note:
                 *      The reason this returns as TaskType is that its easy to convert a lambda or whatever into a TaskType, but if you do
                 *      it multiple times you get different (!=) values. So to make the auto conversion work easier without needing
                 *      to first create a variable, and then do the add task, you can just do them together. And it avoids mistakes like:
                 *          function<void()> f = ...;
                 *          p.AddTask(f);
                 *          p.RemoveTask (p);   // fails cuz differnt 'TaskType' added - f converted to TaskType twice!
                 */
                nonvirtual TaskType AddTask (const TaskType& task);

            public:
                /**
                 *  It is NOT an error to call this with a task that is not in the Queue
                 *  (since it would be a race to try to find out if it was already executed.
                 *
                 *  It can cancel a task if it has not yet been started, or EVEN if its already in
                 *  progress (see Thread::Abort - it sends abort signal)
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
                 *  Includes those QUEUED AND those Running (IsPresent)
                 */
                nonvirtual Containers::Collection<TaskType> GetTasks () const;

            public:
                /**
                 *  return all tasks which are currently running (assigned to some thread  in the thread pool).
                 *  \note - this is a snapshot in time of something which is often rapidly changing, so by the time
                 *  you look at it, it may have changed (but since we use shared_ptr's, its always safe to look at).
                 */
                nonvirtual Containers::Collection<TaskType> GetRunningTasks () const;

            public:
                /**
                 *  This INCLUDES those with properly IsPresent () - those running or ready to run.
                 *  This is GetRunningTasks().size () + GetPendingTasksCount ()
                 */
                nonvirtual size_t GetTasksCount () const;

            public:
                /**
                 *  This only tasks waiting to be run (queued but not yet running because the thread pool
                 *  has no available slots).
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
                nonvirtual void WaitForNextTask_ (TaskType* result);
                nonvirtual TPInfo_ mkThread_ ();

            private:
                /*
                 *  @todo - make clear what fCriticalSection_ protects, and maybe redo using synchonized.
                 */
                mutable recursive_mutex              fCriticalSection_;
                atomic<bool>                         fAborted_{false};
                Containers::Collection<TPInfo_>      fThreads_;                                    // all threads, and a data member for thread object, and one for running task, if any
                list<TaskType>                       fPendingTasks_;                               // tasks not yet running - @todo Use Stroika Queue
                WaitableEvent                        fTasksMaybeAdded_{WaitableEvent::eAutoReset}; // recheck for new tasks (or other events - wakeup waiters on fTasks)
                atomic<unsigned int>                 fNextThreadEntryNumber_{1};
                Memory::Optional<Characters::String> fThreadPoolName_;

            private:
                friend class MyRunnable_; // So MyRunnable_ can call WaitForNextTask_()
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ThreadPool.inl"

#endif /*_Stroika_Foundation_Execution_ThreadPool_h_*/
