/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"ThreadPool.h"



using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;




class	ThreadPool::MyRunnable_ : public IRunnable {
	public:
		MyRunnable_ (ThreadPool& threadPool)
			: fThreadPool_ (threadPool)
			, fCurTask_ ()
			{
			}

	public:
		virtual	void	Run () override
			{
				// For NOW - allow ThreadAbort to just kill this thread. In the future - we may want to implement some sort of restartability

				// Keep grabbing new tasks, and running them
				while (true) {
					// DO WAITEVENT and GRABNEXT task - maybe have private method of 
					TaskType	t	=	fThreadPool_.WaitForNextTask_ ();
					try {
						t->Run ();
					}
					catch (const ThreadAbortException&) {
						throw;	// cancel this thread
					}
					catch (...) {
						// other excpetions WARNING WITH DEBUG MESSAGE - but otehrwise - EAT/IGNORE
					}
				}
			}

	private:
		ThreadPool&				fThreadPool_;
		ThreadPool::TaskType	fCurTask_;

	public:
		DECLARE_USE_BLOCK_ALLOCATION(MyRunnable_);
};





ThreadPool::ThreadPool (unsigned int nThreads)
	: fCriticalSection_ ()
	, fThreads_ ()
	, fTasks_ ()
	, fTasksAdded_ ()
{
	SetPoolSize (nThreads);
}

unsigned int	ThreadPool::GetPoolSize () const
{
	AutoCriticalSection	critSection (fCriticalSection_);
	return fThreads_.size ();
}

void	ThreadPool::SetPoolSize (unsigned int poolSize)
{
	AutoCriticalSection	critSection (fCriticalSection_);
	fThreads_.reserve (poolSize);
	while (poolSize > fTasks_.size ()) {
		fThreads_.push_back (Thread ());	// ADD MY THREADOBJ
	}

	if (poolSize < fTasks_.size ()) {
		AssertNotImplemented ();

		// MUST STOP THREADS and WAIT FOR THEM TO BE DONE (OR STORE THEM SOMEPLACE ELSE - NO - I THINK MUST ABORTANDWAIT().  Unsure.
		// For now - just assert!!!

		// TODO:
		//		(1)	HOIRRIBLE - NOW
		fThreads_.resize (poolSize);	// remove some off the end. OK if they are running?
	}
	
}

void	ThreadPool::AddTask (const TaskType& task)
{
	{
		AutoCriticalSection	critSection (fCriticalSection_);
		fTasks_.push_back (task);
	}

	// Notify any waiting threads to wakeup and claim the next task
	fTasksAdded_.Set ();
}

void	ThreadPool::AbortTask (const TaskType& task)
{
	{
		// First see if its in the Q
		AutoCriticalSection	critSection (fCriticalSection_);
		for (list<TaskType>::iterator i = fTasks_.begin (); i != fTasks_.end (); ++i) {
			if (*i == task) {
				fTasks_.erase (i);
				return;
			}
		}
	}

	// If we got here - its NOT in the task Q, so maybe its already running.
	{
		// TODO:
		//		We SHOULD walk the list of existing threads and ask each one if its (indirected - running task) is the given one and abort that task.
		//		But that requires we can RESTART an ABORTED thread (or that we remove it from the list - maybe thats better). THat COULD be OK
		//		actually since it involves on API changes and makes sense. The only slight issue is a peformace one but probably for soemthing
		//		quite rare.
		//
		//		Anyhow SB OK for now to just not allow aborting a task which has already started....
		AssertNotImplemented ();
	}
}

bool	ThreadPool::IsPresent (const TaskType& task)
{
	{
		// First see if its in the Q
		AutoCriticalSection	critSection (fCriticalSection_);
		for (list<TaskType>::iterator i = fTasks_.begin (); i != fTasks_.end (); ++i) {
			if (*i == task) {
				fTasks_.erase (i);
				return true;
			}
		}
	}

	return IsRunning (task);
	
	return false;
}

bool	ThreadPool::IsRunning (const TaskType& task)
{
	// TMPHACK - NOT IMPL PROEPRLY - MUST CHECK THOSE ACTIVELY RUNNING IN THREADS
	{
		AssertNotImplemented ();
	}
	return false;
}

vector<ThreadPool::TaskType>	ThreadPool::GetTasks () const
{
	AssertNotImplemented ();
	return vector<ThreadPool::TaskType> ();
}
					
size_t	ThreadPool::GetTasksCount () const
{
	AssertNotImplemented ();
	return 0;
}

void	ThreadPool::WaitForDone (Time::DurationSecondsType timeout) const
{
	AssertNotImplemented ();
}

void	ThreadPool::AbortAndWaitForDone (Time::DurationSecondsType timeout)
{
	AssertNotImplemented ();
}

ThreadPool::TaskType	ThreadPool::WaitForNextTask_ () const
{
	AssertNotImplemented ();

	// ROUGHLY - WAIT on fTasksAdded_
	//	THEN ENTER CRITICAL SECITON to try and see if fTasks is empoty (it could be since osmeone else could beat us to the punch)
	// and then if non-empty - return it, else wait again on fTasksAdded_.

	return TaskType();
}
