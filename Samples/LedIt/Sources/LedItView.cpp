/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#if qPlatform_MacOS
#include <AERegistry.h>
#include <ColorPicker.h>
#include <Dialogs.h>
#include <TextUtils.h>

#include <LDialogBox.h>
#include <LEditField.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <LStdControl.h>
#include <PP_Messages.h>
#include <UAppleEventsMgr.h>
#include <UModalDialogs.h>
#elif qPlatform_Windows
#include <afxodlgs.h> // MFC OLE dialog classes
#endif

#include "Stroika/Frameworks/Led/StdDialogs.h"

#include "ColorMenu.h"
#if qPlatform_Windows
#include "LedItControlItem.h"
#endif
#include "LedItDocument.h"
#include "LedItResources.h"
#include "Options.h"

#include "LedItApplication.h"

#include "LedItView.h"

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(4 : 4800) //qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;
using namespace Stroika::Frameworks::Led::StyledTextIO;

class My_CMDNUM_MAPPING : public
#if qPlatform_Windows
                          Platform::MFC_CommandNumberMapping
#elif qPlatform_MacOS
                          Platform::PP_CommandNumberMapping
#elif qXWindows
                          Platform::Gtk_CommandNumberMapping
#endif
{
public:
    My_CMDNUM_MAPPING ()
    {
        AddAssociation (kCmdUndo, LedItView::kUndo_CmdID);
        AddAssociation (kCmdRedo, LedItView::kRedo_CmdID);
        AddAssociation (kCmdSelectAll, LedItView::kSelectAll_CmdID);
        AddAssociation (kCmdCut, LedItView::kCut_CmdID);
        AddAssociation (kCmdCopy, LedItView::kCopy_CmdID);
        AddAssociation (kCmdPaste, LedItView::kPaste_CmdID);
        AddAssociation (kCmdClear, LedItView::kClear_CmdID);
        AddAssociation (kFindCmd, LedItView::kFind_CmdID);
        AddAssociation (kFindAgainCmd, LedItView::kFindAgain_CmdID);
        AddAssociation (kEnterFindStringCmd, LedItView::kEnterFindString_CmdID);
        AddAssociation (kReplaceCmd, LedItView::kReplace_CmdID);
        AddAssociation (kReplaceAgainCmd, LedItView::kReplaceAgain_CmdID);
#if qIncludeBakedInDictionaries
        // If we have no dictionaries - assume no spellcheck command should be enabled (mapped)
        AddAssociation (kSpellCheckCmd, LedItView::kSpellCheck_CmdID);
#endif
        AddAssociation (kSelectWordCmd, LedItView::kSelectWord_CmdID);
        AddAssociation (kSelectTextRowCmd, LedItView::kSelectTextRow_CmdID);
        AddAssociation (kSelectParagraphCmd, LedItView::kSelectParagraph_CmdID);
        AddAssociation (kSelectTableIntraCellAllCmd, LedItView::kSelectTableIntraCellAll_CmdID);
        AddAssociation (kSelectTableCellCmd, LedItView::kSelectTableCell_CmdID);
        AddAssociation (kSelectTableRowCmd, LedItView::kSelectTableRow_CmdID);
        AddAssociation (kSelectTableColumnCmd, LedItView::kSelectTableColumn_CmdID);
        AddAssociation (kSelectTableCmd, LedItView::kSelectTable_CmdID);

        AddAssociation (kFontSize9Cmd, LedItView::kFontSize9_CmdID);
        AddAssociation (kFontSize10Cmd, LedItView::kFontSize10_CmdID);
        AddAssociation (kFontSize12Cmd, LedItView::kFontSize12_CmdID);
        AddAssociation (kFontSize14Cmd, LedItView::kFontSize14_CmdID);
        AddAssociation (kFontSize18Cmd, LedItView::kFontSize18_CmdID);
        AddAssociation (kFontSize24Cmd, LedItView::kFontSize24_CmdID);
        AddAssociation (kFontSize36Cmd, LedItView::kFontSize36_CmdID);
        AddAssociation (kFontSize48Cmd, LedItView::kFontSize48_CmdID);
        AddAssociation (kFontSize72Cmd, LedItView::kFontSize72_CmdID);
#if qSupportOtherFontSizeDlg
        AddAssociation (kFontSizeOtherCmd, LedItView::kFontSizeOther_CmdID);
#endif
        AddAssociation (kFontSizeSmallerCmd, LedItView::kFontSizeSmaller_CmdID);
        AddAssociation (kFontSizeLargerCmd, LedItView::kFontSizeLarger_CmdID);

        AddAssociation (kBlackColorCmd, LedItView::kFontColorBlack_CmdID);
        AddAssociation (kMaroonColorCmd, LedItView::kFontColorMaroon_CmdID);
        AddAssociation (kGreenColorCmd, LedItView::kFontColorGreen_CmdID);
        AddAssociation (kOliveColorCmd, LedItView::kFontColorOlive_CmdID);
        AddAssociation (kNavyColorCmd, LedItView::kFontColorNavy_CmdID);
        AddAssociation (kPurpleColorCmd, LedItView::kFontColorPurple_CmdID);
        AddAssociation (kTealColorCmd, LedItView::kFontColorTeal_CmdID);
        AddAssociation (kGrayColorCmd, LedItView::kFontColorGray_CmdID);
        AddAssociation (kSilverColorCmd, LedItView::kFontColorSilver_CmdID);
        AddAssociation (kRedColorCmd, LedItView::kFontColorRed_CmdID);
        AddAssociation (kLimeColorCmd, LedItView::kFontColorLime_CmdID);
        AddAssociation (kYellowColorCmd, LedItView::kFontColorYellow_CmdID);
        AddAssociation (kBlueColorCmd, LedItView::kFontColorBlue_CmdID);
        AddAssociation (kFuchsiaColorCmd, LedItView::kFontColorFuchsia_CmdID);
        AddAssociation (kAquaColorCmd, LedItView::kFontColorAqua_CmdID);
        AddAssociation (kWhiteColorCmd, LedItView::kFontColorWhite_CmdID);
        AddAssociation (kFontColorOtherCmd, LedItView::kFontColorOther_CmdID);

        AddAssociation (kJustifyLeftCmd, LedItView::kJustifyLeft_CmdID);
        AddAssociation (kJustifyCenterCmd, LedItView::kJustifyCenter_CmdID);
        AddAssociation (kJustifyRightCmd, LedItView::kJustifyRight_CmdID);
        AddAssociation (kJustifyFullCmd, LedItView::kJustifyFull_CmdID);

#if qSupportParagraphSpacingDlg
        AddAssociation (kParagraphSpacingCmd, LedItView::kParagraphSpacingCommand_CmdID);
#endif
#if qSupportParagraphIndentsDlg
        AddAssociation (kParagraphIndentsCmd, LedItView::kParagraphIndentsCommand_CmdID);
#endif

        AddAssociation (kListStyle_NoneCmd, LedItView::kListStyle_None_CmdID);
        AddAssociation (kListStyle_BulletCmd, LedItView::kListStyle_Bullet_CmdID);

        AddAssociation (kIncreaseIndentCmd, LedItView::kIncreaseIndent_CmdID);
        AddAssociation (kDecreaseIndentCmd, LedItView::kDecreaseIndent_CmdID);

        AddRangeAssociation (
            kBaseFontNameCmd, kLastFontNameCmd,
            LedItView::kFontMenuFirst_CmdID, LedItView::kFontMenuLast_CmdID);

        AddAssociation (kFontStylePlainCmd, LedItView::kFontStylePlain_CmdID);
        AddAssociation (kFontStyleBoldCmd, LedItView::kFontStyleBold_CmdID);
        AddAssociation (kFontStyleItalicCmd, LedItView::kFontStyleItalic_CmdID);
        AddAssociation (kFontStyleUnderlineCmd, LedItView::kFontStyleUnderline_CmdID);
#if qPlatform_MacOS
        AddAssociation (kFontStyleOutlineCmd, LedItView::kFontStyleOutline_CmdID);
        AddAssociation (kFontStyleShadowCmd, LedItView::kFontStyleShadow_CmdID);
        AddAssociation (kFontStyleCondensedCmd, LedItView::kFontStyleCondensed_CmdID);
        AddAssociation (kFontStyleExtendedCmd, LedItView::kFontStyleExtended_CmdID);
#endif
#if qPlatform_Windows
        AddAssociation (kFontStyleStrikeoutCmd, LedItView::kFontStyleStrikeout_CmdID);
#endif
        AddAssociation (kSubScriptCmd, LedItView::kSubScriptCommand_CmdID);
        AddAssociation (kSuperScriptCmd, LedItView::kSuperScriptCommand_CmdID);
#if qPlatform_Windows || qXWindows
        AddAssociation (kChooseFontDialogCmd, LedItView::kChooseFontCommand_CmdID);
#endif

        AddAssociation (kInsertTableCmd, LedItView::kInsertTable_CmdID);
        AddAssociation (kInsertTableRowAboveCmd, LedItView::kInsertTableRowAbove_CmdID);
        AddAssociation (kInsertTableRowBelowCmd, LedItView::kInsertTableRowBelow_CmdID);
        AddAssociation (kInsertTableColBeforeCmd, LedItView::kInsertTableColBefore_CmdID);
        AddAssociation (kInsertTableColAfterCmd, LedItView::kInsertTableColAfter_CmdID);
        AddAssociation (kInsertURLCmd, LedItView::kInsertURL_CmdID);
#if qPlatform_Windows
        AddAssociation (kInsertSymbolCmd, LedItView::kInsertSymbol_CmdID);
#endif

        //              AddAssociation (kPropertiesForSelectionCmd,     LedItView::kSelectedEmbeddingProperties_CmdID);
        AddRangeAssociation (
            kFirstSelectedEmbeddingCmd, kLastSelectedEmbeddingCmd,
            LedItView::kFirstSelectedEmbedding_CmdID, LedItView::kLastSelectedEmbedding_CmdID);

        AddAssociation (kHideSelectionCmd, LedItView::kHideSelection_CmdID);
        AddAssociation (kUnHideSelectionCmd, LedItView::kUnHideSelection_CmdID);
        AddAssociation (kRemoveTableRowsCmd, LedItView::kRemoveTableRows_CmdID);
        AddAssociation (kRemoveTableColumnsCmd, LedItView::kRemoveTableColumns_CmdID);

        AddAssociation (kShowHideParagraphGlyphsCmd, LedItView::kShowHideParagraphGlyphs_CmdID);
        AddAssociation (kShowHideTabGlyphsCmd, LedItView::kShowHideTabGlyphs_CmdID);
        AddAssociation (kShowHideSpaceGlyphsCmd, LedItView::kShowHideSpaceGlyphs_CmdID);

#if qPlatform_Windows
        AddAssociation (IDC_FONTSIZE, IDC_FONTSIZE);
        AddAssociation (IDC_FONTNAME, IDC_FONTNAME);
#endif
    }
};
My_CMDNUM_MAPPING sMy_CMDNUM_MAPPING;

struct LedIt_DialogSupport : TextInteractor::DialogSupport, WordProcessor::DialogSupport {
public:
    using CommandNumber = TextInteractor::DialogSupport::CommandNumber;

public:
    LedIt_DialogSupport ()
    {
        TextInteractor::SetDialogSupport (this);
        WordProcessor::SetDialogSupport (this);
    }
    ~LedIt_DialogSupport ()
    {
        WordProcessor::SetDialogSupport (NULL);
        TextInteractor::SetDialogSupport (NULL);
    }

//  TextInteractor::DialogSupport
#if qSupportStdFindDlg
public:
    virtual void DisplayFindDialog (Led_tString* findText, const vector<Led_tString>& recentFindSuggestions, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK) override
    {
#if qPlatform_MacOS
        Led_StdDialogHelper_FindDialog findDialog;
#elif qPlatform_Windows
        Led_StdDialogHelper_FindDialog                 findDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
#elif qXWindows
        Led_StdDialogHelper_FindDialog       findDialog (GTK_WINDOW (LedItApplication::Get ().GetAppWindow ()));
#endif

        findDialog.fFindText              = *findText;
        findDialog.fRecentFindTextStrings = recentFindSuggestions;
        findDialog.fWrapSearch            = *wrapSearch;
        findDialog.fWholeWordSearch       = *wholeWordSearch;
        findDialog.fCaseSensativeSearch   = *caseSensative;

        findDialog.DoModal ();

        *findText        = findDialog.fFindText;
        *wrapSearch      = findDialog.fWrapSearch;
        *wholeWordSearch = findDialog.fWholeWordSearch;
        *caseSensative   = findDialog.fCaseSensativeSearch;
        *pressedOK       = findDialog.fPressedOK;
    }
#endif
#if qSupportStdReplaceDlg
public:
    virtual ReplaceButtonPressed DisplayReplaceDialog (Led_tString* findText, const vector<Led_tString>& recentFindSuggestions, Led_tString* replaceText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative) override
    {
#if qPlatform_MacOS
        Led_StdDialogHelper_ReplaceDialog replaceDialog;
#elif qPlatform_Windows
        Led_StdDialogHelper_ReplaceDialog              replaceDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
#elif qXWindows
        Led_StdDialogHelper_ReplaceDialog    replaceDialog (GTK_WINDOW (LedItApplication::Get ().GetAppWindow ()));
#endif

        replaceDialog.fFindText              = *findText;
        replaceDialog.fRecentFindTextStrings = recentFindSuggestions;
        replaceDialog.fReplaceText           = *replaceText;
        replaceDialog.fWrapSearch            = *wrapSearch;
        replaceDialog.fWholeWordSearch       = *wholeWordSearch;
        replaceDialog.fCaseSensativeSearch   = *caseSensative;

        replaceDialog.DoModal ();

        *findText        = replaceDialog.fFindText;
        *replaceText     = replaceDialog.fReplaceText;
        *wrapSearch      = replaceDialog.fWrapSearch;
        *wholeWordSearch = replaceDialog.fWholeWordSearch;
        *caseSensative   = replaceDialog.fCaseSensativeSearch;

        switch (replaceDialog.fPressed) {
            case Led_StdDialogHelper_ReplaceDialog::eCancel:
                return eReplaceButton_Cancel;
            case Led_StdDialogHelper_ReplaceDialog::eFind:
                return eReplaceButton_Find;
            case Led_StdDialogHelper_ReplaceDialog::eReplace:
                return eReplaceButton_Replace;
            case Led_StdDialogHelper_ReplaceDialog::eReplaceAll:
                return eReplaceButton_ReplaceAll;
            case Led_StdDialogHelper_ReplaceDialog::eReplaceAllInSelection:
                return eReplaceButton_ReplaceAllInSelection;
        }
        Assert (false);
        return eReplaceButton_Cancel;
    }
#endif
#if qSupportStdSpellCheckDlg
public:
    virtual void DisplaySpellCheckDialog (SpellCheckDialogCallback& callback) override
    {
        Led_StdDialogHelper_SpellCheckDialog::CallbackDelegator<SpellCheckDialogCallback> delegator (callback);
#if qPlatform_MacOS
        Led_StdDialogHelper_SpellCheckDialog spellCheckDialog (delegator);
#elif qPlatform_Windows
        Led_StdDialogHelper_SpellCheckDialog           spellCheckDialog (delegator, ::AfxGetResourceHandle (), ::GetActiveWindow ());
#elif qXWindows
        Led_StdDialogHelper_SpellCheckDialog spellCheckDialog (delegator, GTK_WINDOW (LedItApplication::Get ().GetAppWindow ()));
#endif

        spellCheckDialog.DoModal ();
    }
#endif

    //  WordProcessor::DialogSupport
public:
    virtual FontNameSpecifier CmdNumToFontName (CommandNumber cmdNum) override
    {
        Require (cmdNum >= WordProcessor::kFontMenuFirst_CmdID);
        Require (cmdNum <= WordProcessor::kFontMenuLast_CmdID);
#if qPlatform_MacOS
        static LMenu*        fontMenu = LMenuBar::GetCurrentMenuBar ()->FetchMenu (cmd_FontMenu);
        static vector<short> sFontIDMapCache; // OK to keep static cuz never changes during run of app

        size_t idx = cmdNum - WordProcessor::kFontMenuFirst_CmdID;

        // Pre-fill cache - at least to the cmd were looking for...
        for (size_t i = sFontIDMapCache.size (); i <= idx; i++) {
            Str255 pFontName = {0};
            UInt16 menuItem  = fontMenu->IndexFromCommand (i + WordProcessor::kFontMenuFirst_CmdID);
            ::GetMenuItemText (fontMenu->GetMacMenuH (), menuItem, pFontName);
            short familyID = 0;
            ::GetFNum (pFontName, &familyID);
            sFontIDMapCache.push_back (familyID);
        }
        return sFontIDMapCache[idx];
#elif qPlatform_Windows
        return LedItApplication::Get ().CmdNumToFontName (MFC_CommandNumberMapping::Get ().ReverseLookup (cmdNum)).c_str ();
#elif qXWindows
        const vector<Led_SDK_String>& fontNames = LedItApplication::Get ().fInstalledFonts.GetUsableFontNames ();
        Led_Assert (cmdNum - LedItView::kFontMenuFirst_CmdID < fontNames.size ());
        return (fontNames[cmdNum - LedItView::kFontMenuFirst_CmdID]);
#endif
    }
#if qSupportOtherFontSizeDlg
    virtual Led_Distance PickOtherFontHeight (Led_Distance origHeight) override
    {
#if qPlatform_MacOS
        Led_StdDialogHelper_OtherFontSizeDialog dlg;
#elif qPlatform_Windows
        Led_StdDialogHelper_OtherFontSizeDialog        dlg (::AfxGetResourceHandle (), ::GetActiveWindow ());
#endif
        dlg.InitValues (origHeight);
        if (dlg.DoModal ()) {
            return dlg.fFontSize_Result;
        }
        else {
            return 0;
        }
    }
#endif
#if qSupportParagraphSpacingDlg
    virtual bool PickNewParagraphLineSpacing (Led_TWIPS* spaceBefore, bool* spaceBeforeValid, Led_TWIPS* spaceAfter, bool* spaceAfterValid, Led_LineSpacing* lineSpacing, bool* lineSpacingValid) override
    {
#if qPlatform_MacOS
        Led_StdDialogHelper_ParagraphSpacingDialog dlg;
#elif qPlatform_Windows
        Led_StdDialogHelper_ParagraphSpacingDialog     dlg (::AfxGetResourceHandle (), ::GetActiveWindow ());
#endif
        dlg.InitValues (*spaceBefore, *spaceBeforeValid, *spaceAfter, *spaceAfterValid, *lineSpacing, *lineSpacingValid);

        if (dlg.DoModal ()) {
            *spaceBeforeValid = dlg.fSpaceBefore_Valid;
            if (*spaceBeforeValid) {
                *spaceBefore = dlg.fSpaceBefore_Result;
            }
            *spaceAfterValid = dlg.fSpaceAfter_Valid;
            if (*spaceAfterValid) {
                *spaceAfter = dlg.fSpaceAfter_Result;
            }
            *lineSpacingValid = dlg.fLineSpacing_Valid;
            if (*lineSpacingValid) {
                *lineSpacing = dlg.fLineSpacing_Result;
            }
            return true;
        }
        else {
            return false;
        }
    }
#endif
#if qSupportParagraphIndentsDlg
    virtual bool PickNewParagraphMarginsAndFirstIndent (Led_TWIPS* leftMargin, bool* leftMarginValid, Led_TWIPS* rightMargin, bool* rightMarginValid, Led_TWIPS* firstIndent, bool* firstIndentValid) override
    {
#if qPlatform_MacOS
        Led_StdDialogHelper_ParagraphIndentsDialog dlg;
#elif qPlatform_Windows
        Led_StdDialogHelper_ParagraphIndentsDialog     dlg (::AfxGetResourceHandle (), ::GetActiveWindow ());
#endif
        dlg.InitValues (*leftMargin, *leftMarginValid, *rightMargin, *rightMarginValid, *firstIndent, *firstIndentValid);
        if (dlg.DoModal ()) {
            *leftMarginValid = dlg.fLeftMargin_Valid;
            if (*leftMarginValid) {
                *leftMargin = dlg.fLeftMargin_Result;
            }
            *rightMarginValid = dlg.fRightMargin_Valid;
            if (*rightMarginValid) {
                *rightMargin = dlg.fRightMargin_Result;
            }
            *firstIndentValid = dlg.fFirstIndent_Valid;
            if (*firstIndentValid) {
                *firstIndent = dlg.fFirstIndent_Result;
            }
            return true;
        }
        else {
            return false;
        }
    }
#endif
#if qXWindows
    virtual bool PickOtherFontColor (Led_Color* color) override
    {
        StdColorPickBox dlg (GTK_WINDOW (LedItApplication::Get ().GetAppWindow ()), *color);
        dlg.DoModal ();
        if (dlg.GetWasOK ()) {
            *color = dlg.fColor;
            return true;
        }
        return false;
    }
#endif
#if qXWindows
    virtual bool ChooseFont (Led_IncrementalFontSpecification* font) override
    {
        StdFontPickBox dlg (GTK_WINDOW (LedItApplication::Get ().GetAppWindow ()), *font);
        dlg.DoModal ();
        if (dlg.GetWasOK ()) {
            *font = dlg.fFont;
            return true;
        }
        return false;
    }
#endif
#if qPlatform_MacOS || qPlatform_Windows
    virtual void ShowSimpleEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName) override
    {
// unknown embedding...
#if qPlatform_MacOS
        Led_StdDialogHelper_UnknownEmbeddingInfoDialog infoDialog;
#elif qPlatform_Windows
        Led_StdDialogHelper_UnknownEmbeddingInfoDialog infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
#endif
#if qPlatform_MacOS || qPlatform_Windows
        infoDialog.fEmbeddingTypeName = embeddingTypeName;
        (void)infoDialog.DoModal ();
#endif
    }
#endif
#if qPlatform_MacOS || qPlatform_Windows
    virtual bool ShowURLEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName, Led_SDK_String* urlTitle, Led_SDK_String* urlValue) override
    {
#if qPlatform_MacOS
        Led_StdDialogHelper_URLXEmbeddingInfoDialog infoDialog;
#elif qPlatform_Windows
        Led_StdDialogHelper_URLXEmbeddingInfoDialog    infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
#endif
#if qPlatform_MacOS || qPlatform_Windows
        infoDialog.fEmbeddingTypeName = embeddingTypeName;
        infoDialog.fTitleText         = *urlTitle;
        infoDialog.fURLText           = *urlValue;
        if (infoDialog.DoModal ()) {
            *urlTitle = infoDialog.fTitleText;
            *urlValue = infoDialog.fURLText;
            return true;
        }
        else {
            return false;
        }
#else
        return false;
#endif
    }
#endif
#if qPlatform_MacOS || qPlatform_Windows
    virtual bool ShowAddURLEmbeddingInfoDialog (Led_SDK_String* urlTitle, Led_SDK_String* urlValue) override
    {
#if qPlatform_MacOS
        Led_StdDialogHelper_AddURLXEmbeddingInfoDialog infoDialog;
#elif qPlatform_Windows
        Led_StdDialogHelper_AddURLXEmbeddingInfoDialog infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
#endif
#if qPlatform_MacOS || qPlatform_Windows
        infoDialog.fTitleText = *urlTitle;
        infoDialog.fURLText   = *urlValue;
        if (infoDialog.DoModal ()) {
            *urlTitle = infoDialog.fTitleText;
            *urlValue = infoDialog.fURLText;
            return true;
        }
        else {
            return false;
        }
#else
        return false;
#endif
    }
#endif
#if qSupportAddNewTableDlg
    bool AddNewTableDialog (size_t* nRows, size_t* nCols)
    {
        RequireNotNull (nRows);
        RequireNotNull (nCols);
#if qPlatform_MacOS
        Led_StdDialogHelper_AddNewTableDialog infoDialog;
#elif qPlatform_Windows
        Led_StdDialogHelper_AddNewTableDialog infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
#endif
        infoDialog.fRows    = *nRows;
        infoDialog.fColumns = *nCols;
        if (infoDialog.DoModal ()) {
            *nRows = infoDialog.fRows;
            *nCols = infoDialog.fColumns;
            return true;
        }
        else {
            return false;
        }
    }
#endif
#if qSupportEditTablePropertiesDlg
    virtual bool EditTablePropertiesDialog (TableSelectionPropertiesInfo* tableProperties) override
    {
        RequireNotNull (tableProperties);

        using DLGTYPE = Led_StdDialogHelper_EditTablePropertiesDialog;
#if qPlatform_MacOS
        DLGTYPE infoDialog;
#elif qPlatform_Windows
        DLGTYPE                               infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
#endif
#if qTemplatedMemberFunctionsFailBug
        Led_StdDialogHelper_EditTablePropertiesDialog_cvt<DLGTYPE::Info, TableSelectionPropertiesInfo> (&infoDialog.fInfo, *tableProperties);
#else
        DLGTYPE::cvt<DLGTYPE::Info, TableSelectionPropertiesInfo> (&infoDialog.fInfo, *tableProperties);
#endif
        if (infoDialog.DoModal ()) {
#if qTemplatedMemberFunctionsFailBug
            Led_StdDialogHelper_EditTablePropertiesDialog_cvt<TableSelectionPropertiesInfo, DLGTYPE::Info> (tableProperties, infoDialog.fInfo);
#else
            DLGTYPE::cvt<TableSelectionPropertiesInfo, DLGTYPE::Info> (tableProperties, infoDialog.fInfo);
#endif
            return true;
        }
        else {
            return false;
        }
    }
#endif
};
static LedIt_DialogSupport sLedIt_DialogSupport;

/*
 ********************************************************************************
 ************************************ LedItView *********************************
 ********************************************************************************
 */
#if qPlatform_Windows
IMPLEMENT_DYNCREATE (LedItView, CView)

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4407) // Not sure this is safe to ignore but I think it is due to qMFCRequiresCWndLeftmostBaseClass
#endif
BEGIN_MESSAGE_MAP (LedItView, LedItView::inherited)
ON_WM_SETFOCUS ()
ON_WM_SIZE ()
ON_WM_CONTEXTMENU ()
ON_COMMAND (ID_OLE_INSERT_NEW, &OnInsertObject)
ON_COMMAND (ID_CANCEL_EDIT_CNTR, &OnCancelEditCntr)
ON_COMMAND (ID_CANCEL_EDIT_SRVR, &OnCancelEditSrvr)
ON_COMMAND (ID_FILE_PRINT, &OnFilePrint)
ON_COMMAND (ID_FILE_PRINT_DIRECT, &OnFilePrint)
ON_NOTIFY (NM_RETURN, ID_VIEW_FORMATBAR, &OnBarReturn)
END_MESSAGE_MAP ()
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif
#endif

LedItView::LedItView (
#if qXWindows
    LedItDocument* owningDoc
#endif
    )
    : inherited ()
    , fWrapToWindow (Options ().GetWrapToWindow ())
{
    SetSmartCutAndPasteMode (Options ().GetSmartCutAndPaste ());

    SetShowParagraphGlyphs (Options ().GetShowParagraphGlyphs ());
    SetShowTabGlyphs (Options ().GetShowTabGlyphs ());
    SetShowSpaceGlyphs (Options ().GetShowSpaceGlyphs ());
#if qPlatform_MacOS
    SetScrollBarType (h, fWrapToWindow ? eScrollBarNever : eScrollBarAsNeeded);
    SetScrollBarType (v, eScrollBarAlways);
#elif qPlatform_Windows
    SetScrollBarType (h, fWrapToWindow ? eScrollBarNever : eScrollBarAsNeeded);
    SetScrollBarType (v, eScrollBarAlways);
#elif qXWindows
    SpecifyTextStore (&owningDoc->GetTextStore ());
    SetStyleDatabase (owningDoc->GetStyleDatabase ());
    SetParagraphDatabase (owningDoc->GetParagraphDatabase ());
    SetHidableTextDatabase (owningDoc->GetHidableTextDatabase ());
    //SetShowHiddenText (Options ().GetShowHiddenText ());
    SetCommandHandler (&owningDoc->GetCommandHandler ());
    SetSpellCheckEngine (&LedItApplication::Get ().fSpellCheckEngine);
#endif
#if qPlatform_MacOS || qPlatform_Windows
    SetUseSecondaryHilight (true);
#endif
#if qPlatform_Windows
    // SHOULD be supported on other platforms, but only Win32 for now...
    SetDefaultWindowMargins (Led_TWIPS_Rect (kLedItViewTopMargin, kLedItViewLHSMargin, kLedItViewBottomMargin - kLedItViewTopMargin, kLedItViewRHSMargin - kLedItViewLHSMargin));
#endif
}

LedItView::~LedItView ()
{
    SpecifyTextStore (NULL);
    SetCommandHandler (NULL);
    SetSpellCheckEngine (NULL);
}

#if qPlatform_Windows
void LedItView::OnInitialUpdate ()
{
    inherited::OnInitialUpdate ();
    SpecifyTextStore (&GetDocument ().GetTextStore ());
    SetStyleDatabase (GetDocument ().GetStyleDatabase ());
    SetParagraphDatabase (GetDocument ().GetParagraphDatabase ());
    SetHidableTextDatabase (GetDocument ().GetHidableTextDatabase ());
    SetShowHiddenText (Options ().GetShowHiddenText ());
    SetCommandHandler (&GetDocument ().GetCommandHandler ());
    SetSpellCheckEngine (&LedItApplication::Get ().fSpellCheckEngine);

    {
        // Don't let this change the docs IsModified flag
        bool docModified = GetDocument ().IsModified ();
        // For an empty doc - grab from the default, and otherwise grab from the document itself
        if (GetEnd () == 0) {
            SetEmptySelectionStyle (Options ().GetDefaultNewDocFont ());
        }
        else {
            SetEmptySelectionStyle ();
        }
        InvalidateAllCaches (); // under rare circumstances, the caches don't all get cleared without this...
        GetDocument ().SetModifiedFlag (docModified);
    }
    Invariant ();
}
#endif

#if qPlatform_Windows
bool LedItView::OnUpdateCommand (CommandUpdater* enabler)
{
    RequireNotNull (enabler);
    if (inherited::OnUpdateCommand (enabler)) {
        return true;
    }
    // See SPR#1462 - yet assure these items in the formatBar remain enabled...
    switch (enabler->GetCmdID ()) {
        case IDC_FONTSIZE: {
            enabler->SetEnabled (true);
            return true;
        }
        case IDC_FONTNAME: {
            enabler->SetEnabled (true);
            return true;
        }
    }
    return false;
}
#endif

void LedItView::SetWrapToWindow (bool wrapToWindow)
{
    if (fWrapToWindow != wrapToWindow) {
        fWrapToWindow = wrapToWindow;
        SetScrollBarType (h, fWrapToWindow ? eScrollBarNever : eScrollBarAsNeeded);
        InvalidateAllCaches ();
        InvalidateScrollBarParameters ();
        Refresh ();
    }
}

void LedItView::GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const
{
    inherited::GetLayoutMargins (row, lhs, rhs);
    if (fWrapToWindow) {
        // Make the layout width of each line (paragraph) equal to the windowrect. Ie, wrap to the
        // edge of the window. NB: because of this choice, we must 'InvalidateAllCaches' when the
        // WindowRect changes in our SetWindowRect() override.
        if (rhs != NULL) {
            *rhs = (max (GetWindowRect ().GetWidth (), Led_Distance (1)));
        }
    }
}

void LedItView::SetWindowRect (const Led_Rect& windowRect)
{
    Led_Rect oldWindowRect = GetWindowRect ();
    // Hook all changes in the window width, so we can invalidate the word-wrap info (see LedItView::GetLayoutWidth)
    if (windowRect != oldWindowRect) {
        // NB: call "WordWrappedTextImager::SetWindowRect() instead of base class textinteractor to avoid infinite recursion"
        WordWrappedTextImager::SetWindowRect (windowRect);
        if (fWrapToWindow and windowRect.GetSize () != oldWindowRect.GetSize ()) {
            InvalidateAllCaches ();
        }
    }
}

#if qPlatform_MacOS
void LedItView::FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, UInt16& outMark, Str255 outName)
{
    outUsesMark = false;
    switch (inCommand) {
        case cmd_ListStyleMenu:
            outEnabled = true;
            break;
        case cmd_FontMenu:
            outEnabled = true;
            break;
        case cmd_SizeMenu:
            outEnabled = true;
            break;
        case cmd_StyleMenu:
            outEnabled = true;
            break;
        case cmd_ColorMenu:
            outEnabled = true;
            break;
        case cmd_JustificationMenu:
            outEnabled = true;
            break;

        default: {
            inherited::FindCommandStatus (inCommand, outEnabled, outUsesMark, outMark, outName);
        } break;
    }
}
#endif

#if qPlatform_Windows
void LedItView::OnContextMenu (CWnd* /*pWnd*/, CPoint pt)
{
    CMenu menu;
    if (menu.LoadMenu (kContextMenu)) {
        CMenu* popup = menu.GetSubMenu (0);
        AssertNotNull (popup);
        LedItApplication::Get ().FixupFontMenu (popup->GetSubMenu (16));
        popup->TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, ::AfxGetMainWnd ());
    }
}

BOOL LedItView::IsSelected (const CObject* pDocItem) const
{
    // The implementation below is adequate if your selection consists of
    //  only LedItControlItem objects.  To handle different selection
    //  mechanisms, the implementation here should be replaced.

    // TODO: implement this function that tests for a selected OLE client item
    return pDocItem == GetSoleSelectedOLEEmbedding ();
}
#endif

WordProcessor::IncrementalParagraphInfo LedItView::GetParaFormatSelection ()
{
    //COULD SPEED TWEEK THIS - LIKE I DID FOR fCachedCurSelJustificationUnique
    IncrementalParagraphInfo ipi;
    StandardTabStopList      tabstops;
    if (GetStandardTabStopList (GetSelectionStart (), GetSelectionEnd (), &tabstops)) {
        ipi.SetTabStopList (tabstops);
    }
    Led_TWIPS lhsMargin = Led_TWIPS (0);
    Led_TWIPS rhsMargin = Led_TWIPS (0);
    if (GetMargins (GetSelectionStart (), GetSelectionEnd (), &lhsMargin, &rhsMargin)) {
        ipi.SetMargins (lhsMargin, rhsMargin);
    }
    Led_TWIPS firstIndent = Led_TWIPS (0);
    if (GetFirstIndent (GetSelectionStart (), GetSelectionEnd (), &firstIndent)) {
        ipi.SetFirstIndent (firstIndent);
    }
    return ipi;
}

void LedItView::SetParaFormatSelection (const IncrementalParagraphInfo& pf)
{
    if (pf.GetTabStopList_Valid ()) {
        InteractiveSetStandardTabStopList (pf.GetTabStopList ());
    }
    if (pf.GetMargins_Valid () and pf.GetFirstIndent_Valid ()) {
        InteractiveSetMarginsAndFirstIndent (pf.GetLeftMargin (), pf.GetRightMargin (), pf.GetFirstIndent ());
    }
    else {
        if (pf.GetMargins_Valid ()) {
            InteractiveSetMargins (pf.GetLeftMargin (), pf.GetRightMargin ());
        }
        if (pf.GetFirstIndent_Valid ()) {
            InteractiveSetFirstIndent (pf.GetFirstIndent ());
        }
    }
}

void LedItView::OnShowHideGlyphCommand (CommandNumber cmdNum)
{
    inherited::OnShowHideGlyphCommand (cmdNum);

    Options ().SetShowParagraphGlyphs (GetShowParagraphGlyphs ());
    Options ().SetShowTabGlyphs (GetShowTabGlyphs ());
    Options ().SetShowSpaceGlyphs (GetShowSpaceGlyphs ());
}

LedItView::SearchParameters LedItView::GetSearchParameters () const
{
    return Options ().GetSearchParameters ();
}

void LedItView::SetSearchParameters (const SearchParameters& sp)
{
    Options ().SetSearchParameters (sp);
}

void LedItView::SetShowHiddenText (bool showHiddenText)
{
    if (showHiddenText) {
        GetHidableTextDatabase ()->ShowAll ();
    }
    else {
        GetHidableTextDatabase ()->HideAll ();
    }
}

#if qPlatform_Windows
void LedItView::OnInsertObject ()
{
    // Invoke the standard Insert Object dialog box to obtain information
    //  for new LedItControlItem object.
    COleInsertDialog dlg (IOF_SELECTCREATECONTROL | IOF_SHOWINSERTCONTROL | IOF_VERIFYSERVERSEXIST);
    if (dlg.DoModal () != IDOK) {
        return;
    }

    CWaitCursor busy;

    LedItControlItem* pItem = NULL;
    TRY
    {
        // Create new item connected to this document.
        LedItDocument& doc = GetDocument ();
        pItem              = new LedItControlItem (&doc);
        ASSERT_VALID (pItem);

        dlg.m_io.dwFlags |= IOF_SELECTCREATENEW;
        // Initialize the item from the dialog data.
        if (!dlg.CreateItem (pItem)) {
            Led_ThrowBadFormatDataException ();
        }
        ASSERT_VALID (pItem);

        BreakInGroupedCommands ();
        UndoableContextHelper context (*this, Led_SDK_TCHAROF ("Insert OLE Embedding"), false);
        {
            AddEmbedding (pItem, GetTextStore (), GetSelectionStart (), GetDocument ().GetStyleDatabase ().get ());
            SetSelection (GetSelectionStart () + 1, GetSelectionStart () + 1);
        }
        context.CommandComplete ();
        BreakInGroupedCommands ();

        // If item created from class list (not from file) then launch
        //  the server to edit the item.
        if (dlg.GetSelectionType () == COleInsertDialog::createNewItem) {
            pItem->DoVerb (OLEIVERB_SHOW, this);
        }

        ASSERT_VALID (pItem);

        doc.UpdateAllViews (NULL);
    }
    CATCH (CException, e)
    {
        if (pItem != NULL) {
            ASSERT_VALID (pItem);
            pItem->Delete ();
        }
        AfxMessageBox (IDP_FAILED_TO_CREATE);
    }
    END_CATCH
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the container (not the server) causes the deactivation.
void LedItView::OnCancelEditCntr ()
{
    // Close any in-place active item on this view.
    COleClientItem* pActiveItem = GetDocument ().GetInPlaceActiveItem (this);
    if (pActiveItem != NULL) {
        pActiveItem->Close ();
    }
    ASSERT (GetDocument ().GetInPlaceActiveItem (this) == NULL);
}

// Special handling of OnSetFocus and OnSize are required for a container
//  when an object is being edited in-place.
void LedItView::OnSetFocus (CWnd* pOldWnd)
{
    COleClientItem* pActiveItem = GetDocument ().GetInPlaceActiveItem (this);
    if (pActiveItem != NULL && pActiveItem->GetItemState () == COleClientItem::activeUIState) {
        // need to set focus to this item if it is in the same view
        CWnd* pWnd = pActiveItem->GetInPlaceWindow ();
        if (pWnd != NULL) {
            pWnd->SetFocus (); // don't call the base class
            return;
        }
    }
    inherited::OnSetFocus (pOldWnd);
}

void LedItView::OnSize (UINT nType, int cx, int cy)
{
    inherited::OnSize (nType, cx, cy);

    // ajust any active OLE embeddings, as needed
    COleClientItem* pActiveItem = GetDocument ().GetInPlaceActiveItem (this);
    if (pActiveItem != NULL) {
        pActiveItem->SetItemRects ();
    }
}

void LedItView::OnCancelEditSrvr ()
{
    GetDocument ().OnDeactivateUI (FALSE);
}
#endif

#if qPlatform_Windows
LedItControlItem* LedItView::GetSoleSelectedOLEEmbedding () const
{
    return dynamic_cast<LedItControlItem*> (GetSoleSelectedEmbedding ());
}

void LedItView::OnBarReturn (NMHDR*, LRESULT*)
{
    // When we return from doing stuff in format bar, reset focus to our edit view. Try it without and
    // you'll see how awkward it is...
    SetFocus ();
}

#ifdef _DEBUG
void LedItView::AssertValid () const
{
    inherited::AssertValid ();
}

void LedItView::Dump (CDumpContext& dc) const
{
    inherited::Dump (dc);
}

LedItDocument& LedItView::GetDocument () const // non-debug version is inline
{
    ASSERT (m_pDocument->IsKindOf (RUNTIME_CLASS (LedItDocument)));
    ASSERT_VALID (m_pDocument);
    return *(LedItDocument*)m_pDocument;
}
#endif //_DEBUG
#endif
