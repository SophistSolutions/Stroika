/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1995.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Sources/som_LedPartInit.cpp,v 2.5 1996/12/13 18:10:13 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: som_LedPartInit.cpp,v $
 *	Revision 2.5  1996/12/13 18:10:13  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.4  1996/09/01  15:44:51  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.3  1996/04/18  16:15:19  lewis
 *	added LedPartCFMTerminate () as per OpenDoc 1.0.2 release notes.
 *
 *	Revision 2.2  1996/01/11  08:54:08  lewis
 *	commented out the __sinit() call - maybe obsolete with CW8? Anyhow things
 *	seem to work fine without it for now. Look into again later if
 *	I see any need...
 *
 *	Revision 2.1  1995/12/15  04:57:04  lewis
 *	if building for mwerks compiler, msut call __sinit() to get static members etc
 *	inited (since we've no main - we are a code fragment).
 *
 *	
 *
 *
 *
 */

#include	<CodeFragments.h>

#include	<UseRsrcM.h>
#include	<ODMemory.h>




#if		defined __MWERKS__
extern "C" void __sinit();		// From Metrowerks runtime library, in <Startup.c>
#endif




//------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------

extern	"C"	pascal	OSErr	LedPartCFMInit (CFragInitBlockPtr initBlkPtr);
extern	"C"	pascal	void	LedPartCFMTerminate ();


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

extern	"C"	pascal	OSErr	LedPartCFMInit (CFragInitBlockPtr initBlkPtr)
{
	// If the mouse button is down while we are constructing
	// Then enter the debugger so that we can set breakpoints 
	#ifdef ODDebug
	if (Button())
		DebugStr("\pBreak at LedPart Constructor.");
	#endif

#if	defined __MWERKS__ && 0
	__sinit();		// Initialize static data and construct any global objects
#endif

	// We are using OpenDoc's memory mgmt calls, so we must
	// initialize the interface to the memory manager.
	OSErr err1 = InitODMemory();
	// We must also capture the relevant information about
	// our library file so that we may access our resources.
	OSErr err2 = InitLibraryResources(initBlkPtr);
	
	return (OSErr) (err1 != noErr) ? err1 : err2;
}

extern	"C"	pascal	void	LedPartCFMTerminate ()
{
	// If the mouse button is down while we are destructing
	// Then enter the debugger 
	#ifdef ODDebug
	if (Button())
		DebugStr("\pBreak at LedPart Constructor.");
	#endif

	CloseLibraryResources ();
}



