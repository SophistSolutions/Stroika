/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Platform_Windows
DISABLE_COMPILER_MSC_WARNING_START (5054)
#include <afxodlgs.h> // MFC OLE dialog classes
DISABLE_COMPILER_MSC_WARNING_END (5054)
#endif

#include "Stroika/Foundation/DataExchange/BadFormatException.h"

#include "Stroika/Frameworks/Led/StdDialogs.h"

#include "ColorMenu.h"
#if qStroika_Platform_Windows
#include "LedItControlItem.h"
#endif
#include "LedItDocument.h"
#include "LedItResources.h"
#include "Options.h"

#include "LedItApplication.h"

#include "LedItView.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;
using namespace Stroika::Frameworks::Led::StyledTextIO;

class My_CMDNUM_MAPPING : public
#if qStroika_Platform_Windows
                          Platform::MFC_CommandNumberMapping
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
#if qStroika_Platform_Windows
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

#if qStroika_Platform_Windows
        AddAssociation (kParagraphSpacingCmd, LedItView::kParagraphSpacingCommand_CmdID);
#endif
#if qStroika_Platform_Windows
        AddAssociation (kParagraphIndentsCmd, LedItView::kParagraphIndentsCommand_CmdID);
#endif

        AddAssociation (kListStyle_NoneCmd, LedItView::kListStyle_None_CmdID);
        AddAssociation (kListStyle_BulletCmd, LedItView::kListStyle_Bullet_CmdID);

        AddAssociation (kIncreaseIndentCmd, LedItView::kIncreaseIndent_CmdID);
        AddAssociation (kDecreaseIndentCmd, LedItView::kDecreaseIndent_CmdID);

        AddRangeAssociation (kBaseFontNameCmd, kLastFontNameCmd, LedItView::kFontMenuFirst_CmdID, LedItView::kFontMenuLast_CmdID);

        AddAssociation (kFontStylePlainCmd, LedItView::kFontStylePlain_CmdID);
        AddAssociation (kFontStyleBoldCmd, LedItView::kFontStyleBold_CmdID);
        AddAssociation (kFontStyleItalicCmd, LedItView::kFontStyleItalic_CmdID);
        AddAssociation (kFontStyleUnderlineCmd, LedItView::kFontStyleUnderline_CmdID);
#if qStroika_Platform_Windows
        AddAssociation (kFontStyleStrikeoutCmd, LedItView::kFontStyleStrikeout_CmdID);
#endif
        AddAssociation (kSubScriptCmd, LedItView::kSubScriptCommand_CmdID);
        AddAssociation (kSuperScriptCmd, LedItView::kSuperScriptCommand_CmdID);
#if qStroika_Platform_Windows
        AddAssociation (kChooseFontDialogCmd, LedItView::kChooseFontCommand_CmdID);
#endif

        AddAssociation (kInsertTableCmd, LedItView::kInsertTable_CmdID);
        AddAssociation (kInsertTableRowAboveCmd, LedItView::kInsertTableRowAbove_CmdID);
        AddAssociation (kInsertTableRowBelowCmd, LedItView::kInsertTableRowBelow_CmdID);
        AddAssociation (kInsertTableColBeforeCmd, LedItView::kInsertTableColBefore_CmdID);
        AddAssociation (kInsertTableColAfterCmd, LedItView::kInsertTableColAfter_CmdID);
        AddAssociation (kInsertURLCmd, LedItView::kInsertURL_CmdID);
#if qStroika_Platform_Windows
        AddAssociation (kInsertSymbolCmd, LedItView::kInsertSymbol_CmdID);
#endif

        //              AddAssociation (kPropertiesForSelectionCmd,     LedItView::kSelectedEmbeddingProperties_CmdID);
        AddRangeAssociation (kFirstSelectedEmbeddingCmd, kLastSelectedEmbeddingCmd, LedItView::kFirstSelectedEmbedding_CmdID,
                             LedItView::kLastSelectedEmbedding_CmdID);

        AddAssociation (kHideSelectionCmd, LedItView::kHideSelection_CmdID);
        AddAssociation (kUnHideSelectionCmd, LedItView::kUnHideSelection_CmdID);
        AddAssociation (kRemoveTableRowsCmd, LedItView::kRemoveTableRows_CmdID);
        AddAssociation (kRemoveTableColumnsCmd, LedItView::kRemoveTableColumns_CmdID);

        AddAssociation (kShowHideParagraphGlyphsCmd, LedItView::kShowHideParagraphGlyphs_CmdID);
        AddAssociation (kShowHideTabGlyphsCmd, LedItView::kShowHideTabGlyphs_CmdID);
        AddAssociation (kShowHideSpaceGlyphsCmd, LedItView::kShowHideSpaceGlyphs_CmdID);

#if qStroika_Platform_Windows
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
#if qStroika_Platform_Windows
public:
    virtual void DisplayFindDialog (Led_tString* findText, const vector<Led_tString>& recentFindSuggestions, bool* wrapSearch,
                                    bool* wholeWordSearch, bool* caseSensative, bool* pressedOK) override
    {
#if qStroika_Platform_Windows
        Led_StdDialogHelper_FindDialog findDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
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
#if qStroika_Platform_Windows
public:
    virtual ReplaceButtonPressed DisplayReplaceDialog (Led_tString* findText, const vector<Led_tString>& recentFindSuggestions,
                                                       Led_tString* replaceText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative) override
    {
#if qStroika_Platform_Windows
        Led_StdDialogHelper_ReplaceDialog replaceDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
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
#if qStroika_Platform_Windows
public:
    virtual void DisplaySpellCheckDialog (SpellCheckDialogCallback& callback) override
    {
        Led_StdDialogHelper_SpellCheckDialog::CallbackDelegator<SpellCheckDialogCallback> delegator (callback);
#if qStroika_Platform_Windows
        Led_StdDialogHelper_SpellCheckDialog spellCheckDialog (delegator, ::AfxGetResourceHandle (), ::GetActiveWindow ());
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
#if qStroika_Platform_Windows
        return LedItApplication::Get ().CmdNumToFontName (MFC_CommandNumberMapping::Get ().ReverseLookup (cmdNum)).c_str ();
#endif
    }
#if qStroika_Platform_Windows
    virtual DistanceType PickOtherFontHeight (DistanceType origHeight) override
    {
#if qStroika_Platform_Windows
        Led_StdDialogHelper_OtherFontSizeDialog dlg (::AfxGetResourceHandle (), ::GetActiveWindow ());
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
#if qStroika_Platform_Windows
    virtual bool PickNewParagraphLineSpacing (TWIPS* spaceBefore, bool* spaceBeforeValid, TWIPS* spaceAfter, bool* spaceAfterValid,
                                              LineSpacing* lineSpacing, bool* lineSpacingValid) override
    {
#if qStroika_Platform_Windows
        Led_StdDialogHelper_ParagraphSpacingDialog dlg (::AfxGetResourceHandle (), ::GetActiveWindow ());
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
#if qStroika_Platform_Windows
    virtual bool PickNewParagraphMarginsAndFirstIndent (TWIPS* leftMargin, bool* leftMarginValid, TWIPS* rightMargin,
                                                        bool* rightMarginValid, TWIPS* firstIndent, bool* firstIndentValid) override
    {
#if qStroika_Platform_Windows
        Led_StdDialogHelper_ParagraphIndentsDialog dlg (::AfxGetResourceHandle (), ::GetActiveWindow ());
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
#if qStroika_Platform_Windows
    virtual void ShowSimpleEmbeddingInfoDialog (const SDKString& embeddingTypeName) override
    {
// unknown embedding...
#if qStroika_Platform_Windows
        Led_StdDialogHelper_UnknownEmbeddingInfoDialog infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
#endif
#if qStroika_Platform_Windows
        infoDialog.fEmbeddingTypeName = embeddingTypeName;
        (void)infoDialog.DoModal ();
#endif
    }
#endif
#if qStroika_Platform_Windows
    virtual bool ShowURLEmbeddingInfoDialog (const SDKString& embeddingTypeName, SDKString* urlTitle, SDKString* urlValue) override
    {
#if qStroika_Platform_Windows
        Led_StdDialogHelper_URLXEmbeddingInfoDialog infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
#endif
#if qStroika_Platform_Windows
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
#if qStroika_Platform_Windows
    virtual bool ShowAddURLEmbeddingInfoDialog (SDKString* urlTitle, SDKString* urlValue) override
    {
#if qStroika_Platform_Windows
        Led_StdDialogHelper_AddURLXEmbeddingInfoDialog infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
#endif
#if qStroika_Platform_Windows
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
#if qStroika_Platform_Windows
    bool AddNewTableDialog (size_t* nRows, size_t* nCols)
    {
        RequireNotNull (nRows);
        RequireNotNull (nCols);
#if qStroika_Platform_Windows
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
#if qStroika_Platform_Windows
    virtual bool EditTablePropertiesDialog (TableSelectionPropertiesInfo* tableProperties) override
    {
        RequireNotNull (tableProperties);

        using DLGTYPE = Led_StdDialogHelper_EditTablePropertiesDialog;
#if qStroika_Platform_Windows
        DLGTYPE infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
#endif
        DLGTYPE::cvt<DLGTYPE::Info, TableSelectionPropertiesInfo> (&infoDialog.fInfo, *tableProperties);
        if (infoDialog.DoModal ()) {
            DLGTYPE::cvt<TableSelectionPropertiesInfo, DLGTYPE::Info> (tableProperties, infoDialog.fInfo);
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
#if qStroika_Platform_Windows
IMPLEMENT_DYNCREATE (LedItView, CView)

DISABLE_COMPILER_MSC_WARNING_START (4407) // Not sure this is safe to ignore but I think it is due to qMFCRequiresCWndLeftmostBaseClass
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
DISABLE_COMPILER_MSC_WARNING_END (4407)
#endif

LedItView::LedItView ()
    : inherited ()
    , fWrapToWindow (Options{}.GetWrapToWindow ())
{
    SetSmartCutAndPasteMode (Options{}.GetSmartCutAndPaste ());

    SetShowParagraphGlyphs (Options{}.GetShowParagraphGlyphs ());
    SetShowTabGlyphs (Options{}.GetShowTabGlyphs ());
    SetShowSpaceGlyphs (Options{}.GetShowSpaceGlyphs ());
#if qStroika_Platform_Windows
    SetScrollBarType (h, fWrapToWindow ? eScrollBarNever : eScrollBarAsNeeded);
    SetScrollBarType (v, eScrollBarAlways);
#endif
#if qStroika_Platform_Windows
    SetUseSecondaryHilight (true);
#endif
#if qStroika_Platform_Windows
    // SHOULD be supported on other platforms, but only Win32 for now...
    SetDefaultWindowMargins (TWIPS_Rect (kLedItViewTopMargin, kLedItViewLHSMargin, kLedItViewBottomMargin - kLedItViewTopMargin,
                                         kLedItViewRHSMargin - kLedItViewLHSMargin));
#endif
}

LedItView::~LedItView ()
{
    SpecifyTextStore (NULL);
    SetCommandHandler (NULL);
    SetSpellCheckEngine (NULL);
}

#if qStroika_Platform_Windows
void LedItView::OnInitialUpdate ()
{
    inherited::OnInitialUpdate ();
    SpecifyTextStore (&GetDocument ().GetTextStore ());
    SetStyleDatabase (GetDocument ().GetStyleDatabase ());
    SetParagraphDatabase (GetDocument ().GetParagraphDatabase ());
    SetHidableTextDatabase (GetDocument ().GetHidableTextDatabase ());
    SetShowHiddenText (Options{}.GetShowHiddenText ());
    SetCommandHandler (&GetDocument ().GetCommandHandler ());
    SetSpellCheckEngine (LedItApplication::Get ().fSpellCheckEngine.get ());

    {
        // Don't let this change the docs IsModified flag
        bool docModified = GetDocument ().IsModified ();
        // For an empty doc - grab from the default, and otherwise grab from the document itself
        if (GetEnd () == 0) {
            SetEmptySelectionStyle (Options{}.GetDefaultNewDocFont ());
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

#if qStroika_Platform_Windows
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

void LedItView::GetLayoutMargins (RowReference row, CoordinateType* lhs, CoordinateType* rhs) const
{
    inherited::GetLayoutMargins (row, lhs, rhs);
    if (fWrapToWindow) {
        // Make the layout width of each line (paragraph) equal to the windowrect. Ie, wrap to the
        // edge of the window. NB: because of this choice, we must 'InvalidateAllCaches' when the
        // WindowRect changes in our SetWindowRect() override.
        if (rhs != NULL) {
            *rhs = (max (GetWindowRect ().GetWidth (), DistanceType (1)));
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

#if qStroika_Platform_Windows
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

IncrementalParagraphInfo LedItView::GetParaFormatSelection ()
{
    //COULD SPEED TWEEK THIS - LIKE I DID FOR fCachedCurSelJustificationUnique
    IncrementalParagraphInfo ipi;
    StandardTabStopList      tabstops;
    if (GetStandardTabStopList (GetSelectionStart (), GetSelectionEnd (), &tabstops)) {
        ipi.SetTabStopList (tabstops);
    }
    TWIPS lhsMargin = TWIPS{0};
    TWIPS rhsMargin = TWIPS{0};
    if (GetMargins (GetSelectionStart (), GetSelectionEnd (), &lhsMargin, &rhsMargin)) {
        ipi.SetMargins (lhsMargin, rhsMargin);
    }
    TWIPS firstIndent = TWIPS{0};
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

    Options{}.SetShowParagraphGlyphs (GetShowParagraphGlyphs ());
    Options{}.SetShowTabGlyphs (GetShowTabGlyphs ());
    Options{}.SetShowSpaceGlyphs (GetShowSpaceGlyphs ());
}

LedItView::SearchParameters LedItView::GetSearchParameters () const
{
    return Options{}.GetSearchParameters ();
}

void LedItView::SetSearchParameters (const SearchParameters& sp)
{
    Options{}.SetSearchParameters (sp);
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

#if qStroika_Platform_Windows
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
            Execution::Throw (DataExchange::BadFormatException::kThe);
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

#if qStroika_Platform_Windows
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
