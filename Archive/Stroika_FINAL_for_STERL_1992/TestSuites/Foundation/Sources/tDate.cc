/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tDate.cc,v 1.5 1992/12/03 07:47:50 lewis Exp $
 *
 * TODO:
 *
 *
 *
 * Changes:
 *	$Log: tDate.cc,v $
 *		Revision 1.5  1992/12/03  07:47:50  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *		Got rid of #define qUseStringGlobals.
 *
 *		Revision 1.4  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.3  1992/09/11  13:36:59  lewis
 *		Use endl instead of newline. and iostream.h instead of StreamUtils.hh.
 *
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

#include	<iostream.h>

#include	"Debug.hh"
#include	"Date.hh"

#include	"TestSuite.hh"



int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Date..." << endl;

	cout << "Done Testing Date" << endl;
	return (0);
}






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
