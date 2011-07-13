/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tMemory.cc,v 1.5 1992/12/03 07:45:01 lewis Exp $
 *
 * TODO:
 *
 *		-		Do tests of the alignment macros.
 *
 *		-		Do tests of exceptions being raised not causing memory leaks and Realloc
 *				/malloc/free all working correctly in the presence of exceptions.
 *
 *
 * Changes:
 *	$Log: tMemory.cc,v $
 *		Revision 1.5  1992/12/03  07:45:01  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.4  1992/09/11  13:37:42  lewis
 *		Use endl instead of newline, got rid of use of string globals.
 *
 *		Revision 1.3  1992/09/05  06:16:07  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.9  92/03/11  23:11:46  23:11:46  lewis (Lewis Pringle)
 *		Use cout/cerr instead of gDebugStream since thats not available when debug turned off anyhow, and using
 *		two different streams allows us to differentiate info from actual errors.
 *		
 *		Revision 1.7  1992/01/28  10:31:44  lewis
 *		Added test to assure that our memory overwrite code was working, and that allocs, and putting stuff into
 *		the buffer did not overwrite gaurds.
 *
 *		Revision 1.6  1992/01/22  04:11:16  lewis
 *		Always call OSInit, and cleanups.
 *
 *		Revision 1.5  1992/01/09  07:18:04  lewis
 *		Added checks for realloc(0), and new char [0], and assured they return unique pointers.
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
#include	"Memory.hh"

#include	"TestSuite.hh"


static	void	TestCharArrays ();
static	void	TestRealloc ();
static	void	TestAllocOfZero ();
static	void	TestAllocsEnuf ();


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing memory..." << endl;

	TestCharArrays ();
	TestRealloc ();
	TestAllocOfZero ();
	TestAllocsEnuf ();

	cout << "Done Testing memory" << endl;
	return (0);
}

static	void	TestCharArrays ()
{
	/*
	 * Test that rudimentary memory allocations and access work.
	 */
	char*	p = new char [101];
	for (int i = 0; i < 101; i++) {
		p[i] = i;
	}
	for (i = 0; i < 101; i++) {
		TestCondition (p[i] == i);
	}
	delete (p);
}

static	void	TestRealloc ()
{
	/*
	 * Test that ReAlloc works - that is it copies things properly.
	 */
	char*	p = new char [101];
	for (int i = 0; i < 101; i++) {
		p[i] = i;
	}
	p = (char*)ReAlloc (p, 202);
	for (i = 0; i < 101; i++) {
		TestCondition (p[i] == i);
	}
	delete (p);
}

static	void	TestAllocOfZero ()
{
	/*
	 * Test that Alloc (0) returns good value, and alloc always returns differnt values.
	 * Then test that all still works well with realloc.
	 */
	{
		void*	aX			=	new char [0];
		void*	anotherX	=	new char [0];

		TestCondition (aX != Nil);
		TestCondition (anotherX != Nil);
		TestCondition (aX != anotherX);
	}

	{
		char*	aChar		=	new char [1];
		char*	anotherChar	=	new char [1];

		TestCondition (aChar != Nil);
		TestCondition (anotherChar != Nil);
		TestCondition (aChar != anotherChar);
	}

	{
		void*	aX			=	new char [0];
		void*	anotherX	=	ReAlloc (aX, 100);
		TestCondition (anotherX != Nil);
		// aX value is bad now - don't access - but anotherX should be good
		memcpy (anotherX, "junk", 5);
	}
}


static	void	TestAllocsEnuf ()
{
	/*
	 * Be sure (esp with debug turned on) that when we allocate memory, we can scribble on all of it, up to the
	 * end, and not get an overwrite. This also tests our overwrite detection code.
	 */
	for (size_t bufSize = 1; bufSize <= 1024; bufSize++) {
		UInt8*	buf = new UInt8 [bufSize];
#if		qDebug
		ValidateHeapStorage ();
#endif
		for (int i = 0; i < bufSize; i++) {
			buf [i] = i;		// scribble some random stuff to test if we get an overwrite
		}
#if		qDebug
		ValidateHeapStorage ();
#endif
		delete (buf);
#if		qDebug
		ValidateHeapStorage ();
#endif
	}
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

