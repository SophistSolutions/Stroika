/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Sequence__
#define	__Sequence__

/*
 * $Header: /fuji/lewis/RCS/Sequence.hh,v 1.32 1992/11/23 21:46:43 lewis Exp $
 *
 * Description:
 *
 *		SmallTalk book page 153
 *
 *
 * TODO:
 *
 * 	->	At some point in the near future we may add the ability to start at an
 *		arbitrary point in a sequence, and end at an arbitrary point. This
 *		requires more thought though. That functionality is probably not too
 *		important in light of being able to compute the current index easily
 *		in an iteration. Also, it requires more thought how to fit in with
 *		the sequenceDirection. Do we have a seperate constructor speciing
 *		two start and endpoints and use their relative order to decide a
 *		direction? Do we just add the two start and end values to the end of
 *		the param list? How hard is this todo with Sequence_DLL?? If this
 *		functionality is subsumed by smart-iterators, does it make sense to
 *		wait to we provide that functionality?
 *
 *	->	Figure out exactly what we will do about sorting/lookup function
 *		specification. Stroustrup like class param with somehow defaulting
 *		to op==????
 *
 *	->	Add SetLength() method. Make sure it is optimally efficeint, but try
 *		to avoid introducing a virtual function. Probably overload, and 1 arg
 *		version will use T default CTOR. If done nonvirtually with templates
 *		then we only require no arg CTOR when this function called - GOOD.
 *		Cannot really do with GenClass (would need to compile in seperate .o,
 *		even that wont work - need to not compile except when called).
 *
 *	->	Condider doing a T	operator[] (size_t index) const that returns a
 *		T& by having it return a different object that does magic - not
 *		quite sure how ???
 *
 *	->	Consider patching iterators on insertions??? If not, document more
 *		clearly why not. Document exact details of patching in SEQUENCE as
 *		part of API!!!!
 *
 * Notes:
 *
 *		Note: the decsion on arguments to a Sort() function was difficult.
 *	Making the arg default to op <= would not work since for type int it
 *	wouldnt be defined, and sometimes people define it as a member function,
 *	or taking const T& args. Thus the function pointer type would not match.
 *	The other alternative is to overload, and have the no arg function just
 *	have a static private CompareFunction that calls op<=. This does work
 *	pretty well, BUT it fails in cases like Sequence(Picture) where there
 *	is no op<= defined. Here, we could force the definition of this function,
 *	but that would be genrally awkward and was jugdged not worth the trouble.
 *	Just define your own little compare function that does op <=. Thats simple.
 *
 *		The other approach sterl's been pushing is that of functional objects
 *	described in Coplain, and the latest Stroustrup book (Nov 91). I haven't
 *	looked closely enuf to decide.
 *
 *		Another imporant addition was the CurrentIndex method. This was
 *	decided since it allowed for easy filtering (like only third thru eight
 *	elements, or only odd elements) without keeping an extra index variable
 *	which was often very awkward. This feature will probably be seldom used,
 *	and is seldom needed, but is one of the few things that differentiate
 *	a SequenceForEach from a Sequence (ie SequenceIterator from
 *	CollectionIterator). This statement really comes down to our really only
 *	needing sequence iterators rarely, and mostly using CollectionIterators.
 *
 *
 *
 * Changes:
 *	$Log: Sequence.hh,v $
 *		Revision 1.32  1992/11/23  21:46:43  lewis
 *		Use Iterator<T> instead of IteratorRep<T>* conversion operatore.
 *
 *		Revision 1.31  1992/11/17  05:29:58  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.30  1992/11/12  08:09:10  lewis
 *		#include Shared.hh
 *
 *		Revision 1.29  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.28  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.27  1992/10/22  04:16:52  lewis
 *		Cleanup code and add virtual DTOR for SequenceRep<T> - it was accidentally
 *		deleted with CollectionRep.
 *
 *		Revision 1.25  1992/10/19  01:41:12  lewis
 *		Made Contains a nonvirtual method here rather than indirecting to
 *		the backend reps.
 *		also fix comments.
 *
 *		Revision 1.24  1992/10/14  16:21:17  lewis
 *		Get rid of Collection.hh and CollectionRep.
 *
 *		Revision 1.23  1992/10/14  16:03:36  lewis
 *		Minor cleanups.
 *
 *		Revision 1.22  1992/10/13  05:27:21  lewis
 *		Lots of cleanups, including moving all the inline defintions to
 *		the bottom of the file.
 *
 *		Revision 1.21  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.20  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.19  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.18  1992/10/07  22:58:12  sterling
 *		For !qRealTemplatesAvailable and using new GenClass stuff add
 *		SharedOfSequenceRepOf<T>_hh include instead of ## stuff in CollectionInfo.
 *		And fix macro names to be more readable.
 *
 *		Revision 1.17  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.16  1992/09/25  21:14:26  lewis
 *		Fix lots of templates problems and get working using new GenClass stuff.
 *
 *		Revision 1.15  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.14  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.13  1992/09/15  17:05:42  lewis
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
 *		Revision 1.2  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.11  1992/07/21  06:11:42  lewis
 *		Tons of changes - looks like mostly generated macro version from template?
 *
 *		Revision 1.9  1992/07/19  03:52:28  lewis
 *		Re-order overrides so all the Collection overrides together, and in same order
 *		as base class.
 *
 *		Revision 1.8  1992/07/17  18:24:05  lewis
 *		Replaced old qTemplatesHasRepository with a new, much more elaborate, and
 *		hopefully more correct version.
 *
 *		Revision 1.7  1992/07/17  03:50:50  lewis
 *		Got rid of class Collection/SequenceMutator, and consider non-const
 *		CollectionIterator to be a Mutator. Very careful line by line
 *		analysis, and bugfixing, and implemented and tested (compiling)
 *		the macro version under GCC.
 *
 *		Revision 1.5  1992/07/16  07:03:19  lewis
 *		Use qRealTemplatesAvailable && qTemplatesHasRepository instead of qBorlandCPlus
 *		including hacks at bottom of file. Also, added  || qBorlandCPlus to
 *		qMPW_CFront in short macro name hack for macro based sequences.
 *		Minor cleanups of comments - trying to approach that elusive 80 column
 *		mark.
 *
 *		Revision 1.4  1992/07/08  05:31:24  lewis
 *		Some miscelaneous cleanups of templated version in hopes will work
 *		better with BCC.
 *		Also, Changed file name to include for template .cc stuff to UNIX
 *		pathname, and use PCName mapper to rename to appropriate PC File -
 *		cuz we may need to do this with gcc/aux too.
 *		Also, so I have PC Names (mapped to names) in just one place - so they
 *		are easier to change.
 *
 *		Revision 1.3  1992/07/07  07:04:08  lewis
 *		Fixed template problem - had two copies of IndirectSequenceIterator and
 *		none of IndirectSequenceMutator - fixed second copy.
 *
 *		Revision 1.18  1992/06/10  04:13:13  lewis
 *		Got rid of Sort function - do later as an external object operating on a sequence, ala
 *		Stroustrups second edition.
 *
 *		Revision 1.17  92/06/09  22:06:20  22:06:20  lewis (Lewis Pringle)
 *		Get rid of SequenceForEach() - use ForEachS instead.
 *		
 *		Revision 1.16  92/05/23  00:55:30  00:55:30  lewis (Lewis Pringle)
 *		Add comments/cleanup - get rid of override xxx= Nil - now they are inherited.
 *		
 *		Revision 1.15  92/05/21  17:26:21  17:26:21  lewis (Lewis Pringle)
 *		Changes only for template version.
 *		Add mutators, and support for CollectionView changes in Collection.hh.
 *		Some methods that were pure virtual like IndexOf () made nonvirtal and
 *		implemnted here, since tough to see how to do better and reduces code size
 *		(no virtuals pulled in unless needed).
 *		Got rid of sort - will do that differently.
 *		(with class Comparator<T>).
 *		
 *		Revision 1.14  92/05/09  00:54:13  00:54:13  lewis (Lewis Pringle)
 *		Port to BC++/PC and templates.
 *		
 *		Revision 1.13  92/03/30  12:48:36  12:48:36  lewis (Lewis Pringle)
 *		Implemented Templates.
 *		
 *		Revision 1.11  1992/02/27  20:57:37  lewis
 *		Comment on things todo.
 *		Inline some ctors and dtors now that apple bug with symtab overflow gone.
 *		These are so small and can often be optimized away.
 *		Add notion of CurrentIndex (not yet done start/end index - see notes).
 *
 *		Revision 1.10  1992/02/21  18:32:23  lewis
 *		remove qMPW_SymbolTableOverflowProblem.
 *
 *		Revision 1.9  1992/02/04  22:43:11  lewis
 *		Make Append/Prepend nonvirtual, and arg to IndexOfSequence const.
 *
 *		Revision 1.8  1992/01/22  05:25:24  lewis
 *		Get rid of const in front of T as arg to most methods (relic from const T& days).
 *		And got rid of pure virtuals - they are now inherited since we count on having at least a 2.1 compat.
 *		compiler.
 *
 *		Revision 1.7  1992/01/22  02:24:49  lewis
 *		Add Sort function.
 *
 *		Revision 1.6  1992/01/21  20:54:05  lewis
 *		Get rid of operator++/More from IndirectSequneceIterator (see Collection.hh).
 *		Also, define SequenceForEach() in case someone cannot use ForEach () - need to
 *		know your a SequenceIterator...
 *		We have no such cases of this yet, but if we add functionality to the sequence
 *		stuff - as we plane to, that will be more important.
 *
 *
 */
 

#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Iterator.hh"
#include	"Shared.hh"
	


// Probably get rid of this - just use a boolean!!
enum	SequenceDirection {
	eSequenceForward,
	eSequenceBackward,
};


const	size_t	kBadSequenceIndex	=	kMaxSize_T;

// I think these are a crock, and need to be looked at more closely. There are used in unsafe,
// pun fasion - investigate VERY soon - LGP May 23, 1992.
enum AddMode {
	ePrepend = 1,
	eAppend = -1,
};


#if		qRealTemplatesAvailable

template	<class T>	class	SequenceRep;
template	<class T>	class	SequenceIteratorRep;
template	<class T>	class	SequenceMutatorRep;
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfSequenceRepOf<T>_hh
#endif

template	<class T> class	SequenceIterator : public Iterator<T> {
	public:
		SequenceIterator (SequenceIteratorRep<T>* it);

		nonvirtual	size_t				CurrentIndex ()	const;
		nonvirtual	SequenceDirection	GetDirection () const;
};

template	<class T> class	SequenceMutator : public SequenceIterator<T> {
	public:
		SequenceMutator (SequenceMutatorRep<T>* it);

		nonvirtual	void	RemoveCurrent ();
		nonvirtual	void	UpdateCurrent (T newValue);
};

template	<class T>	class	Sequence {
	public:
		Sequence ();
		Sequence (const Sequence<T>& src);
		Sequence (const T* items, size_t size);

	protected:
		Sequence (SequenceRep<T>* rep);

	public:
		nonvirtual	Sequence<T>& operator= (const Sequence<T>& src);

	public:
		nonvirtual	size_t	GetLength () const;
		nonvirtual	Boolean	IsEmpty () const;
		nonvirtual	Boolean	Contains (T item) const;
		nonvirtual	void	RemoveAll ();
		nonvirtual	void	Compact ();

		nonvirtual	T		GetAt (size_t index) const;
		nonvirtual	void	SetAt (T item, size_t index);

		nonvirtual	T		operator[] (size_t index) const;

		/*
		 *		Search the sequence and see if the given item is contained in
		 *	it, and return the index of that item. Comparison is done with
		 *	operator==.
		 */
		nonvirtual	size_t	IndexOf (T item) const;
		nonvirtual	size_t	IndexOf (const Sequence<T>& s) const;

		/*
		 *		Insert the given item into the sequence at the given index.
		 *	Any active iterators will encounter the given item if their
		 *	cursor encounters the new index in the course of iteration.
		 *	Put another way, If you are iterating forwards, and you add an
		 *	item after what you are up to you will hit it - if you are iterating
		 *	backwards and you add an item before where you are, you will hit it -
		 *	otherwise you will miss the added item during iteration.
		 *
		 *		NB: Adding an item at the CURRENT index has no effect on
		 *	what the iterator says is the current item.
		 */
		nonvirtual	void	InsertAt (T item, size_t index);
		nonvirtual	void	InsertAt (const Sequence<T>& items, size_t index);

		nonvirtual	void	Prepend (T item);
		nonvirtual	void	Prepend (const Sequence<T>& items);
		nonvirtual	void	Append (T item);
		nonvirtual	void	Append (const Sequence<T>& items);

		nonvirtual	Sequence<T>&	operator+= (T item);
		nonvirtual	Sequence<T>&	operator+= (const Sequence<T>& items);

		/*
		 *		Remove the item at the given position of the sequence. Make sure
		 *	that iteration is not disturbed by this removal. In particular, any
		 *	items (other than the one at index) that would have been seen, will
		 *	still be, and no new items will be seen that wouldn't have been.
		 */
		nonvirtual	void	RemoveAt (size_t index);
		nonvirtual	void	RemoveAt (size_t index, size_t amountToRemove);

		/*
		 * Not an error to remove an item that is not an element of the list, instead has no effect.
		 */
		nonvirtual	void	Remove (T item);
		nonvirtual	void	Remove (const Sequence<T>& items);

		nonvirtual	Sequence<T>&	operator-= (T item);
		nonvirtual	Sequence<T>&	operator-= (const Sequence<T>& items);

		nonvirtual	operator Iterator<T> () const;
		nonvirtual	SequenceIteratorRep<T>*		MakeSequenceIterator (SequenceDirection d) const;
		nonvirtual	SequenceMutatorRep<T>*		MakeSequenceMutator (SequenceDirection d);
		nonvirtual	operator SequenceIterator<T> () const;
		nonvirtual	operator SequenceMutator<T> ();

	protected:
		nonvirtual	void	AddItems (const T* items, size_t size);

		nonvirtual	const SequenceRep<T>*	GetRep () const;
		nonvirtual	SequenceRep<T>*			GetRep ();

	private:
		Shared<SequenceRep<T> >	fRep;

		static	SequenceRep<T>*	Clone (const SequenceRep<T>& rep);

	friend	Boolean	operator== (const Sequence<T>& lhs, const Sequence<T>& rhs);
};

template	<class T> Boolean	operator== (const Sequence<T>& lhs, const Sequence<T>& rhs);
template	<class T> Boolean	operator!= (const Sequence<T>& lhs, const Sequence<T>& rhs);

template	<class T> Sequence<T>	operator+ (const Sequence<T>& lhs, const Sequence<T>& rhs);
template	<class T> Sequence<T>	operator- (const Sequence<T>& lhs, const Sequence<T>& rhs);


template	<class T>	class	SequenceRep {
	protected:
		SequenceRep ();

	public:
		virtual ~SequenceRep ();

	public:
		virtual	SequenceRep<T>*	Clone () const										=	Nil;
		virtual	size_t			GetLength () const 									=	Nil;
		virtual	void			Compact ()											=	Nil;
		virtual	void			RemoveAll () 										=	Nil;

		virtual	T				GetAt (size_t index) const							=	Nil;
		virtual	void			SetAt (T item, size_t index)						=	Nil;
		virtual	void			InsertAt (T item, size_t index)						=	Nil;
		virtual	void			RemoveAt (size_t index, size_t amountToRemove)		=	Nil;
		virtual	void			Remove (T item)										=	Nil;

		virtual	IteratorRep<T>* 		MakeIterator () 							=	Nil;
		virtual	SequenceIteratorRep<T>*	MakeSequenceIterator (SequenceDirection d)	=	Nil;
		virtual	SequenceMutatorRep<T>*	MakeSequenceMutator (SequenceDirection d)	=	Nil;
};

template	<class T>	class	SequenceIteratorRep : public IteratorRep<T> {
	protected:
		SequenceIteratorRep ();

	public:
		virtual	size_t				CurrentIndex ()	const	=	Nil;
		virtual	SequenceDirection	GetDirection () const	=	Nil;
};

template	<class T>	class	SequenceMutatorRep : public SequenceIteratorRep<T> {
	protected:
		SequenceMutatorRep ();

	public:
		virtual	void	RemoveCurrent () 			=	Nil;
		virtual	void	UpdateCurrent (T newValue) 	=	Nil;
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

	// class SequenceRep<T>
	template	<class T>	inline	SequenceRep<T>::SequenceRep ()
	{
	}
	template	<class T>	inline	SequenceRep<T>::~SequenceRep ()
	{
	}

	// class Sequence<T>
	template	<class T>	inline	Sequence<T>::Sequence (const Sequence<T>& src) :
		fRep (src.fRep)
	{
	}
	template	<class T>	inline	Sequence<T>::Sequence (SequenceRep<T>* rep) :
		fRep (&Clone, rep)
	{
	}
	template	<class T>	inline	Sequence<T>& Sequence<T>::operator= (const Sequence<T>& src)
	{
		fRep = src.fRep;
		return (*this);
	}
	template	<class T>	inline	const SequenceRep<T>*	Sequence<T>::GetRep () const
	{
		return (fRep.GetPointer ());
	}
	template	<class T>	inline	SequenceRep<T>*	Sequence<T>::GetRep ()
	{
		return (fRep.GetPointer ());
	}
	template	<class T>	inline	size_t	Sequence<T>::GetLength () const
	{
		return (fRep->GetLength ());
	}
	template	<class T>	inline	Boolean	Sequence<T>::IsEmpty () const
	{
		return (Boolean (GetLength () == 0));
	}
	template	<class T>	inline	void	Sequence<T>::RemoveAll ()
	{
		fRep->RemoveAll ();
	}
	template	<class T>	inline	void	Sequence<T>::Compact ()
	{
		fRep->Compact ();
	}
	template	<class T>	inline	T		Sequence<T>::GetAt (size_t index) const
	{
		return (fRep->GetAt (index));
	}
	template	<class T>	inline	T		Sequence<T>::operator[] (size_t index) const
	{
		return (fRep->GetAt (index));
	}
	template	<class T>	inline	void	Sequence<T>::Prepend (T item)
	{
		InsertAt (item, 1);
	}
	template	<class T>	inline	void	Sequence<T>::Prepend (const Sequence<T>& items)
	{
		InsertAt (items, 1);
	}
	template	<class T>	inline	void	Sequence<T>::Append (T item)
	{
		InsertAt (item, GetLength ()+1);
	}
	template	<class T>	inline	void	Sequence<T>::Append (const Sequence<T>& items)
	{
		InsertAt (items, GetLength ()+1);
	}
	template	<class T>	inline	Sequence<T>&	Sequence<T>::operator+= (T item)
	{
		Append (item);
		return (*this);
	}
	template	<class T>	inline	Sequence<T>&	Sequence<T>::operator+= (const Sequence<T>& items)
	{
		Append (items);
		return (*this);
	}
	template	<class T>	inline	Sequence<T>&	Sequence<T>::operator-= (T item)
	{
		Remove (item);
		return (*this);
	}
	template	<class T>	inline	Sequence<T>&	Sequence<T>::operator-= (const Sequence<T>& items)
	{
		Remove (items);
		return (*this);
	}
	template	<class T>	inline	Sequence<T>::operator Iterator<T> () const
	{
		// (~const) to force break references...
		return (((Sequence<T>*) this)->fRep->MakeIterator ());
	}		


	// operator!=
	template	<class T> inline	Boolean	operator!= (const Sequence<T>& lhs, const Sequence<T>& rhs)
	{
		return (not operator== (lhs, rhs));
	}


	// class SequenceIterator<T>
	template	<class T> inline	SequenceIterator<T>::SequenceIterator (SequenceIteratorRep<T>* it) :
		Iterator<T> (it)
	{
	}
	template	<class T> inline	size_t	SequenceIterator<T>::CurrentIndex () const
	{
		return (((SequenceIteratorRep<T>*)fIterator)->CurrentIndex ());
	}
	template	<class T> inline	SequenceDirection	SequenceIterator<T>::GetDirection () const
	{
		return (((SequenceIteratorRep<T>*)fIterator)->GetDirection ());
	}
	

	// class SequenceMutator<T>
	template	<class T> inline	SequenceMutator<T>::SequenceMutator (SequenceMutatorRep<T>* it) :
		SequenceIterator<T> (it)
	{
	}
	template	<class T> inline	void	SequenceMutator<T>::RemoveCurrent ()
	{
		((SequenceMutatorRep<T>*)fIterator)->RemoveCurrent ();
	}
	template	<class T> inline	void	SequenceMutator<T>::UpdateCurrent (T newValue)
	{
		((SequenceMutatorRep<T>*)fIterator)->UpdateCurrent (newValue);
	}



	// class SequenceIteratorRep<T>
	template	<class T>	inline	SequenceIteratorRep<T>::SequenceIteratorRep ():
		IteratorRep<T> ()
	{
	}

	// class SequenceMutatorRep<T>
	template	<class T>	inline	SequenceMutatorRep<T>::SequenceMutatorRep ():
		SequenceIteratorRep<T> ()
	{
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
	#define	qIncluding_SequenceCC	1
		#include	"../Sources/Sequence.cc"
	#undef	qIncluding_SequenceCC
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Sequence__*/

