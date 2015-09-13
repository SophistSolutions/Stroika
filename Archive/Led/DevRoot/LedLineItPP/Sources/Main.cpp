/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItPP/Sources/Main.cpp,v 2.9 2003/03/11 19:37:50 lewis Exp $
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
 *	Revision 2.9  2003/03/11 19:37:50  lewis
 *	SPR#1249 - get compiling with UNICODE/Carbon
 *	
 *	Revision 2.8  2002/05/06 21:31:30  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.7  2001/11/27 00:28:29  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.6  2001/09/19 15:58:16  lewis
 *	SPR#1030- applied tmpmem / malloc / debug new fixes I had in LedIt! to LedLineIt (still must test)- MACOS
 *	
 *	Revision 2.5  2001/08/30 01:01:09  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  2001/07/20 00:17:19  lewis
 *	SPR#0960- CW6Pro support (compiling target mac on PC)
 *	
 *	Revision 2.3  1999/07/13 20:27:54  lewis
 *	now must turn on NEWMODE = NEWMODE_NORMAL to get tmp-mem stuff working with CW5Pro, and must set
 *	flag gDebugNewFlags so tmpmem works with debugchecking
 *	
 *	Revision 2.2  1997/12/24 04:52:55  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.1  1997/07/27  16:03:37  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.0  1997/01/10  03:51:52  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 */
#include	<new.h>

#include	<UMemoryMgr.h>			// for InitializeHeap
#include	<UDrawingState.h>		// for class UQDGlobals

#include	"LedLineItApplication.h"





/*
 *	Profiling/Mac memory Management.
 */
#if		qMacOS

	#if		__profile__
		#define	qProfile		1
	#endif

	#ifndef	qProfile
		#define	qProfile		0
	#endif

	#if		qProfile && defined (__MWERKS__)
		#include	"profiler.h"
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
				::HLock (h);
				return *h;
			}
		inline	void	DoSysFree (void* p)
			{
				Handle	h	=	::RecoverHandle (reinterpret_cast<Ptr> (p));
				::HUnlock (h);
				::DisposeHandle (h);
			}
		inline	size_t	DoGetPtrSize (void* p)
			{
				Handle	h	=	::RecoverHandle (reinterpret_cast<Ptr> (p));
				return ::GetHandleSize (h);
			}
	#endif

	#if		qUseMacTmpMemForAllocs
		/*
		 *	In CW5Pro and earlier - I hooked into the Metrowerks memory allocation system via #including "New.cpp" and
		 *	doing #defines of NewPtr and DisposePtr () etc. But that no longer works with CWPro7 (I'm not sure about
		 *	CWPro6). As of CWPro7 - there are several malloc implementations. This below code seems to work for all of
		 *	them (from purusing the code in alloc.c, pool_alloc.c, pool_alloc.h, pool_alloc.mac.c). And empiricly it
		 *	works fine for the default allocation algorithm -- LGP 2001-09-19
		 */
		#include	"pool_alloc.h"
		extern	"C" {
			void*	__sys_alloc (std::size_t n)
				{
					return DoSysAlloc (n);
				}
			void	__sys_free (void* p)
				{
					DoSysFree (p);
				}
			std::size_t	__sys_pointer_size (void* p)
				{
					return ::DoGetPtrSize (p);
				}
		}
	#endif

	#if		DEBUG_NEW
	#define	NEWMODE	NEWMODE_MALLOC
	#include	"DebugNew.cp"
	#endif

#endif







void	main ()
{
	#if		qDebug
		// Set Debugging options
		SetDebugThrow_(debugAction_Alert);
		SetDebugSignal_(debugAction_Alert);
	#endif

	#if		!TARGET_CARBON
		const	long	kMinStack	=	32l*1024l;	// reserve  32K for stack
		::SetApplLimit ((Ptr) ((long) (::GetApplLimit ()) - kMinStack));
	#endif

	/*
	 *	Initialize Memory Manager Parameter is number of Master Pointer blocks to allocate
	 */
	::InitializeHeap (3);


	// Initialize standard Toolbox managers
	UQDGlobals::InitializeToolbox ();	

	#if		qProfile && defined (__MWERKS__)
		OSErr	proErr	=	::ProfilerInit (collectDetailed, bestTimeBase, 10000, 1000);
	#endif

	LedLineItApplication	theApp;		// Create instance of your Application
	theApp.Run ();					//   class and run it

	#if		qProfile && defined (__MWERKS__)
		::ProfilerDump ("\pLedProfile.prof");
		::ProfilerTerm ();
	#endif

	#if		DEBUG_NEW > 0 && DEBUG_NEW >= DEBUG_NEW_LEAKS
		// Hmm. produces link error doing this on CW6Pro? But seems to work OK without... LGP 2001-07-17
		::DebugNewValidateAllBlocks ();
	#endif
}




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

