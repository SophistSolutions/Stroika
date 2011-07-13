/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Set_Array__
#define	__Set_Array__

/*
 * $Header: /fuji/lewis/RCS/Set_Array.hh,v 1.19 1992/11/11 03:55:13 lewis Exp $
 *
 * Description:
 *		Use the Array template to implement a Set abstraction.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Set_Array.hh,v $
 *		Revision 1.19  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.18  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.17  1992/10/22  04:18:13  lewis
 *		Moved GetReps to .cc file.
 *
 *		Revision 1.16  1992/10/15  02:12:26  lewis
 *		Move SetRep_Array<T> and iterators into .cc file. Also move
 *		related inlines.
 *		Get rid of hintSize CTOR.
 *
 *		Revision 1.15  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.14  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.13  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.12  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.11  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.10  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default src
 *		if no arg given.
 *
 *		Revision 1.9  1992/09/11  16:14:29  sterling
 *		use new Shared implementation
 *
 *		Revision 1.8  1992/09/04  20:41:19  sterling
 *		Contains () method.
 *
 *		Revision 1.6  1992/07/21  06:22:16  lewis
 *		Use SizeInContainer instead of Container::Size as a workaround for
 *		CFront 2.1 bugs with nested types, and needed for macros - we do it
 *		in the template here too so we can automate coversion from templates
 *		to macros more easily.
 *
 *		Revision 1.5  1992/07/18  17:01:37  lewis
 *		Updated these classes for latest changes in Collection stuff,
 *		patching, etc, and redid macro based on template version - both
 *		should now be totally in sync, and tested (compiling) macro version
 *		under gcc.
 *
 *		Revision 1.4  1992/07/17  18:25:33  lewis
 *		Replaced old qTemplatesHasRepository with a new, much more elaborate, and
 *		hopefully more correct version.
 *
 *		Revision 1.3  1992/07/16  07:50:28  lewis
 *		Change magic include at end to be depend on "qRealTemplatesAvailable && !!qTemplatesHasRepository"
 *		instead of qBorlandCPlus.
 *
 *		Revision 1.2  1992/07/08  05:20:53  lewis
 *		Changed file name to include for template .cc stuff to UNIX pathname, and
 *		use PCName mapper to rename to appropriate PC File - cuz we may need to do
 *		this with gcc/aux too.
 *
 *		Revision 1.7  1992/05/09  00:56:12  lewis
 *		Ported to BC++/Tempates/PC.
 *
 *		Revision 1.6  92/04/23  17:09:38  17:09:38  lewis (Lewis Pringle)
 *		Did qRealTemplatesAvailable implementation.
 *		
 *		Revision 1.4  1992/02/21  18:41:53  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem in name shorting, and got rid
 *		of outofline destructor.
 *
 *		Revision 1.3  1992/01/22  05:28:46  lewis
 *		Fix for changes in interface of Collection.
 *
 *		Revision 1.2  1992/01/21  20:58:07  lewis
 *		Use the ForEach() macro.
 *
 */

#include	"Set.hh"



#if		qRealTemplatesAvailable

template	<class T> class	SetRep_Array;
template	<class T> class	Set_Array : public Set<T> {
	public:
		Set_Array ();
		Set_Array (const T* items, size_t size);
		Set_Array (const Set<T>& src);
		Set_Array (const Set_Array<T>& src);

		nonvirtual	Set_Array<T>& operator= (const Set_Array<T>& src);

		nonvirtual	size_t	GetSlotsAlloced () const;
		nonvirtual	void	SetSlotsAlloced (size_t slotsAlloced);

	private:
		nonvirtual	const SetRep_Array<T>*	GetRep () const;
		nonvirtual	SetRep_Array<T>*		GetRep ();
};

#endif	/*qRealTemplatesAvailable*/






/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
	template	<class T> inline	Set_Array<T>::Set_Array (const Set_Array<T>& src) :
		Set<T> (src)
	{
	}
	template	<class T> inline	Set_Array<T>& Set_Array<T>::operator= (const Set_Array<T>& src)
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
	#define qIncluding_Set_ArrayCC	1
		#include	"../Sources/Set_Array.cc"
	#undef  qIncluding_Set_ArrayCC
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Set_Array__*/

