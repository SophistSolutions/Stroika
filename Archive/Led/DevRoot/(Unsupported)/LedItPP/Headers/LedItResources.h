/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItResources__
#define	__LedItResources__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItPP/Headers/LedItResources.h,v 2.22 2000/10/19 15:51:34 lewis Exp $
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
 *	$Log: LedItResources.h,v $
 *	Revision 2.22  2000/10/19 15:51:34  lewis
 *	use new portable About code and Find code. Further cleanups/improvements.
 *	
 *	Revision 2.21  2000/10/16 19:34:42  lewis
 *	prelim support for Led_StdDialogHelper_AboutBox
 *	
 *	Revision 2.20  2000/06/15 20:08:16  lewis
 *	preliminary work on UI for paragraph spacing - SPR#0785
 *	
 *	Revision 2.19  2000/06/14 13:59:50  lewis
 *	get (hopefully) compiling with new PickNewParagraphLineSpacing() cmd enabled- SPR#0785
 *	
 *	Revision 2.18  1999/12/27 23:25:37  lewis
 *	Added support for options menu (ugly UI - but this is mostly a demo of Led features) - so can show/hide invisible characters etc
 *	
 *	Revision 2.17  1999/12/27 17:32:44  lewis
 *	SPR#0669 - work on this, but not done. Just added most of the support so this command is autohandled by WordProcessorCommonCommandHelper<> template. And adjusted the characters output (at least for UNICODE).
 *	
 *	Revision 2.16  1997/12/24 04:41:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.15  1997/09/29  17:56:14  lewis
 *	Add justification support.
 *
 *	Revision 2.14  1997/07/27  16:01:58  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.13  1997/06/18  03:29:47  lewis
 *	sub/superscript command#s
 *
 *	Revision 2.12  1997/03/04  20:35:56  lewis
 *	*** empty log message ***
 *
 *	Revision 2.11  1997/01/20  05:42:16  lewis
 *	Added color command ids
 *
 *	Revision 2.10  1996/12/13  18:11:04  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.9  1996/12/05  21:23:57  lewis
 *	*** empty log message ***
 *
 *	Revision 2.8  1996/09/30  15:07:19  lewis
 *	Add res ids for windows list, and window commands
 *
 *	Revision 2.7  1996/09/01  15:45:23  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.6  1996/07/03  01:37:22  lewis
 *	Add defines for menu ids, text traits, etc. And cleanup some other defines.
 *
 *	Revision 2.5  1996/06/01  02:53:42  lewis
 *	misc cleansps and added items to about box
 *
 *	Revision 2.4  1996/05/23  20:43:20  lewis
 *	New commands for font size commands and new other font size dialog
 *
 *	Revision 2.3  1996/05/05  14:54:27  lewis
 *	added new alerts for excpetions, and kUnsupportedDIBFormatPictID
 *
 *	Revision 2.2  1996/03/16  19:16:45  lewis
 *	Added cmdEnterFindString
 *
 *	Revision 2.1  1996/03/04  08:19:53  lewis
 *	Added res ids for kURLPictID/kUnknownEmbeddingPictID.
 *
 *	Revision 2.0  1996/02/26  23:29:06  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	"LedItConfig.h"


#define	kAppleMenuID		128		//	PowerPlant assumes this ID - cannot change - LGP 960614
#define	kFileMenuID			2
#define	kEditMenuID			3
#define	kFormatMenuID		4
#define	kOptionsMenuID		5
#define	kWindowsMenuID		6


#define	cmd_StyleMenu			252
#define	cmd_ColorMenu			253
#define	cmd_JustificationMenu	254





// Command Numbers
#define	cmdFind				4001
#define	cmdFindAgain		4002
#define	cmdEnterFindString	4003
#define	cmdRedo				4004

#define	cmdSaveACopyAs		4101

#define	kBaseFontNameCmdID				20100
#define	kLastFontNameCmdID				20299

#define	kBaseFontSizeCmdID				20300
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

#define	kSubScriptCommand				(kLastFontSizeCmdID + 1)
#define	kSuperScriptCommand				(kLastFontSizeCmdID + 2)

#define	kBaseFontColorCmdID				20400
#define	kFontColorBlackCmdID			(kBaseFontColorCmdID+1)
#define	kFontColorRedCmdID				(kBaseFontColorCmdID+2)
#define	kFontColorGreenCmdID			(kBaseFontColorCmdID+3)
#define	kFontColorBlueCmdID				(kBaseFontColorCmdID+4)
#define	kFontColorCyanCmdID				(kBaseFontColorCmdID+5)
#define	kFontColorMagentaCmdID			(kBaseFontColorCmdID+6)
#define	kFontColorYellowCmdID			(kBaseFontColorCmdID+7)
#define	kFontColorOtherCmdID			(kBaseFontColorCmdID+8)
#define	kLastFontColorCmdID				20499

#define	kJustifyLeftCmdID				20501
#define	kJustifyCenterCmdID				20502
#define	kJustifyRightCmdID				20503
#define	kJustifyFullCmdID				20504

#define	kParagraphSpacingCmdID			kJustifyFullCmdID + 1

#define	kBaseWindowCmdID				20600
#define	kLastWindowCmdID				20699





#define	kFirstShowHideGlyphCmdID		21001
#define	kLastShowHideGlyphCmdID			21003
#define	kShowHideParagraphGlyphsCmdID	21001
#define	kShowHideTabGlyphsCmdID			21002
#define	kShowHideSpaceGlyphsCmdID		21003




// Text Traits
#define	kTextOrForAboutBoxTextTrait			1001
#define	kTextEntryFieldTextTrait			1002
#define	kStaticTextTrait					1003
#define	kButtonTextTrait					1004
#define	kCheckBoxAndRBTextTrait				1005





// Dialogs/Alerts
//#define	kFind_DialogID	1002

#define	kOpenDLOGAdditionItems_DialogID	1003

#define	kSaveDLOGAdditionItems_DialogID	1004

#define	kOtherFontSize_DialogID			1005

#define	kParagraphSpacing_DialogID		1006



#define	kMemoryExceptionAlertID			2001
#define	kPowerPlantExceptionAlertID		2002
#define	kGenericMacOSExceptionAlertID	2003
#define	kUnknownExceptionAlertID		2004
#define	kWarnLowLocalRAMAlertID			2005
#define	kWarnLowRAMAlertID				2006
#define	kCannotOpenHelpFileAlertID		2007
#define	kCannotOpenWebPageAlertID		2008





// Finder BNDL/ICON/etc icon IDs
#define	kPrivateDocument_FinderIconResID			5000
#define	kPrivateDocumentStationary_FinderIconResID	5001
#define	kTEXTDocument_FinderIconResID				5002
#define	kTEXTDocumentStationary_FinderIconResID		5003
#define	kApplicationFinderIconResID					5004
#define	kAnyTypeFinderIconResID						5005


#define	STRx_Untitled	300



#define	kURLPictID					5001
#define	kUnknownEmbeddingPictID		5002
#define	kUnsupportedDIBFormatPictID	5003



#define	WIND_TextDoc		200
#define	prto_TextDoc		201


#endif	/*__LedItResources__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

