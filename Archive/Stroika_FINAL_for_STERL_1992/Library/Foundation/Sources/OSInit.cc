/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/OSInit.cc,v 1.5 1992/12/08 21:30:08 lewis Exp $
 *
 * 
 *
 * TODO:
 *		+	Should really do Stack check, and heap check to see if enuf under
 *			DOS and Windows, since can really be a problem there....
 *
 *
 * Changes:
 *	$Log: OSInit.cc,v $
 *		Revision 1.5  1992/12/08  21:30:08  lewis
 *		Add case to #ifs for qWinNT.
 *
 *		Revision 1.4  1992/11/25  03:05:36  lewis
 *		Get rid of enum IsATool - use Booleans (named constants) instead.
 *		Default is now ISATOOL, since applications will usually use GDIInit(), or
 *		GUIInit() which will subclass and change arg.
 *		Add DTOR, and trap patching stuff for mac in here, instead of separate class
 *		so we can shutoff for Tools and get more reliable calling sequence.
 *		Share more code among identical DOS/WinOS and unix cases.
 *		Don't call UnloadSegs anymore on the mac - may want to change eventually, but
 *		for now...
 *		Dont do patch, or compact mem, and maybe a few others for Tools.
 *
 *		Revision 1.3  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.15  1992/05/09  02:39:00  lewis
 *		Added OSInit () cases for qDOS and qWinOS.
 *
 *		Revision 1.14  92/04/20  14:10:20  14:10:20  lewis (Lewis Pringle)
 *		Get rid of unneeded include of Xt Intrinsics - does not belong at this level.
 *		
 *		Revision 1.12  1992/01/28  19:02:42  lewis
 *		Moved exittoshell patch here from Application, since its really osrelated, and has nothing to do with application.
 *
 *		Revision 1.11  1992/01/27  03:56:31  sterling
 *		made oon Mac
 *
 *		Revision 1.9  1992/01/22  02:58:06  lewis
 *		Started moving GDI/GUI layer stuff out to appropiate layers.
 *
 *		Revision 1.7  1992/01/20  14:12:53  lewis
 *		Added warning handler for xt so we could build no-debug version that didn't dump out those
 *		Callback not found messages.
 *
 *		Revision 1.5  1991/12/27  16:46:36  lewis
 *		Added XInitToolkit, and error handler procs for X.
 *
 *
 */



#include	<stdlib.h>

#if			!qMacOS
#include	<stdio.h>
#endif

#include	"OSRenamePre.hh"
	#if		qMacOS
		#include	<Dialogs.h>
		#include	<Events.h>
		#include	<Fonts.h>
		#include	<Memory.h>
		#include	<Menus.h>
		#include	<OSUtils.h>
		#include	<QuickDraw.h>
		#include	<SegLoad.h>
		#include	<TextEdit.h>
		#include	<Traps.h>
		#include	<ToolUtils.h>
		#include	<Windows.h>
	#endif	/*OS*/
#include	"OSRenamePost.hh"


#include	"OSConfiguration.hh"

#include	"OSInit.hh"




#if		qMacOS
	// So we can safely abort when built for 020, and running on 68000
	#pragma	processor	68000

	const	kAssureThatAlert	=	1024;


	#if		qMPW_CFront
		// If apple implements this call as a macro, we must adjust it cuz we changed type type used in it...
		#ifdef	GetApplLimit
			#undef	GetApplLimit
			#define GetApplLimit() (* (void**) 0x0130)
		#endif	/*GetApplLimit*/
	#endif	/*qMPW_CFront*/

#endif	/*qMacOS*/




/*
 * Support for paching traps.
 */
#if		qMacOS
struct	OSInit_TailPatchSysHeapBlock {			// Basic algorithm from MacApp 2.0b9 trap patching code...
	short			movel;
	long			oldTrapAddr;
	short			jmp;
	pascal	void	(*newTrapAddr) ();
};
#endif	/*qMacOS*/








/*
 ********************************************************************************
 *************************************** OSInit *********************************
 ********************************************************************************
 */

#if		qMacOS
	const	Boolean	OSInit::kRequire68020				=	True;
	const	Boolean	OSInit::kNotRequire68020			=	False;
	#if		mc68020
		const	Boolean	OSInit::kDefaultRequire68020	=	True;
	#else
		const	Boolean	OSInit::kDefaultRequire68020	=	False;
	#endif

	const	Boolean	OSInit::kRequireFPU					=	True;
	const	Boolean	OSInit::kNotRequireFPU				=	False;
	const	Boolean	OSInit::kDefaultRequireFPU			=	False;

	const	Boolean	OSInit::kIsATool					=	True;
	const	Boolean	OSInit::kIsNotATool					=	False;
#endif


#if		qMacOS
OSInit_TailPatchSysHeapBlock*	OSInit::sExitToShellPatch	=	Nil;

extern	"C"	void	_DataInit ();
OSInit::OSInit (Boolean isATool, size_t minHeap, size_t minStack, Boolean require68020Plus, Boolean requireFPU)
{
#if		0
// Don't bother with unloadsegs now since not sure they are a good idea if we use
// model far, and put everything into one segment - perhaps add back later...
	/*
	 * unload data init segment since it is no longer needed. Do as soon as possible since
	 * we want to avoid heap fragmentation.
	 */
	::UnloadSeg (_DataInit);
#endif

	/*
	 * NB: It is important to do these initializations before any other checks since they
	 * might cause an alert to be displayed - that cannot be done until we have called
	 * ::InitDialogs ().
	 *
	 * Also, note that we don't do any of these initializations if we are running under MPW as
	 * a tool.
	 */
	if (not isATool) {
		/*
		 * Really get rid of cruft
		 */
		const	size_t	bigSize	=	0x8fffffff;
		::PurgeMem (bigSize); 
		(void)::CompactMem (bigSize); 

		/*
		 * Call Generic Toolbox Initializer procedures before doing anything else, since we may
		 * want to put up an alert somewhere along the lines.
		 */
		::InitGraf (osPtr (&qd.thePort));
		::InitFonts ();
		::InitWindows ();
		::FlushEvents (mDownMask | mUpMask | keyDownMask | keyUpMask | autoKeyMask, 0);	// flush user-interaction events
		::InitMenus ();
		::TEInit ();
		::InitDialogs (Nil);

		/* 
		 * InitCursor as late in the game as possible, and quickly thereafter set cursor to be
		 * busycursor, since thats what it should be til app starts up.
		 */
		::InitCursor ();
		::osSetCursor (*GetCursor (watchCursor));
	}



	/*
	 * Subtle point here - make automatic variable so as to guarentee a constructor gets called
	 * for OSConfiguration before accessing its methods.  Other places called after
	 * were all started up, can just us gOSConfiguration.
	 */
	OSConfiguration	osConfig;

	if (not isATool) {
		/*
		 * Setup memory stuff
		 */
		// quickly hacked togegher from bried look at IM mem mgr chapter - review, and
		// see if done well...  Review vis-a-vis system 7, AUX, etc...
	
		/*
		 * If more stack requested than OS gives by default, try to grow stack.  Then, grow
		 * heap to max size so that code segs can get loaded at opposite end of mallocs??? Not
		 * too sure about whole strategy.
		 */
		if (osConfig.StackAvailable () < minStack) {
			AssureThat (Boolean (osConfig.StackAvailable () >= 2*1024), "Very Little Stackspace on startup(1).");
			::SetApplLimit (osPtr (size_t (GetApplLimit ()) + osConfig.StackAvailable () - minStack));
			AssureThat (Boolean (osConfig.StackAvailable () >= minStack-5*1024), "Very Little Stackspace on startup(2).");
			AssureThat (Boolean (osConfig.StackAvailable () < minStack+5*1024), "Too much stack space.");
		}
		::MaxApplZone ();
	
		// macapp has some hack where you dont to loop, can set some field, and do all at once...
		// also, has way to check how many really needed for tuning...
		const	kMasterPointerBlocks	=	3;					// Wild guess - used to guess 20??? Who knows???
#if		1
		// if macapp 2.0 does this - I assume apple will continue to support the hack...
		short oldMoreMast = ApplicZone ()->moreMast;
		ApplicZone ()->moreMast = oldMoreMast * kMasterPointerBlocks;
		::MoreMasters ();
		ApplicZone ()->moreMast = oldMoreMast;
#else
		for (int i = 1; i <= kMasterPointerBlocks; i++) {
			::MoreMasters ();
		}
#endif
	}


	/*
	 *		Patch the ExitToShell () trap so if we are under macsbug and say ES, then
	 *	we will get our DTORS called.
	 */
	if (not isATool) {
		/*
		 * MacApp had this thing alloced on the sysheap, and that seems important.
		 * I tried using just plain NewPtr() and it didn't seem to work right? (system 7.0 final).
		 * Not big deal, but I wish this magic was documented better!
		 */
		// Assert (sExitToShellPatch == Nil);	- no asserts available here...
		sExitToShellPatch =	(OSInit_TailPatchSysHeapBlock*)::NewPtrSys (sizeof (OSInit_TailPatchSysHeapBlock));
	
		sExitToShellPatch->movel		=	0x2f3c;		//	move.l #oldTrapAddr,-(sp)
		sExitToShellPatch->oldTrapAddr	=	::NGetTrapAddress (_ExitToShell, ToolTrap);
		sExitToShellPatch->jmp			=	0x4ef9;		//	jmp	#newTrapAddr
		sExitToShellPatch->newTrapAddr	=	&CleanUpProc;
		::NSetTrapAddress (long (sExitToShellPatch), _ExitToShell, ToolTrap);
	}


	// FreeMem gives us amount of heap available for use as heap.  Note, code can get
	// paged in/out, and screw up estimate, but best we can do for now.
	AssureThat (Boolean (::FreeMem () >= minHeap), "Not enough heap space");

	if (require68020Plus) {
		AssureThat (osConfig.Has68020Plus (), "68020+ required to run this program.");
	}
	if (requireFPU) {
		AssureThat (osConfig.HasFPU (), "Floating point processor required to run this program.");
	}


#if		0
// Don't bother with unloadsegs now since not sure they are a good idea if we use
// model far, and put everything into one segment - perhaps add back later...
	/*
	 * For some reason, this doesn't seem to have any effect.
	 * No harm though, and we'll figure it out later.  Low prioity since segment is
	 * very small.
	 */
	::UnloadSeg ((void*)&OSInit::AssureThat);	// hopefully safe as we gracefully exit...
#endif
}

#elif	qDOS || qWinNT || qWinOS || qUnixOS

OSInit::OSInit (size_t minHeap, size_t minStack)
{
}

#endif	/*OS*/

OSInit::~OSInit ()
{
#if		qMacOS
	if (sExitToShellPatch != Nil) {
		::NSetTrapAddress (sExitToShellPatch->oldTrapAddr, _ExitToShell, ToolTrap);
		::DisposPtr (osPtr (sExitToShellPatch));
		sExitToShellPatch = Nil;
	}
#endif
}

void	OSInit::AssureThat (Boolean conditionTrue, const char* conditionName)
{
#if		qMacOS
	if (not (conditionTrue)) {
		/* do paramText, etc... and mac OS alert */
		::paramtext ((char*)conditionName, Nil, Nil, Nil);
		::osStopAlert (kAssureThatAlert, Nil);
		exit (1);
	}
#elif	qDOS || qWinNT || qWinOS || qUnixOS
	if (not conditionTrue) {
		fprintf (stderr, "Sorry: %s\n", conditionName);
		exit (1);
	}
#endif	/*OS*/
}

#if		qMacOS
pascal	void	OSInit::CleanUpProc ()
{
	(void)SetCurrentA5 ();
	::exit (EXIT_FAILURE);			// should call static destructors....
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


