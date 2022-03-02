/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__DoubleLinkList__cc
#define	__DoubleLinkList__cc

/*
 * $Header: /fuji/lewis/RCS/DoubleLinkList.cc,v 1.22 1992/12/10 07:13:04 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: DoubleLinkList.cc,v $
 *		Revision 1.22  1992/12/10  07:13:04  lewis
 *		Totally rewrote based on LinkList and Array code. Got working with
 *		Deque and Queue, but still some problems to be debugged with Sequence_DoubleLinkList.
 *
 *		Revision 1.21  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.20  1992/11/03  22:52:41  lewis
 *		Switch over to new DoubleLinkList and DoubleLinkList_Patch_Forward, etc strategy
 *		for backends.
 *		Debugged (hopefully - not tested yet) iterator patching - based on what
 *		we did in LinkList that did pass testsuites.
 *
 *		Revision 1.19  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.18  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.17  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.16  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.15  1992/10/07  23:13:37  sterling
 *		Lots of changes - mostly it appears with iterator patching (LGP checkedin).
 *
 *		Revision 1.14  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.13  1992/09/25  21:14:26  lewis
 *		Fix lots of templates problems and get working using new GenClass stuff.
 *
 *		Revision 1.12  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.11  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.10  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.9  1992/09/04  21:03:13  sterling
 *		Added implementation of Contains ().
 *
 *		Revision 1.7  1992/07/21  06:04:58  lewis
 *		New freepool stragegy for macro version since BlockAllocated stuff to hard
 *		to use from within a template.
 *
 *		Revision 1.6  1992/07/19  05:39:20  lewis
 *		Added optimized version of IndexToLink (warmed over version of routine
 *		which used to be private method in Sequence_DoubleLinkList - made
 *		no reference to T so we could move it here).
 *
 *		Revision 1.4  1992/07/17  22:06:08  lewis
 *		Rewrote futher. Got rid of use of circular linked list. Added new class
 *		DblLinkList that does all the work, and now DoubleLinkList<T> is NOTHING
 *		but trivial inlines wrappers on base class, so takes up no code (in fact
 *		wrappers really do nothing but casts), with one exception - the DeleteItem()
 *		method. Everything carefully tuned - should be fast and sleek.
 *		Only remaining problem is with block allocation of DoubleLink<T> items
 *		with macros - useing BlockAllocated<T> didn't work well within the
 *		macros - I danced around it for a while, but will probably give up and
 *		just use inline old implemnation, rather than sharing the BlockAllocated<T>
 *		implementation. Sigh.
 *
 *		Revision 1.3  1992/07/17  05:46:29  lewis
 *		Lots of changes - synronized both templated and macro versions of
 *		the class - seperated out class declarations and implementations -
 *		made much more inline. Added DblLink base class to template to
 *		allow sharing of implementation - will do same for DoubleLinkList<T>
 *		too soon. Still some work to go, but this public interface has
 *		not changed and need not immediatly, and the other changes I have in
 *		mind (save getting rid of circularity) wont affect the public interface.
 *
 *		Revision 1.2  1992/07/02  03:23:04  lewis
 *		Renamed DoublyLinkedList->DoubleLinkList.
 *
 *		Revision 1.2  1992/05/09  01:36:04  lewis
 *		Ported to BC++ and fixed templates stuff.
 *
 *
 */



#include	"Debug.hh"
#include	"Memory.hh"

#include	"DoubleLinkList.hh"



#if		qRealTemplatesAvailable


/*
 ********************************************************************************
 ************************************* DoubleLink *******************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfDoubleLinkOf<T>_cc
#endif





/*
 ********************************************************************************
 ********************************* DoubleLinkList *******************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	DoubleLinkList;	// tmp hack so GenClass will fixup below DoubleLinkList CTOR
												// harmless, but silly

#endif

template	<class	T>	DoubleLinkList<T>::DoubleLinkList (const DoubleLinkList<T>& from) :
	fLength (from.fLength),
	fFirst (Nil)
{
	for (const DoubleLink<T>* current = from.fFirst; current != Nil; current = current->fNext) {
		Append (current->fItem);
	}

	Invariant ();
}
template	<class	T>	DoubleLinkList<T>& DoubleLinkList<T>::operator= (const DoubleLinkList<T>& list)
{
	Invariant ();

	RemoveAll ();

	for (const DoubleLink<T>* current = list.fFirst; current != Nil; current = current->fNext) {
		Append (current->fItem);
	}

	Invariant ();

	return (*this);	
}

template	<class	T>	void	DoubleLinkList<T>::Remove (T item)
{
	Invariant ();

	if (fLength != 0) {
		if (item == fFirst->fItem) {
			RemoveFirst ();
		}
		else if (item == fLast->fItem) {
			RemoveLast ();
		}
		else {
			// We could test for link!=fLast, and skip last test, but probably faster
			// to have loop do one useless test in a rare case (item not found) and to
			// test against Nil which is faster test than against field...
			for (register DoubleLink<T>* link = fFirst; link != Nil; link = link->fNext) {
				if (link->fItem == item) {
					Assert (fLength >= 3);		// cuz item != first, or last, but is equal to THIS ONE

					/*
					 * Before (A/C NON-NIL):
					 *	|        |   	|        |   	|        |
					 *	|    A   |   	|    V   |   	|   C    |
					 *	| <-prev |   	| <-prev |   	| <-prev | ...
					 *	| next-> |   	| next-> |   	| next-> |
					 *	|        |   	|        |   	|        |
					 * 
					 * After:
					 *	|        |   	|        |
					 *	|   B    |   	|    C   |
					 *	| <-prev |   	| <-prev | ...
					 *	| next-> |   	| next-> |
					 *	|        |   	|        |
					 */
					register	DoubleLink<T>* victim	=	link;
					AssertNotNil (victim);					// cuz we're in this loop
					AssertNotNil (victim->fPrev);			// cuz we didn't do RemoveFirst()
					AssertNotNil (victim->fNext);			// cuz we didn't do RemoveLast()
					victim->fPrev->fNext = victim->fNext;	// make 'A' point to 'C'
					victim->fNext->fPrev = victim->fPrev;	// make 'C' point to 'A'
					delete (victim);
					fLength--;
					break;
				}
			}
		}
	}

	Invariant ();
}

template	<class	T>	Boolean	DoubleLinkList<T>::Contains (T item) const
{
	for (register const DoubleLink<T>* current = fFirst; current != Nil; current = current->fNext) {
		if (current->fItem == item) {
			return (True);
		}
	}
	return (False);
}

template	<class	T>	void	DoubleLinkList<T>::RemoveAll ()
{
	for (register DoubleLink<T>* i = fFirst; i != Nil;) {
		DoubleLink<T>*	deleteMe	=	i;
		i = i->fNext;
		delete (deleteMe);
	}
	fFirst = Nil;
	fLast = Nil;
	fLength = 0;
}

template	<class	T>	T	DoubleLinkList<T>::GetAt (size_t i) const
{
	Require (i >= 1);
	Require (i <= fLength);

	/*
	 *		See if faster to search forwards or backwards, and then do so. Check by
	 * seeing if we are closer to the beginning or the end. Also, note in each
	 * loop that we setup our loop to just be a decrement and compare on
	 * zero, which tends to be fast.
	 */
	if (i < fLength/2) {
		register	size_t	idx	=	i-1;
		for (register DoubleLink<T>* link = fFirst; idx != 0; idx--, link = link->fNext)
			;
		AssertNotNil (link);
		return (link->fItem);
	}
	else {
		register	size_t	idx	=	fLength - i;
		for (register DoubleLink<T>* link = fLast; idx != 0; idx--, link = link->fPrev)
			;
		AssertNotNil (link);
		return (link->fItem);
	}
	AssertNotReached (); return (fFirst->fItem);
}

template	<class	T>	void	DoubleLinkList<T>::SetAt (T item, size_t i)
{
	Require (i >= 1);
	Require (i <= fLength);

	/*
	 *		See if faster to search forwards or backwards, and then do so. Check by
	 * seeing if we are closer to the beginning or the end. Also, note in each
	 * loop that we setup our loop to just be a decrement and compare on
	 * zero, which tends to be fast.
	 */
	if (i < fLength/2) {
		register	size_t	idx	=	i-1;
		for (register DoubleLink<T>* link = fFirst; idx != 0; idx--, link = link->fNext)
			;
		AssertNotNil (link);
		link->fItem = item;
	}
	else {
		register	size_t	idx	=	fLength - i;
		for (register DoubleLink<T>* link = fLast; idx != 0; idx--, link = link->fPrev)
			;
		AssertNotNil (link);
		link->fItem = item;
	}
}

#if		qDebug
template	<class	T>	void	DoubleLinkList<T>::Invariant_ () const
{
	if (fFirst != Nil) {
		Assert (fFirst->fPrev == Nil);
		if (fFirst->fNext == Nil) {
			Assert (fLength == 1);
			Assert (fFirst == fLast);
		}
		else {
			Assert (fLength >= 2);
			Assert (fFirst->fNext->fPrev == fFirst);
		}
	}
	if (fLast != Nil) {
		Assert (fLast->fNext == Nil);
		if (fLast->fPrev == Nil) {
			Assert (fLength == 1);
			Assert (fFirst == fLast);
		}
		else {
			Assert (fLength >= 2);
			Assert (fLast->fPrev->fNext == fLast);
		}
	}
	Assert (fFirst == Nil or fFirst->fPrev == Nil);
	Assert (fLast == Nil or fLast->fNext == Nil);
	Assert (fFirst != Nil or fLength == 0);
	Assert (fLast != Nil or fLength == 0);

	/*
	 * Check we are properly linked together.
	 */
	size_t	counter	=	0;
	for (DoubleLink<T>* i = fFirst; i != Nil; i = i->fNext) {
		Assert (i->fNext == Nil or i->fNext->fPrev == i);		//	adjacent nodes point at each other
		counter ++;
	}
	Assert (counter == fLength);
	counter	=	0;
	for (i = fLast; i != Nil; i = i->fPrev) {
		Assert (i->fPrev == Nil or i->fPrev->fNext == i);		//	adjacent nodes point at each other
		counter ++;
	}
	Assert (counter == fLength);
}
#endif






/*
 ********************************************************************************
 ******************************** DoubleLinkListIteratorBase ********************
 ********************************************************************************
 */
#if		qDebug
template	<class T>	void	DoubleLinkListIteratorBase<T>::Invariant_ () const
{
}
#endif




/*
 ********************************************************************************
 ****************************** DoubleLinkList_Patch ****************************
 ********************************************************************************
 */
template	<class	T>	void	DoubleLinkList_Patch<T>::Remove (T item)
{
	// Copied from DoubleLinkList<T>::Remove (T item) with patch calls inserted...
	Invariant ();

	if (fLength != 0) {
		if (item == fFirst->fItem) {
			RemoveFirst ();			// patchremove called in here...
		}
		else if (item == fLast->fItem) {
			RemoveLast ();			// patchremove called in here...
		}
		else {
			// We could test for link!=fLast, and skip last test, but probably faster
			// to have loop do one useless test in a rare case (item not found) and to
			// test against Nil which is faster test than against field...
			size_t	index	=	1;	// for the patch call
			for (register DoubleLink<T>* link = fFirst; link != Nil; link = link->fNext) {
				if (link->fItem == item) {
					Assert (fLength >= 3);		// cuz item != first, or last, but is equal to THIS ONE

					PatchViewsRemove (index);

					/*
					 * Before (A/C NON-NIL):
					 *	|        |   	|        |   	|        |
					 *	|    A   |   	|    V   |   	|   C    |
					 *	| <-prev |   	| <-prev |   	| <-prev | ...
					 *	| next-> |   	| next-> |   	| next-> |
					 *	|        |   	|        |   	|        |
					 * 
					 * After:
					 *	|        |   	|        |
					 *	|   B    |   	|    C   |
					 *	| <-prev |   	| <-prev | ...
					 *	| next-> |   	| next-> |
					 *	|        |   	|        |
					 */
					register	DoubleLink<T>* victim	=	link;
					AssertNotNil (victim);					// cuz we're in this loop
					AssertNotNil (victim->fPrev);			// cuz we didn't do RemoveFirst()
					AssertNotNil (victim->fNext);			// cuz we didn't do RemoveLast()
					victim->fPrev->fNext = victim->fNext;	// make 'A' point to 'C'
					victim->fNext->fPrev = victim->fPrev;	// make 'C' point to 'A'
					delete (victim);
					fLength--;
					break;
				}
			}
		}
	}

	Invariant ();
}

#if		qDebug
template	<class	T>	void	DoubleLinkList_Patch<T>::Invariant_ () const
{
	DoubleLinkList<T>::Invariant_ ();

	/*
	 *		Be sure each iterator points back to us. Thats about all we can test from
	 *	here since we cannot call each iterators Invariant(). That would be
	 *	nice, but sadly when this Invariant_ () is called from DoubleLinkList<T> the
	 *	iterators themselves may not have been patched, so they'll be out of
	 *	date. Instead, so that in local shadow of Invariant() done in DoubleLinkList_Patch<T>
	 *	so only called when WE call Invariant().
	 */
	for (DoubleLinkListIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
		Assert (v->fData == this);
	}
}

template	<class	T>	void	DoubleLinkList_Patch<T>::InvariantOnIterators_ () const
{
	/*
	 *		Only here can we iterate over each iterator and calls its Invariant()
	 *	since now we've completed any needed patching.
	 */
	for (DoubleLinkListIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
		Assert (v->fData == this);
		v->Invariant ();
	}
}
#endif	/*qDebug*/







/*
 ********************************************************************************
 ******************************* DoubleLinkListIterator_PatchBase ***************
 ********************************************************************************
 */
#if		qDebug
template	<class T>	void	DoubleLinkListIterator_PatchBase<T>::Invariant_ () const
{
	DoubleLinkListIteratorBase<T>::Invariant_ ();
	Assert (fData == DoubleLinkListIteratorBase<T>::fData);
}
#endif	/*qDebug*/





#endif	/*qRealTemplatesAvailable*/



#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

