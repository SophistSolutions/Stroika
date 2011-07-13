/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tSimpleTree.cc,v 1.7 1992/12/03 07:43:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tSimpleTree.cc,v $
 *		Revision 1.7  1992/12/03  07:43:00  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug and use it.More() instead
 *		of it.Next().
 *
 *		Revision 1.6  1992/11/26  03:37:54  lewis
 *		fixed typo.
 *
 *		Revision 1.5  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.4  1992/10/15  21:13:12  lewis
 *		Updated for SimpleClass.
 *
 *		Revision 1.3  1992/10/14  06:13:09  lewis
 *		Moved a little closer to supporting new simpleclass stuff and
 *		GenClass usage - still needs work though.
 *
 *		Revision 1.2  1992/09/20  01:27:45  lewis
 *		Lots of extentions and debugging of tree code. Main addition
 *		was TestStillSorted.
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
#include	"Format.hh"
#include	"SimpleTree.hh"
#include	"StreamUtils.hh"
#include	"Time.hh"

#include	"SimpleClass.hh"


#define		qPrintTimings		!qDebug


#include	"TestSuite.hh"


static	void	Test1 ();
static	void	Test2 ();

#if		!qRealTemplatesAvailable
    #include    "TFileMap.hh"
	#include	SimpleTreeOfUInt32_hh
	#include	SimpleTreeOfSimpleClass_hh
#endif	/*!qRealTemplatesAvailable*/


#if		qRealTemplatesAvailable
	static	void	TestStillSorted (SimpleTree<UInt32>& s);
	static	void	TestStillSorted (SimpleTree<SimpleClass>& s);
#else
	static	void	TestStillSorted (SimpleTree(UInt32)& s);
	static	void	TestStillSorted (SimpleTree(SimpleClass)& s);
#endif


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing SimpleTrees..." << endl;

#if		qPrintTimings
	Time t = GetCurrentTime ();
	cout << tab << "timing SimpleTrees..." << endl;
#endif
	Test1 ();
#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished timing SimpleTrees; time elapsed = " << t << endl;
#endif

	Test2 ();
	cout << "Tested SimpleTrees!" << endl;
	return (0);
}


static	void	Test1 ()
{
#if		qRealTemplatesAvailable
	SimpleTree <UInt32>	someSimpleTree;
#else
	SimpleTree (UInt32)	someSimpleTree;
#endif

	const	size_t	kTestSize	=	100;
	
	for (size_t i = 1; i <= kTestSize; i++) {
		TestCondition (not someSimpleTree.Contains (i));
		someSimpleTree.Add (i);
		TestCondition (someSimpleTree.Contains (i));
	}
		TestStillSorted (someSimpleTree);
	for (i = 1; i <= kTestSize; i++) {
		TestCondition (someSimpleTree.Contains (i));
		someSimpleTree.Remove (i);
		TestStillSorted (someSimpleTree);
		TestCondition (not someSimpleTree.Contains (i));
	}
	for (i = 1; i <= kTestSize; i++) {
		TestCondition (not someSimpleTree.Contains (i));
		someSimpleTree.Add (i);
		TestStillSorted (someSimpleTree);
		TestCondition (someSimpleTree.Contains (i));
	}
	for (i = kTestSize; i >= 1; i--) {
		TestCondition (someSimpleTree.Contains (i));
		someSimpleTree.Remove (i);
		TestStillSorted (someSimpleTree);
		TestCondition (not someSimpleTree.Contains (i));
	}
}

static	void	Test2 ()
{
#if		qRealTemplatesAvailable
	SimpleTree <SimpleClass>	someSimpleTree;
#else
	SimpleTree (SimpleClass)	someSimpleTree;
#endif

	const	size_t	kTestSize	=	100;
	
	for (size_t i = 1; i <= kTestSize; i++) {
		SimpleClass	s	=	i;
		TestCondition (not someSimpleTree.Contains (s));
		someSimpleTree.Add (s);
		TestStillSorted (someSimpleTree);
		TestCondition (someSimpleTree.Contains (s));
	}

	for (i = 1; i <= kTestSize; i++) {
		SimpleClass	s	=	i;
		TestCondition (someSimpleTree.Contains (s));
		someSimpleTree.Remove (s);
		TestStillSorted (someSimpleTree);
		TestCondition (not someSimpleTree.Contains (s));
	}

	for (i = 1; i <= kTestSize; i++) {
		SimpleClass	s	=	i;
		TestCondition (not someSimpleTree.Contains (s));
		someSimpleTree.Add (s);
		TestStillSorted (someSimpleTree);
		TestCondition (someSimpleTree.Contains (s));
	}

	for (i = kTestSize; i >= 1; i--) {
		SimpleClass	s	=	i;
		TestCondition (someSimpleTree.Contains (s));
		someSimpleTree.Remove (s);
		TestStillSorted (someSimpleTree);
		TestCondition (not someSimpleTree.Contains (s));
	}
}






#if		qRealTemplatesAvailable
static	void	TestStillSorted (SimpleTree<UInt32>& s)
#else
static	void	TestStillSorted (SimpleTree(UInt32)& s)
#endif
{
	UInt32	prev		=	0;
	Boolean	prevGood	=	False;
#if		qRealTemplatesAvailable
	for (SimpleTreeIterator<UInt32> it (s);
#else
	for (SimpleTreeIterator(UInt32) it (s);
#endif
		 it.More ();) {
		if (prevGood) {
			TestCondition (prev <= it.Current ());
		}
		prev = it.Current ();
		prevGood = True;
	}
}

#if		qRealTemplatesAvailable
static	void	TestStillSorted (SimpleTree<SimpleClass>& s)
#else
static	void	TestStillSorted (SimpleTree(SimpleClass)& s)
#endif
{
	SimpleClass	prev		=	SimpleClass (1);
	Boolean		prevGood	=	False;
#if		qRealTemplatesAvailable
	for (SimpleTreeIterator<SimpleClass> it (s);
#else
	for (SimpleTreeIterator(SimpleClass) it (s);
#endif
		 it.More ();) {
		if (prevGood) {
			TestCondition ((prev < it.Current ()) or (prev == it.Current ()));
		}
		prev = it.Current ();
		prevGood = True;
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

