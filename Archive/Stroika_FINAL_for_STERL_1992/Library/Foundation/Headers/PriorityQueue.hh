/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PriorityQueue__
#define	__PriorityQueue__

/*
 * $Header: /fuji/lewis/RCS/PriorityQueue.hh,v 1.11 1992/11/13 03:36:34 lewis Exp $
 *
 * Description:
 *
 *	PriorityQueues are a kind of Queue that allow retrieval based the priority assigned an item.
 *	This priority is given either at the time when the item is Enqueueed to the PriorityQueue, or
 *	by a function. The default function always assigns the lowest possible priority to an item.
 *	Priority start at zero and work upwards, so a zero priority item will be the last item
 *	removed from the PriorityQueue.
 *
 *	PriorityQueues support two kinds of iteration: over type T, or over ProirityQueueEntrys of
 *	type T. A PriorityQueueEntry is a simple structure that couples together the item and its
 *	priority.
 *
 *	
 *	PriorityQueues always iterate from highest to lowest priority.
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: PriorityQueue.hh,v $
 *		Revision 1.11  1992/11/13  03:36:34  lewis
 *		#if 0'd out everything - fix later.
 *
 *		Revision 1.10  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.9  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.8  1992/10/22  19:03:21  lewis
 *		Started (but not finished) conversion to new genclass stuff and
 *		using templates. Obviously template version was way off - no testsuite
 *		and maybe macro version worked but...
 *
 *		Revision 1.7  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.6  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.5  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.4  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.3  1992/09/21  05:55:16  sterling
 *		Sterl made lots of changes - not sure what.
 *
 *		Revision 1.2  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.3  1992/09/12  02:42:31  lewis
 *		Got rid of separate Rep.hh files - put into AbXX files inline.
 *
 *		Revision 1.2  1992/09/11  14:58:43  sterling
 *		used new Shared implementation
 *
 *
 *
 */

#include	"Debug.hh"
#include	"Iterator.hh"


typedef	UInt16	Priority;
const	Priority	kMinPriority	=	kMinUInt16;
const	Priority	kMaxPriority	=	kMaxUInt16;


#if		qRealTemplatesAvailable && 0
	
template	<class	T>	Priority	DefaultPrioritizer (T /*item*/);


// Someday this should be renamed ...
template	<class T>	class	PQEntry {
	public:
		PQEntry (T item, Priority p);
	
		T			fItem;
		Priority	fPriority;
};
template	<class T> Boolean	operator== (const PQEntry<T>& lhs, const PQEntry<T>& rhs);



template	<class T> class	PriorityQueue {
	public:
		PriorityQueue ();
		PriorityQueue (const PriorityQueue<T>& src);

	protected:
		PriorityQueue (PriorityQueueRep<T>* rep);

	public:
		nonvirtual	PriorityQueue<T>& operator= (const PriorityQueue<T>& src);

		nonvirtual	size_t	GetLength () const;
		nonvirtual	Boolean	IsEmpty () const;
		nonvirtual	void	RemoveAll ();
		nonvirtual	void	Compact ();

	 	nonvirtual	operator IteratorRep<PQEntry<T> >* () const;
	 	nonvirtual	operator IteratorRep<T>* () const;

		nonvirtual	void		Enqueue (T item);
		nonvirtual	void		Enqueue (T item, Priority priority);
		nonvirtual	T			Dequeue ();
		nonvirtual	T			Head () const;
		nonvirtual	Priority	TopPriority () const;
		nonvirtual	void		RemoveHead ();
	
		nonvirtual	PriorityQueue<T>&	operator+= (T item);
		nonvirtual	PriorityQueue<T>&	operator+= (IteratorRep<PQEntry<T> >* it);
		nonvirtual	PriorityQueue<T>&	operator-- ();

	protected:
		nonvirtual	const PriorityQueueRep<T>*	GetRep () const;
		nonvirtual	PriorityQueueRep<T>*		GetRep ();

	private:
		Shared<PriorityQueueRep<T> >	fRep;

		static	PriorityQueueRep<T>*	Clone (const PriorityQueueRep<T>& rep);

	friend	Boolean	operator== (const PriorityQueue<T>& lhs, const PriorityQueue<T>& rhs);
};

template	<class T> Boolean	operator== (const PriorityQueue<T>& lhs, const PriorityQueue<T>& rhs);
template	<class T> Boolean	operator!= (const PriorityQueue<T>& lhs, const PriorityQueue<T>& rhs);




template	<class	T>	class	PriorityQueueRep {
	protected:
		PriorityQueueRep ();
		PriorityQueueRep (Priority (*f) (T));

	public:
		virtual	PriorityQueueRep<T>*	Clone () const				=	Nil;
		virtual	size_t	GetLength () const 							=	Nil;
		virtual	void	Compact ()									=	Nil;
	 	virtual	void	RemoveAll () 								=	Nil;

	 	virtual	IteratorRep<PQEntry<T>>* MakeEntryIterator ()		=	Nil;

		nonvirtual	void		Enqueue (T item);
		virtual		void		Enqueue (T item, Priority priority)	=	Nil;
		virtual		T			Dequeue ()							=	Nil;
		virtual		T			Head () const						=	Nil;
		virtual		Priority	TopPriority () const				=	Nil;
		virtual		void		RemoveHead ()						=	Nil;

		nonvirtual	void	SetPrioritizer (Priority (*f) (T));

	protected:
		Priority	(*fPrioritize) (T);
};


#endif	/*qRealTemplatesAvailable*/





/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable && 0
	// DefaultPrioritizer
	template	<class	T>	Priority	DefaultPrioritizer (T /*item*/)
	{
		return (kMinPriority);
	}

	// class PQEntry<T>
	template	<class T>	inline	PQEntry<T>::PQEntry (T item, Priority p) :
		fItem (item),
		fPriority (p)
	{
	}

	// operator==
// Not sure why this is necessary???? Should probably never be used???
	template	<class T> inline	Boolean	operator== (const PQEntry<T>& lhs, const PQEntry<T>& rhs)
	{
		if (not (lhs.fItem == rhs.fItem)) {
			return False;
		}
		return (Boolean (lhs.fPriority == rhs.fPriority));
	}



	// class	PriorityQueueRep<T>
	template	<class T> inline	PriorityQueueRep<T>::PriorityQueueRep () :
		fPrioritize (&DefaultPrioritizer)
	{
	}
	template	<class T> inline	PriorityQueueRep<T>::PriorityQueueRep (Priority (*f) (T)) :
		fPrioritize (f)
	{
	}
	template	<class T> inline	void	PriorityQueueRep<T>::Enqueue (T item)
	{
		RequireNotNil (fPrioritize);
		Enqueue (item, (*fPrioritize) (item));
	}
	template	<class T> inline	void	PriorityQueueRep<T>::SetPrioritizer (Priority (*f) (T))
	{
		fPrioritize = f;
	}

	// class	PriorityQueue<T>
	template	<class T> inline	PriorityQueue<T>::PriorityQueue (const PriorityQueue<T>& src) :
		fRep (src.fRep)
	{
	}
	template	<class T> inline	PriorityQueue<T>::PriorityQueue (PriorityQueueRep<T>* rep) :
		fRep (&Clone, rep)
	{
	}
	template	<class T> inline	PriorityQueue<T>& PriorityQueue<T>::operator= (const PriorityQueue<T>& src)
	{
		fRep = src.fRep;
		return (*this);
	}
	template	<class T> inline	size_t	PriorityQueue<T>::GetLength () const
	{
		return (fRep->GetLength ());
	}
	template	<class T> inline	Boolean	PriorityQueue<T>::IsEmpty () const
	{
		return (Boolean (GetLength () == 0));
	}
	template	<class T> inline	void	PriorityQueue<T>::RemoveAll ()
	{
		fRep->RemoveAll ();
	}
	template	<class T> inline	void	PriorityQueue<T>::Compact ()
	{
		fRep->Compact ();
	}
	template	<class T> inline	PriorityQueue<T>::operator IteratorRep<PQEntry<T> >* () const
	{
		return (((PriorityQueue<T>*) this)->fRep->MakeEntryIterator ());
	}
	template	<class T> inline	void		PriorityQueue<T>::Enqueue (T item)
	{
		fRep->Enqueue (item);
	}
	template	<class T> inline	void		PriorityQueue<T>::Enqueue (T item, Priority priority)
	{
		fRep->Enqueue (item, priority);
	}
	template	<class T> inline	T	PriorityQueue<T>::Dequeue ()
	{
		return (fRep->Dequeue ());
	}
	template	<class T> inline	T	PriorityQueue<T>::Head () const
	{
		return (fRep->Head ());
	}
	template	<class T> inline	Priority	PriorityQueue<T>::TopPriority () const
	{
		return (fRep->TopPriority ());
	}
	template	<class T> inline	void	PriorityQueue<T>::RemoveHead ()
	{
		fRep->RemoveHead ();
	}
	template	<class T> inline	PriorityQueue<T>&	PriorityQueue<T>::operator+= (T item)
	{
		Enqueue (item);
		return (*this);
	}
	template	<class T> inline	PriorityQueue<T>&	PriorityQueue<T>::operator-- ()
	{
		RemoveHead ();
		return (*this);
	}
	template	<class T> inline	const PriorityQueueRep<T>*	PriorityQueue<T>::GetRep () const
	{
		return ((const PriorityQueueRep<T>*) fRep.GetPointer ());
	}
	template	<class T> inline	PriorityQueueRep<T>*	PriorityQueue<T>::GetRep ()
	{
		return ((PriorityQueueRep<T>*) fRep.GetPointer ());
	}

	// operator!=
	template	<class T> inline	Boolean	operator!= (const PriorityQueue<T>& lhs, const PriorityQueue<T>& rhs)
	{
		return (not operator== (lhs, rhs));
	}
#endif	/*qRealTemplatesAvailable*/


#if		qRealTemplatesAvailable && !qTemplatesHasRepository
	/*
	 *		We must include our .cc file here because of this limitation.
	 *	But, under some systems (notably UNIX) we cannot compile some parts
	 *	of our .cc file from the .hh.
	 *
	 *		The other problem is that there is sometimes some untemplated code
	 *	in the .cc file, and to detect this fact, we define another magic
	 *	flag which is looked for in those files.
	 */
	#define qIncluding_PriorityQueueCC	1
		#include	"../Sources/PriorityQueue.cc"
	#undef  qIncluding_PriorityQueueCC
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__PriorityQueue__*/
