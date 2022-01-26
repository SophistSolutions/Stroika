/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Bag_LinkList__
#define	__Bag_LinkList__

/*
 * $Header: /fuji/lewis/RCS/Bag_LinkList.hh,v 1.18 1992/11/29 02:42:48 lewis Exp $
 *
 * Description:
 *		Use the LinkList template to implement a Bag abstraction.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Bag_LinkList.hh,v $
 *		Revision 1.18  1992/11/29  02:42:48  lewis
 *		Add workarounds for qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.16  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.15  1992/10/15  02:07:02  lewis
 *		Move  BagRep_LinkList<T> into .cc file, and stuff that goes
 *		with it (like a bunch of inlines that depend on it).
 *
 *		Revision 1.14  1992/10/14  02:26:02  lewis
 *		Cleanup format - move all inlines to the bottom of the file in
 *		implementation section. Move declarations of concrete iterators
 *		(except bag.hh) to the .cc file.
 *
 *		Revision 1.13  1992/10/10  20:16:36  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.12  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.11  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.10  1992/09/26  22:27:48  lewis
 *		Updated for new GenClass stuff - got to compile - havent tried linking.
 *
 *		Revision 1.9  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.8  1992/09/21  05:18:22  sterling
 *		Call BagRep(T) base class CTOR explicitly in BagRep_Array CTOR.
 *
 *		Revision 1.7  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.6  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default rep
 *		if no arg given.
 *
 *		Revision 1.5  1992/09/12  02:42:31  lewis
 *		Got rid of separate Rep.hh files - put into AbXX files inline.
 *
 *		Revision 1.4  1992/09/11  15:07:33  sterling
 *		used new Shared implementation
 *
 *		Revision 1.3  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.2  1992/09/04  21:42:40  sterling
 *		Added Contains () method override.
 *
 *
 */

#include	"Bag.hh"


#if		qRealTemplatesAvailable

template	<class T> class	Bag_LinkList : public Bag<T> {
	public:
		Bag_LinkList ();
		Bag_LinkList (const T* items, size_t size);
		Bag_LinkList (const Bag<T>& bag);
		Bag_LinkList (const Bag_LinkList<T>& bag);

		nonvirtual	Bag_LinkList<T>&	operator= (const Bag_LinkList<T>& bag);
};

#endif	/*qRealTemplatesAvailable*/







/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#if		qRealTemplatesAvailable

	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifdef	inline
			#error "Ooops - I guess my workaround wont work!!!"
		#endif
		#define	inline
	#endif

	// class Bag_LinkList<T>
	template	<class T>	inline	Bag_LinkList<T>& Bag_LinkList<T>::operator= (const Bag_LinkList<T>& bag)
	{
		Bag<T>::operator= (bag);
		return (*this);
	}

	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifndef	inline
			#error	"How did it get undefined?"
		#endif
		#undef	inline
	#endif
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
	#define	qIncluding_Bag_LinkListCC	1
		#include	"../Sources/Bag_LinkList.cc"
	#undef	qIncluding_Bag_LinkListCC
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Bag_LinkList__*/

