/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tMapping.cc,v 1.3 1992/12/03 07:45:01 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tMapping.cc,v $
 *		Revision 1.3  1992/12/03  07:45:01  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.2  1992/09/29  15:54:35  sterling
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
#define		qUseStringGlobals	1
#include	"Stroika-Foundation-Globals.hh"

#include	"Debug.hh"
#include	"Mapping.hh"
#include	"StreamUtils.hh"

#include	"MappingTests.hh"
#include	"TestSuite.hh"


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Mapping" << endl;

	{
#if		qRealTemplatesAvailable
		Mapping<SimpleClass, UInt32>	s;
#else
		Mapping(SimpleClass, UInt32)	s;
#endif
		SimpleMappingTests (s);
	}

	cout << "Finished testing Mapping" << endl;
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

