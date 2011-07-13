/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tArray.cc,v 1.12 1992/12/03 07:45:38 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tArray.cc,v $
 *		Revision 1.12  1992/12/03  07:45:38  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *		Got rid of #define qUseDebugGlobals.
 *
 *		Revision 1.11  1992/11/15  04:12:00  lewis
 *		#include "SimpleClass.hh". Also, squeeze out spaces before () calls.
 *
 *		Revision 1.10  1992/10/30  01:43:12  lewis
 *		Use macro names instead of absolute file names for GenClassed files.
 *
 *		Revision 1.9  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.8  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.7  1992/09/25  21:49:16  lewis
 *		Started hacking in support for new GenClass stuff - just testing.
 *
 *		Revision 1.6  1992/09/18  15:34:40  lewis
 *		Got rid of unneeded Declare of Collection.
 *
 *		Revision 1.5  1992/09/11  13:17:48  lewis
 *		Use endl instead of newline, use <iostream.h> instead of StreamUtils.hh,
 *		and properly indent #ifs as per new style.
 *
 *		Revision 1.3  1992/07/21  06:50:09  lewis
 *		Had to help along gcc's overloading by adding explict conversion
 *		of "-" to String. Not sure who is right. Also, re-enable code that
 *		was commneted out for mac - said caused compiler crash - if so,
 *		add real bug define to Config.hh and update date.
 *
 *		Revision 1.2  1992/07/03  04:24:05  lewis
 *		Rename Strings.hh -> String.hh.
 *		Rename DoublyLinkedList->DoubleLinkList.
 *		Rename ArrayOfPointers -> ArrayPtr.
 *		Rename DoubleLLOfPointers DoubleLinkListPtr.
 *
 *		Revision 1.2  1992/05/10  00:54:58  lewis
 *		Port to BC++/templates.
 *
 *		Revision 1.7  92/03/11  23:11:46  23:11:46  lewis (Lewis Pringle)
 *		Use cout/cerr instead of gDebugStream since thats not available when debug turned off anyhow, and using
 *		two different streams allows us to differentiate info from actual errors.
 *		
 *		Revision 1.6  1992/01/22  04:10:43  lewis
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
#include	"Stroika-Foundation-Globals.hh"

#include	<iostream.h>

#include	"Array.hh"
#include	"Debug.hh"
#include	"SimpleClass.hh"


#include	"TestSuite.hh"


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	ArrayOfUInt32_hh
	#include	ArrayOfSimpleClass_hh
#endif

static	void	Test1();
static	void	Test2();




int	main(int /*argc*/, char* /*argv*/[])
{
	cout << "Testing arrays..." << endl;
	Test1();
	Test2();
	cout << "Tested arrays!" << endl;

	return(0);
}



static	void	Test1()
{
#if		qRealTemplatesAvailable
	Array<UInt32>	someArray;
#else
	Array(UInt32)	someArray;
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
	Array<SimpleClass>	someArray;
#else
	Array(SimpleClass)	someArray;
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

