/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tSet.cc,v 1.3 1992/12/03 07:43:28 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tSet.cc,v $
 *		Revision 1.3  1992/12/03  07:43:28  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.2  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 * < copied from tSet_Array.cc >
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
#include	"Set.hh"

#include	"SetTests.hh"
#include	"TestSuite.hh"



#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	SetOfUInt32_hh
	#include	SetOfSimpleClass_hh
#endif


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Set" << endl;

	{
#if		qRealTemplatesAvailable
		Set<UInt32>	s;
#else
		Set(UInt32)	s;
#endif
		SimpleTests (s);
	}

	{
#if		qRealTemplatesAvailable
		Set<SimpleClass>	s;
#else
		Set(SimpleClass)	s;
#endif
		SimpleTests (s);
	}
	cout << "Finished testing Set" << endl;
	return (0);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

