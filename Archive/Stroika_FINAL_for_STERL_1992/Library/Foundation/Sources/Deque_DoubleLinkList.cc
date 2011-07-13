/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Deque_DoubleLinkList__cc
#define	__Deque_DoubleLinkList__cc

/*
 * $Header: /fuji/lewis/RCS/Deque_DoubleLinkList.cc,v 1.18 1992/12/10 07:13:34 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Deque_DoubleLinkList.cc,v $
 *		Revision 1.18  1992/12/10  07:13:34  lewis
 *		Updated for changes to rewritten DoubleLinkList.
 *
 *		Revision 1.17  1992/12/04  17:53:40  lewis
 *		Lots of name changes to make DQ API more reasonable.
 *		Also, got rid of op+= since ambiguous.
 *
 *		Revision 1.16  1992/12/03  19:18:56  lewis
 *		Add qBCC_InlinesBreakTemplates workaround.
 *
 *		Revision 1.15  1992/12/03  07:10:03  lewis
 *		Use new X(X&) in copies, and use compiler generated one
 *		rather than doing it ourselves manually. Use partially hacked
 *		new DoubleLinkList to get rid of fFirstTime stuff - now use new
 *		More syntax, etc.
 *		Get rid of unneeded fOwner.
 *
 *		Revision 1.14  1992/11/20  19:27:53  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.13  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.12  1992/11/03  22:52:41  lewis
 *		Switch over to new DoubleLinkList and DoubleLinkList_Patch_Forward, etc strategy
 *		for backends.
 *		Debugged (hopefully - not tested yet) iterator patching - based on what
 *		we did in LinkList that did pass testsuites.
 *
 *		Revision 1.11  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.10  1992/10/22  04:05:33  lewis
 *		Moved inlines to implementation section, moved concrete reps to .cc files.
 *		Got rid of unneeded methods (like GetReps in concrete class where
 *		not used). DequeRep no longer subclasses from CollectionRep - do
 *		in particular subclasses where needed.
 *
 *		Revision 1.9  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.8  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.7  1992/10/07  23:12:22  sterling
 *		Fixed typos in template implementation.
 *
 *		Revision 1.6  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.5  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg DequeRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.4  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.2  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *
 */


#include	"DoubleLinkList.hh"

#include	"Deque_DoubleLinkList.hh"




#if		qRealTemplatesAvailable

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class T>	class	Deque_DoubleLinkListIteratorRep;
template	<class T>	class	Deque_DoubleLinkListRep : public DequeRep<T> {
	public:
		Deque_DoubleLinkListRep ();

		override	DequeRep<T>*	Clone () const;
		override	size_t	GetLength () const;
		override	void	Compact ();
		override	void	RemoveAll ();
	 	override	IteratorRep<T>* MakeIterator ();

		override	void	AddHead (T item);
		override	T		RemoveHead ();
		override	T		Head () const;
		override	void	AddTail (T item);
		override	T		RemoveTail ();
		override	T		Tail () const;

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

	friend	class	Deque_DoubleLinkListIteratorRep<T>;
};

template	<class T> class	Deque_DoubleLinkListIteratorRep : public IteratorRep<T> {
	public:
		Deque_DoubleLinkListIteratorRep (Deque_DoubleLinkListRep<T>& owner);

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
		ForwardDoubleLinkListIterator_Patch<T>	fIterator;
	friend	class	Deque_DoubleLinkListRep<T>;
};






/*
 ********************************************************************************
 *************************** Deque_DoubleLinkListIteratorRep ********************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfDeque_DoubleLinkListIteratorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Deque_DoubleLinkListIteratorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Deque_DoubleLinkListIteratorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Deque_DoubleLinkListIteratorRep<T>::operator delete (void* p)
{
	BlockAllocated<Deque_DoubleLinkListIteratorRep<T> >::operator delete (p);
}

template	<class T>	Deque_DoubleLinkListIteratorRep<T>::Deque_DoubleLinkListIteratorRep (Deque_DoubleLinkListRep<T>& owner) :
	fIterator (owner.fData)
{
}

template	<class T>	Boolean	Deque_DoubleLinkListIteratorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class T>	Boolean	Deque_DoubleLinkListIteratorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class T>	T		Deque_DoubleLinkListIteratorRep<T>::Current () const
{
	return (fIterator.Current ());
}

template	<class T>	IteratorRep<T>*	Deque_DoubleLinkListIteratorRep<T>::Clone () const
{
	return (new Deque_DoubleLinkListIteratorRep<T> (*this));
}






/*
 ********************************************************************************
 ******************************* Deque_DoubleLinkListRep ************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfDeque_DoubleLinkListRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Deque_DoubleLinkListRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Deque_DoubleLinkListRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Deque_DoubleLinkListRep<T>::operator delete (void* p)
{
	BlockAllocated<Deque_DoubleLinkListRep<T> >::operator delete (p);
}

template	<class T>	inline	Deque_DoubleLinkListRep<T>::Deque_DoubleLinkListRep () :
	fData ()
{
}

template	<class T>	DequeRep<T>*	Deque_DoubleLinkListRep<T>::Clone () const
{
	return (new Deque_DoubleLinkListRep<T> (*this));
}

template	<class T>	size_t Deque_DoubleLinkListRep<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class T>	void Deque_DoubleLinkListRep<T>::Compact ()
{
}

template	<class T>	void Deque_DoubleLinkListRep<T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class T>	IteratorRep<T>*	Deque_DoubleLinkListRep<T>::MakeIterator ()
{
	return (new Deque_DoubleLinkListIteratorRep<T> (*this));
}

template	<class T>	void	Deque_DoubleLinkListRep<T>::AddHead (T item)
{
	fData.Prepend (item);
}

template	<class T>	T	Deque_DoubleLinkListRep<T>::RemoveHead ()
{
	Require (GetLength () != 0);
	T	result	=	fData.GetFirst ();
	fData.RemoveFirst ();
	return (result);
}

template	<class T>	T	Deque_DoubleLinkListRep<T>::Head () const
{
	Require (GetLength () != 0);
	return (fData.GetFirst ());
}

template	<class T>	void	Deque_DoubleLinkListRep<T>::AddTail (T item)
{
	fData.Append (item);
}

template	<class T>	T	Deque_DoubleLinkListRep<T>::RemoveTail ()
{
	Require (GetLength () != 0);
	T	result	=	fData.GetLast ();
	fData.RemoveLast ();
	return (result);
}

template	<class T>	T	Deque_DoubleLinkListRep<T>::Tail () const
{
	Require (GetLength () != 0);
	return (fData.GetLast ());
}






/*
 ********************************************************************************
 ********************************* Deque_DoubleLinkList *************************
 ********************************************************************************
 */
template	<class	T>	class	Deque_DoubleLinkList;	// Tmp hack so GenClass will fixup following CTOR/DTORs
														// Harmless, but silly.

template	<class T>	Deque_DoubleLinkList<T>::Deque_DoubleLinkList () :
	Deque<T> (new Deque_DoubleLinkListRep<T> ())
{
}

template	<class T>	Deque_DoubleLinkList<T>::Deque_DoubleLinkList (const Deque<T>& src) :
	Deque<T> (new Deque_DoubleLinkListRep<T> ())
{
	ForEach (T, it, src) {
		AddTail (it.Current ());
	}
}

template	<class T>	Deque_DoubleLinkList<T>::Deque_DoubleLinkList (const Deque_DoubleLinkList<T>& src) :
	Deque<T> (src)
{
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

