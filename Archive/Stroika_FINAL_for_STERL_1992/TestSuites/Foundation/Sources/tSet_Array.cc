/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tSet_Array.cc,v 1.9 1992/12/03 07:43:28 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tSet_Array.cc,v $
 *		Revision 1.9  1992/12/03  07:43:28  lewis
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
 *		Revision 1.3  1992/09/11  13:41:43  lewis
 *		Use endl/iostream.h instead of newline/StreamUtils.hh and properly indnet macro declares.
 *		Use cout instead of cerr.
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
#include	"Set_Array.hh"

#include	"SetTests.hh"
#include	"TestSuite.hh"



#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	Set_ArrayOfUInt32_hh
	#include	Set_ArrayOfSimpleClass_hh
#endif


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Set_Array" << endl;

	{
#if		qRealTemplatesAvailable
		Set_Array<UInt32>	s;
#else
		Set_Array(UInt32)	s;
#endif
		SimpleTests (s);
	}

	{
#if		qRealTemplatesAvailable
		Set_Array<SimpleClass>	s;
#else
		Set_Array(SimpleClass)	s;
#endif
		SimpleTests (s);
	}
	cout << "Finished testing Set_Array" << endl;
	return (0);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

