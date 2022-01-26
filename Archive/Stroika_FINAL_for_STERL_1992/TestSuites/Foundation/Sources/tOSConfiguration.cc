/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tOSConfiguration.cc,v 1.6 1992/12/03 07:45:01 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tOSConfiguration.cc,v $
 *		Revision 1.6  1992/12/03  07:45:01  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.5  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.4  1992/09/11  13:43:52  lewis
 *		Got rid of GDI stuff.
 *
 *		Revision 1.3  1992/09/04  02:02:35  lewis
 *		Converted to REALLY be just for OSConfiguration - now seperate testsuite for
 *		gdi layer stuff.
 *
 *		Revision 1.5  1992/01/22  04:11:30  lewis
 *		Always call OSInit, and cleanups.
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

#include	<iostream.h>

#include	"Debug.hh"


#include	"OSConfiguration.hh"

#include	"TestSuite.hh"




int	main (int /*argc*/, char* /*argv*/[])
{
	OSConfiguration	config;

	/*
	 * Some consistency checks.
	 */
// NYI	TestCondition (config.TotalStackSpace () > config.StackAvailable ());
#if		qMacOS
	TestCondition (OSConfiguration ().Has68020Plus () or not (OSConfiguration ().HasFPU ()));
#endif



	/*
	 * Some tests for my current machine so I know things are working.
	 */
// NYI	DebugMessage ("config.TotalStackSpace () = %lx", config.TotalStackSpace ());
	DebugMessage ("config.StackAvailable () = %lx", config.StackAvailable ());
// NYI	DebugMessage ("config.TotalHeapSpace () = %lx", config.TotalHeapSpace ());
// NYI	DebugMessage ("config.HeapAvailable () = %lx", config.HeapAvailable ());

	return (0);
};




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

