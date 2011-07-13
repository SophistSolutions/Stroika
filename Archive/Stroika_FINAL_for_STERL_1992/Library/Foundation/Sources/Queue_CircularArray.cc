/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Queue_CircularArray__cc
#define	__Queue_CircularArray__cc

/*
 * $Header: /fuji/lewis/RCS/Queue_CircularArray.cc,v 1.18 1992/12/04 16:19:36 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Queue_CircularArray.cc,v $
 *		Revision 1.18  1992/12/04  16:19:36  lewis
 *		Fixed errand qBCC_InlinesBreakTemplates workaround.
 *		Reordered and renamed a bunch of Q methods - general Enqueue became AddTail
 *		and Dequeue became RemoveHead().
 *		Got rid of op+= so we have to do that manually in CTOR Queue_Car<T>::Queue_Car<T>(const Queue<T>&).
 *
 *		Revision 1.17  1992/12/03  19:19:12  lewis
 *		Add qBCC_InlinesBreakTemplates workaround, and comment back in
 *		call to it.More() - CircularArray<T> now supports it.
 *
 *		Revision 1.16  1992/12/03  07:20:06  lewis
 *		Rename Skrunch->Compact.
 *		Move towards new More() API for CircularArray but not done
 *		yet so this file is sort of in limbo. Fix as soon as we
 *		convert CircularArray. Could temporarily put back fFirstTime
 *		hack but why bother.
 *
 *		Revision 1.15  1992/11/20  19:27:53  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.14  1992/11/19  05:09:05  lewis
 *		Get rid of dependency on Collection.hh. Use new CircularArray support.
 *		Implement QuueRep_CircularArray->Queue_CircularArrayRep.
 *
 *		Revision 1.13  1992/11/13  03:47:26  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *
 *		Revision 1.12  1992/11/12  08:24:52  lewis
 *		Added #if !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
 *		around template predeclare intendent to ameliorate GenClass limitation.
 *
 *		Revision 1.11  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.10  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.9  1992/10/15  13:22:53  lewis
 *		Moved inlines in class declaration down to implemenation detail section,
 *		moved concrete rep classes to .cc file, and related inlines.
 *		Got rid of CollectionRep from QueueRep, and so mix in in each concrete
 *		rep class now.
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
 *		Revision 1.2  1992/09/05  03:38:42  lewis
 *		Rename class from Queue_Array to Queue_CircularArray - was not totally consistent.
 *
 *
 */


#include	"BlockAllocated.hh"
#include	"CircularArray.hh"
#include	"Debug.hh"
#include	"Queue.hh"				// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Queue_CircularArray.hh"



#if		qRealTemplatesAvailable

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class	T> class	Queue_CircularArrayRep : public QueueRep<T> {
	public:
		Queue_CircularArrayRep ();

		override	QueueRep<T>*	Clone () const;
		override	size_t	GetLength () const;
		override	void	Compact ();
		override	void	RemoveAll ();
	 	override	IteratorRep<T>* MakeIterator ();

		override	void	AddTail (T item);
		override	T		Head () const;
		override	T		RemoveHead ();

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

		CircularArray_Patch<T>	fData;
};




template	<class T> class	Queue_CircularArrayIteratorRep : public IteratorRep<T> {
	public:
		Queue_CircularArrayIteratorRep (Queue_CircularArrayRep<T>& owner);

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
		ForwardCircularArrayIterator_Patch<T>	fIterator;
	friend	class	Queue_CircularArrayRep<T>;
};





/*
 ********************************************************************************
 **************************** Queue_CircularArrayIteratorRep ********************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfQueue_CircularArrayIteratorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Queue_CircularArrayIteratorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Queue_CircularArrayIteratorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Queue_CircularArrayIteratorRep<T>::operator delete (void* p)
{
	BlockAllocated<Queue_CircularArrayIteratorRep<T> >::operator delete (p);
}

template	<class	T>	Queue_CircularArrayIteratorRep<T>::Queue_CircularArrayIteratorRep (Queue_CircularArrayRep<T>& owner) :
	IteratorRep<T> (),
	fIterator (owner.fData)
{
}

template	<class	T>	Boolean	Queue_CircularArrayIteratorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class	T>	Boolean	Queue_CircularArrayIteratorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class	T>	T	Queue_CircularArrayIteratorRep<T>::Current () const
{
	return (fIterator.Current());
}

template	<class T>	IteratorRep<T>*	Queue_CircularArrayIteratorRep<T>::Clone () const
{
	return (new Queue_CircularArrayIteratorRep<T> (*this));
}








/*
 ********************************************************************************
 ********************************** Queue_CircularArrayRep **********************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfQueue_CircularArrayRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Queue_CircularArrayRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Queue_CircularArrayRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Queue_CircularArrayRep<T>::operator delete (void* p)
{
	BlockAllocated<Queue_CircularArrayRep<T> >::operator delete (p);
}

template	<class	T>	inline	Queue_CircularArrayRep<T>::Queue_CircularArrayRep () :
	fData ()
{
}

template	<class T>	size_t	Queue_CircularArrayRep<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class T>	void	Queue_CircularArrayRep<T>::Compact ()
{
	fData.Compact ();
}

template	<class T>	QueueRep<T>*	Queue_CircularArrayRep<T>::Clone () const
{
	return (new Queue_CircularArrayRep<T> (*this));
}

template	<class T>	void	Queue_CircularArrayRep<T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class T>	IteratorRep<T>*	Queue_CircularArrayRep<T>::MakeIterator ()
{
	return (new Queue_CircularArrayIteratorRep<T> (*this));
}

template	<class T>	void	Queue_CircularArrayRep<T>::AddTail (T item)
{
	fData.InsertAt (item, GetLength () + 1);
}

template	<class T>	T	Queue_CircularArrayRep<T>::RemoveHead ()
{
	Require (GetLength () != 0);
	T	result = fData.GetAt (1);
	fData.RemoveAt (1);
	return (result);
}

template	<class T>	T	Queue_CircularArrayRep<T>::Head () const
{
	Require (GetLength () != 0);
	return (fData.GetAt (1));
}




/*
 ********************************************************************************
 ****************************** Queue_CircularArray *****************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Queue_CircularArray;	// Tmp hack so GenClass will fixup following CTOR/DTORs
														// Harmless, but silly.

#endif

template	<class T>	Queue_CircularArray<T>::Queue_CircularArray () :
	Queue<T> (new Queue_CircularArrayRep<T> ())
{
}

template	<class T>	Queue_CircularArray<T>::Queue_CircularArray (const Queue<T>& src) :
	Queue<T> (new Queue_CircularArrayRep<T> ())
{
	GetRep ()->fData.SetSlotsAlloced (src.GetLength ());
	ForEach (T, it, src) {
		AddTail (it.Current ());
	}
}

template	<class T>	inline	const Queue_CircularArrayRep<T>*	Queue_CircularArray<T>::GetRep () const
{
	return ((const Queue_CircularArrayRep<T>*) Queue<T>::GetRep ());
}
template	<class T>	inline	Queue_CircularArrayRep<T>*			Queue_CircularArray<T>::GetRep ()
{
	return ((Queue_CircularArrayRep<T>*) Queue<T>::GetRep ());
}

template	<class T>	size_t	Queue_CircularArray<T>::GetSlotsAlloced () const
{
	return (GetRep ()->fData.GetSlotsAlloced ());
}

template	<class T>	void	Queue_CircularArray<T>::SetSlotsAlloced (size_t slots)
{
	GetRep ()->fData.SetSlotsAlloced (slots);
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

