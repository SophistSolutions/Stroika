/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Mapping_LinkList__cc
#define	__Mapping_LinkList__cc

/*
 * $Header: /fuji/lewis/RCS/Mapping_LinkList.cc,v 1.14 1992/12/05 06:40:03 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Mapping_LinkList.cc,v $
 *		Revision 1.14  1992/12/05  06:40:03  lewis
 *		In workaround for qBCC_OpEqualsTemplateNotAutoExpanded, its important
 *		that Bogus()'s params be by value - not reference. This may be a key
 *		in understanding the pattern of brain-dead template behavior with BCC.
 *
 *		Revision 1.13  1992/12/04  20:15:37  lewis
 *		Work around qBCC_InlinesBreakTemplates, and qBCC_OpEqualsTemplateNotAutoExpanded.
 *
 *		Revision 1.12  1992/12/03  07:22:44  lewis
 *		Largly rewritten since LinkList now has greatly chagned API - no more
 *		direct access to link pointers - instead use mutators to append, and
 *		insert.
 *
 *		Revision 1.11  1992/11/20  19:27:53  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.10  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.9  1992/11/02  19:52:11  lewis
 *		Use new LinkList iterator support. Rename Reps. Use BlockAllocator.
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
 *		Revision 1.6  1992/10/13  05:32:12  lewis
 *		if !qRealTemplatesAvailable then #include LinkListOfMapElementOf<Key>_<T>_cc.
 *
 *		Revision 1.5  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.4  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.3  1992/10/07  23:24:03  sterling
 *		Fixed typo.
 *
 *		Revision 1.2  1992/09/26  03:50:48  lewis
 *		First cut at cleanups for new GenClass tool - untested yet.
 *
 *
 */



#include	"Debug.hh"
#include	"LinkList.hh"

#include	"Mapping_LinkList.hh"







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

template	<class Key, class T>	class	Mapping_LinkListIteratorRep;
template	<class Key, class T>	class	Mapping_LinkListRep : public MappingRep<Key,T> {
	public:
		Mapping_LinkListRep ();

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
		LinkList_Patch<MapElement<Key,T> >	fData;

	friend	class	Mapping_LinkListIteratorRep<Key,T>;
};



template	<class Key, class T>	class	Mapping_LinkListIteratorRep : public IteratorRep<MapElement<Key,T> > {
	public:
		Mapping_LinkListIteratorRep (Mapping_LinkListRep<Key,T>& owner);

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
		LinkListIterator_Patch<MapElement<Key,T> >	fIterator;
};










/*
 ********************************************************************************
 ****************************** Mapping_LinkListIteratorRep *********************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfMapping_LinkListIteratorRepOf<Key>_<T>_cc
#endif
template	<class Key, class T>	inline	void*	Mapping_LinkListIteratorRep<Key,T>::operator new (size_t size)
{
	return (BlockAllocated<Mapping_LinkListIteratorRep<Key,T> >::operator new (size));
}

template	<class Key, class T>	inline	void	Mapping_LinkListIteratorRep<Key,T>::operator delete (void* p)
{
	BlockAllocated<Mapping_LinkListIteratorRep<Key,T> >::operator delete (p);
}

template	<class Key, class T>	Mapping_LinkListIteratorRep<Key,T>::Mapping_LinkListIteratorRep (Mapping_LinkListRep<Key,T>& owner) :
	fIterator (owner.fData)
{
}

template	<class Key, class T> Boolean	Mapping_LinkListIteratorRep<Key,T>::Done () const
{
	return (fIterator.Done());
}

template	<class Key, class T> Boolean	Mapping_LinkListIteratorRep<Key,T>::More ()
{
	return (fIterator.More());
}

template	<class Key, class T>	MapElement<Key,T>	Mapping_LinkListIteratorRep<Key,T>::Current () const
{
	return (fIterator.Current ());
}

template	<class Key, class T>	IteratorRep<MapElement<Key,T> >*	Mapping_LinkListIteratorRep<Key,T>::Clone () const
{
	return (new Mapping_LinkListIteratorRep<Key,T> (*this));
}






/*
 ********************************************************************************
 ********************************* Mapping_LinkListRep **************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfMapping_LinkListRepOf<Key>_<T>_cc
#endif
template	<class Key, class T>	inline	void*	Mapping_LinkListRep<Key,T>::operator new (size_t size)
{
	return (BlockAllocated<Mapping_LinkListRep<Key,T> >::operator new (size));
}

template	<class Key, class T>	inline	void	Mapping_LinkListRep<Key,T>::operator delete (void* p)
{
	BlockAllocated<Mapping_LinkListRep<Key,T> >::operator delete (p);
}

template	<class Key, class T>	Mapping_LinkListRep<Key,T>::Mapping_LinkListRep () :
	fData ()
{
}

template	<class Key, class T>	MappingRep<Key,T>*	Mapping_LinkListRep<Key,T>::Clone () const
{
	return (new Mapping_LinkListRep<Key,T> (*this));
}

template	<class Key, class T> size_t	Mapping_LinkListRep<Key,T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class Key, class T> void	Mapping_LinkListRep<Key,T>::Compact ()
{
}

template	<class Key, class T> void	Mapping_LinkListRep<Key,T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class Key, class T> Boolean	Mapping_LinkListRep<Key,T>::Lookup (Key key, T* eltResult) const
{
	RequireNotNil (eltResult);
	for (LinkListIterator<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fKey == key) {
			*eltResult = cur.fElt;
			return (True);
		}
	}
	return (False);
}

template	<class Key, class T> void	Mapping_LinkListRep<Key,T>::Enter (Key key, T elt)
{
	for (LinkListMutator_Patch<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fKey == key) {
			it.UpdateCurrent (MapElement<Key,T> (key, elt));
			return;
		}
	}
	Assert (it.Done ());
	it.AddBefore (MapElement<Key,T> (key, elt));		// AddBefore() end means Append...
}

template	<class Key, class T> void	Mapping_LinkListRep<Key,T>::Remove (T elt)
{
	for (LinkListMutator_Patch<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fElt == elt) {
			it.RemoveCurrent ();
			break;
		}
	}
}

template	<class Key, class T> void	Mapping_LinkListRep<Key,T>::RemoveAt (Key key)
{
	for (LinkListMutator_Patch<MapElement<Key,T> > it (fData); it.More (); ) {
		MapElement<Key,T>	cur	=	it.Current();
		if (cur.fKey == key) {
			it.RemoveCurrent ();
			return;
		}
	}
	RequireNotReached ();		// An error to remove by key and have it fail???
}

template	<class Key, class T> IteratorRep<MapElement<Key,T> >* Mapping_LinkListRep<Key,T>::MakeElementIterator ()
{
	return (new Mapping_LinkListIteratorRep<Key,T> (*this));
}






/*
 ********************************************************************************
 ********************************* Mapping_LinkList *****************************
 ********************************************************************************
 */

template	<class Key, class T>	class	Mapping_LinkList;	// Tmp hack so GenClass will fixup following CTOR/DTORs
																// Harmless, but silly.

template	<class Key, class T> Mapping_LinkList<Key,T>::Mapping_LinkList () :
	Mapping<Key,T> (new Mapping_LinkListRep<Key,T> ())
{
}

template	<class Key, class T> Mapping_LinkList<Key,T>::Mapping_LinkList (const Mapping<Key,T>& src) :
	Mapping<Key,T> (new Mapping_LinkListRep<Key,T> ())
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

