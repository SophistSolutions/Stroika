/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/CheckForLocks.cc,v 1.3 1992/11/05 07:32:17 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: CheckForLocks.cc,v $
 *		Revision 1.3  1992/11/05  07:32:17  lewis
 *		Fixup spincursor usage.
 *
 *
 *
 *
 */


/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */
#pragma	push
#pragma	force_active	on
#include	"OSInit.hh"
static	OSInit	sOSInit	=	OSInit ();
#pragma	pop

#define		qUseAllGlobals		0
#define		qUseStringGlobals	1
#define		qUseFileGlobals		1
#define		qUseDebugGlobals	1
#include	"Stroika-Foundation-Globals.hh"

#include	<stdlib.h>
#include	<strstream.h>

#include	"OSRenamePre.hh"
#if		qMacOS
	#include	<CursorCtl.h>
#endif
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"Memory.hh"
#include	"Options.hh"
#include	"StreamUtils.hh"

#include	"File.hh"

static	void	CheckForLocks (ostream& to, const PathName& startFrom, Boolean showLocked, Boolean recursive, Boolean indent, int tabLevel);


int	main (int argc, char* argv[])
{
#if		qMacOS
	::InitCursorCtl (Nil);

	/*
	 * I have no idea why this is necessary, but the cursor doesn't really spin
	 * unless you give it a good "push" here.
	 */
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);
	::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);
	::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);
	::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
#endif

	const	String	kDirectory	=	"-D";
	const	String	kIndent		=	"-I";
	const	String	kShowLocks	=	"-L";
	const	String	kRecursive	=	"-R";

	Boolean 	showLocked = False;
	Boolean 	indent = False;
	Boolean 	recursive = False;

	AbstractFileSystem& f = FileSystem::Get ();
	PathName p = f.GetCurrentDirectory ();
	p = f.GetFullPathName (p);		// work around time	

	for (int i = 1; i < argc; i++) {
		String	argi	=	toupper (argv[i]);
		if (argi == kDirectory) {
			i++;
			p = FileSystem::Get ().ParseStringToPathName (argv[i]);
		}
		else if (argi == kShowLocks) {
			showLocked = True;
		}
		else if (argi == kIndent) {
			indent = True;
		}
		else if (argi == kRecursive) {
			recursive = True;
		}
		else {
			const	String	kUsage	=
				"CheckForLocks \n"
					"\t[-d <directory>]\n"
					"\t-i\n"
					"\t-l\n"
					"\t-r\n";

			cerr << "Usage: " << kUsage << endl;
			::exit (1);
		}
	}
	
	cout << "Checking for Locked Files, starting at ";
	cout << f.PathNameToString (p) << endl;
	CheckForLocks (cout, p, showLocked, recursive, indent, 0);
	return (0);
}

void	CheckForLocks (ostream& to, const PathName& startFrom, Boolean showLocked, Boolean recursive, Boolean indent, int tabLevel)
{
	AbstractFileSystem& f = FileSystem::Get ();

	ForEach (PathComponent, it, f.MakeDirectoryIterator (startFrom)) {
		PathName p = startFrom + it.Current ();	// what a Crock!!!
#if		qMacOS
		::SpinCursor(10);
#endif
		
		switch (f.GetFileType (p)) {
			case AbstractFileSystem::eBadFileType:			
				break;

			case AbstractFileSystem::ePlainFileType:
				{
					# if qMacOS
						if (FileSystem::Get_MacOS ().GetLocked (p) == showLocked) {
							#if	qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport
								for (int i = 1; i <= tabLevel; i++ {
									to << tab;
								}
							#else
								to << tab (tabLevel);
							#endif				
							to << f.PathNameToString (p) << endl;
#if		qMacOS
							::SpinCursor(1);
#endif
						}
					#endif
				}
				break;
			
			case AbstractFileSystem::eDirectoryFileType:
				if (recursive) {
					if (indent) {
						++tabLevel;
					}
					CheckForLocks (to, p, showLocked, True, indent, tabLevel);
					if (indent) {
						--tabLevel;
					}
				}
				break;

			case AbstractFileSystem::eSymbolicLink:			
				break;

			case AbstractFileSystem::eSpecialFileType:		
				break;
				
			default:	
				AssertNotReached ();
		}
	}
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

