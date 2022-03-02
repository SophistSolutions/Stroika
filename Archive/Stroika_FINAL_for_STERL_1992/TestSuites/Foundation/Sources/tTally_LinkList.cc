/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tTally_LinkList.cc,v 1.5 1992/12/03 06:42:55 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tTally_LinkList.cc,v $
 *		Revision 1.5  1992/12/03  06:42:55  lewis
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
#include	"Tally_LinkList.hh"

#include	"TallyTests.hh"
#include	"TestSuite.hh"

#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	Tally_LinkListOfUInt32_hh
	#include	Tally_LinkListOfSimpleClass_hh
#endif	/*!qRealTemplatesAvailable*/

int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Tally_LinkList" << endl;

	{
#if		qRealTemplatesAvailable
		Tally_LinkList<UInt32>	s;
#else
		Tally_LinkList(UInt32)	s;
#endif
		SimpleTallyTests (s);
	}

	{
#if		qRealTemplatesAvailable
		Tally_LinkList<SimpleClass>	s;
#else
		Tally_LinkList(SimpleClass)	s;
#endif
		SimpleTallyTests (s);
	}
	cout << "Finished testing Tally_LinkList" << endl;
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

