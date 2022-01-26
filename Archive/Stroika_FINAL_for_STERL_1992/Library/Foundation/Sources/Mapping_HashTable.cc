/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Mapping_HashTable__cc
#define	__Mapping_HashTable__cc

/*
 * $Header: /fuji/lewis/RCS/Mapping_HashTable.cc,v 1.22 1992/12/04 20:15:16 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Mapping_HashTable.cc,v $
 *		Revision 1.22  1992/12/04  20:15:16  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.21  1992/12/03  07:22:14  lewis
 *		Override Done() since it was added as virtual method to IteratorRep.
 *
 *		Revision 1.20  1992/11/20  19:29:48  lewis
 *		Support new it.More() stuff by adjusting how our iterator works
 *
 *		Revision 1.19  1992/11/15  04:16:36  lewis
 *		Use explicit Iterator<T> instead of typedef as workaround for
 *		ForEach() of something with comma problem, since CFront 3.0 barfs
 *		on typedefs in template functions.
 *
 *		Revision 1.18  1992/11/13  03:46:08  lewis
 *		Work around new problem with templates, and MapElement<a,b> ForEach()
 *		macro.
 *
 *		Revision 1.17  1992/11/12  08:24:05  lewis
 *		Fix template declaration (missing template part).
 *		Also, hack workaround for ForEach (MappingEleemnt<Key,T>, ...) problem.
 *
 *		Revision 1.16  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.15  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.14  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.13  1992/10/23  17:18:58  lewis
 *		Moved inlines to implemenation section, and concrete reps to the .cc files.
 *		Other minor cleanups for GenClass. Got rid of CollectionRep as base
 *		class of MappingRep and moved it to each concrete rep.
 *
 *		Revision 1.12  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.11  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.10  1992/10/07  23:22:46  sterling
 *		Lots of changes - LGP checked in???
 *
 *		Revision 1.9  1992/10/02  04:15:19  lewis
 *		Stuff to try to get GenClass working.
 *
 *		Revision 1.8  1992/09/26  03:50:48  lewis
 *		First cut at cleanups for new GenClass tool - untested yet.
 *
 *		Revision 1.7  1992/09/21  06:19:45  sterling
 *		Sterl made lots of changes, and I just blindly checked them in
 *		loosing my g++ workaround. Will have to encounter it again (LGP checkedin).
 *
 *		Revision 1.6  1992/09/20  01:16:11  lewis
 *		Explicitly cast away const in two usages of ForEach macros here,
 *		cuz g++ fails to compile it otherwise. Should consider better
 *		alternatives, however.
 *
 *		Revision 1.5  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.4  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.3  1992/09/05  12:22:07  lewis
 *		Include "String.hh"
 *
 *		Revision 1.2  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 */

#include	"Debug.hh"
#include	"HashTable.hh"
#include	"String.hh"

#include	"Mapping_HashTable.hh"


#if		!qIncluding_Mapping_HashTableCC
size_t	DefaultStringHashFunction (const String& key)
{
	register	size_t	result		=	0;
	UInt32						maxLength	= key.GetLength ();
	for (register size_t i = 1; i <= maxLength; i++) {
// Gross hack - throw in bogus mod - later rewrite to take modulo as
// argument...
		result = (result*64 + key[i].GetAsciiCode ()) % 1093943;
	}
	return (result);
}
#endif



#if		qRealTemplatesAvailable

#include	"Collection.hh"

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class Key, class T>	class	Mapping_HashTableElement {
	public:
		Mapping_HashTableElement (MapElement<Key,T> entry, Mapping_HashTableElement<Key,T>* next) :
			fEntry (entry),
			fNext (next)
		{
		}

		MapElement<Key,T>					fEntry;
		Mapping_HashTableElement<Key,T>*	fNext;
};
		
template	<class Key, class T>	class	MappingRep_HashTable : public MappingRep <Key,T>, public CollectionRep {
	public:
		MappingRep_HashTable (size_t (*hashFunction) (const Key&), size_t hashTableSize);
		~MappingRep_HashTable ();

		override	size_t		GetLength () const;
		override	void				Compact ();
		override	MappingRep<Key,T>*	Clone () const;

		override	Boolean	Lookup (Key key, T* elt) const;
		override	void	Enter (Key key, T elt);
		override	void	Remove (T elt);
		override	void	RemoveAt (Key key);
		override	void	RemoveAll ();

	 	override	IteratorRep<MapElement<Key,T> >* MakeElementIterator ();
		
		/* return 0 .. fTableSize-1 */
		nonvirtual	size_t	Hash (const Key& key) const;		
		
		size_t						fItemCount;		// number of entries in table
		size_t 						(*fHashFunction) (const Key&);
		size_t						fTableSize;		// number of slots in table
		Mapping_HashTableElement<Key,T>**	fTable;
};

template	<class Key, class T> class	Mapping_HTIterator : public IteratorRep <MapElement<Key,T> >, public CollectionView {
	public:
		Mapping_HTIterator<Key,T> (const MappingRep_HashTable<Key,T>& owner);

		override	Boolean				Done () const;
		override	Boolean				More ();
		override	MapElement<Key,T>	Current () const;

		override	IteratorRep<MapElement<Key,T> >*		Clone () const;

	private:
		const MappingRep_HashTable<Key,T>&	fOwner;
		Mapping_HashTableElement<Key,T>*	fCurrent;
		size_t								fCurrentTableHead;
		Boolean								fCurDeleted;

		override	void	PatchAdd (CollectionItemKey key);
		override	void	PatchRemove (CollectionItemKey key);
		override	void	PatchRemoveAll ();

		nonvirtual	void	SlogAlongToNext ();
};




/*
 ********************************************************************************
 ******************************** MappingRep_HashTable **************************
 ********************************************************************************
 */
template	<class Key, class T>	MappingRep_HashTable<Key,T>::MappingRep_HashTable (
			size_t (*hashFunction) (const Key&), 
			size_t hashTableSize) :
	fItemCount (0),
	fHashFunction (hashFunction),
	fTableSize (hashTableSize),
	fTable (Nil)
{
	Require (hashTableSize != 0);
	RequireNotNil (hashFunction);
	fTable = new Mapping_HashTableElement<Key,T>*	[fTableSize];
	for (size_t i = 0; i < fTableSize; i++) {
		fTable[i] = Nil;
	}
}

template	<class Key, class T>	MappingRep_HashTable<Key,T>::~MappingRep_HashTable ()
{
	if (fTable != Nil) {
		RemoveAll ();
	}
	delete fTable;
}

template	<class Key, class T>	size_t	MappingRep_HashTable<Key,T>::Hash (const Key& key) const
{
	RequireNotNil (fHashFunction);
	Assert (fTableSize != 0);
	size_t	result		=	(*fHashFunction) (key)	% fTableSize;
	Ensure (result >= 0);
	Ensure (result < fTableSize);
	return (result);
}

template	<class Key, class T>	size_t	MappingRep_HashTable<Key,T>::GetLength () const
{
	return (fItemCount);
}

template	<class Key, class T>	void	MappingRep_HashTable<Key,T>::Compact ()
{
	/*
	 * For now, we do nothing, though there are some things we might imagine doing, like
	 * rehashing...
	 */
}

template	<class Key, class T>	MappingRep<Key,T>*	MappingRep_HashTable<Key,T>::Clone () const
{
	MappingRep_HashTable<Key,T>* rep = new MappingRep_HashTable<Key,T> (fHashFunction, fTableSize);

	/*ForGCC 2.2.1 we must stick in cast here- isthere a better way?*/
#if		qRealTemplatesAvailable && 1
	for (Iterator<MapElement<Key,T> > it (((MappingRep_HashTable<Key,T>*)this)->MakeElementIterator ()); it.More ();)
#else
	ForEach (MapElement<Key,T>, it, ((MappingRep_HashTable<Key,T>*)this)->MakeElementIterator ())
#endif
	{
		rep->Enter (it.Current ().fKey, it.Current ().fElt);
	}
	return (rep);
}

template	<class Key, class T>	Boolean	MappingRep_HashTable<Key,T>::Lookup (Key key, T* elt) const
{
	RequireNotNil (elt);
	RequireNotNil (fTable);
	for (register Mapping_HashTableElement<Key,T>* curT = fTable [Hash (key)]; curT != Nil; curT = curT->fNext) {
		if (curT->fEntry.fKey == key) {
			*elt = curT->fEntry.fElt;
			return (True);
		}
	}
	return (False);
}

template	<class Key, class T>	void	MappingRep_HashTable<Key,T>::Enter (Key key, T elt)
{
	RequireNotNil (fTable);
	size_t	hashVal = Hash (key);
	for (register Mapping_HashTableElement<Key,T>* curT = fTable [hashVal]; curT != Nil; curT = curT->fNext) {
		if (curT->fEntry.fKey == key) {
			curT->fEntry.fElt = elt;
			return;
		}
	}
	Mapping_HashTableElement<Key,T>*	patched = new Mapping_HashTableElement<Key,T> (MapElement<Key,T> (key, elt), fTable [hashVal]);
	fTable [hashVal] = patched;
	fItemCount++;
	PatchViewsAdd (patched);
}

template	<class Key, class T>	void	MappingRep_HashTable<Key,T>::Remove (T elt)
{
	RequireNotNil (fTable);
	for (size_t i = 0; i < fTableSize; i++) {
		Mapping_HashTableElement<Key,T>* prev = Nil;
		for (register Mapping_HashTableElement<Key,T>* curT = fTable [i]; curT != Nil; prev = curT, curT = curT->fNext) {
			if (curT->fEntry.fElt == elt) {
				PatchViewsRemove (curT);

				if (prev == Nil) {
					fTable[i] = curT->fNext;
				}
				else {
					AssertNotNil (prev);
					prev->fNext = curT->fNext;
				}
				Mapping_HashTableElement<Key,T>*	patched = curT;
				PatchViewsRemove (curT);
				delete (curT);
				fItemCount--;
				return;
			}
		}
	}
}

template	<class Key, class T>	void	MappingRep_HashTable<Key,T>::RemoveAt (Key key)
{
	RequireNotNil (fTable);
	size_t	hashVal = Hash (key);
	Mapping_HashTableElement<Key,T>* prev = Nil;
	for (register Mapping_HashTableElement<Key,T>* curT = fTable [hashVal]; curT != Nil; prev = curT, curT = curT->fNext) {
		if (curT->fEntry.fKey == key) {
			if (prev == Nil) {
				fTable[hashVal] = curT->fNext;
			}
			else {
				AssertNotNil (prev);
				prev->fNext = curT->fNext;
			}
			Mapping_HashTableElement<Key,T>*	patched = curT;
			PatchViewsRemove (curT);
			delete (curT);
			fItemCount--;
			return;
		}
	}
	RequireNotReached ();
}

template	<class Key, class T>	void	MappingRep_HashTable<Key,T>::RemoveAll ()
{
	PatchViewsRemoveAll ();
	for (register size_t i = 0; i < fTableSize; i++) {
		for (Mapping_HashTableElement<Key,T>* elt = fTable [i]; elt != Nil; ) {
			Mapping_HashTableElement<Key,T>*	delElt	=	elt;
			elt = elt->fNext;
			delete (delElt);
		}
		fTable [i] = Nil;
	}
	fItemCount = 0;
}

template	<class Key, class T>	IteratorRep<MapElement<Key,T> >*	MappingRep_HashTable<Key,T>::MakeElementIterator  ()
{
	return (new Mapping_HTIterator<Key,T> (*this));
}








/*
 ********************************************************************************
 *********************************** Mapping_HTIterator *************************
 ********************************************************************************
 */
template	<class Key, class T> Mapping_HTIterator<Key,T>::Mapping_HTIterator (const MappingRep_HashTable<Key,T>& owner):
	IteratorRep <MapElement<Key,T> > (),
	CollectionView (owner),
	fOwner (owner),
	fCurrent (Nil),
	fCurrentTableHead (0),
	fCurDeleted (False)
{
}

template	<class Key, class T> Boolean	Mapping_HTIterator<Key,T>::Done () const
{
	return (Boolean (fCurrent == Nil));
}

template	<class Key, class T> Boolean	Mapping_HTIterator<Key,T>::More ()
{
	if (fCurDeleted) {
		fCurDeleted = False;
	}
	else {
		SlogAlongToNext ();
	}
	return (Boolean (fCurrent != Nil));
}

template	<class Key, class T> MapElement<Key,T>	Mapping_HTIterator<Key,T>::Current () const
{
	RequireNotNil (fCurrent);	// cuz means were done!
	return (fCurrent->fEntry);
}

template	<class Key, class T>	IteratorRep<MapElement<Key,T> >*	Mapping_HTIterator<Key,T>::Clone () const
{
	Mapping_HTIterator<Key,T>*	clone	=	new Mapping_HTIterator<Key,T> (fOwner);
	clone->fCurrent = fCurrent;
	clone->fCurrentTableHead = fCurrentTableHead;
	clone->fCurDeleted = fCurDeleted;
	return (clone);
}

template	<class Key, class T>	void	Mapping_HTIterator<Key,T>::SlogAlongToNext ()
{
	/*
	 * Work our way down each chain. When we hit the end of a chain, then start the
	 * next chain. Special case at beginning, with fCurrent = Nil - otherwise this
	 * can only happen when we are done.
	 */
	if (fCurrent != Nil) {
		fCurrent = fCurrent->fNext;
	}
	/*
	 * At this point, either we're just starting out (fCurrent==Nil),
	 * or were at the end of a chain (fCurrent==Nil), or we've found
	 * our next guy (fCurrent != Nil).
	 */
	if (fCurrent == Nil) {
		while (fCurrentTableHead < fOwner.fTableSize) {
			fCurrent = fOwner.fTable[fCurrentTableHead];
			fCurrentTableHead++;
			if (fCurrent != Nil) {
				return;
			}
		}
	}
}

template	<class Key, class T> void	Mapping_HTIterator<Key,T>::PatchAdd (CollectionItemKey /*key*/)
{
	/*
	 * Pretty clear there is nothing to do, except in the case where we are adding
	 * an item to the head of a chain, and we are currently on that head - but in
	 * fact, we dont even have to worry about that case, since we store a pointer to
	 * the element we are on, and will just skip the newly added guy (and more importantly
	 * are guanrateed not to revisit the node we are on, because of an add).
	 */
}

template	<class Key, class T> void	Mapping_HTIterator<Key,T>::PatchRemove (CollectionItemKey key)
{
	if (key != Nil) {
		const	Mapping_HashTableElement<Key,T>*	cur	=	(const	Mapping_HashTableElement<Key,T>*)key;
		if (cur == fCurrent) {
			SlogAlongToNext ();
			fCurDeleted = True;
		}
	}
	else {
		fCurrent = Nil;	// we interprete Nil as removeall (hack city)
	}
}

template	<class Key, class T> void	Mapping_HTIterator<Key,T>::PatchRemoveAll ()
{
	fCurrent = Nil;
	fCurrentTableHead = 0;
	fCurDeleted = True;
}





/*
 ********************************************************************************
 ************************************ Mapping_HashTable *************************
 ********************************************************************************
 */

template	<class Key, class T>	class	Mapping_HashTable;	// Tmp hack so GenClass will fixup following CTOR/DTORs
																// Harmless, but silly.

template	<class Key, class T>	Mapping_HashTable<Key,T>::Mapping_HashTable (
			size_t (*hashFunction) (const Key&),  size_t hashTableSize) :
	Mapping<Key,T> (new MappingRep_HashTable<Key,T> (hashFunction, hashTableSize))
{
}

template	<class Key, class T>	Mapping_HashTable<Key,T>::Mapping_HashTable (
			const Mapping<Key,T>& map, size_t (*hashFunction) (const Key&),  size_t hashTableSize) :
	Mapping<Key,T> (new MappingRep_HashTable<Key,T> (hashFunction, hashTableSize))
{
	operator+= (map);
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

