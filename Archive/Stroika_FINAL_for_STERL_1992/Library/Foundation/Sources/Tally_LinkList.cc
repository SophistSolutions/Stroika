/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Tally_LinkList__cc
#define	__Tally_LinkList__cc

/*
 * $Header: /fuji/lewis/RCS/Tally_LinkList.cc,v 1.15 1992/12/04 20:13:57 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Tally_LinkList.cc,v $
 *		Revision 1.15  1992/12/04  20:13:57  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.14  1992/12/03  05:50:15  lewis
 *		Mostly rewritten using new linklist stuff which gives us no visible
 *		Link ptr. Use more iterators instead. Much shorter and simpler.Use X(X&) in clone methods - this, coupled with new mutator capability in
 *		array to get rid of fOwner field.
 *
 *		Revision 1.13  1992/11/20  19:19:58  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.12  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.11  1992/11/02  19:52:47  lewis
 *		Use new LinkList iterator support. Rename Reps. Use BlockAllocator.
 *
 *		Revision 1.10  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.9  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.8  1992/10/22  14:55:25  lewis
 *		Fixed typos - got testsuites to compile.
 *
 *		Revision 1.7  1992/10/19  21:42:07  lewis
 *		Move inlines from class declaration to implemenation details section at end
 *		of header, and moved concrete reps to .cc file. Added #includes where needed
 *		for GenClass stuff. Other minor cleanups - untested yet.
 *
 *		Revision 1.6  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.5  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.4  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.3  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.2  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *
 */



#include	"Debug.hh"
#include	"LinkList.hh"

#include	"Tally_LinkList.hh"







#if		qRealTemplatesAvailable

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	LinkListOfTallyEntryOf<T>_cc
#endif

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class T>	class	Tally_LinkListMutatorRep;
template	<class T>	class	Tally_LinkListRep : public TallyRep<T> {
	public:
		Tally_LinkListRep ();
		Tally_LinkListRep (const Tally_LinkListRep<T>& from);

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

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

	private:
		LinkList_Patch<TallyEntry<T> >	fData;

	friend	class	Tally_LinkListMutatorRep<T>;
};


template	<class T>	class	Tally_LinkListMutatorRep : public TallyMutatorRep<T> {
	public:
		Tally_LinkListMutatorRep (Tally_LinkListRep<T>& owner);
		Tally_LinkListMutatorRep (const Tally_LinkListMutatorRep<T>& from);

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
		LinkListMutator_Patch<TallyEntry<T> >	fIterator;
};








/*
 ********************************************************************************
 ****************************** Tally_LinkListMutatorRep ************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfTally_LinkListMutatorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Tally_LinkListMutatorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Tally_LinkListMutatorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Tally_LinkListMutatorRep<T>::operator delete (void* p)
{
	BlockAllocated<Tally_LinkListMutatorRep<T> >::operator delete (p);
}

template	<class T>	Tally_LinkListMutatorRep<T>::Tally_LinkListMutatorRep (Tally_LinkListRep<T>& owner) :
	fIterator (owner.fData)
{
}

template	<class T>	Tally_LinkListMutatorRep<T>::Tally_LinkListMutatorRep (const Tally_LinkListMutatorRep<T>& from) :
	fIterator (from.fIterator)
{
}

template	<class T>	Boolean	Tally_LinkListMutatorRep<T>::Done () const
{
	return (fIterator.Done ());
}

template	<class T>	Boolean	Tally_LinkListMutatorRep<T>::More ()
{
	return (fIterator.More ());
}

template	<class T>	TallyEntry<T>	Tally_LinkListMutatorRep<T>::Current () const
{
	return (fIterator.Current ());
}

template	<class T>	IteratorRep<TallyEntry<T> >*	Tally_LinkListMutatorRep<T>::Clone () const
{
	return (new Tally_LinkListMutatorRep<T> (*this));
}

template	<class T> void	Tally_LinkListMutatorRep<T>::RemoveCurrent ()
{
	fIterator.RemoveCurrent ();
}

template	<class T> void	Tally_LinkListMutatorRep<T>::UpdateCount (size_t newCount)
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
 ********************************* Tally_LinkListRep ****************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfTally_LinkListRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Tally_LinkListRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Tally_LinkListRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Tally_LinkListRep<T>::operator delete (void* p)
{
	BlockAllocated<Tally_LinkListRep<T> >::operator delete (p);
}

template	<class T>	inline	Tally_LinkListRep<T>::Tally_LinkListRep () :
	fData ()
{
}

template	<class T>	inline	Tally_LinkListRep<T>::Tally_LinkListRep (const Tally_LinkListRep<T>& from) :
	fData (from.fData)
{
}

template	<class T> size_t	Tally_LinkListRep<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class T> Boolean	Tally_LinkListRep<T>::Contains (T item) const
{
	for (LinkListIterator<TallyEntry<T> > it (fData); it.More (); ) {
		TallyEntry<T>	c	=	it.Current ();
		if (c.fItem == item) {
			Assert (c.fCount != 0);
			return (True);
		}
	}
	return (False);
}

template	<class T> void	Tally_LinkListRep<T>::Compact ()
{
}

template	<class T> TallyRep<T>*	Tally_LinkListRep<T>::Clone () const
{
	return (new Tally_LinkListRep<T> (*this));
}

template	<class T> void	Tally_LinkListRep<T>::Add (T item, size_t count)
{
	if (count != 0) {
		for (LinkListMutator_Patch<TallyEntry<T> > it (fData); it.More (); ) {
			TallyEntry<T>	c	=	it.Current ();
			if (c.fItem == item) {
				c.fCount += count;
				it.UpdateCurrent (c);
				return;
			}
		}
		fData.Prepend (TallyEntry<T> (item, count));
	}
}

template	<class T> void	Tally_LinkListRep<T>::Remove (T item, size_t count)
{
	if (count != 0) {
		for (LinkListMutator_Patch<TallyEntry<T> > it (fData); it.More (); ) {
			TallyEntry<T>	c	=	it.Current ();
			if (c.fItem == item) {
				if (c.fCount > count) {
					c.fCount -= count;
				}
				else {
					c.fCount = 0;		// Should this be an underflow excpetion, assertion???
				}
				if (c.fCount == 0) {
					it.RemoveCurrent ();
				}
				else {
					it.UpdateCurrent (c);
				}
				break;
			}
		}
	}
}

template	<class T> void	Tally_LinkListRep<T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class T> size_t	Tally_LinkListRep<T>::TallyOf (T item) const
{
	for (LinkListIterator<TallyEntry<T> > it (fData); it.More (); ) {
		TallyEntry<T>	c	=	it.Current ();
		if (c.fItem == item) {
			Ensure (c.fCount != 0);
			return (c.fCount);
		}
	}
	return (0);
}

template	<class T> IteratorRep<TallyEntry<T> >*	Tally_LinkListRep<T>::MakeTallyIterator ()
{
	return (new Tally_LinkListMutatorRep<T> (*this));
}

template	<class T> TallyMutatorRep<T>*	Tally_LinkListRep<T>::MakeTallyMutator ()
{
	return (new Tally_LinkListMutatorRep<T> (*this));
}




/*
 ********************************************************************************
 ********************************* Tally_LinkList *******************************
 ********************************************************************************
 */
template	<class	T>	class	Tally_LinkList;	// Tmp hack so GenClass will fixup following CTOR/DTORs
												// Harmless, but silly.

template	<class T> Tally_LinkList<T>::Tally_LinkList () :
	Tally<T> (new Tally_LinkListRep<T> ())
{
}

template	<class T> Tally_LinkList<T>::Tally_LinkList (const T* items, size_t size) :
	Tally<T> (new Tally_LinkListRep<T> ())
{
	AddItems (items, size);
}

template	<class T> Tally_LinkList<T>::Tally_LinkList (const Tally<T>& src) :
	Tally<T> (new Tally_LinkListRep<T> ())
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

