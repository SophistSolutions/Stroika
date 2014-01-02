/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ThreadPool_h_
#define _Stroika_Foundation_Execution_ThreadPool_h_ 1

#include    "../StroikaPreComp.h"

#include    <list>
#include    <mutex>
#include    <vector>

#include    "Event.h"
#include    "Thread.h"



/**
 *  TODO:
 *
 *      @todo   Current approach to aborting a running task is to abort the thread. But the current
 *              thread code doesn't support restarting a thread once its been aborted. We PROBABLY
 *              should correct that at some point - and allow a thread to undo its abort-in-progress.
 *              However - no need immediately. Instead - the current ThreadPool implemenation simply
 *              drops that thread and builds a new one. Performacne overhead yes - but only for the
 *              likely rare case of aborting a running task.
 *
 *      @todo   Consider adding the idea of TaskGroups - which are properties shared by all tasks (or some tasks)
 *              added to a threadpool. If multiple tasks are added to the threadpool with the same
 *              TaskGroup, then they respect that taskgroup's contraints. One example constraint would be
 *              mutual run exclusion. This would allow you to create lockless threaded procedures, because
 *              they would be gauranteed to not be run all at the same time, and yet could STILL leverage
 *              the benefits of thread pooling.
 *
 *              For example, if you had 3 threads in the pool, and 5 thread groups, then typically one or
 *              more thread groups would be idle. The thread groups give you lockless execution, and
 *              the threadpool lets the 5 groups run 'at the same time' on only 3 threads.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  The ThreadPool class takes a small fixed number of Thread objects, and lets you use them
             *  as if there were many more. You submit a task (representable as a copyable IRunnable) -
             *  and it gets eventually executed.
             *
             *
             *  If as Task in the thread pool raises an exception - this will be IGNORED (except for the
             *  special case of ThreadAbortException which  is used internally to end the threadpool or
             *  remove some threads).
             *
             *  ThreadPool mainly useful for servicing lost-cost calls/threads, where the overhead
             *  of constructing the thread is significant compared to the cost of performing the action,
             *  and where the priority & stacksize can all be predeterimed and 'shared'.
             *  Also - where you want to CONTROL the level of thread creation (possibly to avoid
             *  DOS attacks or just accidental overloading).
             */
            class   ThreadPool {
            public:
                ThreadPool (unsigned int nThreads = 0);
                ThreadPool (const ThreadPool&) = delete;

            public:
                nonvirtual  const ThreadPool& operator= (const ThreadPool&) = delete;

            public:
                typedef shared_ptr<IRunnable>    TaskType;

            public:
                nonvirtual  unsigned int    GetPoolSize () const;
                /**
                 *  This could be tricky to implement if tasks are busy running.
                 *  Perhaps require no tasks in Queue to run
                 */
                nonvirtual  void            SetPoolSize (unsigned int poolSize);

            public:
                /**
                 *  Push the given task into the queue.
                 */
                nonvirtual  void    AddTask (const TaskType& task);

            public:
                /**
                 *  It is NOT an error to call this with a task that is not in the Queue
                 *  (since it would be a race to try to find out if it was already executed.
                 *
                 *  It can cancel a task if it has not yet been started, or EVEN if its already in
                 *  progress (see Thread::Abort - it sends abort signal)
                 */
                nonvirtual  void    AbortTask (const TaskType& task, Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 *  returns true if queued OR actively running
                 */
                nonvirtual  bool    IsPresent (const TaskType& task) const;

                /**
                 *  returns true actively running
                 */
                nonvirtual  bool    IsRunning (const TaskType& task) const;

                /**
                 *  throws if timeout. Returns when task has completed (or if not in task q)
                 */
                nonvirtual  void    WaitForTask (const TaskType& task, Time::DurationSecondsType timeout = Time::kInfinite) const;

            public:
                /**
                 *  Includes those QUEUED AND those Running (IsPresent)
                 */
                nonvirtual  vector<TaskType>    GetTasks () const;

            public:
                nonvirtual  vector<TaskType>    GetRunningTasks () const;

            public:
                /**
                 *  This INCLUDES those with properly IsPresent () - those running or ready to run
                 */
                nonvirtual  size_t              GetTasksCount () const;

            public:
                /**
                 *  throws if timeout
                 */
                nonvirtual  void    WaitForDone (Time::DurationSecondsType timeout = Time::kInfinite) const;
                /**
                 * Tells the ThreadPool to shutdown - once aborted - it is an error to keep adding new tasks
                 */
                nonvirtual  void    Abort ();
                /**
                 *  throws if timeout
                 */
                nonvirtual  void    AbortAndWaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);

            private:
                // Called internally from threadpool tasks - to wait until there is a new task to run.
                // This will not return UNTIL it has a new task to proceed with (except via exception like ThreadAbortException)
                nonvirtual  void        WaitForNextTask_ (TaskType* result);
                nonvirtual  Thread      mkThread_ ();

            private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                class   MyRunnable_;
            private:
                mutable recursive_mutex fCriticalSection_;
                bool                    fAborted_;
                vector<Thread>          fThreads_;
                list<TaskType>          fTasks_;            // Use Stroika Queue
                Event                   fTasksAdded_;
            private:
                friend  class   MyRunnable_;                // So MyRunnable_ can call WaitForNextTask_()
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ThreadPool.inl"

#endif  /*_Stroika_Foundation_Execution_ThreadPool_h_*/
