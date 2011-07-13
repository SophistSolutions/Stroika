/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PriorityQueue_LinkList__
#define	__PriorityQueue_LinkList__

/*
 * $Header: /fuji/lewis/RCS/PriorityQueue_LinkList.hh,v 1.13 1992/11/13 03:36:34 lewis Exp $
 *
 * Description:
 *		Use the LinkList template to implement a PriorityQueue abstraction.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: PriorityQueue_LinkList.hh,v $
 *		Revision 1.13  1992/11/13  03:36:34  lewis
 *		#if 0'd out everything - fix later.
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
 *		Revision 1.10  1992/10/23  06:18:39  lewis
 *		Cleaned up some for GenClass stuff, but untested, and looks like it
 *		needs work.
 *
 *		Revision 1.9  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.8  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.7  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.6  1992/09/21  06:00:38  sterling
 *		Lots of changes (LGP checking in, so not sure what).
 *
 *		Revision 1.5  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.4  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default rep
 *		if no arg given.
 *
 *		Revision 1.3  1992/09/11  16:07:26  sterling
 *		use new Shared implementation
 *
 *		Revision 1.2  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *
 *
 */

#include	"PriorityQueue.hh"



#if		qRealTemplatesAvailable && 0

template	<class T>	class	PriorityQueue_LinkList : public PriorityQueue<T> {
	public:
		PriorityQueue_LinkList ();
		PriorityQueue_LinkList (Priority (*f) (T));
		PriorityQueue_LinkList (const PriorityQueue<T>& q);
		PriorityQueue_LinkList (const PriorityQueue_LinkList<T>& q);

		nonvirtual	PriorityQueue_LinkList<T>& operator= (const PriorityQueue_LinkList<T>& q);
};

#endif	/*qRealTemplatesAvailable*/




/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable && 0
	template	<class T>	inline	PriorityQueue_LinkList<T>& PriorityQueue_LinkList<T>::operator= (const PriorityQueue_LinkList<T>& q)
	{
		PriorityQueue<T>::operator= (q);
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
	#define qIncluding_PriorityQueue_LinkListCC	1
		#include	"../Sources/PriorityQueue_LinkList.cc"
	#undef  qIncluding_PriorityQueue_LinkListCC
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__PriorityQueue_LinkList__*/
