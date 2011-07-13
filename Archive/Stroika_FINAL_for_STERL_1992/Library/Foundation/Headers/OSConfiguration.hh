/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__OSConfiguration__
#define	__OSConfiguration__

/*
 * $Header: /fuji/lewis/RCS/OSConfiguration.hh,v 1.2 1992/09/01 15:20:18 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *		(1)		Add support to check for various windowmanagers, and maybe also X x-tensions..???
 *
 * Notes:
 *
 * Changes:
 *	$Log: OSConfiguration.hh,v $
 *		Revision 1.2  1992/09/01  15:20:18  sterling
 *		*** empty log message ***
 *
 *		Revision 1.5  1992/02/08  04:49:43  lewis
 *		Moved some graphix stuff to GDIConfiguration().
 *
 *
 */

#include	"Config-Foundation.hh"


class	OSConfiguration {
	public:
		OSConfiguration ();

		nonvirtual	size_t	TotalStackSpace () const;
		nonvirtual	size_t	StackAvailable () const;

		nonvirtual	size_t	TotalHeapSpace () const;
		nonvirtual	size_t	HeapAvailable () const;		// may not really be available, you have
														// to try alloc and see.
#if		qMacOS
		/*
		 * Hardware questions.
		 */
		nonvirtual	Boolean	Has68020 () const;
		nonvirtual	Boolean	Has68020Plus () const;
		nonvirtual	Boolean	HasFPU () const;


		/*
		 * Check for presense of the given trap.
		 */
		nonvirtual	Boolean	TrapAvailable (short theTrap) const;


		/*
		 * Multifinder
		 */
		nonvirtual	Boolean	WaitNextEventAvailable () const;

		nonvirtual	Boolean	RunningSystemSix () const;				// >= 6.0 < 7.0


		/*
		 * Environmental information - what os features are present?
		 */
		nonvirtual	Boolean	SystemDebuggerInstalled () const;		// can trap into macsbug?
		nonvirtual	Boolean	PopUpControlsPresent () const;			//
		nonvirtual	Boolean	HasVirtualMemory () const;				// currently turned on

		nonvirtual	Boolean	HasSpecialFolders () const;				// e.g. temp folder, prefs folder, etc..

		nonvirtual	Boolean	HasAliasManager () const;				// Symbolic links

		nonvirtual	Boolean	HasAppleEvents () const;				// 

		nonvirtual	Boolean	HasHelpManager () const;				// 

		nonvirtual	Boolean	MachineSupports32BitMode () const;		// Can call SwapMMUMode?
		nonvirtual	Boolean	ToolboxSupports32BitMode () const;		// 32bit Clean ROMS?
		nonvirtual	Boolean	Running32BitMode () const;				// Running in 32bit mode
		nonvirtual	Boolean	BootedIn32BitMode () const;				// Booted in 32bit mode

		nonvirtual	Boolean	TimeManagerAvailable () const;
		nonvirtual	Boolean	RevisedTimeManagerAvailable () const;
		nonvirtual	Boolean	ExtendedTimeManagerAvailable () const;

		nonvirtual	Boolean	HasSlotManager () const;
		nonvirtual	Boolean	HasSlot (UInt8 slotNumber) const;

		nonvirtual	Boolean	HasQuickTime () const;
#endif	/*qMacOS*/
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__OSConfiguration__*/

