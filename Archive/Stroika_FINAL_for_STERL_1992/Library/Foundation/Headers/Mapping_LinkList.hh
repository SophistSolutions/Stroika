/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Mapping_LinkList__
#define	__Mapping_LinkList__

/*
 * $Header: /fuji/lewis/RCS/Mapping_LinkList.hh,v 1.11 1992/11/11 03:55:13 lewis Exp $
 *
 * Description:
 *		Use the LinkList template to implement a Mapping abstraction.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Mapping_LinkList.hh,v $
 *		Revision 1.11  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
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
 *		Revision 1.8  1992/10/13  05:26:14  lewis
 *		Added #include LinkListOfMapElementOf<Key>_<T>_hh if
 *		!qRealTemplatesAvailable
 *
 *		Revision 1.7  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.6  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.5  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.4  1992/10/07  22:52:33  sterling
 *		Fixed a few typos in template version.
 *
 *		Revision 1.3  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.2  1992/09/26  03:50:48  lewis
 *		First cut at cleanups for new GenClass tool - untested yet.
 *
 *
 */

#include	"Mapping.hh"



#if		qRealTemplatesAvailable

template	<class Key, class T> class	Mapping_LinkList : public Mapping<Key,T> {
	public:
		Mapping_LinkList ();
		Mapping_LinkList (const Mapping<Key,T>& src);
		Mapping_LinkList (const Mapping_LinkList<Key,T>& src);

		nonvirtual	Mapping_LinkList<Key,T>& operator= (const Mapping_LinkList<Key,T>& src);
};

#endif	/*qRealTemplatesAvailable*/





/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
	template	<class Key, class T> inline	Mapping_LinkList<Key,T>::Mapping_LinkList (const Mapping_LinkList<Key,T>& src) :				
		Mapping<Key,T> (src)
	{
	}
	template	<class Key, class T> inline	Mapping_LinkList<Key,T>& Mapping_LinkList<Key,T>::operator= (const Mapping_LinkList<Key,T>& src)	
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
	#define	qIncluding_Mapping_LinkListCC	1
		#include	"../Sources/Mapping_LinkList.cc"
	#undef	qIncluding_Mapping_LinkListCC
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Mapping_LinkList__*/

