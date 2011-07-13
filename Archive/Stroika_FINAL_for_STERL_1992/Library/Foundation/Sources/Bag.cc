/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Bag__cc
#define	__Bag__cc

/*
 * $Header: /fuji/lewis/RCS/Bag.cc,v 1.19 1992/12/04 17:48:57 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Bag.cc,v $
 *		Revision 1.19  1992/12/04  17:48:57  lewis
 *		Renamed OccurencesOf() -> TallyOf().
 *
 *		Revision 1.18  1992/11/13  03:38:21  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *
 *		Revision 1.16  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.15  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.14  1992/10/15  12:50:41  lewis
 *		fRep must be initialized with cloner and nil - no arg ctor
 *		for shared is now private.
 *
 *		Revision 1.13  1992/10/15  02:24:09  lewis
 *		Lots of cleanups, and added #include SharedOfBagRepOf<T>_cc if !qRealTemplatesAvailable.
 *
 *		Revision 1.12  1992/10/14  02:26:02  lewis
 *		Cleanup format - move all inlines to the bottom of the file in
 *		implementation section. Move declarations of concrete iterators
 *		(except bag.hh) to the .cc file.
 *
 *		Revision 1.11  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.10  1992/10/02  04:15:19  lewis
 *		Stuff to try to get GenClass working.
 *
 *		Revision 1.9  1992/09/29  20:18:01  lewis
 *		Fixed bug in AddItems () again - macro version only - not sure how it
 *		got lost.
 *		Also, put back include of .cc file at end of header - also not sure
 *		how it got lost - and moved all(most) of the inline methods
 *		out of class declarations and put them into the inline section
 *		where they belong. Fixed minor bugs with some of the methods (unnessary
 *		casts).
 *
 *		Revision 1.8  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.7  1992/09/26  22:27:48  lewis
 *		Updated for new GenClass stuff - got to compile - havent tried linking.
 *
 *		Revision 1.6  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.5  1992/09/20  01:12:52  lewis
 *		AddItems() should loop to < size, not <=.
 *
 *		Revision 1.4  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.2  1992/09/11  16:32:52  sterling
 *		used new Shared implementaiton
 *
 */

#include	"Bag.hh"				// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Bag_Array.hh"			// default implementation
#include	"Debug.hh"






#if		qRealTemplatesAvailable

/*
 ********************************************************************************
 *************************************** Bag ************************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Bag;	// tmp hack so GenClass will fixup below array CTOR
										// harmless, but silly

#endif

template	<class T>	Bag<T>::Bag () :
	fRep (&Clone, Nil)
{
	*this = Bag_Array<T> ();
}

template	<class T>	Bag<T>::Bag (const T* items, size_t size) :
	fRep (&Clone, Nil)
{
	*this = Bag_Array<T> (items, size);
}

template	<class T> void	Bag<T>::Add (T item)
{
	GetRep ()->Add (item);
	Ensure (not IsEmpty ());
}

template	<class T> void	Bag<T>::Add (Bag<T> items)
{
	ForEach (T, it, items) {
		GetRep ()->Add (it.Current ());
	}
}

template	<class T> void	Bag<T>::Remove (T item)
{
	GetRep ()->Remove (item);
}

template	<class T> void	Bag<T>::Remove (const Bag<T>& items)
{
	if (GetRep () == items.GetRep ()) {
		RemoveAll ();
	}
	else {
		ForEach (T, it, items) {
			GetRep ()->Remove (it.Current ());
		}
	}
}

template	<class T> size_t	Bag<T>::TallyOf (T item) const
{
	size_t count = 0;
	ForEach (T, it, *this) {
		if (it.Current () == item) {
			count++;
		}
	}
	return (count);
}

template	<class T> void	Bag<T>::AddItems (const T* items, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		Add (items[i]);
	}
}

template	<class T>	BagRep<T>*	Bag<T>::Clone (const BagRep<T>& rep)
{
	return (rep.Clone ());
}



/*
 ********************************************************************************
 ************************************ operators *********************************
 ********************************************************************************
 */
template	<class T> Boolean	operator== (const Bag<T>& lhs, const Bag<T>& rhs)
{
	if (lhs.GetRep () == rhs.GetRep ()) {
		return (True);
	}
	if (lhs.GetLength () != rhs.GetLength ()) {
		return (False);
	}

	// n^2 algorithm!!!
	ForEach (T, it, lhs) {
		if (lhs.TallyOf (it.Current ()) != rhs.TallyOf (it.Current ())) {
			return (False);
		}
	}
	return (True);
}

template	<class T> Bag<T>	operator+ (const Bag<T>& lhs, const Bag<T>& rhs)
{
	Bag<T>	temp	= lhs;
	temp += rhs;
	return (temp);
}

template	<class T> Bag<T>	operator- (const Bag<T>& lhs, const Bag<T>& rhs)
{
	Bag<T>	temp	= lhs;
	temp -= rhs;
	return (temp);
}


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfBagRepOf<T>_cc
#endif

#endif	/*qRealTemplatesAvailable*/

#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

