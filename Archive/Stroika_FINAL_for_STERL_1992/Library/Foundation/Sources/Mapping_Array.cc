/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Mapping_Array__cc
#define	__Mapping_Array__cc

/*
 * $Header: /fuji/lewis/RCS/Mapping_Array.cc,v 1.19 1992/12/05 06:40:03 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Mapping_Array.cc,v $
 *		Revision 1.19  1992/12/05  06:40:03  lewis
 *		In workaround for qBCC_OpEqualsTemplateNotAutoExpanded, its important
 *		that Bogus()'s params be by value - not reference. This may be a key
 *		in understanding the pattern of brain-dead template behavior with BCC.
 *
 *		Revision 1.18  1992/12/04  20:14:49  lewis
 *		Work around qBCC_InlinesBreakTemplates and qBCC_OpEqualsTemplateNotAutoExpanded.
 *
 *		Revision 1.17  1992/12/03  05:49:02  lewis
 *		Get rid of fFirstTime hack - now taken care of by Array iterators themselves.
 *		Use X(X&) in clone methods - this, coupled with new mutator capability in
 *		array to get rid of fOwner field.
 *		Rename Skrunch->Compact.
 *		Use array provided iterators and mutators internally more instead of old
 *		indexing forloops.
 *
 *		Revision 1.16  1992/11/20  19:27:53  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.15  1992/11/13  03:41:53  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *		Include Blockallocated.hh.
 *		template predeclare cuz used in freind.
 *
 *		Revision 1.14  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.13  1992/11/07  16:30:37  lewis
 *		Try new arrayiteratorbase stuff suggested by kdj, and use
 *		in various mixins that use Array.
 *
 *		Revision 1.12  1992/11/05  15:22:16  lewis
 *		Make Mapping_ArrayIteratorRep::CTOR(CTOR&) be const reference.
 *
 *		Revision 1.11  1992/11/01  01:26:35  lewis
 *		New Array Iterator support with separate subclass for patching.
 *
 *		Revision 1.10  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.8  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.7  1992/10/23  17:18:58  lewis
 *		Moved inlines to implemenation section, and concrete reps to the .cc files.
 *		Other minor cleanups for GenClass. Got rid of CollectionRep as base
 *		class of MappingRep and moved it to each concrete rep.
 *
 *		Revision 1.6  1992/10/13  18:23:53  lewis
 *		Add include of Array of MappingElt.cc
 *
 *		Revision 1.5  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.4  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.3  1992/10/02  21:48:18  lewis
 *		Added <Key,T> arg to CTOR name in template.
 *
 *		Revision 1.2  1992/09/26  03:50:48  lewis
 *		First cut at cleanups for new GenClass tool - untested yet.
 *
 *
 */



#include	"Array.hh"
#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"Mapping.hh"			// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository

#include	"Mapping_Array.hh"







#if		qRealTemplatesAvailable

#if		!qRealTemplatesAvailable
		#include	"TFileMap.hh"
		#include	ArrayOfMapElementOf<Key>_<T>_hh
#endif

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class Key, class T>	class	Mapping_ArrayIteratorRep;

template	<class Key, class T>	class	Mapping_ArrayRep : public MappingRep<Key,T> {
	public:
		Mapping_ArrayRep ();

#if		qRealTemplatesAvailable && qBCC_OpEqualsTemplateNotAutoExpanded
		// Without this hack, get error expanding op= in Array::Contains, and elsewhere..
		Boolean	Bogus(MapElement<Key,T> lhs, MapElement<Key,T> rhs) { return (lhs == rhs); }
#endif

		override	MappingRep<Key,T>*	Clone () const;
		override	size_t	GetLength () const;
		override	void	Compact ();
	 	override	void	RemoveAll ();

		override	Boolean	Lookup (Key key, T* eltResult) const;
		override	void	Enter (Key key, T newElt);
		override	void	Remove (T elt);
		override	void	RemoveAt (Key key);

		override	IteratorRep<MapElement<Key,T> >* MakeElementIterator ();

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

	private:
		Array_Patch<MapElement<Key,T> >	fData;

	friend	class	Mapping_Array<Key,T>;
	friend	class	Mapping_ArrayIteratorRep<Key,T>;
};


template	<class Key, class T>	class	Mapping_ArrayIteratorRep : public IteratorRep<MapElement<Key,T> > {
	public:
		Mapping_ArrayIteratorRep (Mapping_ArrayRep<Key,T>& owner);

		override	Boolean				Done () const;
		override	Boolean				More ();
		override	MapElement<Key,T>	Current () const;

		override	IteratorRep<MapElement<Key,T> >*	Clone () const;

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

	private:
		ForwardArrayIterator_Patch<MapElement<Key,T> >	fIterator;
};




/*
 ********************************************************************************
 ******************************* Mapping_ArrayIteratorRep ***********************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfMapping_ArrayIteratorRepOf<Key>_<T>_cc
#endif
template	<class Key, class T> inline	void*	Mapping_ArrayIteratorRep<Key,T>::operator new (size_t size)
{
	return (BlockAllocated<Mapping_ArrayIteratorRep<Key,T> >::operator new (size));
}

template	<class Key, class T> inline	void	Mapping_ArrayIteratorRep<Key,T>::operator delete (void* p)
{
	BlockAllocated<Mapping_ArrayIteratorRep<Key,T> >::operator delete (p);
}

template	<class Key, class T> Mapping_ArrayIteratorRep<Key,T>::Mapping_ArrayIteratorRep (Mapping_ArrayRep<Key,T>& owner) :
	IteratorRep<MapElement<Key,T> > (),
	fIterator (owner.fData)
{
}

template	<class Key, class T> Boolean	Mapping_ArrayIteratorRep<Key,T>::Done () const
{
	return (fIterator.Done());
}

template	<class Key, class T> Boolean	Mapping_ArrayIteratorRep<Key,T>::More ()
{
	return (fIterator.More());
}

template	<class Key, class T> MapElement<Key,T>	Mapping_ArrayIteratorRep<Key,T>::Current () const
{
	return (fIterator.Current());
}

template	<class Key, class T> IteratorRep<MapElement<Key,T> >*	Mapping_ArrayIteratorRep<Key,T>::Clone () const
{
	return (new Mapping_ArrayIteratorRep<Key,T> (*this));
}







/*
 ********************************************************************************
 ********************************* Mapping_ArrayRep *****************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfMapping_ArrayRepOf<Key>_<T>_cc
#endif
template	<class Key, class T> inline	void*	Mapping_ArrayRep<Key,T>::operator new (size_t size)
{
	return (BlockAllocated<Mapping_ArrayRep<Key,T> >::operator new (size));
}

template	<class Key, class T> inline	void	Mapping_ArrayRep<Key,T>::operator delete (void* p)
{
	BlockAllocated<Mapping_ArrayRep<Key,T> >::operator delete (p);
}

template	<class Key, class T> inline	Mapping_ArrayRep<Key,T>::Mapping_ArrayRep () :
	fData ()
{
}

template	<class Key, class T> MappingRep<Key,T>*	Mapping_ArrayRep<Key,T>::Clone () const
{
	return (new Mapping_ArrayRep<Key,T> (*this));
}

template	<class Key, class T> size_t	Mapping_ArrayRep<Key,T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class Key, class T> void	Mapping_ArrayRep<Key,T>::Compact ()
{
	fData.Compact ();
}

template	<class Key, class T> void	Mapping_ArrayRep<Key,T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class Key, class T> Boolean	Mapping_ArrayRep<Key,T>::Lookup (Key key, T* eltResult) const
{
	RequireNotNil (eltResult);
	for (ForwardArrayIterator<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fKey == key) {
			*eltResult = cur.fElt;
			return (True);
		}
	}
	return (False);
}

template	<class Key, class T> void	Mapping_ArrayRep<Key,T>::Enter (Key key, T elt)
{
	for (ForwardArrayMutator<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fKey == key) {
			it.UpdateCurrent (MapElement<Key,T> (key, elt));
			return;
		}
	}
	fData.InsertAt (MapElement<Key,T> (key, elt), fData.GetLength () + 1);
}

template	<class Key, class T> void	Mapping_ArrayRep<Key,T>::Remove (T elt)
{
	for (ForwardArrayMutator_Patch<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fElt == elt) {
			it.RemoveCurrent ();
			break;
		}
	}
}

template	<class Key, class T> void	Mapping_ArrayRep<Key,T>::RemoveAt (Key key)
{
	for (ForwardArrayMutator_Patch<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fKey == key) {
			it.RemoveCurrent ();
			return;
		}
	}
	RequireNotReached ();		// An error to remove by key and have it fail???
}

template	<class Key, class T> IteratorRep<MapElement<Key,T> >* Mapping_ArrayRep<Key,T>::MakeElementIterator ()
{
	return (new Mapping_ArrayIteratorRep<Key,T> (*this));
}





/*
 ********************************************************************************
 ************************************ Mapping_Array *****************************
 ********************************************************************************
 */

template	<class Key, class T>	class	Mapping_Array;	// Tmp hack so GenClass will fixup following CTOR/DTORs
															// Harmless, but silly.

template	<class Key, class T> Mapping_Array<Key,T>::Mapping_Array () :
	Mapping<Key,T> (new Mapping_ArrayRep<Key,T> ())
{
}

template	<class Key, class T>	Mapping_Array<Key,T>::Mapping_Array (const Mapping<Key,T>& src) :
	Mapping<Key,T> (new Mapping_ArrayRep<Key,T> ())
{
	SetSlotsAlloced (src.GetLength ());
	operator+= (src);
}

template	<class Key, class T> inline	const Mapping_ArrayRep<Key,T>*	Mapping_Array<Key,T>::GetRep () const
{
	return ((const Mapping_ArrayRep<Key,T>*) Mapping<Key,T>::GetRep ());
}

template	<class Key, class T> inline	Mapping_ArrayRep<Key,T>*	Mapping_Array<Key,T>::GetRep ()
{
	return ((Mapping_ArrayRep<Key,T>*) Mapping<Key,T>::GetRep ());
}

template	<class Key, class T> size_t	Mapping_Array<Key,T>::GetSlotsAlloced () const
{
	return (GetRep ()->fData.GetSlotsAlloced ());
}

template	<class Key, class T> void	Mapping_Array<Key,T>::SetSlotsAlloced (size_t slotsAlloced)
{
	GetRep ()->fData.SetSlotsAlloced (slotsAlloced);
}

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifndef	inline
		#error	"How did it get undefined?"
	#endif
	#undef	inline
#endif

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	ArrayOfMapElementOf<Key>_<T>_cc
#endif

#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

