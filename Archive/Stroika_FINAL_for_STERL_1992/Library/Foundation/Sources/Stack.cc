/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Stack__cc
#define	__Stack__cc

/*
 * $Header: /fuji/lewis/RCS/Stack.cc,v 1.14 1992/12/04 16:13:13 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Stack.cc,v $
 *		Revision 1.14  1992/12/04  16:13:13  lewis
 *		Got rid of Pop - now inline call to REP.
 *
 *		Revision 1.13  1992/11/23  22:46:06  lewis
 *		Work around qGCC_ConversionOperatorInitializerConfusionBug.
 *
 *		Revision 1.12  1992/11/20  19:24:24  lewis
 *		Fix operator== to use new Iterator stuff and it.More().
 *
 *		Revision 1.11  1992/11/13  03:45:26  lewis
 *		Added missing template      <class  T>.
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *		Work around !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug.
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
 *		Revision 1.8  1992/10/15  20:23:10  lewis
 *		Syntax Error.
 *
 *		Revision 1.7  1992/10/15  02:34:59  lewis
 *		Dont reference StackRep_Array any longer - use Stack_Array, and op=.
 *
 *		Revision 1.6  1992/10/14  06:04:02  lewis
 *		Cleanups - moved inlines to implementaion section at end of header
 *		file, and moved things we could to the .cc file. Got rid of method
 *		defintions from inside class declarations.
 *
 *		Revision 1.5  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.4  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.3  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *
 */



#include	"Debug.hh"
#include	"Stack.hh"				// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Stack_Array.hh"





#if		qRealTemplatesAvailable
	
/*
 ********************************************************************************
 *************************************** Stack **********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Stack;	// tmp hack so GenClass will fixup below CTOR/DTORs
										// harmless, but silly

#endif

template	<class	T>	Stack<T>::Stack () :
	fRep (&Clone, Nil)
{
	*this = Stack_Array<T> ();
}

template	<class	T>	StackRep<T>*	Stack<T>::Clone (const StackRep<T>& src)
{
	return (src.Clone ());
}





/*
 ********************************************************************************
 ************************************ operator== ********************************
 ********************************************************************************
 */
template	<class	T>	Boolean	operator== (const Stack<T>& lhs, const Stack<T>& rhs)
{
	if (lhs.GetRep () == rhs.GetRep ()) {
		return (True);
	}
	if (lhs.GetLength () != rhs.GetLength ()) {
		return (False);
	}
	
#if		qGCC_ConversionOperatorInitializerConfusionBug
	Iterator<T>	it1	=	(Iterator<T>)rhs;
#else
	Iterator<T>	it1	=	rhs;
#endif
	(void)it1.More ();
	ForEach (T, it, lhs) {
		if (not (it.Current () == it1.Current ())) {
			return (False);
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

