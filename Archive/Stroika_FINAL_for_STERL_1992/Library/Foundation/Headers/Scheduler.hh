/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Scheduler__
#define	__Scheduler__

/*
 * $Header: /fuji/lewis/RCS/Scheduler.hh,v 1.8 1992/10/10 20:19:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Scheduler.hh,v $
 *		Revision 1.8  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.7  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.6  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.4  1992/07/21  06:24:11  lewis
 *		Use Sequence instead of SequencePtr since
 *		the later is now obsolete.
 *
 *		Revision 1.3  1992/07/10  22:24:25  lewis
 *		Scope reference to time constant.
 *
 *		Revision 1.2  1992/07/02  03:03:00  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *
 */

#if 0

#include	<setjmp.h>

#include	"Config-Foundation.hh"
#include	"Task.hh"
#include	"Time.hh"
#include	"Sequence.hh"


// Not sure if we want to do a portable one of these
// not sure what to do about this...

#if		qMacOS

// Since this stuff insnt real, put task queue stuff here


// probably all these queues should inherit from a base SCHEDULER class.  Maybe application
// subclasses from this to provide idle tasks?

/*
 * NB: Periodicity for this class means end of one run to start of next (not start to start).
 */

class	TimeManagerQueue {
	public:
		TimeManagerQueue ();
		~TimeManagerQueue ();

		nonvirtual	void	Add (Task* task, const Time& periodicity);
		nonvirtual	void	Add (Task* task);
		nonvirtual	void	Remove (Task* task);

		nonvirtual	Time	GetPeriodicity (Task* task);
		nonvirtual	void	SetPeriodicity (Task* task, const Time& periodicity);

		static	TimeManagerQueue&	Get ();

	private:
		Sequence(TaskPtr)	fTasks;
		Sequence(Time)		fTasksLastRun;
		Sequence(Time)		fTasksRunInterval;
		UInt8				fSemaphore;
		size_t		fNextIndex;
		UInt16				fTimeMgrPeriodicity;		// in milliseconds

		nonvirtual	void	CalcTimeMgrPeriodicity ();

	static	pascal	void	TimeMgrProc ();
};






class	Scheduler {
	public:
		Scheduler ();

		nonvirtual	void	Go ();			// Start scheduling loop, and never stop.

		static	Scheduler&	THE;

	protected:
		virtual		void	Reschedule ();

	private:
		jmp_buf						fJumpBuffer;	// to return from Thread
//		Sequence<ProcessPtr>	fProcesses;

		friend	class	Thread;
};

#endif

#endif

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Scheduler__*/

