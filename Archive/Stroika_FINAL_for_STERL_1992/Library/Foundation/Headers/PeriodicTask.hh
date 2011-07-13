/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PeriodicTask__
#define	__PeriodicTask__

/*
 * $Header: /fuji/lewis/RCS/PeriodicTask.hh,v 1.11 1992/10/10 03:56:51 lewis Exp $
 *
 * Description:
 *
 *	Better design for this stuff would be to use a mapping from PeriodTask
 *	ptrs to info structures containing the per-q iformation. This allows for a
 *	single task to be on more than 1 Queue, and for the task Q's to keep different
 *	kinds of information!!!
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: PeriodicTask.hh,v $
 *		Revision 1.11  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.10  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.9  1992/10/07  22:55:43  sterling
 *		Conditionally compile/use new GenClass stuff.
 *
 *		Revision 1.8  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.7  1992/09/15  16:59:13  lewis
 *		Got rid of AbSequence.
 *
 *		Revision 1.6  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.5  1992/09/01  15:20:18  sterling
 *		*** empty log message ***
 *
 *		Revision 1.4  1992/07/21  06:30:25  lewis
 *		Use Sequence_DoubleLinkList instead of obsolete Sequence_DoubleLinkListPTr.
 *
 *		Revision 1.3  1992/07/08  15:08:41  lewis
 *		Fixed refernece to kZeroSeconds to be Time::
 *
 *		Revision 1.2  1992/07/02  02:58:35  lewis
 *		Renamed Sequence_DoublyLLOfPointers to Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.1  1992/06/19  22:28:16  lewis
 *		Initial revision
 *
 *		Revision 1.6  1992/03/30  12:57:01  lewis
 *		Ported to BorlandC++ and added ifdefs for qRealTemplatesAvaialble.
 *
 *
 *
 */


#include	"Sequence.hh"
#include	"Time.hh"




class	PeriodicTask {
	public:
		PeriodicTask ();
		PeriodicTask (const Time& periodicity);

		/*
		 * OverRide this to do your thing.  Please be sure to return from this procedure
		 * quickly.  It will be called again at a time dependent of periodicity, and the
		 * kind of queue you install it on.
		 */
		virtual	void	RunABit ()	=	Nil;

		nonvirtual	Time	GetPeriodicity () const;
		nonvirtual	void	SetPeriodicity (const Time& periodicity);

		nonvirtual	Time	GetLastRanTime () const;
		nonvirtual	void	SetLastRanTime (const Time& lastRanTime);

	private:
		Time	fPeriodicity;
		Time	fLastRanAt;
};



#if		!qRealTemplatesAvailable
	typedef	PeriodicTask*	PeriodicTaskPtr;
	#include "TFileMap.hh"
	#include SequenceOfPeriodicTaskPtr_hh
#endif


class	PeriodicTaskQueue {
	public:
		PeriodicTaskQueue ();

		nonvirtual	void	AddTask (PeriodicTask* task);
		nonvirtual	void	RemoveTask (PeriodicTask* task);

		/*
		 * Run as many tasks as possible (no more than once) in the alloted time.
		 */
		nonvirtual	void	Run (const Time& timeToUse);

	private:
#if		qRealTemplatesAvailable
		Sequence<PeriodicTask*>	fTasks;
#else
		Sequence(PeriodicTaskPtr)		fTasks;
#endif
		size_t			fLastTaskIndex;
};




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	Time	PeriodicTask::GetPeriodicity () const					{	return (fPeriodicity);		}
inline	void	PeriodicTask::SetPeriodicity (const Time& periodicity)	{	fPeriodicity = periodicity;	}
inline	Time	PeriodicTask::GetLastRanTime () const					{	return (fLastRanAt);		}
inline	void	PeriodicTask::SetLastRanTime (const Time& lastRanTime)	{	fLastRanAt = lastRanTime;	}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__PeriodicTask__*/


