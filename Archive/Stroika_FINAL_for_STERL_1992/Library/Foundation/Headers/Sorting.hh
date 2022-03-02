/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Sorting__
#define	__Sorting__

/*
 * $Header: /fuji/lewis/RCS/Sorting.hh,v 1.15 1992/11/23 21:48:29 lewis Exp $
 *
 * Description:
 *
 *		A Sorting is a collection whose elements are ordered by an external
 *	comparison function (operator <). The src to can be added to, removed
 *	from, and iterated over. Adding and removing specify only the item -
 *	where it is added is implied by its value, and the operator < function.
 *	Iteration is defined to go forwards from lesser to greater values -
 *	again, as defined by the ordering function (operator <).
 *
 *		We also require an operator== too be able to remove a given item
 *	from a src, and to check whether or not an item is contained in the src.
 *
 *		Sortings do allow redundencies - that is the same element may be
 *	inserted more than once, and still increase the length of the src.
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Sorting.hh,v $
 *		Revision 1.15  1992/11/23  21:48:29  lewis
 *		Return Iterator<T> instead of IteratorRep<T>* in conversion operators.
 *		Also, use Iterator<T> as argument to things like Add() to avoid]
 *		confusion over who deletes what when.
 *
 *		Revision 1.14  1992/11/17  05:31:07  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.13  1992/11/12  08:11:08  lewis
 *		#include Iterator.hh
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
 *		Revision 1.10  1992/10/22  04:14:43  lewis
 *		Add virtual DTOR for SortingRep - accidentally deleted with CollectionRep.
 *
 *		Revision 1.9  1992/10/15  21:10:48  lewis
 *		Move inlines to implementation section. Moved concrete rep
 *		classes to .cc files, and related inlines. #include SharedRep.hh for
 *		non-templated GenClass stuff. Got to compile (not tested yet) with
 *		GenClass.
 *
 *		Revision 1.8  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.7  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.6  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.5  1992/10/02  04:14:22  lewis
 *		Fixed marco names, and add clone arg to fRep initializers - all for getting
 *		GenClass to work.
 *
 *		Revision 1.4  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.3  1992/09/21  04:25:49  lewis
 *		Added SharedImplement(SortingRep(T)) to implement macro.
 *		Added forgotten implemenations of add and remove for iterator(T)*.
 *
 *		Revision 1.2  1992/09/20  01:19:50  lewis
 *		Applied lots of Patches to get rid of Collection base class, and
 *		converted to using < instead of <=.
 *
 *
 */
 
#include	"Debug.hh"
#include	"Iterator.hh"
#include	"Shared.hh"


#if		qRealTemplatesAvailable

template	<class	T>	class	SortingRep;
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfSortingRepOf<T>_hh
#endif

template	<class T> class	Sorting {
	public:
		Sorting ();
		Sorting (const Sorting<T>& src);
		Sorting (const T* items, size_t size);

	protected:
		Sorting (SortingRep<T>* src);

	public:
		nonvirtual	Sorting<T>& operator= (const Sorting<T>& src);

		nonvirtual	size_t		GetLength () const;
		nonvirtual	Boolean		IsEmpty () const;
		nonvirtual	Boolean		Contains (T item) const;
		nonvirtual	void		RemoveAll ();
		nonvirtual	void		Compact ();
	 	nonvirtual	operator Iterator<T> () const;

		nonvirtual	void	Add (T item);
		nonvirtual	void	Add (const Iterator<T>& itemsIterator);
		nonvirtual	void	Remove (T item);
		nonvirtual	void	Remove (const Iterator<T>& itemsIterator);

		nonvirtual	Sorting<T>&	operator+= (T item);
		nonvirtual	Sorting<T>&	operator+= (const Iterator<T>& itemsIterator);
		nonvirtual	Sorting<T>&	operator-= (T item);
		nonvirtual	Sorting<T>&	operator-= (const Iterator<T>& itemsIterator);

		nonvirtual	size_t	OccurencesOf (T item) const;

	protected:
		nonvirtual	void	AddItems (const T* items, size_t size);
		nonvirtual	const SortingRep<T>*	GetRep () const;
		nonvirtual	SortingRep<T>*			GetRep ();

		Shared<SortingRep<T> >	fRep;

		static	SortingRep<T>*	Clone (const SortingRep<T>& src);

	friend	Boolean	operator== (const Sorting<T>& lhs, const Sorting<T>& rhs);
};

template	<class T> Boolean	operator== (const Sorting<T>& lhs, const Sorting<T>& rhs);
template	<class T> Boolean	operator!= (const Sorting<T>& lhs, const Sorting<T>& rhs);


template	<class	T>	class	SortingRep {
	protected:
		SortingRep ();

	public:
		virtual ~SortingRep ();

	public:
		virtual	SortingRep<T>*	Clone () const		=	Nil;
		virtual	Boolean	Contains (T item) const		=	Nil;
		virtual	size_t	GetLength () const 			=	Nil;
		virtual	void	Compact ()					=	Nil;
	 	virtual	void	RemoveAll () 				=	Nil;
		virtual	IteratorRep<T>* MakeIterator ()		=	Nil;
		virtual	void	Add (T item)				=	Nil;
		virtual	void	Remove (T item)				=	Nil;
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

	// class SortingRep<T>
	template	<class	T>	inline	SortingRep<T>::SortingRep ()
	{
	}
	template	<class	T>	inline	SortingRep<T>::~SortingRep ()
	{
	}

	// class Sorting<T>
	template	<class	T>	inline	Sorting<T>::Sorting (const Sorting<T>& src) :
		fRep (src.fRep)
	{
	}
	template	<class	T>	inline	Sorting<T>::Sorting (SortingRep<T>* src) :
		fRep (&Clone, src)
	{
	}
	template	<class	T>	inline	Sorting<T>& Sorting<T>::operator= (const Sorting<T>& src)
	{
		fRep = src.fRep;
		return (*this);
	}
	template	<class	T>	inline	size_t	Sorting<T>::GetLength () const
	{
		return (fRep->GetLength ());
	}
	template	<class	T>	inline	Boolean	Sorting<T>::IsEmpty () const
	{
		return (Boolean (GetLength () == 0));
	}
	template	<class	T>	inline	Boolean	Sorting<T>::Contains (T item) const
	{
		return (fRep->Contains (item));
	}
	template	<class	T>	inline	void	Sorting<T>::RemoveAll ()
	{
		fRep->RemoveAll ();
	}
	template	<class	T>	inline	void	Sorting<T>::Compact ()
	{
		fRep->Compact ();
	}
	template	<class	T>	inline	Sorting<T>::operator Iterator<T> () const
	{
		return (((Sorting<T>*) this)->fRep->MakeIterator ());
	}
	template	<class	T>	inline	Sorting<T>&	Sorting<T>::operator+= (T item)
	{
		Add (item);
		return (*this);
	}
	template	<class	T>	inline	Sorting<T>&	Sorting<T>::operator+= (const Iterator<T>& itemsIterator)
	{
		Add (itemsIterator);
		return (*this);
	}
	template	<class	T>	inline	Sorting<T>&	Sorting<T>::operator-= (T item)
	{
		Remove (item);
		return (*this);
	}
	template	<class	T>	inline	Sorting<T>&	Sorting<T>::operator-= (const Iterator<T>& itemsIterator)
	{
		Remove (itemsIterator);
		return (*this);
	}
	template	<class	T>	inline	const SortingRep<T>*	Sorting<T>::GetRep () const
	{
		return ((const SortingRep<T>*) fRep.GetPointer ());
	}
	template	<class	T>	inline	SortingRep<T>*	Sorting<T>::GetRep ()
	{
		return ((SortingRep<T>*) fRep.GetPointer ());
	}

	// operator!=
	template	<class T> inline	Boolean	operator!= (const Sorting<T>& lhs, const Sorting<T>& rhs)
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
	#define qIncluding_SortingCC	1
		#include	"../Sources/Sorting.cc"
	#undef  qIncluding_SortingCC
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Sorting__*/

