/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Resource_h__
#define	__Resource_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/NetLedIt/Headers/Resource.h,v 1.1 2004/01/01 04:20:23 lewis Exp $
 *
 * Changes:
 *	$Log: Resource.h,v $
 *	Revision 1.1  2004/01/01 04:20:23  lewis
 *	moved NetLedIt to (Unsupported)
 *	
 *	Revision 1.28  2003/04/05 19:29:51  lewis
 *	SPR#1407: support new command mechanism
 *	
 *	Revision 1.27  2003/04/05 16:01:51  lewis
 *	SPR#1407: support new command handling code - and normalize #define names a bit across projects
 *	
 *	Revision 1.26  2003/03/11 02:35:54  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand
 *	
 *	Revision 1.25  2002/05/06 21:34:57  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.24  2001/11/27 00:32:57  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.23  2001/09/26 15:53:04  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.22  2001/09/19 18:44:42  lewis
 *	prelim support for catching exceptions on commands (kBallAllocException etc in LedItView)
 *	
 *	Revision 1.21  2001/09/18 14:03:47  lewis
 *	shorten define name from qStandardURLStyleMarkerNewDisplayMode to qURLStyleMarkerNewDisplayMode
 *	(avoid MSDEV60 warnings for define name length)
 *	
 *	Revision 1.20  2001/09/15 22:43:22  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.19  2001/09/06 20:28:54  lewis
 *	SPR#1003/SPR#0998- cleanup about box and add new decent netledit icon (Win only so far)
 *	
 *	Revision 1.18  2001/08/30 00:43:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.17  2001/08/27 13:44:29  lewis
 *	SPR#0972- add ListStyle popup menu (windows only), and fix LedItView::GetLayoutMargins ()
 *	so bullets display properly
 *	
 *	Revision 1.16  2001/08/20 22:23:13  lewis
 *	SPR#0818- new display of URL objects (now just blue underlined slightly enlarged text).
 *	Can use qStandardURLStyleMarkerNewDisplayMode to get old code
 *	
 *	Revision 1.15  2001/08/17 17:03:02  lewis
 *	fix Windows to include unknown etc embedding picts/bmps. SPR#0959 - GetInfo support Mac
 *	and Windows (untested on mac yet)
 *	
 *	Revision 1.14  2001/05/29 23:01:06  lewis
 *	SPR#0943- fix to use new color list from combined LedIt! app. Fix a few names/color values.
 *	Fix Windows code to update command names (as in Undo XXX) and checkmarks.
 *	
 *	Revision 1.13  2001/04/12 19:46:39  lewis
 *	SPR#0861- Expiration support for demo-mode
 *	
 *	Revision 1.12  2000/10/19 15:52:11  lewis
 *	use new portable About code and Find code. Further cleanups/improvements.
 *	
 *	Revision 1.11  2000/10/18 20:58:36  lewis
 *	fixed windoze removemenu DUP menu items on Windows code. Added hooks into new
 *	Led_StdDialogHelper_FindDialog code
 *	
 *	Revision 1.10  2000/10/17 18:16:26  lewis
 *	mac aboutbox work
 *	
 *	Revision 1.9  2000/10/14 14:31:30  lewis
 *	start implementing help/aboutbox for Mac/Windows. Got the cmds hooked in. Only tested on
 *	Windows (help). Still to write is aboutbox code (for both platoforms)
 *	
 *	Revision 1.8  2000/10/05 17:35:12  lewis
 *	kUnsupportedDIBFormatPictID
 *	
 *	Revision 1.7  2000/10/03 21:52:53  lewis
 *	Lots more work relating to SPR#0839. Got a bit more of the context menu stuff working
 *	(about 75% now working on the mac)
 *	
 *	Revision 1.6  2000/10/03 13:40:19  lewis
 *	SPR#0840- new WaterMarkHelper<> template for watermark/demomode. SPR#0839- use new
 *	WordProcessorCommonCommandHelper/TextInteractorCommonCommandHelper<> templates.
 *	A bit more code handling context menus (still more todo)
 *	
 *	Revision 1.5  2000/09/30 19:37:24  lewis
 *	more work on context menus
 *	
 *	Revision 1.4  2000/09/29 18:06:39  lewis
 *	cleanup first cut at mac context menu code, and did first cut at PC context menu code -
 *	still less than halfway done
 *	
 *	Revision 1.3  2000/09/29 00:18:16  lewis
 *	Add kEditMenu for macos - and macos flags
 *	
 *	Revision 1.2  2000/09/28 21:03:39  lewis
 *	Create separeate LedItView module, and move stuff in there, and start cloning/copying
 *	stuff from ActiveLedIt!
 *	
 *
 *
 *
 *
 */
#include	"NetLedItConfig.h"



#define	kContextMenu	203

#define	kAcceleratorTableID				301


#if		qWindows
#define IDI_ABOUTDLL                    1
#endif

#if		qMacOS
#define	cmd_StyleMenu			252
#define	cmd_ColorMenu			253
#define	cmd_JustificationMenu	254
#define	cmd_FontMenu			250
#define	cmd_SizeMenu			251
#endif


#if		!qURLStyleMarkerNewDisplayMode
#define	kURLPictID						1001
#endif
#define	kUnknownEmbeddingPictID			1002
#define	kUnsupportedPICTFormatPictID	1003
#define	kUnsupportedDIBFormatPictID		5003




// MFC must define something like this someplace, but I haven't found where....
// Use this for now, so I can update things more easily when I find the MFC definition...
// PowerPlant/Gtk don't seem picky about this number - same # works in both places - but it would be OK to have
// this differ across platforms.
// NOTE: MFC commands tend to be in the range:
//		0xE100 (ID_FILE_NEW) ... 0xEffff
#define	kUserCommandBase			0x5000

#define	kFileCmdBase				(kUserCommandBase+0x1000)
#define	kEditCmdBase				(kUserCommandBase+0x2000)
#define	kInsertCmdBase				(kUserCommandBase+0x2a00)
#define	kFormatCmdBase				(kUserCommandBase+0x3000)
#define	kMiscCmdBase				(kUserCommandBase+0x4000)



// Global command numbers


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
#ifndef	kHideSelectionCmd
#define	kHideSelectionCmd				(kEditCmdBase+11)
#endif
#ifndef	kUnHideSelectionCmd
#define	kUnHideSelectionCmd				(kEditCmdBase+12)
#endif

#ifndef	kFirstSelectedEmbeddingCmd
#define	kFirstSelectedEmbeddingCmd		(kEditCmdBase+20)
#endif
	#ifndef	kPropertiesForSelectionCmd
	#define	kPropertiesForSelectionCmd		kFirstSelectedEmbeddingCmd
	#endif
	#ifndef	kFirstPrivateEmbeddingCmd
	#define	kFirstPrivateEmbeddingCmd		kFirstSelectedEmbeddingCmd+1
	#endif
#ifndef	kLastSelectedEmbeddingCmd
#define	kLastSelectedEmbeddingCmd		(kEditCmdBase+30)
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

// Options - soon to be replaced with an prefs dialog...
#define	kToggleUseSmartCutNPasteCmd		kUserCommandBase+3
#define	kToggleWrapToWindowCmd			kUserCommandBase+4
#define	kToggleShowHiddenTextCmd		kUserCommandBase+5
#define	kFirstShowHideGlyphCmd			kUserCommandBase+6
	#define	kShowHideParagraphGlyphsCmd		kFirstShowHideGlyphCmd+0
	#define	kShowHideTabGlyphsCmd			kFirstShowHideGlyphCmd+1
	#define	kShowHideSpaceGlyphsCmd			kFirstShowHideGlyphCmd+2
#define	kLastShowHideGlyphCmd			kShowHideSpaceGlyphsCmd


#define	cmdChooseDefaultFontDialog		(kUserCommandBase+25)


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
#endif

#define	ID_HELP			111




// Dialog IDs, and their dialog items...

#define kUnknownExceptionOnCmdDialogID	1013

#define kBadAllocExceptionOnCmdDialogID	1014

#define kBadUserInputExceptionOnCmdDialogID	1015



#if		qDemoMode
#define	kDemoExpired_DialogID			2001
#endif


#endif	/*__Resource_h__*/
