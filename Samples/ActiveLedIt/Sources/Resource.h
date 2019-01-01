/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef __Resource_h__
#define __Resource_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "ActiveLedItConfig.h"

#define IDS_ACTIVELEDIT 1
#define IDD_ABOUTBOX_ACTIVELEDIT 1
#define IDB_ACTIVELEDIT 1
#define IDI_ABOUTDLL 1
#define IDS_ACTIVELEDIT_PPG 2
#define IDS_ACTIVELEDIT_PPG_CAPTION 200
#define IDD_PROPPAGE_ACTIVELEDIT 200
#define kShowVerticalScrollbarID 201
#define kShowHorizontalScrollbarID 202

#if !qURLStyleMarkerNewDisplayMode
#define kURLPictID 1001
#endif
#define kUnknownEmbeddingPictID 1002
#define kUnsupportedPICTFormatPictID 1003

#define kToolbarButton_Cut_ResID 2001
#define kToolbarButton_Copy_ResID 2002
#define kToolbarButton_Paste_ResID 2003
#define kToolbarButton_Undo_ResID 2004
#define kToolbarButton_Redo_ResID 2005
#define kToolbarButton_Print_ResID 2006
#define kToolbarButton_Bold_ResID 2007
#define kToolbarButton_Italics_ResID 2008
#define kToolbarButton_Underline_ResID 2009
#define kToolbarButton_FontColor_ResID 2010
#define kToolbarButton_JustifyLeft_ResID 2011
#define kToolbarButton_JustifyCenter_ResID 2012
#define kToolbarButton_JustifyRight_ResID 2013
#define kToolbarButton_JustifyFull_ResID 2014
#define kToolbarButton_ListStyle_None_ResID 2015
#define kToolbarButton_ListStyle_Bullet_ResID 2016
#define kToolbarButton_Find_ResID 2017
#define kToolbarButton_Replace_ResID 2018
#define kToolbarButton_CheckSpelling_ResID 2019
#define kToolbarButton_ActiveLedIt_ResID 2020
#define kToolbarButton_ParagraphSpacing_ResID 2021
#define kToolbarButton_ParagraphIndents_ResID 2022
#define kToolbarButton_InsertTable_ResID 2023
#define kToolbarButton_InsertURL_ResID 2024
#define kToolbarButton_InsertSymbol_ResID 2025

// MFC must define something like this someplace, but I haven't found where....
// Use this for now, so I can update things more easily when I find the MFC definition...
#define kUserCommandBase 0x2000

#define kEditCmdBase (kUserCommandBase + 0x1100)
#define kSelectCmdBase (kUserCommandBase + 0x1200)
#define kInsertCmdBase (kUserCommandBase + 0x1300)
#define kFormatCmdBase (kUserCommandBase + 0x2000)
#define kRemoveCmdBase (kUserCommandBase + 0x3000)
#define kMiscCmdBase (kUserCommandBase + 0x3100)

#define kFindCmd ID_EDIT_FIND
#define kFindAgainCmd (kEditCmdBase + 1)
#define kEnterFindStringCmd (kEditCmdBase + 2)
#define kReplaceCmd ID_EDIT_REPLACE
#define kReplaceAgainCmd (kEditCmdBase + 3)

#ifndef kSpellCheckCmd
#define kSpellCheckCmd (kEditCmdBase + 12)
#endif

#ifndef kFirstSelectedEmbeddingCmd
#define kFirstSelectedEmbeddingCmd (kEditCmdBase + 20)
#endif
#ifndef kPropertiesForSelectionCmd
#define kPropertiesForSelectionCmd kFirstSelectedEmbeddingCmd
#endif
#ifndef kFirstPrivateEmbeddingCmd
#define kFirstPrivateEmbeddingCmd (kFirstSelectedEmbeddingCmd + 1)
#endif
#ifndef kLastPrivateEmbeddingCmd
#define kLastPrivateEmbeddingCmd (kFirstPrivateEmbeddingCmd + 5)
#endif
// # here is syncronized with value in WordProcessor::kLastSelectedEmbedding_CmdID
#ifndef kLastSelectedEmbeddingCmd
#define kLastSelectedEmbeddingCmd kLastPrivateEmbeddingCmd
#endif

#ifndef kSelectWordCmd
#define kSelectWordCmd (kSelectCmdBase + 1)
#endif

#ifndef kSelectTextRowCmd
#define kSelectTextRowCmd (kSelectCmdBase + 2)
#endif

#ifndef kSelectParagraphCmd
#define kSelectParagraphCmd (kSelectCmdBase + 3)
#endif

#ifndef kSelectTableIntraCellAllCmd
#define kSelectTableIntraCellAllCmd (kSelectCmdBase + 4)
#endif

#ifndef kSelectTableCellCmd
#define kSelectTableCellCmd (kSelectCmdBase + 5)
#endif

#ifndef kSelectTableRowCmd
#define kSelectTableRowCmd (kSelectCmdBase + 6)
#endif

#ifndef kSelectTableColumnCmd
#define kSelectTableColumnCmd (kSelectCmdBase + 7)
#endif

#ifndef kSelectTableCmd
#define kSelectTableCmd (kSelectCmdBase + 8)
#endif

#ifndef kInsertTableCmd
#define kInsertTableCmd (kInsertCmdBase + 1)
#endif

#ifndef kInsertTableRowAboveCmd
#define kInsertTableRowAboveCmd (kInsertCmdBase + 2)
#endif

#ifndef kInsertTableRowBelowCmd
#define kInsertTableRowBelowCmd (kInsertCmdBase + 3)
#endif

#ifndef kInsertTableColBeforeCmd
#define kInsertTableColBeforeCmd (kInsertCmdBase + 4)
#endif

#ifndef kInsertTableColAfterCmd
#define kInsertTableColAfterCmd (kInsertCmdBase + 5)
#endif

#ifndef kInsertURLCmd
#define kInsertURLCmd (kInsertCmdBase + 6)
#endif

#ifndef kInsertSymbolCmd
#define kInsertSymbolCmd (kInsertCmdBase + 7)
#endif

#define kChooseFontDialogCmd (kFormatCmdBase + 1)

#define kBaseFontNameCmd (kFormatCmdBase + 0x100)
#define kLastFontNameCmd (kBaseFontNameCmd + 0x400)

#define kBaseFontSizeCmd kLastFontNameCmd + 1
#define kFontSize9Cmd kBaseFontSizeCmd
#define kFontSize10Cmd (kFontSize9Cmd + 1)
#define kFontSize12Cmd (kFontSize9Cmd + 2)
#define kFontSize14Cmd (kFontSize9Cmd + 3)
#define kFontSize18Cmd (kFontSize9Cmd + 4)
#define kFontSize24Cmd (kFontSize9Cmd + 5)
#define kFontSize36Cmd (kFontSize9Cmd + 6)
#define kFontSize48Cmd (kFontSize9Cmd + 7)
#define kFontSize72Cmd (kFontSize9Cmd + 8)
#define kFontSizeOtherCmd (kFontSize9Cmd + 9)
#define kFontSizeSmallerCmd (kFontSize9Cmd + 10)
#define kFontSizeLargerCmd (kFontSize9Cmd + 11)
#define kLastFontSizeCmd kFontSizeLargerCmd

#define kFontStylePlainCmd (kLastFontSizeCmd + 1)
#define kFontStyleBoldCmd (kLastFontSizeCmd + 2)
#define kFontStyleItalicCmd (kLastFontSizeCmd + 3)
#define kFontStyleUnderlineCmd (kLastFontSizeCmd + 4)
#define kFontStyleStrikeoutCmd (kLastFontSizeCmd + 5)
#define kSubScriptCmd (kLastFontSizeCmd + 6)
#define kSuperScriptCmd (kLastFontSizeCmd + 7)

#ifndef kBaseFontColorCmd
#define kBaseFontColorCmd (kSuperScriptCmd + 100)
#endif
#define kBlackColorCmd (kBaseFontColorCmd + 0)
#define kMaroonColorCmd (kBaseFontColorCmd + 1)
#define kGreenColorCmd (kBaseFontColorCmd + 2)
#define kOliveColorCmd (kBaseFontColorCmd + 3)
#define kNavyColorCmd (kBaseFontColorCmd + 4)
#define kPurpleColorCmd (kBaseFontColorCmd + 5)
#define kTealColorCmd (kBaseFontColorCmd + 6)
#define kGrayColorCmd (kBaseFontColorCmd + 7)
#define kSilverColorCmd (kBaseFontColorCmd + 8)
#define kRedColorCmd (kBaseFontColorCmd + 9)
#define kLimeColorCmd (kBaseFontColorCmd + 10)
#define kYellowColorCmd (kBaseFontColorCmd + 11)
#define kBlueColorCmd (kBaseFontColorCmd + 12)
#define kFuchsiaColorCmd (kBaseFontColorCmd + 13)
#define kAquaColorCmd (kBaseFontColorCmd + 14)
#define kWhiteColorCmd (kBaseFontColorCmd + 15)
#define kFontColorOtherCmd (kBaseFontColorCmd + 16)
#define kLastFontNamedColorCmd kWhiteColorCmd

#define kFirstJustificationCmd (kLastFontNamedColorCmd + 0x10)
#define kJustifyLeftCmd kFirstJustificationCmd + 0
#define kJustifyCenterCmd kFirstJustificationCmd + 1
#define kJustifyRightCmd kFirstJustificationCmd + 2
#define kJustifyFullCmd kFirstJustificationCmd + 3
#define kLastJustificationCmd kJustifyFullCmd

#define kParagraphSpacingCmd kLastJustificationCmd + 1
#define kParagraphIndentsCmd kLastJustificationCmd + 2

#define kListStyle_NoneCmd (kFormatCmdBase + 0xa00)
#define kListStyle_BulletCmd (kFormatCmdBase + 0xa00) + 1

#define kIncreaseIndentCmd (kFormatCmdBase + 0xb00)
#define kDecreaseIndentCmd (kFormatCmdBase + 0xb00) + 1

#ifndef kRemoveTableRowsCmd
#define kRemoveTableRowsCmd (kRemoveCmdBase + 3)
#endif
#ifndef kRemoveTableColumnsCmd
#define kRemoveTableColumnsCmd (kRemoveCmdBase + 4)
#endif

#define kGotoLedItWebPageCmdID kUserCommandBase + 0x1000
#define kGotoSophistsWebPageCmdID kUserCommandBase + 0x1001
#define kCheckForUpdatesWebPageCmdID kUserCommandBase + 0x1007
#define kPasteFormatTextCmdID kUserCommandBase + 0x1003

// Options - soon to be replaced with an prefs dialog...
#define kToggleUseSmartCutNPasteCmdID kUserCommandBase + 0x1050

#define kFirstOLEUserCmdCmdID kUserCommandBase + 0x4000
#define kLastOLEUserCmdCmdID kFirstOLEUserCmdCmdID + 0xfff

#if qDemoMode
#define kDemoExpired_DialogID 2001
#define kDemoExpired_Dialog_CancelFieldID 2301
#endif

// Next default values for new objects
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE 204
#define _APS_NEXT_COMMAND_VALUE 32768
#define _APS_NEXT_CONTROL_VALUE 204
#define _APS_NEXT_SYMED_VALUE 101
#endif
#endif

#endif /*__Resource_h__*/
