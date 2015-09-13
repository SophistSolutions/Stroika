/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Resource_h__
#define	__Resource_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Headers/Resource.h,v 2.43 2004/02/11 22:41:40 lewis Exp $
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
 *	Revision 2.43  2004/02/11 22:41:40  lewis
 *	SPR#1576: update: added 'Replace Again' command to LedIt, LedLineItMFC, ActiveLedIt
 *	
 *	Revision 2.42  2004/02/07 23:38:57  lewis
 *	a whole bunch of new icons commands for toolbar - for SPR#1639
 *	
 *	Revision 2.41  2004/02/07 01:32:34  lewis
 *	SPR#1629: update: Added font-color icon, and associated the icon with the color popup menu. Added support to ActiveLedIt_IconButtonToolbarElement to make that work. Including - changed ToggleButton property to a 'ButtonStyle' property - where one style was 'ICON/POPUP MENU'.
 *	
 *	Revision 2.40  2004/02/04 21:16:12  lewis
 *	SPR#1618: added justification and liststyle icons to standard icon bar (builtins). And allow different default internal/external cmd nmae s(BuiltinCmd class)
 *	
 *	Revision 2.39  2004/02/03 14:51:28  lewis
 *	SPR#1618: added bold, italics, underline (fixed paste icon) to toolbar. Fixed SETFOCUS issue clicking on toolbar buttons
 *	
 *	Revision 2.38  2004/02/03 03:39:38  lewis
 *	SPR#1618: added print command to toolbar. Forced fixing a couple bugs - using MFC UnUpdate dispatcher (CCmdUI.DoUpdate) instead of Led one - because currently some commands - like print - use the MFC dispatch mechanism (could have redone to juse use Led one). Also - fixed commandname matcher to match properly on kAllCmds list using INTENRALNAME mapper
 *	
 *	Revision 2.37  2004/02/03 02:23:04  lewis
 *	SPR#1618: Added undo/redo buttons, and separator support
 *	
 *	Revision 2.36  2004/02/02 21:15:09  lewis
 *	SPR#1618: Added preliminary toolbar support. A small toolbar with a few icons appears - but no DO command or UPDATE_ENABLE_DISABLE CMD support yet (and just plain icon cmd items)
 *	
 *	Revision 2.35  2004/01/30 04:36:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.34  2004/01/25 23:26:55  lewis
 *	SPR#1621: problem with CHecKForUpdatesOnWeb was cuz code to map associated cmd#s returns
 *	zero when a failed mapping occurs, and we had bad association between cmds# between Led internal
 *	and LedIt/ActiveLedIt 'system-dependent' values for Win32. Fix those for kFirstPrivateEmbedding_CmdID/
 *	kLastSelectedEmbedding_CmdID - and clean up first/last private range inside of it.
 *	
 *	Revision 2.33  2004/01/25 21:51:44  lewis
 *	SPR#1284: Added new PasteText command (not part of default menus - but builtin. Added example
 *	of how to use this to the HTML test scriptCommandSupportTester.html). Add extra arg to _AcceleratorTable::Add ()
 *	for position to insert. Added IndexOf () method (to COM API - for IALCommandList. Implement the
 *	IALCommandList::Remove method.
 *	
 *	Revision 2.32  2004/01/25 16:19:22  lewis
 *	SPR#1617: lose old resource-based contextmenu/accelerators, and use new COM-based accelerator
 *	mechanism (part of new COM-based command support)
 *	
 *	Revision 2.31  2004/01/23 23:11:17  lewis
 *	SPR#1617: Added preliminary (largely functional) implementation of OLE automation-based context
 *	menus. Tons of new OLE APIs. Can create context menus from scratch, or built from building
 *	blocks of existing submenus (like font menu).
 *	
 *	Revision 2.30  2003/06/12 17:15:44  lewis
 *	SPR#1526: Added COM-based spellchecker support to ActiveLedIt. Now you can access SpellCheck
 *	menu command if there is a COM-spellchecker associated with ActiveLedIt. New property 'SpellChecker'
 *	you can get/set to specify the spell checker. Seems to work as well as with LedIt!
 *	
 *	Revision 2.29  2003/05/30 14:26:15  lewis
 *	SPR#1517: Added find/replace dialog and OLE_FindReplace and OLE_LaunchReplaceDialog
 *	new OLE AUTOMATION methods
 *	
 *	Revision 2.28  2003/05/08 16:12:48  lewis
 *	SPR#1467: Added Select Menu (moving find/select all under it in context menu). Also fixed
 *	bug with cmd#s (internal# vs led# with CmdToFontName()
 *	
 *	Revision 2.27  2003/04/15 23:37:38  lewis
 *	SPR#1425: added RemoveCol/Row commands
 *	
 *	Revision 2.26  2003/04/04 19:36:51  lewis
 *	SPR#1407: convert ActiveLedIt to using new command-number vectoring etc scheme (not
 *	template based). Revised cmd #define names in a few cases for greater consistency
 *	across demo apps.
 *	
 *	Revision 2.25  2003/03/11 02:33:12  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand,
 *	and other cleanups to Led_URLManager calls and context menu
 *	
 *	Revision 2.24  2003/01/23 20:29:15  lewis
 *	SPR#1256 - use Led_StdDialogHelper_ParagraphSpacingDialog etc instead of local copy. SPR#1186 - added
 *	support for Led_StdDialogHelper_ParagraphIndentsDialog
 *	
 *	Revision 2.23  2002/11/21 16:13:59  lewis
 *	SPR#1182 - a few changes to qDemoMode code. Write bin version# too. Add code to only give
 *	one day additional when changing version numbers. Dialogs come up more often. Small related
 *	logic fixes and disable ESC key from dismissing nag dialog
 *	
 *	Revision 2.22  2002/11/21 14:35:58  lewis
 *	SPR#1177- add insert table row/col menu items
 *	
 *	Revision 2.21  2002/10/25 17:27:08  lewis
 *	SPR#1146 - Added 'Insert Symbol' menu item
 *	
 *	Revision 2.20  2002/10/23 01:42:43  lewis
 *	SPR#1137- add Insert menu - including insert table and insert URL. Both seem to work as
 *	well as with LedIt WIN
 *	
 *	Revision 2.19  2002/05/06 21:34:20  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.18  2001/11/27 00:32:29  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.17  2001/09/18 14:03:34  lewis
 *	shorten define name from qStandardURLStyleMarkerNewDisplayMode to qURLStyleMarkerNewDisplayMode
 *	(avoid MSDEV60 warnings for define name length)
 *	
 *	Revision 2.16  2001/08/30 00:35:56  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.15  2001/08/27 13:43:42  lewis
 *	SPR#0972- add ListStyle popup menu, and fix LedItView::GetLayoutMargins () so bullets
 *	display properly
 *	
 *	Revision 2.14  2001/08/20 22:23:01  lewis
 *	SPR#0818- new display of URL objects (now just blue underlined slightly enlarged text).
 *	Can use qStandardURLStyleMarkerNewDisplayMode to get old code
 *	
 *	Revision 2.13  2001/08/17 16:30:31  lewis
 *	SPR#0959- GetInfo (DIB/URL/etc) command support
 *	
 *	Revision 2.12  2001/05/29 23:12:45  lewis
 *	SPR#0943- new font color list (from ledit)
 *	
 *	Revision 2.11  2001/04/11 22:55:23  lewis
 *	SPR#0861- DEMO MODE annoyance dialog. Keep track in registry of how long demo been going on.
 *	Bring up annoyance dialog after a while - when demo expired. And add note in about box saying
 *	how long left in demo
 *	
 *	Revision 2.10  2000/11/09 05:00:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.9  2000/10/18 20:59:52  lewis
 *	cleanups. Lose FindDialog code - and inherit new sahred implm in Led_StdDialogHelper_FindDialog etc
 *	
 *	Revision 2.8  2000/06/14 13:52:53  lewis
 *	cleanups(moved FindDialog etc into scope of function - by getting rid of unused message map declaration).
 *	And SPR#0785 - Added 'Paragraph Spacing cmd/dialog support
 *	
 *	Revision 2.7  1998/03/04 20:43:51  lewis
 *	Major cleanups - Major addition to OLEAUT support.
 *	
 *	Revision 2.6  1997/12/24  04:43:52  lewis
 *	*** empty log message ***
 *
 *	Revision 2.5  1997/12/24  04:29:58  lewis
 *	new resources/icons for url/unknown embedding etc picts
 *
 *	Revision 2.4  1997/09/29  18:11:11  lewis
 *	Justification support
 *
 *	Revision 2.3  1997/07/27  15:59:52  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.2  1997/06/28  17:44:01  lewis
 *	add subscript command, and find dialog and fontsize dialog.
 *
 *	Revision 2.1  1997/06/23  16:56:16  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	"ActiveLedItConfig.h"


#define IDS_ACTIVELEDIT                 1
#define IDD_ABOUTBOX_ACTIVELEDIT        1
#define IDB_ACTIVELEDIT                 1
#define IDI_ABOUTDLL                    1
#define IDS_ACTIVELEDIT_PPG             2
#define IDS_ACTIVELEDIT_PPG_CAPTION     200
#define IDD_PROPPAGE_ACTIVELEDIT        200
#define kShowVerticalScrollbarID        201
#define kShowHorizontalScrollbarID      202



#if		!qURLStyleMarkerNewDisplayMode
#define	kURLPictID						1001
#endif
#define	kUnknownEmbeddingPictID			1002
#define	kUnsupportedPICTFormatPictID	1003


#define	kToolbarButton_Cut_ResID				2001
#define	kToolbarButton_Copy_ResID				2002
#define	kToolbarButton_Paste_ResID				2003
#define	kToolbarButton_Undo_ResID				2004
#define	kToolbarButton_Redo_ResID				2005
#define	kToolbarButton_Print_ResID				2006
#define	kToolbarButton_Bold_ResID				2007
#define	kToolbarButton_Italics_ResID			2008
#define	kToolbarButton_Underline_ResID			2009
#define	kToolbarButton_FontColor_ResID			2010
#define	kToolbarButton_JustifyLeft_ResID		2011
#define	kToolbarButton_JustifyCenter_ResID		2012
#define	kToolbarButton_JustifyRight_ResID		2013
#define	kToolbarButton_JustifyFull_ResID		2014
#define	kToolbarButton_ListStyle_None_ResID		2015
#define	kToolbarButton_ListStyle_Bullet_ResID	2016
#define	kToolbarButton_Find_ResID				2017
#define	kToolbarButton_Replace_ResID			2018
#define	kToolbarButton_CheckSpelling_ResID		2019
#define	kToolbarButton_ActiveLedIt_ResID		2020
#define	kToolbarButton_ParagraphSpacing_ResID	2021
#define	kToolbarButton_ParagraphIndents_ResID	2022
#define	kToolbarButton_InsertTable_ResID		2023
#define	kToolbarButton_InsertURL_ResID			2024
#define	kToolbarButton_InsertSymbol_ResID		2025


// MFC must define something like this someplace, but I haven't found where....
// Use this for now, so I can update things more easily when I find the MFC definition...
#define	kUserCommandBase			0x2000



	#define	kEditCmdBase				(kUserCommandBase+0x1100)
	#define	kSelectCmdBase				(kUserCommandBase+0x1200)
	#define	kInsertCmdBase				(kUserCommandBase+0x1300)
	#define	kFormatCmdBase				(kUserCommandBase+0x2000)
	#define	kRemoveCmdBase				(kUserCommandBase+0x3000)
	#define	kMiscCmdBase				(kUserCommandBase+0x3100)

#define	kFindCmd					ID_EDIT_FIND
#define	kFindAgainCmd				(kEditCmdBase+1)
#define	kEnterFindStringCmd			(kEditCmdBase+2)
#define	kReplaceCmd					ID_EDIT_REPLACE
#define	kReplaceAgainCmd			(kEditCmdBase+3)

	#ifndef	kSpellCheckCmd
	#define	kSpellCheckCmd					(kEditCmdBase+12)
	#endif

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



#define	kChooseFontDialogCmd		(kFormatCmdBase+1)

#define	kBaseFontNameCmd			(kFormatCmdBase+0x100)
#define	kLastFontNameCmd			(kBaseFontNameCmd+0x400)

#define	kBaseFontSizeCmd			kLastFontNameCmd+1
#define	kFontSize9Cmd				kBaseFontSizeCmd
#define	kFontSize10Cmd				(kFontSize9Cmd + 1)
#define	kFontSize12Cmd				(kFontSize9Cmd + 2)
#define	kFontSize14Cmd				(kFontSize9Cmd + 3)
#define	kFontSize18Cmd				(kFontSize9Cmd + 4)
#define	kFontSize24Cmd				(kFontSize9Cmd + 5)
#define	kFontSize36Cmd				(kFontSize9Cmd + 6)
#define	kFontSize48Cmd				(kFontSize9Cmd + 7)
#define	kFontSize72Cmd				(kFontSize9Cmd + 8)
#define	kFontSizeOtherCmd			(kFontSize9Cmd + 9)
#define	kFontSizeSmallerCmd			(kFontSize9Cmd + 10)
#define	kFontSizeLargerCmd			(kFontSize9Cmd + 11)
#define	kLastFontSizeCmd			kFontSizeLargerCmd


#define	kFontStylePlainCmd			(kLastFontSizeCmd+1)
#define	kFontStyleBoldCmd			(kLastFontSizeCmd+2)
#define	kFontStyleItalicCmd			(kLastFontSizeCmd+3)
#define	kFontStyleUnderlineCmd		(kLastFontSizeCmd+4)
#define	kFontStyleStrikeoutCmd		(kLastFontSizeCmd+5)
#define	kSubScriptCmd				(kLastFontSizeCmd+6)
#define	kSuperScriptCmd				(kLastFontSizeCmd+7)


#ifndef	kBaseFontColorCmd
#define	kBaseFontColorCmd				(kSuperScriptCmd + 100)
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


#define	kFirstJustificationCmd		(kLastFontNamedColorCmd+0x10)
#define	kJustifyLeftCmd				kFirstJustificationCmd+0
#define	kJustifyCenterCmd				kFirstJustificationCmd+1
#define	kJustifyRightCmd				kFirstJustificationCmd+2
#define	kJustifyFullCmd				kFirstJustificationCmd+3
#define	kLastJustificationCmd			kJustifyFullCmd

#define	kParagraphSpacingCmd			kLastJustificationCmd + 1
#define	kParagraphIndentsCmd			kLastJustificationCmd + 2

#define	kListStyle_NoneCmd				(kFormatCmdBase+0xa00)
#define	kListStyle_BulletCmd			(kFormatCmdBase+0xa00) + 1

#define	kIncreaseIndentCmd				(kFormatCmdBase+0xb00)
#define	kDecreaseIndentCmd				(kFormatCmdBase+0xb00) + 1


#ifndef	kRemoveTableRowsCmd
#define	kRemoveTableRowsCmd				(kRemoveCmdBase+3)
#endif
#ifndef	kRemoveTableColumnsCmd
#define	kRemoveTableColumnsCmd			(kRemoveCmdBase+4)
#endif


#define	kGotoLedItWebPageCmdID			kUserCommandBase+0x1000
#define	kGotoSophistsWebPageCmdID		kUserCommandBase+0x1001
#define	kCheckForUpdatesWebPageCmdID	kUserCommandBase+0x1007
#define	kPasteFormatTextCmdID			kUserCommandBase+0x1003

// Options - soon to be replaced with an prefs dialog...
#define	kToggleUseSmartCutNPasteCmdID	kUserCommandBase+0x1050


#define	kFirstOLEUserCmdCmdID		kUserCommandBase+0x4000
#define	kLastOLEUserCmdCmdID		kFirstOLEUserCmdCmdID+0xfff



#if		qDemoMode
#define	kDemoExpired_DialogID			2001
	#define	kDemoExpired_Dialog_CancelFieldID		2301
#endif

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        204
#define _APS_NEXT_COMMAND_VALUE         32768
#define _APS_NEXT_CONTROL_VALUE         204
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif


#endif	/*__Resource_h__*/
