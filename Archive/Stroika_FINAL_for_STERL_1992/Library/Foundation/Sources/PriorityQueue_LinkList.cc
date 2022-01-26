/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__PriorityQueue_LinkList__cc
#define	__PriorityQueue_LinkList__cc

/*
 * $Header: /fuji/lewis/RCS/PriorityQueue_LinkList.cc,v 1.9 1992/11/13 03:36:34 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PriorityQueue_LinkList.cc,v $
 *		Revision 1.9  1992/11/13  03:36:34  lewis
 *		#if 0'd out everything - fix later.
 *
 *		Revision 1.8  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.7  1992/10/23  06:18:39  lewis
 *		Cleaned up some for GenClass stuff, but untested, and looks like it
 *		needs work.
 *
 *		Revision 1.6  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.5  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.4  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.3  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.2  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *
 */

#include	"LinkList.hh"

#include	"PriorityQueue_LinkList.hh"




#if		qRealTemplatesAvailable && 0

#include	"Collection.hh"


template	<class T> class	PriorityQueueRep_LinkList <PQEntry <T> > : public PriorityQueueRep<T>, public CollectionRep {
	public:
		PriorityQueueRep_LinkList ()					{}
		PriorityQueueRep_LinkList (Priority (*f) (T))	: PriorityQueueRep<T> (f)	{}

		override	size_t		GetLength () const;
	 	override	IteratorRep<PQEntry <T> >* MakeEntryIterator () const;
		override	void		Compact ();
		override	PriorityQueueRep<T>*	Clone () const;
		override	void		RemoveAll ();

		override	void		Enqueue (T item, Priority priority);
		override	T			Dequeue ();
		override	T			Head () const;
		override	Priority	TopPriority () const;
		override	void		RemoveHead ();

		LinkList<PQEntry<T> >	fData;
		
		nonvirual	Link<PQEntry<T> >*	ScanFor (Priority minPriority, size_t& index) const;
};




/*
 ********************************************************************************
 ******************************* PriorityQueueRep_LinkList **********************
 ********************************************************************************
 */
template	<class T>	size_t PriorityQueueRep_LinkList<PQEntry <T> >::GetLength () const
{
	return (fData.GetLength ());
}

template	<class	T>	Boolean	PriorityQueueRep_LinkList<PQEntry <T> >::Contains (PQEntry<T> item) const
{
	return (fData.Contains (item));
}

template	<class T>	PriorityQueueRep_LinkList<PQEntry <T> >::operator IteratorRep<T>* () const
{
	return (new LinkListIterator<T> (*this, fData));
}

template	<class T>	void PriorityQueueRep_LinkList<PQEntry <T> >::Compact ()
{
}

template	<class T>	CollectionRep*	PriorityQueueRep_LinkList<PQEntry <T> >::Clone () const
{
	PriorityQueueRep_LinkList<PQEntry <T> >* rep = new PriorityQueueRep_LinkList<PQEntry <T> > ();
	rep->fData = fData;
	rep->fPrioritize = fPrioritize;
	return (rep);
}

template	<class T>	void PriorityQueueRep_LinkList<PQEntry <T> >::RemoveAll ()
{
	PatchViewsRemoveAll ();
	fData.RemoveAll ();
}

template	<class T>	PriorityQueueRep_LinkList<PQEntry <T> >::operator IteratorRep PQEntry<T> * () const
{
	return (new LinkListIterator< PQEntry<T> > (*this, fData));
}

template	<class	T>	Link<PQEntry<T> >*	PriorityQueueRep_LinkList<PQEntry <T> >::ScanFor (Priority minPriority, size_t& index) const
{
	Link< PQEntry<T> >*	prev = Nil;
	Link< PQEntry<T> >*	node = fData.GetFirst ();
	index = 1;
	while ((node != Nil) and (node->GetItem ().fPriority > minPriority)) {
		prev = node;
		node = fData.GetNext (node);
		index++;
	}
	if (prev != Nil) {
		index++;
	}
	return (prev);
}

template	<class	T>	void	PriorityQueueRep_LinkList<PQEntry <T> >::Add (T item, Priority priority)
{
	size_t	index = 0;
	Link< PQEntry<T> >*	prev = ScanFor (priority, index);
	PQEntry	entry (item, priority);
	if (prev == Nil) {
		fData.Prepend (entry);
	}
	else {
		fData.AddAfter (entry, prev);
	}
	PatchViewsAdd (&index);
}

template	<class	T>	T		PriorityQueueRep_LinkList<PQEntry <T> >::Pop ()
{
	Require (GetLength () > 0);
	
	size_t index = 1;
	PatchViewsRemove (&index);
	T	item =	fData.GetFirst ()->GetItem ().fItem;
 	fData.Remove (fData.GetFirst ());
	return (item);	
}

template	<class	T>	Boolean	PriorityQueueRep_LinkList<PQEntry <T> >::PopIf (T& item, Priority minPriority)
{
	size_t	index = 0;
	Link< PQEntry<T> >*	prev = ScanFor (minPriority, index);
	if (prev == Nil) {
		return (False);
	}
	
	Link< PQEntry<T> >*	node = fData.GetNext (prev);
	AssertNotNil (node);
	item = node->GetItem ().fItem;
	PatchViewsRemove (&index);
	fData.Remove (node);
	return (True);
}

template	<class	T>	T		PriorityQueueRep_LinkList<PQEntry <T> >::First () const
{
	Require (GetLength () > 0);

	return (fData.GetFirst ()->GetItem ());
}

template	<class	T>	Boolean	PriorityQueueRep_LinkList<PQEntry <T> >::FirstIf (T& item, Priority minPriority) const
{
	size_t index = 0;
	Link< PQEntry<T> >*	prev = ScanFor (minPriority, index);
	if (prev == Nil) {
		return (False);
	}
	
	Link< PQEntry<T> >*	node = fData.GetNext (prev);
	AssertNotNil (node);
	item = node->GetItem ().fItem;
	return (True);
}

template	<class	T>	void	PriorityQueueRep_LinkList<PQEntry <T> >::RemoveFirstIf (Priority minPriority)
{
	size_t index = 0;
	Link< PQEntry<T> >*	prev = ScanFor (minPriority, index);
	if (prev != Nil) {
		Link< PQEntry<T> >*	node = fData.GetNext (prev);
		AssertNotNil (node);
		PatchViewsRemove (&index);
		fData.Remove (node);
	}
}


/*
 ********************************************************************************
 ********************************* PriorityQueue_LinkList ***********************
 ********************************************************************************
 */
template	<class T>	PriorityQueueLinkList<PQEntry <T> >::PriorityQueue_LinkList ()
{
	SetRep (new PriorityQueueRep_LinkList<PQEntry <T> > ());
}

template	<class T>	PriorityQueueLinkList<PQEntry <T> >::PriorityQueue_LinkList (Priority (*f) (T))
{
	SetRep (new PriorityQueueRep_LinkList<PQEntry <T> > (f));
}

template	<class T>	PriorityQueueLinkList<PQEntry <T> >::PriorityQueue_LinkList (const T* items, size_t size)
{
	SetRep (new PriorityQueueRep_LinkList<PQEntry <T> > ());
	AddItems (items, size);
}

template	<class T>	PriorityQueueLinkList<PQEntry <T> >::PriorityQueue_LinkList (const PriorityQueue<T>& q)
{
	SetRep (new PriorityQueueRep_LinkList<PQEntry <T> > ());
	operator+= (q);
}

template	<class T>	PriorityQueueLinkList<PQEntry <T> >::PriorityQueue_LinkList (const PriorityQueueLinkList<PQEntry <T> >& q) :
	PriorityQueue<T> (q)
{
}


#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

