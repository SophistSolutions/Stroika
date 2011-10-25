/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"ThreadPool.h"



using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;



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