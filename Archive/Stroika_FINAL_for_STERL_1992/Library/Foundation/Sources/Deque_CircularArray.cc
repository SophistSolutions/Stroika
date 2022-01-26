/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Deque_CircularArray__cc
#define	__Deque_CircularArray__cc

/*
 * $Header: /fuji/lewis/RCS/Deque_CircularArray.cc,v 1.20 1992/12/04 17:53:03 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Deque_CircularArray.cc,v $
 *		Revision 1.20  1992/12/04  17:53:03  lewis
 *		Lots of name changes to make DQ API more reasonable.
 *		Also, got rid of op+= since ambiguous.
 *
 *		Revision 1.19  1992/12/03  19:18:17  lewis
 *		Got rid of fFirstTime hack - now taken care of for us by CircularArray<T>.
 *
 *		Revision 1.18  1992/12/03  05:59:07  lewis
 *		Rename Skrunch->Compact.
 *		Use X(X&) in clone methods. Let compiler autogenerate them.
 *
 *		Revision 1.17  1992/11/29  02:45:42  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.16  1992/11/20  19:27:53  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.15  1992/11/19  05:07:57  lewis
 *		Get rid of dependency on Collection.hh. Use new CircularArray support.
 *		Implement Deque_Deque_CircularArrayIteratorRep.
 *		Renamed DequeRep_CircularArray->Deque_CircularArrayRep.
 *
 *		Revision 1.14  1992/11/13  03:41:04  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *
 *		Revision 1.12  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
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
 *		Revision 1.7  1992/10/07  23:11:08  sterling
 *		Fixed typos in template implementation.
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
 *		Revision 1.2  1992/09/05  04:05:38  lewis
 *		Fix name of classes to be Deque_CircularArray instead of _Array
 *
 *
 */


#include	"BlockAllocated.hh"
#include	"CircularArray.hh"
#include	"Debug.hh"
#include	"Deque.hh"				// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Deque_CircularArray.hh"




#if		qRealTemplatesAvailable

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class	T> class	Deque_CircularArrayRep : public DequeRep<T> {
	public:
		Deque_CircularArrayRep ();

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

		CircularArray_Patch<T>	fData;
};

template	<class T> class	Deque_CircularArrayIteratorRep : public IteratorRep<T> {
	public:
		Deque_CircularArrayIteratorRep (Deque_CircularArrayRep<T>& owner);

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
	friend	class	Deque_CircularArrayRep<T>;
};





/*
 ********************************************************************************
 **************************** Deque_CircularArrayIteratorRep ********************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfDeque_CircularArrayIteratorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Deque_CircularArrayIteratorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Deque_CircularArrayIteratorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Deque_CircularArrayIteratorRep<T>::operator delete (void* p)
{
	BlockAllocated<Deque_CircularArrayIteratorRep<T> >::operator delete (p);
}

template	<class	T>	Deque_CircularArrayIteratorRep<T>::Deque_CircularArrayIteratorRep (Deque_CircularArrayRep<T>& owner) :
	IteratorRep<T> (),
	fIterator (owner.fData)
{
}

template	<class	T>	Boolean	Deque_CircularArrayIteratorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class	T>	Boolean	Deque_CircularArrayIteratorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class	T>	T	Deque_CircularArrayIteratorRep<T>::Current () const
{
	return (fIterator.Current());
}

template	<class T>	IteratorRep<T>*	Deque_CircularArrayIteratorRep<T>::Clone () const
{
	return (new Deque_CircularArrayIteratorRep<T> (*this));
}








/*
 ********************************************************************************
 ********************************** Deque_CircularArrayRep **********************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfDeque_CircularArrayRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Deque_CircularArrayRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Deque_CircularArrayRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Deque_CircularArrayRep<T>::operator delete (void* p)
{
	BlockAllocated<Deque_CircularArrayRep<T> >::operator delete (p);
}

template	<class T>	Deque_CircularArrayRep<T>::Deque_CircularArrayRep () :
	fData ()
{
}

template	<class T>	DequeRep<T>*	Deque_CircularArrayRep<T>::Clone () const
{
	return (new Deque_CircularArrayRep<T> (*this));
}

template	<class T>	size_t	Deque_CircularArrayRep<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class T>	void	Deque_CircularArrayRep<T>::Compact ()
{
	fData.Compact ();
}

template	<class T>	void	Deque_CircularArrayRep<T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class T>	IteratorRep<T>*	Deque_CircularArrayRep<T>::MakeIterator ()
{
	return (new Deque_CircularArrayIteratorRep<T> (*this));
}

template	<class T>	void	Deque_CircularArrayRep<T>::AddHead (T item)
{
	fData.InsertAt (item, 1);
}

template	<class T>	T	Deque_CircularArrayRep<T>::RemoveHead ()
{
	Require (GetLength () != 0);
	T	result	=	fData[1];
	fData.RemoveAt (1);
	return (result);
}

template	<class T>	T	Deque_CircularArrayRep<T>::Head () const
{
	Require (GetLength () != 0);
	return (fData[1]);
}

template	<class T>	void	Deque_CircularArrayRep<T>::AddTail (T item)
{
	fData.InsertAt (item, GetLength ()+1);
}

template	<class T>	T	Deque_CircularArrayRep<T>::RemoveTail ()
{
	Require (GetLength () != 0);
	T	result	=	fData[GetLength ()];
	fData.RemoveAt (GetLength ());
	return (result);
}

template	<class T>	T	Deque_CircularArrayRep<T>::Tail () const
{
	Require (GetLength () != 0);
	return (fData[GetLength ()]);
}





/*
 ********************************************************************************
 ********************************** Deque_CircularArray *************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Deque_CircularArray;	// tmp hack so GenClass will fixup below array CTOR
														// harmless, but silly

#endif

template	<class T>	Deque_CircularArray<T>::Deque_CircularArray () :
	Deque<T> (new Deque_CircularArrayRep<T> ())
{
}

template	<class T>	Deque_CircularArray<T>::Deque_CircularArray (const Deque<T>& src) :
	Deque<T> (new Deque_CircularArrayRep<T> ())
{
	SetSlotsAlloced (src.GetLength ());
	ForEach (T, it, src) {
		AddTail (it.Current ());
	}
}

template	<class T>	inline	const Deque_CircularArrayRep<T>*	Deque_CircularArray<T>::GetRep () const
{
	return ((const Deque_CircularArrayRep<T>*) Deque<T>::GetRep ());
}

template	<class T>	inline	Deque_CircularArrayRep<T>*	Deque_CircularArray<T>::GetRep ()
{
	return ((Deque_CircularArrayRep<T>*) Deque<T>::GetRep ());
}

template	<class T>	size_t	Deque_CircularArray<T>::GetSlotsAlloced () const
{
	return (GetRep ()->fData.GetSlotsAlloced ());
}

template	<class T>	void	Deque_CircularArray<T>::SetSlotsAlloced (size_t slots)
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

