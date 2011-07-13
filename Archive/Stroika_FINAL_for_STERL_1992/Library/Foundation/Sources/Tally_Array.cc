/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Tally_Array__cc
#define	__Tally_Array__cc

/*
 * $Header: /fuji/lewis/RCS/Tally_Array.cc,v 1.21 1992/12/05 06:40:03 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Tally_Array.cc,v $
 *		Revision 1.21  1992/12/05  06:40:03  lewis
 *		In workaround for qBCC_OpEqualsTemplateNotAutoExpanded, its important
 *		that Bogus()'s params be by value - not reference. This may be a key
 *		in understanding the pattern of brain-dead template behavior with BCC.
 *
 *		Revision 1.20  1992/12/04  20:14:15  lewis
 *		Work around qBCC_InlinesBreakTemplates, and qBCC_OpEqualsTemplateNotAutoExpanded.
 *
 *		Revision 1.19  1992/12/03  05:52:00  lewis
 *		Get rid of fFirstTime hack - now taken care of by Array iterators themselves.
 *		Use X(X&) in clone methods - this, coupled with new mutator capability in
 *		array to get rid of fOwner field.
 *		Rename Skrunch->Compact.
 *		Use array provided iterators and mutators internally more instead of old
 *		indexing forloops.
 *		Fix buggy handling of when size of item in list went to zero - not always
 *		deleted before - it is now - we assert count TallyEntry in list never
 *		zero.
 *
 *		Revision 1.18  1992/11/20  19:21:06  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.17  1992/11/13  03:33:55  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *		Also, be more careful about !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
 *		hacks.
 *
 *		Revision 1.16  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.15  1992/11/07  16:30:37  lewis
 *		Try new arrayiteratorbase stuff suggested by kdj, and use
 *		in various mixins that use Array.
 *
 *		Revision 1.14  1992/11/04  19:41:43  lewis
 *		Misc cleanups - reorder classes, use blockallocater.
 *
 *		Revision 1.13  1992/11/02  19:53:02  lewis
 *		Finish using new Array Iterator support.
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
 *		Revision 1.10  1992/10/22  14:55:25  lewis
 *		Fixed typos - got testsuites to compile.
 *
 *		Revision 1.9  1992/10/19  21:42:07  lewis
 *		Move inlines from class declaration to implemenation details section at end
 *		of header, and moved concrete reps to .cc file. Added #includes where needed
 *		for GenClass stuff. Other minor cleanups - untested yet.
 *
 *		Revision 1.8  1992/10/18  07:04:53  lewis
 *		#if !qRealTemplatesAvailable, then #include ArrayOfTallyEntryOf<T>_cc.
 *
 *		Revision 1.7  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.6  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.5  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.4  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.3  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		
 *
 */



#include	"Array.hh"
#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"Tally.hh"				// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Tally_Array.hh"







#if		qRealTemplatesAvailable


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	ArrayOfTallyEntryOf<T>_cc
#endif


#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class T>	class	Tally_ArrayMutatorRep;
template	<class T>	class	Tally_ArrayRep : public TallyRep<T> {
	public:
		Tally_ArrayRep ();

#if		qRealTemplatesAvailable && qBCC_OpEqualsTemplateNotAutoExpanded
		// See similar hack on Mapping_Array
		Boolean	Bogus(TallyEntry<T> lhs, TallyEntry<T> rhs) { return (lhs == rhs); }
#endif

		override	size_t	GetLength () const;
		override	Boolean	Contains (T item) const;
		override	void	Compact ();
		override	TallyRep<T>*	Clone () const;
	 	override	void	RemoveAll ();

		override	void	Add (T item, size_t count);
		override	void	Remove (T item, size_t count);
		override	size_t	TallyOf (T item) const;

	 	override	IteratorRep<TallyEntry<T> >*	MakeTallyIterator ();
		override	TallyMutatorRep<T>*				MakeTallyMutator ();

		nonvirtual	void	RemoveAt (size_t index);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

	private:
		Array_Patch<TallyEntry<T> >	fData;

		nonvirtual	size_t	Find (TallyEntry<T>& item) const;

	friend	class	Tally_Array<T>;
	friend	class	Tally_ArrayMutatorRep<T>;
};

template	<class T> class	Tally_ArrayMutatorRep : public TallyMutatorRep<T> {
	public:
		Tally_ArrayMutatorRep (Tally_ArrayRep<T>& owner);

		override	Boolean			Done () const;
		override	Boolean			More ();
		override	TallyEntry<T>	Current () const;

		override	IteratorRep<TallyEntry<T> >*	Clone () const;

		override	void	RemoveCurrent ();
		override	void	UpdateCount (size_t newCount);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

	private:
		ForwardArrayMutator_Patch<TallyEntry<T> >	fIterator;
	friend	class	Tally_ArrayRep<T>;
};






/*
 ********************************************************************************
 ********************************* Tally_ArrayMutatorRep ************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfTally_ArrayMutatorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Tally_ArrayMutatorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Tally_ArrayMutatorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Tally_ArrayMutatorRep<T>::operator delete (void* p)
{
	BlockAllocated<Tally_ArrayMutatorRep<T> >::operator delete (p);
}

template	<class	T>	Tally_ArrayMutatorRep<T>::Tally_ArrayMutatorRep (Tally_ArrayRep<T>& owner) :
	TallyMutatorRep<T> (),
	fIterator (owner.fData)
{
}

template	<class	T>	Boolean	Tally_ArrayMutatorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class	T>	Boolean	Tally_ArrayMutatorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class	T>	TallyEntry<T>	Tally_ArrayMutatorRep<T>::Current () const
{
	return (fIterator.Current());
}

template	<class T>	IteratorRep<TallyEntry<T> >*	Tally_ArrayMutatorRep<T>::Clone () const
{
	return (new Tally_ArrayMutatorRep<T> (*this));
}

template	<class T>	void	Tally_ArrayMutatorRep<T>::RemoveCurrent ()
{
	fIterator.RemoveCurrent ();
}

template	<class T>	void	Tally_ArrayMutatorRep<T>::UpdateCount (size_t newCount)
{
	if (newCount == 0) {
		fIterator.RemoveCurrent ();
	}
	else {
		TallyEntry<T>	c	=	fIterator.Current ();
		c.fCount = newCount;
		fIterator.UpdateCurrent (c);
	}
}





/*
 ********************************************************************************
 ********************************* Tally_ArrayRep *******************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfTally_ArrayRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Tally_ArrayRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Tally_ArrayRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Tally_ArrayRep<T>::operator delete (void* p)
{
	BlockAllocated<Tally_ArrayRep<T> >::operator delete (p);
}

template	<class T>	inline	Tally_ArrayRep<T>::Tally_ArrayRep () :
	TallyRep<T> (),
	fData ()
{
}

template	<class T>	size_t	Tally_ArrayRep<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class T>	Boolean	Tally_ArrayRep<T>::Contains (T item) const
{
	TallyEntry<T> tmp (item);
	return (Boolean (Find (tmp) != 0));
}

template	<class T>	void	Tally_ArrayRep<T>::Compact ()
{
	fData.Compact ();
}

template	<class T>	TallyRep<T>*	Tally_ArrayRep<T>::Clone () const
{
	return (new Tally_ArrayRep<T> (*this));
}

template	<class T>	void	Tally_ArrayRep<T>::Add (T item, size_t count)
{
	TallyEntry<T> tmp (item, count);

	size_t index = Find (tmp);
	if (index == 0) {
		fData.InsertAt (tmp, GetLength () + 1);
	}
	else {
		tmp.fCount += count;
		fData.SetAt (tmp, index);
	}
}

template	<class T>	void	Tally_ArrayRep<T>::Remove (T item, size_t count)
{
	TallyEntry<T> tmp (item);
	
	size_t index = Find (tmp);
	if (index != 0) {
		Assert (index <= GetLength ());
		Assert (tmp.fCount >= count);
		tmp.fCount -= count;
		if (tmp.fCount == 0) {
			RemoveAt (index);
		}
		else {
			fData.SetAt (tmp, index);
		}
	}
}

template	<class T>	void	Tally_ArrayRep<T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class T>	size_t	Tally_ArrayRep<T>::TallyOf (T item) const
{
	TallyEntry<T> tmp (item);

	size_t index = Find (tmp);
	Require (index != 0);
	Require (index <= GetLength ());
	return (tmp.fCount);
}

template	<class T>	IteratorRep<TallyEntry<T> >*	Tally_ArrayRep<T>::MakeTallyIterator ()
{
	return (new Tally_ArrayMutatorRep<T> (*this));
}

template	<class T>	TallyMutatorRep<T>*	Tally_ArrayRep<T>::MakeTallyMutator ()
{
	return (new Tally_ArrayMutatorRep<T> (*this));
}

template	<class T>	void	Tally_ArrayRep<T>::RemoveAt (size_t index)
{
	fData.RemoveAt (index);
}

template	<class T>	size_t	Tally_ArrayRep<T>::Find (TallyEntry<T>& item) const
{
	register	size_t length = fData.GetLength ();
	for (register size_t i = 1; i <= length; i++) {
		if (fData.GetAt (i).fItem == item.fItem) {
			item = fData.GetAt (i);
			return (i);
		}
	}
	return (0);
}







/*
 ********************************************************************************
 ********************************* Tally_Array **********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Tally_Array;	// tmp hack so GenClass will fixup following CTOR/DTORs
												// harmless, but silly

#endif

template	<class T>	Tally_Array<T>::Tally_Array () :
	Tally<T> (new Tally_ArrayRep<T> ())
{
}

template	<class T>	Tally_Array<T>::Tally_Array (const T* items, size_t size) :
	Tally<T> (new Tally_ArrayRep<T> ())
{
	SetSlotsAlloced (size);
	AddItems (items, size);
}

template	<class T>	Tally_Array<T>::Tally_Array (const Tally<T>& src) :
	Tally<T> (new Tally_ArrayRep<T> ())
{
	SetSlotsAlloced (src.GetLength ());
	operator+= (src);
}

template	<class T>	inline	const Tally_ArrayRep<T>*	Tally_Array<T>::GetRep () const
{
	return ((const Tally_ArrayRep<T>*) Tally<T>::GetRep ());
}

template	<class T>	inline	Tally_ArrayRep<T>*	Tally_Array<T>::GetRep ()
{
	return ((Tally_ArrayRep<T>*) Tally<T>::GetRep ());
}

template	<class T>	size_t	Tally_Array<T>::GetSlotsAlloced () const
{
	return (GetRep ()->fData.GetSlotsAlloced ());
}

template	<class T>	void	Tally_Array<T>::SetSlotsAlloced (size_t slotsAlloced)
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

