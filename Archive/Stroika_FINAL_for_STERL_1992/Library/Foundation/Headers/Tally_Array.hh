/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Tally_Array__
#define	__Tally_Array__

/*
 * $Header: /fuji/lewis/RCS/Tally_Array.hh,v 1.15 1992/11/11 03:55:13 lewis Exp $
 *
 * Description:
 *		Use the Array template to implement the Tally abstraction.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Tally_Array.hh,v $
 *		Revision 1.15  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.14  1992/11/04  19:49:55  lewis
 *		*** empty log message ***
 *
 *		Revision 1.13  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.12  1992/10/20  18:00:07  lewis
 *		Minor fixes to get to compile.
 *
 *		Revision 1.11  1992/10/19  21:42:07  lewis
 *		Move inlines from class declaration to implemenation details section at end
 *		of header, and moved concrete reps to .cc file. Added #includes where needed
 *		for GenClass stuff. Other minor cleanups - untested yet.
 *
 *		Revision 1.10  1992/10/18  07:02:50  lewis
 *		Include ArrayOfTallyEntryOf<T>_hh if using GenClass
 *
 *		Revision 1.9  1992/10/10  20:21:39  lewis
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
 *		Revision 1.6  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.5  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.4  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *
 *
 */

#include	"Tally.hh"


#if		qRealTemplatesAvailable

template	<class T>	class	Tally_ArrayRep;
template	<class T>	class	Tally_Array : public Tally<T> {
	public:
		Tally_Array ();
		Tally_Array (const Tally<T>& src);
		Tally_Array (const Tally_Array<T>& src);
		Tally_Array (const T* items, size_t size);

		nonvirtual	Tally_Array<T>& operator= (const Tally_Array<T>& src);

		nonvirtual	size_t	GetSlotsAlloced () const;
		nonvirtual	void	SetSlotsAlloced (size_t slotsAlloced);

	private:
		nonvirtual	const Tally_ArrayRep<T>*	GetRep () const;
		nonvirtual	Tally_ArrayRep<T>*			GetRep ();
};

#endif	/*qRealTemplatesAvailable*/






/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#if		qRealTemplatesAvailable
	// class Tally_Array<T>
	template	<class T>	inline	Tally_Array<T>::Tally_Array (const Tally_Array<T>& src) :
		Tally<T> (src)
	{
	}
	template	<class T>	inline	Tally_Array<T>&	Tally_Array<T>::operator= (const Tally_Array<T>& src)
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
	#define	qIncluding_Tally_ArrayCC	1
		#include	"../Sources/Tally_Array.cc"
	#undef	qIncluding_Tally_ArrayCC
#endif





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Tally_Array__*/

