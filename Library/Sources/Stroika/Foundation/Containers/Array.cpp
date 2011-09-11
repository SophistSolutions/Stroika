/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
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
