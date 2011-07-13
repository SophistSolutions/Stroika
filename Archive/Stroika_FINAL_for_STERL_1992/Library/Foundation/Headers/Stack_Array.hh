/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Stack_Array__
#define	__Stack_Array__

/*
 * $Header: /fuji/lewis/RCS/Stack_Array.hh,v 1.19 1992/11/01 01:26:35 lewis Exp $
 *
 * Description:
 *		Use the Array template to implement a Stack abstraction.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Stack_Array.hh,v $
 *		Revision 1.19  1992/11/01  01:26:35  lewis
 *		New Array Iterator support with separate subclass for patching.
 *
 *		Revision 1.18  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.17  1992/10/15  02:15:20  lewis
 *		Move StackRep_Array<T> and related inlines to .cc file.
 *
 *		Revision 1.16  1992/10/14  06:04:02  lewis
 *		Cleanups - moved inlines to implementaion section at end of header
 *		file, and moved things we could to the .cc file. Got rid of method
 *		defintions from inside class declarations.
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
 *		Revision 1.12  1992/10/02  21:46:57  lewis
 *		Fixed typo in template - extra ; in defn of SetSlotsAlloced.
 *
 *		Revision 1.11  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.10  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.9  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default rep
 *		if no arg given.
 *
 *		Revision 1.8  1992/09/12  02:42:31  lewis
 *		Got rid of separate Rep.hh files - put into AbXX files inline.
 *
 *		Revision 1.7  1992/09/11  16:19:11  sterling
 *		used new Shared implementaiton
 *
 *		Revision 1.6  1992/09/04  20:44:58  sterling
 *		Added Contains ().
 *
 *
 *
 *
 */

#include	"Stack.hh"



#if		qRealTemplatesAvailable

template	<class T> class	Stack_ArrayRep;
template	<class T> class	Stack_Array : public Stack<T> {
	public:
		Stack_Array ();
		Stack_Array (const Stack<T>& bag);
		Stack_Array (const Stack_Array<T>& bag);

		nonvirtual	Stack_Array<T>& operator= (const Stack_Array<T>& stk);

		nonvirtual	size_t	GetSlotsAlloced () const;
		nonvirtual	void	SetSlotsAlloced (size_t slotsAlloced);

	private:
		nonvirtual	const Stack_ArrayRep<T>*	GetRep () const;
		nonvirtual	Stack_ArrayRep<T>*			GetRep ();
};

#endif	/*qRealTemplatesAvailable*/







/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
	// class Stack_Array<T>
	template	<class T> inline	Stack_Array<T>::Stack_Array (const Stack_Array<T>& stack) :
		Stack<T> (stack)
	{
	}
	template	<class T> inline	Stack_Array<T>& Stack_Array<T>::operator= (const Stack_Array<T>& stk)
	{
		Stack<T>::operator= (stk);
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
	#define qIncluding_Stack_ArrayCC	1
		#include	"../Sources/Stack_Array.cc"
	#undef  qIncluding_Stack_ArrayCC
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Stack_Array__*/

