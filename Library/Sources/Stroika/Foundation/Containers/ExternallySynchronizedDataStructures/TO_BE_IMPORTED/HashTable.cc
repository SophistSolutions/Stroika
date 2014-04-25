/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__HashTable__cc
#define	__HashTable__cc

/*
 * $Header: /fuji/lewis/RCS/HashTable.cc,v 1.8 1992/11/12 08:23:55 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: HashTable.cc,v $
 *		Revision 1.8  1992/11/12  08:23:55  lewis
 *		*** empty log message ***
 *
 *		Revision 1.7  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.6  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.5  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.4  1992/09/11  13:12:20  lewis
 *		Use size_t rathern than UInt32.
 *
 *		Revision 1.3  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.3  1992/05/09  02:37:07  lewis
 *		Ported to BC++ and fixed template bugs.
 *
 *		Revision 1.2  92/04/07  13:11:38  13:11:38  lewis (Lewis Pringle)
 *		Got rid of method Add().
 *
 */



#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Memory.hh"

#include	"HashTable.hh"





/*
 ********************************************************************************
 ******************************** HashTableElement ******************************
 ********************************************************************************
 */


#if		qRealTemplatesAvailable

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	HashTableElement;	// tmp hack so GenClass will fixup below array CTOR
													// harmless, but silly

#endif

template	<class	T>	HashTableElement<T>::HashTableElement (T element, HashTableElement<T>* next):
	fElement (element),
	fNext (next)
{
}

#endif	/*qRealTemplatesAvailable*/




/*
 ********************************************************************************
 ********************************** HashTable ***********************************
 ********************************************************************************
 */

#if		qRealTemplatesAvailable

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	HashTable;	// tmp hack so GenClass will fixup below array CTOR
											// harmless, but silly

#endif

template	<class	T>	HashTable<T>::HashTable (size_t (*hashFunction) (const T&), size_t hashTableSize):
	fHashFunction (hashFunction),
	fTableSize (hashTableSize),
	fTable (Nil)
{
	Require (hashTableSize != 0);
	RequireNotNil (hashFunction);
	fTable = new HashTableElement<T>*	[fTableSize];
	for (size_t i = 0; i < fTableSize; i++) {
		fTable[i] = Nil;
	}
}

template	<class	T>	HashTable<T>::~HashTable ()
{
	if (fTable != Nil) {
		Clear ();
		delete (fTable);
	}
}

template	<class	T>	Boolean	HashTable<T>::Lookup (T& elt) const
{
	RequireNotNil (fTable);
	for (register HashTableElement<T>* curT = fTable [Hash (elt)]; curT != Nil; curT = curT->fNext) {
		if (curT->fElement == elt) {
			elt = curT->fElement;
			return (True);
		}
	}
	return (False);
}

template	<class	T>	Boolean	HashTable<T>::Enter (T elt)	/* return True iff new record added */
{
	RequireNotNil (fTable);
	size_t	hashVal = Hash (elt);
	for (register HashTableElement<T>* curT = fTable [hashVal]; curT != Nil; curT = curT->fNext) {
		if (curT->fElement == elt) {
			curT->fElement = elt;
			return (False);
		}
	}
	fTable [hashVal] = new HashTableElement<T> (elt, fTable [hashVal]);
	return (True);
}

template	<class	T>	void	HashTable<T>::Remove (T elt)
{
	RequireNotNil (fTable);
	size_t	hashVal = Hash (elt);
	HashTableElement<T>* prev = Nil;
	for (register HashTableElement<T>* curT = fTable [hashVal]; curT != Nil; prev = curT, curT = curT->fNext) {
		if (curT->fElement == elt) {
			/* if were at the head of the hash table chain */
			if (prev == Nil) {
				fTable[hashVal] = curT->fNext;
			}
			else {
				AssertNotNil (prev);
				prev->fNext = curT->fNext;
			}
			delete (curT);
			return;
		}
	}
	AssertNotReached ();	/* its a client bug to call remove on an item which isn't there. */
}

template	<class	T>	void	HashTable<T>::Clear ()
{
	RequireNotNil (fTable);
	for (register size_t i = 0; i < fTableSize; i++) {
		for (HashTableElement<T>* elt = fTable [i]; elt != Nil; ) {
			HashTableElement<T>*	delElt	=	elt;
			elt = elt->fNext;
			delete (delElt);
		}
		fTable [i] = Nil;
	}
}

template	<class	T>	size_t	HashTable<T>::Hash (const T& hElt) const
{
	RequireNotNil (fHashFunction);
	Assert (fTableSize != 0);
	size_t	result		=	(*fHashFunction) (hElt)	% fTableSize;
	Ensure (result >= 0);
	Ensure (result < fTableSize);
	return (result);
}


#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

