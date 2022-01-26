/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Sorting_Array__
#define	__Sorting_Array__

/*
 * $Header: /fuji/lewis/RCS/Sorting_Array.hh,v 1.11 1992/11/01 01:26:35 lewis Exp $
 *
 * Description:
 *		Use an Array to implement a Sorting abstraction.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Sorting_Array.hh,v $
 *		Revision 1.11  1992/11/01  01:26:35  lewis
 *		New Array Iterator support with separate subclass for patching.
 *
 *		Revision 1.10  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
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
 *		Revision 1.5  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.4  1992/09/23  13:45:49  lewis
 *		Fixed bad assert in Add.
 *
 *		Revision 1.3  1992/09/21  04:27:06  lewis
 *		Lengthed name for sorting macro (was manged for MPW) and
 *		fix assert in Add method - was backwards.
 *
 *		Revision 1.2  1992/09/20  01:22:36  lewis
 *		Applied lots of Patches to get rid of Collection base class, and
 *		converted to using < instead of <=.
 *
 *
 *
 */

#include	"Sorting.hh"



#if		qRealTemplatesAvailable

template	<class T> class	Sorting_ArrayRep;
template	<class T> class	Sorting_Array : public Sorting<T> {
	public:
		Sorting_Array ();
		Sorting_Array (const T* items, size_t size);
		Sorting_Array (const Sorting<T>& sorting);
		Sorting_Array (const Sorting_Array<T>& sorting);

		nonvirtual	Sorting_Array<T>& operator= (const Sorting_Array<T>& sorting);

		nonvirtual	size_t	GetSlotsAlloced () const;
		nonvirtual	void	SetSlotsAlloced (size_t slotsAlloced);

	private:
		nonvirtual	const Sorting_ArrayRep<T>*	GetRep () const;
		nonvirtual	Sorting_ArrayRep<T>*		GetRep ();
};

#endif	/*qRealTemplatesAvailable*/




/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
	template	<class	T>	inline	Sorting_Array<T>::Sorting_Array (const Sorting_Array<T>& sorting) :
		Sorting<T> (sorting)
	{
		// Since exact type is correct, we just bump reference count--
		// otherwise we would have to copy...
	}
	template	<class	T>	inline	Sorting_Array<T>& Sorting_Array<T>::operator= (const Sorting_Array<T>& sorting)
	{
		// Since exact type is correct, we just bump reference count--
		// otherwise we would have to copy...
		Sorting<T>::operator= (sorting);
		return (*this);
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
	#define qIncluding_Sorting_ArrayCC	1
		#include	"../Sources/Sorting_Array.cc"
	#undef  qIncluding_Sorting_ArrayCC
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Sorting_Array__*/

