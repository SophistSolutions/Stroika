/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Deque__cc
#define	__Deque__cc

/*
 * $Header: /fuji/lewis/RCS/Deque.cc,v 1.14 1992/12/04 16:29:11 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Deque.cc,v $
 *		Revision 1.14  1992/12/04  16:29:11  lewis
 *		Pop and Dequeue() are gone - confusing names. op+= gone.
 *
 *		Revision 1.13  1992/11/29  02:45:21  lewis
 *		Work around qGCC_ConversionOperatorInitializerConfusionBug.
 *
 *		Revision 1.12  1992/11/20  19:23:36  lewis
 *		Fix operator== to use new Iterator stuff and it.More().
 *
 *		Revision 1.11  1992/11/13  03:41:20  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *
 *		Revision 1.10  1992/11/12  08:20:55  lewis
 *		*** empty log message ***
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
 *		Revision 1.7  1992/10/22  04:05:33  lewis
 *		Moved inlines to implementation section, moved concrete reps to .cc files.
 *		Got rid of unneeded methods (like GetReps in concrete class where
 *		not used). DequeRep no longer subclasses from CollectionRep - do
 *		in particular subclasses where needed.
 *
 *		Revision 1.6  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.5  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.4  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.2  1992/09/11  16:32:52  sterling
 *		used new Shared implementaiton
 *
 */


#include	"Debug.hh"
#include	"Deque.hh"				// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Deque_CircularArray.hh"






#if		qRealTemplatesAvailable

/*
 ********************************************************************************
 *************************************** Deque **********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Deque;					// tmp hack so GenClass will fixup below array CTOR
														// harmless, but silly

#endif

template	<class T>	Deque<T>::Deque () :
	fRep (&Clone, Nil)
{
	*this = Deque_CircularArray<T> ();
}

template	<class T> DequeRep<T>*	Deque<T>::Clone (const DequeRep<T>& rep)
{
	return (rep.Clone ());
}




/*
 ********************************************************************************
 *********************************** operator== *********************************
 ********************************************************************************
 */
template	<class T> Boolean	operator== (const Deque<T>& lhs, const Deque<T>& rhs)
{
	if (lhs.GetRep () == rhs.GetRep ()) {
		return (True);
	}
	if (lhs.GetLength () != rhs.GetLength ()) {
		return (False);
	}

#if		qGCC_ConversionOperatorInitializerConfusionBug
	Iterator<T>	it1 = (Iterator<T>)rhs;
#else
	Iterator<T>	it1 = rhs;
#endif
	it1.More ();
	ForEach (T, it, lhs) {
		if (not (it.Current () == it1.Current ())) {
			return (False);
			break;
		}
		(void)it1.More ();
	}
	return (True);
}


#endif	/*qRealTemplatesAvailable*/

#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

