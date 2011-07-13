/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Task.cc,v 1.8 1992/10/10 03:16:25 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Task.cc,v $
 *		Revision 1.8  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.7  1992/10/07  23:34:24  sterling
 *		Add conditional support for new GenClass stuff.
 *
 *		Revision 1.6  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.5  1992/09/15  17:18:06  lewis
 *		Get rid of AbSequence.
 *
 *		Revision 1.4  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.3  1992/07/21  05:47:18  lewis
 *		Use Sequence_DoubleLinkList instead of Sequence_DoubleLinkListPtr since
 *		the later is now obsolete.
 *
 *		Revision 1.2  1992/07/02  03:34:20  lewis
 *		Renamed Sequence_DoublyLLOfPointers ->Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.1  1992/06/19  22:29:57  lewis
 *		Initial revision
 *
 *		
 *
 */



#include	"Debug.hh"
#include	"Memory.hh"

#include	"Task.hh"




/*
 ********************************************************************************
 ************************************** Task ************************************
 ********************************************************************************
 */

Task::Task (Priority priority):
	fPriority (priority)
{
}

Task::Priority	Task::GetPriority ()
{
	return (fPriority);
}

void	Task::SetPriority (Priority p)
{
	fPriority = p;
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


