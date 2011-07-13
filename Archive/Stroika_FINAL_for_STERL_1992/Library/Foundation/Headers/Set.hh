/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Set__
#define	__Set__

/*
 * $Header: /fuji/lewis/RCS/Set.hh,v 1.21 1992/11/23 21:47:40 lewis Exp $
 *
 * Description:
 *
 *		The Set class is based on SmallTalk-80, The Language & Its Implementation,
 *		page 148.  Also, see documentation for Collection<T>.
 *
 *		The basic idea here is that you cannot have multiple entries of the same
 *		thing into the set.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Set.hh,v $
 *		Revision 1.21  1992/11/23  21:47:40  lewis
 *		Return Iterator<T> instead of IteratorRep<T>* - similaryly for
 *		SetIterator and Mutator.
 *
 *		Revision 1.20  1992/11/17  05:29:58  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.19  1992/11/12  08:10:00  lewis
 *		#include Shared.hh
 *
 *		Revision 1.18  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.17  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.16  1992/10/22  04:14:12  lewis
 *		Add virtual DTOR for SetRep - accidentally deleted with CollectionRep.
 *
 *		Revision 1.15  1992/10/15  02:11:25  lewis
 *		Make inlines go to bottom of file.
 *
 *		Revision 1.14  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.13  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.12  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.11  1992/10/07  22:59:34  sterling
 *		For !qRealTemplatesAvailable and using new GenClass stuff add
 *		SharedOfSetRepreOf<T>_hh include instead of ## stuff in CollectionInfo.
 *
 *		Revision 1.10  1992/10/02  04:12:22  lewis
 *		Fixed some macro names - mac/unix versions must be same for GenClass to work.
 *
 *		Revision 1.9  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.8  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.7  1992/09/15  17:05:42  lewis
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
 *		Revision 1.5  1992/07/18  15:25:20  lewis
 *		Scrunch macro versions.
 *
 *		Revision 1.4  1992/07/17  18:25:17  lewis
 *		Replaced old qTemplatesHasRepository with a new, much more elaborate, and
 *		hopefully more correct version.
 *
 *		Revision 1.3  1992/07/17  17:07:21  lewis
 *		Fixed typo.
 *
 *		Revision 1.2  1992/07/17  16:12:25  lewis
 *		Cleaned up comments, and did(finished) template implementation.
 *
 *		Revision 1.13  1992/04/16  10:57:40  lewis
 *		Had to duplicate #defines for macro based sets, so we could get small
 *		code size on unix, and still use loaddump on mac. Mac had
 *		qMPW_CFRONT_2_1_SingleObjectSubclassThenAddPureVirtualBreaksLoadDump_BUG
 *		bug that haunts us again.
 *
 *		Revision 1.12  92/04/14  07:30:57  07:30:57  lewis (Lewis Pringle)
 *		Added comment about qSoleInlineVirtualsNotStripped as why we added out
 *		of line virtual dtor.
 *		
 *		Revision 1.11  92/04/13  21:18:13  21:18:13  lewis (Lewis Pringle)
 *		Make Set have an out of line virtual dtor, since
 *		under UNIX (at least HPs) the compiler/linker generates 1 dtor and
 *		probably vtable per .o file compiled, and these are all included
 *		redundently in the final executable. This makes things MUCH bigger. I
 *		am hopping we dont need to do this kind of crap with the template
 *		version, since hopefully any system sophisticated enuf to support
 *		templates, and strip these redundent defintiions too. If not, its easier
 *		to development-environment by development-environment, make these
 *		decisions wiht ifdefs in the template version.
 *		
 *		Revision 1.10  92/03/30  13:39:45  13:39:45  lewis (Lewis Pringle)
 *		Templates.
 *		
 *		Revision 1.8  1992/02/21  18:39:23  lewis
 *		Got rid of outofline ctor/dtor for qMPW_SymbolTableOverflowProblem. And
 *		name shortening...
 *
 *		Revision 1.7  1992/01/22  05:28:22  lewis
 *		Fix for changes in interface of Collection. And stop repeating pure
 *		virtuals - they are now inherited.
 *
 *		Revision 1.6  1992/01/21  20:57:55  lewis
 *		Use ForEach() macro.
 *
 *		Revision 1.5  1992/01/16  00:36:21  lewis
 *		Add outofline dtor to avoid mac loaddump compiler bug.
 *
 *		Revision 1.4  1991/12/13  07:09:21  lewis
 *		Get rid of inline dtor since agravated qMPW_SymbolTableOverflowProblem.
 *		Also, removed trailing crap from # directives, and added RCS headers,
 *		and cleaned stuff up.
 *
 */
 
#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Iterator.hh"
#include	"Shared.hh"



#if		qRealTemplatesAvailable
	
template	<class	T>	class	SetRep;
template	<class	T>	class	SetIteratorRep;
template	<class	T>	class	SetMutatorRep;
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfSetRepOf<T>_hh
#endif



template	<class T> class	SetIterator : public Iterator<T> {
	public:
		SetIterator (SetIteratorRep<T>* it);
};

template	<class T> class	SetMutator : public Iterator<T> {
	public:
		SetMutator (SetMutatorRep<T>* it);

		nonvirtual	void	RemoveCurrent ();
};

template	<class T> class	Set {
	public:
		Set ();
		Set (const Set<T>& src);
		Set (const T* items, size_t size);

	protected:
		Set (SetRep<T>* rep);

	public:
		nonvirtual	Set<T>& operator= (const Set<T>& src);

	public:
		nonvirtual	size_t	GetLength () const;
		nonvirtual	Boolean	IsEmpty () const;
		nonvirtual	Boolean	Contains (T item) const;
		nonvirtual	void	RemoveAll ();
		nonvirtual	void	Compact ();

		nonvirtual	void	Add (T item);
		nonvirtual	void	Add (Set<T> items);	// note passed by value to avoid s.Add(s) problems
		nonvirtual	void	Remove (T item);
		nonvirtual	void	Remove (const Set<T>& items);

		nonvirtual	Set<T>&	operator+= (T item);
		nonvirtual	Set<T>&	operator+= (const Set<T>& items);
		nonvirtual	Set<T>&	operator-= (T item);
		nonvirtual	Set<T>&	operator-= (const Set<T>& items);

	 	nonvirtual	operator Iterator<T> () const;		
	 	nonvirtual	operator SetIterator<T> () const;
	 	nonvirtual	operator SetMutator<T> ();

	protected:
		nonvirtual	void	AddItems (const T* items, size_t size);

		nonvirtual	const SetRep<T>*	GetRep () const;
		nonvirtual	SetRep<T>*			GetRep ();

	private:
		Shared<SetRep<T> >	fRep;

		static	SetRep<T>*	Clone (const SetRep<T>& rep);

	friend	Boolean	operator== (const Set<T>& lhs, const Set<T>& rhs);
};

template	<class T> Boolean	operator== (const Set<T>& lhs, const Set<T>& rhs);
template	<class T> Boolean	operator!= (const Set<T>& lhs, const Set<T>& rhs);

template	<class T> Set<T>	operator+ (const Set<T>& lhs, const Set<T>& rhs);
template	<class T> Set<T>	operator- (const Set<T>& lhs, const Set<T>& rhs);



template	<class T> class	SetIteratorRep : public IteratorRep<T> {
	protected:
		SetIteratorRep ();
};

template	<class T> class	SetMutatorRep : public SetIteratorRep<T> {
	protected:
		SetMutatorRep ();

	public:
		virtual	void	RemoveCurrent () 	= Nil;
};

template	<class	T>	class	SetRep {
	protected:
		SetRep ();

	public:
		virtual ~SetRep ();

	public:
		virtual	SetRep<T>*	Clone () const				=	Nil;
		virtual	Boolean		Contains (T item) const		=	Nil;
		virtual	size_t		GetLength () const 			=	Nil;
		virtual	void		Compact ()					=	Nil;
	 	virtual	void		RemoveAll () 				=	Nil;

		virtual	void		Add (T item)				=	Nil;
		virtual	void		Remove (T item)				=	Nil;

	 	virtual	IteratorRep<T>* 	MakeIterator ()		=	Nil;
		virtual	SetIteratorRep<T>*	MakeSetIterator ()	=	Nil;
		virtual	SetMutatorRep<T>* 	MakeSetMutator ()	=	Nil;
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

	// class SetRep<T>
	template	<class	T>	inline	SetRep<T>::SetRep ()
	{
	}
	template	<class	T>	inline	SetRep<T>::~SetRep ()
	{
	}

	// class SetIteratorRep<T>
	template	<class	T>	inline	SetIteratorRep<T>::SetIteratorRep ()
	{
	}

	// class SetMutatorRep<T>
	template	<class	T>	inline	SetMutatorRep<T>::SetMutatorRep ()
	{
	}

	// class SetIterator<T>
	template	<class	T>	inline	SetIterator<T>::SetIterator (SetIteratorRep<T>* it) :
		Iterator<T> (it)
	{
	}

	// class SetMutator<T>
	template	<class	T>	inline	SetMutator<T>::SetMutator (SetMutatorRep<T>* it) :
		Iterator<T> (it)
	{
	}
	template	<class	T>	inline	void	SetMutator<T>::RemoveCurrent ()
	{
		((SetMutatorRep<T>*)fIterator)->RemoveCurrent ();
	}



	// class Set<T>
	template	<class	T>	inline	Set<T>::Set (const Set<T>& src) :
		fRep (src.fRep)
	{
	}
	template	<class	T>	inline	Set<T>::Set (SetRep<T>* rep) :
		fRep (&Clone, rep)
	{
	}
	template	<class	T>	inline	Set<T>& Set<T>::operator= (const Set<T>& src)
	{
		fRep = src.fRep;
		return (*this);
	}
	template	<class	T>	inline	size_t	Set<T>::GetLength () const
	{
		return (fRep->GetLength ());
	}
	template	<class	T>	inline	Boolean	Set<T>::IsEmpty () const
	{
		return (Boolean (GetLength () == 0));
	}
	template	<class	T>	inline	Boolean	Set<T>::Contains (T item) const
	{
		return (fRep->Contains (item));
	}
	template	<class	T>	inline	void	Set<T>::RemoveAll ()
	{
		fRep->RemoveAll ();
	}
	template	<class	T>	inline	void	Set<T>::Compact ()
	{
		fRep->Compact ();
	}
	template	<class	T>	inline	Set<T>&	Set<T>::operator+= (T item)
	{
		Add (item);
		return (*this);
	}
	template	<class	T>	inline	Set<T>&	Set<T>::operator+= (const Set<T>& items)
	{
		Add (items);
		return (*this);
	}
	template	<class	T>	inline	Set<T>&	Set<T>::operator-= (T item)
	{
		Remove (item);
		return (*this);
	}
	template	<class	T>	inline	Set<T>&	Set<T>::operator-= (const Set<T>& items)
	{
		Remove (items);
		return (*this);
	}
	template	<class	T>	inline	Set<T>::operator Iterator<T> () const
	{
	 	return (((Set<T>*) this)->fRep->MakeIterator ());
	}		
	template	<class	T>	inline	Set<T>::operator SetIterator<T> () const
	{
		return (((Set<T>*) this)->fRep->MakeSetIterator ());
	}
	template	<class	T>	inline	Set<T>::operator SetMutator<T> ()
	{
		return (fRep->MakeSetMutator ());
	}
	template	<class	T>	inline	const SetRep<T>*	Set<T>::GetRep () const
	{
		return ((const SetRep<T>*) fRep.GetPointer ());
	}
	template	<class	T>	inline	SetRep<T>*	Set<T>::GetRep ()
	{
		return ((SetRep<T>*) fRep.GetPointer ());
	}


	// operator!=
	template	<class T> inline	Boolean	operator!= (const Set<T>& lhs, const Set<T>& rhs)
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
	#define qIncluding_SetCC	1
		#include	"../Sources/Set.cc"
	#undef  qIncluding_SetCC
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Set__*/

