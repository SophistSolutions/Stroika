/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItResourceIDs_h__
#define	__LedLineItResourceIDs_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Headers/Resource.h,v 2.25 2004/02/26 03:43:06 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Resource.h,v $
 *	Revision 2.25  2004/02/26 03:43:06  lewis
 *	added qSupportGenRandomCombosCommand hack my dad can more easily generate a bunch of potential namess
 *	
 *	Revision 2.24  2004/02/11 22:41:38  lewis
 *	SPR#1576: update: added 'Replace Again' command to LedIt, LedLineItMFC, ActiveLedIt
 *	
 *	Revision 2.23  2004/01/27 20:36:11  lewis
 *	SPR#1625: added a dialog and code to catch the case of reading a file with long lines, and offer to break it at (user specifyable  in the dialog) character count breakpoints.
 *	
 *	Revision 2.22  2003/06/03 12:31:12  lewis
 *	SPR#1513: hook in spellcheck engine
 *	
 *	Revision 2.21  2003/05/30 03:12:32  lewis
 *	SPR#1517: preliminary version of REPLACE DIALOG support (WIN32 only so far).
 *	
 *	Revision 2.20  2003/04/03 22:49:41  lewis
 *	SPR#1407: more work on getting this covnersion to new command handling working (mostly MacOS/Linux now)
 *	
 *	Revision 2.19  2003/03/11 02:32:00  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand
 *	
 *	Revision 2.18  2003/01/23 17:04:38  lewis
 *	SPR#1256 - use new Led_StdDialogHelper_OtherFontSizeDialog
 *	
 *	Revision 2.17  2002/05/06 21:31:16  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.16  2001/11/27 00:28:21  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.15  2001/09/17 14:43:06  lewis
 *	SPR#1032- added SetDefaultFont dialog and saved preferences (pref for new docs)
 *	
 *	Revision 2.14  2001/09/12 00:22:32  lewis
 *	SPR#1023- added LedItApplication::HandleBadUserInputException () code and other related fixes
 *	
 *	Revision 2.13  2001/09/09 22:36:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.12  2001/09/09 22:31:56  lewis
 *	SPR#0994, 0697- Added fCodePage flag to the LedLineItDocument class, and did alot
 *	of work reworking the calling of the CFileDialog so it could use my new replacement
 *	version (which includes an Encoding popup). Pretty ugly code for passing args through
 *	the MFC layers (like codepage from dialog to Serialize method). Most of the REAL guts
 *	code is in the CodePage module, and the stuff here is very MFC-specific.
 *	
 *	Revision 2.11  2001/08/30 01:02:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.10  2000/10/18 21:00:37  lewis
 *	Lose FindDialog code - and inherit new sahred implm in Led_StdDialogHelper_FindDialog etc
 *	
 *	Revision 2.9  2000/10/16 22:50:08  lewis
 *	use new Led_StdDialogHelper_AboutBox
 *	
 *	Revision 2.8  2000/06/08 21:49:49  lewis
 *	SPR#0774- added option to treat TAB as indent-selection command - as MSDEV does
 *	
 *	Revision 2.7  2000/06/08 20:32:15  lewis
 *	SPR#0771- close and closeall menu items in windows menu
 *	
 *	Revision 2.6  1999/12/25 04:15:51  lewis
 *	Add MENU COMMANDS for the qSupportSyntaxColoring option. And debug it so it worked properly
 *	turning on/off and having multiple windows onto the same doc at the same time (scrolled
 *	to same or differnet) places
 *	
 *	Revision 2.5  1997/12/24 04:50:02  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  1997/07/27  16:02:50  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.3  1997/06/18  03:45:44  lewis
 *	 Support shiftleft and shiftright
 *
 *	Revision 2.2  1997/03/23  01:06:40  lewis
 *	kBadAllocExceptionOnCmdDialogID
 *
 *	Revision 2.1  1997/03/04  20:17:59  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/01/10  03:35:47  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 *
 */

#include	<afxres.h>

#include	"LedLineItConfig.h"





// MFC must define something like this someplace, but I haven't found where....
// Use this for now, so I can update things more easily when I find the MFC definition...
#define	kUserCommandBase			0x1000


#define IDR_MAINFRAME				128

#define IDR_SRVR_INPLACE			4
#define IDR_SRVR_EMBEDDED			5
#define IDR_CNTR_INPLACE			6
#define IDP_OLE_INIT_FAILED			100
#define IDP_FAILED_TO_CREATE		102
#define ID_CANCEL_EDIT_CNTR			32768
#define ID_CANCEL_EDIT_SRVR			32769


#define	ID_LINENUMBER		999







// Menus
#define	kContextMenu		32312



#define kUnknownExceptionOnCmdDialogID	1003


#define	kGotoLine_DialogID				1006
	#define	kGotoLine_Dialog_LineNumberEditFieldID	2303


#define kBadAllocExceptionOnCmdDialogID	1007


#define kBadUserInputExceptionOnCmdDialogID	1008

#define	kLineTooLongOnRead_DialogID				1009
	#define	kLineTooLongOnRead_Dialog_MessageFieldID	2303
	#define	kLineTooLongOnRead_Dialog_BreakNumFieldID	2304

#define	kFileDialogAddOnID				2001
	#define	kFileDialog_EncodingComboBox		1001




// What is the right set of valid number ranges for my commands/??

#define	kFileCmdBase				(kUserCommandBase+0x100)
#define	kEditCmdBase				(kUserCommandBase+0x200)
#define	kFormatCmdBase				(kUserCommandBase+0x300)


#define	kFindCmd					ID_EDIT_FIND
#define	kFindAgainCmd				(kEditCmdBase+1)
#define	kEnterFindStringCmd			(kEditCmdBase+2)
#define	kReplaceCmd					ID_EDIT_REPLACE
#define	kReplaceAgainCmd			(kEditCmdBase+3)
#define	kGotoLineCmdID				(kEditCmdBase+4)
#define	kShiftLeftCmdID				(kEditCmdBase+5)
#define	kShiftRightCmdID			(kEditCmdBase+6)
#ifndef	kSpellCheckCmd
#define	kSpellCheckCmd				(kEditCmdBase+7)
#endif


#define	cmdChooseFontDialog			(kFormatCmdBase+1)
#define	cmdChooseDefaultFontDialog	(kFormatCmdBase+2)

#define	cmdFontMenuFirst			(kFormatCmdBase+0x100)
#define	cmdFontMenuLast				(cmdFontMenuFirst+0x100)

#define	kBaseFontSizeCmdID				cmdFontMenuLast+1
#define	kFontSize9CmdID					kBaseFontSizeCmdID
#define	kFontSize10CmdID				(kFontSize9CmdID + 1)
#define	kFontSize12CmdID				(kFontSize9CmdID + 2)
#define	kFontSize14CmdID				(kFontSize9CmdID + 3)
#define	kFontSize18CmdID				(kFontSize9CmdID + 4)
#define	kFontSize24CmdID				(kFontSize9CmdID + 5)
#define	kFontSize36CmdID				(kFontSize9CmdID + 6)
#define	kFontSize48CmdID				(kFontSize9CmdID + 7)
#define	kFontSize72CmdID				(kFontSize9CmdID + 8)
#define	kFontSizeOtherCmdID				(kFontSize9CmdID + 9)
#define	kFontSizeSmallerCmdID			(kFontSize9CmdID + 10)
#define	kFontSizeLargerCmdID			(kFontSize9CmdID + 11)
#define	kLastFontSizeCmdID				kFontSizeLargerCmdID


#define	cmdFontStylePlain			(kLastFontSizeCmdID+1)
#define	cmdFontStyleBold			(kLastFontSizeCmdID+2)
#define	cmdFontStyleItalic			(kLastFontSizeCmdID+3)
#define	cmdFontStyleUnderline		(kLastFontSizeCmdID+4)
#define	cmdFontStyleStrikeout		(kLastFontSizeCmdID+5)


// Options - soon to be replaced with an prefs dialog...
#define	kToggleUseSmartCutNPasteCmdID			kUserCommandBase+0x1002
#define	kToggleAutoIndentOptionCmd				kUserCommandBase+0x2000
#define	kToggleTreatTabAsIndentCharOptionCmd	kUserCommandBase+0x2101
#define	kNoSyntaxColoringCmd					kUserCommandBase+0x2102
#define	kCPlusPlusSyntaxColoringCmd				kUserCommandBase+0x2103
#define	kVBSyntaxColoringCmd					kUserCommandBase+0x2104

#define	kGotoLedLineItWebPageCmdID		kUserCommandBase+0x3001
#define	kCheckForUpdatesWebPageCmdID	kUserCommandBase+0x3002
#define	kGotoSophistsWebPageCmdID		kUserCommandBase+0x3003


#define	kCloseWindowCmdID				kUserCommandBase+0x4001
#define	kCloseAllWindowsCmdID			kUserCommandBase+0x4002

#if		qSupportGenRandomCombosCommand
	#define	kGenRandomCombosCmdID			kUserCommandBase+0x4003
#endif


#endif	/*__LedLineItResourceIDs_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

