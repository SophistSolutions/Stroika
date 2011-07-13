/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__HashTable__
#define	__HashTable__

/*
 * $Header: /fuji/lewis/RCS/HashTable.hh,v 1.11 1992/11/11 03:55:13 lewis Exp $
 *
 * Description:
 *
 *		This is a simple parameterized version of a seperate chaining
 *		hash table. You supply the entity that is to be stored in the table
 *		(thats key+data), and a hash function, and a compareForEquality
 *		function, and we'll do the rest!
 *
 * TODO:
 *		It might make sense to support rehashing in the future - there would
 *		be no cost since its all non-virtual functions (no linker worries).
 *
 *		HashTableElement(T) Should be scoped inside  HashTable(T), but for
 *		various compiler bugs.
 *
 *		Do overloaded new delete for HashTableElement(T), so we make build
 *		lots of these quickly using usual fixed size pool technique.
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: HashTable.hh,v $
 *		Revision 1.11  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.10  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.9  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.8  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.7  1992/09/11  12:59:08  lewis
 *		Lot of work debugging under gcc, and trouble turned out to be
 *		gcc 2.2.1 bug in the testsuite! Also, use size_t instead of UInt32.
 *
 *		Revision 1.6  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.4  1992/07/17  18:23:51  lewis
 *		Replaced old qTemplatesHasRepository with a new, much more elaborate,
 *		and hopefully more correct version.
 *
 *		Revision 1.3  1992/07/16  07:48:19  lewis
 *		Change magic include at end to be depend on "qRealTemplatesAvailable && !!qTemplatesHasRepository"
 *		instead of qBorlandCPlus.
 *
 *		Revision 1.2  1992/07/08  05:27:26  lewis
 *		Changed file name to include for template .cc stuff to UNIX pathname,
 *		and use PCName mapper to rename to appropriate PC File - cuz we may
 *		need to do this with gcc/aux too.
 *		Also, so I have PC Names (mapped to names) in just one place - so
 *		they are easier to change.
 *
 *		Revision 1.12  1992/05/09  01:28:31  lewis
 *		Ported to BC++/PC/Templates.
 *
 *		Revision 1.11  92/04/07  13:18:19  13:18:19  lewis (Lewis Pringle)
 *		Got rid of Add() method - not really useful.
 *		
 *		Revision 1.10  92/04/07  09:50:13  09:50:13  lewis (Lewis Pringle)
 *		Add Add() method. Sometimes convientent to have hashtable of elements
 *		which are ??? Maybe rethink - maybe bad idea???
 *		Made Hash() function public.
 *		Did template version and created HashTable.cc
 *		Add SetIthTableElement.
 *		delete removed elt in Remove () - was memory leak.
 *		
 *		Revision 1.8  1992/02/21  17:30:49  lewis
 *		No longer support qGPlus_FunctParamBug.
 *
 *		Revision 1.7  1992/01/16  00:31:13  lewis
 *		Out of lines a few ctors/dtors in hopes of working around mac compiler bugs with PCXlate - effort failed.
 *
 */


#include	"Config-Foundation.hh"




#if		qRealTemplatesAvailable


template	<class	T>	class	HashTableElement {
	public:
		HashTableElement (T element, HashTableElement<T>* next);

		T						fElement;
		HashTableElement<T>*	fNext;
};





template	<class	T>	class	HashTable {
	public:
		HashTable (size_t (*hashFunction) (const T&), size_t hashTableSize = 101);
		~HashTable ();

		nonvirtual	Boolean	Lookup (T& elt) const;
		nonvirtual	Boolean	Enter (T elt);	/* return True iff new record added */
		nonvirtual	void	Remove (T elt);
		nonvirtual	void	Clear ();

		nonvirtual	size_t 	(*GetHashFunction () const) (const T&)
			{
				return (fHashFunction);
			}

		nonvirtual	size_t 	GetTableSize () const
			{
				return (fTableSize);
			}

		nonvirtual	HashTableElement<T>* 	GetIthTableHead (size_t i) const	/* 1 based */
			{
				RequireNotNil (fTable);
				Require (i >= 1);
				Require (i <= fTableSize);
				return (fTable[i-1]);
			}

		nonvirtual	void	SetIthTableHead (size_t i, HashTableElement<T>* newElt) const	/* 1 based */
			{
				RequireNotNil (fTable);
				Require (i >= 1);
				Require (i <= fTableSize);
				fTable[i-1] = newElt;
			}

		/* return 0 .. fTableSize-1 */
		nonvirtual	size_t	Hash (const T& hElt) const;

	private:
		size_t 					(*fHashFunction) (const T&);
		size_t					fTableSize;
		HashTableElement<T>**	fTable;
};





#endif	/*qRealTemplatesAvailable*/








/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */


#if		qRealTemplatesAvailable
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
	#define	qIncluding_HashTableCC	1
		#include	"../Sources/HashTable.cc"
	#undef	qIncluding_HashTableCC
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__HashTable__*/

