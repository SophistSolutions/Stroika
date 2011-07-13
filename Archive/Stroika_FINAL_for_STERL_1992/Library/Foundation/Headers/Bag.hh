/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Bag__
#define	__Bag__

/*
 * $Header: /fuji/lewis/RCS/Bag.hh,v 1.32 1992/12/04 18:40:26 lewis Exp $
 *
 * Description:
 *
 *		A Bag is the simplest kind of collection. It allows addition and
 *	removal of elements, but makes no guarantees about element ordering. Two
 *	bags are considered equal if they contain the same items, even if iteration
 *	order is different.
 *
 *		Bags are typically designed to optimize item addition and iteration.
 *	They are fairly slow at item access (as they have no keys). Removing items
 *	is usually slow, except in the context of a BagMutator, where it is usually
 *	very fast. Bag comparison (operator==) is often very slow in the worst
 *	case (n^2) and this worst case is the relatively common case of identical
 *	bags.
 *
 *		Although Bag has an TallyOf () method, it is nonvirtual, and therefore
 *	not optimized for the various backends. There is a separate class, Tally,
 *	for cases where you are primarily interested in keeping an summary count
 *	of the occurences of each item.
 *
 *		Bags allow calls to Remove with an item not contained within the bag.
 *
 *		As syntactic sugar, using either functional (Add, Remove) or
 *	operator (+,-) is allowed.
 *
 *
 * TODO:
 *		+	Do CTOR () that takes ITERATOR<T> - but not til after next release....
 *
 *		+	Have Bag_Difference/Union/Interesection??? methods/?? Do research....
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Bag.hh,v $
 *		Revision 1.32  1992/12/04  18:40:26  lewis
 *		Disable hack for #define Bag(T) Bag<T> since failed with BCC - investigate
 *		later.
 *
 *		Revision 1.31  1992/12/04  16:08:46  lewis
 *		After discussions with sterl: rename OccurencesOf() to TallyOf().
 *		Sterl likes name better, I like smalltalk comatability better, but
 *		OH, well.
 *		Comment on a few methods API.
 *		Note things to be implemented.
 *
 *		Revision 1.30  1992/12/03  08:06:17  lewis
 *		Work around GenClass quirks.
 *
 *		Revision 1.29  1992/12/03  07:30:49  lewis
 *		Added Template/Macro compatability #defines for Bag* etc on
 *		a trail basis.
 *
 *		Revision 1.28  1992/11/23  21:21:31  lewis
 *		Return Iterator<T> instead of IteratorRep<T> - same for BagIterator/Mutator.
 *
 *		Revision 1.27  1992/11/17  05:28:19  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.26  1992/11/12  08:01:42  lewis
 *		Include Shared.hh.
 *
 *		Revision 1.24  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.23  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.22  1992/10/22  04:01:06  lewis
 *		Add virtual DTOR to BagRep. Accidentally lost when I got rid of
 *		base class CollectionRep.
 *
 *		Revision 1.21  1992/10/20  17:51:01  lewis
 *		Cleaned up a few comments.
 *
 *		Revision 1.19  1992/10/15  02:04:45  lewis
 *		Get rid of unnessary casts, and got rid of base class CollectionRep-
 *		if needed, use in subclasses (eg BagRep_Array).Comment on new stuff
 *		todo and ambiguities in defintions.
 *
 *		Revision 1.17  1992/10/14  02:26:02  lewis
 *		Cleanup format - move all inlines to the bottom of the file in
 *		implementation section. Move declarations of concrete iterators
 *		(except bag.hh) to the .cc file.
 *
 *		Revision 1.16  1992/10/10  20:16:36  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.15  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.14  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.13  1992/10/07  21:50:45  sterling
 *		For !qRealTemplatesAvailable and using new GenClass stuff add
 *		SharedOfBagRepOf<T>_hh include instead of ## stuff in CollectionInfo.
 *
 *		Revision 1.12  1992/10/02  04:10:02  lewis
 *		Fixed macro defines, and op== method SB op= - typeo, and iterator typo.
 *
 *		Revision 1.11  1992/09/29  20:18:01  lewis
 *		Fixed bug in AddItems () again - macro version only - not sure how it
 *		got lost.
 *		Also, put back include of .cc file at end of header - also not sure
 *		how it got lost - and moved all(most) of the inline methods
 *		out of class declarations and put them into the inline section
 *		where they belong. Fixed minor bugs with some of the methods (unnessary
 *		casts).
 *
 *		Revision 1.10  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.5  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default rep
 *		if no arg given.
 *
 *		Revision 1.4  1992/09/12  02:42:31  lewis
 *		Got rid of separate Rep.hh files - put into AbXX files inline.
 *
 *		Revision 1.3  1992/09/11  14:58:43  sterling
 *		used new Shared implementation
 *
 *		Revision 1.2  1992/09/04  19:12:47  sterling
 *		Class description.
 *
 *		Revision 1.3  1992/07/17  18:45:11  lewis
 *		Fixed up templated bags, and translated to macro based version.
 *
 *		Revision 1.2  1992/06/25  02:49:15  lewis
 *		Fix template bugs.
 *
 *		Revision 1.5  1992/06/10  16:32:13  lewis
 *		Fixed up, so should compile.
 *
 *
 *
 */
 
#include	"Debug.hh"
#include	"Iterator.hh"
#include	"Shared.hh"


#if		qRealTemplatesAvailable

template	<class T>	class	BagRep;
template	<class T>	class	BagIteratorRep;
template	<class T>	class	BagMutatorRep;
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfBagRepOf<T>_hh
#endif

template	<class T>	class	BagIterator : public Iterator<T> {
	public:
		BagIterator (BagIteratorRep<T>* it);
};

template	<class T>	class	BagMutator : public Iterator<T> {
	public:
		BagMutator (BagMutatorRep<T>* it);

	public:
		void	RemoveCurrent ();
		void	UpdateCurrent (T newValue);
};

template	<class T>	class	Bag {
	public:
		Bag ();
		Bag (const Bag<T>& bag);
		Bag (const T* items, size_t size);

	protected:
		Bag (BagRep<T>* rep);

	public:
		nonvirtual	Bag<T>& operator= (const Bag<T>& bag);

		nonvirtual	size_t	GetLength () const;
		nonvirtual	Boolean	IsEmpty () const;
		nonvirtual	Boolean	Contains (T item) const;
		nonvirtual	void	RemoveAll ();
		nonvirtual	void	Compact ();
		nonvirtual	operator Iterator<T> () const;

	/*
	 *
	 */
	public:
		nonvirtual	void	Add (T item);
		nonvirtual	void	Add (Bag<T> items);


	/*
	 * It is legal to remove something that is not there.
	 */
	public:
		nonvirtual	void	Remove (T item);
		nonvirtual	void	Remove (const Bag<T>& items);



// Have Bag_Difference/Union/Interesection??? methods/??


	/*
	 *		+=/-= are equivilent Add() and Remove(). They
	 *	are just syntactic sugar.
	 */
	public:
		nonvirtual	Bag<T>&	operator+= (T item);
		nonvirtual	Bag<T>&	operator+= (const Bag<T>& items);
		nonvirtual	Bag<T>&	operator-= (T item);
		nonvirtual	Bag<T>&	operator-= (const Bag<T>& items);


	/*
	 *	Build BagIterators or Mutators.
	 */
	public:
		nonvirtual	operator BagIterator<T> () const;
		nonvirtual	operator BagMutator<T> ();

	public:
		nonvirtual	size_t	TallyOf (T item) const;


	protected:
		nonvirtual	void	AddItems (const T* items, size_t size);

		// Are these allowed to return Nil??? If not, add asserts!!!
		nonvirtual	const BagRep<T>*	GetRep () const;
		nonvirtual	BagRep<T>*			GetRep ();

	private:
		Shared<BagRep<T> >	fRep;

		static	BagRep<T>*	Clone (const BagRep<T>& rep);

	friend	Boolean	operator== (const Bag<T>& lhs, const Bag<T>& rhs);	// friend to check if reps equal...
};

template	<class T>	Boolean	operator== (const Bag<T>& lhs, const Bag<T>& rhs);
template	<class T>	Boolean	operator!= (const Bag<T>& lhs, const Bag<T>& rhs);

template	<class T>	Bag<T>	operator+ (const Bag<T>& lhs, const Bag<T>& rhs);
template	<class T>	Bag<T>	operator- (const Bag<T>& lhs, const Bag<T>& rhs);



template	<class T>	class	BagIteratorRep : public IteratorRep<T> {
	protected:
		BagIteratorRep ();
};

template	<class T>	class	BagMutatorRep : public BagIteratorRep<T> {
	protected:
		BagMutatorRep ();

	public:
		virtual	void	RemoveCurrent () 			=	Nil;
		virtual	void	UpdateCurrent (T newValue) 	=	Nil;
};

template	<class T>	class	BagRep {
	protected:
		BagRep ();

	public:
		virtual ~BagRep ();

	public:
		virtual	BagRep<T>*	Clone () const					=	Nil;
		virtual	Boolean		Contains (T item) const			=	Nil;
		virtual	size_t		GetLength () const 				=	Nil;
		virtual	void		Compact ()						=	Nil;
		virtual	void		RemoveAll () 					=	Nil;

		virtual	void		Add (T item)					=	Nil;
		virtual	void		Remove (T item)					=	Nil;

		virtual	IteratorRep<T>*		MakeIterator () 		=	Nil;
		virtual	BagIteratorRep<T>*	MakeBagIterator () 		=	Nil;
		virtual	BagMutatorRep<T>*	MakeBagMutator () 		=	Nil;
};


#endif	/*qRealTemplatesAvailable*/




/*
 *	Template/Macro compatability.
 *		Use macro notation so that the code will compiler properly whether
 *	using macros or templates. This will go away when we stop supporting
 *	macros-based templates altogether.
 *
 *	NB:	&& 1 below cuz of GenClass quirks
 */
#if		qRealTemplatesAvailable && 0
// Disabled for now - did not work with BCC 3.1 - not sure why - investigate
// later. -- LGP Friday, December 4, 1992 1:22:47 PM
	#define	Bag(T)				Bag<T>
	#define	BagRep(T)			BagRep<T>
	#define	BagIterator(T)		BagIterator<T>
	#define	BagIteratorRep(T)	BagIteratorRep<T>
	#define	BagMutator(T)		BagMutator<T>
	#define	BagMutatorRep(T)	BagMutatorRep<T>
#endif





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

	// operator!=
	template	<class T>	inline	Boolean	operator!= (const Bag<T>& lhs, const Bag<T>& rhs)
	{
		return (not operator== (lhs, rhs));
	}

	// class BagIteratorRep<T>
	template	<class T>	inline	BagIteratorRep<T>::BagIteratorRep () :
		IteratorRep<T> ()
	{
	}

	// class BagMutatorRep<T>
	template	<class T>	inline	BagMutatorRep<T>::BagMutatorRep () :
		BagIteratorRep<T> ()
	{
	}

	// class BagIterator<T>
	template	<class T>	inline	BagIterator<T>::BagIterator (BagIteratorRep<T>* it) :
		Iterator<T> (it)
	{
	}

	// class BagMutator<T>
	template	<class T>	inline	BagMutator<T>::BagMutator (BagMutatorRep<T>* it) :
		Iterator<T> (it)
	{
	}
	template	<class T>	inline	void	BagMutator<T>::RemoveCurrent ()
	{
		/*
		 * Because of the way we construct BagMutators, it is gauranteed that
		 * this cast is safe. We could have kept an extra var of the right
		 * static type, but this would have been a waste of time and memory.
		 */
		((BagMutatorRep<T>*)fIterator)->RemoveCurrent ();
	}
	template	<class T>	inline	void	BagMutator<T>::UpdateCurrent (T newValue)
	{
		/*
		 * Because of the way we construct BagMutators, it is gauranteed that
		 * this cast is safe. We could have kept an extra var of the right
		 * static type, but this would have been a waste of time and memory.
		 */
		 ((BagMutatorRep<T>*)fIterator)->UpdateCurrent (newValue);
	}

	// class BagRep<T>
	template	<class T>	inline	BagRep<T>::BagRep ()
	{
	}
	template	<class T>	inline	BagRep<T>::~BagRep ()
	{
	}

	// class Bag<T>
	template	<class T>	inline	Bag<T>::Bag (const Bag<T>& bag) :
		fRep (bag.fRep)
	{
	}
	template	<class T>	inline	Bag<T>::Bag (BagRep<T>* rep) :
		fRep (&Clone, rep)
	{
	}
	template	<class T>	inline	Bag<T>&	Bag<T>::operator= (const Bag<T>& bag)
	{
		fRep = bag.fRep;
		return (*this);
	}
	template	<class T>	inline	size_t	Bag<T>::GetLength () const
	{
		return (fRep->GetLength ());
	}
	template	<class T>	inline	Boolean	Bag<T>::IsEmpty () const
	{
		return (Boolean (GetLength () == 0));
	}
	template	<class T>	inline	Boolean	Bag<T>::Contains (T item) const
	{
		return (fRep->Contains (item));
	}
	template	<class T>	inline	void	Bag<T>::RemoveAll ()
	{
		fRep->RemoveAll ();
	}
	template	<class T>	inline	void	Bag<T>::Compact ()
	{
		fRep->Compact ();
	}
	template	<class T>	inline	Bag<T>::operator Iterator<T> () const
	{
		// (~const) to force a break references
		return (((Bag<T>*) this)->fRep->MakeIterator ());
	}
	template	<class T>	inline	Bag<T>&	Bag<T>::operator+= (T item)
	{
		Add (item);
		return (*this);
	}
	template	<class T>	inline	Bag<T>&	Bag<T>::operator+= (const Bag<T>& items)
	{
		Add (items);
		return (*this);
	}
	template	<class T>	inline	Bag<T>&	Bag<T>::operator-= (T item)
	{
		Remove (item);
		return (*this);
	}
	template	<class T>	inline	Bag<T>&	Bag<T>::operator-= (const Bag<T>& items)
	{
		Remove (items);
		return (*this);
	}
	template	<class T>	inline	Bag<T>::operator BagIterator<T> () const
	{
		// (~const) to force a break references
		return (((Bag<T>*) this)->fRep->MakeBagIterator ());
	}
	template	<class T>	inline	Bag<T>::operator BagMutator<T> ()
	{
		return (fRep->MakeBagMutator ());
	}
	template	<class T>	inline	const BagRep<T>*	Bag<T>::GetRep () const
	{
		return (fRep.GetPointer ());
	}
	template	<class T>	inline	BagRep<T>*	Bag<T>::GetRep ()
	{
		return (fRep.GetPointer ());
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
	#define qIncluding_BagCC	1
		#include	"../Sources/Bag.cc"
	#undef  qIncluding_BagCC
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Bag__*/

