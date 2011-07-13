/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Stack_LinkList__
#define	__Stack_LinkList__

/*
 * $Header: /fuji/lewis/RCS/Stack_LinkList.hh,v 1.14 1992/11/11 03:55:13 lewis Exp $
 *
 * Description:
 *		Use the LinkList template to implement a Stack abstraction.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Stack_LinkList.hh,v $
 *		Revision 1.14  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.13  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.12  1992/10/15  02:22:45  lewis
 *		Moved StackRep_LinkList<T> into .cc file along with a few related
 *		inline methods. Also got rid of GetRep shaddows in Stack_LinkList
 *		as they were ununsed.
 *
 *		Revision 1.11  1992/10/14  06:04:02  lewis
 *		Cleanups - moved inlines to implementaion section at end of header
 *		file, and moved things we could to the .cc file. Got rid of method
 *		defintions from inside class declarations.
 *
 *		Revision 1.10  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.9  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.8  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.7  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.6  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.5  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default src
 *		if no arg given.
 *
 *		Revision 1.4  1992/09/11  16:19:11  sterling
 *		used new Shared implementaiton
 *
 *		Revision 1.3  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.2  1992/09/04  20:50:05  sterling
 *		Override Contains () method.
 *
 *
 *
 *
 */

#include	"Stack.hh"



#if		qRealTemplatesAvailable

template	<class T> class	Stack_LinkList : public Stack<T> {
	public:
		Stack_LinkList ();
		Stack_LinkList (const Stack<T>& src);
		Stack_LinkList (const Stack_LinkList<T>& src);

		nonvirtual	Stack_LinkList<T>& operator= (const Stack_LinkList<T>& src);
};

#endif	/*qRealTemplatesAvailable*/





/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
	// class Stack_LinkList<T>
	template	<class T>	inline	Stack_LinkList<T>::Stack_LinkList (const Stack_LinkList<T>& src) :
		Stack<T> (src)
	{
	}
	template	<class T>	inline	Stack_LinkList<T>&	Stack_LinkList<T>::operator= (const Stack_LinkList<T>& src)
	{
		Stack<T>::operator= (src);
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
	#define qIncluding_Stack_LinkListCC	1
		#include	"../Sources/Stack_LinkList.cc"
	#undef  qIncluding_Stack_LinkListCC
#endif






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Stack_LinkList__*/
