/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef __LedLineItResourceIDs_h__
#define __LedLineItResourceIDs_h__ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <afxres.h>

#include "LedLineItConfig.h"

// MFC must define something like this someplace, but I haven't found where....
// Use this for now, so I can update things more easily when I find the MFC definition...
#define kUserCommandBase 0x1000

#define IDR_MAINFRAME 128

#define IDR_SRVR_INPLACE 4
#define IDR_SRVR_EMBEDDED 5
#define IDR_CNTR_INPLACE 6
#define IDP_OLE_INIT_FAILED 100
#define IDP_FAILED_TO_CREATE 102
#define ID_CANCEL_EDIT_CNTR 32768
#define ID_CANCEL_EDIT_SRVR 32769

#define ID_LINENUMBER 999

// Menus
#define kContextMenu 32312

#define kUnknownExceptionOnCmdDialogID 1003

#define kGotoLine_DialogID 1006
#define kGotoLine_Dialog_LineNumberEditFieldID 2303

#define kBadAllocExceptionOnCmdDialogID 1007

#define kBadUserInputExceptionOnCmdDialogID 1008

#define kLineTooLongOnRead_DialogID 1009
#define kLineTooLongOnRead_Dialog_MessageFieldID 2303
#define kLineTooLongOnRead_Dialog_BreakNumFieldID 2304

#define kFileDialogAddOnID 2001
#define kFileDialog_EncodingComboBox 1001

// What is the right set of valid number ranges for my commands/??

#define kFileCmdBase (kUserCommandBase + 0x100)
#define kEditCmdBase (kUserCommandBase + 0x200)
#define kFormatCmdBase (kUserCommandBase + 0x300)

#define kFindCmd ID_EDIT_FIND
#define kFindAgainCmd (kEditCmdBase + 1)
#define kEnterFindStringCmd (kEditCmdBase + 2)
#define kReplaceCmd ID_EDIT_REPLACE
#define kReplaceAgainCmd (kEditCmdBase + 3)
#define kGotoLineCmdID (kEditCmdBase + 4)
#define kShiftLeftCmdID (kEditCmdBase + 5)
#define kShiftRightCmdID (kEditCmdBase + 6)
#ifndef kSpellCheckCmd
#define kSpellCheckCmd (kEditCmdBase + 7)
#endif

#define cmdChooseFontDialog (kFormatCmdBase + 1)
#define cmdChooseDefaultFontDialog (kFormatCmdBase + 2)

#define cmdFontMenuFirst (kFormatCmdBase + 0x100)
#define cmdFontMenuLast (cmdFontMenuFirst + 0x100)

#define kBaseFontSizeCmdID cmdFontMenuLast + 1
#define kFontSize9CmdID kBaseFontSizeCmdID
#define kFontSize10CmdID (kFontSize9CmdID + 1)
#define kFontSize12CmdID (kFontSize9CmdID + 2)
#define kFontSize14CmdID (kFontSize9CmdID + 3)
#define kFontSize18CmdID (kFontSize9CmdID + 4)
#define kFontSize24CmdID (kFontSize9CmdID + 5)
#define kFontSize36CmdID (kFontSize9CmdID + 6)
#define kFontSize48CmdID (kFontSize9CmdID + 7)
#define kFontSize72CmdID (kFontSize9CmdID + 8)
#define kFontSizeOtherCmdID (kFontSize9CmdID + 9)
#define kFontSizeSmallerCmdID (kFontSize9CmdID + 10)
#define kFontSizeLargerCmdID (kFontSize9CmdID + 11)
#define kLastFontSizeCmdID kFontSizeLargerCmdID

#define cmdFontStylePlain (kLastFontSizeCmdID + 1)
#define cmdFontStyleBold (kLastFontSizeCmdID + 2)
#define cmdFontStyleItalic (kLastFontSizeCmdID + 3)
#define cmdFontStyleUnderline (kLastFontSizeCmdID + 4)
#define cmdFontStyleStrikeout (kLastFontSizeCmdID + 5)

// Options - soon to be replaced with an prefs dialog...
#define kToggleUseSmartCutNPasteCmdID kUserCommandBase + 0x1002
#define kToggleAutoIndentOptionCmd kUserCommandBase + 0x2000
#define kToggleTreatTabAsIndentCharOptionCmd kUserCommandBase + 0x2101
#define kNoSyntaxColoringCmd kUserCommandBase + 0x2102
#define kCPlusPlusSyntaxColoringCmd kUserCommandBase + 0x2103
#define kVBSyntaxColoringCmd kUserCommandBase + 0x2104

#define kGotoLedLineItWebPageCmdID kUserCommandBase + 0x3001
#define kCheckForUpdatesWebPageCmdID kUserCommandBase + 0x3002
#define kGotoSophistsWebPageCmdID kUserCommandBase + 0x3003

#define kCloseWindowCmdID kUserCommandBase + 0x4001
#define kCloseAllWindowsCmdID kUserCommandBase + 0x4002

#if qSupportGenRandomCombosCommand
#define kGenRandomCombosCmdID kUserCommandBase + 0x4003
#endif

#endif /*__LedLineItResourceIDs_h__*/
