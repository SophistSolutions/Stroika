/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#pragma warning(push)
#pragma warning(disable : 5054)
#include <afxwin.h>
#pragma warning(pop)

#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/IO/FileSystem/FileUtils.h"

#include "Stroika/Frameworks/Led/ChunkedArrayTextStore.h"
#include "Stroika/Frameworks/Led/StdDialogs.h"

#include "DispIDs.h"
#include "FontMenu.h"
#include "Resource.h"
#include "UserConfigCommands.h"

#include "LedItView.h"

#if qFunnyDisplayInDesignMode
#include "Stroika/Frameworks/Led/HandySimple.h"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;

using SearchParameters = LedItView::SearchParameters;

namespace {
    struct Options_ {
        SearchParameters fSearchParameters{};
    };

    struct Options_Storage_IMPL_ {
        Options_Storage_IMPL_ ()
            : fOptionsFile_{
                  L"AppSettings"sv,
                  [] () -> ObjectVariantMapper {
                      ObjectVariantMapper mapper;

                      // really should use String, no longer Led_tString, but for now... (note this only works as is for wchar_t Led_tString
                      mapper.Add<Led_tString> (
                          [] (const ObjectVariantMapper& /*mapper*/, const Led_tString* obj) -> VariantValue {
                              return String{*obj};
                          },
                          [] (const ObjectVariantMapper& /*mapper*/, const VariantValue& d, Led_tString* intoObj) -> void {
                              *intoObj = d.As<String> ().As<Led_tString> ();
                          });
                      mapper.AddCommonType<vector<Led_tString>> ();

                      mapper.AddClass<SearchParameters> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                          {L"MatchString", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fMatchString)},
                          {L"WrapSearch", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fWrapSearch)},
                          {L"WholeWordSearch", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fWholeWordSearch)},
                          {L"CaseSensativeSearch", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fCaseSensativeSearch)},
                          {L"RecentMatchStrings", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SearchParameters, fRecentFindStrings)},
                      });

                      mapper.AddClass<Options_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                          {L"Search-Parameters", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Options_, fSearchParameters)},
                      });
                      return mapper;
                  }(),

                  OptionsFile::kDefaultUpgrader,

                  // override the default name mapper to assure folder created, since no installer for activex controls
                  [] (const String& moduleName, const String& fileSuffix) {
                      static const auto kDefaultMapper_ = OptionsFile::mkFilenameMapper (L"ActiveLedIt"sv);
                      String            fileName        = kDefaultMapper_ (moduleName, fileSuffix);
                      IO::FileSystem::CreateDirectoryForFile (fileName);
                      return fileName;
                  }}
            , fActualCurrentConfigData_ (fOptionsFile_.Read<Options_> (Options_{}))
        {
            Set (fActualCurrentConfigData_); // assure derived data (and changed fields etc) up to date
        }
        Options_ Get () const
        {
            return fActualCurrentConfigData_;
        }
        void Set (const Options_& v)
        {
            fActualCurrentConfigData_ = v;
            fOptionsFile_.Write (v);
        }

    private:
        OptionsFile fOptionsFile_;
        Options_    fActualCurrentConfigData_;
    };

    ModuleGetterSetter<Options_, Options_Storage_IMPL_> sOptions_;
}

class My_CMDNUM_MAPPING : public MFC_CommandNumberMapping {
public:
    My_CMDNUM_MAPPING ()
    {
        AddAssociation (kFindAgainCmd, LedItView::kFindAgain_CmdID);
        AddAssociation (kEnterFindStringCmd, LedItView::kEnterFindString_CmdID);
        AddAssociation (kReplaceCmd, LedItView::kReplace_CmdID);
        AddAssociation (kReplaceAgainCmd, LedItView::kReplaceAgain_CmdID);
        AddAssociation (kSpellCheckCmd, LedItView::kSpellCheck_CmdID);

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
        AddAssociation (kFontStyleStrikeoutCmd, LedItView::kFontStyleStrikeout_CmdID);
        AddAssociation (kSubScriptCmd, LedItView::kSubScriptCommand_CmdID);
        AddAssociation (kSuperScriptCmd, LedItView::kSuperScriptCommand_CmdID);
        AddAssociation (kChooseFontDialogCmd, LedItView::kChooseFontCommand_CmdID);

        AddAssociation (kInsertTableCmd, LedItView::kInsertTable_CmdID);
        AddAssociation (kInsertTableRowAboveCmd, LedItView::kInsertTableRowAbove_CmdID);
        AddAssociation (kInsertTableRowBelowCmd, LedItView::kInsertTableRowBelow_CmdID);
        AddAssociation (kInsertTableColBeforeCmd, LedItView::kInsertTableColBefore_CmdID);
        AddAssociation (kInsertTableColAfterCmd, LedItView::kInsertTableColAfter_CmdID);
        AddAssociation (kInsertURLCmd, LedItView::kInsertURL_CmdID);
        AddAssociation (kInsertSymbolCmd, LedItView::kInsertSymbol_CmdID);

        //              AddAssociation (kPropertiesForSelectionCmd,     LedItView::kSelectedEmbeddingProperties_CmdID);
        AddRangeAssociation (
            kFirstSelectedEmbeddingCmd, kLastSelectedEmbeddingCmd,
            LedItView::kFirstSelectedEmbedding_CmdID, LedItView::kLastSelectedEmbedding_CmdID);

// Not 100% sure why this are disabled??? But they were before in AL (as of 2003-04-04 - AL 3.1a6x so leave it for now)
#if 0
        AddAssociation (kHideSelectionCmd,              LedItView::kHideSelection_CmdID);
        AddAssociation (kUnHideSelectionCmd,            LedItView::kUnHideSelection_CmdID);
#endif
        AddAssociation (kRemoveTableRowsCmd, LedItView::kRemoveTableRows_CmdID);
        AddAssociation (kRemoveTableColumnsCmd, LedItView::kRemoveTableColumns_CmdID);

// Not 100% sure why this are disabled??? But they were before in AL (as of 2003-04-04 - AL 3.1a6x so leave it for now)
#if 0
        AddAssociation (kShowHideParagraphGlyphsCmd,    LedItView::kShowHideParagraphGlyphs_CmdID);
        AddAssociation (kShowHideTabGlyphsCmd,          LedItView::kShowHideTabGlyphs_CmdID);
        AddAssociation (kShowHideSpaceGlyphsCmd,        LedItView::kShowHideSpaceGlyphs_CmdID);
#endif
    }
};
My_CMDNUM_MAPPING sMy_CMDNUM_MAPPING;

struct ActiveLedIt_DialogSupport : TextInteractor::DialogSupport, WordProcessor::DialogSupport {
public:
    using CommandNumber = TextInteractor::DialogSupport::CommandNumber;

public:
    ActiveLedIt_DialogSupport ()
    {
        TextInteractor::SetDialogSupport (this);
        WordProcessor::SetDialogSupport (this);
    }
    ~ActiveLedIt_DialogSupport ()
    {
        WordProcessor::SetDialogSupport (NULL);
        TextInteractor::SetDialogSupport (NULL);
    }

//  TextInteractor::DialogSupport
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
        Led_StdDialogHelper_SpellCheckDialog       spellCheckDialog (delegator, ::AfxGetResourceHandle (), ::GetActiveWindow ());
#elif qStroika_FeatureSupported_XWindows
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
        return ::CmdNumToFontName (MFC_CommandNumberMapping::Get ().ReverseLookup (cmdNum)).c_str ();
    }
#if qSupportOtherFontSizeDlg
    virtual Led_Distance PickOtherFontHeight (Led_Distance origHeight) override
    {
#if qPlatform_MacOS
        Led_StdDialogHelper_OtherFontSizeDialog dlg;
#elif qPlatform_Windows
        Led_StdDialogHelper_OtherFontSizeDialog    dlg (::AfxGetResourceHandle (), ::GetActiveWindow ());
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
#if qSupportParagraphIndentsDlg
    virtual bool PickNewParagraphMarginsAndFirstIndent (Led_TWIPS* leftMargin, bool* leftMarginValid, Led_TWIPS* rightMargin, bool* rightMarginValid, Led_TWIPS* firstIndent, bool* firstIndentValid) override
    {
#if qPlatform_MacOS
        Led_StdDialogHelper_ParagraphIndentsDialog dlg;
#elif qPlatform_Windows
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
    virtual void ShowSimpleEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName) override
    {
        // unknown embedding...
        Led_StdDialogHelper_UnknownEmbeddingInfoDialog infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
        infoDialog.fEmbeddingTypeName = embeddingTypeName;
        (void)infoDialog.DoModal ();
    }
    virtual bool ShowURLEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName, Led_SDK_String* urlTitle, Led_SDK_String* urlValue) override
    {
        Led_StdDialogHelper_URLXEmbeddingInfoDialog infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
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
    }
    virtual bool ShowAddURLEmbeddingInfoDialog (Led_SDK_String* urlTitle, Led_SDK_String* urlValue) override
    {
        Led_StdDialogHelper_AddURLXEmbeddingInfoDialog infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
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
    }
#if qSupportAddNewTableDlg
    bool AddNewTableDialog (size_t* nRows, size_t* nCols)
    {
        RequireNotNull (nRows);
        RequireNotNull (nCols);
        Led_StdDialogHelper_AddNewTableDialog infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
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
        DLGTYPE                                    infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
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
static ActiveLedIt_DialogSupport sActiveLedIt_DialogSupport;

static BOOL CALLBACK _AfxAbortProc (HDC, int)
{
    _AFX_WIN_STATE* pWinState = _afxWinState;
    MSG             msg;
    while (!pWinState->m_bUserAbort &&
           ::PeekMessage (&msg, NULL, NULL, NULL, PM_NOREMOVE)) {
        if (!AfxGetThread ()->PumpMessage ())
            return FALSE; // terminate if WM_QUIT received
    }
    return !pWinState->m_bUserAbort;
}

/*
 ********************************************************************************
 ************************** LedItViewController *********************************
 ********************************************************************************
 */
LedItViewController::LedItViewController ()
    : fTextStore ()
    , fCommandHandler (kMaxNumUndoLevels)
    , fHidableTextDatabase ()

{
    fHidableTextDatabase = WordProcessor::HidableTextDatabasePtr (new ColoredUniformHidableTextMarkerOwner (fTextStore));
}

LedItViewController::~LedItViewController ()
{
}

/*
 ********************************************************************************
 ************************************ LedItView *********************************
 ********************************************************************************
 */
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4407) // Not sure this is safe to ignore but I think it is due to qMFCRequiresCWndLeftmostBaseClass
#endif
BEGIN_MESSAGE_MAP (LedItView, LedItView::inherited)
ON_WM_MOUSEACTIVATE ()
ON_WM_INITMENUPOPUP ()
ON_WM_CONTEXTMENU ()
ON_WM_CHAR ()
ON_WM_KEYDOWN ()
ON_WM_KEYUP ()
ON_WM_SYSKEYDOWN ()
ON_WM_SYSKEYUP ()
ON_WM_MOUSEMOVE ()
ON_WM_LBUTTONDOWN ()
ON_WM_LBUTTONUP ()
ON_WM_LBUTTONDBLCLK ()
ON_WM_RBUTTONDOWN ()
ON_WM_RBUTTONUP ()

ON_COMMAND (ID_FILE_PRINT, OnFilePrint)
ON_COMMAND (ID_FILE_PRINT_SETUP, OnFilePrintSetup)
ON_COMMAND (ID_HELP_FINDER, OnBrowseHelpCommand)
ON_COMMAND (kCheckForUpdatesWebPageCmdID, OnCheckForUpdatesWebPageCommand)
ON_COMMAND (ID_APP_ABOUT, OnAboutBoxCommand)
ON_COMMAND (kPasteFormatTextCmdID, OnPasteAsTextCommand)
ON_UPDATE_COMMAND_UI (kPasteFormatTextCmdID, OnUpdatePasteAsTextCommand)
ON_COMMAND_RANGE (kFirstOLEUserCmdCmdID, kLastOLEUserCmdCmdID, OnOLEUserCommand)
ON_UPDATE_COMMAND_UI_RANGE (kFirstOLEUserCmdCmdID, kLastOLEUserCmdCmdID, OnUpdateOLEUserCommand)
END_MESSAGE_MAP ()
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

LedItView::LedItView ()
    : inherited ()
    , fController (NULL)
    , fSupportContextMenu (true)
    , fHideDisabledContextMenuItems (true)
    , fWrapToWindow (true)
    , fMaxLength (-1)
    , fEnableAutoChangesBackgroundColor (true)
    , fInDrawMetaFileMode (false)
{
    SetScrollBarType (v, eScrollBarAsNeeded);
    SetScrollBarType (h, eScrollBarAsNeeded);
    if (qFunnyDisplayInDesignMode) {
        SetUseBitmapScrollingOptimization (false);
    }
    SetUseSecondaryHilight (true); // default to TRUE since I think this looks better and maybe a differentiator with other controls
    const Led_TWIPS kLedItViewTopMargin    = Led_TWIPS (120);
    const Led_TWIPS kLedItViewBottomMargin = Led_TWIPS (0);
    const Led_TWIPS kLedItViewLHSMargin    = Led_TWIPS (150);
    const Led_TWIPS kLedItViewRHSMargin    = Led_TWIPS (0);
    SetDefaultWindowMargins (Led_TWIPS_Rect (kLedItViewTopMargin, kLedItViewLHSMargin, kLedItViewBottomMargin - kLedItViewTopMargin, kLedItViewRHSMargin - kLedItViewLHSMargin));
}

LedItView::~LedItView ()
{
    SetController (NULL);
}

void LedItView::SetController (LedItViewController* controller)
{
    if (fController != NULL) {
        SetHidableTextDatabase (NULL);
        SetCommandHandler (NULL);
        SpecifyTextStore (NULL);
    }
    fController = controller;
    if (fController != NULL) {
        SpecifyTextStore (&fController->fTextStore);
        SetCommandHandler (&fController->fCommandHandler);
        SetHidableTextDatabase (fController->fHidableTextDatabase);
    }
}

void LedItView::SetSupportContextMenu (bool allowContextMenu)
{
    fSupportContextMenu = allowContextMenu;
}

void LedItView::SetHideDisabledContextMenuItems (bool hideDisabledContextMenuItems)
{
    fHideDisabledContextMenuItems = hideDisabledContextMenuItems;
}

void LedItView::SetWrapToWindow (bool wrapToWindow)
{
    if (fWrapToWindow != wrapToWindow) {
        fWrapToWindow = wrapToWindow;
        InvalidateAllCaches ();
        Refresh ();
    }
}

void LedItView::SetMaxLength (long maxLength)
{
    fMaxLength = maxLength;
}

void LedItView::GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const
{
    if (GetWrapToWindow ()) {
        // Make the layout right margin of each line (paragraph) equal to the windowrect. Ie, wrap to the
        // edge of the window. NB: because of this choice, we must 'InvalidateAllCaches' when the
        // WindowRect changes in our SetWindowRect() override.
        Led_Coordinate l = 0;
        Led_Coordinate r = 0;
        inherited::GetLayoutMargins (row, &l, &r);
        r = max (static_cast<Led_Coordinate> (GetWindowRect ().GetWidth ()), l + 1);
        Ensure (r > l);
        if (lhs != NULL) {
            *lhs = l;
        }
        if (rhs != NULL) {
            *rhs = r;
        }
    }
    else {
        inherited::GetLayoutMargins (row, lhs, rhs);
    }
}

void LedItView::SetWindowRect (const Led_Rect& windowRect)
{
    // Hook all changes in the window width, so we can invalidate the word-wrap info (see LedItView::GetLayoutMargins)
    Led_Rect oldWindowRect = GetWindowRect ();
    if (windowRect != oldWindowRect) {
        WordWrappedTextImager::SetWindowRect (windowRect); //  NB: use XX instead of inherited to avoid infinite recurse, due to sloppy mixin ambiguity resoltion in base classes (LGP990623)
        if (GetWrapToWindow () and windowRect.GetSize () != oldWindowRect.GetSize ()) {
            InvalidateAllCaches ();
        }
    }
}

Led_Distance LedItView::CalculateFarthestRightMarginInWindow () const
{
    if (fWrapToWindow) {
        return GetWindowRect ().GetWidth ();
    }
    else {
        return inherited::CalculateFarthestRightMarginInWindow () / 20;
    }
}

void LedItView::PostNcDestroy ()
{
}

int LedItView::OnMouseActivate (CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
    if (fController != NULL) {
        fController->ForceUIActive ();
    }
    return inherited::OnMouseActivate (pDesktopWnd, nHitTest, message);
}

void LedItView::OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    // Disable inappropriate commands.
    // Remove disabled menu items from the popup (to save space).
    // Remove consecutive (or leading) separators as useless.

    CCmdUI state;
    state.m_pSubMenu = NULL;
    state.m_pMenu    = pPopupMenu;
    Assert (state.m_pOther == NULL);
    Assert (state.m_pParentMenu == NULL);

    // Set the enable/disable state of each menu item.
    state.m_nIndexMax = pPopupMenu->GetMenuItemCount ();
    for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++) {
        state.m_nID = pPopupMenu->GetMenuItemID (state.m_nIndex);
        if (state.m_nID != 0) {
            Assert (state.m_pOther == NULL);
            Assert (state.m_pMenu != NULL);
            state.DoUpdate (this, true);
        }
    }

    // Remove disabled items (and unneeded separators)
    if (GetHideDisabledContextMenuItems ()) {
        bool prevItemSep = true; // prevent initial separators
        for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;) {
            state.m_nID = pPopupMenu->GetMenuItemID (state.m_nIndex);
            if (state.m_nID == 0 and prevItemSep) {
                pPopupMenu->RemoveMenu (state.m_nIndex, MF_BYPOSITION);
                state.m_nIndexMax--;
                continue;
            }
            if (state.m_nID != 0) {
                MENUITEMINFO mInfo;
                memset (&mInfo, 0, sizeof (mInfo));
                mInfo.cbSize = sizeof (mInfo);
                mInfo.fMask  = MIIM_STATE;
                Verify (::GetMenuItemInfo (pPopupMenu->GetSafeHmenu (), state.m_nIndex, true, &mInfo));
                if (mInfo.fState & MFS_DISABLED) {
                    pPopupMenu->RemoveMenu (state.m_nIndex, MF_BYPOSITION);
                    state.m_nIndexMax--;
                    continue;
                }
            }
            prevItemSep = bool (state.m_nID == 0);
            state.m_nIndex++;
        }
        // if LAST item is a separator - remove THAT
        if (prevItemSep) {
            pPopupMenu->RemoveMenu (state.m_nIndexMax - 1, MF_BYPOSITION);
        }
    }

    inherited::OnInitMenuPopup (pPopupMenu, nIndex, bSysMenu);
}

void LedItView::OnContextMenu (CWnd* /*pWnd*/, CPoint pt)
{
    if (GetSupportContextMenu ()) {
        CMenu menu;
        AssertNotNull (fController);
        if (menu.Attach (fController->GenerateContextMenu ())) {
            menu.TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
        }
    }
}
extern short AFXAPI _AfxShiftState ();

void LedItView::OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (fController != NULL) {
        USHORT shortChar = static_cast<USHORT> (nChar);
        fController->FireKeyDown (&shortChar, _AfxShiftState ());
    }
    inherited::OnSysKeyDown (nChar, nRepCnt, nFlags);
}

void LedItView::OnSysKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (fController != NULL) {
        USHORT shortChar = static_cast<USHORT> (nChar);
        fController->FireKeyUp (&shortChar, _AfxShiftState ());
    }
    inherited::OnSysKeyUp (nChar, nRepCnt, nFlags);
}

void LedItView::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (fController != NULL) {
        USHORT shortChar = static_cast<USHORT> (nChar);
        fController->FireKeyDown (&shortChar, _AfxShiftState ());
    }
    inherited::OnKeyDown (nChar, nRepCnt, nFlags);
}

void LedItView::OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (fController != NULL) {
        USHORT shortChar = static_cast<USHORT> (nChar);
        fController->FireKeyUp (&shortChar, _AfxShiftState ());
    }
    inherited::OnKeyUp (nChar, nRepCnt, nFlags);
}

void LedItView::OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed)
{
    if (fController != NULL) {
        USHORT shortChar = theChar;
        fController->FireKeyPress (&shortChar);
    }
    inherited::OnTypedNormalCharacter (theChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);
}

void LedItView::OnLButtonDown (UINT nFlags, CPoint oPoint)
{
    if (fController != NULL) {
        fController->FireMouseDown (LEFT_BUTTON, _AfxShiftState (), oPoint.x, oPoint.y);
    }
    inherited::OnLButtonDown (nFlags, oPoint);
}

void LedItView::OnLButtonUp (UINT nFlags, CPoint oPoint)
{
    if (fController != NULL) {
        fController->FireMouseUp (LEFT_BUTTON, _AfxShiftState (), oPoint.x, oPoint.y);
        if (GetCurClickCount () == 1) {
            fController->FireClick ();
        }
    }
    inherited::OnLButtonUp (nFlags, oPoint);
}

void LedItView::OnLButtonDblClk (UINT nFlags, CPoint oPoint)
{
    if (fController != NULL) {
        fController->FireDblClick ();
    }
    inherited::OnLButtonDblClk (nFlags, oPoint);
}

void LedItView::OnRButtonDown (UINT nFlags, CPoint oPoint)
{
    if (fController != NULL) {
        fController->FireMouseDown (RIGHT_BUTTON, _AfxShiftState (), oPoint.x, oPoint.y);
    }
    inherited::OnRButtonDown (nFlags, oPoint);
}

void LedItView::OnRButtonUp (UINT nFlags, CPoint oPoint)
{
    if (fController != NULL) {
        fController->FireMouseUp (RIGHT_BUTTON, _AfxShiftState (), oPoint.x, oPoint.y);
    }
    inherited::OnRButtonUp (nFlags, oPoint);
}

void LedItView::OnMouseMove (UINT nFlags, CPoint oPoint)
{
    if (fController != NULL) {
        fController->FireMouseMove (LEFT_BUTTON, _AfxShiftState (), oPoint.x, oPoint.y);
    }
    inherited::OnMouseMove (nFlags, oPoint);
}

void LedItView::OnPasteAsTextCommand ()
{
    InteractiveModeUpdater iuMode (*this);
    BreakInGroupedCommands ();

    if (OnPasteCommand_Before ()) {
        try {
            UndoableContextHelper undoContext (*this, Led_SDK_TCHAROF ("Paste Text"), false);
            {
                OnPasteCommand_PasteFlavor_Specific (kTEXTClipFormat);
            }
            undoContext.CommandComplete ();
        }
        catch (...) {
            OnPasteCommand_After ();
            throw;
        }
        OnPasteCommand_After ();
    }
    BreakInGroupedCommands ();
}

void LedItView::OnUpdatePasteAsTextCommand (CCmdUI* pCmdUI)
{
    OnUpdatePasteCommand (Led_MFC_TmpCmdUpdater (pCmdUI));
}

void LedItView::OnOLEUserCommand (UINT nID)
{
    if (fController != NULL) {
        fController->FireUserCommand (UserCommandNameNumberRegistry::Get ().Lookup (nID));
    }
}

void LedItView::OnUpdateOLEUserCommand (CCmdUI* pCmdUI)
{
    if (fController != NULL) {
        VARIANT_BOOL enabled = true;
        VARIANT_BOOL checked = false;
        wstring      name;
        // SHOULD DO BETTER TO GRAB THIS VALUE FROM THE pCMDUI GUY- BUT NOT CRITICAL...
        fController->FireUpdateUserCommand (UserCommandNameNumberRegistry::Get ().Lookup (pCmdUI->m_nID), &enabled, &checked, &name);
        pCmdUI->Enable (enabled);
        pCmdUI->SetCheck (checked);
        if (not name.empty ()) {
            pCmdUI->SetText (Led_Wide2SDKString (name).c_str ());
        }
    }
}

LedItView::SearchParameters LedItView::GetSearchParameters () const
{
    return sOptions_.Get ().fSearchParameters;
}

void LedItView::SetSearchParameters (const SearchParameters& sp)
{
    sOptions_.Update ([=] (Options_ d) { d.fSearchParameters = sp; return d; });
}

void LedItView::SetSelection (size_t start, size_t end)
{
    bool changed = start != GetSelectionStart () or end != GetSelectionEnd ();
    inherited::SetSelection (start, end);
    if (changed and fController != NULL) {
        fController->FireOLEEvent (DISPID_SelChange);
    }
}

void LedItView::AboutToUpdateText (const UpdateInfo& updateInfo)
{
    if (GetMaxLength () != -1) {
        long textAdded = static_cast<long> (updateInfo.fTextLength) - static_cast<long> (updateInfo.fReplaceTo - updateInfo.fReplaceFrom);
        if (textAdded + static_cast<long> (GetLength ()) > GetMaxLength ()) {
            throw "";
        }
    }

    inherited::AboutToUpdateText (updateInfo);
}

void LedItView::EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing)
{
    if (fEnableAutoChangesBackgroundColor) {
        inherited::EraseBackground (tablet, subsetToDraw, printing);
    }
    else {
        TextImager::EraseBackground (tablet, subsetToDraw, printing);
    }

#if qFunnyDisplayInDesignMode
    if (fController->IsInDesignMode ()) {
        Led_tString              message = LED_TCHAR_OF ("Design Mode");
        static WaterMarkHelper<> waterMarkerImager (message); // make this static - just as a performance hack. Also could be an instance variable of 'this'.
        waterMarkerImager.SetWatermarkColor (Led_Color::kYellow);
        Led_Rect designModeRect = Led_Rect (0, 0, 20, 150);
        Led_Rect wr             = GetWindowRect ();
        {
            if (static_cast<Led_Distance> (designModeRect.bottom) > wr.GetHeight ()) {
                designModeRect.bottom = wr.GetHeight ();
            }
            if (static_cast<Led_Distance> (designModeRect.right) > wr.GetWidth ()) {
                designModeRect.right = wr.GetWidth ();
            }
            designModeRect        = CenterRectInRect (designModeRect, wr);
            Led_Distance h        = designModeRect.GetHeight ();
            designModeRect.bottom = wr.bottom;
            designModeRect.top    = designModeRect.bottom - h;
        }
        waterMarkerImager.DrawWatermark (tablet, designModeRect, subsetToDraw);
        if (fController->DrawExtraDesignModeBorder ()) {
            Led_Color useBorderColor = Led_Color::kBlack;
            {
                Led_Color defBackgrndColor = Led_GetTextBackgroundColor ();
                if (GetHWND () != NULL) {
                    DWORD dwStyle = GetStyle ();
                    if (((dwStyle & WS_DISABLED) or (dwStyle & ES_READONLY)) and (not printing)) {
                        defBackgrndColor = Led_Color (::GetSysColor (COLOR_BTNFACE));
                    }
                }
                // opposite of background color
                useBorderColor = Led_Color (~defBackgrndColor.GetRed (), ~defBackgrndColor.GetGreen (), ~defBackgrndColor.GetBlue ());
            }

            Led_Rect wmr = tablet->CvtFromTWIPS (GetDefaultWindowMargins ());
            Led_Rect cr  = wr;
            cr.top -= wmr.GetTop ();
            cr.left -= wmr.GetLeft ();
            cr.bottom += wmr.GetBottom ();
            cr.right += wmr.GetRight ();
            tablet->FrameRectangle (cr, useBorderColor, 1);
        }
    }
#endif
}

long LedItView::OLE_FindReplace (long searchFrom, const Led_tString& findText, const Led_tString& replaceText, BOOL wrapSearch, BOOL wholeWordSearch, BOOL caseSensativeSearch)
{
    TextStore::SearchParameters parameters;
    parameters.fMatchString         = findText;
    parameters.fWrapSearch          = !!wrapSearch;
    parameters.fWholeWordSearch     = !!wholeWordSearch;
    parameters.fCaseSensativeSearch = !!caseSensativeSearch;

    size_t whereTo = GetTextStore ().Find (parameters, searchFrom);
    if (whereTo != kBadIndex) {
        size_t                                replaceStart = whereTo;
        size_t                                replaceEnd   = whereTo + parameters.fMatchString.length ();
        TextInteractor::UndoableContextHelper undoContext (*this, TextInteractor::GetCommandNames ().fReplaceCommandName, replaceStart, replaceEnd, GetSelectionStart (), GetSelectionEnd (), false);
        {
            InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), replaceText.c_str (), replaceText.length ());
        }
        undoContext.CommandComplete ();
        return static_cast<long> (whereTo + replaceText.length ());
    }

    return -1;
}

void LedItView::UpdateScrollBars ()
{
    if (fInDrawMetaFileMode) {
        return; // ignore while in print mode...
    }
    inherited::UpdateScrollBars ();
}

void LedItView::OnBrowseHelpCommand ()
{
    if (fController != NULL) {
        fController->OnBrowseHelpCommand ();
    }
}

void LedItView::OnCheckForUpdatesWebPageCommand ()
{
    Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/CheckForUpdates.asp", kAppName, ""));
}

void LedItView::OnAboutBoxCommand ()
{
    if (fController != NULL) {
        fController->OnAboutBoxCommand ();
    }
}

void LedItView::OnFilePrintOnce ()
{
    DoPrintHelper (false);
}

void LedItView::OnFilePrint ()
{
    DoPrintHelper (true);
}

void LedItView::OnFilePrintSetup ()
{
    CPrintDialog pd (TRUE);
    AfxGetApp ()->DoPrintDialog (&pd);
}

/*
 *  Based on CView::OnFilePrint() - but with one bug fixed (at least originally). That code was buggy when used in an OCX cuz there
 *  is no doc and no frame window (perhaps I could have created one - but I wasn't sure what other bugs/problems that might create).
 *
 *      Then - when I tried to compile that code here - there were all sorts of things that wouldn't compile. So I had to make a bunch of
 *  OTHER changes due to the fact that I don't include all the private MFC headers here. And as long as I was making such bug changes - I threw away
 *  a bunch of detritus as well.
 *
 *      -   LGP 2001-04-17
 */
void LedItView::DoPrintHelper (bool showPrintDlg)
{
    class CPrintingDialog : public CDialog {
    public:
        enum { IDD = AFX_IDD_PRINTDLG };
        CPrintingDialog (CWnd* pParent)
        {
            Create (CPrintingDialog::IDD, pParent); // modeless !
            _afxWinState->m_bUserAbort = FALSE;
        }
        virtual ~CPrintingDialog () {}
        virtual BOOL OnInitDialog ()
        {
            SetWindowText (AfxGetAppName ());
            CenterWindow ();
            return CDialog::OnInitDialog ();
        }
        virtual void OnCancel ()
        {
            _afxWinState->m_bUserAbort = TRUE; // flag that user aborted print
            CDialog::OnCancel ();
        }
    };

    // get default print info
    CPrintInfo printInfo;
    ASSERT (printInfo.m_pPD != NULL); // must be set

    if (not showPrintDlg) {
        printInfo.m_bDirect = true;
        printInfo.m_dwFlags &= ~PRINTFLAG_PROMPTUSER;
    }

    if (OnPreparePrinting (&printInfo)) {
        // hDC must be set (did you remember to call DoPreparePrinting?)
        ASSERT (printInfo.m_pPD->m_pd.hDC != NULL);

        // gather file to print to if print-to-file selected
        CString strOutput;
        if (printInfo.m_pPD->m_pd.Flags & PD_PRINTTOFILE && !printInfo.m_bDocObject) {
            // construct CFileDialog for browsing
            CString     strDef (MAKEINTRESOURCE (AFX_IDS_PRINTDEFAULTEXT));
            CString     strPrintDef (MAKEINTRESOURCE (AFX_IDS_PRINTDEFAULT));
            CString     strFilter (MAKEINTRESOURCE (AFX_IDS_PRINTFILTER));
            CString     strCaption (MAKEINTRESOURCE (AFX_IDS_PRINTCAPTION));
            CFileDialog dlg (FALSE, strDef, strPrintDef,
                             OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);
            dlg.m_ofn.lpstrTitle = strCaption;

            if (dlg.DoModal () != IDOK)
                return;

            // set output device to resulting path name
            strOutput = dlg.GetPathName ();
        }

        // set up document info and start the document printing process
        CString strTitle = "ActiveLedIt! document";
        DOCINFO docInfo;
        memset (&docInfo, 0, sizeof (DOCINFO));
        docInfo.cbSize      = sizeof (DOCINFO);
        docInfo.lpszDocName = strTitle;
        CString strPortName;
        int     nFormatID;
        if (strOutput.IsEmpty ()) {
            docInfo.lpszOutput = NULL;
            strPortName        = printInfo.m_pPD->GetPortName ();
            nFormatID          = AFX_IDS_PRINTONPORT;
        }
        else {
            docInfo.lpszOutput = strOutput;
            extern UINT AFXAPI AfxGetFileTitle (LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);
            AfxGetFileTitle (strOutput, strPortName.GetBuffer (_MAX_PATH), _MAX_PATH);
            nFormatID = AFX_IDS_PRINTTOFILE;
        }

        // setup the printing DC
        CDC dcPrint;
        if (!printInfo.m_bDocObject) {
            dcPrint.Attach (printInfo.m_pPD->m_pd.hDC); // attach printer dc
            dcPrint.m_bPrinting = TRUE;
        }
        OnBeginPrinting (&dcPrint, &printInfo);

        if (!printInfo.m_bDocObject) {
            dcPrint.SetAbortProc (_AfxAbortProc);
        }

        // disable main window while printing & init printing status dialog
        HWND mainWnd = AfxGetMainWnd ()->m_hWnd;
        ::EnableWindow (mainWnd, FALSE);
        CPrintingDialog dlgPrintStatus (this);
        BOOL            bError = FALSE;
        try {
            CString strTemp;
            dlgPrintStatus.SetDlgItemText (AFX_IDC_PRINT_DOCNAME, strTitle);
            dlgPrintStatus.SetDlgItemText (AFX_IDC_PRINT_PRINTERNAME, printInfo.m_pPD->GetDeviceName ());
            AfxFormatString1 (strTemp, nFormatID, strPortName);
            dlgPrintStatus.SetDlgItemText (AFX_IDC_PRINT_PORTNAME, strTemp);
            dlgPrintStatus.ShowWindow (SW_SHOW);
            dlgPrintStatus.UpdateWindow ();

            // start document printing process
            if (!printInfo.m_bDocObject && dcPrint.StartDoc (&docInfo) == SP_ERROR) {
                // enable main window before proceeding
                ::EnableWindow (mainWnd, true);

                // cleanup and show error message
                OnEndPrinting (&dcPrint, &printInfo);
                dlgPrintStatus.DestroyWindow ();
                dcPrint.Detach (); // will be cleaned up by CPrintInfo destructor
                AfxMessageBox (AFX_IDP_FAILED_TO_START_PRINT);
                return;
            }

            // Guarantee values are in the valid range
            UINT nEndPage   = printInfo.GetToPage ();
            UINT nStartPage = printInfo.GetFromPage ();

            if (nEndPage < printInfo.GetMinPage ())
                nEndPage = printInfo.GetMinPage ();
            if (nEndPage > printInfo.GetMaxPage ())
                nEndPage = printInfo.GetMaxPage ();

            if (nStartPage < printInfo.GetMinPage ())
                nStartPage = printInfo.GetMinPage ();
            if (nStartPage > printInfo.GetMaxPage ())
                nStartPage = printInfo.GetMaxPage ();

            int nStep = (nEndPage >= nStartPage) ? 1 : -1;
            nEndPage  = (nEndPage == 0xffff) ? 0xffff : nEndPage + nStep;

            VERIFY (strTemp.LoadString (AFX_IDS_PRINTPAGENUM));

            // If it's a doc object, we don't loop page-by-page
            // because doc objects don't support that kind of levity.

            if (printInfo.m_bDocObject) {
                OnPrepareDC (&dcPrint, &printInfo);
                OnPrint (&dcPrint, &printInfo);
            }
            else {
                // begin page printing loop
                for (printInfo.m_nCurPage = nStartPage; printInfo.m_nCurPage != nEndPage; printInfo.m_nCurPage += nStep) {
                    OnPrepareDC (&dcPrint, &printInfo);

                    // check for end of print
                    if (!printInfo.m_bContinuePrinting)
                        break;

                    // write current page
                    TCHAR szBuf[80];
                    wsprintf (szBuf, strTemp, printInfo.m_nCurPage);
                    dlgPrintStatus.SetDlgItemText (AFX_IDC_PRINT_PAGENUM, szBuf);

                    // set up drawing rect to entire page (in logical coordinates)
                    printInfo.m_rectDraw.SetRect (0, 0,
                                                  dcPrint.GetDeviceCaps (HORZRES),
                                                  dcPrint.GetDeviceCaps (VERTRES));
                    dcPrint.DPtoLP (&printInfo.m_rectDraw);

                    // attempt to start the current page
                    if (dcPrint.StartPage () < 0) {
                        bError = TRUE;
                        break;
                    }

                    // must call OnPrepareDC on newer versions of Windows because
                    // StartPage now resets the device attributes.
                    BOOL bMarked4 = false;
                    {
                        DISABLE_COMPILER_MSC_WARNING_START (4996)
                        DWORD dwVersion = ::GetVersion ();
                        BOOL  bWin4     = (BYTE)dwVersion >= 4;
                        // determine various metrics based on EXE subsystem version mark
                        if (bWin4) {
                            bMarked4 = (GetProcessVersion (0) >= 0x00040000);
                        }
                        DISABLE_COMPILER_MSC_WARNING_END (4996)
                    }
                    if (bMarked4)
                        OnPrepareDC (&dcPrint, &printInfo);

                    ASSERT (printInfo.m_bContinuePrinting);

                    // page successfully started, so now render the page
                    OnPrint (&dcPrint, &printInfo);
                    if (dcPrint.EndPage () < 0 || !_AfxAbortProc (dcPrint.m_hDC, 0)) {
                        bError = TRUE;
                        break;
                    }
                }
            }
        }
        catch (...) {
            // cleanup document printing process
            if (!printInfo.m_bDocObject) {
                dcPrint.AbortDoc ();
            }

            ::EnableWindow (mainWnd, true);

            OnEndPrinting (&dcPrint, &printInfo); // clean up after printing
            throw;
        }

        // cleanup document printing process
        if (!printInfo.m_bDocObject) {
            if (!bError)
                dcPrint.EndDoc ();
            else
                dcPrint.AbortDoc ();
        }

        ::EnableWindow (mainWnd, true);

        OnEndPrinting (&dcPrint, &printInfo); // clean up after printing
        dlgPrintStatus.DestroyWindow ();

        dcPrint.Detach (); // will be cleaned up by CPrintInfo destructor
    }
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
#endif //_DEBUG
