/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__LinkList__cc
#define	__LinkList__cc

/*
 * $Header: /fuji/lewis/RCS/LinkList.cc,v 1.17 1992/12/03 07:12:15 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: LinkList.cc,v $
 *		Revision 1.17  1992/12/03  07:12:15  lewis
 *		Almost totally rewritten by kdj and lewis. Main points where
 *		getting rid of base class LnkList - no inheriance and virtual
 *		DELETEME method anymore - not sure was a good idea but kdj
 *		thought so.
 *		No longer advertise Link* in interface - totally hidden. Add
 *		mutator support, and addbefore/after (needed to be able to append
 *		since no more direct link access).
 *		Lots of invariant support. Support for new iterator update
 *		/ patching defintions.
 *		Added SetAt/GetAt.
 *		Kdj had tried to do real FM with fPrev in mutator being Link**, but
 *		I didn't quite get it - not too sure he did. If its really a good
 *		idea, then try again later. Similarly with constant time remove
 *		technique for iterators from link list of iterators maintained by
 *		LinkList_Patch.
 *
 *		Revision 1.16  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.15  1992/11/03  22:51:57  lewis
 *		ItemToIndex takes const ptr rather than ptr.
 *
 *		Revision 1.13  1992/11/02  19:48:55  lewis
 *		Added LinkedList::ItemToIndex. Make Invariant only defined #if qDebug.
 *		LinkedList::LinkedList () now inline. Use BlockAllocator for Link nodes.
 *		Delete LinkList iterator - really redone/moved to header.
 *
 *		Revision 1.12  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.11  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.10  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.9  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.8  1992/10/07  23:18:48  sterling
 *		Lots of changes - it looks like fixes to the patching stuff. LGP checkedin.
 *
 *		Revision 1.7  1992/09/25  21:14:26  lewis
 *		Fix lots of templates problems and get working using new GenClass stuff.
 *
 *		Revision 1.6  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.5  1992/09/21  06:07:40  sterling
 *		AddAfter and RemoveItem allowed now to take Nil after/only
 *		parameter (used to be assert).
 *
 *		Revision 1.4  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.3  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/04  21:03:57  sterling
 *		Add implementation of Contains.
 *
 *		
 *
 */



#include	"Debug.hh"
#include	"Memory.hh"

#include	"LinkList.hh"





#if		qRealTemplatesAvailable


/*
 ********************************************************************************
 ************************************* Link *************************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfLinkOf<T>_cc
#endif





/*
 ********************************************************************************
 *************************************** LinkList *******************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	LinkList;	// tmp hack so GenClass will fixup below array CTOR
											// harmless, but silly

#endif

template	<class	T>	LinkList<T>::LinkList (const LinkList<T>& from) :
	fLength (from.fLength),
	fFirst (Nil)
{
	/*
	 *		Copy the link list by keeping a point to the new current and new
	 *	previous, and sliding them along in parallel as we construct the
	 *	new list. Only do this if we have at least one element - then we
	 *	don't have to worry about the head of the list, or Nil ptrs, etc - that
	 *	case is handled outside, before the loop.
	 */
	if (from.fFirst != Nil) {
		fFirst = new Link<T> (from.fFirst->fItem, Nil);
		register	Link<T>*	newCur	=	fFirst;
		for (register const Link<T>* cur = from.fFirst->fNext; cur != Nil; cur = cur->fNext) {
			register	Link<T>*	newPrev	=	newCur;
			newCur = new Link<T> (cur->fItem, Nil);
			newPrev->fNext = newCur;
		}
	}

	Invariant ();
}
template	<class	T>	LinkList<T>& LinkList<T>::operator= (const LinkList<T>& list)
{
	Invariant ();

	RemoveAll ();

	/*
	 *		Copy the link list by keeping a point to the new current and new
	 *	previous, and sliding them along in parallel as we construct the
	 *	new list. Only do this if we have at least one element - then we
	 *	don't have to worry about the head of the list, or Nil ptrs, etc - that
	 *	case is handled outside, before the loop.
	 */
	if (list.fFirst != Nil) {
		fFirst = new Link<T> (list.fFirst->fItem, Nil);
		register	Link<T>*	newCur	=	fFirst;
		for (register const Link<T>* cur = list.fFirst->fNext; cur != Nil; cur = cur->fNext) {
			register	Link<T>*	newPrev	=	newCur;
			newCur = new Link<T> (cur->fItem, Nil);
			newPrev->fNext = newCur;
		}
	}

	fLength = list.fLength;

	Invariant ();

	return (*this);	
}

template	<class	T>	void	LinkList<T>::Remove (T item)
{
	Require (fLength >= 1);

	Invariant ();

	if (item == fFirst->fItem) {
		RemoveFirst ();
	}
	else {
		register Link<T>*	prev	=	Nil;
		for (register Link<T>* link = fFirst; link != Nil; prev = link, link = link->fNext) {
			if (link->fItem == item) {
				AssertNotNil (prev);		// cuz otherwise we would have hit it in first case!
				prev->fNext = link->fNext;
				delete (link);
				fLength--;
				break;
			}
		}
	}

	Invariant ();
}

template	<class	T>	Boolean	LinkList<T>::Contains (T item) const
{
	for (register const Link<T>* current = fFirst; current != Nil; current = current->fNext) {
		if (current->fItem == item) {
			return (True);
		}
	}
	return (False);
}

template	<class	T>	void	LinkList<T>::RemoveAll ()
{
	for (register Link<T>* i = fFirst; i != Nil;) {
		Link<T>*	deleteMe	=	i;
		i = i->fNext;
		delete (deleteMe);
	}
	fFirst = Nil;
	fLength = 0;
}

template	<class	T>	T	LinkList<T>::GetAt (size_t i) const
{
	Require (i >= 1);
	Require (i <= fLength);
	for (register const Link<T>* cur = fFirst; --i != 0; cur = cur->fNext) {
		AssertNotNil (cur);	// cuz i <= fLength
	}
	AssertNotNil (cur);		// cuz i <= fLength
	return (cur->fItem);
}

template	<class	T>	void	LinkList<T>::SetAt (T item, size_t i)
{
	Require (i >= 1);
	Require (i <= fLength);
	for (register Link<T>* cur = fFirst; --i != 0; cur = cur->fNext) {
		AssertNotNil (cur);	// cuz i <= fLength
	}
	AssertNotNil (cur);		// cuz i <= fLength
	cur->fItem = item;
}

#if		qDebug
template	<class	T>	void	LinkList<T>::Invariant_ () const
{
	/*
	 * Check we are properly linked together.
	 */
	size_t	counter	=	0;
	for (Link<T>* i = fFirst; i != Nil; i = i->fNext) {
		counter++;
		Assert (counter <= fLength);	// to this test in the loop so we detect circularities...
	}
	Assert (counter == fLength);
}
#endif






/*
 ********************************************************************************
 ********************************* LinkListIterator *****************************
 ********************************************************************************
 */
#if		qDebug
template	<class	T>	void	LinkListIterator<T>::Invariant_ () const
{
}
#endif





/*
 ********************************************************************************
 ************************************* LinkList_Patch ***************************
 ********************************************************************************
 */
template	<class	T>	void	LinkList_Patch<T>::Remove (T item)
{
	Require (fLength >= 1);

	Invariant ();

	for (LinkListMutator_Patch<T> it (*this); it.More (); ) {
		if (it.Current () == item) {
			it.RemoveCurrent ();
			break;
		}
	}

	Invariant ();
}


#if		qDebug
template	<class	T>	void	LinkList_Patch<T>::Invariant_ () const
{
	LinkList<T>::Invariant_ ();
	/*
	 *		Be sure each iterator points back to us. Thats about all we can test from
	 *	here since we cannot call each iterators Invariant(). That would be
	 *	nice, but sadly when this Invariant_ () is called from LinkList<T> the
	 *	iterators themselves may not have been patched, so they'll be out of
	 *	date. Instead, so that in local shadow of Invariant() done in LinkList_Patch<T>
	 *	so only called when WE call Invariant().
	 */
	for (LinkListIterator_Patch<T>* v = fIterators; v != Nil; v = v->fNext) {
		Assert (v->fData == this);
	}
}

template	<class	T>	void	LinkList_Patch<T>::InvariantOnIterators_ () const
{
	/*
	 *		Only here can we iterate over each iterator and calls its Invariant()
	 *	since now we've completed any needed patching.
	 */
	for (LinkListIterator_Patch<T>* v = fIterators; v != Nil; v = v->fNext) {
		Assert (v->fData == this);
		v->Invariant ();
	}
}
#endif



/*
 ********************************************************************************
 ******************************** LinkListIterator_Patch ************************
 ********************************************************************************
 */
#if		qDebug
template	<class	T>	void	LinkListIterator_Patch<T>::Invariant_ () const
{
	LinkListIterator<T>::Invariant_ ();

	/*
	 *	fPrev could be Nil, but if it isn't then its next must be fCurrent.
	 */
	Assert ((fPrev == Nil) or (fPrev->fNext == fCurrent));
}
#endif



#endif	/*qRealTemplatesAvailable*/



#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

