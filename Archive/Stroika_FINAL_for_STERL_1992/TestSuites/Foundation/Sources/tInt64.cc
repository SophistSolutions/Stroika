/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tInt64.cc,v 1.6 1992/12/03 07:45:01 lewis Exp $
 *
 * TODO:
 *
 *
 *
 * Changes:
 *	$Log: tInt64.cc,v $
 *		Revision 1.6  1992/12/03  07:45:01  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.5  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.4  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
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
#include	"Math.hh"

#include	"TestSuite.hh"


static	void	TestUInt64 ();


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing UInt64..." << endl;

	TestUInt64 ();

	cout << "Done Testing UInt64" << endl;
	return (0);
}

static	void	TestUInt64 ()
{
	UInt64	fCount = 0;
	UInt32	lCount = 0;
	for (int i = 1; i <= 1000; i++) {
		fCount += i;
		lCount += i;
	}
	TestCondition (lCount == fCount);
	fCount = 1;
	TestCondition (fCount == 1);
	fCount *= 5;
	TestCondition (fCount == 5);
	fCount = fCount * 10;
	TestCondition (fCount == 50);
	fCount /= 10;
	TestCondition (fCount == 5);
	fCount = 5;
	fCount = fCount / 5;
	TestCondition (fCount == 1);

	fCount = 65536;
	fCount /= 2;
	TestCondition (fCount == 32768);

	fCount = 100000;
	fCount /= 10;
	TestCondition (fCount == 10000);
	
	lCount = 1;
	fCount = 1;
	for (i = 1; i <= 12; i++) {
		fCount *= i;
		lCount *= i;
		TestCondition (lCount == fCount);
	}

	for (i = 12; i >= 1; i--) {
		fCount /= i;
		lCount /= i;
		TestCondition (lCount == fCount);
	}
	
	TestCondition (((UInt64 (10000) * UInt64 (10000)) / 10000) == 10000);
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
