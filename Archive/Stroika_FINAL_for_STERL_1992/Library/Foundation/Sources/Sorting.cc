/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Sorting__cc
#define	__Sorting__cc

/*
 * $Header: /fuji/lewis/RCS/Sorting.cc,v 1.12 1992/11/23 22:46:45 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Sorting.cc,v $
 *		Revision 1.12  1992/11/23  22:46:45  lewis
 *		Work around qGCC_ConversionOperatorInitializerConfusionBug.
 *		Also, use const Iterator<T>& for arg to Add/Remove instead of
 *		IteratorRep<T>*.
 *
 *		Revision 1.11  1992/11/20  19:25:58  lewis
 *		Fix operator== to use new Iterator stuff and it.More().
 *
 *		Revision 1.10  1992/11/13  03:50:51  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *		Also, be more careful about !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
 *		hacks.
 *
 *		Revision 1.9  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.8  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.7  1992/10/15  21:10:48  lewis
 *		Move inlines to implementation section. Moved concrete rep
 *		classes to .cc files, and related inlines. #include SharedRep.hh for
 *		non-templated GenClass stuff. Got to compile (not tested yet) with
 *		GenClass.
 *
 *		Revision 1.6  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.5  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.4  1992/10/02  04:15:19  lewis
 *		Stuff to try to get GenClass working.
 *
 *		Revision 1.3  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.2  1992/09/20  01:20:20  lewis
 *		Applied lots of Patches to get rid of Collection base class, and
 *		converted to using < instead of <=.
 *
 */



#include	"Debug.hh"
#include	"Sorting.hh"				// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Sorting_Array.hh"			// default implementation





#if		qRealTemplatesAvailable

/*
 ********************************************************************************
 *************************************** Sorting ********************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Sorting;	// tmp hack so GenClass will fixup below array CTOR
											// harmless, but silly

#endif

template	<class T>	Sorting<T>::Sorting () :
	fRep (&Clone, Nil)
{
	*this = Sorting_Array<T> ();
}

template	<class T>	Sorting<T>::Sorting (const T* items, size_t size) :
	fRep (&Clone, Nil)
{
	*this = Sorting_Array<T> (items, size);
}

template	<class T> void	Sorting<T>::Add (T item)
{
	GetRep ()->Add (item);
	Ensure (Contains (item));
}

template	<class T> void	Sorting<T>::Add (const Iterator<T>& itemsIterator)
{
	ForEach (T, it, itemsIterator) {
		GetRep ()->Add (it.Current ());
	}
}

template	<class T> void	Sorting<T>::Remove (T item)
{
	Require (Contains (item));
	GetRep ()->Remove (item);
}

template	<class T> void	Sorting<T>::Remove (const Iterator<T>& itemsIterator)
{
	ForEach (T, it, itemsIterator) {
		GetRep ()->Remove (it.Current ());
	}
}

template	<class T> void	Sorting<T>::AddItems (const T* items, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		Add (items[i]);
	}
}

template	<class T> SortingRep<T>*	Sorting<T>::Clone (const SortingRep<T>& src)
{
	return (src.Clone ());
}



/*
 ********************************************************************************
 ************************************ operator== ********************************
 ********************************************************************************
 */

template	<class T> Boolean	operator== (const Sorting<T>& lhs, const Sorting<T>& rhs)
{
	if (lhs.GetRep () == rhs.GetRep ()) {
		return (True);
	}
	if (lhs.GetLength () != rhs.GetLength ()) {
		return (False);
	}

	/*
	 * Since equality for a sorting is defined to mean that you have the same
	 * elements (and since the elements are - of course - in the same order)
	 * we can just iterate over both sortings at the same time, and assure
	 * the sortings are equal at each item.
	 */
#if		qGCC_ConversionOperatorInitializerConfusionBug
	Iterator<T>	rhsIt	=	(Iterator<T>)rhs;
#else
	Iterator<T>	rhsIt	=	rhs;
#endif
	(void)rhsIt.More ();
	ForEach (T, it, lhs) {
		if (not (it.Current () == rhsIt.Current ())) {
			return (False);
		}
		(void)rhsIt.More ();
	}
	return (True);
}

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfSortingRepOf<T>_cc
#endif

#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

