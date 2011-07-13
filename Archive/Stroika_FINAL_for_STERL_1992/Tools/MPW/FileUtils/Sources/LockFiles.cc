/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/LockFiles.cc,v 1.3 1992/11/05 07:32:17 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: LockFiles.cc,v $
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

#include	"Bag.hh"
#include	"Debug.hh"
#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"File.hh"


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
#endif


#if		qRealTemplatesAvailable
	static	void	LockFiles (ostream& to, const PathName& startFrom, Boolean unlock, Boolean recursive, Boolean verbose, const Sequence<PathComponent>& filesToSkip);
#else
	static	void	LockFiles (ostream& to, const PathName& startFrom, Boolean unlock, Boolean recursive, Boolean verbose, const Sequence(PathComponent)& filesToSkip);
#endif

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
	const	String	kUnlock		=	"-U";
	const	String	kRecursive	=	"-R";
	const	String	kSkip		=	"-S";
	const	String	kVerbose	=	"-V";

	Boolean 	unlock = False;
	Boolean 	recursive = False;
	Boolean 	verbose = False;
	#if		qRealTemplatesAvailable
		Sequence<PathComponent>	filesToSkip;
	#else
		Sequence(PathComponent)	filesToSkip;
	#endif

	AbstractFileSystem& f = FileSystem::Get ();
	PathName p = f.GetCurrentDirectory ();
	p = f.GetFullPathName (p);		// work around time	

	for (int i = 1; i < argc; i++) {
		String	argi	=	toupper (argv[i]);
		if (argi == kDirectory) {
			i++;
			p = FileSystem::Get ().ParseStringToPathName (argv[i]);
		}
		else if (argi == kRecursive) {
			recursive = True;
		}
		else if (argi == kSkip) {
			i++;
			filesToSkip += PathComponent (argv[i]);
		}
		else if (argi == kUnlock) {
			unlock = True;
		}
		else if (argi == kVerbose) {
			verbose = True;
		}
		else {
			const	String	kUsage	=
				"LockFiles \n"
					"\t[-d <directory>]\n"
					"\t-r\n"
					"\t[-s <file name>]\n"
					"\t-u\n"
					"\t-v\n";

			cerr << "Usage: " << kUsage << endl;
			::exit (1);
		}
	}
	
	if (verbose) {
		if (unlock) {
			cout << "Unlocking Files, starting at ";
		}
		else {
			cout << "Locking Files, starting at ";
		}
		cout << f.PathNameToString (p) << endl;
	}
	LockFiles (cout, p, unlock, recursive, verbose, filesToSkip);
	return (0);
}

#if		qRealTemplatesAvailable
	void	LockFiles (ostream& to, const PathName& startFrom, Boolean unlock, Boolean recursive, Boolean verbose, const Sequence<PathComponent>& filesToSkip)
#else
	void	LockFiles (ostream& to, const PathName& startFrom, Boolean unlock, Boolean recursive, Boolean verbose, const Sequence(PathComponent)& filesToSkip)
#endif
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
						if (not filesToSkip.Contains (p.GetFileName ())) {
							if ((verbose) and (FileSystem::Get_MacOS ().GetLocked (p) == unlock)) {
								if (unlock) {
									to << "unlocking ";
								}
								else {
									to << "locking ";
								}
								to << f.PathNameToString (p) << endl;
							}
#if		qMacOS
							::SpinCursor(1);
#endif
							FileSystem::Get_MacOS ().SetLocked (p, Boolean (not unlock));
						}
					#endif
				}
				break;
			
			case AbstractFileSystem::eDirectoryFileType:
				if (recursive) {
					LockFiles (to, p, unlock, True, verbose, filesToSkip);
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

