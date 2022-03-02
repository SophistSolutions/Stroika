/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/OSConfiguration.cc,v 1.2 1992/09/01 15:26:25 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: OSConfiguration.cc,v $
 *		Revision 1.2  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.5  1992/02/08  04:50:00  lewis
 *		Moved some graphix stuff to GDIConfiguration().
 *
 *
 */



#if		qMacOS
#include	"OSRenamePre.hh"
#include	<GestaltEqu.h>
#include	<Memory.h>
#include	<OSUtils.h>
#include	<ToolUtils.h>
#include	<Traps.h>

/// GROSS TEMP HACK  - FOR NEW MPW - missing some procedures...
#include <Strings.h>

#include	"OSRenamePost.hh"
#endif	/*qMacOS*/

#include	"OSConfiguration.hh"


#if		qMacOS
static	SysEnvRec	sSysEnvironsRec;


// So we can safely abort when built for 020, and running on 68000
#pragma	processor	68000


/*
 *	gestalt:
 * 		About new gestalt stuff. MPW 3.2+ glue done in such a way that we dont need to check if
 * 		Getstalt trap is present. It will just fail and return false if not.
 */



#if		qMPW_CFront
// If apple implements this call as a macro, we must adjust it cuz we changed type type used in it...
#ifdef	GetApplLimit
#undef	GetApplLimit
#define GetApplLimit() (* (void**) 0x0130)
#endif
#endif	/*qMPW_CFront*/

#endif	/*qMacOS*/


/*
 ********************************************************************************
 ****************************** OSConfiguration *********************************
 ********************************************************************************
 */

OSConfiguration::OSConfiguration ()
{
	static	Boolean	inited	=	False;
	if (not inited) {
		inited = True;
#if		qMacOS
		(void)::SysEnvirons (curSysEnvVers, &sSysEnvironsRec);
#endif	/*qMacOS*/
	}
}

size_t	OSConfiguration::TotalStackSpace () const
{
//	AssertNotReached ();
	return (StackAvailable ());		// not quite right.
}

size_t	OSConfiguration::StackAvailable () const
{
#if		qMacOS
#if		qDebug
	/*
	 * in MPW 3.0b1,MPW 3.2b8 ::StackSpace () was buggy on macplus,
	 * declared as returning long, as it must, but IM said
	 * returns short??? not too sure works OK now???
	 */
	const	size_t	kAllowableDifference	=	3*1024;		// dont know why but seem to be
															// typically off by 2.5K.
															// Should look at mem mgr docs again.
															// Also, this was under sys 7/MacIIx, not
															// a normal system.
	auto	char	stackProbe;
			size_t	stackAvail		=	size_t (&stackProbe) - size_t (GetApplLimit ());
			size_t	allegedAvail	=	size_t (::StackSpace ());
	if (stackAvail > allegedAvail) {
		if (stackAvail - allegedAvail > kAllowableDifference) {
			DebugStr ("\pBad Stack Calc1");
		}
	}
	else if (stackAvail < allegedAvail) {
		if (allegedAvail- stackAvail > kAllowableDifference) {
			DebugStr ("\pBad Stack Calc2");
		}
	}
#endif	/*qDebug*/
	return (::StackSpace ());
#elif	qUnixOS
	return (1024*1024);		// we always have lots of stackspace available in UNIX - no tellin' ha much tho?
#endif	/*OS*/
}

size_t	OSConfiguration::TotalHeapSpace () const
{
#if		qMacOS
//	AssertNotReached ();		// minus stack and global data, etc...
#endif	/*qMacOS*/
	return (0);
}

size_t	OSConfiguration::HeapAvailable () const
{
#if		qMacOS
//	AssertNotReached ();		// mac heap calls may not count mallocable memory
								// does not count MF memory available, cuz we have no
								// interface to keep track...
#endif	/*qMacOS*/
	return (0);
}

#if		qMacOS

Boolean	OSConfiguration::Has68020 () const
{
	return Boolean (sSysEnvironsRec.processor == env68020);
}

Boolean	OSConfiguration::Has68020Plus () const
{
	return Boolean (sSysEnvironsRec.processor >= env68020);
}

Boolean	OSConfiguration::HasFPU () const
{
	return Boolean (sSysEnvironsRec.hasFPU);
}

Boolean	OSConfiguration::TrapAvailable (short theTrap) const
{
	// algorithm from page 3-8 of IM V-6 October 1990 draft
	TrapType	trapType	=	(theTrap & 0x800)? ToolTrap: OSTrap;
	if (trapType == ToolTrap) {
		theTrap &= 0x7ff;
		short	numToolboxTraps	=	(::NGetTrapAddress (_InitGraf, ToolTrap) == ::NGetTrapAddress (0xaa6e, ToolTrap))? 0x200: 0x400;
		if (theTrap >= numToolboxTraps) {
			theTrap = _Unimplemented;
		}
	}
	return Boolean (::NGetTrapAddress (theTrap, trapType) != ::NGetTrapAddress (_Unimplemented, ToolTrap));
}

Boolean	OSConfiguration::WaitNextEventAvailable () const
{
	static	Boolean	answer	=	TrapAvailable (_WaitNextEvent);
	return (answer);
}

Boolean	OSConfiguration::RunningSystemSix () const
{
	return Boolean ((sSysEnvironsRec.systemVersion >= 0x0600) and (sSysEnvironsRec.systemVersion < 0x0700));
}

Boolean	OSConfiguration::SystemDebuggerInstalled () const
{
	// As of Monday, June 10, 1991 11:54:32 PM, this seems to fuck up on system six, so
	// force ourselves to see a debugger in that case!
	if (RunningSystemSix ()) {
		return (True);
	}

	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltOSAttr, &response);
	if (err != noErr) {
		// might be some other way to tell, but assume there is a debugger if gestalt cannot
		// tell.  That way we can still debug on old machines...
		return (True);
	}
	return (Boolean (!!((response & (1 << gestaltSysDebuggerSupport)))));
}

Boolean	OSConfiguration::PopUpControlsPresent () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltPopupAttr, &response);
	if (err != noErr) {
		return (False);			// if no Gestalt, then not available
	}
	return (Boolean (!!((response & (1 << gestaltPopupPresent)))));
}

Boolean	OSConfiguration::HasVirtualMemory () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltVMAttr, &response);
	if (err != noErr) {
		return (False);			// if no Gestalt, then not available
	}
	return (Boolean (!!((response & (1 << gestaltVMPresent)))));
}

Boolean	OSConfiguration::HasSpecialFolders () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltFindFolderAttr, &response);
	if (err != noErr) {
		return (False);			// if no Gestalt, then not available
	}
	return (Boolean (!!((response & (1 << gestaltFindFolderPresent)))));
}

Boolean	OSConfiguration::HasAliasManager () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltAliasMgrAttr, &response);
	if (err != noErr) {
		return (False);			// if no Gestalt, then not available
	}
	return (Boolean (!!((response & (1 << gestaltAliasMgrPresent)))));
}

Boolean	OSConfiguration::HasAppleEvents () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltAppleEventsAttr, &response);
	if (err != noErr) {
		return (False);			// if no Gestalt, then not available
	}
	return (Boolean (!!((response & (1 << gestaltAppleEventsPresent)))));
}

Boolean	OSConfiguration::HasHelpManager () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltHelpMgrAttr, &response);
	if (err != noErr) {
		return (False);			// if no Gestalt, then not available
	}
	return (Boolean (!!((response & (1 << gestaltHelpMgrPresent)))));
}

Boolean	OSConfiguration::MachineSupports32BitMode () const
{
	return (TrapAvailable (_SwapMMUMode));
}

Boolean	OSConfiguration::ToolboxSupports32BitMode () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltAddressingModeAttr, &response);
	if (err != noErr) {
		return (False);			// if no Gestalt, then not available
	}
	return (Boolean (!!((response & (1 << gestalt32BitCapable)))));
}

Boolean	OSConfiguration::Running32BitMode () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltAddressingModeAttr, &response);
	if (err != noErr) {
		return (False);			// if no Gestalt, then not available
	}
	return (Boolean (!!((response & (1 << gestalt32BitAddressing)))));
}

Boolean	OSConfiguration::BootedIn32BitMode () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltAddressingModeAttr, &response);
	if (err != noErr) {
		return (False);			// if no Gestalt, then not available
	}
	return (Boolean (!!((response & (1 << gestalt32BitAddressing)))));
}

Boolean	OSConfiguration::TimeManagerAvailable () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltTimeMgrVersion, &response);
	if (err != noErr) {
		// might be some other way to tell, but assume the worst, if Gestalt cannot tell 
		return (False);
	}
	return (Boolean (response >= gestaltStandardTimeMgr));
}

Boolean	OSConfiguration::RevisedTimeManagerAvailable () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltTimeMgrVersion, &response);
	if (err != noErr) {
		// might be some other way to tell, but assume the worst, if Gestalt cannot tell 
		return (False);
	}
	return (Boolean (response >= gestaltRevisedTimeMgr));
}

Boolean	OSConfiguration::ExtendedTimeManagerAvailable () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltTimeMgrVersion, &response);
	if (err != noErr) {
		return (False);			// if no Gestalt, then not available
	}
	return (Boolean (response >= gestaltExtendedTimeMgr));
}

Boolean	OSConfiguration::HasSlotManager () const
{
	static	Boolean	answer	=	TrapAvailable (_SlotManager);
	return (answer);
}

Boolean	OSConfiguration::HasSlot (UInt8 slotNumber) const
{
	if (HasSlotManager ()) {
		long	response	=	0;
		OSErr	err			=	::Gestalt (gestaltNuBusConnectors, &response);
		if (err != noErr) {
			return (True);			// if no Gestalt, but have SlotMgr, not sure what to say - assume
									// there are slots there.
		}
		return (Boolean (!!((response & (1 << slotNumber)))));
	}
	else {
		return (False);			// if not slot manager, no slots
	}
}

Boolean	OSConfiguration::HasQuickTime () const
{
	return (False);
}







/// GROSS TEMP HACK  - FOR NEW MPW - missing some procedures...
osStringPtr c2pstr(char *aStr){return C2PStr(aStr);}
char *p2cstr(osStringPtr aStr){return P2CStr(aStr);}



#endif	/*qMacOS*/




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


