/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tSequence_DoubleLinkList.cc,v 1.12 1992/12/03 07:43:28 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tSequence_DoubleLinkList.cc,v $
 *		Revision 1.12  1992/12/03  07:43:28  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.11  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.10  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.9  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.6  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
 *
 *		Revision 1.3  1992/07/21  06:52:44  lewis
 *		Use  Sequence_DoubleLinkList instead of obsolete  Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.2  1992/07/03  04:24:05  lewis
 *		Rename Strings.hh -> SimpleClass.hh.
 *		Rename DoublyLinkedList->DoubleLinkList.
 *		Rename ArrayOfPointers -> ArrayPtr.
 *		Rename DoubleLLOfPointers DoubleLinkListPtr.
 *
 *		Revision 1.2  1992/06/11  00:08:19  lewis
 *		Got rid of sort stuff - redo later with different approach.
 *
 *		Revision 1.2  92/05/11  17:51:28  17:51:28  lewis (Lewis Pringle)
 *		Ported to BC++/templates/PC.
 *		Got rid of old qMPW/osInit/eIsATool hack for MPW.
 *		
 *		Revision 1.11  92/03/11  23:11:46  23:11:46  lewis (Lewis Pringle)
 *		Use cout/cerr instead of gDebugStream since thats not available when debug turned off anyhow, and using
 *		two different streams allows us to differentiate info from actual errors.
 *		
 *		Revision 1.10  1992/01/22  04:12:22  lewis
 *		Always call OSInit.  Cleaups, and test sorting.
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
#include	"Sequence_DoubleLinkList.hh"

#include    "SequenceTests.hh"
#include	"TestSuite.hh"


#if		!qRealTemplatesAvailable
	#include Sequence_DoubleLinkListOfUInt32_hh
	#include Sequence_DoubleLinkListOfSimpleClass_hh
#endif


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Sequence_DoubleLinkList" << endl;

	{
#if		qRealTemplatesAvailable
		Sequence_DoubleLinkList<UInt32>	s;
#else
		Sequence_DoubleLinkList(UInt32)	s;
#endif
		SimpleTests (s);
		s.RemoveAll ();
		Test_Deletions_During_Iteration (s);
	}

	{
#if		qRealTemplatesAvailable
		Sequence_DoubleLinkList<SimpleClass>	s2;
#else
		Sequence_DoubleLinkList(SimpleClass)	s2;
#endif
		SimpleTests (s2);
		s2.RemoveAll ();
		Test_Deletions_During_Iteration (s2);
	}

	{
#if		qRealTemplatesAvailable
		Sequence_DoubleLinkList<UInt32>			s2;
		Sequence_DoubleLinkList<UInt32>			t2;
		Sequence_DoubleLinkList<SimpleClass>	sS;
		Sequence_DoubleLinkList<SimpleClass>	tS;
#else
		Sequence_DoubleLinkList(UInt32)			s2;
		Sequence_DoubleLinkList(UInt32)			t2;
		Sequence_DoubleLinkList(SimpleClass)	sS;
		Sequence_DoubleLinkList(SimpleClass)	tS;
#endif
	}

	cout << "Finished testing Sequence_DoubleLinkList" << endl;
	return (0);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

