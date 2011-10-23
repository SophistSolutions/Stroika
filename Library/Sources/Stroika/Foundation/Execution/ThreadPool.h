/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_ThreadPool_h_
#define	_Stroika_Foundation_Execution_ThreadPool_h_	1

#include	"../StroikaPreComp.h"

#include	<list>
#include	<vector>

#include	"CriticalSection.h"
#include	"Thread.h"


/*
 *	TODO:
 *
 *			IMPLEMENT - this is just a rough draft/outline. NOT WORKABLE YET.
 */

// ADD THREADPOOL (Thread Pool) SUPPORT:
//
//		Basic idea is to separate out this existing class into two parts - the part that has
//		NotifyOfAbort/MainLoop () - and the surrounding code that does the ThreadMain etc stuff.
//
//		Then do one backend impl that does just what we have here, and one that pools the thread objects
//		(handles) - and keeps them sleeping when not used.
//
//		LOW PRIORITY - but a modest change. Its mainly useful for servicing lost-cost calls/threads, where
//		the overhead of constructing the thread is significant compared to the cost of performing the
//		action, and where the priority & stacksize can all be predeterimed and 'shared'.
//

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			using	Memory::SharedPtr;


			/*
			 * The ThreadPool class takes a small fixed number of Thread objects, and lets you use them as if there were many more.
			 * You submit a task (representable as a copyable IRunnable) - and it gets eventually executed.
			 */
			class	ThreadPool {
				public:
					ThreadPool (unsigned int nThreads = 0);
				private:
					NO_COPY_CONSTRUCTOR (ThreadPool);
					NO_ASSIGNMENT_OPERATOR (ThreadPool);

				public:
					typedef	SharedPtr<IRunnable>	TaskType;

				public:
					nonvirtual	unsigned int	GetPoolSize () const;
					// This could be tricky to implement if tasks are busy running. Perhaps require no tasks in Queue to run
					nonvirtual	void			SetPoolSize (unsigned int poolSize);

					/*
					 * Push the given task into the queue.
					 */
					nonvirtual	void	AddTask (const TaskType& task);

					/*
					 * It is NOT an error to call this with a task that is not in the Queue (since it would be a race to try to find out if it was
					 * already executed.
					 *
					 * It can cancel a task if it has not yet been started, or EVEN if its already in
					 * progress (see Thread::Abort - it sends abort signal)
					 */
					nonvirtual	void	AbortTask (const TaskType& task);

					// returns true if queued OR actively running
					nonvirtual	bool	IsQueued (const TaskType& task);
					
					// returns true if queued OR actively running
					nonvirtual	bool	IsRunning (const TaskType& task);

				public:
					nonvirtual	vector<TaskType>	GetTasks () const;
					
					// This INCLUDES those with properly IsQueued () - those running or ready to run
					nonvirtual	size_t				GetTasksCount () const;
				
				public:
					// throws if timeout
					nonvirtual	void	WaitForDone (Time::DurationSecondsType timeout = Time::kInfinite) const;
					// throws if timeout
					nonvirtual	void	AbortAndWaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);

				private:
					CriticalSection	fCriticalSection_;
					vector<Thread>	fThreads_;
					list<TaskType>	fTasks_;			// Use Stroika Queue
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_ThreadPool_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ThreadPool.inl"
