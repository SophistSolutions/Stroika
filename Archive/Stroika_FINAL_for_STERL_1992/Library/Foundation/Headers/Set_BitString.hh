/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Set_BitString__
#define	__Set_BitString__

/*
 * $Header: /fuji/lewis/RCS/Set_BitString.hh,v 1.22 1992/11/11 03:55:13 lewis Exp $
 *
 * Description:
 *		Use the BitString template to implement a Set abstraction.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Set_BitString.hh,v $
 *		Revision 1.22  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.21  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.20  1992/10/15  02:13:23  lewis
 *		Move SetRep_BitString<T> and iterators into .cc file, and move
 *		related inlines that were here.
 *		Get rid of hintSize CTOR.
 *
 *		Revision 1.19  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.18  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.17  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.16  1992/10/02  21:44:34  lewis
 *		Straighten out macros, and template bug to get to work with GenClass.
 *
 *		Revision 1.15  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.14  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.13  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default rep
 *		if no arg given.
 *
 *		Revision 1.12  1992/09/12  02:42:31  lewis
 *		Got rid of separate Rep.hh files - put into AbXX files inline.
 *
 *		Revision 1.11  1992/09/11  16:14:29  sterling
 *		use new Shared implementation
 *
 *		Revision 1.10  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.9  1992/09/04  20:43:03  sterling
 *		Added Contains () override.
 *
 *		Revision 1.7  1992/07/21  17:27:09  lewis
 *		In UpdateCurrent () we must cast data field to non-const.
 *
 *		Revision 1.6  1992/07/21  06:25:08  lewis
 *		Do both const and non-const overloads of Peek () method.
 *		Cast value in call to remove in UpdateCurent of iterator to convert
 *		from our type T to index in BitString.
 *		Use SizeInContainer instead of Container::Size as a workaround for
 *		CFront 2.1 bugs with nested types, and needed for macros - we do it
 *		in the template here too so we can automate coversion from templates
 *		to macros more easily.
 *
 *		Revision 1.5  1992/07/19  02:33:48  lewis
 *		Made compatable with latest revs to Collection stuff, cleaning up
 *		template implemenation, and re-translating it into the macro implemenation.
 *
 *		Revision 1.4  1992/07/17  18:26:16  lewis
 *		Cleaned up comments, and starting updating for new changes in
 *		Collection - new iterator/mutator stuff and getting rid of private
 *		list of iterators to patch - now done by base class.
 *		Replaced old qTemplatesHasRepository with a new, much more elaborate, and
 *		hopefully more correct version.
 *
 *		Revision 1.3  1992/07/16  07:51:28  lewis
 *		Change magic include at end to be depend on "qRealTemplatesAvailable && !!qTemplatesHasRepository"
 *		instead of qBorlandCPlus.
 *
 *		Revision 1.2  1992/07/08  05:33:23  lewis
 *		Changed file name to include for template .cc stuff to UNIX pathname,
 *		and use PCName mapper to rename to appropriate PC File - cuz we may need
 *		to do this with gcc/aux too.
 *		Also, so I have PC Names (mapped to names) in just one place - so they
 *		are easier to change.
 *
 *		Revision 1.11  1992/05/09  00:58:35  lewis
 *		Ported to BC++/Tempates/PC.
 *
 *		Revision 1.10  92/03/30  13:33:23  13:33:23  lewis (Lewis Pringle)
 *		Implemented templates version.
 *		
 *		Revision 1.8  1992/02/21  18:43:41  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem in name shorting, and got
 *		rid of outofline destructor.
 *
 *
 */

#include	"Set.hh"


#if		qRealTemplatesAvailable

template	<class T> class	Set_BitString : public Set<T> {
	public:
		Set_BitString ();
		Set_BitString (const T* items, size_t size);
		Set_BitString (const Set<T>& src);
		Set_BitString (const Set_BitString<T>& src);

		nonvirtual	Set_BitString<T>& operator= (const Set_BitString<T>& src);
};

#endif	/*qRealTemplatesAvailable*/



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
	template	<class T> inline	Set_BitString<T>::Set_BitString (const Set_BitString<T>& src) :
		Set<T> (src)
	{
	}
	template	<class T> inline	Set_BitString<T>& Set_BitString<T>::operator= (const Set_BitString<T>& src)
	{
		Set<T>::operator= (src);
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
	#define qIncluding_Set_BitStringCC	1
		#include	"../Sources/Set_BitString.cc"
	#undef  qIncluding_Set_BitStringCC
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Set_BitString__*/

