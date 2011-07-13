/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SortingTests__
#define	__SortingTests__

/*
 * $Header: /fuji/lewis/RCS/SortingTests.hh,v 1.3 1992/10/15 21:12:46 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: SortingTests.hh,v $
 *		Revision 1.3  1992/10/15  21:12:46  lewis
 *		Updated for SimpleClass and GenClass stuff.
 *
 *		Revision 1.2  1992/09/21  04:29:34  lewis
 *		Get rid of include of CollectionTests.hh, and copy declare crap
 *		from Stroika-Foundation-Globals.hh. Get rid of declare for MyIntPtr-
 *		we dont use that anymore. And do declare of Interator (UInt32) - was
 *		done in CollectionTests.
 *
 *
 */
 
 

#include	"Sorting.hh"

#include	"SimpleClass.hh"


#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	SortingOfUInt32_hh
	#include	SortingOfSimpleClass_hh
#endif

#if		qRealTemplatesAvailable
	extern	void	SimpleSortingTests (Sorting<UInt32>& s);
	extern	void	SimpleSortingTests (Sorting<SimpleClass>& s);
#else
	extern	void	SimpleSortingTests (Sorting(UInt32)& s);
	extern	void	SimpleSortingTests (Sorting(SimpleClass)& s);
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__SortingTests__*/


