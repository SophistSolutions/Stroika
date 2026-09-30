/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#ifndef __LedItResourceIDs_h__
#define __LedItResourceIDs_h__ 1

#if defined(WIN32)
#include <afxres.h>
#endif

#include "LedItConfig.h"

// MFC must define something like this someplace, but I haven't found where....
// Use this for now, so I can update things more easily when I find the MFC definition...
// NOTE: MFC commands tend to be in the range:
//      0xE100 (ID_FILE_NEW) ... 0xEffff
#define kUserCommandBase 0x1000

#define kFileCmdBase (kUserCommandBase + 0x1000)
#define kEditCmdBase (kUserCommandBase + 0x1100)
#define kSelectCmdBase (kUserCommandBase + 0x1200)
#define kInsertCmdBase (kUserCommandBase + 0x1300)
#define kFormatCmdBase (kUserCommandBase + 0x2000)
#define kRemoveCmdBase (kUserCommandBase + 0x3000)
#define kMiscCmdBase (kUserCommandBase + 0x3100)

// Global command numbers
#if qStroika_Platform_Windows
#define kFindCmd ID_EDIT_FIND
#define kReplaceCmd ID_EDIT_REPLACE
#define kCmdUndo ID_EDIT_UNDO
#define kCmdRedo ID_EDIT_REDO
#define kCmdSelectAll ID_EDIT_SELECT_ALL
#define kCmdCut ID_EDIT_CUT
#define kCmdCopy ID_EDIT_COPY
#define kCmdPaste ID_EDIT_PASTE
#define kCmdClear ID_EDIT_CLEAR
#endif

#ifndef kCmdSaveACopyAs
#define kCmdSaveACopyAs (kFileCmdBase + 1)
#endif

#ifndef kFindCmd
#define kFindCmd (kEditCmdBase + 1)
#endif
#ifndef kFindAgainCmd
#define kFindAgainCmd (kEditCmdBase + 2)
#endif
#ifndef kEnterFindStringCmd
#define kEnterFindStringCmd (kEditCmdBase + 3)
#endif
#ifndef kCmdUndo
#define kCmdUndo (kEditCmdBase + 4)
#endif
#ifndef kCmdRedo
#define kCmdRedo (kEditCmdBase + 5)
#endif
#ifndef kCmdSelectAll
#define kCmdSelectAll (kEditCmdBase + 6)
#endif
#ifndef kCmdCut
#define kCmdCut (kEditCmdBase + 7)
#endif
#ifndef kCmdCopy
#define kCmdCopy (kEditCmdBase + 8)
#endif
#ifndef kCmdPaste
#define kCmdPaste (kEditCmdBase + 9)
#endif
#ifndef kCmdClear
#define kCmdClear (kEditCmdBase + 10)
#endif
#ifndef kReplaceCmd
#define kReplaceCmd (kEditCmdBase + 11)
#endif
#ifndef kSpellCheckCmd
#define kSpellCheckCmd (kEditCmdBase + 12)
#endif

#define kReplaceAgainCmd (kEditCmdBase + 13)

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

#ifndef kChooseFontDialogCmd
#define kChooseFontDialogCmd (kFormatCmdBase + 11)
#endif

#ifndef kBaseFontNameCmd
#define kBaseFontNameCmd (kFormatCmdBase + 0x100)
#endif
#ifndef kLastFontNameCmd
#define kLastFontNameCmd (kFormatCmdBase + 0x500)
#endif

#ifndef kBaseFontSizeCmd
#define kBaseFontSizeCmd (kFormatCmdBase + 0x600)
#endif
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

#ifndef kBaseFontColorCmd
#define kBaseFontColorCmd (kFormatCmdBase + 0x700)
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

#ifndef kFirstFontStyleCmd
#define kFirstFontStyleCmd (kFormatCmdBase + 0x800)
#endif
#ifndef kFontStylePlainCmd
#define kFontStylePlainCmd (kFirstFontStyleCmd + 0)
#endif
#ifndef kFontStyleBoldCmd
#define kFontStyleBoldCmd (kFirstFontStyleCmd + 1)
#endif
#ifndef kFontStyleItalicCmd
#define kFontStyleItalicCmd (kFirstFontStyleCmd + 2)
#endif
#ifndef kFontStyleUnderlineCmd
#define kFontStyleUnderlineCmd (kFirstFontStyleCmd + 3)
#endif
#ifndef kFontStyleOutlineCmd
#define kFontStyleOutlineCmd (kFirstFontStyleCmd + 4)
#endif
#ifndef kFontStyleShadowCmd
#define kFontStyleShadowCmd (kFirstFontStyleCmd + 5)
#endif
#ifndef kFontStyleCondensedCmd
#define kFontStyleCondensedCmd (kFirstFontStyleCmd + 6)
#endif
#ifndef kFontStyleExtendedCmd
#define kFontStyleExtendedCmd (kFirstFontStyleCmd + 7)
#endif
#ifndef kFontStyleStrikeoutCmd
#define kFontStyleStrikeoutCmd (kFirstFontStyleCmd + 8)
#endif
#ifndef kSubScriptCmd
#define kSubScriptCmd (kFirstFontStyleCmd + 9)
#endif
#ifndef kSuperScriptCmd
#define kSuperScriptCmd (kFirstFontStyleCmd + 10)
#endif

#define kFirstJustificationCmd (kFormatCmdBase + 0x900)
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

#ifndef kHideSelectionCmd
#define kHideSelectionCmd (kRemoveCmdBase + 1)
#endif
#ifndef kUnHideSelectionCmd
#define kUnHideSelectionCmd (kRemoveCmdBase + 2)
#endif
#ifndef kRemoveTableRowsCmd
#define kRemoveTableRowsCmd (kRemoveCmdBase + 3)
#endif
#ifndef kRemoveTableColumnsCmd
#define kRemoveTableColumnsCmd (kRemoveCmdBase + 4)
#endif

// Options - soon to be replaced with an prefs dialog...
#define kToggleUseSmartCutNPasteCmd kMiscCmdBase + 10
#define kToggleWrapToWindowCmd kMiscCmdBase + 11
#define kToggleShowHiddenTextCmd kMiscCmdBase + 12
#define kFirstShowHideGlyphCmd kMiscCmdBase + 13
#define kShowHideParagraphGlyphsCmd kFirstShowHideGlyphCmd + 0
#define kShowHideTabGlyphsCmd kFirstShowHideGlyphCmd + 1
#define kShowHideSpaceGlyphsCmd kFirstShowHideGlyphCmd + 2
#define kLastShowHideGlyphCmd kShowHideSpaceGlyphsCmd

#define cmdChooseDefaultFontDialog (kMiscCmdBase + 25)

#ifndef kAboutBoxCmd
#define kAboutBoxCmd kMiscCmdBase + 1
#endif
#ifndef kGotoLedItWebPageCmd
#define kGotoLedItWebPageCmd kMiscCmdBase + 2
#endif
#ifndef kGotoSophistsWebPageCmd
#define kGotoSophistsWebPageCmd kMiscCmdBase + 3
#endif
#ifndef kCheckForUpdatesWebPageCmdID
#define kCheckForUpdatesWebPageCmdID kMiscCmdBase + 4
#endif

// Command Numbers

#if qStroika_Platform_Windows

#define IDR_MAINFRAME 128
#define kLedItDocumentIconID 129
#define kLedItRTFDocumentIconID 130

#define IDR_SRVR_INPLACE 4
#define IDR_SRVR_EMBEDDED 5
#define IDR_CNTR_INPLACE 6
#define IDP_OLE_INIT_FAILED 100
#define IDP_FAILED_TO_CREATE 102
#define ID_CANCEL_EDIT_CNTR 32768
#define ID_CANCEL_EDIT_SRVR 32769

//FormatBar stuff
#define IDC_FONTSIZE 166
#define IDC_FONTNAME 167
#define IDB_FONTTYPE 168
#define ID_CHAR_COLOR 32801
#define IDB_FORMATBAR 139
#define IDS_TITLE_FORMATBAR 1167
#define ID_VIEW_FORMATBAR 59396

//RulerBar stuff
#define IDB_RULER_BLOCK 131
#define IDB_RULER_BLOCKM 132
#define IDB_RULER_UP 137
#define IDB_RULER_UPM 138
#define IDB_RULER_DOWN 133
#define IDB_RULER_DOWNM 134
#define IDB_RULER_TAB 135
#define IDB_RULER_TABM 136
#define ID_VIEW_RULER 59397

// Menus
#define kContextMenu 32312

#define kUnknownExceptionOnCmdDialogID 1003

#define kBadAllocExceptionOnCmdDialogID 1007

#define kBadUserInputExceptionOnCmdDialogID 1008

#define kUnknownEmbeddingPictID 1002
#define kUnsupportedPICTFormatPictID 1003

#endif

#endif /*__LedItResourceIDs_h__*/
