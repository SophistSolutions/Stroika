/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tSorting.cc,v 1.4 1992/12/03 07:43:28 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tSorting.cc,v $
 *		Revision 1.4  1992/12/03  07:43:28  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.3  1992/10/16  22:56:24  lewis
 *		Use SimpleClass instead of String, and use new genclass template stuff.
 *
 *		Revision 1.2  1992/09/21  04:32:39  lewis
 *		Got rid of declares - now done in SortingTests.cc. Also, got rid
 *		of MyIntPtr testsd.
 *
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

#include	"Debug.hh"
#include	"Sorting.hh"

#include	"TestSuite.hh"
#include	"SortingTests.hh"


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Sorting" << endl;

	{
#if		qRealTemplatesAvailable
		Sorting<UInt32>	s;
#else
		Sorting(UInt32)	s;
#endif
		SimpleSortingTests (s);
	}

	{
#if		qRealTemplatesAvailable
		Sorting<SimpleClass>	s;
#else
		Sorting(SimpleClass)	s;
#endif
		SimpleSortingTests (s);
	}
	cout << "Finished testing Sorting" << endl;
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

