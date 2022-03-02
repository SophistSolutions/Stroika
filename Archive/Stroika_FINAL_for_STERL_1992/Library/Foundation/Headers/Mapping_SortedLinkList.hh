/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Mapping_SortedLinkList__
#define	__Mapping_SortedLinkList__

/*
 * $Header: /fuji/lewis/RCS/Mapping_SortedLinkList.hh,v 1.11 1992/11/02 19:50:52 lewis Exp $
 *
 * Description:
 *		Use the LinkList template to implement a Mapping abstraction. In some ways
 *	faster than Mapping_LinkList (failed lookups take half as long) but requires
 *	that operator< be defined for keys.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *		$Log: Mapping_SortedLinkList.hh,v $
 *		Revision 1.11  1992/11/02  19:50:52  lewis
 *		*** empty log message ***
 *
 *		Revision 1.10  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.9  1992/10/23  17:18:58  lewis
 *		Moved inlines to implemenation section, and concrete reps to the .cc files.
 *		Other minor cleanups for GenClass. Got rid of CollectionRep as base
 *		class of MappingRep and moved it to each concrete rep.
 *
 *		Revision 1.7  1992/10/13  20:40:50  lewis
 *		if !qRealTemplatesAvailable then #include LinkList of MapElt .hh.
 *		Also, moved rep declaration to implementation section at end of file,
 *		and moved inlines there as well.
 *
 *
 */

#include	"Mapping.hh"



#if		qRealTemplatesAvailable

template	<class Key, class T> class	Mapping_SortedLinkList : public Mapping<Key,T> {
	public:
		Mapping_SortedLinkList ();
		Mapping_SortedLinkList (const Mapping<Key,T>& src);
		Mapping_SortedLinkList (const Mapping_SortedLinkList<Key,T>& src);

		nonvirtual	Mapping_SortedLinkList<Key,T>& operator= (const Mapping_SortedLinkList<Key,T>& src);
};

#endif	/*qRealTemplatesAvailable*/




/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
	// class Mapping_SortedLinkList
	template	<class Key, class T> inline	Mapping_SortedLinkList<Key,T>::Mapping_SortedLinkList (const Mapping_SortedLinkList<Key,T>& src) :
		Mapping<Key,T> (src)
	{
	}
	template	<class Key, class T> inline	Mapping_SortedLinkList<Key,T>& Mapping_SortedLinkList<Key,T>::operator= (const Mapping_SortedLinkList<Key,T>& src)
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
	#define	qIncluding_Mapping_SortedLinkListCC	1
		#include	"../Sources/Mapping_SortedLinkList.cc"
	#undef	qIncluding_Mapping_SortedLinkListCC
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Mapping_SortedLinkList__*/

