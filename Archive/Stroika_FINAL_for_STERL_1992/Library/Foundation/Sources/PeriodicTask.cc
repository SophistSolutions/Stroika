/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PeriodicTask.cc,v 1.10 1992/10/10 04:27:57 lewis Exp $
 *
 * TODO:
 *
 *		-	Big performance win by cleaning up PeriodTask running code - pay attention to periodicities, and
 *			calc a figure of how often the peridoci task manager istlf must be called, for X AddIdleTask call,
 *			and for Mac WaitNextEvent call.
 *
 *
 * Changes:
 *	$Log: PeriodicTask.cc,v $
 *		Revision 1.10  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.9  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.8  1992/10/07  23:25:42  sterling
 *		Started getting ready for swithc to GenClass macros.
 *
 *		Revision 1.7  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.6  1992/09/15  17:14:39  lewis
 *		Get rid of AbSequence.
 *
 *		Revision 1.5  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.3  1992/07/21  06:02:42  lewis
 *		Use Sequence_DoubleLinkList instead of obsolete Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.2  1992/07/02  03:24:47  lewis
 *		Renamed Sequence_DoublyLLOfPointers->Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.1  1992/06/19  22:29:57  lewis
 *		Initial revision
 *
 *		Revision 1.6  1992/03/30  14:19:41  lewis
 *		Add ifdefs around Implement macro call (no templates only).
 *
 *
 *
 */



#include	"Debug.hh"
#include	"Memory.hh"

#include	"PeriodicTask.hh"





#if		!qRealTemplatesAvailable
	#include SequenceOfPeriodicTaskPtr_cc
#endif




/*
 ********************************************************************************
 *********************************** PeriodicTask *******************************
 ********************************************************************************
 */

PeriodicTask::PeriodicTask ():
	fPeriodicity (0)
{
}

PeriodicTask::PeriodicTask (const Time& periodicity):
	fPeriodicity (periodicity)
{
}




/*
 ********************************************************************************
 ****************************** PeriodicTaskQueue *******************************
 ********************************************************************************
 */
PeriodicTaskQueue::PeriodicTaskQueue ():
	fTasks (),
	fLastTaskIndex (1)
{
}

void	PeriodicTaskQueue::AddTask (PeriodicTask* task)
{
	RequireNotNil (task);
	fTasks.Append (task);
}

void	PeriodicTaskQueue::RemoveTask (PeriodicTask* task)
{
	RequireNotNil (task);
	fTasks.RemoveAt (fTasks.IndexOf (task));
}

void	PeriodicTaskQueue::Run (const Time& timeToUse)
{
	size_t	firstTaskRun	=	fLastTaskIndex;		// dont run any task more than once
	Time			stopAt			=	timeToUse	+	GetCurrentTime ();

	do {
		if (fLastTaskIndex <= fTasks.GetLength ()) {
			/*
			 * If there is a task to execute, then execute it.
			 */
			PeriodicTask*	t	=	fTasks [fLastTaskIndex];
			AssertNotNil (t);
			t->RunABit ();
		}
		if (++fLastTaskIndex > fTasks.GetLength ()) {
			fLastTaskIndex = 1;
		}
	} while ((stopAt >= GetCurrentTime ()) and (firstTaskRun != fLastTaskIndex));
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

