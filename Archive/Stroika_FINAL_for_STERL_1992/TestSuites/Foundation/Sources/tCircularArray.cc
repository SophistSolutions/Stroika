/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tCircularArray.cc,v 1.4 1992/12/03 07:46:54 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tCircularArray.cc,v $
 *		Revision 1.4  1992/12/03  07:46:54  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.3  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *
 *		<<Based on tArray.cc>>
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

#include	"CircularArray.hh"
#include	"Debug.hh"
#include	"SimpleClass.hh"


#include	"TestSuite.hh"


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	CircularArrayOfUInt32_hh
	#include	CircularArrayOfSimpleClass_hh
#endif

static	void	Test1();
static	void	Test2();




int	main(int /*argc*/, char* /*argv*/[])
{
	cout << "Testing CircularArrays..." << endl;
	Test1();
	Test2();
	cout << "Tested CircularArrays!" << endl;

	return(0);
}



static	void	Test1()
{
#if		qRealTemplatesAvailable
	CircularArray<UInt32>	someArray;
#else
	CircularArray(UInt32)	someArray;
#endif

#if		qWinOS
	const	size_t	kBigSize	=	101;
#else
	const	size_t	kBigSize	=	1001;
#endif
	TestCondition(someArray.GetLength() == 0);
	someArray.SetLength(kBigSize, 0);
	someArray.RemoveAll();
	someArray.SetLength(kBigSize, 0);
	someArray.SetLength(10, 0);
	someArray.SetLength(kBigSize, 0);

	TestCondition(someArray.GetLength() == kBigSize);
	someArray [55] = 55;
	TestCondition(someArray [55] == 55);
	TestCondition(someArray [55] != 56);
	someArray.InsertAt(1, 100);
	TestCondition(someArray.GetLength() == kBigSize+1);
	TestCondition(someArray [100] == 1);
	
// if debug on, new C compiler on mac chokes here - should fix better but hack it for now...
// LGP SEPT 8, 1991

// Try again - LGP 7/20/82
// Still broken as of: Wednesday, July 22, 1992 11:05:17 AM
// CFront            3.2  
//	  C              3.2.2     
#if	!qMacOS ||	!qDebug
	someArray [101] = -someArray [100];
	someArray.RemoveAt(1);
	TestCondition(someArray [100] == -1);
#endif
}

static	void	Test2()
{
#if		qRealTemplatesAvailable
	CircularArray<SimpleClass>	someArray;
#else
	CircularArray(SimpleClass)	someArray;
#endif
#if		qWinOS
	const	size_t	kBigSize	=	101;
#else
	const	size_t	kBigSize	=	1000;
#endif
	TestCondition(someArray.GetLength() == 0);
	someArray.SetLength(kBigSize, 0);
	someArray.SetLength(0, 0);
	someArray.SetLength(kBigSize, 0);
	someArray.SetLength(10, 0);
	someArray.SetLength(kBigSize, 0);
	
	TestCondition(someArray.GetLength() == kBigSize);
	someArray [55] = 55;
	TestCondition(someArray [55] == 55);
	TestCondition(not(someArray [55] == 56));

	someArray.RemoveAt(100);
	while(someArray.GetLength() > 0) {
		someArray.RemoveAt(1);
	}
	while(someArray.GetLength() < kBigSize) {
		someArray.InsertAt(1, someArray.GetLength()+1);
	}

	someArray.InsertAt(1, 100);
	TestCondition(someArray.GetLength() == kBigSize+1);
	TestCondition(someArray [100] == 1);
	someArray [101] = 1 + someArray [100].GetValue();
	someArray.RemoveAt(1);
	TestCondition(someArray [100].GetValue() == 2);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

