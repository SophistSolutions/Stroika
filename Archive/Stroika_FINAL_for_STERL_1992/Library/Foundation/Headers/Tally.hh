/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Tally__
#define	__Tally__

/*
 * $Header: /fuji/lewis/RCS/Tally.hh,v 1.24 1992/12/04 17:52:34 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Tally.hh,v $
 *		Revision 1.24  1992/12/04  17:52:34  lewis
 *		Comments about UpdateCurrent() in Tally iterator.
 *
 *		Revision 1.23  1992/11/23  21:50:00  lewis
 *		Return Iterator<T> instead of IteratorRep<T>* - similary for TallyMutator.
 *		Also, move up TallyMutator declaration for this, and #include of iterator defns.
 *
 *		Revision 1.22  1992/11/17  05:29:39  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.21  1992/11/12  08:13:47  lewis
 *		#include Debug.hh and Shared.h
 *
 *		Revision 1.20  1992/11/05  22:12:57  lewis
 *		Got rid of extra ';' that made CFront 2.1 barf.
 *
 *		Revision 1.19  1992/11/02  19:51:06  lewis
 *		Comment on meaning of UpdateCurrent () - talk over with sterl.
 *
 *		Revision 1.18  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.17  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.16  1992/10/22  04:12:57  lewis
 *		Add virtual DTOR for TallyRep - accidentally deleted with CollectionRep.
 *
 *		Revision 1.15  1992/10/20  17:59:29  lewis
 *		Lots of typos etc, and missing includes fixed to get it to compiler.
 *
 *		Revision 1.14  1992/10/19  21:42:07  lewis
 *		Move inlines from class declaration to implemenation details section at end
 *		of header, and moved concrete reps to .cc file. Added #includes where needed
 *		for GenClass stuff. Other minor cleanups - untested yet.
 *
 *		Revision 1.13  1992/10/13  05:29:51  lewis
 *		Include IteratorOfTallyEntryOf<T>_hh instead of IteratorOfTYEntryOf<T>_hh.
 *
 *		Revision 1.12  1992/10/10  20:21:39  lewis
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
 *		Revision 1.9  1992/10/07  23:07:50  sterling
 *		For !qRealTemplatesAvailable and using new GenClass stuff add
 *		IteratorOfTYEntryOf<T>_hh include instead of ## stuff in CollectionInfo.
 *		And make out of line op== for TallyEntry.
 *
 *		Revision 1.8  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.7  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.6  1992/09/23  13:46:32  lewis
 *		Work around qGCC_BadDefaultCopyConstructorGeneration bug with TallyEntry(T).
 *
 *		Revision 1.5  1992/09/23  01:17:02  lewis
 *		Added missing inline directive for what should have been inline
 *		op== functions.
 *
 *		Revision 1.4  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *
 *
 */
 
#include	"Debug.hh"
#include	"Iterator.hh"
#include	"Shared.hh"



#if		qRealTemplatesAvailable

template	<class T>	class	TallyEntry {
	public:
		TallyEntry (T item);
		TallyEntry (T item, size_t count);

#if		qGCC_BadDefaultCopyConstructorGeneration
		TallyEntry (const TallyEntry<T>& from) :	fItem (from.fItem), fCount (from.fCount) { }
		nonvirtual	TallyEntry<T>& operator= (const TallyEntry<T>& from) {fItem = from.fItem; fCount = from.fCount; return *this; }
#endif
	
		T		fItem;
		size_t	fCount;
};
template	<class T> Boolean	operator== (const TallyEntry<T>& lhs, const TallyEntry<T>& rhs);



#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	IteratorOfTallyEntryOf<T>_hh
#endif


template	<class T> class	TallyRep;
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfTallyRepOf<T>_hh
#endif

template	<class T> class	IteratorRep;
#if		!qRealTemplatesAvailable
	template	<class T> class	IteratorRep<TallyEntry<T>>;
#endif
template	<class T> class	TallyMutatorRep;


// Must be more careful about copying TallyMutators...Now that iterators are copyable...
template	<class T> class	TallyMutator : public Iterator<TallyEntry<T> > {
	public:
		TallyMutator (TallyMutatorRep<T>* it);

		nonvirtual	void	RemoveCurrent ();

	/*
	 *		Update the
	 * NB: if newCount == 0, equivilent to RemoveCurrent().
	 */
	public:
		nonvirtual	void	UpdateCount (size_t newCount);
};


template	<class T> class	Tally {
	public:
		Tally ();
		Tally (const Tally<T>& src);
		Tally (const T* items, size_t size);

	protected:
		Tally (TallyRep<T>* rep);

	public:
		nonvirtual	Tally<T>& operator= (const Tally<T>& src);

	public:
		nonvirtual	size_t	GetLength () const;
		nonvirtual	Boolean	IsEmpty () const;
		nonvirtual	Boolean	Contains (T item) const;
		nonvirtual	void	RemoveAll ();
		nonvirtual	void	Compact ();

	 	nonvirtual	operator Iterator<T> () const;
	 	nonvirtual	operator Iterator<TallyEntry<T> > () const;
	 	nonvirtual	operator TallyMutator<T> ();

		nonvirtual	void	Add (T item);
		nonvirtual	void	Add (T item, size_t count);
		nonvirtual	void	Remove (T item);
		nonvirtual	void	Remove (T item, size_t count);
		nonvirtual	void	RemoveAll (T item);
		nonvirtual	size_t	TallyOf (T item) const;
		nonvirtual	size_t	TotalTally () const;

		nonvirtual	Tally<T>&	operator+= (T item);
		nonvirtual	Tally<T>&	operator+= (Tally<T> t);

	protected:
		nonvirtual	void	AddItems (const T* items, size_t size);

		nonvirtual	const TallyRep<T>*	GetRep () const;
		nonvirtual	TallyRep<T>*		GetRep ();

	private:
		Shared<TallyRep<T> >	fRep;

		static	TallyRep<T>*	Clone (const TallyRep<T>& rep);

	friend	Boolean	operator== (const Tally<T>& lhs, const Tally<T>& rhs);
};

template	<class T> Boolean	operator== (const Tally<T>& lhs, const Tally<T>& rhs);
template	<class T> Boolean	operator!= (const Tally<T>& lhs, const Tally<T>& rhs);




template	<class T> class	TallyRep {
	protected:
		TallyRep ();

	public:
		virtual ~TallyRep ();

	public:
		virtual	TallyRep<T>*	Clone () const							=	Nil;
		virtual	Boolean	Contains (T item) const							=	Nil;
		virtual	size_t	GetLength () const 								=	Nil;
		virtual	void	Compact ()										=	Nil;
	 	virtual	void	RemoveAll () 									=	Nil;

		virtual	void	Add (T item, size_t count)						=	Nil;
		virtual	void	Remove (T item, size_t count)					=	Nil;
		virtual	size_t	TallyOf (T item) const							=	Nil;

		nonvirtual	IteratorRep<T>*				MakeIterator ();
		virtual	IteratorRep<TallyEntry<T> >*	MakeTallyIterator ()	=	Nil;
		virtual	TallyMutatorRep<T>*				MakeTallyMutator ()		=	Nil;
};



template	<class T> class	TallyMutatorRep : public IteratorRep<TallyEntry<T> > {
	protected:
		TallyMutatorRep ();

	public:
		virtual	void	RemoveCurrent () 				=	Nil;
		virtual	void	UpdateCount (size_t newCount) 	=	Nil;
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

	// class TallyEntry<T>
	template	<class T>	inline	TallyEntry<T>::TallyEntry (T item) :
		fItem (item),
		fCount (1)
	{
	}
	template	<class T>	inline	TallyEntry<T>::TallyEntry (T item, size_t count) :
		fItem (item),
		fCount (count)
	{
	}

	// class TallyMutatorRep<T>
	template	<class T> inline	TallyMutatorRep<T>::TallyMutatorRep () :
		IteratorRep<TallyEntry<T> > ()
	{
	}

	// class TallyRep<T>
	template	<class T>	inline	TallyRep<T>::TallyRep ()
	{
	}
	template	<class T>	inline	TallyRep<T>::~TallyRep ()
	{
	}

	// class TallyMutator<T>
	template	<class T>	inline	TallyMutator<T>::TallyMutator (TallyMutatorRep<T>* it) :
		Iterator<TallyEntry<T> > (it)
	{
	}
	template	<class T>	inline	void	TallyMutator<T>::RemoveCurrent ()
	{
		((TallyMutatorRep<T>*)fIterator)->RemoveCurrent ();
	}
	template	<class T>	inline	void	TallyMutator<T>::UpdateCount (size_t newCount)
	{
		((TallyMutatorRep<T>*)fIterator)->UpdateCount (newCount);
	}

	// class Tally<T>
	template	<class T>	inline	Tally<T>::Tally (const Tally<T>& src) :
		fRep (src.fRep)
	{
	}
	template	<class T>	inline	Tally<T>::Tally (TallyRep<T>* rep) :
		fRep (&Clone, rep)
	{
	}
	template	<class T>	inline	Tally<T>& Tally<T>::operator= (const Tally<T>& src)
	{
		fRep = src.fRep;
		return (*this);
	}
	template	<class T>	inline	size_t	Tally<T>::GetLength () const
	{
		return (fRep->GetLength ());
	}
	template	<class T>	inline	Boolean	Tally<T>::IsEmpty () const
	{
		return (Boolean (GetLength () == 0));
	}
	template	<class T>	inline	Boolean	Tally<T>::Contains (T item) const
	{
		return (fRep->Contains (item));
	}
	template	<class T>	inline	void	Tally<T>::RemoveAll ()
	{
		fRep->RemoveAll ();
	}
	template	<class T>	inline	void	Tally<T>::Compact ()
	{
		fRep->Compact ();
	}
	template	<class T>	inline	Tally<T>::operator Iterator<T> () const
	{
		return (((Tally<T>*) this)->fRep->MakeIterator ());
	}
	template	<class T>	inline	Tally<T>::operator Iterator<TallyEntry<T> > () const
	{
		return (((Tally<T>*) this)->fRep->MakeTallyIterator ());
	}
	template	<class T>	inline	Tally<T>::operator TallyMutator<T> ()
	{
		return (fRep->MakeTallyMutator ());
	}
	template	<class T>	inline	void	Tally<T>::Add (T item)
	{
		fRep->Add (item, 1);
	}
	template	<class T>	inline	void	Tally<T>::Add (T item, size_t count)
	{
		fRep->Add (item, count);
	}
	template	<class T>	inline	void	Tally<T>::Remove (T item)
	{
		fRep->Remove (item, 1);
	}
	template	<class T>	inline	void	Tally<T>::Remove (T item, size_t count)
	{
		fRep->Remove (item, count);
	}
	template	<class T>	inline	size_t	Tally<T>::TallyOf (T item) const
	{
		return (fRep->TallyOf (item));
	}
	template	<class T>	inline	Tally<T>&	Tally<T>::operator+= (T item)
	{
		fRep->Add (item, 1);
		return (*this);
	}
	template	<class T>	inline	const TallyRep<T>*	Tally<T>::GetRep () const
	{
		return ((const TallyRep<T>*) fRep.GetPointer ());
	}
	template	<class T>	inline	TallyRep<T>*		Tally<T>::GetRep ()
	{
		return ((TallyRep<T>*) fRep.GetPointer ());
	}

	// operator!=
	template	<class T> inline	Boolean	operator!= (const Tally<T>& lhs, const Tally<T>& rhs)
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
	 * 		We must include our .cc file here because of this limitation.
	 *	But, under some systems (notably UNIX) we cannot compile some parts
	 *	of our .cc file from the .hh.
	 *
	 *		The other problem is that there is sometimes some untemplated code
	 *	in the .cc file, and to detect this fact, we define another magic
	 *	flag which is looked for in those files.
	 */
	#define	qIncluding_TallyCC	1
		#include	"../Sources/Tally.cc"
	#undef	qIncluding_TallyCC
#endif





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Tally__*/

