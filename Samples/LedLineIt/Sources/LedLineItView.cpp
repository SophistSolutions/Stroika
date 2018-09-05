/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxodlgs.h> // MFC OLE dialog classes
#include <afxwin.h>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Frameworks/Led/StdDialogs.h"

#include "FontMenu.h"
#include "LedLineItApplication.h"
#include "LedLineItDocument.h"
#include "LedLineItMainFrame.h"
#include "Options.h"
#include "Resource.h"

#include "LedLineItView.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

using Memory::SmallStackBuffer;

class My_CMDNUM_MAPPING : public MFC_CommandNumberMapping {
public:
    My_CMDNUM_MAPPING ()
    {
        AddAssociation (ID_EDIT_UNDO, LedLineItView::kUndo_CmdID);
        AddAssociation (ID_EDIT_REDO, LedLineItView::kRedo_CmdID);
        AddAssociation (ID_EDIT_SELECT_ALL, LedLineItView::kSelectAll_CmdID);
        AddAssociation (ID_EDIT_CUT, LedLineItView::kCut_CmdID);
        AddAssociation (ID_EDIT_COPY, LedLineItView::kCopy_CmdID);
        AddAssociation (ID_EDIT_PASTE, LedLineItView::kPaste_CmdID);
        AddAssociation (ID_EDIT_CLEAR, LedLineItView::kClear_CmdID);
        AddAssociation (kFindCmd, LedLineItView::kFind_CmdID);
        AddAssociation (kFindAgainCmd, LedLineItView::kFindAgain_CmdID);
        AddAssociation (kEnterFindStringCmd, LedLineItView::kEnterFindString_CmdID);
        AddAssociation (kReplaceCmd, LedLineItView::kReplace_CmdID);
        AddAssociation (kReplaceAgainCmd, LedLineItView::kReplaceAgain_CmdID);
#if qIncludeBasicSpellcheckEngine
        AddAssociation (kSpellCheckCmd, LedLineItView::kSpellCheck_CmdID);
#endif
    }
};
My_CMDNUM_MAPPING sMy_CMDNUM_MAPPING;

struct LedLineIt_DialogSupport : TextInteractor::DialogSupport {
public:
    LedLineIt_DialogSupport ()
    {
        TextInteractor::SetDialogSupport (this);
    }
    ~LedLineIt_DialogSupport ()
    {
        TextInteractor::SetDialogSupport (NULL);
    }
#if qSupportStdFindDlg
public:
    virtual void DisplayFindDialog (Led_tString* findText, const vector<Led_tString>& recentFindSuggestions, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK) override
    {
        Led_StdDialogHelper_FindDialog findDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());

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
        Led_StdDialogHelper_ReplaceDialog replaceDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());

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
        Led_StdDialogHelper_SpellCheckDialog spellCheckDialog (delegator, ::AfxGetResourceHandle (), ::GetActiveWindow ());
#elif qXWindows
        Led_StdDialogHelper_SpellCheckDialog spellCheckDialog (delegator, GTK_WINDOW (LedItApplication::Get ().GetAppWindow ()));
#endif

        spellCheckDialog.DoModal ();
    }
#endif
};
static LedLineIt_DialogSupport sLedLineIt_DialogSupport;

inline Led_IncrementalFontSpecification::FontSize FontCmdToSize (UINT cmd)
{
    switch (cmd) {
        case kFontSize9CmdID:
            return 9;
        case kFontSize10CmdID:
            return 10;
        case kFontSize12CmdID:
            return 12;
        case kFontSize14CmdID:
            return 14;
        case kFontSize18CmdID:
            return 18;
        case kFontSize24CmdID:
            return 24;
        case kFontSize36CmdID:
            return 36;
        case kFontSize48CmdID:
            return 48;
        case kFontSize72CmdID:
            return 72;
        case kFontSizeOtherCmdID:
            return 0;
        case kFontSizeSmallerCmdID:
            return 0;
        case kFontSizeLargerCmdID:
            return 0;
        default:
            Assert (false);
            return 0;
    }
}
inline bool IsPredefinedFontSize (int size)
{
    switch (size) {
        case 9:
            return true;
        case 10:
            return true;
        case 12:
            return true;
        case 14:
            return true;
        case 18:
            return true;
        case 24:
            return true;
        case 36:
            return true;
        case 48:
            return true;
        case 72:
            return true;
        default:
            return false;
    }
}

inline bool IsASCIISpace (int c)
{
    return isascii (c) and isspace (c);
}

// Perhaps replace this with a user-configuration option in the options dialog?
const unsigned int kCharsPerTab = 4;

const Led_Distance kBadDistance = Led_Distance (-1);

class GotoLineDialog : public CDialog {
public:
    GotoLineDialog (size_t origLine)
        : CDialog (kGotoLine_DialogID)
        , fOrigLine (origLine)
        , fResultLine (0)
    {
    }
    virtual BOOL OnInitDialog () override
    {
        BOOL result = CDialog::OnInitDialog ();
        SetDlgItemInt (kGotoLine_Dialog_LineNumberEditFieldID, fOrigLine);
        return (result);
    }
    virtual void OnOK () override
    {
        BOOL trans  = false;
        fResultLine = GetDlgItemInt (kGotoLine_Dialog_LineNumberEditFieldID, &trans);
        if (not trans) {
            fResultLine = 0;
        }
        CDialog::OnOK ();
    }

public:
    size_t fOrigLine;
    size_t fResultLine;

protected:
    DECLARE_MESSAGE_MAP ()
};
BEGIN_MESSAGE_MAP (GotoLineDialog, CDialog)
END_MESSAGE_MAP ()

#if qSupportGenRandomCombosCommand
namespace {
    struct AtStartup {
        AtStartup ()
        {
            ::srand ((unsigned)::time (NULL));
        }
    } X_AtStartup;
}
#endif

/*
 ********************************************************************************
 ************************************ LedLineItView *****************************
 ********************************************************************************
 */
IMPLEMENT_DYNCREATE (LedLineItView, CView)

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4407) // Not sure this is safe to ignore but I think it is due to qMFCRequiresCWndLeftmostBaseClass
#endif
BEGIN_MESSAGE_MAP (LedLineItView, LedLineItView::inherited)

ON_WM_SETFOCUS ()
ON_WM_CONTEXTMENU ()
ON_COMMAND (ID_CANCEL_EDIT_CNTR, OnCancelEditCntr)
ON_COMMAND (ID_CANCEL_EDIT_SRVR, OnCancelEditSrvr)
ON_COMMAND (ID_FILE_PRINT, OnFilePrint)
ON_COMMAND (ID_FILE_PRINT_DIRECT, OnFilePrint)
ON_COMMAND (ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)

ON_COMMAND (kGotoLineCmdID, OnGotoLineCommand)
#if qSupportGenRandomCombosCommand
ON_COMMAND (kGenRandomCombosCmdID, OnGenRandomCombosCommand)
#endif
ON_COMMAND (kShiftLeftCmdID, OnShiftLeftCommand)
ON_COMMAND (kShiftRightCmdID, OnShiftRightCommand)

ON_COMMAND (cmdChooseFontDialog, OnChooseFontCommand)

ON_UPDATE_COMMAND_UI_RANGE (cmdFontMenuFirst, cmdFontMenuLast, OnUpdateFontNameChangeCommand)
ON_COMMAND_RANGE (cmdFontMenuFirst, cmdFontMenuLast, OnFontNameChangeCommand)

ON_UPDATE_COMMAND_UI_RANGE (kBaseFontSizeCmdID, kLastFontSizeCmdID, OnUpdateFontSizeChangeCommand)
ON_COMMAND_RANGE (kBaseFontSizeCmdID, kLastFontSizeCmdID, OnFontSizeChangeCommand)
END_MESSAGE_MAP ()
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

LedLineItView::LedLineItView ()
    : inherited ()
    , fTabStopList (Led_TWIPS (1440 / 3))
    , fCachedLayoutWidth (kBadDistance)
#if qSupportSyntaxColoring
    , fSyntaxColoringMarkerOwner (NULL)
#endif
{
    SetDefaultFont (Options ().GetDefaultNewDocFont ());

    SetSmartCutAndPasteMode (Options ().GetSmartCutAndPaste ());
    SetControlArrowsScroll (true);
    SetScrollBarType (h, eScrollBarAlways);
    SetScrollBarType (v, eScrollBarAlways);

    /*
     *  No logic to picking these margins (I know of). Just picked them based on what looked good.
     *  You can comment this out entirely to get the default (zero margins).
     */
    const Led_TWIPS kLedItViewTopMargin    = Led_TWIPS (60);
    const Led_TWIPS kLedItViewBottomMargin = Led_TWIPS (0);
    const Led_TWIPS kLedItViewLHSMargin    = Led_TWIPS (60);
    const Led_TWIPS kLedItViewRHSMargin    = Led_TWIPS (60);
#if qPlatform_Windows
    // This SHOULD be available on other platforms, but only now done for WIN32
    SetDefaultWindowMargins (Led_TWIPS_Rect (kLedItViewTopMargin, kLedItViewLHSMargin, kLedItViewBottomMargin - kLedItViewTopMargin, kLedItViewRHSMargin - kLedItViewLHSMargin));
#endif

#if qPlatform_MacOS || qPlatform_Windows
    SetUseSecondaryHilight (true);
#endif
}

LedLineItView::~LedLineItView ()
{
    SpecifyTextStore (NULL);
    SetCommandHandler (NULL);
#if qIncludeBasicSpellcheckEngine
    SetSpellCheckEngine (NULL);
#endif
#if qSupportSyntaxColoring
    Assert (fSyntaxColoringMarkerOwner == NULL);
#endif
}

void LedLineItView::OnInitialUpdate ()
{
    inherited::OnInitialUpdate ();
    SpecifyTextStore (&GetDocument ().GetTextStore ());
    SetCommandHandler (&GetDocument ().GetCommandHandler ());
#if qIncludeBasicSpellcheckEngine
    SetSpellCheckEngine (&LedLineItApplication::Get ().fSpellCheckEngine);
#endif
}

#if qSupportSyntaxColoring
void LedLineItView::ResetSyntaxColoringTable ()
{
    if (PeekAtTextStore () != NULL) {
        //  static  const   TrivialRGBSyntaxAnalyzer            kAnalyzer;
        static const TableDrivenKeywordSyntaxAnalyzer kCPlusPlusAnalyzer (TableDrivenKeywordSyntaxAnalyzer::kCPlusPlusKeywords);
        static const TableDrivenKeywordSyntaxAnalyzer kVisualBasicAnalyzer (TableDrivenKeywordSyntaxAnalyzer::kVisualBasicKeywords);

        const SyntaxAnalyzer* analyzer = NULL;
        switch (Options ().GetSyntaxColoringOption ()) {
            case Options::eSyntaxColoringNone: /* nothing - analyzer already NULL*/
                break;
            case Options::eSyntaxColoringCPlusPlus:
                analyzer = &kCPlusPlusAnalyzer;
                break;
            case Options::eSyntaxColoringVB:
                analyzer = &kVisualBasicAnalyzer;
                break;
        }

        delete fSyntaxColoringMarkerOwner;
        fSyntaxColoringMarkerOwner = NULL;

        if (analyzer != NULL) {
#if qSupportOnlyMarkersWhichOverlapVisibleRegion
            fSyntaxColoringMarkerOwner = new WindowedSyntaxColoringMarkerOwner (*this, GetTextStore (), *analyzer);
#else
            fSyntaxColoringMarkerOwner = new SimpleSyntaxColoringMarkerOwner (*this, GetTextStore (), *analyzer);
#endif
            fSyntaxColoringMarkerOwner->RecheckAll ();
        }
    }
}

void LedLineItView::HookLosingTextStore ()
{
    delete fSyntaxColoringMarkerOwner;
    fSyntaxColoringMarkerOwner = NULL;
    inherited::HookLosingTextStore ();
}

void LedLineItView::HookGainedNewTextStore ()
{
    Assert (fSyntaxColoringMarkerOwner == NULL);
    inherited::HookGainedNewTextStore ();
    ResetSyntaxColoringTable ();
}
#endif

#if qSupportSyntaxColoring
vector<LedLineItView::RunElement> LedLineItView::SummarizeStyleMarkers (size_t from, size_t to) const
{
    // See SPR#1293 - may want to get rid of this eventually
    StyleMarkerSummarySinkForSingleOwner summary (*fSyntaxColoringMarkerOwner, from, to);
    GetTextStore ().CollectAllMarkersInRangeInto (from, to, TextStore::kAnyMarkerOwner, summary);
    return summary.ProduceOutputSummary ();
}

vector<LedLineItView::RunElement> LedLineItView::SummarizeStyleMarkers (size_t from, size_t to, const TextLayoutBlock& text) const
{
    StyleMarkerSummarySinkForSingleOwner summary (*fSyntaxColoringMarkerOwner, from, to, text);
    GetTextStore ().CollectAllMarkersInRangeInto (from, to, TextStore::kAnyMarkerOwner, summary);
    return summary.ProduceOutputSummary ();
}
#endif

size_t LedLineItView::GetCurUserLine () const
{
    // Either do caching here, or inside of GetRowContainingPosition () so this is quick.
    // Called ALOT - even for very large documents
    // LGP 970303
    return GetRowContainingPosition (GetSelectionStart ()) + 1; // unclear if we should use selstart or selEnd - pick arbitrarily...
}

void LedLineItView::SetCurUserLine (size_t newCurLine)
{
    // Assume line chosen 1 based, regardless of how Led is built. This is the most common UI.
    // And pin to ends if user choses too large / too small line number
    size_t selPos = 0;
    if (newCurLine >= 1) {
        if (newCurLine > GetRowCount ()) {
            newCurLine = GetRowCount ();
        }
        selPos = GetStartOfRow (newCurLine + (0 - 1));
    }
    size_t endPos = min (GetRealEndOfRowContainingPosition (selPos), GetLength ());
    SetSelection (selPos, endPos);
    ScrollToSelection ();
}

const LedLineItView::TabStopList& LedLineItView::GetTabStopList (size_t /*containingPos*/) const
{
    return fTabStopList;
}

void LedLineItView::TabletChangedMetrics ()
{
    inherited::TabletChangedMetrics ();

    Tablet_Acquirer tablet_ (this);
    Led_Tablet      tablet        = tablet_;
    fTabStopList.fTWIPSPerTabStop = tablet->CvtToTWIPSH (kCharsPerTab * GetFontMetricsAt (0).GetMaxCharacterWidth ());
    fCachedLayoutWidth            = kBadDistance;
#if qSupportSyntaxColoring
    if (fSyntaxColoringMarkerOwner != NULL) {
        fSyntaxColoringMarkerOwner->RecheckAll ();
    }
#endif
}

void LedLineItView::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    inherited::DidUpdateText (updateInfo);
    fCachedLayoutWidth = kBadDistance;
}

void LedLineItView::UpdateScrollBars ()
{
    // scrolling can change the longest row in window, so update our LayoutWidth
    fCachedLayoutWidth = kBadDistance;
    inherited::UpdateScrollBars ();
#if qSupportSyntaxColoring && qSupportOnlyMarkersWhichOverlapVisibleRegion
    if (fSyntaxColoringMarkerOwner != NULL) {
        fSyntaxColoringMarkerOwner->RecheckScrolling ();
    }
#endif
}

Led_Distance LedLineItView::ComputeMaxHScrollPos () const
{
    if (fCachedLayoutWidth == kBadDistance) {
        /*
         *  Figure the largest amount we might need to scroll given the current windows contents.
         *  But take into account where we've scrolled so far, and never invalidate that
         *  scroll amount. Always leave at least as much layout-width as needed to
         *  preserve the current scroll-to position.
         */
        Led_Distance width = CalculateLongestRowInWindowPixelWidth ();
        if (GetHScrollPos () != 0) {
            width = max (width, GetHScrollPos () + GetWindowRect ().GetWidth ());
        }
        fCachedLayoutWidth = max (width, Led_Distance (1));
    }
    Led_Distance wWidth = GetWindowRect ().GetWidth ();
    if (fCachedLayoutWidth > wWidth) {
        return (fCachedLayoutWidth - wWidth);
    }
    else {
        return 0;
    }
}

void LedLineItView::OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed)
{
    if (theChar == '\t' and Options ().GetTreatTabAsIndentChar ()) {
        // Check if the selection looks like its likely an auto-indent situation...
        bool shiftSituation = GetSelectionStart () != GetSelectionEnd ();
        if (shiftSituation) {
            // If we select an entire line, or multiple lines - then the user likely intended to auto-indent. But if the selection
            // is strictly INTRALINE - he probably intedned to replace the selected text with a tab.
            size_t selStart         = GetSelectionStart ();
            size_t selEnd           = GetSelectionEnd ();
            size_t selStartRowStart = GetStartOfRowContainingPosition (selStart);
            size_t selEndRowStart   = GetStartOfRowContainingPosition (selEnd);
            size_t selEndRowEnd     = GetEndOfRowContainingPosition (selEnd);

            // The logic is simpler to understand this way - but terser to express in the negative - I choose clarity over brevity here...
            if (selStartRowStart != selEndRowStart) {
                // OK - in case of multiple rows - we shift
            }
            else if (selStartRowStart == selEndRowStart and selStart == selStartRowStart and selEnd == selEndRowEnd) {
                // if its a single line - but fully selected- then OK - we shift
            }
            else {
                // otherwise - its an intrarow selection - and we treat that as NOT a shift - but just a replace
                shiftSituation = false;
            }
        }

        if (shiftSituation) {
            if (shiftPressed) {
                OnShiftLeftCommand ();
            }
            else {
                OnShiftRightCommand ();
            }
            return;
        }
    }

    inherited::OnTypedNormalCharacter (theChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);
    if (theChar == '\n' and GetSelectionStart () == GetSelectionEnd () and Options ().GetAutoIndent ()) {
        // Find the "indent level" of the previous line, and insert that much space here.
        size_t rowStart     = GetStartOfRowContainingPosition (GetSelectionEnd ());
        size_t prevRowStart = GetStartOfPrevRowFromRowContainingPosition (GetSelectionEnd ());
        // Check first we're not on the first line - don't want to auto-indent there..
        if (rowStart != prevRowStart) {
            size_t prevRowEnd = GetEndOfRowContainingPosition (prevRowStart);
            Assert (prevRowEnd >= prevRowStart);
            size_t                      prevRowLen = prevRowEnd - prevRowStart;
            SmallStackBuffer<Led_tChar> buf (prevRowLen);
            CopyOut (prevRowStart, prevRowLen, buf);
            size_t nTChars = 0;
            for (size_t i = 0; i < prevRowLen; ++i) {
                // use ANY space characters to auto-indent - should only use SPACE and TAB?
                if (not IsASCIISpace (buf[i])) {
                    break;
                }
                nTChars++;
            }
            InteractiveReplace (buf, nTChars);
        }
    }
}

void LedLineItView::OnContextMenu (CWnd* /*pWnd*/, CPoint pt)
{
    CMenu menu;
    if (menu.LoadMenu (kContextMenu)) {
        CMenu* popup = menu.GetSubMenu (0);
        AssertNotNull (popup);
        popup->TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, ::AfxGetMainWnd ());
    }
}

BOOL LedLineItView::IsSelected (const CObject* pDocItem) const
{
    // The implementation below is adequate if your selection consists of
    //  only LedLineItControlItem objects.  To handle different selection
    //  mechanisms, the implementation here should be replaced.

    // TODO: implement this function that tests for a selected OLE client item
    //return pDocItem == GetSoleSelectedOLEEmbedding ();
    [[maybe_unused]] BOOL test = inherited::IsSelected (pDocItem);
    return false; // should I even need to override this? Probably NO!!!
}

void LedLineItView::OnUpdateFontNameChangeCommand (CCmdUI* pCmdUI)
{
    RequireNotNull (pCmdUI);

    // check the item iff it is the currently selected font.
    // But always enable them...
    Led_SDK_String fontName = CmdNumToFontName (pCmdUI->m_nID);

    pCmdUI->SetCheck (fontName == GetDefaultFont ().GetFontName ());
    pCmdUI->Enable (true);
}

void LedLineItView::OnFontNameChangeCommand (UINT cmdNum)
{
    Led_SDK_String                   fontName = CmdNumToFontName (cmdNum);
    Led_IncrementalFontSpecification applyFontSpec;
    applyFontSpec.SetFontName (fontName);
    SetDefaultFont (applyFontSpec);
}

void LedLineItView::OnUpdateFontSizeChangeCommand (CCmdUI* pCmdUI)
{
    RequireNotNull (pCmdUI);
    int chosenFontSize = FontCmdToSize (pCmdUI->m_nID);

    if (chosenFontSize == 0) {
        switch (pCmdUI->m_nID) {
            case kFontSizeSmallerCmdID:
            case kFontSizeLargerCmdID: {
                pCmdUI->Enable (true);
            } break;

            case kFontSizeOtherCmdID: {
                TCHAR nameBuf[1024];
                Characters::CString::Copy (nameBuf, NEltsOf (nameBuf), _T ("Other"));
                pCmdUI->SetCheck (false);
                {
                    int pointSize = GetDefaultFont ().GetPointSize ();
                    if (not IsPredefinedFontSize (pointSize)) {
                        Characters::CString::Cat (nameBuf, NEltsOf (nameBuf), _T (" ("));
                        TCHAR nBuf[100];
                        DISABLE_COMPILER_MSC_WARNING_START (4996)
                        _stprintf (nBuf, _T ("%d"), GetDefaultFont ().GetPointSize ());
                        DISABLE_COMPILER_MSC_WARNING_END (4996)
                        Characters::CString::Cat (nameBuf, NEltsOf (nameBuf), nBuf);
                        Characters::CString::Cat (nameBuf, NEltsOf (nameBuf), _T (")"));
                        pCmdUI->SetCheck (true);
                    }
                }
                Characters::CString::Cat (nameBuf, NEltsOf (nameBuf), _T ("..."));
                pCmdUI->SetText (nameBuf);
            } break;
        }
    }
    else {
        pCmdUI->SetCheck (GetDefaultFont ().GetPointSize () == chosenFontSize);
        pCmdUI->Enable (true);
    }
}

void LedLineItView::OnFontSizeChangeCommand (UINT cmdNum)
{
    Led_FontSpecification::FontSize chosenFontSize = FontCmdToSize (cmdNum);
    if (chosenFontSize == 0) {
        switch (cmdNum) {
            case kFontSizeSmallerCmdID: {
                Led_IncrementalFontSpecification applyFontSpec;
                applyFontSpec.SetPointSizeIncrement (-1);
                SetDefaultFont (applyFontSpec);
                return;
            } break;
            case kFontSizeLargerCmdID: {
                Led_IncrementalFontSpecification applyFontSpec;
                applyFontSpec.SetPointSizeIncrement (1);
                SetDefaultFont (applyFontSpec);
                return;
            } break;
            case kFontSizeOtherCmdID: {
                Led_Distance oldSize = GetDefaultFont ().GetPointSize ();
                chosenFontSize       = static_cast<Led_FontSpecification::FontSize> (PickOtherFontHeight (oldSize));
            } break;
        }
    }
    if (chosenFontSize != 0) {
        Led_IncrementalFontSpecification applyFontSpec;
        applyFontSpec.SetPointSize (chosenFontSize);
        SetDefaultFont (applyFontSpec);
    }
}

Led_Distance LedLineItView::PickOtherFontHeight (Led_Distance origHeight)
{
#if qPlatform_MacOS
    Led_StdDialogHelper_OtherFontSizeDialog dlg;
#elif qPlatform_Windows
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

LedLineItView::SearchParameters LedLineItView::GetSearchParameters () const
{
    return Options ().GetSearchParameters ();
}

void LedLineItView::SetSearchParameters (const SearchParameters& sp)
{
    Options ().SetSearchParameters (sp);
}

void LedLineItView::OnGotoLineCommand ()
{
    /*
     *  compute current line, and use that as the default for the dialog box. Put up the dlog box,
     *  prompting user. Then move the selection to that line (and scroll to selection).
     */
    size_t rowNumber = GetCurUserLine ();

    /*
     *  I had coded this before to check if the stats bar was shown, and treat this
     *  as jumping the focus to the status bar. But Simone and Doug barfed violently
     *  on this idea. So lets me more traditional.
     *      --  LGP 970315
     */
    GotoLineDialog dlg (rowNumber);

    if (dlg.DoModal () == IDOK) {
        SetCurUserLine (dlg.fResultLine);
    }
}

#if qSupportGenRandomCombosCommand
void LedLineItView::OnGenRandomCombosCommand ()
{
    vector<Led_tString> srcFrags;

    {
        size_t lastStart = static_cast<size_t> (-1);
        for (size_t i = GetStartOfRowContainingPosition (GetSelectionStart ());
             i < GetSelectionEnd () and lastStart != i;
             lastStart = i, i = GetStartOfNextRowFromRowContainingPosition (i)) {
            size_t                      rowEnd = GetEndOfRowContainingPosition (i);
            size_t                      rowLen = rowEnd - i;
            SmallStackBuffer<Led_tChar> buf (rowLen + 1);
            CopyOut (i, rowLen, buf);
            buf[rowLen] = '\0';
            srcFrags.push_back (static_cast<Led_tString> (buf));
        }
    }

    const unsigned   kGeneratedCount = 100;
    const unsigned   kMaxComboLen    = 4;
    set<Led_tString> resultFrags;
    if (srcFrags.size () != 0) {
        for (size_t i = 0; i < kGeneratedCount; ++i) {
            unsigned int n = (rand () % kMaxComboLen) + 1;
            Led_tString  tmpWord;
            for (size_t ni = 0; ni < n; ++ni) {
                // now pick a random srcWord
                unsigned int wi = (rand () % srcFrags.size ());
                tmpWord += srcFrags[wi];
            }
            resultFrags.insert (tmpWord);
        }
    }

    CDocManager* docMgr = AfxGetApp ()->m_pDocManager;
    AssertNotNull (docMgr);
    CDocTemplate* pTemplate = NULL;
    {
        POSITION p = docMgr->GetFirstDocTemplatePosition ();
        pTemplate  = docMgr->GetNextDocTemplate (p);
    }
    AssertNotNull (pTemplate);
    CDocument* newDoc = pTemplate->OpenDocumentFile (NULL);
    newDoc->SetTitle (newDoc->GetTitle () + Led_SDK_TCHAROF (" {generated-words}"));
    LedLineItDocument* lNewDoc = dynamic_cast<LedLineItDocument*> (newDoc);
    AssertNotNull (lNewDoc);
    TextStore& ts = lNewDoc->GetTextStore ();
    for (set<Led_tString>::const_iterator i = resultFrags.begin (); i != resultFrags.end (); ++i) {
        ts.Replace (ts.GetEnd (), ts.GetEnd (), (*i).c_str (), (*i).length ());
        ts.Replace (ts.GetEnd (), ts.GetEnd (), LED_TCHAR_OF ("\n"), 1);
    }
}
#endif

void LedLineItView::OnShiftLeftCommand ()
{
    OnShiftNCommand (false);
}

void LedLineItView::OnShiftRightCommand ()
{
    OnShiftNCommand (true);
}

void LedLineItView::OnShiftNCommand (bool shiftRight)
{
    BreakInGroupedCommands ();

    // Find the entire set of rows which encompass the selection
    PartitionMarker* firstPM = GetPartitionMarkerContainingPosition (GetSelectionStart ());
    PartitionMarker* lastPM  = GetPartitionMarkerContainingPosition (GetSelectionEnd ());
    AssertNotNull (firstPM);
    AssertNotNull (lastPM);
    if (lastPM != firstPM and lastPM->GetStart () == GetSelectionEnd ()) {
        lastPM = lastPM->GetPrevious ();
        AssertNotNull (lastPM);
    }

    TextInteractor::UndoableContextHelper undoContext (
        *this,
        shiftRight ? Led_SDK_TCHAROF ("Shift Right") : Led_SDK_TCHAROF ("Shift Left"),
        firstPM->GetStart (), min (lastPM->GetEnd (), GetTextStore ().GetEnd ()),
        GetSelectionStart (), GetSelectionEnd (),
        false);
    {
        SetSelection (firstPM->GetStart (), min (lastPM->GetEnd (), GetTextStore ().GetEnd ()));

        // Go through each PM, and either shift it right or left.
        for (PartitionMarker* pm = firstPM; pm != lastPM->GetNext (); pm = pm->GetNext ()) {
            if (shiftRight) {
                InteractiveReplace_ (pm->GetStart (), pm->GetStart (), LED_TCHAR_OF ("\t"), 1, false, false, eDefaultUpdate);
            }
            else {
                size_t                      pmLength     = pm->GetLength ();
                size_t                      lookAtLength = min (pmLength - 1, kCharsPerTab);
                SmallStackBuffer<Led_tChar> buf (lookAtLength);
                CopyOut (pm->GetStart (), lookAtLength, buf);
                size_t deleteLength = lookAtLength; // default to deleting all if all whitespace..
                for (size_t i = 0; i < lookAtLength; ++i) {
                    if (buf[i] == '\t') {
                        deleteLength = i + 1;
                        break;
                    }
                    if (not(IsASCIISpace (buf[i]))) {
                        deleteLength = i;
                        break;
                    }
                }

                // delete the first lookAtLength characters.
                InteractiveReplace_ (pm->GetStart (), pm->GetStart () + deleteLength, LED_TCHAR_OF (""), 0, false, false, eDefaultUpdate);
            }
        }

        // Select the entire set of rows updated.
        SetSelection (firstPM->GetStart (), min (lastPM->GetEnd (), GetTextStore ().GetEnd ()));
    }
    undoContext.CommandComplete ();

    BreakInGroupedCommands ();
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the container (not the server) causes the deactivation.
void LedLineItView::OnCancelEditCntr ()
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
void LedLineItView::OnSetFocus (CWnd* pOldWnd)
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

void LedLineItView::OnCancelEditSrvr ()
{
    GetDocument ().OnDeactivateUI (FALSE);
}

void LedLineItView::OnSelectAllCommand ()
{
    SetSelection (0, GetLength ());
}

void LedLineItView::OnChooseFontCommand ()
{
    // Copy each valid attribute into the LOGFONT to initialize the CFontDialog
    LOGFONT lf;
    (void)::memset (&lf, 0, sizeof (lf));
    {
        Characters::CString::Copy (lf.lfFaceName, NEltsOf (lf.lfFaceName), GetDefaultFont ().GetFontNameSpecifier ().fName);
        Assert (::_tcslen (lf.lfFaceName) < sizeof (lf.lfFaceName)); // cuz our cached entry - if valid - always short enuf...
    }
    lf.lfWeight    = (GetDefaultFont ().GetStyle_Bold ()) ? FW_BOLD : FW_NORMAL;
    lf.lfItalic    = (GetDefaultFont ().GetStyle_Italic ());
    lf.lfUnderline = (GetDefaultFont ().GetStyle_Underline ());
    lf.lfStrikeOut = (GetDefaultFont ().GetStyle_Strikeout ());

    lf.lfHeight = GetDefaultFont ().PeekAtTMHeight ();

    FontDlgWithNoColorNoStyles dlog (&lf);
    if (dlog.DoModal () == IDOK) {
        SetDefaultFont (Led_FontSpecification (*dlog.m_cf.lpLogFont));
    }
}

#ifdef _DEBUG
void LedLineItView::AssertValid () const
{
    inherited::AssertValid ();
}

void LedLineItView::Dump (CDumpContext& dc) const
{
    inherited::Dump (dc);
}

LedLineItDocument& LedLineItView::GetDocument () const // non-debug version is inline
{
    ASSERT (m_pDocument->IsKindOf (RUNTIME_CLASS (LedLineItDocument)));
    ASSERT_VALID (m_pDocument);
    return *(LedLineItDocument*)m_pDocument;
}
#endif //_DEBUG

/*
 ********************************************************************************
 ************************* FontDlgWithNoColorNoStyles ***************************
 ********************************************************************************
 */
FontDlgWithNoColorNoStyles::FontDlgWithNoColorNoStyles (LOGFONT* lf)
    : CFontDialog (lf, CF_SCREENFONTS | CF_SCALABLEONLY | CF_NOVERTFONTS)
{
    //  SPR#1369 : Either CF_SCALABLEONLY or CF_TTONLY seems to get rid of bad fonts that don't work right with tabs
}

BOOL FontDlgWithNoColorNoStyles::OnInitDialog ()
{
    BOOL result = CFontDialog::OnInitDialog ();

    // hide the widgets for the font-style choices
    // Gee - I hope Microslop doesn't change these values!!! LGP 970118

    // Font STYLE list LABEL
    if (GetDlgItem (stc2) != NULL) {
        GetDlgItem (stc2)->ShowWindow (SW_HIDE);
    }
    // Font STYLE list
    if (GetDlgItem (cmb2) != NULL) {
        GetDlgItem (cmb2)->ShowWindow (SW_HIDE);
    }

    // Script list LABEL
    if (GetDlgItem (stc7) != NULL) {
        GetDlgItem (stc7)->ShowWindow (SW_HIDE);
    }
    // Script list
    if (GetDlgItem (cmb5) != NULL) {
        GetDlgItem (cmb5)->ShowWindow (SW_HIDE);
    }
    return result;
}
