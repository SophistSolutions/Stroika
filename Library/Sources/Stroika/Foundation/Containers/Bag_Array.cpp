/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Bag_Array__cc
#define	__Bag_Array__cc

/*
 * $Header: /fuji/lewis/RCS/Bag_Array.cc,v 1.29 1992/12/03 20:41:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Bag_Array.cc,v $
 *		Revision 1.29  1992/12/03  20:41:43  lewis
 *		Remove DTOR for bagiteratorrep - default one is fine, and
 *		add comments.
 *
 *		Revision 1.28  1992/12/03  05:45:30  lewis
 *		Get rid of fFirstTime hack - now taken care of by Array iterators themselves.
 *		Use X(X&) in clone methods - this, coupled with new mutator capability in
 *		array to get rid of fOwner field.
 *
 *		Revision 1.27  1992/11/23  21:51:22  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.26  1992/11/20  19:21:58  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.25  1992/11/13  03:38:38  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *		And include BlockAllocated.
 *
 *		Revision 1.24  1992/11/12  08:18:41  lewis
 *		Fix placemnt of trailig #endif // This ifndef/define/endif stuff.. for
 *		GenClass quirks.
 *
 *		Revision 1.23  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.22  1992/11/07  16:30:37  lewis
 *		Try new arrayiteratorbase stuff suggested by kdj, and use
 *		in various mixins that use Array.
 *
 *		Revision 1.21  1992/11/05  15:24:15  lewis
 *		Bag_ArrayRep<T>::Replace now out of line since called before
 *		defined inline (breaks CFront 2.1).
 *
 *		Revision 1.20  1992/11/04  19:50:25  lewis
 *		Renamed Bag_ArrayIteratorRep->Bag_ArrayMutatorRep.
 *
 *		Revision 1.19  1992/11/01  01:26:35  lewis
 *		New Array Iterator support with separate subclass for patching.
 *
 *		Revision 1.18  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.17  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.16  1992/10/29  15:54:51  lewis
 *		Use qCFront_InlinesOpNewDeleteMustDeclareInline to work around MPW bug, and
 *		fix accidental return of void in iterator::next().
 *
 *		Revision 1.15  1992/10/22  18:15:34  lewis
 *		Rename BagRep_Array -> Bag_ArrayRep.
 *
 *		Revision 1.14  1992/10/22  04:10:17  lewis
 *		Fiddle with operator new stuff - hopefully get working.
 *		Got rid of old code from Remove code - old version.
 *
 *		Revision 1.13  1992/10/20  21:26:23  lewis
 *		Fixed typo in previous Remove fix.
 *		Add operator new/delete overloads for iterator and Rep class using
 *		BlockAllocated.
 *
 *		Revision 1.12  1992/10/20  17:57:50  lewis
 *		Comments.
 *		Use BackwardArrayIterator for Remove - instead of array indexing.
 *		Include Array.hh.
 *		Get rid of SetSlotsAllocated () call before array assignment since
 *		Array assignement takes care of that for us.
 *
 *		Revision 1.11  1992/10/16  22:53:14  lewis
 *		Include mem-initializers in BagRep_Array<T>::CTOR. Forgot them and
 *		left fIterators un-initialized.
 *
 *		Revision 1.10  1992/10/15  02:26:07  lewis
 *		Move BagRep_Array declaration here, and switched to new non-collectionview
 *		based stragegy.
 *
 *		Revision 1.9  1992/10/14  02:26:02  lewis
 *		Cleanup format - move all inlines to the bottom of the file in
 *		implementation section. Move declarations of concrete iterators
 *		(except bag.hh) to the .cc file.
 *
 *		Revision 1.8  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.7  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.6  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.5  1992/09/26  22:27:48  lewis
 *		Updated for new GenClass stuff - got to compile - havent tried linking.
 *
 *		Revision 1.4  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.3  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.2  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		
 *
 */

#include	"Array.hh"
#include	"Bag.hh"				// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"BlockAllocated.hh"
#include	"Debug.hh"

#include	"Bag_Array.hh"







#if		qRealTemplatesAvailable

	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifdef	inline
			#error "Ooops - I guess my workaround wont work!!!"
		#endif
		#define	inline
	#endif

template	<class T>	class	Bag_ArrayMutatorRep;
template	<class T>	class	Bag_ArrayRep : public BagRep<T> {
	public:
		Bag_ArrayRep ();
		~Bag_ArrayRep ();

		override	size_t		GetLength () const;
		override	Boolean		Contains (T item) const;
		override	void		Compact ();
		override	BagRep<T>*	Clone () const;

		override	IteratorRep<T>*		MakeIterator ();
		override	BagIteratorRep<T>*	MakeBagIterator ();
		override	BagMutatorRep<T>*	MakeBagMutator ();

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

	friend	class	Bag_Array<T>;
	friend	class	Bag_ArrayMutatorRep<T>;
};



template	<class T> class	Bag_ArrayMutatorRep : public BagMutatorRep<T> {
	public:
		Bag_ArrayMutatorRep (Bag_ArrayRep<T>& owner);

		override	Boolean	Done () const;
		override	Boolean	More ();
		override	T		Current () const;

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
	friend	class	Bag_ArrayRep<T>;
};





/*
 ********************************************************************************
 ******************************* Bag_ArrayMutatorRep ****************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfBag_ArrayMutatorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Bag_ArrayMutatorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Bag_ArrayMutatorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Bag_ArrayMutatorRep<T>::operator delete (void* p)
{
	BlockAllocated<Bag_ArrayMutatorRep<T> >::operator delete (p);
}

template	<class	T>	Bag_ArrayMutatorRep<T>::Bag_ArrayMutatorRep (Bag_ArrayRep<T>& owner) :
	BagMutatorRep<T> (),
	fIterator (owner.fData)
{
}

template	<class	T>	Boolean	Bag_ArrayMutatorRep<T>::Done () const
{
	return (fIterator.Done ());
}

template	<class	T>	Boolean	Bag_ArrayMutatorRep<T>::More ()
{
	return (fIterator.More ());
}

template	<class	T>	T	Bag_ArrayMutatorRep<T>::Current () const
{
	return (fIterator.Current());
}

template	<class T>	IteratorRep<T>*	Bag_ArrayMutatorRep<T>::Clone () const
{
	return (new Bag_ArrayMutatorRep<T> (*this));
}

template	<class	T>	void	Bag_ArrayMutatorRep<T>::RemoveCurrent ()
{
	fIterator.RemoveCurrent ();
}

template	<class	T>	void	Bag_ArrayMutatorRep<T>::UpdateCurrent (T newValue)
{
	fIterator.UpdateCurrent (newValue);
}






/*
 ********************************************************************************
 ********************************* Bag_ArrayRep *********************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfBag_ArrayRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Bag_ArrayRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Bag_ArrayRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Bag_ArrayRep<T>::operator delete (void* p)
{
	BlockAllocated<Bag_ArrayRep<T> >::operator delete (p);
}

template	<class T>	inline	Bag_ArrayRep<T>::Bag_ArrayRep () :
	fData ()
{
}

template	<class T>	Bag_ArrayRep<T>::~Bag_ArrayRep ()
{
}

template	<class T>	size_t	Bag_ArrayRep<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class T>	void	Bag_ArrayRep<T>::Compact ()
{
	fData.Compact ();
}

template	<class T> Boolean Bag_ArrayRep<T>::Contains (T item) const
{
	return (fData.Contains (item));
}

template	<class T>	IteratorRep<T>*	Bag_ArrayRep<T>:: MakeIterator ()
{
	return (new Bag_ArrayMutatorRep<T> (*this));
}

template	<class T>	BagIteratorRep<T>*	Bag_ArrayRep<T>::MakeBagIterator ()
{
	return (new Bag_ArrayMutatorRep<T> (*this));
}

template	<class T>	BagMutatorRep<T>*	Bag_ArrayRep<T>::MakeBagMutator ()
{
	return (new Bag_ArrayMutatorRep<T> (*this));
}

template	<class T>	void	Bag_ArrayRep<T>::Add (T item)
{
	fData.InsertAt (item, GetLength () + 1);
}

template	<class T>	void	Bag_ArrayRep<T>::Remove (T item)
{
	/*
	 *		Iterate backwards since removing from the end of an array
	 *	will be faster.
	 */
	for (BackwardArrayIterator<T> it (fData); it.More ();) {
		if (it.Current () == item) {
			fData.RemoveAt (it.CurrentIndex ());
			return;
		}
	}
}

template	<class T>	void	Bag_ArrayRep<T>::RemoveAll ()
{
	fData.RemoveAll ();
}






/*
 ********************************************************************************
 ************************************** Bag_Array *******************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Bag_Array;	// tmp hack so GenClass will fixup below array CTOR
											// harmless, but silly

#endif

template	<class T>	Bag_Array<T>::Bag_Array () :
	Bag<T> (new Bag_ArrayRep<T> ())
{
}

template	<class T>	Bag_Array<T>::Bag_Array (const Bag<T>& bag) :
	Bag<T> (new Bag_ArrayRep<T> ())
{
	SetSlotsAlloced (bag.GetLength ());
	operator+= (bag);
}

template	<class T>	Bag_Array<T>::Bag_Array (const T* items, size_t size) :
	Bag<T> (new Bag_ArrayRep<T> ())
{
	SetSlotsAlloced (size);
	AddItems (items, size);
}

template	<class T>	inline	const Bag_ArrayRep<T>*	Bag_Array<T>::GetRep () const
{
	/*
	 * This cast is safe since we are very careful to assure we always have a pointer
	 * to this type. We could keep extra pointer, but that would cost space and time,
	 * and with care we don't need to.
	 */
	return ((const Bag_ArrayRep<T>*) Bag<T>::GetRep ());
}

template	<class T>	inline	Bag_ArrayRep<T>*	Bag_Array<T>::GetRep ()
{
	/*
	 * This cast is safe since we are very careful to assure we always have a pointer
	 * to this type. We could keep extra pointer, but that would cost space and time,
	 * and with care we don't need to.
	 */
	return ((Bag_ArrayRep<T>*) Bag<T>::GetRep ());
}

template	<class T>	size_t	Bag_Array<T>::GetSlotsAlloced () const
{
	return (GetRep ()->fData.GetSlotsAlloced ());
}

template	<class T>	void	Bag_Array<T>::SetSlotsAlloced (size_t slotsAlloced)
{
	GetRep ()->fData.SetSlotsAlloced (slotsAlloced);
}

template	<class T>	BagRep<T>*	Bag_ArrayRep<T>::Clone () const
{
	return (new Bag_ArrayRep<T> (*this));
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

