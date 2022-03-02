/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Queue__
#define	__Queue__

/*
 * $Header: /fuji/lewis/RCS/Queue.hh,v 1.22 1992/12/04 18:39:38 lewis Exp $
 *
 * Description:
 *		Standard LIFO (Last in first out) queue. See Sedgewick, 30-31.(CHECK REFERNECE)
 *	
 *		Queues always iterate from Head to last, same order as removals.
 *
 *		Related classes include Deques, which allow addition and removal at
 *	either end, and PriorityQueues, which allow removal based on the priority
 *	assigned to an item.
 *
 * TODO:
 *			To CTOR() that takes Iterator<T>
 *
 *
 * Notes:
 *		We currently default to the circular array implementation, as it is
 *	fastest under most circumstances. One drawback to it is that it has
 *	unpredictable costs for an Enqueue operation. DoubleLinkList is usually
 *	slower, but has very predictable costs.
 *
 * Changes:
 *	$Log: Queue.hh,v $
 *		Revision 1.22  1992/12/04  18:39:38  lewis
 *		*** empty log message ***
 *
 *		Revision 1.21  1992/12/04  17:50:20  lewis
 *		Rationalized names a bit. Use AddTail/RemoveHead as the main
 *		names, and use Enqueue and Dequeue() as synonyms.
 *		Comments, and delete op+=.
 *
 *		Revision 1.20  1992/11/23  21:24:59  lewis
 *		Return Iterator<T> instead of IteratorRep<T>*.
 *
 *		Revision 1.19  1992/11/17  05:29:58  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.18  1992/11/12  08:08:40  lewis
 *		#include Shared.hh
 *
 *		Revision 1.17  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.16  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.15  1992/10/22  04:12:25  lewis
 *		Add virtual DTOR for QueueRep - accidentally deleted with CollectionRep.
 *
 *		Revision 1.14  1992/10/15  20:20:32  lewis
 *		Typo.
 *
 *		Revision 1.13  1992/10/15  13:22:53  lewis
 *		Moved inlines in class declaration down to implemenation detail section,
 *		moved concrete rep classes to .cc file, and related inlines.
 *		Got rid of CollectionRep from QueueRep, and so mix in in each concrete
 *		rep class now.
 *
 *		Revision 1.12  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.11  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.10  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.9  1992/10/07  22:56:36  sterling
 *		For !qRealTemplatesAvailable and using new GenClass stuff add
 *		SharedOfQueueRepOf<T>_hh include instead of ## stuff in CollectionInfo.
 *
 *		Revision 1.8  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.7  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.6  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default src
 *		if no arg given.
 *
 *		Revision 1.4  1992/09/12  02:42:31  lewis
 *		Got rid of separate Rep.hh files - put into AbXX files inline.
 *
 *		Revision 1.3  1992/09/11  14:58:43  sterling
 *		used new Shared implementation
 *
 *
 *
 */

#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Iterator.hh"
#include	"Shared.hh"


#if		qRealTemplatesAvailable
	
	
template	<class	T>	class	QueueRep;
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfQueueRepOf<T>_hh
#endif

template	<class T> class	Queue {
	public:
		Queue ();
		Queue (const Queue<T>& src);

	protected:
		Queue (QueueRep<T>* src);

	public:
		nonvirtual	Queue<T>& operator= (const Queue<T>& src);

		nonvirtual	size_t	GetLength () const;
		nonvirtual	Boolean	IsEmpty () const;
		nonvirtual	void	RemoveAll ();
		nonvirtual	void	Compact ();
	 	nonvirtual	operator Iterator<T> () const;	


/*
 * Draw a picture of q Q of people waiting in line.
 */

		/*
old DOCS.
		 * Head lets you peek at what would be the result of the next Dequeue. It is
		 * an error to call Head () with an empty Q.
		 */

		nonvirtual	void	AddTail (T item);
		nonvirtual	T		Head () const;
		nonvirtual	T		RemoveHead ();



		/*
old DOCS.
		 * Stick the given item at the end of the Q. Since a Q is a LIFO structure,
		 * this item will be the removed (by a DeQueue) operation only after all other
		 * elements of the Q have been removed (DeQueued).
		 */
		nonvirtual	void	Enqueue (T item);		// AddTail


		/*
old DOCS.
		 * Remove the first item from the Q. This is an error (assertion) if the Q is
		 * empty. This returns that last most distant (historical/time) item from the Q -
		 * IE the one who has been waiting the longest.
		 */
		nonvirtual	T		Dequeue ();				//RemoveHead


	protected:
		nonvirtual	const QueueRep<T>*	GetRep () const;
		nonvirtual	QueueRep<T>*		GetRep ();

	private:
		Shared<QueueRep<T> >	fRep;

		static	QueueRep<T>*	Clone (const QueueRep<T>& src);

	friend	Boolean	operator== (const Queue<T>& lhs, const Queue<T>& rhs);
};

template	<class T> Boolean	operator== (const Queue<T>& lhs, const Queue<T>& rhs);
template	<class T> Boolean	operator!= (const Queue<T>& lhs, const Queue<T>& rhs);




template	<class	T>	class	QueueRep {
	protected:
		QueueRep ();

	public:
		virtual ~QueueRep ();

	public:
		virtual	QueueRep<T>*	Clone () const		=	Nil;
		virtual	void			Compact ()			=	Nil;
		virtual	size_t			GetLength () const 	=	Nil;
	 	virtual	IteratorRep<T>*	MakeIterator () 	=	Nil;
	 	virtual	void			RemoveAll () 		=	Nil;

		virtual	void			AddTail (T item)	=	Nil;
		virtual	T				RemoveHead ()		=	Nil;
		virtual	T				Head () const		=	Nil;
};


#endif	/*qRealTemplatesAvailable*/






/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable

	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifdef	inline
			#error "Ooops - I guess my workaround wont work!!!"
		#endif
		#define	inline
	#endif

	// class QueueRep<T>
	template	<class	T>	inline	QueueRep<T>::QueueRep ()
	{
	}
	template	<class	T>	inline	QueueRep<T>::~QueueRep ()
	{
	}


	// class Queue<T>
	template	<class	T>	inline	Queue<T>::Queue (const Queue<T>& src) :
		fRep (src.fRep)
	{
	}
	template	<class	T>	inline	Queue<T>::Queue (QueueRep<T>* src) :
		fRep (&Clone, src)
	{
	}
	template	<class	T>	inline	Queue<T>& Queue<T>::operator= (const Queue<T>& src)
	{
		fRep = src.fRep;
		return (*this);
	}
	template	<class	T>	inline	size_t	Queue<T>::GetLength () const
	{
		return (fRep->GetLength ());
	}
	template	<class	T>	inline	Boolean	Queue<T>::IsEmpty () const
	{
		return (Boolean (GetLength () == 0));
	}
	template	<class	T>	inline	void	Queue<T>::RemoveAll ()
	{
		fRep->RemoveAll ();
	}
	template	<class	T>	inline	void	Queue<T>::Compact ()
	{
		fRep->Compact ();
	}
	template	<class	T>	inline	Queue<T>::operator Iterator<T> () const
	{
		return (((Queue<T>*) this)->fRep->MakeIterator ());
	}		
	template	<class	T>	inline	void	Queue<T>::AddTail (T item)
	{
		fRep->AddTail (item);
	}
	template	<class	T>	inline	T	Queue<T>::RemoveHead ()
	{
		return (fRep->RemoveHead ());
	}
	template	<class	T>	inline	T	Queue<T>::Head () const
	{
		return (fRep->Head ());
	}
	template	<class	T>	inline	void	Queue<T>::Enqueue (T item)
	{
		fRep->AddTail (item);
	}
	template	<class	T>	inline	T	Queue<T>::Dequeue ()
	{
		return (fRep->RemoveHead ());
	}
	template	<class	T>	inline	const QueueRep<T>*	Queue<T>::GetRep () const
	{
		return ((const QueueRep<T>*) fRep.GetPointer ());
	}
	template	<class	T>	inline	QueueRep<T>*		Queue<T>::GetRep ()
	{
		return ((QueueRep<T>*) fRep.GetPointer ());
	}


	// operators
	template	<class T> inline	Boolean	operator!= (const Queue<T>& lhs, const Queue<T>& rhs)
	{
		return (not operator== (lhs, rhs));
	}

	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifndef	inline
			#error	"How did it get undefined?"
		#endif
		#undef	inline
	#endif

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
	#define qIncluding_QueueCC	1
		#include	"../Sources/Queue.cc"
	#undef  qIncluding_QueueCC
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Queue__*/
