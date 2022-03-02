/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Sequence_Array__cc
#define	__Sequence_Array__cc

/*
 * $Header: /fuji/lewis/RCS/Sequence_Array.cc,v 1.36 1992/12/10 05:58:32 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Sequence_Array.cc,v $
 *		Revision 1.36  1992/12/10  05:58:32  lewis
 *		Renamed iterators from XwardSequenceMutatorRep to XwardSequence_ArrayMutatorRep.
 *		Got rid of friendship of iterators to Sequence_ArrayRep, and so changed
 *		arg to iterators from rep to Array_Patch<T>&.
 *
 *		Revision 1.35  1992/12/03  05:58:34  lewis
 *		Get rid of fFirstTime hack - now taken care of by Array iterators themselves.
 *		Use X(X&) in clone methods - this, coupled with new mutator capability in
 *		array to get rid of fOwner field.
 *		Rename Skrunch->Compact.
 *		Use array provided iterators and mutators internally more instead of old
 *		indexing forloops.
 *
 *		Revision 1.34  1992/11/23  22:47:41  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.33  1992/11/20  19:27:53  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.32  1992/11/16  04:44:09  lewis
 *		Work around qBCC_ConditionalOperatorDemandsTypesExactBug.
 *
 *		Revision 1.31  1992/11/13  03:48:23  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *
 *		Revision 1.30  1992/11/12  08:17:52  lewis
 *		#include BlockAllocated, and wrap template name predeclares
 *		with #of !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug.
 *
 *		Revision 1.29  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.28  1992/11/07  16:30:37  lewis
 *		Try new arrayiteratorbase stuff suggested by kdj, and use
 *		in various mixins that use Array.
 *
 *		Revision 1.27  1992/11/05  15:21:48  lewis
 *		Lots of cleanups, and reorderings in port to MPW.
 *
 *		Revision 1.26  1992/11/01  01:26:35  lewis
 *		New Array Iterator support with separate subclass for patching.
 *
 *		Revision 1.24  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.23  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.22  1992/10/29  15:55:40  lewis
 *		Use qCFront_InlinesOpNewDeleteMustDeclareInline to work around MPW bug, and
 *		fix accidental return of void in iterator::next().
 *
 *		Revision 1.21  1992/10/22  04:19:27  lewis
 *		made SequenceRep_Array<T>::operator delete inline again since I moved
 *		~SequenceRep_Array<T> to .cc file.
 *
 *		Revision 1.20  1992/10/20  21:56:40  lewis
 *		Make SequenceRep_Array<T>::operator delete (void* p) out of line since we
 *		got link error - investigate why needed later...
 *
 *		Revision 1.19  1992/10/20  21:25:21  lewis
 *		Add  operator new/delete overloads for Sequence_ArrayRep using BlockAllocated.
 *
 *		Revision 1.18  1992/10/20  17:53:44  lewis
 *		Cleanup comments and made GetLength,IsEmpty, and GetAt() for rep and
 *		Sequence_Array to be inline - thus avoiding (virtual) function overhead
 *		when we know the static type for these operations.
 *		Use Backward iterator in Remove call - since uses ptr arithmatic sb faster.
 *		Got rid of Contains override - no longer required by Sequence.
 *		Added block allocator for iterators.
 *
 *		Revision 1.17  1992/10/15  02:29:53  lewis
 *		A few cleanups including calling PatchViewsRealloc in SetSlotsAlloced(),
 *		and getting rid of hintSize arg to SequenceRep_Array and a few other minor
 *		changes.
 *
 *		Revision 1.16  1992/10/14  16:22:30  lewis
 *		Delete obsolte iterator stuff. Be more careful where we call patches
 *		- now carefully documented in ArrayIterator header.
 *		Make PatchViewsXXX inline - CFront wont inline them, but decent compilers
 *		will and should. Got rid of hintSize CTORs. Just call SetSlotsAlloced directly.
 *
 *		Revision 1.15  1992/10/13  05:33:43  lewis
 *		New iterator strategy tried out - arrayiterator not subclassing
 *		from iterator - we do our own. Now patch methods inline, not
 *		virtual.
 *
 *		Revision 1.14  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.13  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
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
 *		Revision 1.9  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.6  1992/07/21  05:52:44  lewis
 *		Cast away const in calling SetAt in iterator.
 *
 *		Revision 1.5  1992/07/19  08:53:45  lewis
 *		Fix typos.
 *
 *		Revision 1.4  1992/07/19  07:28:28  lewis
 *		Sped up implementations of iterator::Done () and corrected incorrect
 *		implementation of PatchAdd/PatchRemove - how painful analyzing all
 *		the cases....
 *
 *		Revision 1.3  1992/07/19  03:54:04  lewis
 *		A few more minor cleanups, including assuring that macro version and
 *		template version identical, and testing macros under GCC/AUX.
 *
 *		Revision 1.2  1992/07/17  03:52:59  lewis
 *		Got rid of class Collection/SequenceMutator, and consider non-const
 *		CollectionIterator to be a Mutator. Very careful line by line
 *		analysis, and bugfixing, and implemented and tested (compiling)
 *		the macro version under GCC. In macro version got rid of overload
 *		of new/delete in iterator cuz BlockAllocated<> declare not done
 *		automatically.
 *
 *		Revision 1.1  1992/06/19  22:29:57  lewis
 *		Initial revision
 *
 *		Revision 1.4  1992/05/23  00:58:28  lewis
 *		Added first cut at Sequence_ArrayMutator<T>/
 *
 *		Revision 1.3  92/05/21  17:30:41  17:30:41  lewis (Lewis Pringle)
 *		First cut trying to react to overhaul of Sequence/Collection - template
 *		version only.
 *		
 *		Revision 1.2  92/05/09  23:31:23  23:31:23  lewis (Lewis Pringle)
 *		Fixed minor syntax errors porting to BC++/templates.
 *		
 *
 */



#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"Sequence.hh"			// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Sequence_Array.hh"







#if		qRealTemplatesAvailable

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif


template	<class T> class	ForwardSequence_ArrayMutatorRep : public SequenceMutatorRep<T> {
	public:
		ForwardSequence_ArrayMutatorRep (Array_Patch<T>& data);

		override	Boolean	Done () const;
		override	Boolean	More ();
		override	T		Current () const;
		override	size_t	CurrentIndex ()	const;
		override	SequenceDirection	GetDirection () const;

		override	IteratorRep<T>*		Clone () const;

		override	void	RemoveCurrent ();
		override	void	UpdateCurrent (T newValue);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

	private:
		ForwardArrayMutator_Patch<T>	fIterator;

	friend	class	SequenceRep_Array<T>;
};


template	<class T> class	BackwardSequence_ArrayMutatorRep : public SequenceMutatorRep<T> {
	public:
		BackwardSequence_ArrayMutatorRep (Array_Patch<T>& data);

		override	Boolean	Done () const;
		override	Boolean	More ();
		override	T		Current () const;
		override	size_t	CurrentIndex ()	const;
		override	SequenceDirection	GetDirection () const;

		override	IteratorRep<T>*		Clone () const;

		override	void	RemoveCurrent ();
		override	void	UpdateCurrent (T newValue);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

	private:
		BackwardArrayMutator_Patch<T>	fIterator;

	friend	class	SequenceRep_Array<T>;
};






/*
 ********************************************************************************
 ******************************* ForwardSequence_ArrayMutatorRep **********************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfForwardSequence_ArrayMutatorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	ForwardSequence_ArrayMutatorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<ForwardSequence_ArrayMutatorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	ForwardSequence_ArrayMutatorRep<T>::operator delete (void* p)
{
	BlockAllocated<ForwardSequence_ArrayMutatorRep<T> >::operator delete (p);
}

template	<class	T>	ForwardSequence_ArrayMutatorRep<T>::ForwardSequence_ArrayMutatorRep (Array_Patch<T>& data):
	SequenceMutatorRep<T> (),
	fIterator (data)
{
}

template	<class	T>	Boolean	ForwardSequence_ArrayMutatorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class	T>	Boolean	ForwardSequence_ArrayMutatorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class	T>	T	ForwardSequence_ArrayMutatorRep<T>::Current () const
{
	return (fIterator.Current());
}

template	<class	T>	size_t	ForwardSequence_ArrayMutatorRep<T>::CurrentIndex () const
{
	return (fIterator.CurrentIndex());
}

template	<class	T>	SequenceDirection	ForwardSequence_ArrayMutatorRep<T>::GetDirection () const
{
	return (eSequenceForward);
}

template	<class	T>	void	ForwardSequence_ArrayMutatorRep<T>::RemoveCurrent ()
{
	fIterator.RemoveCurrent ();
}

template	<class	T>	void	ForwardSequence_ArrayMutatorRep<T>::UpdateCurrent (T newValue)
{
	fIterator.UpdateCurrent (newValue);
}

template	<class T>	IteratorRep<T>*	ForwardSequence_ArrayMutatorRep<T>::Clone () const
{
	return (new ForwardSequence_ArrayMutatorRep<T> (*this));
}





/*
 ********************************************************************************
 ************************* BackwardSequence_ArrayMutatorRep *********************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfBackwardSequence_ArrayMutatorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	BackwardSequence_ArrayMutatorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<BackwardSequence_ArrayMutatorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	BackwardSequence_ArrayMutatorRep<T>::operator delete (void* p)
{
	BlockAllocated<BackwardSequence_ArrayMutatorRep<T> >::operator delete (p);
}

template	<class	T>	BackwardSequence_ArrayMutatorRep<T>::BackwardSequence_ArrayMutatorRep (Array_Patch<T>& data):
	SequenceMutatorRep<T> (),
	fIterator (data)
{
}

template	<class	T>	Boolean	BackwardSequence_ArrayMutatorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class	T>	Boolean	BackwardSequence_ArrayMutatorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class	T>	T	BackwardSequence_ArrayMutatorRep<T>::Current () const
{
	return (fIterator.Current());
}

template	<class	T>	size_t	BackwardSequence_ArrayMutatorRep<T>::CurrentIndex () const
{
	return (fIterator.CurrentIndex());
}

template	<class	T>	SequenceDirection	BackwardSequence_ArrayMutatorRep<T>::GetDirection () const
{
	return (eSequenceBackward);
}

template	<class	T>	void	BackwardSequence_ArrayMutatorRep<T>::RemoveCurrent ()
{
	fIterator.RemoveCurrent ();
}

template	<class	T>	void	BackwardSequence_ArrayMutatorRep<T>::UpdateCurrent (T newValue)
{
	fIterator.UpdateCurrent (newValue);
}

template	<class T>	IteratorRep<T>*	BackwardSequence_ArrayMutatorRep<T>::Clone () const
{
	return (new BackwardSequence_ArrayMutatorRep<T> (*this));
}






/*
 ********************************************************************************
 ******************************** SequenceRep_Array *****************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	SequenceRep_Array;	// Tmp hack so GenClass will fixup following CTOR/DTORs
													// Harmless, but silly.

#endif

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfSequenceRep_ArrayOf<T>_cc
#endif

template	<class	T>	inline	void*	SequenceRep_Array<T>::operator new (size_t size)
{
	return (BlockAllocated<SequenceRep_Array<T> >::operator new (size));
}

template	<class	T>	inline	void	SequenceRep_Array<T>::operator delete (void* p)
{
	BlockAllocated<SequenceRep_Array<T> >::operator delete (p);
}

template	<class	T>	SequenceRep_Array<T>::SequenceRep_Array ()
{
}

template	<class	T>	SequenceRep_Array<T>::~SequenceRep_Array ()
{
	// declared here instead of inline since it goes into vtable, and we want it to
	// call inline op delete defined above...
}

template	<class T> IteratorRep<T>*	SequenceRep_Array<T>::MakeIterator ()
{
	return (new ForwardSequence_ArrayMutatorRep<T> (fData));
}

template	<class T>	void SequenceRep_Array<T>::Compact ()
{
	fData.Compact ();
}

template	<class T>	SequenceRep<T>*	SequenceRep_Array<T>::Clone () const
{
	return (new SequenceRep_Array<T> (*this));
}

template	<class T>	void SequenceRep_Array<T>::SetAt (T item, size_t index)
{
	fData.SetAt (item, index);
}

template	<class T>	void SequenceRep_Array<T>::InsertAt (T item, size_t index)
{
	fData.InsertAt (item, index);
}

template	<class T>	void	SequenceRep_Array<T>::Remove (T item)
{
	for (BackwardArrayIterator<T> it (fData); it.More ();) {
		if (it.Current () == item) {
			RemoveAt (it.CurrentIndex (), 1);
			return;
		}
	}
}

template	<class T>	void	SequenceRep_Array<T>::RemoveAt (size_t index, size_t amountToRemove)
{
#if		0
// As optimization, may want to do something like this - disabled cuz
// of new iterator stuff... LGP Saturday, October 31, 1992 10:37:25 AM
	while (amountToRemove-- > 0) {
		PatchViewsRemove (index);
		fData.RemoveAt (index++);
	}
	PatchViewsRealloc ();
#else
	while (amountToRemove-- > 0) {
		fData.RemoveAt (index++);
	}
#endif
}

template	<class T>	void SequenceRep_Array<T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class T>	SequenceIteratorRep<T>*	SequenceRep_Array<T>::MakeSequenceIterator (SequenceDirection d)
{
	return (SequenceRep_Array<T>::MakeSequenceMutator (d));
}

template	<class T>	SequenceMutatorRep<T>*	SequenceRep_Array<T>::MakeSequenceMutator (SequenceDirection d)
{
#if		qBCC_ConditionalOperatorDemandsTypesExactBug
	if (d == eSequenceForward) {
		return (new ForwardSequence_ArrayMutatorRep<T> (fData));
	}
	else {
		return (new BackwardSequence_ArrayMutatorRep<T> (fData));
	}
#else
	return ((d == eSequenceForward)? new ForwardSequence_ArrayMutatorRep<T> (fData): new BackwardSequence_ArrayMutatorRep<T> (fData));
#endif
}





/*
 ********************************************************************************
 ************************************* Sequence_Array ***************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Sequence_Array;	// Tmp hack so GenClass will fixup following CTOR/DTORs
												// Harmless, but silly.

#endif

template	<class T>	Sequence_Array<T>::Sequence_Array ():
	Sequence<T> (new SequenceRep_Array<T> ())
{
}

template	<class T>	Sequence_Array<T>::Sequence_Array (const Sequence<T>& seq):
	Sequence<T> (new SequenceRep_Array<T> ())
{
	SetSlotsAlloced (seq.GetLength ());
	operator+= (seq);
}

template	<class T>	Sequence_Array<T>::Sequence_Array (const T* items, size_t size):
	Sequence<T> (new SequenceRep_Array<T> ())
{
	SetSlotsAlloced (size);
	AddItems (items, size);
}

template	<class	T>	size_t	Sequence_Array<T>::GetSlotsAlloced () const
{
	return (GetRep ()->fData.GetSlotsAlloced ());
}

template	<class	T>	void	Sequence_Array<T>::SetSlotsAlloced (size_t slotsAlloced)
{
	GetRep ()->fData.SetSlotsAlloced (slotsAlloced);
}

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifndef	inline
		#error	"How did it get undefined?"
	#endif
	#undef	inline
#endif


#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

