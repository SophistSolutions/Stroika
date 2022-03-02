/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Bag_Array__
#define	__Bag_Array__

/*
 * $Header: /fuji/lewis/RCS/Bag_Array.hh,v 1.21 1992/11/13 03:30:33 lewis Exp $
 *
 * Description:
 *		Use the Array template to implement a Bag abstraction.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Bag_Array.hh,v $
 *		Revision 1.21  1992/11/13  03:30:33  lewis
 *		*** empty log message ***
 *
 *		Revision 1.19  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.18  1992/10/22  18:12:31  lewis
 *		BagRep_Array -> Bag_ArrayRep.
 *
 *		Revision 1.17  1992/10/20  17:51:23  lewis
 *		Cleanups and moved #include Array.hh to .cc file.
 *
 *		Revision 1.16  1992/10/15  02:06:15  lewis
 *		Move BagRep_Array<T> into .cc file - same with most of the inlines.
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
 *		Revision 1.10  1992/10/07  21:55:35  sterling
 *		Changed macro class name expansions to be more readable.
 *
 *		Revision 1.9  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.8  1992/09/26  22:27:48  lewis
 *		Updated for new GenClass stuff - got to compile - havent tried linking.
 *
 *		Revision 1.7  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.6  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.5  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default rep
 *		if no arg given.
 *
 *		Revision 1.4  1992/09/12  02:42:31  lewis
 *		Got rid of separate Rep.hh files - put into AbXX files inline.
 *
 *		Revision 1.3  1992/09/11  15:07:33  sterling
 *		used new Shared implementation
 *
 *		Revision 1.2  1992/09/04  19:31:45  sterling
 *		Lots of changes including use array implementation of iterator, and
 *		delegate (LGP checkedin).
 *
 *
 *
 *
 */

#include	"Bag.hh"



#if		qRealTemplatesAvailable

template	<class T>	class	Bag_ArrayRep;
template	<class T>	class	Bag_Array : public Bag<T> {
	public:
		Bag_Array ();
		Bag_Array (const Bag<T>& bag);
		Bag_Array (const Bag_Array<T>& bag);
		Bag_Array (const T* items, size_t size);

		nonvirtual	Bag_Array<T>& operator= (const Bag_Array<T>& bag);

		nonvirtual	size_t	GetSlotsAlloced () const;
		nonvirtual	void	SetSlotsAlloced (size_t slotsAlloced);

	private:
		nonvirtual	const Bag_ArrayRep<T>*	GetRep () const;
		nonvirtual	Bag_ArrayRep<T>*		GetRep ();
};

#endif	/*qRealTemplatesAvailable*/







/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#if		qRealTemplatesAvailable
	// class Bag_Array<T>
	template	<class T>	inline	Bag_Array<T>::Bag_Array (const Bag_Array<T>& bag) :
		Bag<T> (bag)
	{
	}
	template	<class T>	inline	Bag_Array<T>&	Bag_Array<T>::operator= (const Bag_Array<T>& bag)
	{
		Bag<T>::operator= (bag);
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
	#define	qIncluding_Bag_ArrayCC	1
		#include	"../Sources/Bag_Array.cc"
	#undef	qIncluding_Bag_ArrayCC
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Bag_Array__*/

