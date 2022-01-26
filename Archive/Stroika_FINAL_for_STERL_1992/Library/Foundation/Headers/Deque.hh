/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Deque__
#define	__Deque__

/*
 * $Header: /fuji/lewis/RCS/Deque.hh,v 1.19 1992/12/04 18:39:57 lewis Exp $
 *
 * Description:
 *
 *		A Deque is a queue that allows additions and removals at either end.
 *	
 *		Deques always iterate from Head to Tail.
 *
 *		We currently default to the circular array implementation, as it is
 *	fastest under most circumstances. One drawback to it is that it has
 *	unpredictable costs for an Add operation. DoubleLinkList is usually
 *	slower, but has very predictable costs.
 *
 * TODO:
 *		+	Add CTOR(Iterator<T>) after next release....
 *
 * Notes:
 *
 *		We currently default to the circular array implementation, as it is
 *	fastest under most circumstances. One drawback to it is that it has
 *	unpredictable costs for an Add operation. DoubleLinkList is usually
 *	slower, but has very predictable costs.
 *
 *
 * Changes:
 *	$Log: Deque.hh,v $
 *		Revision 1.19  1992/12/04  18:39:57  lewis
 *		operator Iterator<T>() instead of operator IteratorRep<T>* ().
 *
 *		Revision 1.18  1992/12/04  16:20:25  lewis
 *		Lots of API changes. Most of the names of methods here were terrible.
 *		Also got rid of op-- and op+= since they were ambiguous.
 *		Now names do not reference stack or Q names - just add/remove head/tail.
 *		(Could possiblly add back Q names as synonms for AddTail/RemoveHead?).
 *
 *		Revision 1.17  1992/11/19  05:03:47  lewis
 *		Add Requires().
 *
 *		Revision 1.16  1992/11/17  05:30:41  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.15  1992/11/12  08:03:54  lewis
 *		#include Shared.hh
 *
 *		Revision 1.13  1992/11/04  02:18:51  lewis
 *		Make DequeRep() virtual DTOR.
 *
 *		Revision 1.12  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.11  1992/10/22  04:05:33  lewis
 *		Moved inlines to implementation section, moved concrete reps to .cc files.
 *		Got rid of unneeded methods (like GetReps in concrete class where
 *		not used). DequeRep no longer subclasses from CollectionRep - do
 *		in particular subclasses where needed.
 *
 *		Revision 1.10  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.9  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.8  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.7  1992/10/07  22:03:00  sterling
 *		For !qRealTemplatesAvailable and using new GenClass stuff add
 *		SharedOfDequeRepOf<T>_hh include instead of ## stuff in CollectionInfo.
 *
 *		Revision 1.6  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.5  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.4  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default rep
 *		if no arg given.
 *
 *		Revision 1.3  1992/09/12  02:42:31  lewis
 *		Got rid of separate Rep.hh files - put into AbXX files inline.
 *
 *		Revision 1.2  1992/09/11  14:58:43  sterling
 *		used new Shared implementation
 *
 *
 */


#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Iterator.hh"
#include	"Shared.hh"


#if		qRealTemplatesAvailable
	
template	<class	T>	class	DequeRep;
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfDequeRepOf<T>_hh
#endif

template	<class T> class	Deque {
	public:
		Deque ();
		Deque (const Deque<T>& src);

	protected:
		Deque (DequeRep<T>* rep);

	public:
		nonvirtual	Deque<T>& operator= (const Deque<T>& src);

		nonvirtual	size_t	GetLength () const;
		nonvirtual	Boolean	IsEmpty () const;
		nonvirtual	void	RemoveAll ();
		nonvirtual	void	Compact ();
	 	nonvirtual	operator Iterator<T> () const;	

		nonvirtual	void	AddHead (T item);
		nonvirtual	T		RemoveHead ();
		nonvirtual	T		Head () const;
		nonvirtual	void	AddTail (T item);
		nonvirtual	T		RemoveTail ();
		nonvirtual	T		Tail () const;

	protected:
		nonvirtual	const DequeRep<T>*	GetRep () const;
		nonvirtual	DequeRep<T>*		GetRep ();

	private:
		Shared<DequeRep<T> >	fRep;

		static	DequeRep<T>*	Clone (const DequeRep<T>& rep);

	friend	Boolean	operator== (const Deque<T>& lhs, const Deque<T>& rhs);
};

template	<class T> Boolean	operator== (const Deque<T>& lhs, const Deque<T>& rhs);
template	<class T> Boolean	operator!= (const Deque<T>& lhs, const Deque<T>& rhs);


template	<class	T>	class	DequeRep {
	protected:
		DequeRep ();

	public:
		virtual	~DequeRep ();

	public:
		virtual	DequeRep<T>*	Clone () const		=	Nil;
		virtual	void			Compact ()			=	Nil;
		virtual	size_t			GetLength () const 	=	Nil;
	 	virtual	IteratorRep<T>* MakeIterator () 	=	Nil;
	 	virtual	void			RemoveAll () 		=	Nil;

		virtual	void			AddHead (T item)	=	Nil;
		virtual	T				RemoveHead ()		=	Nil;
		virtual	T				Head () const		=	Nil;
		virtual	void			AddTail (T item)	=	Nil;
		virtual	T				RemoveTail ()		=	Nil;
		virtual	T				Tail () const		=	Nil;
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

	// class DequeRep<T>
	template	<class	T>	inline	DequeRep<T>::DequeRep ()
	{
	}
	template	<class	T>	inline	DequeRep<T>::~DequeRep ()
	{
	}


	// class Deque<T>
	template	<class	T>	inline	Deque<T>::Deque (const Deque<T>& src) :
		fRep (src.fRep)
	{
	}
	template	<class	T>	inline	Deque<T>::Deque (DequeRep<T>* rep) :
		fRep (&Clone, rep)
	{
	}
	template	<class	T>	inline	Deque<T>& Deque<T>::operator= (const Deque<T>& src)
	{
		fRep = src.fRep;
		return (*this);
	}
	template	<class	T>	inline	size_t	Deque<T>::GetLength () const
	{
		return (fRep->GetLength ());
	}
	template	<class	T>	inline	Boolean	Deque<T>::IsEmpty () const
	{
		return (Boolean (GetLength () == 0));
	}
	template	<class	T>	inline	void	Deque<T>::RemoveAll ()
	{
		fRep->RemoveAll ();
	}
	template	<class	T>	inline	void	Deque<T>::Compact ()
	{
		fRep->Compact ();
	}
	template	<class	T>	inline	Deque<T>::operator Iterator<T> () const
	{
		return (((Deque<T>*) this)->fRep->MakeIterator ());
	}		
	template	<class	T>	inline	void	Deque<T>::AddHead (T item)
	{
		fRep->AddHead (item);
	}
	template	<class	T>	inline	T	Deque<T>::RemoveHead ()
	{
		return (fRep->RemoveHead ());
	}
	template	<class	T>	inline	T		Deque<T>::Head () const
	{
		Require (not IsEmpty ());
		return (fRep->Head ());
	}
	template	<class	T>	inline	void	Deque<T>::AddTail (T item)
	{
		fRep->AddTail (item);
	}
	template	<class	T>	inline	T	Deque<T>::RemoveTail ()
	{
		return (fRep->RemoveTail ());
	}
	template	<class	T>	inline	T		Deque<T>::Tail () const
	{
		Require (not IsEmpty ());
		return (fRep->Tail ());
	}
	template	<class	T>	inline	const DequeRep<T>*	Deque<T>::GetRep () const
	{
		return ((const DequeRep<T>*) fRep.GetPointer ());
	}
	template	<class	T>	inline	DequeRep<T>*	Deque<T>::GetRep ()
	{
		return ((DequeRep<T>*) fRep.GetPointer ());
	}


	// class operator!=
	template	<class T> inline	Boolean	operator!= (const Deque<T>& lhs, const Deque<T>& rhs)
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
	#define qIncluding_DequeCC	1
		#include	"../Sources/Deque.cc"
	#undef  qIncluding_DequeCC
#endif





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Deque__*/
