/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedItResourceIDs_h__
#define	__LedItResourceIDs_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/LedItResources.h,v 1.38 2004/02/11 22:41:31 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedItResources.h,v $
 *	Revision 1.38  2004/02/11 22:41:31  lewis
 *	SPR#1576: update: added 'Replace Again' command to LedIt, LedLineItMFC, ActiveLedIt
 *	
 *	Revision 1.37  2004/01/25 23:25:40  lewis
 *	SPR#1621: problem with CHecKForUpdatesOnWeb was cuz code to map associated cmd#s returns zero
 *	when a failed mapping occurs, and we had bad association between cmds# between Led internal
 *	and LedIt/ActiveLedIt 'system-dependent' values for Win32. Fix those for kFirstPrivateEmbedding_CmdID/
 *	kLastSelectedEmbedding_CmdID - and clean up first/last private range inside of it.
 *	
 *	Revision 1.36  2003/06/02 16:15:42  lewis
 *	SPR#1513: Simple spellcheck engine and spell check dialog support (windows only so far)
 *	
 *	Revision 1.35  2003/05/30 03:11:58  lewis
 *	SPR#1517: preliminary version of REPLACE DIALOG support (WIN32 only so far).
 *	
 *	Revision 1.34  2003/05/08 18:04:05  lewis
 *	SPR#1469: problem with hidden text commands not working may have been cmd# conflict - changed
 *	kUserCommandBase to 0x1000
 *	
 *	Revision 1.33  2003/05/08 00:11:25  lewis
 *	SPR#1467: added kSelectTableIntraCellAll_CmdID and cleaned up menu command name strings for
 *	new select commands(win32)
 *	
 *	Revision 1.32  2003/05/07 23:34:36  lewis
 *	SPR#1467: new Select menu for MacOS
 *	
 *	Revision 1.31  2003/05/07 21:14:07  lewis
 *	SPR#1467: new Select menu (and context menu) for Win32
 *	
 *	Revision 1.30  2003/04/18 00:42:56  lewis
 *	SPR#1444: fixed part of this - fixed CMD# IDs to eliminate conflicts I probably introduced
 *	when I added Remove menu???
 *	
 *	Revision 1.29  2003/04/15 22:36:39  lewis
 *	SPR#1425: added new 'Remove' menu and added to it RemoveTableRows/Cols, and moved Show/Hide
 *	Selection there
 *	
 *	Revision 1.28  2003/04/07 13:27:30  lewis
 *	SPR#1412: for MacOS - add list style menu and indent/unindent
 *	
 *	Revision 1.27  2003/03/11 21:45:24  lewis
 *	SPR#1287 - use Gestalt to see if Aqua UI is present and remove QUIT menu if it is. Also -
 *	if HMGetHelpMenuHandle() == NULL (as it is in OSX) - then append a Help menu and store
 *	our help menu items in that
 *	
 *	Revision 1.26  2003/03/11 02:31:15  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand
 *	
 *	Revision 1.25  2003/01/22 01:08:23  lewis
 *	SPR#1256 - OtherSize&ParagraphSpacing dialogs moved to LedStdDialogs(untested on Mac).
 *	
 *	Revision 1.24  2003/01/21 13:25:53  lewis
 *	SPR#1186 - first cut at Indents dialog support
 *	
 *	Revision 1.23  2002/11/19 19:56:32  lewis
 *	SPR#1176 - Added insert row/col before/after commands
 *	
 *	Revision 1.22  2002/10/25 17:57:01  lewis
 *	SPR#1146 - Added 'Insert Symbol' menu item
 *	
 *	Revision 1.21  2002/10/24 02:21:09  lewis
 *	SPR#1137- add insert menu for LedIt Mac
 *	
 *	Revision 1.20  2002/10/22 00:43:17  lewis
 *	SPR#1136- Add URL support
 *
 *	Revision 1.19  2002/09/11 04:15:50  lewis
 *	SPR#1094- VERY preliminary Table support - an insert menu and InsertTable command
 *	
 *	Revision 1.18  2002/05/06 21:30:58  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.17  2001/11/27 00:28:09  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.16  2001/09/26 15:41:45  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.15  2001/09/18 19:13:42  lewis
 *	SPR#0791- added support for toggle (use smart cut&paste, wrap-to-window,show hidden text)
 *	commands for MacOS
 *	
 *	Revision 1.14  2001/09/18 14:02:41  lewis
 *	shorten define name from qStandardURLStyleMarkerNewDisplayMode to qURLStyleMarkerNewDisplayMode
 *	(avoid MSDEV60 warnings for define name length)
 *	
 *	Revision 1.13  2001/09/18 13:52:36  lewis
 *	change icon for RTF docs
 *	
 *	Revision 1.12  2001/09/17 14:42:16  lewis
 *	SPR#1032- added SetDefaultFont dialog and saved preferences (pref for new docs)
 *	
 *	Revision 1.11  2001/09/12 00:21:55  lewis
 *	SPR#1023- added LedItApplication::HandleBadUserInputException () code and other related fixes
 *
 *	Revision 1.10  2001/08/29 22:59:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.9  2001/08/20 22:22:44  lewis
 *	SPR#0818- new display of URL objects (now just blue underlined slightly enlarged text). Can use
 *	qStandardURLStyleMarkerNewDisplayMode to get old code
 *	
 *	Revision 1.8  2001/08/16 18:53:54  lewis
 *	SPR#0959- more work on 'GetInfo' support for embeddings.
 *	
 *	Revision 1.7  2001/07/17 19:06:28  lewis
 *	SPR#0959- kFirstSelectedEmbeddingCmd support
 *	
 *	Revision 1.6  2001/07/11 21:51:20  lewis
 *	SPR#0906- Partly implemented bullet/list support. Just did basic support (bullet lists, no other
 *	style lists). Did support indent levels (including new Increase/Decrease indent level commands). Still
 *	no RTF/HTML support, and a small display bug on following lines when I add bullet attribute.
 *	
 *	Revision 1.5  2001/05/22 21:44:32  lewis
 *	MAJOR changes - now supporting checked menu items on X-Windows and enabling (SPR#0924).
 *	Also, much improved dialog code - including dialog for aboutbox, font/color pickers
 *	etc (SPR#0923)
 *	
 *	Revision 1.4  2001/05/18 23:00:11  lewis
 *	small cleanups - and gotoWebPage(help menu) support for XWindows
 *	
 *	Revision 1.3  2001/05/18 21:08:30  lewis
 *	About box code cleanups/sharing and support for Linux
 *	
 *	Revision 1.2  2001/05/15 16:43:16  lewis
 *	SPR#0920- lots more misc cleanups to share more code across platforms and standardize cmd names etc
 *	
 *	Revision 1.1  2001/05/14 20:54:43  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *	
 *
 *
 *
 *	<<<***		Based on LedItMFC project from Led 3.0b6		***>>>
 *
 *
 *
 *
 */

#if		defined (WIN32)
	#include	<afxres.h>
#endif

#include	"LedItConfig.h"



	// MFC must define something like this someplace, but I haven't found where....
	// Use this for now, so I can update things more easily when I find the MFC definition...
	// PowerPlant/Gtk don't seem picky about this number - same # works in both places - but it would be OK to have
	// this differ across platforms.
	// NOTE: MFC commands tend to be in the range:
	//		0xE100 (ID_FILE_NEW) ... 0xEffff
	#define	kUserCommandBase			0x1000

	#define	kFileCmdBase				(kUserCommandBase+0x1000)
	#define	kEditCmdBase				(kUserCommandBase+0x1100)
	#define	kSelectCmdBase				(kUserCommandBase+0x1200)
	#define	kInsertCmdBase				(kUserCommandBase+0x1300)
	#define	kFormatCmdBase				(kUserCommandBase+0x2000)
	#define	kRemoveCmdBase				(kUserCommandBase+0x3000)
	#define	kMiscCmdBase				(kUserCommandBase+0x3100)



// Global command numbers
	#if		qMacOS
		#define	kCmdUndo					cmd_Undo
		#define	kFontStylePlainCmd			cmd_Plain
		#define	kFontStyleBoldCmd			cmd_Bold
		#define	kFontStyleItalicCmd			cmd_Italic
		#define	kFontStyleUnderlineCmd		cmd_Underline
		#define	kFontStyleOutlineCmd		cmd_Outline
		#define	kFontStyleShadowCmd			cmd_Shadow
		#define	kFontStyleCondensedCmd		cmd_Condense
		#define	kFontStyleExtendedCmd		cmd_Extend
	#endif
	#if		qWindows
		#define	kFindCmd					ID_EDIT_FIND
		#define	kReplaceCmd					ID_EDIT_REPLACE
		#define	kCmdUndo					ID_EDIT_UNDO
		#define	kCmdRedo					ID_EDIT_REDO
		#define	kCmdSelectAll				ID_EDIT_SELECT_ALL
		#define	kCmdCut						ID_EDIT_CUT
		#define	kCmdCopy					ID_EDIT_COPY
		#define	kCmdPaste					ID_EDIT_PASTE
		#define	kCmdClear					ID_EDIT_CLEAR
	#endif

	#ifndef	kCmdSaveACopyAs
	#define	kCmdSaveACopyAs					(kFileCmdBase+1)
	#endif


	#ifndef	kFindCmd
	#define	kFindCmd						(kEditCmdBase+1)
	#endif
	#ifndef	kFindAgainCmd
	#define	kFindAgainCmd					(kEditCmdBase+2)
	#endif
	#ifndef	kEnterFindStringCmd
	#define	kEnterFindStringCmd				(kEditCmdBase+3)
	#endif
	#ifndef	kCmdUndo
	#define	kCmdUndo						(kEditCmdBase+4)
	#endif
	#ifndef	kCmdRedo
	#define	kCmdRedo						(kEditCmdBase+5)
	#endif
	#ifndef	kCmdSelectAll
	#define	kCmdSelectAll					(kEditCmdBase+6)
	#endif
	#ifndef	kCmdCut
	#define	kCmdCut							(kEditCmdBase+7)
	#endif
	#ifndef	kCmdCopy
	#define	kCmdCopy						(kEditCmdBase+8)
	#endif
	#ifndef	kCmdPaste
	#define	kCmdPaste						(kEditCmdBase+9)
	#endif
	#ifndef	kCmdClear
	#define	kCmdClear						(kEditCmdBase+10)
	#endif
	#ifndef	kReplaceCmd
	#define	kReplaceCmd						(kEditCmdBase+11)
	#endif
	#ifndef	kSpellCheckCmd
	#define	kSpellCheckCmd					(kEditCmdBase+12)
	#endif

	#define	kReplaceAgainCmd				(kEditCmdBase+13)


	#ifndef	kFirstSelectedEmbeddingCmd
	#define	kFirstSelectedEmbeddingCmd		(kEditCmdBase+20)
	#endif
		#ifndef	kPropertiesForSelectionCmd
		#define	kPropertiesForSelectionCmd		kFirstSelectedEmbeddingCmd
		#endif
		#ifndef	kFirstPrivateEmbeddingCmd
		#define	kFirstPrivateEmbeddingCmd		(kFirstSelectedEmbeddingCmd+1)
		#endif
		#ifndef	kLastPrivateEmbeddingCmd
		#define	kLastPrivateEmbeddingCmd		(kFirstPrivateEmbeddingCmd+5)
		#endif
	// # here is syncronized with value in WordProcessor::kLastSelectedEmbedding_CmdID
	#ifndef	kLastSelectedEmbeddingCmd
	#define	kLastSelectedEmbeddingCmd		kLastPrivateEmbeddingCmd
	#endif




	#ifndef	kSelectWordCmd
	#define	kSelectWordCmd					(kSelectCmdBase+1)
	#endif

	#ifndef	kSelectTextRowCmd
	#define	kSelectTextRowCmd				(kSelectCmdBase+2)
	#endif

	#ifndef	kSelectParagraphCmd
	#define	kSelectParagraphCmd				(kSelectCmdBase+3)
	#endif

	#ifndef	kSelectTableIntraCellAllCmd
	#define	kSelectTableIntraCellAllCmd		(kSelectCmdBase+4)
	#endif

	#ifndef	kSelectTableCellCmd
	#define	kSelectTableCellCmd				(kSelectCmdBase+5)
	#endif

	#ifndef	kSelectTableRowCmd
	#define	kSelectTableRowCmd				(kSelectCmdBase+6)
	#endif

	#ifndef	kSelectTableColumnCmd
	#define	kSelectTableColumnCmd			(kSelectCmdBase+7)
	#endif

	#ifndef	kSelectTableCmd
	#define	kSelectTableCmd					(kSelectCmdBase+8)
	#endif






	#ifndef	kInsertTableCmd
	#define	kInsertTableCmd					(kInsertCmdBase+1)
	#endif

	#ifndef	kInsertTableRowAboveCmd
	#define	kInsertTableRowAboveCmd			(kInsertCmdBase+2)
	#endif

	#ifndef	kInsertTableRowBelowCmd
	#define	kInsertTableRowBelowCmd			(kInsertCmdBase+3)
	#endif

	#ifndef	kInsertTableColBeforeCmd
	#define	kInsertTableColBeforeCmd		(kInsertCmdBase+4)
	#endif

	#ifndef	kInsertTableColAfterCmd
	#define	kInsertTableColAfterCmd			(kInsertCmdBase+5)
	#endif

	#ifndef	kInsertURLCmd
	#define	kInsertURLCmd					(kInsertCmdBase+6)
	#endif

	#ifndef	kInsertSymbolCmd
	#define	kInsertSymbolCmd				(kInsertCmdBase+7)
	#endif




	#ifndef	kChooseFontDialogCmd
	#define	kChooseFontDialogCmd			(kFormatCmdBase+11)
	#endif

	#ifndef	kBaseFontNameCmd
	#define	kBaseFontNameCmd				(kFormatCmdBase+0x100)
	#endif
	#ifndef	kLastFontNameCmd
	#define	kLastFontNameCmd				(kFormatCmdBase+0x500)
	#endif

	#ifndef	kBaseFontSizeCmd
	#define	kBaseFontSizeCmd				(kFormatCmdBase+0x600)
	#endif
	#define	kFontSize9Cmd					kBaseFontSizeCmd
	#define	kFontSize10Cmd					(kFontSize9Cmd + 1)
	#define	kFontSize12Cmd					(kFontSize9Cmd + 2)
	#define	kFontSize14Cmd					(kFontSize9Cmd + 3)
	#define	kFontSize18Cmd					(kFontSize9Cmd + 4)
	#define	kFontSize24Cmd					(kFontSize9Cmd + 5)
	#define	kFontSize36Cmd					(kFontSize9Cmd + 6)
	#define	kFontSize48Cmd					(kFontSize9Cmd + 7)
	#define	kFontSize72Cmd					(kFontSize9Cmd + 8)
	#define	kFontSizeOtherCmd				(kFontSize9Cmd + 9)
	#define	kFontSizeSmallerCmd				(kFontSize9Cmd + 10)
	#define	kFontSizeLargerCmd				(kFontSize9Cmd + 11)
	#define	kLastFontSizeCmd				kFontSizeLargerCmd


	#ifndef	kBaseFontColorCmd
	#define	kBaseFontColorCmd				(kFormatCmdBase+0x700)
	#endif
	#define	kBlackColorCmd					(kBaseFontColorCmd + 0)
	#define	kMaroonColorCmd					(kBaseFontColorCmd + 1)
	#define	kGreenColorCmd					(kBaseFontColorCmd + 2)
	#define	kOliveColorCmd					(kBaseFontColorCmd + 3)
	#define	kNavyColorCmd					(kBaseFontColorCmd + 4)
	#define	kPurpleColorCmd					(kBaseFontColorCmd + 5)
	#define	kTealColorCmd					(kBaseFontColorCmd + 6)
	#define	kGrayColorCmd					(kBaseFontColorCmd + 7)
	#define	kSilverColorCmd					(kBaseFontColorCmd + 8)
	#define	kRedColorCmd					(kBaseFontColorCmd + 9)
	#define	kLimeColorCmd					(kBaseFontColorCmd + 10)
	#define	kYellowColorCmd					(kBaseFontColorCmd + 11)
	#define	kBlueColorCmd					(kBaseFontColorCmd + 12)
	#define	kFuchsiaColorCmd				(kBaseFontColorCmd + 13)
	#define	kAquaColorCmd					(kBaseFontColorCmd + 14)
	#define	kWhiteColorCmd					(kBaseFontColorCmd + 15)
	#define	kFontColorOtherCmd				(kBaseFontColorCmd + 16)
	#define	kLastFontNamedColorCmd			kWhiteColorCmd


	#ifndef	kFirstFontStyleCmd
	#define	kFirstFontStyleCmd				(kFormatCmdBase+0x800)
	#endif
	#ifndef	kFontStylePlainCmd
	#define	kFontStylePlainCmd				(kFirstFontStyleCmd+0)
	#endif
	#ifndef	kFontStyleBoldCmd
	#define	kFontStyleBoldCmd				(kFirstFontStyleCmd+1)
	#endif
	#ifndef	kFontStyleItalicCmd
	#define	kFontStyleItalicCmd				(kFirstFontStyleCmd+2)
	#endif
	#ifndef	kFontStyleUnderlineCmd
	#define	kFontStyleUnderlineCmd			(kFirstFontStyleCmd+3)
	#endif
	#ifndef	kFontStyleOutlineCmd
	#define	kFontStyleOutlineCmd			(kFirstFontStyleCmd+4)
	#endif
	#ifndef	kFontStyleShadowCmd
	#define	kFontStyleShadowCmd				(kFirstFontStyleCmd+5)
	#endif
	#ifndef	kFontStyleCondensedCmd
	#define	kFontStyleCondensedCmd			(kFirstFontStyleCmd+6)
	#endif
	#ifndef	kFontStyleExtendedCmd
	#define	kFontStyleExtendedCmd			(kFirstFontStyleCmd+7)
	#endif
	#ifndef	kFontStyleStrikeoutCmd
	#define	kFontStyleStrikeoutCmd			(kFirstFontStyleCmd+8)
	#endif
	#ifndef	kSubScriptCmd
	#define	kSubScriptCmd					(kFirstFontStyleCmd+9)
	#endif
	#ifndef	kSuperScriptCmd
	#define	kSuperScriptCmd					(kFirstFontStyleCmd+10)
	#endif

	#define	kFirstJustificationCmd			(kFormatCmdBase+0x900)
	#define	kJustifyLeftCmd					kFirstJustificationCmd+0
	#define	kJustifyCenterCmd				kFirstJustificationCmd+1
	#define	kJustifyRightCmd				kFirstJustificationCmd+2
	#define	kJustifyFullCmd					kFirstJustificationCmd+3
	#define	kLastJustificationCmd			kJustifyFullCmd

	#define	kParagraphSpacingCmd			kLastJustificationCmd + 1
	#define	kParagraphIndentsCmd			kLastJustificationCmd + 2

	#define	kListStyle_NoneCmd				(kFormatCmdBase+0xa00)
	#define	kListStyle_BulletCmd			(kFormatCmdBase+0xa00) + 1

	#define	kIncreaseIndentCmd				(kFormatCmdBase+0xb00)
	#define	kDecreaseIndentCmd				(kFormatCmdBase+0xb00) + 1


	#ifndef	kHideSelectionCmd
	#define	kHideSelectionCmd				(kRemoveCmdBase+1)
	#endif
	#ifndef	kUnHideSelectionCmd
	#define	kUnHideSelectionCmd				(kRemoveCmdBase+2)
	#endif
	#ifndef	kRemoveTableRowsCmd
	#define	kRemoveTableRowsCmd				(kRemoveCmdBase+3)
	#endif
	#ifndef	kRemoveTableColumnsCmd
	#define	kRemoveTableColumnsCmd			(kRemoveCmdBase+4)
	#endif



	// Options - soon to be replaced with an prefs dialog...
	#define	kToggleUseSmartCutNPasteCmd		kMiscCmdBase+10
	#define	kToggleWrapToWindowCmd			kMiscCmdBase+11
	#define	kToggleShowHiddenTextCmd		kMiscCmdBase+12
	#define	kFirstShowHideGlyphCmd		kMiscCmdBase+13
		#define	kShowHideParagraphGlyphsCmd		kFirstShowHideGlyphCmd+0
		#define	kShowHideTabGlyphsCmd			kFirstShowHideGlyphCmd+1
		#define	kShowHideSpaceGlyphsCmd			kFirstShowHideGlyphCmd+2
	#define	kLastShowHideGlyphCmd			kShowHideSpaceGlyphsCmd


	#define	cmdChooseDefaultFontDialog		(kMiscCmdBase+25)


	#ifndef	kAboutBoxCmd
	#define	kAboutBoxCmd					kMiscCmdBase+1
	#endif
	#ifndef	kGotoLedItWebPageCmd
	#define	kGotoLedItWebPageCmd			kMiscCmdBase+2
	#endif
	#ifndef	kGotoSophistsWebPageCmd
	#define	kGotoSophistsWebPageCmd			kMiscCmdBase+3
	#endif
	#ifndef	kCheckForUpdatesWebPageCmdID
	#define	kCheckForUpdatesWebPageCmdID	kMiscCmdBase+4
	#endif

	// Command Numbers
	#if		qMacOS
		#define	kBaseWindowCmd				20600
		#define	kLastWindowCmd				20699
	#elif	qXWindows
		enum	{
			kNewDocumentCmd,
			kOpenDocumentCmd,
			kSaveDocumentCmd,
			kSaveAsDocumentCmd,
			kQuitCmd,
		};
	#endif



#if		qMacOS
	#define	kAppleMenuID		128		//	PowerPlant assumes this ID - cannot change - LGP 960614
	#define	kFileMenuID			2
	#define	kEditMenuID			3
	#define	kSelectMenuID		4
	#define	kInsertMenuID		5
	#define	kFormatMenuID		6
	#define	kRemoveMenuID		7
	#define	kOptionsMenuID		8
	#define	kWindowsMenuID		9
	#define	kHelpMenuID			10


	#define	cmd_ListStyleMenu		249
	#define	cmd_FontMenu			250
	#define	cmd_SizeMenu			251
	#define	cmd_StyleMenu			252
	#define	cmd_ColorMenu			253
	#define	cmd_JustificationMenu	254


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

#if		!qURLStyleMarkerNewDisplayMode
	#define	kURLPictID					5001
#endif
	#define	kUnknownEmbeddingPictID		5002
	#define	kUnsupportedDIBFormatPictID	5003


	#define	WIND_TextDoc		200
	#define	prto_TextDoc		201

#elif	qWindows

	#define IDR_MAINFRAME				128
	#define kLedItDocumentIconID		129
	#define kLedItRTFDocumentIconID		130

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

	#define kBadAllocExceptionOnCmdDialogID	1007

	#define kBadUserInputExceptionOnCmdDialogID	1008


#if		!qURLStyleMarkerNewDisplayMode
	#define	kURLPictID						1001
#endif
	#define	kUnknownEmbeddingPictID			1002
	#define	kUnsupportedPICTFormatPictID	1003

#endif



#endif	/*__LedItResourceIDs_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

