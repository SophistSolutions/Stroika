/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"Sleep.h"

#include	"ThreadPool.h"



using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;




class	ThreadPool::MyRunnable_ : public IRunnable {
	public:
		MyRunnable_ (ThreadPool& threadPool)
			: fThreadPool_ (threadPool)
			, fCurTask_ ()
			, fNextTask_ ()
			, fCurTaskUpdateCritSection_ ()
			{
			}

	public:
		ThreadPool::TaskType	GetCurrentTask () const
			{
				AutoCriticalSection critSect (fCurTaskUpdateCritSection_);
				// THIS CODE IS TOO SUBTLE - BUT BECAUSE OF HOW THIS IS CALLED - fNextTask_ will NEVER be in the middle of being updated during this code - so this test is OK
				// Caller is never in the middle of doing a WaitForNextTask - and because we have this lock - we aren't updateing fCurTask_ or fNextTask_ either
				Assert (fCurTask_.IsNull () or fNextTask_.IsNull ());	// one or both must be null
				return fCurTask_.IsNull ()? fNextTask_ : fCurTask_;
			}

	public:
		virtual	void	Run () override
			{
				// For NOW - allow ThreadAbort to just kill this thread. In the future - we may want to implement some sort of restartability

				// Keep grabbing new tasks, and running them
				while (true) {
					{
						fThreadPool_.WaitForNextTask_ (&fNextTask_);			// This will block INDEFINITELY until ThreadAbort throws out or we have a new task to run
						AutoCriticalSection critSect (fCurTaskUpdateCritSection_);
						Assert (not fNextTask_.IsNull ());
						Assert (fCurTask_.IsNull ());
						fCurTask_ = fNextTask_;
						fNextTask_.clear ();
						Assert (not fCurTask_.IsNull ());
						Assert (fNextTask_.IsNull ());
					}
					try {
						fCurTask_->Run ();
						fCurTask_.clear ();
					}
					catch (const ThreadAbortException&) {
						AutoCriticalSection critSect (fCurTaskUpdateCritSection_);
						fCurTask_.clear ();
						throw;	// cancel this thread
					}
					catch (...) {
						AutoCriticalSection critSect (fCurTaskUpdateCritSection_);
						fCurTask_.clear ();
						// other exceptions WARNING WITH DEBUG MESSAGE - but otehrwise - EAT/IGNORE
					}
				}
			}

	private:
		ThreadPool&					fThreadPool_;
		mutable CriticalSection		fCurTaskUpdateCritSection_;
		ThreadPool::TaskType		fCurTask_;
		ThreadPool::TaskType		fNextTask_;

	public:
		DECLARE_USE_BLOCK_ALLOCATION(MyRunnable_);
};






/*
 ********************************************************************************
 ********************************* Execution::ThreadPool ************************
 ********************************************************************************
 */
ThreadPool::ThreadPool (unsigned int nThreads)
	: fCriticalSection_ ()
	, fAborted_ (false)
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
	Debug::TraceContextBumper ctx (TSTR ("ThreadPool::SetPoolSize"));
	Require (not fAborted_);
	AutoCriticalSection	critSection (fCriticalSection_);
	fThreads_.reserve (poolSize);
	while (poolSize > fThreads_.size ()) {
		fThreads_.push_back (mkThread_ ());
	}

	if (poolSize < fThreads_.size ()) {
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
	//Debug::TraceContextBumper ctx (TSTR ("ThreadPool::AddTask"));
	Require (not fAborted_);
	{
		AutoCriticalSection	critSection (fCriticalSection_);
		fTasks_.push_back (task);
	}

	// Notify any waiting threads to wakeup and claim the next task
	fTasksAdded_.Set ();
}

void	ThreadPool::AbortTask (const TaskType& task, Time::DurationSecondsType timeout)
{
	Debug::TraceContextBumper ctx (TSTR ("ThreadPool::AbortTask"));
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
	//
	//

	// TODO:
	//		We walk the list of existing threads and ask each one if its (indirected - running task) is the given one and abort that task.
	//		But that requires we can RESTART an ABORTED thread (or that we remove it from the list - maybe thats better). THat COULD be OK
	//		actually since it involves on API changes and makes sense. The only slight issue is a peformace one but probably for soemthing
	//		quite rare.
	//
	//		Anyhow SB OK for now to just not allow aborting a task which has already started....
	Thread	thread2Kill;
	{
		AutoCriticalSection	critSection (fCriticalSection_);
		for (vector<Thread>::iterator i = fThreads_.begin (); i != fThreads_.end (); ++i) {
			SharedPtr<IRunnable>	tr	=	i->GetRunnable ();
			Assert (dynamic_cast<MyRunnable_*> (tr.get ()) != nullptr);
			SharedPtr<IRunnable>	ct	=	dynamic_cast<MyRunnable_&> (*tr.get ()).GetCurrentTask ();
			if (task == ct) {
				thread2Kill	=	*i;
				*i = mkThread_ ();
				break;
			}
		}
	}
	if (not thread2Kill.GetStatus () != Thread::eNull) {
		thread2Kill.AbortAndWaitForDone (timeout);
	}
}

bool	ThreadPool::IsPresent (const TaskType& task) const
{
	{
		// First see if its in the Q
		AutoCriticalSection	critSection (fCriticalSection_);
		for (list<TaskType>::const_iterator i = fTasks_.begin (); i != fTasks_.end (); ++i) {
			if (*i == task) {
				return true;
			}
		}
	}
	return IsRunning (task);
}

bool	ThreadPool::IsRunning (const TaskType& task) const
{
	Require (not task.IsNull ());
	{
		AutoCriticalSection	critSection (fCriticalSection_);
		for (vector<Thread>::const_iterator i = fThreads_.begin (); i != fThreads_.end (); ++i) {
			SharedPtr<IRunnable>	tr	=	i->GetRunnable ();
			Assert (dynamic_cast<MyRunnable_*> (tr.get ()) != nullptr);
			SharedPtr<IRunnable>	rTask	=	dynamic_cast<MyRunnable_&> (*tr.get ()).GetCurrentTask ();
			if (task == rTask) {
				return true;
			}
		}
	}
	return false;
}

void	ThreadPool::WaitForTask (const TaskType& task, Time::DurationSecondsType timeout) const
{
	Debug::TraceContextBumper ctx (TSTR ("ThreadPool::WaitForTask"));
	// Inefficient / VERY SLOPPY impl
	Time::DurationSecondsType	endAt	=	timeout + Time::GetTickCount ();
	while (true) {
		if (not IsPresent (task)) {
			return;
		}
		Time::DurationSecondsType	remaining	=	timeout - Time::GetTickCount ();
		if (remaining <= 0.0) {
			DoThrow (WaitTimedOutException ());
		}
		Execution::Sleep (min (remaining, 1.0));
	}
}

vector<ThreadPool::TaskType>	ThreadPool::GetTasks () const
{
	vector<ThreadPool::TaskType>	result;
	{
		AutoCriticalSection	critSection (fCriticalSection_);
		result.reserve (fTasks_.size () + fThreads_.size ());
		result.insert (result.begin (), fTasks_.begin (), fTasks_.end ());			// copy pending tasks
		for (vector<Thread>::const_iterator i = fThreads_.begin (); i != fThreads_.end (); ++i) {
			SharedPtr<IRunnable>	tr	=	i->GetRunnable ();
			Assert (dynamic_cast<MyRunnable_*> (tr.get ()) != nullptr);
			SharedPtr<IRunnable>	task	=	dynamic_cast<MyRunnable_&> (*tr.get ()).GetCurrentTask ();
			if (not task.IsNull ()) {
				result.push_back (task);
			}
		}
	}
	return result;
}

vector<ThreadPool::TaskType>	ThreadPool::GetRunningTasks () const
{
	vector<ThreadPool::TaskType>	result;
	{
		AutoCriticalSection	critSection (fCriticalSection_);
		result.reserve (fThreads_.size ());
		for (vector<Thread>::const_iterator i = fThreads_.begin (); i != fThreads_.end (); ++i) {
			SharedPtr<IRunnable>	tr	=	i->GetRunnable ();
			Assert (dynamic_cast<MyRunnable_*> (tr.get ()) != nullptr);
			SharedPtr<IRunnable>	task	=	dynamic_cast<MyRunnable_&> (*tr.get ()).GetCurrentTask ();
			if (not task.IsNull ()) {
				result.push_back (task);
			}
		}
	}
	return result;
}

size_t	ThreadPool::GetTasksCount () const
{
	size_t	count	=	0;
	{
		// First see if its in the Q
		AutoCriticalSection	critSection (fCriticalSection_);
		count += fTasks_.size ();
		for (vector<Thread>::const_iterator i = fThreads_.begin (); i != fThreads_.end (); ++i) {
			SharedPtr<IRunnable>	tr	=	i->GetRunnable ();
			Assert (dynamic_cast<MyRunnable_*> (tr.get ()) != nullptr);
			SharedPtr<IRunnable>	task	=	dynamic_cast<MyRunnable_&> (*tr.get ()).GetCurrentTask ();
			if (not task.IsNull ()) {
				count++;
			}
		}
	}
	return count;
}

void	ThreadPool::WaitForDone (Time::DurationSecondsType timeout) const
{
	Debug::TraceContextBumper ctx (TSTR ("ThreadPool::WaitForDone"));
	Require (fAborted_);
	{
		Time::DurationSecondsType	endAt	=	timeout + Time::GetTickCount ();
		AutoCriticalSection	critSection (fCriticalSection_);
		for (vector<Thread>::const_iterator i = fThreads_.begin (); i != fThreads_.end (); ++i) {
			i->WaitForDone (endAt - Time::GetTickCount ());
		}
	}
}

void	ThreadPool::Abort ()
{
	Debug::TraceContextBumper ctx (TSTR ("ThreadPool::Abort"));
	fAborted_ = true;
	{
		// First see if its in the Q
		AutoCriticalSection	critSection (fCriticalSection_);
		fTasks_.clear ();
		for (vector<Thread>::iterator i = fThreads_.begin (); i != fThreads_.end (); ++i) {
			i->Abort ();
		}
	}
}

void	ThreadPool::AbortAndWaitForDone (Time::DurationSecondsType timeout)
{
	Abort ();
	WaitForDone (timeout);
}

// THIS is called NOT from 'this' - but from the context of an OWNED thread of the pool
void	ThreadPool::WaitForNextTask_ (TaskType* result)
{
	RequireNotNull (result);
	if (fAborted_) {
		Execution::DoThrow (ThreadAbortException ());
	}

	while (true) {
		{
			AutoCriticalSection	critSection (fCriticalSection_);
			if (not fTasks_.empty ()) {
				*result	=	fTasks_.front ();
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

Thread		ThreadPool::mkThread_ ()
{
	Thread	t	=	Thread (SharedPtr<IRunnable> (new ThreadPool::MyRunnable_ (*this)));		// ADD MY THREADOBJ
	t.SetThreadName (L"Thread Pool");
	t.Start ();
	return t;
}
