/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tQueue_DoubleLinkList.cc,v 1.8 1992/12/03 07:45:01 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tQueue_DoubleLinkList.cc,v $
 *		Revision 1.8  1992/12/03  07:45:01  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.7  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
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
#include	"Queue_DoubleLinkList.hh"

#include	"QueueTests.hh"
#include	"TestSuite.hh"



#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	Queue_DoubleLinkListOfUInt32_hh
	#include	Queue_DoubleLinkListOfSimpleClass_hh
#endif	/*!qRealTemplatesAvailable*/



int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing DoubleLinkList Queue" << endl;

	{
#if		qRealTemplatesAvailable
		Queue_DoubleLinkList<UInt32>	s;
#else
		Queue_DoubleLinkList(UInt32)	s;
#endif
		SimpleQueueTests (s);
	}

	{
#if		qRealTemplatesAvailable
		Queue_DoubleLinkList<SimpleClass>	s;
#else
		Queue_DoubleLinkList(SimpleClass)	s;
#endif
		SimpleQueueTests (s);
	}
	cout << "Finished testing DoubleLinkList Queue" << endl;
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

