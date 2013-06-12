/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__CircularArray__cc
#define	__CircularArray__cc

/*
 * $Header: /fuji/lewis/RCS/CircularArray.cc,v 1.18 1992/12/09 21:00:17 lewis Exp $
 *
 * TODO:
 *		+	Alot of code in here could be rewritten using ptr arithmatic for speed.
 *		+	GET RID OF USE OF MEMCPY, AND MEMCMP!!!!!!ETC GET RID OF REALLOC!!!
 *
 *
 * Changes:
 *	$Log: CircularArray.cc,v $
 *		Revision 1.18  1992/12/09  21:00:17  lewis
 *		Temporarily include <string.h> for memcpy, etc .. SOON GET RID OF MEMCPY and
 *		REALLOC!!!
 *
 *		Revision 1.17  1992/12/03  07:09:16  lewis
 *		Moved SetSlotsAlloced() call in op=.
 *		Rename qGCC_WarnPointerArithmaticOverzealousOnDeleteVoidStartBug->
 *		qGCC_DeleteOfVoidStarProducesWarningBug.
 *
 *		Revision 1.16  1992/11/29  02:44:26  lewis
 *		Array[NON-CONST] illegal type in a sizeof () - CFront and g++ accepted
 *		it but BCC didn't and ARM disallows.
 *
 *		Revision 1.15  1992/11/20  19:22:55  lewis
 *		Fixed bug in normalize - copyed last bytes instead of
 *		sizeof(CircularArrayNode<T>[last+1])
 *
 *		Revision 1.14  1992/11/19  05:06:33  lewis
 *		Totally rewritten. Was buggy - constructor calls done at wrong time,
 *		read/wrote off end of array - very messy. Now MUCH faster, and hopefully
 *		fully correct.
 *
 *		Revision 1.13  1992/11/15  16:07:02  lewis
 *		Added template      <class  T>      class   CircularArrayNode since we
 *		now call DTOR without explicit <T> at the end.
 *
 *		Revision 1.12  1992/11/13  03:40:21  lewis
 *		(T) -> <T> in a few places.
 *		~CircularArrayNode<T> -> ~CircularArrayNode
 *
 *		Revision 1.10  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.9  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.8  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.7  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.6  1992/10/07  23:10:23  sterling
 *		Fixed typos in template implemetnation.
 *
 *		Revision 1.5  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.4  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.3  1992/09/11  16:44:37  sterling
 *		fixed bug in SetLength
 *
 *		Revision 1.2  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		
 *
 */
#include	<string.h>		// for memcpy - GET RID OF USE OF MEMCPY, AND MEMCMP!!!!!!ETC

#include	"Debug.hh"
#include	"Math.hh"
#include	"Memory.hh"

#include	"CircularArray.hh"





/*
 ********************************************************************************
 ********************************* CircularArray ********************************
 ********************************************************************************
 */


#if		qRealTemplatesAvailable

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	CircularArray;		// tmp hack so GenClass will fixup below array CTOR
													// harmless, but silly
template	<class	T>	class	CircularArrayNode;	// tmp hack so GenClass will fixup below array CTOR
													// harmless, but silly

#endif

// Bugs with GenClass force us to place includes here too!!!
#include	"string.h"	// Use "" instead <> since GenClass mangles brackes!! UGH!
#include	"Math.hh"
#include	"Memory.hh"

template	<class	T>	CircularArray<T>::CircularArray (const CircularArray<T>& from) :
	fSlotsAllocated (0),
	fFirst (0),
	fLength (0),
	fItems (Nil)
{
	Invariant ();

	SetSlotsAlloced (from.GetSlotsAlloced ());

	/*
	 *	Construct the new items in-place into the new memory.
	 */
	register	size_t	newLength		=	from.GetLength ();
	if (newLength > 0) {
		register			CircularArrayNode<T>*	lhs	=	&fItems[0];
		register	const	CircularArrayNode<T>*	rhs	=	&from.fItems[0];
		register			CircularArrayNode<T>*	end	=	&fItems[newLength];
		do {
			#if		qGCC_OperatorNewAndStarPlusPlusBug
				new (lhs) CircularArrayNode<T> (*rhs);
				rhs++;
			#else
				new (lhs) CircularArrayNode<T> (*rhs++);
			#endif
		} while (++lhs < end);
		fLength = newLength;
	}
	Invariant ();
}

template	<class	T>	CircularArray<T>& CircularArray<T>::operator= (const CircularArray<T>& list)
{
	Invariant ();
	RemoveAll ();

	Assert ((fFirst == 0) and (fLength == 0));		// removeall normalizes...

	/*
	 *		SetSlotsAlloced to be big enough to hold the new items, and
	 *	construct them in-place into the new memory.
	 */
	register	size_t	newLength		=	list.GetLength ();
	if (newLength > 0) {
		/*
		 *		In case user already set this, we should not unset,
		 *	but must be sure we are big enuf. Do this before we store any pointers
		 *	cuz it could invalidate them.
		 */
		SetSlotsAlloced (Max (GetSlotsAlloced (), newLength));

		register			CircularArrayNode<T>*	lhs	=	&fItems[0];
		register	const	CircularArrayNode<T>*	rhs	=	&list.fItems[0];
		register			CircularArrayNode<T>*	end	=	&fItems[newLength];
		do {
			#if		qGCC_OperatorNewAndStarPlusPlusBug
				new (lhs) CircularArrayNode<T> (*rhs);
				rhs++;
			#else
				new (lhs) CircularArrayNode<T> (*rhs++);
			#endif
		} while (++lhs < end);
		fLength = newLength;
	}
	Invariant ();
	return (*this);	
}

template	<class	T>	void	CircularArray<T>::SetLength (size_t newLength, T fillValue)
{
	/*
	 * TODO:	This should all be rewritten using ptr arithmatic for speed!
	 */

	Invariant ();

	if (newLength > fSlotsAllocated) {
		/*
		 *		See equivilent code in Array<T>::SetLength() for justification.
		 */
		SetSlotsAlloced (Max (newLength+(64/sizeof (T)), size_t (newLength*1.1)));
	}

	size_t length		=	GetLength ();
	/*
	 *		last is the index of new/current last item. Since last may not
	 *	exist yet, we set it to a bogus value in this case - the end
	 *	of the array, so that the code at the beginning of the loop
	 *	will advance it to last=0, which is what we want. This probably
	 *	should be rewritten more clearly!!! (NB: this is only relevant
	 *	for the case of adding things - if we are removing, GetLast ()
	 *	returns a good value).
	 *
	 */
	size_t last			=	(length==0)? (fSlotsAllocated-1) : GetLast ();

	/*
	 *	Construct new items, as needed. Wrap last around to beginning, if needed.
	 */
	while (newLength > length) {
		Assert (last < fSlotsAllocated);
		if (++last == fSlotsAllocated) {
			Assert ((length==0) or (fFirst >= 1));		// cuz otherwise last would not have wrapped!
			last = 0;
		}
		new (&fItems[last]) CircularArrayNode<T> (fillValue);
		length++;
	}
	/*
	 *	Remove old items, as needed. Wrap last around to end, if needed.
	 */
	while (newLength < length) {
		fItems[last].CircularArrayNode<T>::~CircularArrayNode ();
		length--;
		if (last-- == 0) {
			Assert ((fFirst > 0) or (newLength == 0));		//??? Sterls assert???
															// not totally sure why true???
			last = fSlotsAllocated-1;
		}
	}
	Assert (newLength == length);

	fLength = newLength;

	/*
	 * Take this opportunity to do a cheap normalize if length=0.
	 */
	if (newLength == 0) {
		fFirst = 0;
	}
	
	Ensure (GetLength () == newLength);
	Invariant ();
}

template	<class	T>	void	CircularArray<T>::InsertAt (T item, size_t index)
{
	Require (index >= 1);
	Require (index <= GetLength ()+1);
	Invariant ();
#if		qDebug
	size_t	oldLen	=	GetLength ();
#endif

	/*
	 *		If there is room in the array, and we are prepending, then
	 *	do a cheap insert. Otherwise, if we are inserting elsewhere, 
	 *	use SetLength () to construct one new item, and do any shuffling
	 *	down necessary ( We probably could do somewhat better in the
	 *	common case where newLength <= fSlotsAllocated and index = newLength!!!
	 */
	if ((GetLength () < fSlotsAllocated) and (index == 1)) {
		/*
		 * Backup and wrap if needed.
		 */
		if (fFirst == 0) {
			fFirst = fSlotsAllocated-1;
		}
		else {
			fFirst--;
		}
		/*
		 *	Copy-construct right into place in the array.
		 */
		new (&fItems[fFirst]) CircularArrayNode<T> (item);
		fLength++;
	}
	else {
		SetLength (GetLength ()+1, item);
		for (size_t i = GetLength (); i > index; i--) {
			SetAt (GetAt (i-1), i);
		}
		SetAt (item, index);
	}

	Ensure (GetLength () == oldLen+1);
	Invariant ();
}

template	<class	T>	void	CircularArray<T>::RemoveAt (size_t index)
{
	Require (index >= 1);
	Require (index <= GetLength ());
	Invariant ();
#if		qDebug
	size_t	oldLen	=	GetLength ();
#endif

	if (index == 1) {
		fItems[fFirst].CircularArrayNode<T>::~CircularArrayNode ();
		if (++fFirst == fSlotsAllocated) {
			fFirst = 0;
		}
		Assert (fFirst < fSlotsAllocated);
	}
	else {
		/*
		 * Bump everybody down one, and the destroy the last item.
		 */
		Assert (GetLength () >= 1);
		size_t newLength = GetLength () - 1;
		if (index <= newLength) {
			for (size_t i = index; i <= newLength; i++) {
				SetAt (GetAt (i+1), i);
			}
		}
		fItems[GetLast ()].CircularArrayNode<T>::~CircularArrayNode ();
	}
	fLength--;

	/*
	 * Cheap normalization opportunity.
	 */
	if (fLength == 0) {
		fFirst = 0;
	}

	Ensure (GetLength () == oldLen-1);
	Invariant ();
}

template	<class	T>	void	CircularArray<T>::RemoveAll ()
{
	Invariant ();

	/*
	 *		Check to see if GetLength () != 0 first since otherwise we
	 *	cannot call GetLast ()!!!
	 */
	if (GetLength () != 0) {
		size_t	last	=	GetLast ();
	
		/*
		 *		There are three regions defined by two guards - first and last. Depending
		 *	on whether or not weve wrapped, we must destroy things in the two outer, or one
		 *	inner region.
		 */
		if (fFirst <= last) {
			/*
			 * Easy case.
			 */
			CircularArrayNode<T>* src	= &fItems[fFirst];
			CircularArrayNode<T>* end	= &fItems[last];
			for (CircularArrayNode<T>* p = src; p <= end; p++) {
				p->CircularArrayNode<T>::~CircularArrayNode ();
			}
		}
		else {
			/*
			 *		Delete the items in the initial segment up to last, and then the
			 *	items from first to the end of the array.
			 */
			CircularArrayNode<T>* src	= &fItems[0];
			CircularArrayNode<T>* end	= &fItems[last];
			for (CircularArrayNode<T>* p = src; p <= end; p++) {
				p->CircularArrayNode<T>::~CircularArrayNode ();
			}
			src	= &fItems[fFirst];
			end	= &fItems[fSlotsAllocated];
			for (p = src; p < end; p++) {
				p->CircularArrayNode<T>::~CircularArrayNode ();
			}
		}
		fFirst = 0;
		fLength = 0;
	}

	Invariant ();
	Ensure ((fFirst == 0) and (fLength == 0));		// removeall normalizes...
}

template	<class	T>	Boolean	CircularArray<T>::Contains (T item) const
{
	/*
	 * This can be done MUCH more efficiently.
	 */
	Invariant ();
    size_t length = GetLength ();
	for (size_t i = 1; i <= length; i++) {
		if (GetAt (i) == item) {
			return (True);
		}
	}
	return (False);
}

template	<class	T>	void	CircularArray<T>::SetSlotsAlloced (size_t slotsAlloced)
{
#if		qDebug
	size_t	oldLen	=	GetLength ();
#endif
	Require (GetLength () <= slotsAlloced);
	Invariant ();

	/*
	 *		Since our array indexes count from fFirst to fSlotsAllocated-1, adding
	 *	items after the old fSlotsAllocated would screw up our counting. So, if
	 *	the array wraps, we must normize it before we can adjust slotsAlloced.
	 */
	if ((GetLength () != 0) and (GetLast () < fFirst)) {
		Assert (GetLength () != 0);
		Normalize ();
		Assert (oldLen == GetLength ());
		Assert (fFirst == 0);
	}
	Assert (GetLength () == 0 or fFirst <= GetLast ());

	/*
	 * Now do regular SetSlotsAlloced() code copied from Array<T>.
	 */
	if (fSlotsAllocated != slotsAlloced) {
		if (slotsAlloced == 0) {
			delete (char*)fItems;
			fItems = Nil;
		}
		else {
			/*
			 *		We should consider getting rid of use of realloc since it
			 *	prohibits internal pointers. For example, we cannot have an array
			 *	of patchable_arrays.
			 */
			if (fItems == Nil) {
				fItems = (CircularArrayNode<T>*) new char [sizeof (CircularArrayNode<T>) * slotsAlloced];
			}
			else {
				fItems = (CircularArrayNode<T>*) ReAlloc (fItems, sizeof (CircularArrayNode<T>) * slotsAlloced);
			}
		}
		fSlotsAllocated = slotsAlloced;
	}
	Assert (oldLen == GetLength ());
	Assert (fSlotsAllocated == slotsAlloced);
	Invariant ();
}

template	<class	T>	void	CircularArray<T>::Normalize ()
{
	Invariant ();

	/*
	 * Check if already normalized.
	 */
	if (fFirst == 0) {
		return;
	}

	/*
	 * This case (first <= last):
	 * |   |   |   |   |
	 * |   | 1 | 2 | 3 |
	 * |   |   |   |   |
	 * 		is easy. We can just copy down. This is where fFirst is < last.
	 *	We know we have at least one spot at the beginning free, because
	 *	if we did not, and all spaces were full, since fFirst < last, we know
	 *	we are already normalized, and we are done (checked for above).
	 *
	 * Next case (first > last):
	 *
	 * |   |   |   |   |
	 * | 4 | 1 | 2 | 3 |
	 * |   |   |   |   |
	 *	is more tricky. We want to move 1..3 down but the space is blocked.
	 *	So we copy away the blocked contents into savedBuf, and then copy
	 *	as above. Then, we blast the saved buf onto the end of what we just
	 *	copied.
	 *
	 *		NB: All copies done using memcpy/memmove() so there must be no internal
	 *	pointers!!!
	 *
	 */
	Assert (fLength != 0);				// since otherwise fFirst would == 0


	/*
	 *		If GetLast() > fFirst, things are quite easy. Just copy back to the first
	 *	position, incremting both pointers as you go.
	 */
	size_t length	=	GetLength ();
	size_t last		=	GetLast ();


	/*
	 * Save block from 0..last in front of the array.
	 */
	void*	savedBuf	=	Nil;
	if (fFirst > last) {
		savedBuf = new char [sizeof(CircularArrayNode<T>)*(last+1)];
		memcpy (savedBuf, &fItems[0], sizeof(CircularArrayNode<T>)*(last+1));
	}

	/*
	 *		Move the entries from fFirst..slotsalloced-1 or last back to the begingging,
	 *	now that the space has been cleared (or already was clear). We use memmove() here
	 *	because the regions could overlap.
	 */
	size_t	endItemsToCopy = Min (fSlotsAllocated-fFirst, fFirst+fLength);
	memmove (&fItems[0], &fItems[fFirst], sizeof(CircularArrayNode<T>)*endItemsToCopy);

	Assert ((savedBuf == Nil) == (fFirst <= last));
	if (fFirst > last) {
		AssertNotNil (savedBuf);
		/*
		 *		Now copy the contents of savedBuf to the end of where we stopped copying
		 *	in our last memmove().
		 */
		memcpy (&fItems[endItemsToCopy], savedBuf, sizeof(CircularArrayNode<T>)*(last+1));
#if		qGCC_DeleteOfVoidStarProducesWarningBug
		delete (char*)(savedBuf);
#else
		delete (savedBuf);
#endif
	}
	fFirst = 0;
	Invariant ();
}

#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
