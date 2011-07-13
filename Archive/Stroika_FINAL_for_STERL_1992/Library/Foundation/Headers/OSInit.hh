/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__OSInit__
#define	__OSInit__

/*
 * $Header: /fuji/lewis/RCS/OSInit.hh,v 1.4 1992/12/08 21:13:15 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *		-		Get GDI/GUI/Toolkit stuff out of here, and move it up a level or two - 
 *				make a GDIInit, and ToolkitInit file/class in each layer.
 *
 * Notes:
 *
 * Changes:
 *	$Log: OSInit.hh,v $
 *		Revision 1.4  1992/12/08  21:13:15  lewis
 *		Add qWinNT to case.
 *
 *		Revision 1.3  1992/11/25  03:01:18  lewis
 *		Get rid of enum IsATool - use Booleans (named constants) instead.
 *		Default is now ISATOOL, since applications will usually use GDIInit(), or
 *		GUIInit() which will subclass and change arg.
 *		Add DTOR, and trap patching stuff for mac in here, instead of separate class
 *		so we can shutoff for Tools and get more reliable calling sequence.
 *
 *		Revision 1.11  1992/05/09  00:50:11  lewis
 *		Added qDOS / qWinOS support.
 *
 *		Revision 1.9  1992/01/27  03:58:03  sterling
 *		made compile on Mac
 *
 *		Revision 1.7  1992/01/22  02:24:09  lewis
 *		Start moving GDI/GUI stuff out.
 *		Move to GUIInit/GDIInit.
 *
 */

#include	"Config-Foundation.hh"


class	OSInit {
	public:
#if		qMacOS
		static	const	Boolean	kRequire68020;
		static	const	Boolean	kNotRequire68020;
		static	const	Boolean	kDefaultRequire68020;

		static	const	Boolean	kRequireFPU;
		static	const	Boolean	kNotRequireFPU;
		static	const	Boolean	kDefaultRequireFPU;

		static	const	Boolean	kIsATool;
		static	const	Boolean	kIsNotATool;
#endif

		enum {	kMinHeap	= 100 * 1024	};		/* in addition to code/bss/data/stack */
		enum {	kMinStack	=  64 * 1024	};
#if		qMacOS
		OSInit (
				Boolean isATool = kIsATool,			// TOOL - by default, since apps usually
													// use subclass GDIInit () or GUIInit().
				size_t minHeap = kMinHeap,
				size_t minStack = kMinStack,
				Boolean require68020Plus = kDefaultRequire68020,
				Boolean requireFPU = kDefaultRequireFPU
			);
#elif	qDOS || qWinOS || qWinNT || qUnixOS
		OSInit (size_t minHeap = kMinHeap, size_t minStack = kMinStack);
#endif	/*OS*/

		~OSInit ();

	protected:
		nonvirtual	void	AssureThat (Boolean conditionTrue, const char* conditionName);

#if		qMacOS
	private:
		class	OSInit_TailPatchSysHeapBlock;
		static	OSInit_TailPatchSysHeapBlock*	sExitToShellPatch;
		static	pascal	void					CleanUpProc ();
#endif	/*qMacOS*/
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__OSInit__*/

