/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tException.cc,v 1.7 1992/12/05 17:40:28 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *		$Log: tException.cc,v $
 *		Revision 1.7  1992/12/05  17:40:28  lewis
 *		Renamed Try->try, and Catch->catch - see exception.hh for explation.
 *
 *		Revision 1.6  1992/12/03  07:46:54  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.5  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.4  1992/09/11  13:23:34  lewis
 *		Use endl instead of newline, use <iostream.h> instead of StreamUtils.hh,
 *		and properly indent #ifs as per new style.
 *		Use cout instead of cerr.
 *
 *		Revision 1.3  1992/09/05  06:16:07  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/03/11  23:11:46  lewis
 *		Use cout/cerr instead of gDebugStream since thats not available when debug turned off anyhow, and using
 *		two different streams allows us to differentiate info from actual errors.
 *
 *		Revision 1.5  1992/01/22  04:10:56  lewis
 *		Always call OSInit.
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
#include	"Exception.hh"
#include	"Memory.hh"

#include	"TestSuite.hh"


static	void*	FAKE_ALLOC_POINTER (size_t nBytes);
static	void	TestWithNoThrows ();
static	void	TestWithThrows ();



int	main (int /*argc*/, char* /*argv*/[])
{
	TestWithNoThrows ();
	cout << "TestWithNoThrows did fine." << endl;

	TestWithThrows ();
	cout << "TestWithThrows did fine." << endl;
	return (0);
}

static	void*	FAKE_ALLOC_POINTER (size_t nBytes)
{
	if (nBytes >= 100) {
		gMemoryException.Raise ();
	}
	else {
		return ((void*)10);
	}
}

static	void	TestWithNoThrows ()
{
	void*	p	=	Nil;

	{
		try {
			p	=	FAKE_ALLOC_POINTER (3);
		}
		catch () {
			_this_catch_->Raise ();
		}
	}
	TestCondition (p != Nil);

	p = Nil;
	{
		try {
			p	=	FAKE_ALLOC_POINTER (5);
		}
		catch1 (gMemoryException) {
			cout << "Test_Exceptions:	caught gMemoryException on FAKE_ALLOC_POINTER (5)" << endl;
		}
		catch () {
			TestCondition (False);
			_this_catch_->Raise ();
		}
	}
	TestCondition (p != Nil);
}

static	void	TestWithThrows ()
{
	void*	p	=	Nil;

	/*
	 * Try some catches.
	 *		NB:		must scope catcher blocks since try macro uses same name for
	 *				temporary variables.
	 */
	{
		try {
			p	=	FAKE_ALLOC_POINTER (3);
		}
		catch () {
			_this_catch_->Raise ();
		}
	}
	cout << "Test_Exceptions:	FAKE_ALLOC_POINTER (3) succeeded" << endl;
	TestCondition (p != Nil);

	p = Nil;
	{
		try {
			p	=	FAKE_ALLOC_POINTER (999);
		}
		catch1 (gMemoryException) {
			cout << "Test_Exceptions:	caught gMemoryException on FAKE_ALLOC_POINTER (999)" << endl;
		}
		catch () {
			TestCondition (False);
			_this_catch_->Raise ();
		}
	}
	cout << "Test_Exceptions:	Alloc (999) succeeded?" << endl;
	TestCondition (p == Nil);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

