/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Queue__cc
#define	__Queue__cc

/*
 * $Header: /fuji/lewis/RCS/Queue.cc,v 1.17 1992/12/04 16:27:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Queue.cc,v $
 *		Revision 1.17  1992/12/04  16:27:29  lewis
 *		Got rid of operator+=
 *
 *		Revision 1.16  1992/11/25  15:57:40  lewis
 *		Work around qGCC_ConversionOperatorInitializerConfusionBug.
 *
 *		Revision 1.15  1992/11/20  19:24:09  lewis
 *		Fix operator== to use new Iterator stuff and it.More().
 *
 *		Revision 1.14  1992/11/13  03:47:06  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *
 *		Revision 1.13  1992/11/12  08:11:54  lewis
 *		Add #if           !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
 *		around predeclare of type name for Gen‚lass workaround.
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
 *		Revision 1.10  1992/10/15  20:22:36  lewis
 *		Syntax Error.
 *
 *		Revision 1.9  1992/10/15  13:22:53  lewis
 *		Moved inlines in class declaration down to implemenation detail section,
 *		moved concrete rep classes to .cc file, and related inlines.
 *		Got rid of CollectionRep from QueueRep, and so mix in in each concrete
 *		rep class now.
 *
 *		Revision 1.8  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.7  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.6  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.5  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.2  1992/09/11  16:32:52  sterling
 *		used new Shared implementaiton
 *
 */



#include	"Debug.hh"
#include	"Queue.hh"				// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Queue_CircularArray.hh"






#if		qRealTemplatesAvailable
	
/*
 ********************************************************************************
 *************************************** Queue **********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Queue;	// Tmp hack so GenClass will fixup following CTOR/DTORs
										// Harmless, but silly.

#endif

template	<class T>	Queue<T>::Queue () :
	fRep (&Clone, Nil)
{
	*this = Queue_CircularArray<T> ();
}

template	<class T>	QueueRep<T>*	Queue<T>::Clone (const QueueRep<T>& src)
{
	return (src.Clone ());
}



/*
 ********************************************************************************
 ********************************** operator== **********************************
 ********************************************************************************
 */
template	<class T>	Boolean	operator== (const Queue<T>& lhs, const Queue<T>& rhs)
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

