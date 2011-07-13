/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tFormat.cc,v 1.5 1992/12/03 07:46:54 lewis Exp $
 *
 * TODO:
 *
 *
 *
 * Changes:
 *	$Log: tFormat.cc,v $
 *		Revision 1.5  1992/12/03  07:46:54  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.4  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.3  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.2  1992/09/11  13:40:05  lewis
 *		Use endl and iostream.h instead of newline and StreamUtils.hh
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
#include	"Format.hh"

#include	"TestSuite.hh"


static	void	TestFormatStuff ();


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Format..." << endl;

	TestFormatStuff ();

	cout << "Done Testing Format" << endl;
	return (0);
}

static	void	TestFormatStuff ()
{
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
