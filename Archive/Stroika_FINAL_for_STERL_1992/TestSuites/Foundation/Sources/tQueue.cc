/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tQueue.cc,v 1.10 1992/12/08 17:20:22 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tQueue.cc,v $
 *		Revision 1.10  1992/12/08  17:20:22  lewis
 *		*** empty log message ***
 *
 *		Revision 1.9  1992/12/03  07:45:01  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.8  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.7  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.5  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
 *
 *		Revision 1.2  1992/09/04  21:23:05  sterling
 *		Switched to circular array.
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
#include	"Queue.hh"
#include	"StreamUtils.hh"

#include	"QueueTests.hh"
#include	"TestSuite.hh"



int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Queue" << endl;

	{
#if		qRealTemplatesAvailable
		Queue<UInt32>	s;
#else
		Queue(UInt32)	s;
#endif
		SimpleQueueTests (s);
	}

	{
#if		qRealTemplatesAvailable
		Queue<SimpleClass>	s;
#else
		Queue(SimpleClass)	s;
#endif
		SimpleQueueTests (s);
	}

	cout << "Finished testing Queue" << endl;
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

