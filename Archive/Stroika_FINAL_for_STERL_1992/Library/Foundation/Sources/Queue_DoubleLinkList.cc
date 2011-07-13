/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Queue_DoubleLinkList__cc
#define	__Queue_DoubleLinkList__cc

/*
 * $Header: /fuji/lewis/RCS/Queue_DoubleLinkList.cc,v 1.19 1992/12/10 07:13:50 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Queue_DoubleLinkList.cc,v $
 *		Revision 1.19  1992/12/10  07:13:50  lewis
 *		Updated for changes to rewritten DoubleLinkList.
 *
 *		Revision 1.18  1992/12/04  16:16:25  lewis
 *		Fixed errand qBCC_InlinesBreakTemplates workaround.
 *		Reordered and renamed a bunch of Q methods - general Enqueue became AddTai
 *		and Dequeue became RemoveHead().
 *
 *		Revision 1.17  1992/12/03  19:19:43  lewis
 *		Add workaround for qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.16  1992/12/03  07:17:46  lewis
 *		Use default generated (no longer provide our own) X(X&) for reps,
 *		and use these in cloning. Converted (partially) DoubleLinkList to new
 *		More() API, and use got rid of fFirstTime hack. Use new API.
 *		Override Done() in mutator.
 *
 *		Revision 1.15  1992/11/20  19:27:53  lewis
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
 *		Revision 1.10  1992/10/15  13:22:53  lewis
 *		Moved inlines in class declaration down to implemenation detail section,
 *		moved concrete rep classes to .cc file, and related inlines.
 *		Got rid of CollectionRep from QueueRep, and so mix in in each concrete
 *		rep class now.
 *
 *		Revision 1.9  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.8  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.7  1992/10/07  23:26:26  sterling
 *		Minor bugfix - (lgp checkedin).
 *
 *		Revision 1.6  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
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
 *		Revision 1.3  1992/09/11  16:35:55  sterling
 *		use new Shared implementation
 *
 *		Revision 1.2  1992/09/04  21:06:46  sterling
 *		Moved some inlines out of line here, and added Contains ().
 *
 *
 *
 */

#include	"DoubleLinkList.hh"

#include	"Queue_DoubleLinkList.hh"




#if		qRealTemplatesAvailable

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class T>	class	Queue_DoubleLinkListIteratorRep;
template	<class T>	class	Queue_DoubleLinkListRep : public QueueRep<T> {
	public:
		Queue_DoubleLinkListRep ();

		override	QueueRep<T>*	Clone () const;
		override	size_t	GetLength () const;
		override	void	RemoveAll ();
		override	void	Compact ();
	 	override	IteratorRep<T>* MakeIterator ();

		override	void	AddTail (T item);
		override	T		RemoveHead ();
		override	T		Head () const;

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

	friend	class	Queue_DoubleLinkListIteratorRep<T>;
};



template	<class T> class	Queue_DoubleLinkListIteratorRep : public IteratorRep<T> {
	public:
		Queue_DoubleLinkListIteratorRep (Queue_DoubleLinkListRep<T>& owner);

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
	friend	class	Queue_DoubleLinkListRep<T>;
};






/*
 ********************************************************************************
 *************************** Queue_DoubleLinkListIteratorRep ********************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfQueue_DoubleLinkListIteratorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Queue_DoubleLinkListIteratorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Queue_DoubleLinkListIteratorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Queue_DoubleLinkListIteratorRep<T>::operator delete (void* p)
{
	BlockAllocated<Queue_DoubleLinkListIteratorRep<T> >::operator delete (p);
}

template	<class T>	Queue_DoubleLinkListIteratorRep<T>::Queue_DoubleLinkListIteratorRep (Queue_DoubleLinkListRep<T>& owner) :
	fIterator (owner.fData)
{
}
template	<class T>	Boolean	Queue_DoubleLinkListIteratorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class T>	Boolean	Queue_DoubleLinkListIteratorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class T>	T		Queue_DoubleLinkListIteratorRep<T>::Current () const
{
	return (fIterator.Current ());
}

template	<class T>	IteratorRep<T>*	Queue_DoubleLinkListIteratorRep<T>::Clone () const
{
	return (new Queue_DoubleLinkListIteratorRep<T> (*this));
}





/*
 ********************************************************************************
 ******************************* Queue_DoubleLinkListRep ************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfQueue_DoubleLinkListRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Queue_DoubleLinkListRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Queue_DoubleLinkListRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Queue_DoubleLinkListRep<T>::operator delete (void* p)
{
	BlockAllocated<Queue_DoubleLinkListRep<T> >::operator delete (p);
}

template	<class T>	Queue_DoubleLinkListRep<T>::Queue_DoubleLinkListRep () :
	fData ()
{
}

template	<class T> QueueRep<T>*	Queue_DoubleLinkListRep<T>::Clone () const
{
	return (new Queue_DoubleLinkListRep<T> (*this));
}

template	<class T> size_t Queue_DoubleLinkListRep<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class T> void	Queue_DoubleLinkListRep<T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class T> void Queue_DoubleLinkListRep<T>::Compact ()
{
}

template	<class T> IteratorRep<T>*	Queue_DoubleLinkListRep<T>::MakeIterator ()
{
	return (new Queue_DoubleLinkListIteratorRep<T> (*this));
}

template	<class T> void	Queue_DoubleLinkListRep<T>::AddTail (T item)
{
	fData.Append (item);
}

template	<class T> T	Queue_DoubleLinkListRep<T>::RemoveHead ()
{
	T	item =	fData.GetFirst ();
 	fData.RemoveFirst ();
	return (item);
}

template	<class T> T	Queue_DoubleLinkListRep<T>::Head () const
{
	return (fData.GetFirst ());
}








/*
 ********************************************************************************
 ******************************* Queue_DoubleLinkList ***************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Queue_DoubleLinkList;	// Tmp hack so GenClass will fixup following CTOR/DTORs
														// Harmless, but silly.

#endif

template	<class T> Queue_DoubleLinkList<T>::Queue_DoubleLinkList () :
	Queue<T> (new Queue_DoubleLinkListRep<T> ())
{
}

template	<class T> Queue_DoubleLinkList<T>::Queue_DoubleLinkList (const Queue<T>& src) :
	Queue<T> (new Queue_DoubleLinkListRep<T> ())
{
	ForEach (T, it, src) {
		AddTail (it.Current ());
	}
}

template	<class T> Queue_DoubleLinkList<T>::Queue_DoubleLinkList (const Queue_DoubleLinkList<T>& src) :
	Queue<T> (src)
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

