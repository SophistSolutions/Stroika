/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItPP/Sources/Main.cpp,v 2.15 1999/07/13 14:14:29 lewis Exp $
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
 *	$Log: Main.cpp,v $
 *	Revision 2.15  1999/07/13 14:14:29  lewis
 *	can now do MacTmpMem with DEBUG mode - just must set a flag. And must always define NEWMODE before including New.cpp, cuz now they default to malloc-mode, which doesn't afford us a hook to use TMP-MEM.
 *	
 *	Revision 2.14  1997/12/24 04:42:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.13  1997/07/27  16:02:25  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.12  1996/12/13  18:11:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.11  1996/12/05  21:25:04  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1996/10/31  01:05:15  lewis
 *	Don't use up the last few K of heap-memory cuz mac toolbox doesn't like it! -LGP 961021
 *	CW10/__throws_bad_alloc.
 *	minor cleanups.
 *
 *	Revision 2.9  1996/09/30  15:11:33  lewis
 *	Revised DebugNew code for CW10.
 *
 *	Revision 2.8  1996/09/01  15:45:40  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.7  1996/07/03  01:42:21  lewis
 *	Lose TryToSendSomeTmpMemBackToOS() - maybe try again for some later release.
 *
 *	Revision 2.6  1996/06/01  02:54:49  lewis
 *	trail code to restore heaps back to tmp mem.
 *
 *	Revision 2.5  1996/05/14  20:49:52  lewis
 *	Added profiler support for MWERKS.
 *
 *	Revision 2.4  1996/05/05  14:56:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1996/04/18  16:04:16  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1996/03/16  19:22:22  lewis
 *	Cleanups and moved qUseMacTmpMemForAllocs definition to LedConfig.h
 *
 *	Revision 2.1  1996/03/04  08:22:26  lewis
 *	Set new handler - so when we run out of memory, we throw - instead of
 *	stompin gmemory. Sadly it appears MWKERS doesn't default to throwing.
 *
 *	Revision 2.0  1996/02/26  23:29:57  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */
#include	<new.h>

#include	<UMemoryMgr.h>			// for InitializeHeap
#include	<UDrawingState.h>		// for class UQDGlobals

#include	"LedItApplication.h"



#if		__profile__
	#define	qProfile		1
#endif

#ifndef	qProfile
	#define	qProfile		0
#endif


#if		qProfile && defined (__MWERKS__)
	#include	"profiler.h"
#endif


#if		DEBUG_NEW
#include	"DebugNew.h"
#endif


#if		qUseMacTmpMemForAllocs
	inline	char*	DoSysAlloc (size_t n)
		{
			#if		DEBUG_NEW
			gDebugNewFlags &= ~dnCheckBlocksInApplZone;
			#endif
			OSErr err;	// ingored...
			Handle	h	=	::TempNewHandle (n, &err);	// try temp mem, and use our local mem if no temp mem left
			if (h == NULL) {
				h = ::NewHandle (n);
				// Don't use up the last few K of heap-memory cuz mac toolbox doesn't like it! -LGP 961021
				if (h != NULL) {
					Handle	x	=	::NewHandle (4*1024);
					if (x == NULL) {
						::DisposeHandle (h);
						h = NULL;
					}
					else {
						::DisposeHandle (x);
					}
				}
			}
			if (h == NULL) {
				return NULL;
			}
			HLock (h);
			return *h;
		}
	inline	void	DoSysFree (void* p)
		{
			Handle	h	=	::RecoverHandle (Ptr (p));
			::HUnlock (h);
			::DisposeHandle (h);
		}
	#define	NewPtr 		DoSysAlloc
	#define DisposePtr	DoSysFree
#endif



/*
 *	Malloc/Free/OpNew etc...
 */
#define	NEWMODE	NEWMODE_NORMAL
#if		DEBUG_NEW
	#include	"DebugNew.cp"
#else
	#include	<New.cp>
#endif




void	main ()
{
	#if		qDebug
		// Set Debugging options
		SetDebugThrow_(debugAction_Alert);
		SetDebugSignal_(debugAction_Alert);
	#endif

	const	long	kMinStack	=	32l*1024l;	// reserve  32K for stack
	::SetApplLimit ((Ptr) ((long) (::GetApplLimit ()) - kMinStack));

	::_set_newpoolsize (64*1024);
	::_set_newnonptrmax (64*1024);


	// For CW10 this appears to be what you must do, cuz of bugs in STL where it
	// resets the new_hander to NULL, and never resets it!
	#if		1
		__throws_bad_alloc = 1;
	#else
		// For whatever reason - MWERKS op new does't do throw - by default. So we'll help them along...
		// Probably not needed for CW10 or later - but should do no harm in any case...
		::set_new_handler (Led_ThrowOutOfMemoryException);
	#endif

	/*
	 *	Initialize Memory Manager Parameter is number of Master Pointer blocks to allocate
	 */
	::InitializeHeap (3);


	// Initialize standard Toolbox managers
	UQDGlobals::InitializeToolbox (&qd);	

#if		qProfile && defined (__MWERKS__)
	OSErr	proErr	=	::ProfilerInit (collectDetailed, bestTimeBase, 10000, 1000);
#endif

	LedItApplication	theApp;		// Create instance of your Application
	theApp.Run ();					//   class and run it

#if		qProfile && defined (__MWERKS__)
	::ProfilerDump ("\pLedProfile.prof");
	::ProfilerTerm ();
#endif

	#if		DEBUG_NEW > 0 && DEBUG_NEW >= DEBUG_NEW_LEAKS
		::DebugNewValidateAllBlocks ();
	#endif
}




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

