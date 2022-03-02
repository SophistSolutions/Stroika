/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Task__
#define	__Task__

/*
 * $Header: /fuji/lewis/RCS/Task.hh,v 1.9 1992/10/10 03:16:25 lewis Exp $
 *
 * Description:
 *
 *		Small Hack to support IDLE and VBL tasks.  Not sure how to orgainize
 *	this. Want to also someday support threads, etc...  Many of same
 *	problems????
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Task.hh,v $
 *		Revision 1.9  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.8  1992/10/07  23:08:47  sterling
 *		Conditionanly add supporot for new GenClass instead of declare macros.
 *
 *		Revision 1.7  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.6  1992/09/15  16:53:22  lewis
 *		Get rid of AbSequence
 *
 *		Revision 1.5  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.4  1992/09/01  15:20:18  sterling
 *		Lots.
 *
 *		Revision 1.3  1992/07/21  06:07:21  lewis
 *		Use Sequence_DoubleLinkList instead of obsolete Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.2  1992/07/02  03:20:41  lewis
 *		Renamed Sequence_DoublyLLOfPointers->Sequence_DoubleLinkListPtr.
 *
 */

#include	"Sequence.hh"


#if		!qRealTemplatesAvailable
	typedef	class	Task*	TaskPtr;
	#include "TFileMap.hh"
	#include SequenceOfTaskPtr_hh
#endif


class	Task {
	public:
		enum	Priority { eLoPriority = 0, eHiPriority = 100, eDefaultPriority = 50};

		Task (Priority priority = eDefaultPriority);

		/*
		 * OverRide this to do your thing.  Please be sure to return from this procedure
		 * quickly.  It will be called again at a time dependent of priority, and the
		 * kind of queue you install it on.
		 */
		virtual	void	RunABit ()	=	Nil;

		nonvirtual	Priority	GetPriority ();
		nonvirtual	void		SetPriority (Priority p);

	private:
		Priority	fPriority;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Task__*/


