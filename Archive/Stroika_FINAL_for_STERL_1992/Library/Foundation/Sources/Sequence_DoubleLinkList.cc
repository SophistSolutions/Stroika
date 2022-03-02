/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Sequence_DoubleLinkList__cc
#define	__Sequence_DoubleLinkList__cc

/*
 * $Header: /fuji/lewis/RCS/Sequence_DoubleLinkList.cc,v 1.33 1992/12/11 22:31:17 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Sequence_DoubleLinkList.cc,v $
 *		Revision 1.33  1992/12/11  22:31:17  lewis
 *		Fixed bug in RemoveAt() insertation - logically correct but
 *		did test of input variable that had been munged...
 *
 *		Revision 1.32  1992/12/10  07:25:51  lewis
 *		In Remove(T), just call DoubleLinkList::Remove(T) - very simple - we dont
 *		even need to do the mutator ourselves.
 *		In RemoveAt, add assert that right number of items removed, and
 *		if item Count != 1, then use forward case, as backwards case a bit
 *		more tricky to get right (for deleting more than one).
 *
 *		Revision 1.31  1992/12/10  07:14:14  lewis
 *		Updated for changes to rewritten DoubleLinkList.
 *
 *		Revision 1.30  1992/12/04  19:21:35  lewis
 *		Work around qBCC_InlinesBreakTemplates, and reformat text.
 *
 *		Revision 1.29  1992/12/03  07:15:57  lewis
 *		Use default generated (no longer provide our own) X(X&) for reps,
 *		and use these in cloning. Converted (partially) DoubleLinkList to new
 *		More() API, and use got rid of fFirstTime hack. Use new API.
 *		Override Done() in mutator.
 *
 *		Revision 1.28  1992/11/20  19:27:53  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.27  1992/11/13  03:49:14  lewis
 *		Add missing template      <class T>.
 *
 *		Revision 1.26  1992/11/12  08:22:51  lewis
 *		Wrap template decalration for GenClass hack with
 *		#if !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug.
 *
 *		Revision 1.25  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.24  1992/11/03  22:52:41  lewis
 *		Switch over to new DoubleLinkList and DoubleLinkList_Patch_Forward, etc strategy
 *		for backends.
 *		Debugged (hopefully - not tested yet) iterator patching - based on what
 *		we did in LinkList that did pass testsuites.
 *
 *		Revision 1.23  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.22  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.21  1992/10/22  04:33:22  lewis
 *		Fix typo - (T)-><T>
 *
 *		Revision 1.20  1992/10/15  02:31:04  lewis
 *		Moved some stuff here from the header.
 *
 *		Revision 1.19  1992/10/14  16:27:44  lewis
 *		Oops - should have had Seqeunce_DLLRep subclass from CollectionRep instead.
 *		Again, just temporary.
 *
 *		Revision 1.18  1992/10/14  16:25:47  lewis
 *		Moved rep declaration into .cc file. Have iterators subclass from CollectionRep,
 *		so we can still use old stuff - get rid of soon.
 *
 *		Revision 1.17  1992/10/14  02:55:51  lewis
 *		Move interators to .cc file (declarations) and move inlines to
 *		Implemenation Detail section at end of header file.
 *
 *		Revision 1.16  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.15  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.14  1992/09/25  21:14:26  lewis
 *		Fix lots of templates problems and get working using new GenClass stuff.
 *
 *		Revision 1.13  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.12  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.11  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.10  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.9  1992/09/04  21:08:23  sterling
 *		Added contains override.
 *
 *		Revision 1.7  1992/07/21  17:29:48  lewis
 *		Work around qMPW_TEMP_OF_CLASS_WITH_DTOR_IN_EXPR_BUG.
 *
 *		Revision 1.6  1992/07/21  05:51:47  lewis
 *		Use SizeInContainer instead of Container::Size as a workaround for
 *		CFront 2.1 bugs with nested types, and needed for macros - we do it
 *		in the template here too so we can automate coversion from templates
 *		Ato macros more easily.
 *		nd, fix syntax error returing value in UpdateCurrent when should be
 *		just set.
 *
 *		Revision 1.5  1992/07/19  08:54:30  lewis
 *		Nearly total rewrite of template version and then use that to
 *		generate macro version from scratch. Got macro version to compile
 *		on gcc/aux.
 *
 *		Revision 1.3  1992/07/14  20:59:48  lewis
 *		Added MakeSequenceMutator override.
 *
 *		Revision 1.2  1992/07/02  03:29:57  lewis
 *		Renamed Sequence_DoublyLinkedList->Sequence_DoubleLinkList.
 *
 *		Revision 1.3  1992/06/10  04:12:21  lewis
 *		Got rid of Sort function - do later as an external object operating on a sequence, ala
 *		Stroustrups second edition.
 *
 *		Revision 1.2  92/05/09  23:47:56  23:47:56  lewis (Lewis Pringle)
 *		Fixed minor syntax errors porting to BC++/templates.
 *		
 *
 */



#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"DoubleLinkList.hh"
#include	"Memory.hh"

#include	"Sequence_DoubleLinkList.hh"









#if		qRealTemplatesAvailable

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class T>	class	Sequence_DoubleLinkListRep : public SequenceRep<T> {
	public:
		Sequence_DoubleLinkListRep ();

		override	size_t	GetLength () const;
		override	Boolean	Contains (T item) const;
		override	void	Compact ();
		override	SequenceRep<T>*	Clone () const;
		override	T		GetAt (size_t index) const;
		override	void	SetAt (T item, size_t index);
		override	void	InsertAt (T item, size_t index);
		override	void	RemoveAt (size_t index, size_t amountToRemove);
		override	void	Remove (T item);
		override	void	RemoveAll ();

		override	IteratorRep<T>*			MakeIterator ();
		override	SequenceIteratorRep<T>*	MakeSequenceIterator (SequenceDirection d);
		override	SequenceMutatorRep<T>*	MakeSequenceMutator (SequenceDirection d);

		#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
		#endif
		static	void*	operator new (size_t size);
	
		#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
		#endif
		static	void	operator delete (void* p);

	private:
		DoubleLinkList_Patch<T>	fData;
};

template	<class T> class	ForwardSequence_DoubleLinkListMutatorRep : public SequenceMutatorRep<T> {
	public:
		ForwardSequence_DoubleLinkListMutatorRep<T> (DoubleLinkList_Patch<T>& data);

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
		ForwardDoubleLinkListMutator_Patch<T>	fIterator;
};


template	<class T> class	BackwardSequence_DoubleLinkListMutatorRep : public SequenceMutatorRep<T> {
	public:
		BackwardSequence_DoubleLinkListMutatorRep<T> (DoubleLinkList_Patch<T>& data);

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
		BackwardDoubleLinkListMutator_Patch<T>	fIterator;
};






/*
 ********************************************************************************
 *********************** ForwardSequence_DoubleLinkListMutatorRep ***************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfForwardSequence_DoubleLinkListMutatorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	ForwardSequence_DoubleLinkListMutatorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<ForwardSequence_DoubleLinkListMutatorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	ForwardSequence_DoubleLinkListMutatorRep<T>::operator delete (void* p)
{
	BlockAllocated<ForwardSequence_DoubleLinkListMutatorRep<T> >::operator delete (p);
}

template	<class	T>	ForwardSequence_DoubleLinkListMutatorRep<T>::ForwardSequence_DoubleLinkListMutatorRep (DoubleLinkList_Patch<T>& data):
	SequenceMutatorRep<T> (),
	fIterator (data)
{
}

template	<class	T>	Boolean	ForwardSequence_DoubleLinkListMutatorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class	T>	Boolean	ForwardSequence_DoubleLinkListMutatorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class	T>	T	ForwardSequence_DoubleLinkListMutatorRep<T>::Current () const
{
	return (fIterator.Current ());
}

template	<class	T>	void	ForwardSequence_DoubleLinkListMutatorRep<T>::UpdateCurrent (T newValue)
{
	fIterator.UpdateCurrent (newValue);
}

template	<class	T>	void	ForwardSequence_DoubleLinkListMutatorRep<T>::RemoveCurrent ()
{
	fIterator.RemoveCurrent ();
}

template	<class	T>	size_t	ForwardSequence_DoubleLinkListMutatorRep<T>::CurrentIndex () const
{
	return (fIterator.CurrentIndex ());
}

template	<class	T>	SequenceDirection	ForwardSequence_DoubleLinkListMutatorRep<T>::GetDirection () const
{
	return (eSequenceForward);
}

template	<class T>	IteratorRep<T>*	ForwardSequence_DoubleLinkListMutatorRep<T>::Clone () const
{
	return (new ForwardSequence_DoubleLinkListMutatorRep<T> (*this));
}







/*
 ********************************************************************************
 *********************** BackwardSequence_DoubleLinkListMutatorRep **************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfBackwardSequence_DoubleLinkListMutatorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	BackwardSequence_DoubleLinkListMutatorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<BackwardSequence_DoubleLinkListMutatorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	BackwardSequence_DoubleLinkListMutatorRep<T>::operator delete (void* p)
{
	BlockAllocated<BackwardSequence_DoubleLinkListMutatorRep<T> >::operator delete (p);
}

template	<class	T>	BackwardSequence_DoubleLinkListMutatorRep<T>::BackwardSequence_DoubleLinkListMutatorRep (DoubleLinkList_Patch<T>& data):
	SequenceMutatorRep<T> (),
	fIterator (data)
{
}

template	<class	T>	Boolean	BackwardSequence_DoubleLinkListMutatorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class	T>	Boolean	BackwardSequence_DoubleLinkListMutatorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class	T>	T	BackwardSequence_DoubleLinkListMutatorRep<T>::Current () const
{
	return (fIterator.Current ());
}

template	<class	T>	void	BackwardSequence_DoubleLinkListMutatorRep<T>::UpdateCurrent (T newValue)
{
	fIterator.UpdateCurrent (newValue);
}

template	<class	T>	void	BackwardSequence_DoubleLinkListMutatorRep<T>::RemoveCurrent ()
{
	fIterator.RemoveCurrent ();
}

template	<class	T>	size_t	BackwardSequence_DoubleLinkListMutatorRep<T>::CurrentIndex () const
{
	return (fIterator.CurrentIndex ());
}

template	<class	T>	SequenceDirection	BackwardSequence_DoubleLinkListMutatorRep<T>::GetDirection () const
{
	return (eSequenceBackward);
}

template	<class T>	IteratorRep<T>*	BackwardSequence_DoubleLinkListMutatorRep<T>::Clone () const
{
	return (new BackwardSequence_DoubleLinkListMutatorRep<T> (*this));
}






/*
 ********************************************************************************
 *************************** Sequence_DoubleLinkListRep *************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfSequence_DoubleLinkListRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Sequence_DoubleLinkListRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Sequence_DoubleLinkListRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Sequence_DoubleLinkListRep<T>::operator delete (void* p)
{
	BlockAllocated<Sequence_DoubleLinkListRep<T> >::operator delete (p);
}

template	<class T>	inline	Sequence_DoubleLinkListRep<T>::Sequence_DoubleLinkListRep ()
{
}

template	<class T>	size_t Sequence_DoubleLinkListRep<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class	T>	Boolean	Sequence_DoubleLinkListRep<T>::Contains (T item) const
{
	return (fData.Contains (item));
}

template	<class T> IteratorRep<T>*	Sequence_DoubleLinkListRep<T>::MakeIterator ()
{
	return (new ForwardSequence_DoubleLinkListMutatorRep<T> (fData));
}

template	<class T>	void Sequence_DoubleLinkListRep<T>::Compact ()
{
}

template	<class T>	SequenceRep<T>*	Sequence_DoubleLinkListRep<T>::Clone () const
{
	return (new Sequence_DoubleLinkListRep<T> (*this));
}

template	<class T>	T Sequence_DoubleLinkListRep<T>::GetAt (size_t index) const
{
	/*
	 * Ouch - hadn't you better be using an array???
	 */
	return (fData.GetAt (index));
}

template	<class T>	void Sequence_DoubleLinkListRep<T>::SetAt (T item, size_t index)
{
	/*
	 * Ouch - hadn't you better be using an array???
	 */
	fData.SetAt (item, index);
}

template	<class T>	void Sequence_DoubleLinkListRep<T>::InsertAt (T item, size_t index)
{
	Require (index >= 1);
	Require (index <= GetLength () + 1);

	if (index == 1) {
		fData.Prepend (item);
	}
	else if (index == GetLength () + 1) {
		fData.Append (item);
	}
	else {
		if (index < fData.GetLength () / 2) {
			for (ForwardDoubleLinkListMutator_Patch<T> it (fData); it.More (); ) {
				if (--index == 0) {
					it.AddBefore (item);
					break;
				}
			}
			Assert (not it.Done ());		// cuz that would mean we never added
		}
		else {
			for (BackwardDoubleLinkListMutator_Patch<T> it (fData); it.More (); ) {
				if (index++ == GetLength ()) {
					it.AddBefore (item);
					break;
				}
			}
			Assert (not it.Done ());		// cuz that would mean we never added
		}
	}
	Ensure (GetAt (index) == item);
}

template	<class T>	void	Sequence_DoubleLinkListRep<T>::Remove (T item)
{
	fData.Remove (item);
}

template	<class T>	void Sequence_DoubleLinkListRep<T>::RemoveAt (size_t index, size_t amountToRemove)
{
#if		qDebug
	size_t	oldLen	=	GetLength ();
	Require (oldLen >= amountToRemove);
	size_t	newLen	=	oldLen-amountToRemove;
#endif
	Require (index >= 1);
	Require (index <= GetLength ());
	Require (amountToRemove >= 1);
	Require (index+amountToRemove-1 <= GetLength ());

	if (index == 1) {
		while (amountToRemove-- != 0) {
			fData.RemoveFirst ();
		}
	}
	else if (index == GetLength ()-amountToRemove+1) {
		while (amountToRemove-- != 0) {
			fData.RemoveLast ();
		}
	}
	else {
		// Use forward mutator if closer, or we are removing more than one.
		// dont use backwards iterator for more than one, cuz slightly more
		// tricky (fix later) to get the right guys deleted...
		if ((amountToRemove != 1) or index < fData.GetLength () / 2) {
			for (ForwardDoubleLinkListMutator_Patch<T> it (fData); it.More (); ) {
				if (--index == 0) {
					while (amountToRemove-- != 0) {
						it.RemoveCurrent ();
					}
					break;
				}
			}
		}
		else {
			for (BackwardDoubleLinkListMutator_Patch<T> it (fData); it.More (); ) {
				if (index++ == GetLength ()) {
					it.RemoveCurrent ();
					break;
				}
			}
		}
	}
#if		qDebug
	Ensure (GetLength () == newLen);
#endif
}

template	<class T>	void Sequence_DoubleLinkListRep<T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class	T>	SequenceIteratorRep<T>*	Sequence_DoubleLinkListRep<T>::MakeSequenceIterator (SequenceDirection d)
{
	return (Sequence_DoubleLinkListRep<T>::MakeSequenceMutator (d));
}

template	<class	T>	SequenceMutatorRep<T>*		Sequence_DoubleLinkListRep<T>::MakeSequenceMutator (SequenceDirection d)
{
#if		qBCC_ConditionalOperatorDemandsTypesExactBug
	if (d == eSequenceForward) {
		return (new ForwardSequence_DoubleLinkListMutatorRep<T> (fData));
	}
	else {
		return (new BackwardSequence_DoubleLinkListMutatorRep<T> (fData));
	}
#else
	return ((d == eSequenceForward)? new ForwardSequence_DoubleLinkListMutatorRep<T> (fData) : new BackwardSequence_DoubleLinkListMutatorRep<T> (fData));
#endif
}





/*
 ********************************************************************************
 *************************** Sequence_DoubleLinkList ****************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Sequence_DoubleLinkList;	// Tmp hack so GenClass will fixup following CTOR/DTORs
															// Harmless, but silly.

#endif

template	<class T>	Sequence_DoubleLinkList<T>::Sequence_DoubleLinkList ():
	Sequence<T> (new Sequence_DoubleLinkListRep<T> ())
{
}

template	<class T>	Sequence_DoubleLinkList<T>::Sequence_DoubleLinkList(const T* items, size_t size):
	Sequence<T> (new Sequence_DoubleLinkListRep<T> ())
{
	AddItems (items, size);
}

template	<class T>	Sequence_DoubleLinkList<T>::Sequence_DoubleLinkList (const Sequence<T>& seq):
	Sequence<T> (new Sequence_DoubleLinkListRep<T> ())
{
	operator+= (seq);
}

template	<class T>	Sequence_DoubleLinkList<T>::Sequence_DoubleLinkList (const Sequence_DoubleLinkList<T>& seq) :
	Sequence<T> (seq)
{
}

template	<class T>	Sequence_DoubleLinkList<T>& Sequence_DoubleLinkList<T>::operator= (const Sequence_DoubleLinkList<T>& seq)
{
	Sequence<T>::operator= (seq);
	return (*this);
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

