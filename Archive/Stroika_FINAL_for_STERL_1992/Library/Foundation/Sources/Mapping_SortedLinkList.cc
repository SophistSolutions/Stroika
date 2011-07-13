/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Mapping_SortedLinkList__cc
#define	__Mapping_SortedLinkList__cc

/*
 * $Header: /fuji/lewis/RCS/Mapping_SortedLinkList.cc,v 1.13 1992/12/04 20:16:19 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Mapping_SortedLinkList.cc,v $
 *		Revision 1.13  1992/12/04  20:16:19  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.12  1992/12/03  07:26:28  lewis
 *		Mostly rewritten since LinkList no longer gives direct
 *		access to link ptrs. Use Mutators instead.
 *		fFirstTime hack gone - LinkList takes care of for us now.
 *		Use new X(X&) to Clone now. Let compiler autogen X(X&) instead
 *		of doing manually.
 *
 *		Revision 1.11  1992/11/20  19:29:12  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.10  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.9  1992/11/03  17:09:20  lewis
 *		In Enter () method, call AddAfter () with the right index.
 *
 *		Revision 1.8  1992/11/02  19:52:18  lewis
 *		Use new LinkList iterator support. Rename Reps. Use BlockAllocator.
 *
 *		Revision 1.7  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.6  1992/10/23  17:18:58  lewis
 *		Moved inlines to implemenation section, and concrete reps to the .cc files.
 *		Other minor cleanups for GenClass. Got rid of CollectionRep as base
 *		class of MappingRep and moved it to each concrete rep.
 *
 *		Revision 1.5  1992/10/13  20:40:21  lewis
 *		if !qRealTemplatesAvailable then #include LinkList of MapElt.
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
 */

#include	"Debug.hh"
#include	"LinkList.hh"

#include	"Mapping_SortedLinkList.hh"





#if		qRealTemplatesAvailable

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	LinkListOfMapElementOf<Key>_<T>_cc
#endif

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class Key, class T>	class	Mapping_SortedLinkListIteratorRep;
template	<class Key, class T>	class	Mapping_SortedLinkListRep : public MappingRep<Key,T> {
	public:
		Mapping_SortedLinkListRep ();

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
		LinkList_Patch<MapElement<Key,T> >	fData;

	friend	class	Mapping_SortedLinkListIteratorRep<Key,T>;
};





template	<class Key, class T>	class	Mapping_SortedLinkListIteratorRep : public IteratorRep<MapElement<Key,T> > {
	public:
		Mapping_SortedLinkListIteratorRep (Mapping_SortedLinkListRep<Key,T>& owner);

		override	Boolean	Done () const;
		override	Boolean	More ();
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
		LinkListIterator_Patch<MapElement<Key,T> >	fIterator;
};






/*
 ********************************************************************************
 *************************** Mapping_SortedLinkListIteratorRep ******************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfMapping_SortedLinkListIteratorRepOf<Key>_<T>_cc
#endif
template	<class Key, class T> inline	void*	Mapping_SortedLinkListIteratorRep<Key,T>::operator new (size_t size)
{
	return (BlockAllocated<Mapping_SortedLinkListIteratorRep<Key,T> >::operator new (size));
}

template	<class Key, class T> inline	void	Mapping_SortedLinkListIteratorRep<Key,T>::operator delete (void* p)
{
	BlockAllocated<Mapping_SortedLinkListIteratorRep<Key,T> >::operator delete (p);
}

template	<class Key, class T> Mapping_SortedLinkListIteratorRep<Key,T>::Mapping_SortedLinkListIteratorRep (Mapping_SortedLinkListRep<Key,T>& owner) :
	fIterator (owner.fData)
{
}

template	<class Key, class T> Boolean	Mapping_SortedLinkListIteratorRep<Key,T>::Done () const
{
	return (fIterator.Done());
}

template	<class Key, class T> Boolean	Mapping_SortedLinkListIteratorRep<Key,T>::More ()
{
	return (fIterator.More());
}

template	<class Key, class T> MapElement<Key,T>	Mapping_SortedLinkListIteratorRep<Key,T>::Current () const
{
	return (fIterator.Current ());
}

template	<class Key, class T> IteratorRep<MapElement<Key,T> >*	Mapping_SortedLinkListIteratorRep<Key,T>::Clone () const
{
	return (new Mapping_SortedLinkListIteratorRep<Key,T> (*this));
}








/*
 ********************************************************************************
 ***************************** Mapping_SortedLinkListRep ************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfMapping_SortedLinkListRepOf<Key>_<T>_cc
#endif
template	<class Key, class T> inline	void*	Mapping_SortedLinkListRep<Key,T>::operator new (size_t size)
{
	return (BlockAllocated<Mapping_SortedLinkListRep<Key,T> >::operator new (size));
}

template	<class Key, class T> inline	void	Mapping_SortedLinkListRep<Key,T>::operator delete (void* p)
{
	BlockAllocated<Mapping_SortedLinkListRep<Key,T> >::operator delete (p);
}

template	<class Key, class T> inline	Mapping_SortedLinkListRep<Key,T>::Mapping_SortedLinkListRep () :
	fData ()
{
}

template	<class Key, class T> MappingRep<Key,T>*	Mapping_SortedLinkListRep<Key,T>::Clone () const
{
	return (new Mapping_SortedLinkListRep<Key,T> (*this));
}

template	<class Key, class T> size_t	Mapping_SortedLinkListRep<Key,T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class Key, class T> void	Mapping_SortedLinkListRep<Key,T>::Compact ()
{
}

template	<class Key, class T> void	Mapping_SortedLinkListRep<Key,T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class Key, class T> Boolean	Mapping_SortedLinkListRep<Key,T>::Lookup (Key key, T* eltResult) const
{
	RequireNotNil (eltResult);
	for (LinkListIterator<MapElement<Key,T> > it (fData); it.More (); ) {
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

template	<class Key, class T> void	Mapping_SortedLinkListRep<Key,T>::Enter (Key key, T elt)
{
	for (LinkListMutator_Patch<MapElement<Key,T> > it (fData); it.More (); ) {
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

template	<class Key, class T> void	Mapping_SortedLinkListRep<Key,T>::Remove (T elt)
{
	for (LinkListMutator_Patch<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fElt == elt) {
			it.RemoveCurrent ();
			break;
		}
	}
}

template	<class Key, class T> void	Mapping_SortedLinkListRep<Key,T>::RemoveAt (Key key)
{
	for (LinkListMutator_Patch<MapElement<Key,T> > it (fData); it.More (); ) {
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

template	<class Key, class T> IteratorRep<MapElement<Key,T> >* Mapping_SortedLinkListRep<Key,T>::MakeElementIterator ()
{
	return (new Mapping_SortedLinkListIteratorRep<Key,T> (*this));
}




/*
 ********************************************************************************
 ***************************** Mapping_SortedLinkList ***************************
 ********************************************************************************
 */
template	<class Key, class T>	class	Mapping_SortedLinkList;	// Tmp hack so GenClass will fixup following CTOR/DTORs
																	// Harmless, but silly.

template	<class Key, class T> Mapping_SortedLinkList<Key,T>::Mapping_SortedLinkList () :
	Mapping<Key,T> (new Mapping_SortedLinkListRep<Key,T> ())
{
}

template	<class Key, class T> Mapping_SortedLinkList<Key,T>::Mapping_SortedLinkList (const Mapping<Key,T>& src) :
	Mapping<Key,T> (new Mapping_SortedLinkListRep<Key,T> ())
{
	operator+= (src);
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

