/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tTally_Array.cc,v 1.5 1992/12/03 06:43:11 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tTally_Array.cc,v $
 *		Revision 1.5  1992/12/03  06:43:11  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.4  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.3  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
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
#include	"Tally.hh"
#include	"Tally_Array.hh"
#include	"StreamUtils.hh"

#include	"TallyTests.hh"
#include	"TestSuite.hh"

#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	Tally_ArrayOfUInt32_hh
	#include	Tally_ArrayOfSimpleClass_hh
#endif	/*!qRealTemplatesAvailable*/

int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Tally_Array" << endl;

	{
#if		qRealTemplatesAvailable
		Tally_Array<UInt32>	s;
#else
		Tally_Array(UInt32)	s;
#endif
		SimpleTallyTests (s);
	}

	{
#if		qRealTemplatesAvailable
		Tally_Array<SimpleClass>	s;
#else
		Tally_Array(SimpleClass)	s;
#endif
		SimpleTallyTests (s);
	}
	cout << "Finished testing Tally_Array" << endl;
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

