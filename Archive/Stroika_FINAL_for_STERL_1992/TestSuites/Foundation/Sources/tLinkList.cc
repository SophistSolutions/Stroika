/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tLinkList.cc,v 1.4 1992/12/05 06:24:28 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tLinkList.cc,v $
 *		Revision 1.4  1992/12/05  06:24:28  lewis
 *		Fix Template(X) -> Template<X>.
 *
 *		Revision 1.3  1992/12/05  00:02:35  lewis
 *		*** empty log message ***
 *
 *		Revision 1.2  1992/12/03  07:48:18  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *		Try to transcibe more of the array testuite into real link list
 *		tests. Mostly, but not totaly done.
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

#include	"LinkList.hh"
#include	"Debug.hh"
#include	"SimpleClass.hh"


#include	"TestSuite.hh"


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	LinkListOfUInt32_hh
	#include	LinkListOfSimpleClass_hh
#endif

static	void	Test1();
static	void	Test2();




int	main(int /*argc*/, char* /*argv*/[])
{
	cout << "Testing LinkLists..." << endl;
	Test1();
	Test2();
	cout << "Tested LinkLists!" << endl;

	return(0);
}



static	void	Test1()
{
#if		qRealTemplatesAvailable
	LinkList_Patch<UInt32>	someLL;
#else
	LinkList_Patch(UInt32)	someLL;
#endif

#if		qWinOS
	const	size_t	kBigSize	=	101;
#else
	const	size_t	kBigSize	=	1001;
#endif

	/*
	 *		This testsuite is transcribed from tArray.cc - thats why it looks so bizare for
	 *	a linked list test suite.
	 */

	Assert (kBigSize > 10);		// this transcription from array depends on this..
	TestCondition(someLL.GetLength() == 0);
	{ for (size_t i = 1; i <= kBigSize; i++) { someLL.Prepend (0); } }
	someLL.RemoveAll();
	{ for (size_t i = 1; i <= kBigSize; i++) { someLL.Prepend (0); } }
	{ for (size_t i = 1; i <= kBigSize-10; i++) { someLL.RemoveFirst (); } }
	someLL.RemoveAll();														//	someLL.SetLength(kBigSize, 0);
	{ for (size_t i = 1; i <= kBigSize; i++) { someLL.Prepend (0); } }


	TestCondition(someLL.GetLength() == kBigSize);
	someLL.SetAt (55, 55);																										//	someLL [55] = 55;
	TestCondition (someLL.GetAt (55) == 55);																					//	TestCondition(someArray [55] == 55);
	TestCondition (someLL.GetAt (55) != 56);																					//	TestCondition(someArray [55] != 56);
#if		qRealTemplatesAvailable
	{ size_t i = 1; for (LinkListMutator_Patch<UInt32> it (someLL); it.More(); i++) { if (i==100) {it.AddBefore (1); break;} } }//	someLL.InsertAt(1, 100);
#else
	{ size_t i = 1; for (LinkListMutator_Patch(UInt32) it (someLL); it.More(); i++) { if (i==100) {it.AddBefore (1); break;} } }//	someLL.InsertAt(1, 100);
#endif
	TestCondition(someLL.GetLength() == kBigSize+1);
	TestCondition (someLL.GetAt (100) == 1);																					//	TestCondition(someArray [100] == 1);
	
	someLL.SetAt (-someLL.GetAt (100), 101);		//	someLL [101] = -someLL [100];
	someLL.RemoveFirst ();							//	someArray.RemoveAt(1);
	TestCondition (someLL.GetAt (100) == -1);		//	TestCondition(someLL [100] == -1);
	
}

static	void	Test2()
{
#if		qRealTemplatesAvailable
	LinkList<SimpleClass>	someLL;
#else
	LinkList(SimpleClass)	someLL;
#endif
#if		qWinOS
	const	size_t	kBigSize	=	101;
#else
	const	size_t	kBigSize	=	1000;
#endif
	TestCondition(someLL.GetLength() == 0);
#if		0
	someLL.SetLength(kBigSize, 0);
	someLL.SetLength(0, 0);
	someLL.SetLength(kBigSize, 0);
	someLL.SetLength(10, 0);
	someLL.SetLength(kBigSize, 0);
	
	TestCondition(someLL.GetLength() == kBigSize);
	someLL [55] = 55;
	TestCondition(someLL [55] == 55);
	TestCondition(not(someLL [55] == 56));

	someLL.RemoveAt(100);
	while (someLL.GetLength() > 0) {
		someLL.RemoveAt(1);
	}
	while (someLL.GetLength() < kBigSize) {
		someLL.InsertAt(1, someLL.GetLength()+1);
	}

	someLL.InsertAt(1, 100);
	TestCondition(someLL.GetLength() == kBigSize+1);
	TestCondition(someLL [100] == 1);
	someLL [101] = 1 + someLL [100].GetValue();
	someLL.RemoveAt(1);
	TestCondition(someLL [100].GetValue() == 2);
#endif
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

