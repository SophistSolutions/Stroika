/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tMath.cc,v 1.5 1992/12/03 07:45:01 lewis Exp $
 *
 * TODO:
 *
 *
 *
 * Changes:
 *	$Log: tMath.cc,v $
 *		Revision 1.5  1992/12/03  07:45:01  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.4  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.3  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
 *
 *		Revision 1.2  1992/09/11  13:42:15  lewis
 *		Test Rounding inlines, and switch to using endl instead of newline.
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
#include	"Stroika-Foundation-Globals.hh"

#include	"Debug.hh"
#include	"Math.hh"

#include	"TestSuite.hh"


static	void	TestMath ();
static	void	TestRounding ();


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Math..." << endl;

	TestMath ();
	TestRounding ();

	cout << "Done Testing Math" << endl;
	return (0);
}

static	void	TestMath ()
{
}

static	void	TestRounding ()
{
	const	unsigned short	kMaxUnsignedShort	=	(unsigned short) (-1);	// some better way to say this?
	TestCondition (RoundUpTo (1, 1) == 1);
	{ for (unsigned short i = 0; i < kMaxUnsignedShort; i++) TestCondition (RoundUpTo (i, 1) == i); }
	{ for (unsigned short i = 0; i < kMaxUnsignedShort; i++) TestCondition (RoundDownTo (i, 1) == i); }
	TestCondition (RoundUpTo (1, 100) == 100);
	TestCondition (RoundDownTo (1, 100) == 0);
	TestCondition (RoundUpTo (100, 100) == 100);
	TestCondition (RoundDownTo (100, 100) == 100);
	TestCondition (RoundUpTo (0, 100) == 0);
	TestCondition (RoundDownTo (0, 100) == 0);
	TestCondition (RoundUpTo (-1, 100) == 0);
	TestCondition (RoundDownTo (-1, 100) == -100);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
