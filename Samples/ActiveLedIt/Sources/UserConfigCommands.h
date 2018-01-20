/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef __UserConfigCommands_h__
#define __UserConfigCommands_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <atlbase.h>
#include <atlcom.h>

#include "Stroika/Frameworks/Led/Platform/ATL.h"
#include "Stroika/Frameworks/Led/TextInteractor.h"

#include "ActiveLedItConfig.h"
#include "Resource.h"

#include "ActiveLedIt_h.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

class UserCommandNameNumberRegistry {
public:
    static UserCommandNameNumberRegistry& Get ();

private:
    static UserCommandNameNumberRegistry* sThis;

public:
    UserCommandNameNumberRegistry ();

public:
    nonvirtual UINT Enter (const wstring& internalName);
    nonvirtual bool Lookup (UINT cmdNum, wstring* internalName);
    nonvirtual bool Lookup (const wstring& internalName, UINT* cmdNum);
    nonvirtual wstring Lookup (UINT cmdNum);

private:
    map<wstring, UINT> fName2Num;
    map<UINT, wstring> fNum2Name;
    UINT               fNextUserCmdNum;
};

struct BuiltinCmdSpec {
    BuiltinCmdSpec (WORD cmdNum, const char* cmdName)
        : fCmdNum (cmdNum)
        , fCmdName (cmdName)
        , fInternalCmdName (cmdName)
    {
    }
    BuiltinCmdSpec (WORD cmdNum, const char* cmdName, const char* internalCmdName)
        : fCmdNum (cmdNum)
        , fCmdName (cmdName)
        , fInternalCmdName (internalCmdName)
    {
    }

    WORD        fCmdNum;
    const char* fCmdName;
    const char* fInternalCmdName;
};

const BuiltinCmdSpec kCmd_Separator               = BuiltinCmdSpec (0, "");
const BuiltinCmdSpec kCmd_Undo                    = BuiltinCmdSpec (ID_EDIT_UNDO, "Undo");
const BuiltinCmdSpec kCmd_Redo                    = BuiltinCmdSpec (ID_EDIT_REDO, "Redo");
const BuiltinCmdSpec kCmd_Cut                     = BuiltinCmdSpec (ID_EDIT_CUT, "Cut");
const BuiltinCmdSpec kCmd_Copy                    = BuiltinCmdSpec (ID_EDIT_COPY, "Copy");
const BuiltinCmdSpec kCmd_Paste                   = BuiltinCmdSpec (ID_EDIT_PASTE, "Paste");
const BuiltinCmdSpec kCmd_PasteText               = BuiltinCmdSpec (kPasteFormatTextCmdID, "Paste Text");
const BuiltinCmdSpec kCmd_Clear                   = BuiltinCmdSpec (ID_EDIT_CLEAR, "Clear");
const BuiltinCmdSpec kCmd_SelectWord              = BuiltinCmdSpec (kSelectWordCmd, "Select Word(s)");
const BuiltinCmdSpec kCmd_SelectTextRow           = BuiltinCmdSpec (kSelectTextRowCmd, "Select Text Row(s)");
const BuiltinCmdSpec kCmd_SelectParagraph         = BuiltinCmdSpec (kSelectParagraphCmd, "Select Paragraph(s)");
const BuiltinCmdSpec kCmd_SelectTableIntraCellAll = BuiltinCmdSpec (kSelectTableIntraCellAllCmd, "Select All in Cell");
const BuiltinCmdSpec kCmd_SelectTableCell         = BuiltinCmdSpec (kSelectTableCellCmd, "Select Table Cell");
const BuiltinCmdSpec kCmd_SelectTableRow          = BuiltinCmdSpec (kSelectTableRowCmd, "Select Table Row(s)");
const BuiltinCmdSpec kCmd_SelectTableColumn       = BuiltinCmdSpec (kSelectTableColumnCmd, "Select Table Column(s)");
const BuiltinCmdSpec kCmd_SelectTable             = BuiltinCmdSpec (kSelectTableCmd, "Select Table");
const BuiltinCmdSpec kCmd_SelectAll               = BuiltinCmdSpec (ID_EDIT_SELECT_ALL, "Select All");
const BuiltinCmdSpec kCmd_Find                    = BuiltinCmdSpec (kFindCmd, "Find...");
const BuiltinCmdSpec kCmd_FindAgain               = BuiltinCmdSpec (kFindAgainCmd, "Find Again");
const BuiltinCmdSpec kCmd_EnterFindString         = BuiltinCmdSpec (kEnterFindStringCmd, "Enter 'Find' String");
const BuiltinCmdSpec kCmd_Replace                 = BuiltinCmdSpec (kReplaceCmd, "Replace...");
const BuiltinCmdSpec kCmd_ReplaceAgain            = BuiltinCmdSpec (kReplaceAgainCmd, "Replace Again");
const BuiltinCmdSpec kCmd_CheckSpelling           = BuiltinCmdSpec (kSpellCheckCmd, "Check Spelling...");
const BuiltinCmdSpec kCmd_InsertTable             = BuiltinCmdSpec (kInsertTableCmd, "Insert Table...");
const BuiltinCmdSpec kCmd_InsertTableRowAbove     = BuiltinCmdSpec (kInsertTableRowAboveCmd, "Insert Table Row Above");
const BuiltinCmdSpec kCmd_InsertTableRowBelow     = BuiltinCmdSpec (kInsertTableRowBelowCmd, "Insert Table Row Below");
const BuiltinCmdSpec kCmd_InsertTableColBefore    = BuiltinCmdSpec (kInsertTableColBeforeCmd, "Insert Table Column Before");
const BuiltinCmdSpec kCmd_InsertTableColAfter     = BuiltinCmdSpec (kInsertTableColAfterCmd, "Insert Table Column After");
const BuiltinCmdSpec kCmd_InsertURL               = BuiltinCmdSpec (kInsertURLCmd, "Insert URL...");
const BuiltinCmdSpec kCmd_InsertSymbol            = BuiltinCmdSpec (kInsertSymbolCmd, "Insert Symbol...");
const BuiltinCmdSpec kCmd_RemoveTableRows         = BuiltinCmdSpec (kRemoveTableRowsCmd, "Remove Rows");
const BuiltinCmdSpec kCmd_RemoveTableColumns      = BuiltinCmdSpec (kRemoveTableColumnsCmd, "Remove Columns");
const BuiltinCmdSpec kCmd_FontStylePlain          = BuiltinCmdSpec (kFontStylePlainCmd, "Plain");
const BuiltinCmdSpec kCmd_FontStyleBold           = BuiltinCmdSpec (kFontStyleBoldCmd, "Bold");
const BuiltinCmdSpec kCmd_FontStyleItalic         = BuiltinCmdSpec (kFontStyleItalicCmd, "Italic");
const BuiltinCmdSpec kCmd_FontStyleUnderline      = BuiltinCmdSpec (kFontStyleUnderlineCmd, "Underline");
const BuiltinCmdSpec kCmd_FontStyleStrikeout      = BuiltinCmdSpec (kFontStyleStrikeoutCmd, "Strikeout");
const BuiltinCmdSpec kCmd_SubScript               = BuiltinCmdSpec (kSubScriptCmd, "Subscript");
const BuiltinCmdSpec kCmd_SuperScript             = BuiltinCmdSpec (kSuperScriptCmd, "Superscript");
const BuiltinCmdSpec kCmd_FontSize9               = BuiltinCmdSpec (kFontSize9Cmd, "9 pt");
const BuiltinCmdSpec kCmd_FontSize10              = BuiltinCmdSpec (kFontSize10Cmd, "10 pt");
const BuiltinCmdSpec kCmd_FontSize12              = BuiltinCmdSpec (kFontSize12Cmd, "12 pt");
const BuiltinCmdSpec kCmd_FontSize14              = BuiltinCmdSpec (kFontSize14Cmd, "14 pt");
const BuiltinCmdSpec kCmd_FontSize18              = BuiltinCmdSpec (kFontSize18Cmd, "18 pt");
const BuiltinCmdSpec kCmd_FontSize24              = BuiltinCmdSpec (kFontSize24Cmd, "24 pt");
const BuiltinCmdSpec kCmd_FontSize36              = BuiltinCmdSpec (kFontSize36Cmd, "36 pt");
const BuiltinCmdSpec kCmd_FontSize48              = BuiltinCmdSpec (kFontSize48Cmd, "48 pt");
const BuiltinCmdSpec kCmd_FontSize72              = BuiltinCmdSpec (kFontSize72Cmd, "72 pt");
const BuiltinCmdSpec kCmd_FontSizeSmaller         = BuiltinCmdSpec (kFontSizeSmallerCmd, "Smaller");
const BuiltinCmdSpec kCmd_FontSizeLarger          = BuiltinCmdSpec (kFontSizeLargerCmd, "Larger");
const BuiltinCmdSpec kCmd_FontSizeOther           = BuiltinCmdSpec (kFontSizeOtherCmd, "Other...", "FontSizeOther");
const BuiltinCmdSpec kCmd_BlackColor              = BuiltinCmdSpec (kBlackColorCmd, "Black");
const BuiltinCmdSpec kCmd_MaroonColor             = BuiltinCmdSpec (kMaroonColorCmd, "Maroon");
const BuiltinCmdSpec kCmd_GreenColor              = BuiltinCmdSpec (kGreenColorCmd, "Green");
const BuiltinCmdSpec kCmd_OliveColor              = BuiltinCmdSpec (kOliveColorCmd, "Olive");
const BuiltinCmdSpec kCmd_NavyColor               = BuiltinCmdSpec (kNavyColorCmd, "Navy");
const BuiltinCmdSpec kCmd_PurpleColor             = BuiltinCmdSpec (kPurpleColorCmd, "Purple");
const BuiltinCmdSpec kCmd_TealColor               = BuiltinCmdSpec (kTealColorCmd, "Teal");
const BuiltinCmdSpec kCmd_GrayColor               = BuiltinCmdSpec (kGrayColorCmd, "Gray");
const BuiltinCmdSpec kCmd_SilverColor             = BuiltinCmdSpec (kSilverColorCmd, "Silver");
const BuiltinCmdSpec kCmd_RedColor                = BuiltinCmdSpec (kRedColorCmd, "Red");
const BuiltinCmdSpec kCmd_LimeColor               = BuiltinCmdSpec (kLimeColorCmd, "Lime");
const BuiltinCmdSpec kCmd_YellowColor             = BuiltinCmdSpec (kYellowColorCmd, "Yellow");
const BuiltinCmdSpec kCmd_BlueColor               = BuiltinCmdSpec (kBlueColorCmd, "Blue");
const BuiltinCmdSpec kCmd_FuchsiaColor            = BuiltinCmdSpec (kFuchsiaColorCmd, "Fuchsia");
const BuiltinCmdSpec kCmd_AquaColor               = BuiltinCmdSpec (kAquaColorCmd, "Aqua");
const BuiltinCmdSpec kCmd_WhiteColor              = BuiltinCmdSpec (kWhiteColorCmd, "White");
const BuiltinCmdSpec kCmd_OtherColor              = BuiltinCmdSpec (kFontColorOtherCmd, "Other...", "FontColorOther");
const BuiltinCmdSpec kCmd_ChooseFontDialog        = BuiltinCmdSpec (kChooseFontDialogCmd, "Choose Font Dialog...");
const BuiltinCmdSpec kCmd_JustifyLeft             = BuiltinCmdSpec (kJustifyLeftCmd, "Left", "JustifyLeft");
const BuiltinCmdSpec kCmd_JustifyCenter           = BuiltinCmdSpec (kJustifyCenterCmd, "Center", "JustifyCenter");
const BuiltinCmdSpec kCmd_JustifyRight            = BuiltinCmdSpec (kJustifyRightCmd, "Right", "JustifyRight");
const BuiltinCmdSpec kCmd_JustifyFull             = BuiltinCmdSpec (kJustifyFullCmd, "Full", "JustifyFull");
const BuiltinCmdSpec kCmd_ParagraphIndents        = BuiltinCmdSpec (kParagraphIndentsCmd, "Paragraph Indents...");
const BuiltinCmdSpec kCmd_ParagraphSpacing        = BuiltinCmdSpec (kParagraphSpacingCmd, "Paragraph Spacing...");
const BuiltinCmdSpec kCmd_ListStyle_None          = BuiltinCmdSpec (kListStyle_NoneCmd, "None", "ListStyle_None");
const BuiltinCmdSpec kCmd_ListStyle_Bullet        = BuiltinCmdSpec (kListStyle_BulletCmd, "Bullet", "ListStyle_Bullet");
const BuiltinCmdSpec kCmd_IncreaseIndent          = BuiltinCmdSpec (kIncreaseIndentCmd, "Increase List Indent");
const BuiltinCmdSpec kCmd_DescreaseIndent         = BuiltinCmdSpec (kDecreaseIndentCmd, "Decrease List Indent");
const BuiltinCmdSpec kCmd_PropertiesForSelection  = BuiltinCmdSpec (kPropertiesForSelectionCmd, "Properties");
const BuiltinCmdSpec kCmd_OpenEmbedding           = BuiltinCmdSpec (kFirstPrivateEmbeddingCmd, "Open Embedding");
const BuiltinCmdSpec kCmd_Print                   = BuiltinCmdSpec (ID_FILE_PRINT, "Print...");
const BuiltinCmdSpec kCmd_PrintSetup              = BuiltinCmdSpec (ID_FILE_PRINT_SETUP, "Print Setup...");
const BuiltinCmdSpec kCmd_BrowseOnlineHelp        = BuiltinCmdSpec (ID_HELP_FINDER, "Browse Online Help");
const BuiltinCmdSpec kCmd_CheckForUpdatesOnWeb    = BuiltinCmdSpec (kCheckForUpdatesWebPageCmdID, "Check for ActiveLedIt! Web Updates");
const BuiltinCmdSpec kCmd_About                   = BuiltinCmdSpec (ID_APP_ABOUT, "About ActiveLedIt!...");
const BuiltinCmdSpec kCmd_Help                    = BuiltinCmdSpec (ID_HELP, "Help");
const BuiltinCmdSpec kCmd_ContextHelp             = BuiltinCmdSpec (ID_CONTEXT_HELP, "Context Help");

const BuiltinCmdSpec kAllCmds[] = {
    kCmd_Undo,
    kCmd_Redo,
    kCmd_Cut,
    kCmd_Copy,
    kCmd_Paste,
    kCmd_PasteText,
    kCmd_Clear,
    kCmd_SelectWord,
    kCmd_SelectTextRow,
    kCmd_SelectParagraph,
    kCmd_SelectTableIntraCellAll,
    kCmd_SelectTableCell,
    kCmd_SelectTableRow,
    kCmd_SelectTableColumn,
    kCmd_SelectTable,
    kCmd_SelectAll,
    kCmd_Find,
    kCmd_FindAgain,
    kCmd_EnterFindString,
    kCmd_Replace,
    kCmd_ReplaceAgain,
    kCmd_CheckSpelling,
    kCmd_InsertTable,
    kCmd_InsertTableRowAbove,
    kCmd_InsertTableRowBelow,
    kCmd_InsertTableColBefore,
    kCmd_InsertTableColAfter,
    kCmd_InsertURL,
    kCmd_InsertSymbol,
    kCmd_RemoveTableRows,
    kCmd_RemoveTableColumns,
    kCmd_FontStylePlain,
    kCmd_FontStyleBold,
    kCmd_FontStyleItalic,
    kCmd_FontStyleUnderline,
    kCmd_FontStyleStrikeout,
    kCmd_SubScript,
    kCmd_SuperScript,
    kCmd_FontSize9,
    kCmd_FontSize10,
    kCmd_FontSize12,
    kCmd_FontSize14,
    kCmd_FontSize18,
    kCmd_FontSize24,
    kCmd_FontSize36,
    kCmd_FontSize48,
    kCmd_FontSize72,
    kCmd_FontSizeSmaller,
    kCmd_FontSizeLarger,
    kCmd_FontSizeOther,
    kCmd_BlackColor,
    kCmd_MaroonColor,
    kCmd_GreenColor,
    kCmd_OliveColor,
    kCmd_NavyColor,
    kCmd_PurpleColor,
    kCmd_TealColor,
    kCmd_GrayColor,
    kCmd_SilverColor,
    kCmd_RedColor,
    kCmd_LimeColor,
    kCmd_YellowColor,
    kCmd_BlueColor,
    kCmd_FuchsiaColor,
    kCmd_AquaColor,
    kCmd_WhiteColor,
    kCmd_OtherColor,
    kCmd_ChooseFontDialog,
    kCmd_JustifyLeft,
    kCmd_JustifyCenter,
    kCmd_JustifyRight,
    kCmd_JustifyFull,
    kCmd_ParagraphIndents,
    kCmd_ParagraphSpacing,
    kCmd_ListStyle_None,
    kCmd_ListStyle_Bullet,
    kCmd_IncreaseIndent,
    kCmd_DescreaseIndent,
    kCmd_PropertiesForSelection,
    kCmd_OpenEmbedding,
    kCmd_Print,
    kCmd_PrintSetup,
    kCmd_BrowseOnlineHelp,
    kCmd_CheckForUpdatesOnWeb,
    kCmd_About,
    kCmd_Help,
    kCmd_ContextHelp,
};

class ATL_NO_VTABLE ActiveLedIt_CurrentEventArguments : public CComObjectRootEx<CComSingleThreadModel>,
                                                        public CComCoClass<ActiveLedIt_CurrentEventArguments>,
                                                        public IDispatchImpl<IALCurrentEventArguments, &IID_IALCurrentEventArguments, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion> {
public:
    ActiveLedIt_CurrentEventArguments ();
    virtual ~ActiveLedIt_CurrentEventArguments ();

public:
    DECLARE_NO_REGISTRY ()

public:
    BEGIN_COM_MAP (ActiveLedIt_CurrentEventArguments)
    COM_INTERFACE_ENTRY (IALCurrentEventArguments)
    COM_INTERFACE_ENTRY (IDispatch)
    END_COM_MAP ()

public:
    STDMETHOD (get_InternalCommandName)
    (BSTR* pVal);
    STDMETHOD (put_InternalCommandName)
    (BSTR val);
    STDMETHOD (get_Enabled)
    (VARIANT_BOOL* pVal);
    STDMETHOD (put_Enabled)
    (VARIANT_BOOL val);
    STDMETHOD (get_Checked)
    (VARIANT_BOOL* pVal);
    STDMETHOD (put_Checked)
    (VARIANT_BOOL val);
    STDMETHOD (get_Name)
    (BSTR* pVal);
    STDMETHOD (put_Name)
    (BSTR val);

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();

public:
    wstring fInternalName;
    bool    fEnabled;
    bool    fChecked;
    wstring fName;
};

class ATL_NO_VTABLE AL_CommandHelper : public IDispatchImpl<IALCommand, &IID_IALCommand, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion> {
public:
    AL_CommandHelper ();
    virtual ~AL_CommandHelper ();

public:
    STDMETHOD (get_Name)
    (BSTR* pVal);
    STDMETHOD (get_InternalName)
    (BSTR* pVal);
    STDMETHOD (AppendSelfToMenu)
    (HMENU menu, IDispatch* acceleratorTable);

protected:
    wstring fName;
    wstring fInternalName;
    WORD    fCommandNumber;
};

class ATL_NO_VTABLE AL_UserCommandHelper : public IDispatchImpl<IALUserCommand, &IID_IALUserCommand, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
                                           public AL_CommandHelper {
public:
    AL_UserCommandHelper ();
    virtual ~AL_UserCommandHelper ();

public:
    STDMETHOD (put_Name)
    (BSTR val);
    STDMETHOD (put_InternalName)
    (BSTR val);

    // Not sure these needed/effective - maybe have to be in final class???
    // disambiguate mixins
public:
    STDMETHOD (get_Name)
    (BSTR* pVal) { return AL_CommandHelper::get_Name (pVal); }
    STDMETHOD (get_InternalName)
    (BSTR* pVal) { return AL_CommandHelper::get_InternalName (pVal); }
    STDMETHOD (AppendSelfToMenu)
    (HMENU menu, IDispatch* acceleratorTable) { return AL_CommandHelper::AppendSelfToMenu (menu, acceleratorTable); }
};

class ATL_NO_VTABLE AL_CommandListHelper : public IDispatchImpl<IALCommandList, &IID_IALCommandList, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion> {
public:
    AL_CommandListHelper ();
    virtual ~AL_CommandListHelper ();

public:
    STDMETHOD (GeneratePopupMenu)
    (IDispatch* acceleratorTable, HMENU* val);
    STDMETHOD (LookupCommand)
    (BSTR internalName, IDispatch** val);
    STDMETHOD (IndexOf)
    (VARIANT internalNameOrObject, UINT* val);
    STDMETHOD (get__NewEnum)
    (IUnknown** ppUnk);
    STDMETHOD (get_Item)
    (long Index, IDispatch** pVal);
    STDMETHOD (get_Count)
    (long* pVal);

public:
    nonvirtual void AppendBuiltinCmd (const BuiltinCmdSpec& cmdSpec);
    nonvirtual void AppendBuiltinCmds (const BuiltinCmdSpec* cmdSpecsStart, const BuiltinCmdSpec* cmdSpecsEnd);

protected:
    vector<CComPtr<IDispatch>> fOwnedItems;
};

class ATL_NO_VTABLE AL_UserCommandListHelper : public AL_CommandListHelper,
                                               public IDispatchImpl<IALUserCommandList, &IID_IALUserCommandList, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion> {
public:
    AL_UserCommandListHelper ();
    virtual ~AL_UserCommandListHelper ();

    // disambiguate mixins
public:
    STDMETHOD (GeneratePopupMenu)
    (IDispatch* acceleratorTable, HMENU* val) { return AL_CommandListHelper::GeneratePopupMenu (acceleratorTable, val); }
    STDMETHOD (LookupCommand)
    (BSTR internalName, IDispatch** val) { return AL_CommandListHelper::LookupCommand (internalName, val); }
    STDMETHOD (IndexOf)
    (VARIANT internalNameOrObject, UINT* val) { return AL_CommandListHelper::IndexOf (internalNameOrObject, val); }
    STDMETHOD (get__NewEnum)
    (IUnknown** ppUnk) { return AL_CommandListHelper::get__NewEnum (ppUnk); }
    STDMETHOD (get_Item)
    (long Index, IDispatch** pVal) { return AL_CommandListHelper::get_Item (Index, pVal); }
    STDMETHOD (get_Count)
    (long* pVal) { return AL_CommandListHelper::get_Count (pVal); }

public:
    STDMETHOD (Insert)
    (IDispatch* newElt, UINT afterElt = static_cast<UINT> (-1));
    STDMETHOD (Remove)
    (VARIANT eltIntNameOrIndex);
    STDMETHOD (Clear)
    ();
};

// read-only command object (unmodifyable) - but still seen/use externally as a COM object
class ATL_NO_VTABLE ActiveLedIt_BuiltinCommand : public CComObjectRootEx<CComSingleThreadModel>,
                                                 public CComCoClass<ActiveLedIt_BuiltinCommand>,
                                                 public AL_CommandHelper {
public:
    ActiveLedIt_BuiltinCommand ();
    virtual ~ActiveLedIt_BuiltinCommand ();

public:
    static ActiveLedIt_BuiltinCommand* mk (const BuiltinCmdSpec& cmdSpec);
    void                               SetName (const wstring& name) { fName = name; }

public:
    DECLARE_NO_REGISTRY ()

public:
    BEGIN_COM_MAP (ActiveLedIt_BuiltinCommand)
    COM_INTERFACE_ENTRY (IALCommand)
    COM_INTERFACE_ENTRY (IDispatch)
    END_COM_MAP ()

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();
};

class ATL_NO_VTABLE ActiveLedIt_UserCommand : public CComObjectRootEx<CComSingleThreadModel>,
                                              public CComCoClass<ActiveLedIt_BuiltinCommand>,
                                              public IDispatchImpl<IALUserCommand, &IID_IALUserCommand, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
                                              public AL_CommandHelper {
public:
    ActiveLedIt_UserCommand ();
    virtual ~ActiveLedIt_UserCommand ();

public:
    DECLARE_NO_REGISTRY ()

public:
    BEGIN_COM_MAP (ActiveLedIt_UserCommand)
    COM_INTERFACE_ENTRY (IALUserCommand)
    COM_INTERFACE_ENTRY2 (IALCommand, AL_CommandHelper)
    COM_INTERFACE_ENTRY2 (IDispatch, IALUserCommand)
    END_COM_MAP ()

public:
    STDMETHOD (get_Name)
    (BSTR* pVal) { return AL_CommandHelper::get_Name (pVal); }
    STDMETHOD (get_InternalName)
    (BSTR* pVal) { return AL_CommandHelper::get_InternalName (pVal); }
    STDMETHOD (AppendSelfToMenu)
    (HMENU menu, IDispatch* acceleratorTable) { return AL_CommandHelper::AppendSelfToMenu (menu, acceleratorTable); }

public:
    STDMETHOD (put_Name)
    (BSTR val);
    STDMETHOD (put_InternalName)
    (BSTR val);

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();
};

class ATL_NO_VTABLE ActiveLedIt_AcceleratorElement : public CComObjectRootEx<CComSingleThreadModel>,
                                                     public CComCoClass<ActiveLedIt_AcceleratorElement>,
                                                     public IDispatchImpl<IALAcceleratorElement, &IID_IALAcceleratorElement, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion> {
public:
    ActiveLedIt_AcceleratorElement ();
    virtual ~ActiveLedIt_AcceleratorElement ();

public:
    DECLARE_NO_REGISTRY ()

public:
    BEGIN_COM_MAP (ActiveLedIt_AcceleratorElement)
    COM_INTERFACE_ENTRY (IALAcceleratorElement)
    COM_INTERFACE_ENTRY (IDispatch)
    END_COM_MAP ()

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

public:
    STDMETHOD (get_CommandInternalName)
    (BSTR* pVal);
    STDMETHOD (put_CommandInternalName)
    (BSTR val);
    STDMETHOD (get_ModifierFlag)
    (AcceleratorModifierFlag* pVal);
    STDMETHOD (put_ModifierFlag)
    (AcceleratorModifierFlag val);
    STDMETHOD (get_Key)
    (WORD* pVal);
    STDMETHOD (put_Key)
    (WORD val);

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();

private:
    wstring                 fCommandInternalName;
    AcceleratorModifierFlag fAcceleratorModifierFlag;
    WORD                    fKey;
};

class ATL_NO_VTABLE ActiveLedIt_AcceleratorTable : public CComObjectRootEx<CComSingleThreadModel>,
                                                   public CComCoClass<ActiveLedIt_AcceleratorTable>,
                                                   public IDispatchImpl<IALAcceleratorTable, &IID_IALAcceleratorTable, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion> {
public:
    ActiveLedIt_AcceleratorTable ();
    virtual ~ActiveLedIt_AcceleratorTable ();

public:
    DECLARE_NO_REGISTRY ()

public:
    BEGIN_COM_MAP (ActiveLedIt_AcceleratorTable)
    COM_INTERFACE_ENTRY (IALAcceleratorTable)
    COM_INTERFACE_ENTRY (IDispatch)
    END_COM_MAP ()

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

public:
    nonvirtual void AppendACCEL (const char* internalCmdName, AcceleratorModifierFlag modifierFlag, WORD key);

public:
    STDMETHOD (get__NewEnum)
    (IUnknown** ppUnk);
    STDMETHOD (get_Item)
    (long Index, IDispatch** pVal);
    STDMETHOD (get_Count)
    (long* pVal);
    STDMETHOD (Lookup)
    (BSTR cmdInternalName, IDispatch** pVal);
    STDMETHOD (Add)
    (IDispatch* newElt, UINT atIndex = 0xffffffff);
    STDMETHOD (Remove)
    (VARIANT eltIntNameOrIndex);
    STDMETHOD (Clear)
    ();
    STDMETHOD (GenerateWin32AcceleratorTable)
    (HACCEL* pVal);

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();

private:
    vector<CComPtr<IDispatch>> fAccelerators;
};

// A static - unchangable (by users) command list object
class ATL_NO_VTABLE ActiveLedIt_StaticCommandList : public CComObjectRootEx<CComSingleThreadModel>,
                                                    public CComCoClass<ActiveLedIt_StaticCommandList>,
                                                    public AL_CommandListHelper {
public:
    ActiveLedIt_StaticCommandList ();
    virtual ~ActiveLedIt_StaticCommandList ();

public:
    DECLARE_NO_REGISTRY ()

public:
    BEGIN_COM_MAP (ActiveLedIt_StaticCommandList)
    COM_INTERFACE_ENTRY (IALCommandList)
    COM_INTERFACE_ENTRY (IDispatch)
    END_COM_MAP ()

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

public:
    nonvirtual void Append (IDispatch* p);

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();
};

// A command list that can be modified arbitrarily by users (callers)
class ATL_NO_VTABLE ActiveLedIt_UserCommandList : public CComObjectRootEx<CComSingleThreadModel>,
                                                  public CComCoClass<ActiveLedIt_UserCommandList>,
                                                  public AL_UserCommandListHelper {
public:
    ActiveLedIt_UserCommandList ();
    virtual ~ActiveLedIt_UserCommandList ();

public:
    DECLARE_NO_REGISTRY ()

public:
    BEGIN_COM_MAP (ActiveLedIt_UserCommandList)
    COM_INTERFACE_ENTRY (IALUserCommandList)
    COM_INTERFACE_ENTRY2 (IALCommandList, IALUserCommandList)
    COM_INTERFACE_ENTRY2 (IDispatch, IALUserCommandList)
    END_COM_MAP ()

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

    // disambiguate mixins
public:
    STDMETHOD (GeneratePopupMenu)
    (IDispatch* acceleratorTable, HMENU* val) { return AL_UserCommandListHelper::GeneratePopupMenu (acceleratorTable, val); }
    STDMETHOD (LookupCommand)
    (BSTR internalName, IDispatch** val) { return AL_UserCommandListHelper::LookupCommand (internalName, val); }
    STDMETHOD (IndexOf)
    (VARIANT internalNameOrObject, UINT* val) { return AL_UserCommandListHelper::IndexOf (internalNameOrObject, val); }
    STDMETHOD (get__NewEnum)
    (IUnknown** ppUnk) { return AL_UserCommandListHelper::get__NewEnum (ppUnk); }
    STDMETHOD (get_Item)
    (long Index, IDispatch** pVal) { return AL_UserCommandListHelper::get_Item (Index, pVal); }
    STDMETHOD (get_Count)
    (long* pVal) { return AL_UserCommandListHelper::get_Count (pVal); }

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();
};

class ATL_NO_VTABLE ActiveLedIt_MenuItemPopup : public CComObjectRootEx<CComSingleThreadModel>,
                                                public CComCoClass<ActiveLedIt_MenuItemPopup, &CLSID_ActiveLedIt_MenuItemPopup>,
                                                public IDispatchImpl<IALMenuItemPopup, &IID_IALMenuItemPopup, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
                                                public AL_CommandListHelper,
                                                public AL_UserCommandHelper {
public:
    ActiveLedIt_MenuItemPopup ();
    virtual ~ActiveLedIt_MenuItemPopup ();

public:
    DECLARE_NO_REGISTRY ()

    // disambiguate mixins
public:
    STDMETHOD (GeneratePopupMenu)
    (IDispatch* acceleratorTable, HMENU* val) { return AL_CommandListHelper::GeneratePopupMenu (acceleratorTable, val); }
    STDMETHOD (LookupCommand)
    (BSTR internalName, IDispatch** val) { return AL_CommandListHelper::LookupCommand (internalName, val); }
    STDMETHOD (IndexOf)
    (VARIANT internalNameOrObject, UINT* val) { return AL_CommandListHelper::IndexOf (internalNameOrObject, val); }
    STDMETHOD (get__NewEnum)
    (IUnknown** ppUnk) { return AL_CommandListHelper::get__NewEnum (ppUnk); }
    STDMETHOD (get_Item)
    (long Index, IDispatch** pVal) { return AL_CommandListHelper::get_Item (Index, pVal); }
    STDMETHOD (get_Count)
    (long* pVal) { return AL_CommandListHelper::get_Count (pVal); }

public:
    STDMETHOD (get_Name)
    (BSTR* pVal) { return AL_CommandHelper::get_Name (pVal); }
    STDMETHOD (get_InternalName)
    (BSTR* pVal) { return AL_CommandHelper::get_InternalName (pVal); }
    STDMETHOD (AppendSelfToMenu)
    (HMENU menu, IDispatch* acceleratorTable);

public:
    STDMETHOD (put_Name)
    (BSTR val) { return AL_UserCommandHelper::put_Name (val); }
    STDMETHOD (put_InternalName)
    (BSTR val) { return AL_UserCommandHelper::put_InternalName (val); }

public:
    STDMETHOD (Insert)
    (IDispatch*, UINT = static_cast<UINT> (-1));
    STDMETHOD (Remove)
    (VARIANT eltIntNameOrIndex);
    STDMETHOD (Clear)
    ();

public:
    BEGIN_COM_MAP (ActiveLedIt_MenuItemPopup)
    COM_INTERFACE_ENTRY (IALMenuItemPopup)
    COM_INTERFACE_ENTRY2 (IALCommandList, IALMenuItemPopup)
    COM_INTERFACE_ENTRY (IALUserCommand)
    COM_INTERFACE_ENTRY2 (IALCommand, AL_CommandHelper)
    COM_INTERFACE_ENTRY2 (IDispatch, IALMenuItemPopup)
    END_COM_MAP ()

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();
};

string mkFontNameCMDName (const Led_SDK_String& fName);

CComPtr<IDispatch> GenerateBuiltinCommandsObject ();

WORD    CmdObjOrName2Num (const VARIANT& cmdObjOrName);
wstring CmdNum2Name (WORD cmdNum);

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*__UserConfigCommands_h__*/
