/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Tally_LinkList__
#define	__Tally_LinkList__

/*
 * $Header: /fuji/lewis/RCS/Tally_LinkList.hh,v 1.10 1992/11/11 03:55:13 lewis Exp $
 *
 * Description:
 *		Use the LinkList template to implement a src abstraction.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Tally_LinkList.hh,v $
 *		Revision 1.10  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.9  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.8  1992/10/19  21:42:07  lewis
 *		Move inlines from class declaration to implemenation details section at end
 *		of header, and moved concrete reps to .cc file. Added #includes where needed
 *		for GenClass stuff. Other minor cleanups - untested yet.
 *
 *		Revision 1.7  1992/10/10  20:21:39  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.6  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.5  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.4  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.3  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.2  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *
 */

#include	"Tally.hh"


#if		qRealTemplatesAvailable

template	<class T> class	Tally_LinkList : public Tally<T> {
	public:
		Tally_LinkList ();
		Tally_LinkList (const Tally<T>& src);
		Tally_LinkList (const Tally_LinkList<T>& src);
		Tally_LinkList (const T* items, size_t size);

		nonvirtual	Tally_LinkList<T>& operator= (const Tally_LinkList<T>& src);
};

#endif	/*qRealTemplatesAvailable*/




/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#if		qRealTemplatesAvailable
	template	<class T> inline	Tally_LinkList<T>::Tally_LinkList (const Tally_LinkList<T>& src) :
		Tally<T> (src)
	{
	}
	template	<class T> inline	Tally_LinkList<T>& Tally_LinkList<T>::operator= (const Tally_LinkList<T>& src)
	{
		Tally<T>::operator= (src);
		return (*this);
	}
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
	#define	qIncluding_Tally_LinkListCC	1
		#include	"../Sources/Tally_LinkList.cc"
	#undef	qIncluding_Tally_LinkListCC
#endif

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Tally_LinkList__*/

