/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tDeque.cc,v 1.10 1992/12/03 07:44:40 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tDeque.cc,v $
 *		Revision 1.10  1992/12/03  07:44:40  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.8  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.7  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.6  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.4  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
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
#include	"Deque.hh"
#include	"StreamUtils.hh"

#include	"DequeTests.hh"
#include	"TestSuite.hh"




int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Deque" << endl;

	{
#if		qRealTemplatesAvailable
		Deque<UInt32>	s;
#else
		Deque(UInt32)	s;
#endif
		SimpleDequeTests (s);
	}

	{
#if		qRealTemplatesAvailable
		Deque<SimpleClass>	s;
#else
		Deque(SimpleClass)	s;
#endif
		SimpleDequeTests (s);
	}
	cout << "Finished testing Deque" << endl;
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

