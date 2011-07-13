/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Sequence_DoubleLinkList__
#define	__Sequence_DoubleLinkList__

/*
 * $Header: /fuji/lewis/RCS/Sequence_DoubleLinkList.hh,v 1.27 1992/11/11 03:55:13 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Sequence_DoubleLinkList.hh,v $
 *		Revision 1.27  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.26  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.25  1992/10/15  02:09:41  lewis
 *		Move SequenceRep_DoubleLinkList inlines into .cc file.
 *
 *		Revision 1.24  1992/10/14  16:24:59  lewis
 *		Got rid of GetRep methods since unused, and this class not meant
 *		to be subclassed.
 *		Moved rep definition to .cc file.
 *
 *		Revision 1.23  1992/10/14  02:55:51  lewis
 *		Move interators to .cc file (declarations) and move inlines to
 *		Implemenation Detail section at end of header file.
 *
 *		Revision 1.22  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.21  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.20  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.19  1992/09/25  21:14:26  lewis
 *		Fix lots of templates problems and get working using new GenClass stuff.
 *
 *		Revision 1.18  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.17  1992/09/21  04:24:53  lewis
 *		#if 1 -> #if 0 wrapper around test declare macros at the end - was
 *		left in inadvertantly.
 *
 *		Revision 1.16  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.15  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default rep
 *		if no arg given.
 *
 *		Revision 1.14  1992/09/11  16:14:29  sterling
 *		use new Shared implementation
 *
 *		Revision 1.13  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.12  1992/09/04  20:39:13  sterling
 *		Added Contains () method.
 *
 *		Revision 1.10  1992/07/21  17:29:48  lewis
 *		Work around qMPW_TEMP_OF_CLASS_WITH_DTOR_IN_EXPR_BUG.
 *
 *		Revision 1.9  1992/07/21  16:02:38  lewis
 *		Add missing cast in operator delete.
 *
 *		Revision 1.8  1992/07/21  06:35:35  lewis
 *		Work on macro version of op new for iterators cuz we cannot use
 *		BlockAllocated directly.
 *
 *		Revision 1.7  1992/07/19  08:54:30  lewis
 *		Nearly total rewrite of template version and then use that to
 *		generate macro version from scratch. Got macro version to compile
 *		on gcc/aux.
 *
 *		Revision 1.6  1992/07/17  18:25:03  lewis
 *		Replaced old qTemplatesHasRepository with a new, much more elaborate, and
 *		hopefully more correct version.
 *
 *		Revision 1.5  1992/07/16  07:15:13  lewis
 *		Change magic include at end to be depend on "qRealTemplatesAvailable && !!qTemplatesHasRepository"
 *		instead of qBorlandCPlus.
 *
 *		Revision 1.4  1992/07/14  20:52:17  lewis
 *		Cleanup headers. Comment out obsolete fCurrentIterors / fNextIterator
 *		in template - done in base classes Collection/CollectionView now.
 *		Added override of MakeSequenceMutator - accidentally overlooked.
 *
 *		Revision 1.3  1992/07/08  05:38:43  lewis
 *		Changed file name to include for template .cc stuff to UNIX pathname, and
 *		use PCName mapper to rename to appropriate PC File - cuz we may need to do
 *		this with gcc/aux too.
 *		Also, so I have PC Names (mapped to names) in just one place - so they are
 *		easier to change.
 *
 *		Revision 1.2  1992/07/02  03:10:06  lewis
 *		Renamed Sequence_DoublyLinkedList Sequence_DoubleLinkList.
 *
 *		Revision 1.22  1992/06/10  10:16:47  lewis
 *		Fix minor syntax error in macro - simple, but took an hour to find!!!
 *
 *		Revision 1.21  1992/06/10  04:13:31  lewis
 *		Got rid of Sort function - do later as an external object operating on
 *		a seq, ala Stroustrups second edition.
 *
 *		Revision 1.20  92/05/09  01:09:41  01:09:41  lewis (Lewis Pringle)
 *		Ported to BC++ and fixed templates stuff.
 *		
 *		Revision 1.19  92/03/30  16:52:19  16:52:19  lewis (Lewis Pringle)
 *		Implemted template support.
 *		
 *		Revision 1.17  1992/02/27  21:02:21  lewis
 *		Added support for new Sequence notion of current index. Probably not
 *		very well done, and some bugs with seq updates during iteration
 *		noted herein.
 *
 *		Revision 1.16  1992/02/21  18:35:17  lewis
 *		Got rid of workarounds for qMPW_SymbolTableOverflowProblem.
 *
 *		Revision 1.15  1992/02/04  22:44:06  lewis
 *		Get rid of Append/Prepend overrides and put Append override optimization
 *		into InsertAt.
 *		Also, get rid of g++ backward compatability.
 *
 *		Revision 1.14  1992/01/22  05:28:13  lewis
 *		Fix for changes in interface of Collection.
 *
 *		Revision 1.13  1992/01/22  04:14:35  lewis
 *		Fix bug in bubble sort.
 *
 *		Revision 1.12  1992/01/22  02:57:42  lewis
 *		Add Sort method (sloppy bubble sort).
 *
 *		Revision 1.11  1992/01/21  20:57:20  lewis
 *		Use foreach macro, and add standard rcs header stuff.
 *
 *
 *
 */

#include	"Sequence.hh"



#if		qRealTemplatesAvailable

template	<class T> class	Sequence_DoubleLinkList : public Sequence <T> {
	public:
		Sequence_DoubleLinkList ();
		Sequence_DoubleLinkList (const Sequence<T>& seq);
		Sequence_DoubleLinkList (const Sequence_DoubleLinkList<T>& seq);
		Sequence_DoubleLinkList (const T* items, size_t size);

		nonvirtual	Sequence_DoubleLinkList<T>& operator= (const Sequence_DoubleLinkList<T>& seq);
};

#endif	/*qRealTemplatesAvailable*/







/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
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
	#define	qIncluding_Sequence_DoubleLinkListCC	1
		#include	"../Sources/Sequence_DoubleLinkList.cc"
	#undef	qIncluding_Sequence_DoubleLinkListCC
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Sequence_DoubleLinkList__*/
