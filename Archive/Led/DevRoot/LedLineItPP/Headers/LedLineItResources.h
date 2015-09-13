/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItResources__
#define	__LedLineItResources__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItPP/Headers/LedLineItResources.h,v 2.15 2004/02/12 20:58:01 lewis Exp $
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
 *	$Log: LedLineItResources.h,v $
 *	Revision 2.15  2004/02/12 20:58:01  lewis
 *	SPR#1576: replaceall in selection supprot for MacOS LedLineIt
 *	
 *	Revision 2.14  2003/06/04 03:08:26  lewis
 *	SPR#1513: spellcheck engine support
 *	
 *	Revision 2.13  2003/05/30 14:43:31  lewis
 *	SPR#1517: MacOS replace dialog support
 *	
 *	Revision 2.12  2003/04/03 22:50:02  lewis
 *	SPR#1407: more work on getting this covnersion to new command handling working (mostly MacOS/Linux now)
 *	
 *	Revision 2.11  2003/03/11 21:45:06  lewis
 *	SPR#1287 - use Gestalt to see if Aqua UI is present and remove QUIT menu if it is. Also -
 *	if HMGetHelpMenuHandle() == NULL (as it is in OSX) - then append a Help menu and store our
 *	help menu items in that
 *	
 *	Revision 2.10  2003/01/23 17:09:59  lewis
 *	SPR#1256 - use new Led_StdDialogHelper_OtherFontSizeDialog
 *	
 *	Revision 2.9  2002/05/06 21:31:27  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.8  2001/11/27 00:28:27  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.7  2001/09/12 00:22:17  lewis
 *	SPR#1023- added LedItApplication::HandleBadUserInputException () code and other related fixes
 *	
 *	Revision 2.6  2001/08/30 01:01:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.5  2000/10/16 19:34:28  lewis
 *	prelim support for Led_StdDialogHelper_AboutBox
 *	
 *	Revision 2.4  1997/12/24 04:52:10  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  1997/07/27  16:03:25  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.2  1997/06/18  03:34:40  lewis
 *	shiftleft/right commands
 *
 *	Revision 2.1  1997/03/04  20:27:02  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/01/10  03:51:06  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 */

#include	"LedLineItConfig.h"


#define	kAppleMenuID		128		//	PowerPlant assumes this ID - cannot change - LGP 960614
#define	kFileMenuID			2
#define	kEditMenuID			3
#define	kOptionsMenuID		4
#define	kWindowsMenuID		5
#define	kHelpMenuID			6


// Command Numbers
#define	cmdFind							4001
#define	cmdFindAgain					4002
#define	cmdEnterFindString				4003
#define	kReplaceCmd						4004
#define	kReplaceAgain					4005
#define	cmdRedo							4006
#define	kGotoLineCmdID					4007
#ifndef	kSpellCheckCmd
#define	kSpellCheckCmd					4008
#endif

#define	kShiftLeftCmdID					4010
#define	kShiftRightCmdID				4011

#define	cmdSaveACopyAs					4101

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

#define	kBaseWindowCmdID				20400
#define	kLastWindowCmdID				20499






// Text Traits
#define	kTextOrForAboutBoxTextTrait			1001
#define	kTextEntryFieldTextTrait			1002
#define	kStaticTextTrait					1003
#define	kButtonTextTrait					1004
#define	kCheckBoxAndRBTextTrait				1005





// Dialogs/Alerts
#define	kFind_DialogID	1002

#define	kOpenDLOGAdditionItems_DialogID	1003

#define	kSaveDLOGAdditionItems_DialogID	1004

#define	kGotoLine_DialogID	1006




#define	kMemoryExceptionAlertID			2001
#define	kPowerPlantExceptionAlertID		2002
#define	kGenericMacOSExceptionAlertID	2003
#define	kUnknownExceptionAlertID		2004
#define	kWarnLowLocalRAMAlertID			2005
#define	kWarnLowRAMAlertID				2006
#define	kCannotOpenHelpFileAlertID		2007
#define	kCannotOpenWebPageAlertID		2008
#define	kBadUserInputExceptionAlertID	2009





// Finder BNDL/ICON/etc icon IDs
#define	kPrivateDocument_FinderIconResID			5000
#define	kPrivateDocumentStationary_FinderIconResID	5001
#define	kTEXTDocument_FinderIconResID				5002
#define	kTEXTDocumentStationary_FinderIconResID		5003
#define	kApplicationFinderIconResID					5004
#define	kAnyTypeFinderIconResID						5005


#define	STRx_Untitled	300




#define	WIND_TextDoc		200
#define	prto_TextDoc		201


#endif	/*__LedLineItResources__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

