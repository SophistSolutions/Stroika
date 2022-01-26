/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tFile.cc,v 1.10 1992/12/03 07:47:12 lewis Exp $
 *
 * TODO:
 *		-	Nearly total rewrite of this testsuite is needed. We must do a version
 *			where we make NO modifications to the file system, and one proc that does
 *			that is only invoked with special command line argument.
 *
 * Changes:
 *	$Log: tFile.cc,v $
 *		Revision 1.10  1992/12/03  07:47:12  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *		Got rid of reams of old crap that didn't compile anyway - grossly out of
 *		date test code. Write decent tests from scratch someday soon.
 *
 *		Revision 1.9  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.8  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
 *
 *		Revision 1.7  1992/09/11  13:39:38  lewis
 *		Use endl instead of newline
 *
 *		Revision 1.6  1992/09/05  06:16:07  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/04  01:13:10  lewis
 *		Added qActuallyPrintStuff flag, and parameterize writting to stream conditioned on
 *		this flag, and not always directly to cout. Use proper bug define ifdefs, and test
 *		under gcc.
 *
 *		Revision 1.3  1992/07/21  17:45:11  lewis
 *		Hack around gcc bug.
 *
 *		Revision 1.2  1992/07/10  22:38:30  lewis
 *		Hacked up for new file support - this needs a total rewrite.
 *
 *		Revision 1.2  1992/05/11  17:40:05  lewis
 *		BC++ overloading problem passing const char* to PathName::CTOR -
 *		must explicityly cast to String() first??? Not sure whose right?
 *
 *		Revision 1.11  92/03/11  23:11:46  23:11:46  lewis (Lewis Pringle)
 *		Use cout/cerr instead of gDebugStream since thats not available when debug turned off anyhow, and using
 *		two different streams allows us to differentiate info from actual errors.
 *		
 *		Revision 1.10  1992/02/20  06:43:09  lewis
 *		Comment out unix/dos specific test for mac.
 *
 *		Revision 1.9  1992/01/22  04:11:06  lewis
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
#define		qUseFileGlobals		1
#include	"Stroika-Foundation-Globals.hh"

#include	<iostream.h>
#include	<strstream.h>

#include	"Debug.hh"
#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"File.hh"

#include	"TestSuite.hh"



#define	qActuallyPrintStuff	0


static	void	Test_Files (ostream& to);
static	void	PrintFileListing (ostream& out, const PathName& startFrom, Boolean showFullPath, Boolean recurse = True, int tabLevel = 0);
static	void	BobFileTest ();

int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing files..." << endl;
#if		qActuallyPrintStuff
	Test_Files (cout);
#else
	strstream	devNull;
	#if		qGCC_InserterToSubclassOfOStreamBug
		Test_Files ((ostream&)devNull);		// redirect to devNull from here???,
	#else
		Test_Files (devNull);				// redirect to devNull from here???,
	#endif
#endif
	cout <<"Tested files!" << endl;
	return (0);
}

void	Test_Files (ostream& to)
{
	to << "Mounted volumes (and one level ls of those volumes) are:" << endl;

	ForEach (PathComponent, it, FileSystem::Get ().MakeMountedVolumeIterator ()) {
		to << tab << it.Current ().GetName () << endl;
	}
	to << "End of volumes" << endl;
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
