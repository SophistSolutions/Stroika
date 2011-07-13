/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Set__cc
#define	__Set__cc

/*
 * $Header: /fuji/lewis/RCS/Set.cc,v 1.11 1992/11/13 03:49:41 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Set.cc,v $
 *		Revision 1.11  1992/11/13  03:49:41  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *		Also, be more careful about !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
 *		hacks.
 *
 *		Revision 1.10  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.9  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.8  1992/10/15  12:50:41  lewis
 *		fRep must be initialized with cloner and nil - no arg ctor
 *		for shared is now private.
 *
 *		Revision 1.7  1992/10/15  02:31:42  lewis
 *		Moved some stuff here from the headers. And include SharedOfSetRepOf<T>_cc.
 *
 *		Revision 1.6  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.5  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.4  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.3  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.2  1992/07/17  16:12:46  lewis
 *		Did/Finished template implementation.
 *
 *
 */



#include	"Debug.hh"
#include	"Set.hh"			// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Set_Array.hh"






/*
 ********************************************************************************
 ****************************************** Set *********************************
 ********************************************************************************
 */

#if		qRealTemplatesAvailable

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Set;	// tmp hack so GenClass will fixup following CTOR/DTORs
										// harmless, but silly

#endif

template	<class T> Set<T>::Set () :
	fRep (&Clone, Nil)
{
	*this = Set_Array<T> ();
}

template	<class T> Set<T>::Set (const T* items, size_t size) :
	fRep (&Clone, Nil)
{
	*this = Set_Array<T> (items, size);
}

template	<class T> void	Set<T>::Add (T item)
{
	fRep->Add (item);
}

template	<class T> void	Set<T>::Add (Set<T> items)
{
	ForEach (T, it, items) {
		fRep->Add (it.Current ());
	}
}

template	<class T> void	Set<T>::Remove (T item)
{
	fRep->Remove (item);
}

template	<class T> void	Set<T>::Remove (const Set<T>& items)
{
	ForEach (T, it, items) {
		fRep->Remove (it.Current ());
	}
}

template	<class T> void	Set<T>::AddItems (const T* items, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		fRep->Add (items[i]);
	}
}

template	<class T> SetRep<T>*	Set<T>::Clone (const SetRep<T>& rep)
{
	return (rep.Clone ());
}




/*
 ********************************************************************************
 ************************************ operators *********************************
 ********************************************************************************
 */

template	<class T> Boolean	operator== (const Set<T>& lhs, const Set<T>& rhs)
{
	if (lhs.GetLength () != rhs.GetLength ()) {
		return (False);
	}
	ForEach (T, it, lhs) {
		if (not rhs.Contains (it.Current ())) {
			return (False);
		}
	}
	return (True);
}

template	<class T> Set<T>	operator+ (const Set<T>& lhs, const Set<T>& rhs)
{
	Set<T> tmp = lhs;
	tmp += rhs;
	return (tmp);
}

template	<class T> Set<T>	operator- (const Set<T>& lhs, const Set<T>& rhs)
{
	Set<T> tmp = lhs;
	tmp -= rhs;
	return (tmp);
}


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfSetRepOf<T>_cc
#endif


#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

