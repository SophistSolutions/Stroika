/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Mapping_SortedArray__cc
#define	__Mapping_SortedArray__cc

/*
 * $Header: /fuji/lewis/RCS/Mapping_SortedArray.cc,v 1.18 1992/12/05 17:37:40 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Mapping_SortedArray.cc,v $
 *		Revision 1.18  1992/12/05  17:37:40  lewis
 *		Added workaround for qBCC_OpEqualsTemplateNotAutoExpanded.
 *
 *		Revision 1.17  1992/12/04  20:15:58  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.16  1992/12/03  07:25:03  lewis
 *		Use new mutator stuff much more internally instead of indexed
 *		array access.
 *		Get rid of fFirstTime hack - now take care of for us by new
 *		Array API.
 *		use new X(X&) to Clone() now.
 *
 *		Revision 1.15  1992/11/20  19:29:12  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.14  1992/11/15  03:57:50  lewis
 *		Add template declare for Mapping_SortedArrayIteratorRep; since used as friend.
 *
 *		Revision 1.13  1992/11/13  03:46:40  lewis
 *		#include "BlockAllocated.hh"
 *
 *		Revision 1.12  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.11  1992/11/07  16:30:37  lewis
 *		Try new arrayiteratorbase stuff suggested by kdj, and use
 *		in various mixins that use Array.
 *
 *		Revision 1.10  1992/11/05  22:13:55  lewis
 *		Made Mapping_SortedArrayIteratorRep::CTOR(CTOR&) const ref instead.
 *
 *		Revision 1.9  1992/11/01  01:26:35  lewis
 *		New Array Iterator support with separate subclass for patching.
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
 *		Revision 1.6  1992/10/14  05:38:19  lewis
 *		Don't include ArrayOfMapElementOf<Key>_<T>_cc since causes linker
 *		errors under UNIX cuz included allready.
 *
 *		Revision 1.5  1992/10/13  18:52:53  lewis
 *		if !qTemplatesAvailable then #include Array of MapElements.
 *
 *		Revision 1.4  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.3  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.2  1992/09/26  03:50:48  lewis
 *		First cut at cleanups for new GenClass tool - untested yet.
 *
 *		
 *
 */

#include	"Array.hh"
#include	"BlockAllocated.hh"
#include	"Debug.hh"

#include	"Mapping_SortedArray.hh"







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

template	<class Key, class T> class	Mapping_SortedArrayIteratorRep;
template	<class Key, class T> class	Mapping_SortedArrayRep : public MappingRep<Key,T> {
	public:
		Mapping_SortedArrayRep ();

#if		qRealTemplatesAvailable && qBCC_OpEqualsTemplateNotAutoExpanded
		// See similar hack on Mapping_Array
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

	friend	class	Mapping_SortedArray<Key,T>;
	friend	class	Mapping_SortedArrayIteratorRep<Key,T>;
};




template	<class Key, class T>	class	Mapping_SortedArrayIteratorRep : public IteratorRep<MapElement<Key,T> > {
	public:
		Mapping_SortedArrayIteratorRep (Mapping_SortedArrayRep<Key,T>& owner);

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
 *************************** Mapping_SortedArrayIteratorRep *********************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfMapping_SortedArrayIteratorRepOf<Key>_<T>_cc
#endif
template	<class Key, class T> inline	void*	Mapping_SortedArrayIteratorRep<Key,T>::operator new (size_t size)
{
	return (BlockAllocated<Mapping_SortedArrayIteratorRep<Key,T> >::operator new (size));
}

template	<class Key, class T> inline	void	Mapping_SortedArrayIteratorRep<Key,T>::operator delete (void* p)
{
	BlockAllocated<Mapping_SortedArrayIteratorRep<Key,T> >::operator delete (p);
}

template	<class Key, class T> Mapping_SortedArrayIteratorRep<Key,T>::Mapping_SortedArrayIteratorRep (Mapping_SortedArrayRep<Key,T>& owner) :
	IteratorRep<MapElement<Key,T> > (),
	fIterator (owner.fData)
{
}

template	<class Key, class T> Boolean	Mapping_SortedArrayIteratorRep<Key,T>::Done () const
{
	return (fIterator.Done());
}

template	<class Key, class T> Boolean	Mapping_SortedArrayIteratorRep<Key,T>::More ()
{
	return (fIterator.More());
}

template	<class Key, class T> MapElement<Key,T>	Mapping_SortedArrayIteratorRep<Key,T>::Current () const
{
	return (fIterator.Current());
}

template	<class Key, class T> IteratorRep<MapElement<Key,T> >*	Mapping_SortedArrayIteratorRep<Key,T>::Clone () const
{
	return (new Mapping_SortedArrayIteratorRep<Key,T> (*this));
}








/*
 ********************************************************************************
 **************************** Mapping_SortedArrayRep ****************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfMapping_SortedArrayRepOf<Key>_<T>_cc
#endif
template	<class Key, class T> inline	void*	Mapping_SortedArrayRep<Key,T>::operator new (size_t size)
{
	return (BlockAllocated<Mapping_SortedArrayRep<Key,T> >::operator new (size));
}

template	<class Key, class T> inline	void	Mapping_SortedArrayRep<Key,T>::operator delete (void* p)
{
	BlockAllocated<Mapping_SortedArrayRep<Key,T> >::operator delete (p);
}

template	<class Key, class T> inline	Mapping_SortedArrayRep<Key,T>::Mapping_SortedArrayRep ()
{
}

template	<class Key, class T> MappingRep<Key,T>*	Mapping_SortedArrayRep<Key,T>::Clone () const
{
	return (new Mapping_SortedArrayRep<Key,T> (*this));
}

template	<class Key, class T> size_t	Mapping_SortedArrayRep<Key,T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class Key, class T> void	Mapping_SortedArrayRep<Key,T>::Compact ()
{
	fData.Compact ();
}

template	<class Key, class T> void	Mapping_SortedArrayRep<Key,T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class Key, class T> Boolean	Mapping_SortedArrayRep<Key,T>::Lookup (Key key, T* eltResult) const
{
	RequireNotNil (eltResult);
	for (ForwardArrayIterator<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fKey == key) {
			*eltResult = cur.fElt;
			return (True);
		}
		if (not (cur.fKey < key)) {
			return (False);
		}
	}
	return (False);
}

template	<class Key, class T> void	Mapping_SortedArrayRep<Key,T>::Enter (Key key, T elt)
{
	for (ForwardArrayMutator_Patch<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fKey == key) {
			it.UpdateCurrent (MapElement<Key,T> (key, elt));
			return;
		}
		if (not (cur.fKey < key)) {		// ie if we went past...
			break;
		}
	}
	/*
	 *		If we got here, then our item is > it.Current (or all items),
	 *	and should be appended.
	 */
	it.AddBefore (MapElement<Key,T> (key, elt));
}

template	<class Key, class T> void	Mapping_SortedArrayRep<Key,T>::Remove (T elt)
{
	for (ForwardArrayMutator_Patch<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fElt == elt) {
			it.RemoveCurrent ();
			break;
		}
	}
}

template	<class Key, class T> void	Mapping_SortedArrayRep<Key,T>::RemoveAt (Key key)
{
	for (ForwardArrayMutator_Patch<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fKey == key) {
			it.RemoveCurrent ();
			break;
		}
		if (not (cur.fKey < key)) {		// ie if we went past...
			RequireNotReached ();		// An error to remove by key and have it fail???
		}
	}
}

template	<class Key, class T> IteratorRep<MapElement<Key,T> >* Mapping_SortedArrayRep<Key,T>::MakeElementIterator ()
{
	return (new Mapping_SortedArrayIteratorRep<Key,T> (*this));
}





/*
 ********************************************************************************
 ******************************* Mapping_SortedArray ****************************
 ********************************************************************************
 */
template	<class Key, class T>	class	Mapping_SortedArray;	// Tmp hack so GenClass will fixup following CTOR/DTORs
																	// Harmless, but silly.


template	<class Key, class T> Mapping_SortedArray<Key,T>::Mapping_SortedArray () :
	Mapping<Key,T> (new Mapping_SortedArrayRep<Key,T> ())
{
}

template	<class Key, class T> Mapping_SortedArray<Key,T>::Mapping_SortedArray (const Mapping<Key,T>& src) :
	Mapping<Key,T> (new Mapping_SortedArrayRep<Key,T> ())
{
	SetSlotsAlloced (src.GetLength ());
	operator+= (src);
}

template	<class Key, class T> inline	const Mapping_SortedArrayRep<Key,T>*	Mapping_SortedArray<Key,T>::GetRep () const
{
	return ((const Mapping_SortedArrayRep<Key,T>*) Mapping<Key,T>::GetRep ());
}

template	<class Key, class T> inline	Mapping_SortedArrayRep<Key,T>*	Mapping_SortedArray<Key,T>::GetRep ()
{
	return ((Mapping_SortedArrayRep<Key,T>*) Mapping<Key,T>::GetRep ());
}

template	<class Key, class T> size_t	Mapping_SortedArray<Key,T>::GetSlotsAlloced () const
{
	return (GetRep ()->fData.GetSlotsAlloced ());
}

template	<class Key, class T> void	Mapping_SortedArray<Key,T>::SetSlotsAlloced (size_t slotsAlloced)
{
	GetRep ()->fData.SetSlotsAlloced (slotsAlloced);
}


#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifndef	inline
		#error	"How did it get undefined?"
	#endif
	#undef	inline
#endif


#if		!qRealTemplatesAvailable && 0
	// Don't actually include this - to use Mapping_SortedArrayRep, you
	// must use Mapping which uses Mapping_Array which already pulls this in,
	// and you'd get linker errors under UNIX.
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

