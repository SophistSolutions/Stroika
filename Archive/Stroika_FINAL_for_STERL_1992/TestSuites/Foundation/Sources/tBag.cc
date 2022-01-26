/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tBag.cc,v 1.8 1992/12/03 07:46:01 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tBag.cc,v $
 *		Revision 1.8  1992/12/03  07:46:01  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.7  1992/10/22  18:18:51  lewis
 *		Added tests for SimpleClass::GetTotalLiveCount () to be sure
 *		all items destroyed by the end.
 *		(DO SOON TO OTHER TESTS - only tried here!!!).
 *
 *		Revision 1.6  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.5  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.3  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
 *
 *		Revision 1.2  1992/09/11  13:22:59  lewis
 *		Use endl instead of newline, use <iostream.h> instead of StreamUtils.hh,
 *		and properly indent #ifs as per new style.
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

#include	"Bag.hh"
#include	"Debug.hh"

#include	"TestSuite.hh"
#include	"BagTests.hh"



int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Bag" << endl;

	TestCondition (SimpleClass::GetTotalLiveCount () == 0);
	{
#if		qRealTemplatesAvailable
		Bag<UInt32>	s;
#else
		Bag(UInt32)	s;
#endif
		BagTests (s);
	}

	{
#if		qRealTemplatesAvailable
		Bag<SimpleClass>	s;
#else
		Bag(SimpleClass)	s;
#endif
		BagTests (s);

		s.Add (SimpleClass (1));
		TestCondition (SimpleClass::GetTotalLiveCount () != 0);
	}
	TestCondition (SimpleClass::GetTotalLiveCount () == 0);

	cout << "Finished testing Bag" << endl;
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

