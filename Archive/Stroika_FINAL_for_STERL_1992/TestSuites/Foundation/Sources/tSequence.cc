/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tSequence.cc,v 1.9 1992/12/03 07:43:28 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tSequence.cc,v $
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
 *		Revision 1.3  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
 *
 *		Revision 1.2  1992/07/03  04:24:05  lewis
 *		Rename Strings.hh -> SimpleClass.hh.
 *		Rename DoublyLinkedList->DoubleLinkList.
 *		Rename ArrayOfPointers -> ArrayPtr.
 *		Rename DoubleLLOfPointers DoubleLinkListPtr.
 *
 *		Revision 1.3  1992/06/11  00:08:16  lewis
 *		Got rid of sort stuff - redo later with different approach.
 *
 *		Revision 1.2  92/05/11  17:45:36  17:45:36  lewis (Lewis Pringle)
 *		Port to BC++/templates/PC.
 *		
 *		Revision 1.10  92/03/11  23:11:46  23:11:46  lewis (Lewis Pringle)
 *		Use cout/cerr instead of gDebugStream since thats not available when debug turned off anyhow, and using
 *		two different streams allows us to differentiate info from actual errors.
 *		
 *		Revision 1.8  1992/01/22  04:12:04  lewis
 *		Always call OSInit.  Cleaups, and test sorting.
 *
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
#include	"Sequence.hh"

#include	"SequenceTests.hh"
#include	"TestSuite.hh"


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Sequence" << endl;

	{
#if		qRealTemplatesAvailable
		Sequence<UInt32>	s;
#else
		Sequence(UInt32)	s;
#endif
		SimpleTests (s);
		s.RemoveAll ();
		Test_Deletions_During_Iteration (s);
	}

	{
#if		qRealTemplatesAvailable
		Sequence<SimpleClass>	s;
#else
		Sequence(SimpleClass)	s;
#endif
		SimpleTests (s);
		s.RemoveAll ();
		Test_Deletions_During_Iteration (s);
	}

	{
#if		qRealTemplatesAvailable
		Sequence<UInt32>		s2;
		Sequence<UInt32>		t2;
		Sequence<SimpleClass>	sS;
		Sequence<SimpleClass>	tS;
#else	/*qRealTemplatesAvailable*/
		Sequence(UInt32)		s2;
		Sequence(UInt32)		t2;
		Sequence(SimpleClass)	sS;
		Sequence(SimpleClass)	tS;
#endif	/*qRealTemplatesAvailable*/
	}

	cout << "Finished testing Sequence" << endl;
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

