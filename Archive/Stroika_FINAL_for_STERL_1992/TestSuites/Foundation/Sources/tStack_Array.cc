/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tStack_Array.cc,v 1.9 1992/12/03 07:43:28 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tStack_Array.cc,v $
 *		Revision 1.9  1992/12/03  07:43:28  lewis
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
 *		Revision 1.4  1992/09/11  13:49:57  lewis
 *		Use endl insted of newline.
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
#include	"Stack_Array.hh"

#include	"StackTests.hh"
#include	"TestSuite.hh"



#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	Stack_ArrayOfUInt32_hh
	#include	Stack_ArrayOfSimpleClass_hh
#endif	/*!qRealTemplatesAvailable*/


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Stack_Array" << endl;

	{
#if		qRealTemplatesAvailable
		Stack_Array<UInt32>	s;
#else
		Stack_Array(UInt32)	s;
#endif
		SimpleTests (s);
	}

	{
#if		qRealTemplatesAvailable
		Stack_Array<SimpleClass>	s;
#else
		Stack_Array(SimpleClass)	s;
#endif
		SimpleTests (s);
	}
	cout << "Finished testing Stack_Array" << endl;
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

