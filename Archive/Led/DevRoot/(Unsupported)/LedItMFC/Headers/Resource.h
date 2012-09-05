/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItResourceIDs_h__
#define	__LedItResourceIDs_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/Resource.h,v 2.25 2000/10/18 21:00:19 lewis Exp $
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
 *	Revision 2.25  2000/10/18 21:00:19  lewis
 *	Lose FindDialog code - and inherit new sahred implm in Led_StdDialogHelper_FindDialog etc
 *	
 *	Revision 2.24  2000/10/16 22:49:57  lewis
 *	use new Led_StdDialogHelper_AboutBox
 *	
 *	Revision 2.23  2000/06/13 22:59:06  lewis
 *	cleanups. And SPR#0785- at least a crufty UI for adjusting line spacing params
 *	
 *	Revision 2.22  2000/03/17 22:40:14  lewis
 *	SPR#0717- preliminary support - for showhiddentext testing and UI
 *	
 *	Revision 2.21  1999/12/27 17:32:57  lewis
 *	SPR#0669 - work on this, but not done. Just added most of the support so this command is autohandled by WordProcessorCommonCommandHelper<> template. And adjusted the characters output (at least for UNICODE).
 *	
 *	Revision 2.20  1999/02/08 22:31:34  lewis
 *	Support new optional 'wrap-to-window' feature. Used to be we ALWAYS did that. Now can be on/off. Next todo is to fix the OFF case to properly use ruler
 *	
 *	Revision 2.19  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.18  1997/12/24  04:04:28  lewis
 *	new resources for toolbar, color menu changes, fontbar, ruler etc.
 *
 *	Revision 2.17  1997/09/29  15:59:31  lewis
 *	added justification support
 *
 *	Revision 2.16  1997/07/27  16:01:09  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.15  1997/06/18  03:40:43  lewis
 *	new sub/superscript commands
 *
 *	Revision 2.14  1997/03/23  01:01:01  lewis
 *	bad-alloc exception dialog
 *
 *	Revision 2.13  1997/03/04  20:14:40  lewis
 *	Add a few commands
 *
 *	Revision 2.12  1997/01/20  05:32:29  lewis
 *	Font color support.
 *
 *	Revision 2.11  1996/12/13  18:02:41  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.10  1996/12/05  21:16:12  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1996/09/30  14:55:27  lewis
 *	Now we have a LedItConfig file - so move some stuff there.
 *
 *	Revision 2.8  1996/09/01  15:43:38  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.7  1996/07/03  01:52:19  lewis
 *	Cleanup DITL ids, - wrap in parens to avoid bugs computing differences between DITL ids.
 *
 *	Revision 2.6  1996/06/01  02:33:22  lewis
 *	Lots of changes in cmd numbers, for about box, etc....
 *
 *	Revision 2.5  1996/05/23  20:16:21  lewis
 *	Added new kOtherFontSize_DialogID.
 *	And new/revised font size command names
 *
 *	Revision 2.4  1996/05/05  14:40:13  lewis
 *	new picts kURLPictID, kUnknownEmbeddingPictID, kUnsupportedPICTFormatPictID
 *	for embeddings.
 *
 *	Revision 2.3  1996/03/16  19:08:37  lewis
 *	Added cmdEnterFindString.
 *
 *	Revision 2.2  1996/02/05  04:40:58  lewis
 *	Lose cmdReplace, and fixup findidialog, and add strikeout font cmd,
 *	and lose a few others (that were macs specific).
 *
 *	Revision 2.1  1996/01/22  05:38:58  lewis
 *	Dialog IDs for Find and Replace dialogs
 *
 *	Revision 2.0  1996/01/04  00:55:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.3  1996/01/04  00:55:22  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/12/08  07:46:23  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	<afxres.h>

#include	"LedItConfig.h"





// MFC must define something like this someplace, but I haven't found where....
// Use this for now, so I can update things more easily when I find the MFC definition...
#define	kUserCommandBase			0x5000


#define IDR_MAINFRAME				128
#define kLedItDocumentIconID		129

#define IDR_SRVR_INPLACE			4
#define IDR_SRVR_EMBEDDED			5
#define IDR_CNTR_INPLACE			6
#define IDP_OLE_INIT_FAILED			100
#define IDP_FAILED_TO_CREATE		102
#define ID_CANCEL_EDIT_CNTR			32768
#define ID_CANCEL_EDIT_SRVR			32769


//FormatBar stuff
#define IDC_FONTSIZE				166
#define IDC_FONTNAME				167
#define	IDB_FONTTYPE				168
#define ID_CHAR_COLOR                   32801
#define IDB_FORMATBAR                   139
#define IDS_TITLE_FORMATBAR             1167
#define ID_VIEW_FORMATBAR               59396



//RulerBar stuff
#define IDB_RULER_BLOCK                 131
#define IDB_RULER_BLOCKM                132
#define IDB_RULER_UP                    137
#define IDB_RULER_UPM                   138
#define IDB_RULER_DOWN                  133
#define IDB_RULER_DOWNM                 134
#define IDB_RULER_TAB                   135
#define IDB_RULER_TABM                  136
#define ID_VIEW_RULER                   59397


// Menus
#define	kContextMenu		32312



#define kUnknownExceptionOnCmdDialogID	1003

#define	kOtherFontSize_DialogID			1005
	#define	kOtherFontSize_Dialog_FontSizeEditFieldID	2303

#define	kParagraphSpacing_DialogID		1006
	#define	kParagraphSpacing_Dialog_SpaceBeforeFieldID		2301
	#define	kParagraphSpacing_Dialog_SpaceAfterFieldID		2302
	#define	kParagraphSpacing_Dialog_LineSpaceModeFieldID	2303
	#define	kParagraphSpacing_Dialog_LineSpaceArgFieldID	2304

#define kBadAllocExceptionOnCmdDialogID	1007


#define	kURLPictID						1001
#define	kUnknownEmbeddingPictID			1002
#define	kUnsupportedPICTFormatPictID	1003





// What is the right set of valid number ranges for my commands/??

#define	kFileCmdBase				(kUserCommandBase+0x100)
#define	kEditCmdBase				(kUserCommandBase+0x200)
#define	kFormatCmdBase				(kUserCommandBase+0x300)


#define	cmdFind						ID_EDIT_FIND
#define	cmdFindAgain				(kEditCmdBase+1)
#define	cmdEnterFindString			(kEditCmdBase+2)

#define	kHideSelectionCmd			(kEditCmdBase+11)
#define	kUnHideSelectionCmd			(kEditCmdBase+12)


#define	cmdChooseFontDialog			(kFormatCmdBase+1)

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
#define	kSubScriptCommand			(kLastFontSizeCmdID+6)
#define	kSuperScriptCommand			(kLastFontSizeCmdID+7)




#if 1
#define	kBaseFontColorCmdID				cmdFontStyleStrikeout + 100
#define	ID_COLOR0						(kBaseFontColorCmdID)
#define	ID_COLOR1						(ID_COLOR0+1)
#define	ID_COLOR2						(ID_COLOR0+2)
#define	ID_COLOR3						(ID_COLOR0+3)
#define	ID_COLOR4						(ID_COLOR0+4)
#define	ID_COLOR5						(ID_COLOR0+5)
#define	ID_COLOR6						(ID_COLOR0+6)
#define	ID_COLOR7						(ID_COLOR0+7)
#define	ID_COLOR8						(ID_COLOR0+8)
#define	ID_COLOR9						(ID_COLOR0+9)
#define	ID_COLOR10						(ID_COLOR0+10)
#define	ID_COLOR11						(ID_COLOR0+11)
#define	ID_COLOR12						(ID_COLOR0+12)
#define	ID_COLOR13						(ID_COLOR0+13)
#define	ID_COLOR14						(ID_COLOR0+14)
#define	ID_COLOR15						(ID_COLOR0+15)
#define	kFontColorOtherCmdID			(ID_COLOR15+1)
#define	kLastFontColorCmdID				kFontColorOtherCmdID
#else
#define	kBaseFontColorCmdID				cmdFontStyleStrikeout + 100
#define	kFontColorBlackCmdID			(kBaseFontColorCmdID+1)
#define	kFontColorRedCmdID				(kBaseFontColorCmdID+2)
#define	kFontColorGreenCmdID			(kBaseFontColorCmdID+3)
#define	kFontColorBlueCmdID				(kBaseFontColorCmdID+4)
#define	kFontColorCyanCmdID				(kBaseFontColorCmdID+5)
#define	kFontColorMagentaCmdID			(kBaseFontColorCmdID+6)
#define	kFontColorYellowCmdID			(kBaseFontColorCmdID+7)
#define	kFontColorOtherCmdID			(kBaseFontColorCmdID+8)
#define	kLastFontColorCmdID				kBaseFontColorCmdID+100
#endif

#define	kFirstJustificationCmdID		(kLastFontColorCmdID+1)
#define	kJustifyLeftCmdID				kFirstJustificationCmdID+0
#define	kJustifyCenterCmdID				kFirstJustificationCmdID+1
#define	kJustifyRightCmdID				kFirstJustificationCmdID+2
#define	kJustifyFullCmdID				kFirstJustificationCmdID+3
#define	kLastJustificationCmdID			kJustifyFullCmdID

#define	kParagraphSpacingCmdID			kLastJustificationCmdID + 1


#define	kGotoLedItWebPageCmdID			kUserCommandBase+0x1000
#define	kGotoSophistsWebPageCmdID		kUserCommandBase+0x1001

// Options - soon to be replaced with an prefs dialog...
#define	kToggleUseSmartCutNPasteCmdID	kUserCommandBase+0x1002
#define	kToggleWrapToWindowCmdID		kUserCommandBase+0x1003
#define	kToggleShowHiddenTextCmdID		kUserCommandBase+0x1004


#define	kFirstShowHideGlyphCmdID		kUserCommandBase+0x2001
#define	kLastShowHideGlyphCmdID			kUserCommandBase+0x2003
#define	kShowHideParagraphGlyphsCmdID	kUserCommandBase+0x2001
#define	kShowHideTabGlyphsCmdID			kUserCommandBase+0x2002
#define	kShowHideSpaceGlyphsCmdID		kUserCommandBase+0x2003



#endif	/*__LedItResourceIDs_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

