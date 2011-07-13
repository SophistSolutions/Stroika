/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tDoubleLinkList.cc,v 1.2 1992/12/03 07:46:54 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tDoubleLinkList.cc,v $
 *		Revision 1.2  1992/12/03  07:46:54  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
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
#include	"Stroika-Foundation-Globals.hh"

#include	<iostream.h>

#include	"DoubleLinkList.hh"
#include	"Debug.hh"
#include	"SimpleClass.hh"


#include	"TestSuite.hh"


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	DoubleLinkListOfUInt32_hh
	#include	DoubleLinkListOfSimpleClass_hh
#endif

static	void	Test1();
static	void	Test2();




int	main(int /*argc*/, char* /*argv*/[])
{
	cout << "Testing DoubleLinkLists..." << endl;
	Test1();
	Test2();
	cout << "Tested DoubleLinkLists!" << endl;

	return(0);
}



static	void	Test1()
{
#if		qRealTemplatesAvailable
	DoubleLinkList<UInt32>	someDLL;
#else
	DoubleLinkList(UInt32)	someDLL;
#endif

#if		qWinOS
	const	size_t	kBigSize	=	101;
#else
	const	size_t	kBigSize	=	1001;
#endif
	TestCondition(someDLL.GetLength() == 0);
#if		0
	someDLL.SetLength(kBigSize, 0);
	someDLL.RemoveAll();
	someDLL.SetLength(kBigSize, 0);
	someDLL.SetLength(10, 0);
	someDLL.SetLength(kBigSize, 0);

	TestCondition(someDLL.GetLength() == kBigSize);
	someDLL [55] = 55;
	TestCondition(someDLL [55] == 55);
	TestCondition(someDLL [55] != 56);
	someDLL.InsertAt(1, 100);
	TestCondition(someDLL.GetLength() == kBigSize+1);
	TestCondition(someDLL [100] == 1);
	
// if debug on, new C compiler on mac chokes here - should fix better but hack it for now...
// LGP SEPT 8, 1991

// Try again - LGP 7/20/82
// Still broken as of: Wednesday, July 22, 1992 11:05:17 AM
// CFront            3.2  
//	  C              3.2.2     
#if	!qMacOS ||	!qDebug
	someDLL [101] = -someDLL [100];
	someDLL.RemoveAt(1);
	TestCondition(someDLL [100] == -1);
#endif
#endif

}

static	void	Test2()
{
#if		qRealTemplatesAvailable
	DoubleLinkList<SimpleClass>	someDLL;
#else
	DoubleLinkList(SimpleClass)	someDLL;
#endif
#if		qWinOS
	const	size_t	kBigSize	=	101;
#else
	const	size_t	kBigSize	=	1000;
#endif
#if		0
	TestCondition(someDLL.GetLength() == 0);
	someDLL.SetLength(kBigSize, 0);
	someDLL.SetLength(0, 0);
	someDLL.SetLength(kBigSize, 0);
	someDLL.SetLength(10, 0);
	someDLL.SetLength(kBigSize, 0);
	
	TestCondition(someDLL.GetLength() == kBigSize);
	someDLL [55] = 55;
	TestCondition(someDLL [55] == 55);
	TestCondition(not(someDLL [55] == 56));

	someDLL.RemoveAt(100);
	while(someDLL.GetLength() > 0) {
		someDLL.RemoveAt(1);
	}
	while(someDLL.GetLength() < kBigSize) {
		someDLL.InsertAt(1, someDLL.GetLength()+1);
	}

	someDLL.InsertAt(1, 100);
	TestCondition(someDLL.GetLength() == kBigSize+1);
	TestCondition(someDLL [100] == 1);
	someDLL [101] = 1 + someDLL [100].GetValue();
	someDLL.RemoveAt(1);
	TestCondition(someDLL [100].GetValue() == 2);
#endif
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

