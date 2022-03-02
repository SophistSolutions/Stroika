/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Scheduler.cc,v 1.7 1992/12/05 17:39:08 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Scheduler.cc,v $
 *		Revision 1.7  1992/12/05  17:39:08  lewis
 *		Renamed Try->try, and Catch->catch - see exception.hh for explation.
 *
 *		Revision 1.6  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.5  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.4  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.2  1992/07/08  01:27:08  lewis
 *		Get rid of use of old GetMicroseconds () method of Time - GetSeconds () now
 *		returns a real ...
 *
 *		Revision 1.1  1992/06/19  22:29:57  lewis
 *		Initial revision
 *
 *
 */

#if 0

#include	"OSRenamePre.hh"
#if		qMacOS
#include	<OSUtils.h>
#include	<Timer.h>
#endif	/*qMacOS*/
#include	"OSRenamePost.hh"


#include	"Exception.hh"
#include	"OSConfiguration.hh"

#include	"Scheduler.hh"


#if		qMacOS
#define	qUseTimeManager	1

Exception	gTimeManagerNotSupportedOnThisMachine;



/************ERZOTZ STUFF ***********/


static	struct	TMBlock {		// time mgr after sys 6.03 puts ptr to this in a0
	size_t	savedA5;
	TMTask	qElt;
}	sTMBlock;


TimeManagerQueue&	TimeManagerQueue::Get ()
{
	static	TimeManagerQueue	theTMQ;
	return (theTMQ);
}

TimeManagerQueue::TimeManagerQueue ():
	fTasks (),
	fTasksLastRun (),
	fTasksRunInterval (),
	fSemaphore (0),
	fNextIndex (1),		// since we incr first, and use that one, then wrap
	fTimeMgrPeriodicity (kMaxInt16)
{
#if		qUseTimeManager
	if (OSConfiguration ().RevisedTimeManagerAvailable ()) {
		register	size_t	a5	=	(size_t)SetA5 (0);
		SetA5 (a5);
	
		sTMBlock.savedA5 = a5;
		sTMBlock.qElt.qLink = Nil;
		sTMBlock.qElt.qType = vType;
		sTMBlock.qElt.tmAddr = &TimeMgrProc;
		sTMBlock.qElt.tmCount = 0;
		::InsTime ((QElem*)&sTMBlock.qElt);

		CalcTimeMgrPeriodicity ();
	}
#endif	/*qUseTimeManager*/
}

TimeManagerQueue::~TimeManagerQueue ()
{
	Require (fTasks.IsEmpty ());
#if		qUseTimeManager
	if (OSConfiguration ().RevisedTimeManagerAvailable ()) {
		::RmvTime ((QElem*)&sTMBlock.qElt);		// otw we get bad crashes (used to under sys 6?)
	}
#endif	/*qUseTimeManager*/
}

void	TimeManagerQueue::Add (Task* task)
{
	Add (task, 0);
}

void	TimeManagerQueue::Add (Task* task, const Time& periodicity)
{
#if		qUseTimeManager
	if (not OSConfiguration ().RevisedTimeManagerAvailable ()) {
		gTimeManagerNotSupportedOnThisMachine.Raise ();
	}
	while (fSemaphore++) 	fSemaphore--;		// snag resource
	fTasks.Append (task);
	fTasksLastRun.Append (Time (0));
	fTasksRunInterval.Append (periodicity);
	Assert (fTasks.GetLength () >= 1);
	CalcTimeMgrPeriodicity ();
	fSemaphore--;								// release resource
#else	/*qUseTimeManager*/
	AssertNotReached ();
#endif	/*qUseTimeManager*/
}

void	TimeManagerQueue::Remove (Task* task)
{
#if		qUseTimeManager
	if (not OSConfiguration ().RevisedTimeManagerAvailable ()) {
		gTimeManagerNotSupportedOnThisMachine.Raise ();
	}
	while (fSemaphore++) 	fSemaphore--;		// snag resource
	size_t	i	=	fTasks.IndexOf (task);
	fTasks.RemoveAt (i);
	fTasksLastRun.RemoveAt (i);
	fTasksRunInterval.RemoveAt (i);
	CalcTimeMgrPeriodicity ();
	fSemaphore--;								// release resource
#endif	/*qUseTimeManager*/
}

Time	TimeManagerQueue::GetPeriodicity (Task* task)
{
	Require (OSConfiguration ().RevisedTimeManagerAvailable ());
	return (fTasksRunInterval [fTasks.IndexOf (task)]);
}

void	TimeManagerQueue::SetPeriodicity (Task* task, const Time& periodicity)
{
	Require (OSConfiguration ().RevisedTimeManagerAvailable ());
	fTasksRunInterval.SetAt (periodicity, fTasks.IndexOf (task));
	CalcTimeMgrPeriodicity ();
}

void	TimeManagerQueue::CalcTimeMgrPeriodicity ()
{
#if		qUseTimeManager
	Require (OSConfiguration ().RevisedTimeManagerAvailable ());

	fTimeMgrPeriodicity = kMaxInt16;		// for now - should loop thru intervals.

	ForEach (Time, it, fTasksRunInterval) {
		Int32	milliSeconds  = it.Current ().GetSeconds () / 1000.0;
		if (milliSeconds < fTimeMgrPeriodicity) {
			Assert (sizeof (fTimeMgrPeriodicity) == sizeof (kMaxInt16));
			fTimeMgrPeriodicity = (UInt16) ((milliSeconds <= kMaxInt16)? milliSeconds: kMaxInt16);
		}
	}
	if (fTasksRunInterval.GetLength () != 0) {
		fTimeMgrPeriodicity = UInt16 (fTimeMgrPeriodicity / fTasksRunInterval.GetLength ());	// so definitly soon enuf.(worst case all q elts same)
	}

	if (fTimeMgrPeriodicity == 0) {
		fTimeMgrPeriodicity = 1;	// zero means shut off
	}

// not sure about this logic - maybe wrong or undocumented???
	if ((fTimeMgrPeriodicity < sTMBlock.qElt.tmCount) or (sTMBlock.qElt.tmCount == 0)) {
		/*
		 * I think I need to do this, since otherwise if lowering count, seems to wait
		 * for longer count, which when going from 32secs to 1/24 secs is a big loss.  From
		 * docs it seems a call to ::PrimeTime () is all that is necessary, but experimentally
		 * for system 7.0a9 (at least) the below hack is necesary.
		 */
		::RmvTime ((QElem*)&sTMBlock.qElt);
		::InsTime ((QElem*)&sTMBlock.qElt);
		::PrimeTime ((QElem*)&sTMBlock.qElt, fTimeMgrPeriodicity);
	}
#endif	/*qUseTimeManager*/
}

static	pascal	size_t	SnagA5BackOffA0 () = { 0x2ea9, 0xfffc };	// after system 6.0.3 can snag
																	// proper a5 in a0 (see TMBlock above, and technotes)
pascal	void	TimeManagerQueue::TimeMgrProc ()
{
#if		qUseTimeManager
	size_t				properA5	=	SnagA5BackOffA0 ();
	size_t				savedA5		=	(size_t)SetA5 (properA5);
	TimeManagerQueue&	theTMQ		=	Get ();

	Require (OSConfiguration ().RevisedTimeManagerAvailable ());

	/* should do some hack to test globals and be sure proper - using magic # */

	/*
	 * If cannot snag resource, try later - someone accessing and we cannot sleep.
	 */
	if (theTMQ.fSemaphore++) {
		goto Done;
	}

	if (theTMQ.fNextIndex > theTMQ.fTasks.GetLength ()) {
		theTMQ.fNextIndex = 0;
	}
	else {
		Time	now			=	GetCurrentTime ();
		Time	lastRunAt	=	theTMQ.fTasksLastRun [theTMQ.fNextIndex];
		Time	runInterval	=	theTMQ.fTasksRunInterval [theTMQ.fNextIndex];
		if (now > lastRunAt + runInterval) {
			try {
				theTMQ.fTasks [theTMQ.fNextIndex]->RunABit ();
			}
			catch () {
#if		qDebug
				DebugMessage ("caught exception in time manager proc - bad news?");
#endif	/*qDebug*/
			}
			theTMQ.fTasksLastRun.SetAt (GetCurrentTime (), theTMQ.fNextIndex);
		}
	}
	theTMQ.fNextIndex++;


Done:
	// always need to reset
	::PrimeTime ((QElem*)&sTMBlock.qElt, theTMQ.fTimeMgrPeriodicity);
	theTMQ.fSemaphore--;								// release resource

	(void)SetA5 (savedA5);
#endif	/*qUseTimeManager*/
}


#endif	/*qMacOS*/



/**************** REAL STUFFF */


#if		0
Scheduler::Scheduler ()
{
}

void	Scheduler::Go ()
{
#if		0
Again:
	/*
	 * The way we go back and forth from process to process, is by
	 * doing a longjump to the right process, or scheduler.  When we
	 * get jumped to, reshedule, and  run the highest priority process.
	 */
	(void)setjmp (fJumpBuffer);

	Reschedule ();

	if (not (fProcesses.Empty ())) {
/*		fProcesses[1]->DoRun ();	*/
	}
	goto	Again;
#endif
}

void	Scheduler::Reschedule ()
{
	/* WHAT ABOUT FAIRNESS??? */
	/* JUST SHUFFLE FIRST TO LAST, AND BUMP EVERYONE UP ONE */
	/* FAIR, BUT NO PRIORITIES...  RESEARCH PROJECT TO FIGURE OUT */
	/* GOOD ALGORITHM */
}
#endif


#endif

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


