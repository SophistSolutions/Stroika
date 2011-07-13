/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Mapping_SortedArray__
#define	__Mapping_SortedArray__

/*
 * $Header: /fuji/lewis/RCS/Mapping_SortedArray.hh,v 1.10 1992/11/11 03:55:13 lewis Exp $
 *
 * Description:
 *		Use the Array template to implement a Mapping abstraction. Faster than
 *	Mapping_Array for most important operations, and should be used in
 *	preference to it except in cases where the key type does not have a
 *	operator< defined.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Mapping_SortedArray.hh,v $
 *		Revision 1.10  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.9  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.8  1992/10/23  17:18:58  lewis
 *		Moved inlines to implemenation section, and concrete reps to the .cc files.
 *		Other minor cleanups for GenClass. Got rid of CollectionRep as base
 *		class of MappingRep and moved it to each concrete rep.
 *
 *		Revision 1.7  1992/10/13  18:52:53  lewis
 *		if !qTemplatesAvailable then #include Array of MapElements.
 *
 *		Revision 1.6  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.5  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.4  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.3  1992/10/07  22:53:23  sterling
 *		Fixed a few typos in template version.
 *
 *		Revision 1.2  1992/09/26  03:50:48  lewis
 *		First cut at cleanups for new GenClass tool - untested yet.
 *
 *
 */

#include	"Mapping.hh"



#if		qRealTemplatesAvailable

template	<class Key, class T> class	Mapping_SortedArrayRep;
template	<class Key, class T> class	Mapping_SortedArray : public Mapping<Key,T> {
	public:
		Mapping_SortedArray ();
		Mapping_SortedArray (const Mapping<Key,T>& src);
		Mapping_SortedArray (const Mapping_SortedArray<Key,T>& src);

		nonvirtual	Mapping_SortedArray<Key,T>& operator= (const Mapping_SortedArray<Key,T>& src);

		nonvirtual	size_t	GetSlotsAlloced () const;
		nonvirtual	void	SetSlotsAlloced (size_t slotsAlloced);

	private:
		nonvirtual	const Mapping_SortedArrayRep<Key,T>*	GetRep () const;
		nonvirtual	Mapping_SortedArrayRep<Key,T>*			GetRep ();
};

#endif	/*qRealTemplatesAvailable*/







/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
	template	<class Key, class T> inline	Mapping_SortedArray<Key,T>::Mapping_SortedArray (const Mapping_SortedArray<Key,T>& src) :
		Mapping<Key,T> (src)
	{
	}
	template	<class Key, class T> inline	Mapping_SortedArray<Key,T>& Mapping_SortedArray<Key,T>::operator= (const Mapping_SortedArray<Key,T>& src)
	{
		Mapping<Key,T>::operator= (src);
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
	#define	qIncluding_Mapping_SortedArrayCC	1
		#include	"../Sources/Mapping_SortedArray.cc"
	#undef	qIncluding_Mapping_SortedArrayCC
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Mapping_SortedArray__*/

