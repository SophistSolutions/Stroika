/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tSorting_Array.cc,v 1.4 1992/12/03 07:43:28 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tSorting_Array.cc,v $
 *		Revision 1.4  1992/12/03  07:43:28  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.3  1992/10/16  22:57:00  lewis
 *		Use SimpleClass instead of String, and use new genclass template stuff.
 *
 *		Revision 1.2  1992/09/21  04:33:22  lewis
 *		Got rid of declares - now done in SortingTests.cc. Also, got rid
 *		of MyIntPtr.
 *
 *
 */

/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */
#include	"OSInit.hh"
#pragma	push
	#pragma	force_active	on
	#if		qGCC_SimpleClassStaticInitializerBug
		static	OSInit	sOSInit;
	#else
		static	OSInit	sOSInit	=	OSInit ();
	#endif
#pragma	pop

#define		qUseAllGlobals		0
#define		qUseDebugGlobals	1
#include	"Stroika-Foundation-Globals.hh"

#include	<iostream.h>

#include	"Debug.hh"
#include	"Sorting_Array.hh"

#include	"SortingTests.hh"
#include	"TestSuite.hh"


#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	Sorting_ArrayOfUInt32_hh
	#include	Sorting_ArrayOfSimpleClass_hh
#endif	/*!qRealTemplatesAvailable*/


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Sorting_Array" << endl;

	{
#if		qRealTemplatesAvailable
		Sorting_Array<UInt32>	s;
#else
		Sorting_Array(UInt32)	s;
#endif
		SimpleSortingTests (s);
	}

	{
#if		qRealTemplatesAvailable
		Sorting_Array<SimpleClass>	s;
#else
		Sorting_Array(SimpleClass)	s;
#endif
		SimpleSortingTests (s);
	}
	cout << "Finished testing Sorting_Array" << endl;
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

