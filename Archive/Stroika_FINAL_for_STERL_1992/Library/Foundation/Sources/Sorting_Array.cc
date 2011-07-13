/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Sorting_Array__cc
#define	__Sorting_Array__cc

/*
 * $Header: /fuji/lewis/RCS/Sorting_Array.cc,v 1.18 1992/12/03 19:20:31 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Sorting_Array.cc,v $
 *		Revision 1.18  1992/12/03  19:20:31  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.17  1992/12/03  05:47:37  lewis
 *		Get rid of fFirstTime hack - now taken care of by Array iterators themselves.
 *		Use X(X&) in clone methods - this, coupled with new mutator capability in
 *		array to get rid of fOwner field.
 *		Add qGCC_ConversionOperatorCausesHangBug
 *		Rename Skrunch->Compact.
 *
 *		Revision 1.16  1992/11/20  19:30:24  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.15  1992/11/13  03:51:07  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *		Also, be more careful about !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
 *		hacks.
 *		Add missing template      <class T>.
 *		#include      "BlockAllocated.hh"
 *
 *		Revision 1.14  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.13  1992/11/07  16:30:37  lewis
 *		Try new arrayiteratorbase stuff suggested by kdj, and use
 *		in various mixins that use Array.
 *
 *		Revision 1.12  1992/11/05  22:13:22  lewis
 *		Made Sorting_ArrayIteratorRep<T>::Sorting_ArrayIteratorRep(Sorting_ArrayIteratorRep&)
 *		const Ref instead.
 *
 *		Revision 1.11  1992/11/01  01:37:36  lewis
 *		New Array Iterator support with separate subclass for patching.
 *
 *		Revision 1.10  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.9  1992/10/15  21:10:48  lewis
 *		Move inlines to implementation section. Moved concrete rep
 *		classes to .cc files, and related inlines. #include SharedRep.hh for
 *		non-templated GenClass stuff. Got to compile (not tested yet) with
 *		GenClass.
 *
 *		Revision 1.8  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.7  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.6  1992/10/02  04:15:19  lewis
 *		Stuff to try to get GenClass working.
 *
 *		Revision 1.5  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.4  1992/09/23  13:45:49  lewis
 *		Fixed bad assert in Add.
 *
 *		Revision 1.3  1992/09/21  04:28:13  lewis
 *		fix assert in Add method - was backwards.
 *
 *		Revision 1.2  1992/09/20  01:23:08  lewis
 *		Applied lots of Patches to get rid of Collection base class, and
 *		converted to using < instead of <=.
 *
 *
 */



#include	"Array.hh"
#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"Sorting.hh"				// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Sorting_Array.hh"







#if		qRealTemplatesAvailable


#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class T>	class	Sorting_ArrayIteratorRep;

template	<class T>	class	Sorting_ArrayRep : public SortingRep<T> {
	public:
		Sorting_ArrayRep ();

		override	size_t	GetLength () const;
		override	Boolean	Contains (T item) const;
	 	override	IteratorRep<T>* MakeIterator ();
		override	void	Compact ();
		override	SortingRep<T>*	Clone () const;

		override	void	Add (T item);
		override	void	Remove (T item);
		override	void	RemoveAll ();

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

	private:
		Array_Patch<T>	fData;

	friend	class	Sorting_Array<T>;
	friend	class	Sorting_ArrayIteratorRep<T>;
};


template	<class T> class	Sorting_ArrayIteratorRep : public IteratorRep<T> {
	public:
		Sorting_ArrayIteratorRep (Sorting_ArrayRep<T>& owner);

		override	Boolean	Done () const;
		override	Boolean	More ();
		override	T		Current () const;

		override	IteratorRep<T>*		Clone () const;

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

	private:
		ForwardArrayIterator_Patch<T>	fIterator;
};








/*
 ********************************************************************************
 ******************************* Sorting_ArrayIteratorRep ***********************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfSorting_ArrayIteratorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Sorting_ArrayIteratorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Sorting_ArrayIteratorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Sorting_ArrayIteratorRep<T>::operator delete (void* p)
{
	BlockAllocated<Sorting_ArrayIteratorRep<T> >::operator delete (p);
}

template	<class	T>	Sorting_ArrayIteratorRep<T>::Sorting_ArrayIteratorRep (Sorting_ArrayRep<T>& owner) :
	IteratorRep<T> (),
	fIterator (owner.fData)
{
}

template	<class	T>	Boolean	Sorting_ArrayIteratorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class	T>	Boolean	Sorting_ArrayIteratorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class	T>	T	Sorting_ArrayIteratorRep<T>::Current () const
{
	return (fIterator.Current());
}

template	<class T>	IteratorRep<T>*	Sorting_ArrayIteratorRep<T>::Clone () const
{
	return (new Sorting_ArrayIteratorRep<T> (*this));
}






/*
 ********************************************************************************
 ********************************* Sorting_ArrayRep *****************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfSorting_ArrayRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Sorting_ArrayRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Sorting_ArrayRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Sorting_ArrayRep<T>::operator delete (void* p)
{
	BlockAllocated<Sorting_ArrayRep<T> >::operator delete (p);
}

template	<class T>	inline	Sorting_ArrayRep<T>::Sorting_ArrayRep ()
{
}

template	<class T>	size_t	Sorting_ArrayRep<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class T>	void	Sorting_ArrayRep<T>::Compact ()
{
	fData.Compact ();
}

template	<class T>	SortingRep<T>*	Sorting_ArrayRep<T>::Clone () const
{
	return (new Sorting_ArrayRep<T> (*this));
}

template	<class T> Boolean Sorting_ArrayRep<T>::Contains (T item) const
{
	return (fData.Contains (item));
}

template	<class T>	IteratorRep<T>*	Sorting_ArrayRep<T>::MakeIterator ()
{
	return (new Sorting_ArrayIteratorRep<T> (*this));		// for now assume forwards....
}

template	<class T>	void	Sorting_ArrayRep<T>::Add (T item)
{
	// Insert the item at the first position where it is greater than or
	// equal to an item in the array (possibly at the end)...
	for (size_t index = 1; index <= GetLength (); index++) {
		if (item < fData[index]) {
			fData.InsertAt (item, index);
			return;
		}
		Assert (not (item < fData[index]));
	}
	Assert (index == GetLength () + 1);
	fData.InsertAt (item, index);
}

template	<class T>	void	Sorting_ArrayRep<T>::Remove (T item)
{
	register	size_t length = fData.GetLength ();
	for (size_t i = 1; i <= length; i++) {
		if (fData[i] == item) {
			fData.RemoveAt (i);
			return;
		}
	}
}

template	<class T>	void	Sorting_ArrayRep<T>::RemoveAll ()
{
	fData.RemoveAll ();
}




/*
 ********************************************************************************
 ********************************* Sorting_Array ********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Sorting_Array;	// tmp hack so GenClass will fixup below array CTOR
												// harmless, but silly

#endif

template	<class T>	Sorting_Array<T>::Sorting_Array ():
	Sorting<T> (new Sorting_ArrayRep<T> ())
{
}

template	<class T>	Sorting_Array<T>::Sorting_Array (const Sorting<T>& sorting):
	Sorting<T> (new Sorting_ArrayRep<T> ())
{
	SetSlotsAlloced (sorting.GetLength ());
#if 	qGCC_ConversionOperatorCausesHangBug
	Add ((Iterator<T>)sorting);
#else
	Add (sorting);
#endif
}

template	<class T>	Sorting_Array<T>::Sorting_Array (const T* items, size_t size):
	Sorting<T> (new Sorting_ArrayRep<T> ())
{
	SetSlotsAlloced (size);
	AddItems (items, size);
}

template	<class T>	inline	const Sorting_ArrayRep<T>*	Sorting_Array<T>::GetRep () const
{
	return ((const Sorting_ArrayRep<T>*) Sorting<T>::GetRep ());
}

template	<class T>	inline	Sorting_ArrayRep<T>*	Sorting_Array<T>::GetRep ()
{
	return ((Sorting_ArrayRep<T>*) Sorting<T>::GetRep ());
}

template	<class T>	size_t	Sorting_Array<T>::GetSlotsAlloced () const
{
	return (GetRep ()->fData.GetSlotsAlloced ());
}

template	<class T>	void	Sorting_Array<T>::SetSlotsAlloced (size_t slotsAlloced)
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

