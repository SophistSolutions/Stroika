/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Array__cc
#define	__Array__cc

/*
 * $Header: /fuji/lewis/RCS/Array.cc,v 1.31 1992/12/04 16:07:12 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Array.cc,v $
 *		Revision 1.31  1992/12/04  16:07:12  lewis
 *		Work around qCFront_SorryNotImplementedStatementAfterReturnInline.
 *
 *		Revision 1.30  1992/12/03  05:44:02  lewis
 *		Added lots of Invariant() calls and implementations - especially one
 *		to iterate over iterators owned by an array_patch and call their invariants.
 *
 *		Revision 1.29  1992/11/19  05:01:46  lewis
 *		Cleanup comments, especially about call to SetSlotsAlloced() in SetLength().
 *		Change how we call it based on calculations in new comment.
 *
 *		Revision 1.28  1992/11/15  16:05:09  lewis
 *		Comment, plus use 64/sizeof(T) instead of 50/sizeof(T) as arithmentic
 *		bump in SetLength().
 *
 *		Revision 1.27  1992/11/13  03:37:38  lewis
 *		Added hack in #if 0 to bottom of file to aid in debugging template
 *		stuff.
 *
 *		Revision 1.26  1992/11/12  08:16:08  lewis
 *		Predeclare template names.
 *		Get rid of <T> at end of ~ArrayNode<T> in explicit calls.
 *		#include Math and Memory inside of #if qRealTempaltes... to work around
 *		buggy GenClass.
 *
 *		Revision 1.25  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.24  1992/11/04  19:41:08  lewis
 *		Make ptrs const in Contains().
 *
 *		Revision 1.23  1992/11/01  01:26:35  lewis
 *		New Array Iterator support with separate subclass for patching.
 *
 *		Revision 1.22  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.21  1992/10/29  16:36:06  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *		And, trashed some commented out code.
 *
 *		Revision 1.20  1992/10/19  01:39:22  lewis
 *		ArrayNode(T)->ArrayNode<T> - oops!
 *		Moved Array CTOR/DTOR to be inline, and rewrote op= and X(X&) to be
 *		(hopefully) smaller and faster - or at least faster.
 *
 *		Revision 1.19  1992/10/14  15:47:11  lewis
 *		Tweek RemoveAll.
 *
 *		Revision 1.18  1992/10/13  20:24:47  lewis
 *		Got rid of ugly g++ workarounds when slightly better ones found to
 *		work. Also, use SetSlotsAlloced(Max(..)) to combine size bumping
 *		strategies. Commenting.
 *
 *		Revision 1.17  1992/10/13  05:20:45  lewis
 *		Lots of changes. Added three new iterators - not based on subclassing
 *		from IteratorRep, and all inline. Also, made them ptr based - not
 *		indexing based. Changed op= to not call RemoveAll () unless necessary.
 *		Change SetLength() code to change setslotsalloced by scale of 1.1,
 *		instead of +32/sizeof(T). Use op new {} synax with g++ instead of old
 *		hack with new and memcpy. Try getting rid of lhs++ workaround in
 *		InserAt/RemoveAt - instead use new bug workaround for
 *		op= and X(X&) problem in ArrayNode for gcc.
 *
 *		Revision 1.16  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.15  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.14  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.13  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.12  1992/09/25  21:14:26  lewis
 *		Fix lots of templates problems and get working using new GenClass stuff.
 *
 *		Revision 1.11  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.10  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.8  1992/09/11  13:10:08  lewis
 *		Say delete (char*) fItems since we manually destruct things. Dont
 *		We'll re-invent it
 *		if we need it when I get to port again.
 *
 *		Revision 1.7  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/04  20:54:19  sterling
 *		Added Contains ().
 *
 *		Revision 1.5  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.4  1992/07/17  17:33:00  lewis
 *		Changed arg to InsertAt from const T& to T - just an oversight when
 *		we made this change to all the other container clases before.
 *		Made CTOR/DTOR and SetLength inline - this is important becasue
 *		of getting rid of Sequence_ArrayPtr - also they are VERY short, and
 *		there are no virtual methods.
 *		Rewrote SetSlotsAlloced to use size_t rather than int as index - so gauranteed sizes
 *		right (sizeof (int) not necessarily = sizeof (size_t)).
 *
 *		Revision 1.3  1992/07/17  04:05:40  lewis
 *		Minor cleanups, and re-enable line which caused bug (assert failure)
 *		in Array code. Hopefully works now?
 *
 *		Revision 1.2  1992/07/01  07:19:19  lewis
 *		Changed delete calls back to delete [] - I accidentally went too far when
 *		removing the old vfasioned delete [xx] a; That may have been what suddenly
 *		caused my array code to go to hell on the PC - innocuous(except dtors not called)
 *		under CFront - but could easily cause corruption calling wrong version!
 *
 *		Revision 1.3  1992/05/21  17:21:25  lewis
 *		Modify template implmentation to use size_t rather than UInt32.
 *		And, a few other minor cleanups of the template stuff (no explicity count
 *		for delete of arrays).
 *
 *		Revision 1.2  92/05/09  01:31:49  01:31:49  lewis (Lewis Pringle)
 *		Ported to BC++/PC/Templates. Had to work around BC++ bug by just commenting
 *		out assertion - investigate more fully later...
 *		
 *
 */


#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Math.hh"
#include	"Memory.hh"

#include	"Array.hh"










#if		qRealTemplatesAvailable

// Bugs with GenClass force us to place includes here too!!!
#include	"Math.hh"
#include	"Memory.hh"


/*
 ********************************************************************************
 ************************************* Array ************************************
 ********************************************************************************
 */


#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	ArrayNode;	// tmp hack so GenClass will fixup below array CTOR
											// harmless, but silly
template	<class	T>	class	Array;		// tmp hack so GenClass will fixup below array CTOR
											// harmless, but silly

#endif

template	<class	T>	Array<T>::Array (const Array<T>& from) :
	fLength (0),
	fSlotsAllocated (0),
	fItems (Nil)
{
	from.Invariant ();
	SetSlotsAlloced (from.GetSlotsAlloced ());

	/*
	 *	Construct the new items in-place into the new memory.
	 */
	register	size_t	newLength		=	from.GetLength ();
	if (newLength > 0) {
		register			ArrayNode<T>*	lhs	=	&fItems[0];
		register	const	ArrayNode<T>*	rhs	=	&from.fItems[0];
		register			ArrayNode<T>*	end	=	&fItems[newLength];
		do {
			#if		qGCC_OperatorNewAndStarPlusPlusBug
				new (lhs) ArrayNode<T> (*rhs);
				rhs++;
			#else
				new (lhs) ArrayNode<T> (*rhs++);
			#endif
		} while (++lhs < end);
	}
	fLength = newLength;
	Invariant ();
}

template	<class	T>	void	Array<T>::InsertAt (T item, size_t index)
{
	Require (index >= 1);
	Require (index <= fLength+1);
	Invariant ();

	/*
	 * Delicate matter so that we assure ctors/dtors/op= called at
	 * right time.
	 */
	SetLength (fLength + 1, item);			//	Add space for extra item

	if (index < fLength) {
		/*
		 * Slide items down, and add our new entry
		 */
		register	ArrayNode<T>*	lhs	=	&fItems [fLength-1];
		register	ArrayNode<T>*	rhs	=	&fItems [fLength-2];
		for (register size_t i = fLength-1; i >= index; i--) {
			*lhs-- = *rhs--;
		}
		Assert (i == index-1);
		Assert (lhs == &fItems [index-1]);
		lhs->fItem = item;
	}
	Invariant ();
}

template	<class	T>	void	Array<T>::RemoveAt (size_t index)
{
	Require (index >= 1);
	Require (index <= fLength);
	Invariant ();

	if (index < fLength) {
		/*
		 * Slide items down.
		 */
		register	ArrayNode<T>*	lhs	=	&fItems [index-1];
		register	ArrayNode<T>*	rhs	=	&fItems [index];
// We tried getting rid of index var and using ptr compare but
// did much worse on CFront/MPW Thursday, August 27, 1992 4:12:08 PM
		for (register size_t i = fLength - index; i > 0; i--) {
			*lhs++ = *rhs++;
		}
	}
	fItems [--fLength].ArrayNode<T>::~ArrayNode ();
	Invariant ();
}

template	<class	T>	void	Array<T>::RemoveAll ()
{
	Invariant ();
	register	ArrayNode<T>*	p	=	&fItems[0];
	for (register size_t i = fLength; i > 0; i--, p++) {
		p->ArrayNode<T>::~ArrayNode ();
	}
	fLength = 0;
	Invariant ();
}

template	<class	T>	Boolean	Array<T>::Contains (T item) const
{
	Invariant ();
	if (fLength > 0) {
		register	const	ArrayNode<T>*	current	=	&fItems [0];
		register	const	ArrayNode<T>*	last	=	&fItems [fLength-1];	// safe to -1 since fLength>0
		for (; current <= last; current++) {
			if (current->fItem == item) {
				return (True);
			}
		}
	}
	return (False);
}

template	<class	T>	void	Array<T>::SetSlotsAlloced (size_t slotsAlloced)
{
	Require (GetLength () <= slotsAlloced);
	Invariant ();
	if (fSlotsAllocated != slotsAlloced) {
		if (slotsAlloced == 0) {
			delete (char*)fItems;
			fItems = Nil;
		}
		else {
			/*
			 * We should consider getting rid of use of realloc since it prohibits
			 * internal pointers. For example, we cannot have an array of patchable_arrays.
			 */
			if (fItems == Nil) {
				fItems = (ArrayNode<T>*) new char [sizeof (ArrayNode<T>) * slotsAlloced];
			}
			else {
				fItems = (ArrayNode<T>*) ReAlloc (fItems, sizeof (ArrayNode<T>) * slotsAlloced);
			}
		}
		fSlotsAllocated = slotsAlloced;
	}
	Invariant ();
}

template	<class	T>	Array<T>& Array<T>::operator= (const Array<T>& list)
{
	Invariant ();
	size_t	newLength		=	list.GetLength ();

	/*
	 *		In case user already set this, we should not unset,
	 *	but must be sure we are big enuf. Do this before we store any pointers
	 *	cuz it could invalidate them.
	 */
	SetSlotsAlloced (Max (GetSlotsAlloced (), newLength));

	/*
	 * Copy array elements where both sides where constructed.
	 */
	size_t	commonLength	=	Min (fLength, newLength);
	register	ArrayNode<T>*	lhs	=	&fItems[0];
	register	ArrayNode<T>*	rhs	=	&list.fItems[0];
	for (register size_t i = commonLength; i-- > 0; ) {
		*lhs++ = *rhs++;
	}

	/*
	 * Now if new length smaller, we must destroy entries at the end, and
	 * otherwise we must copy in new entries.
	 */
	Assert (lhs == &fItems[commonLength]);						// point 1 past first guy to destroy/overwrite
	if (fLength > newLength) {
		register	ArrayNode<T>*	end	=	&fItems[fLength];	// point 1 past last old guy
		/*
		 * Then we must destruct entries at the end.
		 */
		Assert (lhs < end);
		do {
			lhs->ArrayNode<T>::~ArrayNode ();
		} while (++lhs < end);
	}
	else if (fLength < newLength) {
		register	ArrayNode<T>*	end	=	&fItems[newLength];	// point 1 past last new guy
		Assert (lhs < end);
		do {
#if		qGCC_PlacementOpNewGeneratesSyntaxError
			new {lhs} ArrayNode<T> (*rhs++);
#else
			new (lhs) ArrayNode<T> (*rhs++);
#endif
		} while (++lhs < end);
	}
	fLength = newLength;
	Invariant ();
	return (*this);	
}

template	<class	T>	void	Array<T>::SetLength (size_t newLength, T fillValue)
{
	Invariant ();

	/*
	 * Safe to grow the memory, but not to shrink it here, since
	 * we may need to destruct guys in the shrinking case.
	 */
	if (newLength > fSlotsAllocated) {
		/*
		 *		Bump up Slots alloced to be at least big enuf for our
		 * new length. We could be minimalistic here, and just bump up
		 * exactly, but this function can be expensive because it calls
		 * realloc which could cause lots of memory copying. There are two
		 * plausible strategies for bumping up memory in big chunks-
		 * rounding up, and scaling up.
		 *
		 *		Rounding up works well at small scales - total memory
		 *	waste is small (bounded). It is simple, and it helps speed up
		 *	loops like while condition { append (); } considerably.
		 *
		 *		Scaling up has the advantage that for large n, we get
		 *	logn reallocs (rather than n/IncSize in the roundup case).
		 *	This is much better long-term large-size performance.
		 *	The only trouble with this approach is that in order to keep
		 *	memory waste small, we must scale by a small number (here 1.1)
		 *	and so we need array sizes > 100 before we start seeing any real
		 *	benefit at all. Such cases do happen, but we want to be able to
		 *	optimize the much more common, small array cases too.
		 *
		 *		So the compromise is to use a roundup-like strategy for
		 *	small n, and a scaling approach as n gets larger.
		 *
		 *		Also, we really should be more careful about overflow here...
		 *
		 *		Some math:
		 *			k*n = n + 64/sizeof (T) and so
		 *			n = (64/sizeof (T))/(k-1)
		 *		If we assume k = 1.1 and sizeof(T) = 4 then n = 160. This is
		 *	the value for length where we start scaling up by 10% as opposed to
		 *	our arithmetic + 16.
		 *		
		 */
		//SetSlotsAlloced (Max (newLength+(64/sizeof (T)), size_t (newLength*1.1)));
		// Based on the above arithmatic, we can take a shortcut...
		SetSlotsAlloced ((newLength > 160)? size_t (newLength*1.1): (newLength+(64/sizeof (T))));
	}
	register	ArrayNode<T>*	cur	=	&fItems[fLength];	// point 1 past first guy
	register	ArrayNode<T>*	end	=	&fItems[newLength];	// point 1 past last guy
	if (newLength > fLength) {
		Assert (cur < end);
		do {
#if		qGCC_PlacementOpNewGeneratesSyntaxError
			new {cur} ArrayNode<T> (fillValue);
#else
			new (cur) ArrayNode<T> (fillValue);
#endif
		} while (++cur < end);
	}
	else {
		Assert (cur >= end);
		while (cur-- > end) {
			cur->ArrayNode<T>::~ArrayNode ();
		}
	}
	fLength = newLength;
	Invariant ();
}

#if		qDebug
template	<class	T>	void	Array<T>::Invariant_ () const
{
	Assert ((fSlotsAllocated == 0) == (fItems == Nil));		// always free iff slots alloced = 0
	Assert (fLength <= fSlotsAllocated);
}
#endif




/*
 ********************************************************************************
 ******************************** ArrayIteratorBase *****************************
 ********************************************************************************
 */
#if		qDebug
template	<class T>	void	ArrayIteratorBase<T>::Invariant_ () const
{
	AssertNotNil (fData);
	Assert (fStart == fData->fItems);
	Assert ((fEnd-fStart) == fData->GetLength ());
	Assert ((fCurrent >= fStart) and (fCurrent <= fEnd));	// ANSI C requires this is always TRUE
}
#endif



/*
 ********************************************************************************
 ******************************** BackwardArrayIterator *************************
 ********************************************************************************
 */
#if		qCFront_SorryNotImplementedStatementAfterReturnInline
template	<class T>	Boolean	BackwardArrayIterator<T>::More ()
{
	Invariant ();
	if (fSupressMore) {
		fSupressMore = False;
		return (not Done ());
	}
	else {
		if (Done ()) {
			return (False);
		}
		else {
			if (fCurrent == fStart) {
				fCurrent = fEnd;	// magic to indicate done
				Ensure (Done ());
				return (False);
			}
			else {
				fCurrent--;
				Ensure (not Done ());
				return (True);
			}
		}
	}
	AssertNotReached ();	return (False);
}
#endif	/*qCFront_SorryNotImplementedStatementAfterReturnInline*/




/*
 ********************************************************************************
 ******************************** Array_Patch ***********************************
 ********************************************************************************
 */
#if		qDebug
template	<class	T>	void	Array_Patch<T>::Invariant_ () const
{
	Array<T>::Invariant_ ();

	/*
	 *		Be sure each iterator points back to us. Thats about all we can test from
	 *	here since we cannot call each iterators Invariant(). That would be
	 *	nice, but sadly when this Invariant_ () is called from Array<T> the
	 *	iterators themselves may not have been patched, so they'll be out of
	 *	date. Instead, so that in local shadow of Invariant() done in Array_Patch<T>
	 *	so only called when WE call Invariant().
	 */
	for (ArrayIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
		Assert (v->fData == this);
	}
}

template	<class	T>	void	Array_Patch<T>::InvariantOnIterators_ () const
{
	/*
	 *		Only here can we iterate over each iterator and calls its Invariant()
	 *	since now we've completed any needed patching.
	 */
	for (ArrayIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
		Assert (v->fData == this);
		v->Invariant ();
	}
}
#endif	/*qDebug*/




/*
 ********************************************************************************
 ******************************* ArrayIterator_PatchBase ************************
 ********************************************************************************
 */
#if		qDebug
template	<class T>	void	ArrayIterator_PatchBase<T>::Invariant_ () const
{
	ArrayIteratorBase<T>::Invariant_ ();
	Assert (fData == ArrayIteratorBase<T>::fData);
}
#endif	/*qDebug*/








/*
 ********************************************************************************
 *************************** BackwardArrayIterator_Patch ************************
 ********************************************************************************
 */

#if		qCFront_SorryNotImplementedStatementAfterReturnInline
	// Careful to keep hdr and src copies identical...
	template	<class T>	Boolean	BackwardArrayIterator_Patch<T>::More ()
	{
		Invariant ();
		if (fSupressMore) {
			fSupressMore = False;
			return (not Done ());
		}
		else {
			if (Done ()) {
				return (False);
			}
			else {
				if (fCurrent == fStart) {
					fCurrent = fEnd;	// magic to indicate done
					Ensure (Done ());
					return (False);
				}
				else {
					fCurrent--;
					Ensure (not Done ());
					return (True);
				}
			}
		}
		AssertNotReached ();	return (False);
	}
#endif	/*qCFront_SorryNotImplementedStatementAfterReturnInline*/

#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
