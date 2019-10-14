/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef __ActiveLedItControl_h__
#define __ActiveLedItControl_h__ 1

/*
 * Description:
 *      Declaration of the ActiveLedItControl ActiveX Control class.
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxctl.h>
#include <objsafe.h> // for IObjectSafety; in ActiveX SDK
#include <set>

#include "Stroika/Frameworks/Led/SpellCheckEngine.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_HTML.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_RTF.h"

#include "ActiveLedItConfig.h"
#include "LedItView.h"

#include "ActiveLedIt_h.h"

enum Led_FileFormat {
    eTextFormat,
    eLedPrivateFormat,
    eRTFFormat,
    eHTMLFormat,
    eUnknownFormat,
    eDefaultFormat = eRTFFormat,
};

class COMBased_SpellCheckEngine : public SpellCheckEngine,
                                  private SpellCheckEngine::UDInterface,
                                  private TextBreaks {
private:
    using inherited = SpellCheckEngine;

public:
    COMBased_SpellCheckEngine (IDispatch* engine);

private:
    CComPtr<IDispatch> fEngine;

public:
    virtual bool ScanForUndefinedWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
                                       const Led_tChar** wordStartResult, const Led_tChar** wordEndResult) override;

protected:
    virtual bool LookupWord_ (const Led_tString& checkWord, Led_tString* matchedWordResult) override;

public:
    virtual vector<Led_tString> GenerateSuggestions (const Led_tString& misspelledWord) override;

public:
    virtual TextBreaks* PeekAtTextBreaksUsed () override;

    // From TextBreaks
public:
    virtual void FindWordBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
                                 size_t* wordStartResult, size_t* wordEndResult, bool* wordReal) const override;
    virtual void FindLineBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
                                 size_t* wordEndResult, bool* wordReal) const override;

public:
    virtual UDInterface* GetUDInterface () override;

    // From SpellCheckEngine::UDInterface
public:
    virtual bool AddWordToUserDictionarySupported () const override;
    virtual void AddWordToUserDictionary (const Led_tString& word) override;
};

DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning

class ActiveLedItControl : public COleControl, public MarkerOwner, public LedItViewController {
private:
    DECLARE_DYNCREATE (ActiveLedItControl)

public:
    ActiveLedItControl ();

protected:
    ~ActiveLedItControl ();

#if qKeepListOfALInstancesForSPR_1599BWA
public:
    static const set<ActiveLedItControl*>& GetAll ();

private:
    static set<ActiveLedItControl*> sAll;
#endif

protected:
    virtual void       DidUpdateText (const UpdateInfo& /*updateInfo*/) noexcept override;
    virtual TextStore* PeekAtTextStore () const override;

public:
    virtual void OnDraw (CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid) override;
    virtual void OnDrawMetafile (CDC* pDC, const CRect& rcBounds) override;
    afx_msg BOOL  OnEraseBkgnd (CDC* pDC);
    virtual void  DoPropExchange (CPropExchange* pPX) override;
    virtual void  OnResetState () override;
    virtual DWORD GetControlFlags () override;
    virtual void  OnGetControlInfo (LPCONTROLINFO pControlInfo) override;
    virtual BOOL  PreTranslateMessage (MSG* pMsg) override;
    virtual void  AddFrameLevelUI () override;
    virtual void  RemoveFrameLevelUI () override;
    virtual BOOL  OnSetObjectRects (LPCRECT lprcPosRect, LPCRECT lprcClipRect) override;
    virtual BOOL  OnGetNaturalExtent (DWORD /* dwAspect */, LONG /* lindex */,
                                      DVTARGETDEVICE* /* ptd */, HDC /* hicTargetDev */,
                                      DVEXTENTINFO* /* pExtentInfo */, LPSIZEL /* psizel */
                                      ) override;

private:
    struct OnCreateExtras {
        OnCreateExtras ();
        bool fReadOnly;
        bool fEnabled;
    };
    unique_ptr<OnCreateExtras> fOnCreateExtras;

private:
    nonvirtual void ExchangeTextAsRTFBlob (CPropExchange* pPX);

public:
    DECLARE_INTERFACE_MAP ()

    BEGIN_INTERFACE_PART (ObjSafe, IObjectSafety)
    STDMETHOD_ (HRESULT, GetInterfaceSafetyOptions)
    (
        /* [in] */ REFIID riid,
        /* [out] */ DWORD __RPC_FAR* pdwSupportedOptions,
        /* [out] */ DWORD __RPC_FAR* pdwEnabledOptions);

    STDMETHOD_ (HRESULT, SetInterfaceSafetyOptions)
    (
        /* [in] */ REFIID riid,
        /* [in] */ DWORD  dwOptionSetMask,
        /* [in] */ DWORD  dwEnabledOptions);
    END_INTERFACE_PART (ObjSafe);

protected:
    DECLARE_OLECREATE_EX (ActiveLedItControl) // Class factory and guid
    DECLARE_OLETYPELIB (ActiveLedItControl)   // GetTypeInfo
    DECLARE_PROPPAGEIDS (ActiveLedItControl)  // Property page IDs
    DECLARE_OLECTLTYPE (ActiveLedItControl)   // Type name and misc status

    // Reflect to OWNED window, since we have two separate windows. If we ever merge these, then most of this reflecting code can die.
protected:
    afx_msg int  OnCreate (LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnWindowPosChanged (WINDOWPOS* lpwndpos);
    afx_msg void OnSetFocus (CWnd* pOldWnd);

protected:
    nonvirtual void Layout ();

protected:
    afx_msg void OLE_InvalidateLayout ();

#if qDontUIActivateOnOpen
protected:
    virtual HRESULT OnOpen (BOOL bTryInPlace, LPMSG pMsg) override;
#endif

protected:
    virtual void OnBackColorChanged () override;
#if qFunnyDisplayInDesignMode
    virtual void OnAmbientPropertyChange (DISPID dispid) override;
#endif

protected:
    afx_msg UINT OLE_VersionNumber ();
    afx_msg BSTR OLE_ShortVersionString ();
    afx_msg BOOL OLE_GetReadOnly ();
    afx_msg void OLE_SetReadOnly (BOOL bNewValue);
    afx_msg BOOL OLE_GetEnabled ();
    afx_msg void OLE_SetEnabled (BOOL bNewValue);
    afx_msg BOOL OLE_GetEnableAutoChangesBackgroundColor ();
    afx_msg void OLE_SetEnableAutoChangesBackgroundColor (BOOL bNewValue);
    afx_msg int  OLE_GetWindowMarginTop ();
    afx_msg void OLE_SetWindowMarginTop (int windowMarginTop);
    afx_msg int  OLE_GetWindowMarginLeft ();
    afx_msg void OLE_SetWindowMarginLeft (int windowMarginLeft);
    afx_msg int  OLE_GetWindowMarginBottom ();
    afx_msg void OLE_SetWindowMarginBottom (int windowMarginBottom);
    afx_msg int  OLE_GetWindowMarginRight ();
    afx_msg void OLE_SetWindowMarginRight (int windowMarginRight);
    afx_msg int  OLE_GetPrintMarginTop ();
    afx_msg void OLE_SetPrintMarginTop (int printMarginTop);
    afx_msg int  OLE_GetPrintMarginLeft ();
    afx_msg void OLE_SetPrintMarginLeft (int printMarginLeft);
    afx_msg int  OLE_GetPrintMarginBottom ();
    afx_msg void OLE_SetPrintMarginBottom (int printMarginBottom);
    afx_msg int  OLE_GetPrintMarginRight ();
    afx_msg void OLE_SetPrintMarginRight (int printMarginRight);
    afx_msg UINT GetHasVerticalScrollBar ();
    afx_msg void SetHasVerticalScrollBar (UINT bNewValue);
    afx_msg UINT GetHasHorizontalScrollBar ();
    afx_msg void SetHasHorizontalScrollBar (UINT bNewValue);
    afx_msg BSTR GetBufferText ();
    afx_msg void SetBufferText (LPCTSTR text);
    afx_msg BSTR GetBufferTextCRLF ();
    afx_msg void SetBufferTextCRLF (LPCTSTR text);
    afx_msg BSTR GetBufferTextAsRTF ();
    afx_msg string GetBufferTextAsRTF_ ();
    afx_msg void   SetBufferTextAsRTF (LPCTSTR text);
    afx_msg BSTR GetBufferTextAsHTML ();
    afx_msg void SetBufferTextAsHTML (LPCTSTR text);
    afx_msg VARIANT GetBufferTextAsDIB ();
    afx_msg long    GetBufferLength ();
    afx_msg long    GetMaxLength ();
    afx_msg void    SetMaxLength (long maxLength);
    afx_msg BOOL GetSupportContextMenu ();
    afx_msg void SetSupportContextMenu (BOOL bNewValue);
    afx_msg BOOL OLE_GetHideDisabledContextMenuItems ();
    afx_msg void OLE_SetHideDisabledContextMenuItems (BOOL bNewValue);
    afx_msg BOOL GetSmartCutAndPaste ();
    afx_msg void SetSmartCutAndPaste (BOOL bNewValue);
    afx_msg BOOL OLE_GetSmartQuoteMode ();
    afx_msg void OLE_SetSmartQuoteMode (BOOL bNewValue);
    afx_msg BOOL GetWrapToWindow ();
    afx_msg void SetWrapToWindow (BOOL bNewValue);
    afx_msg BOOL GetShowParagraphGlyphs ();
    afx_msg void SetShowParagraphGlyphs (BOOL bNewValue);
    afx_msg BOOL GetShowTabGlyphs ();
    afx_msg void SetShowTabGlyphs (BOOL bNewValue);
    afx_msg BOOL GetShowSpaceGlyphs ();
    afx_msg void SetShowSpaceGlyphs (BOOL bNewValue);
    afx_msg BOOL OLE_GetUseSelectEOLBOLRowHilightStyle ();
    afx_msg void OLE_SetUseSelectEOLBOLRowHilightStyle (BOOL bNewValue);
    afx_msg BOOL OLE_GetShowSecondaryHilight ();
    afx_msg void OLE_SetShowSecondaryHilight (BOOL bNewValue);

    // Hidable text
protected:
    afx_msg BOOL OLE_GetShowHidableText ();
    afx_msg void OLE_SetShowHidableText (BOOL bNewValue);
    afx_msg OLE_COLOR OLE_GetHidableTextColor ();
    afx_msg void      OLE_SetHidableTextColor (OLE_COLOR color);
    afx_msg BOOL OLE_GetHidableTextColored ();
    afx_msg void OLE_SetHidableTextColored (BOOL bNewValue);

    // Spell checking
protected:
    afx_msg VARIANT OLE_GetSpellChecker ();
    afx_msg void    OLE_SetSpellChecker (VARIANT& newValue);

private:
    IDispatch*                fSpellChecker;
    COMBased_SpellCheckEngine fLedSpellCheckWrapper;

private:
    nonvirtual void ChangedSpellCheckerCOMObject ();

    // Context Menu (and commands) support
private:
    CComPtr<IDispatch> fConextMenu;
    CComPtr<IDispatch> fToolbarList;
    CComPtr<IDispatch> fBuiltinCommands;
    CComPtr<IDispatch> fPredefinedMenus;
    CComPtr<IDispatch> fAcceleratorTable;

protected:
    afx_msg VARIANT OLE_GetContextMenu ();
    afx_msg void    OLE_SetContextMenu (VARIANT& newValue);
    afx_msg VARIANT OLE_GetToolbarList ();
    afx_msg void    OLE_SetToolbarList (VARIANT& newValue);
    afx_msg VARIANT OLE_GetBuiltinCommands ();
    afx_msg VARIANT OLE_GetPredefinedMenus ();
    afx_msg IDispatch* OLE_GetDefaultContextMenu ();
    afx_msg IDispatch* OLE_GetDefaultAcceleratorTable ();
    afx_msg IDispatch* OLE_MakeNewPopupMenuItem ();
    afx_msg IDispatch* OLE_MakeNewUserMenuItem ();
    afx_msg IDispatch* OLE_MakeNewAcceleratorElement ();
    afx_msg void       OLE_InvokeCommand (const VARIANT& command);
    afx_msg BOOL OLE_CommandEnabled (const VARIANT& command);
    afx_msg BOOL OLE_CommandChecked (const VARIANT& command);
    afx_msg IDispatch* OLE_MakeNewToolbarList ();
    nonvirtual CComPtr<IDispatch> MakeNewToolbar ();
    afx_msg IDispatch* OLE_MakeNewToolbar ();
    afx_msg IDispatch* OLE_MakeIconButtonToolbarItem ();
    nonvirtual CComPtr<IDispatch> MakeSeparatorToolbarItem ();
    afx_msg IDispatch* OLE_MakeSeparatorToolbarItem ();
    nonvirtual CComPtr<IDispatch> MakeBuiltinToolbar (LPCOLESTR builtinToolbarName);
    afx_msg IDispatch* OLE_MakeBuiltinToolbar (LPCOLESTR builtinToolbarName);
    nonvirtual CComPtr<IDispatch> MakeBuiltinToolbarItem (LPCOLESTR builtinToolbarItemName);
    afx_msg IDispatch* OLE_MakeBuiltinToolbarItem (LPCOLESTR builtinToolbarItemName);
    afx_msg VARIANT OLE_GetAcceleratorTable ();
    afx_msg void    OLE_SetAcceleratorTable (VARIANT& newValue);

private:
    struct ToolBarIconSpec {
        const TCHAR*    fIconName;
        int             fIconResId;
        const TCHAR*    fCmdName;
        IconButtonStyle fButtonStyle;
    };
    nonvirtual CComPtr<IDispatch> mkIconElement (int iconResID);
    nonvirtual CComPtr<IDispatch> mkIconElement (const ToolBarIconSpec& s);
    nonvirtual CComPtr<IDispatch> mkIconElement (int iconResID, CComPtr<IDispatch> cmdList);
    nonvirtual CComPtr<IDispatch> MakeBuiltinComboBoxToolbarItem (CComPtr<IDispatch> cmdList);

private:
    nonvirtual HACCEL GetCurrentWin32AccelTable ();

private:
    HACCEL                    fWin32AccelTable;
    Time::DurationSecondsType fLastAccelTableUpdateAt; // speed tweek

protected:
    afx_msg VARIANT OLE_GetCurrentEventArguments ();

private:
    CComPtr<IDispatch> fCurrentEventArguments;

protected:
    //selection-based APIs
    afx_msg long GetSelStart ();
    afx_msg void SetSelStart (long start);
    afx_msg long GetSelLength ();
    afx_msg void SetSelLength (long length);
    afx_msg BSTR GetSelText ();
    afx_msg void SetSelText (LPCTSTR text);
    afx_msg BSTR GetSelTextAsRTF ();
    afx_msg void SetSelTextAsRTF (LPCTSTR text);
    afx_msg BSTR GetSelTextAsHTML ();
    afx_msg void SetSelTextAsHTML (LPCTSTR text);
    afx_msg OLE_COLOR GetSelColor ();
    afx_msg void      SetSelColor (OLE_COLOR color);
    afx_msg BSTR GetSelFontFace ();
    afx_msg void SetSelFontFace (LPCTSTR fontFace);
    afx_msg long GetSelFontSize ();
    afx_msg void SetSelFontSize (long size);
    afx_msg long GetSelBold ();
    afx_msg void SetSelBold (long bold);
    afx_msg long GetSelItalic ();
    afx_msg void SetSelItalic (long italic);
    afx_msg long GetSelStrikeThru ();
    afx_msg void SetSelStrikeThru (long strikeThru);
    afx_msg long GetSelUnderline ();
    afx_msg void SetSelUnderline (long underline);
    afx_msg UINT OLE_GetSelJustification ();
    afx_msg void OLE_SetSelJustification (UINT justification);
    afx_msg UINT OLE_GetSelListStyle ();
    afx_msg void OLE_SetSelListStyle (UINT listStyle);
    afx_msg UINT OLE_GetSelHidable ();
    afx_msg void OLE_SetSelHidable (UINT hidable);

    afx_msg void AboutBox ();
    afx_msg void LoadFile (LPCTSTR filename);
    afx_msg void SaveFile (LPCTSTR filename);
    afx_msg void SaveFileCRLF (LPCTSTR filename);
    afx_msg void SaveFileRTF (LPCTSTR filename);
    afx_msg void SaveFileHTML (LPCTSTR filename);
    afx_msg void Refresh ();
    afx_msg void ScrollToSelection ();

    //UNDO support
public:
    afx_msg long OLE_GetMaxUndoLevel ();
    afx_msg void OLE_SetMaxUndoLevel (long maxUndoLevel);
    afx_msg BOOL OLE_GetCanUndo ();
    afx_msg BOOL OLE_GetCanRedo ();
    afx_msg void OLE_Undo ();
    afx_msg void OLE_Redo ();
    afx_msg void OLE_CommitUndo ();

public:
    afx_msg void OLE_LaunchFontSettingsDialog ();
    afx_msg void OLE_LaunchParagraphSettingsDialog ();

    // Find dialog
public:
    afx_msg void OLE_LaunchFindDialog ();
    afx_msg long OLE_Find (long searchFrom, const VARIANT& findText, BOOL wrapSearch, BOOL wholeWordSearch, BOOL caseSensativeSearch);

    // Replace dialog
public:
    afx_msg void OLE_LaunchReplaceDialog ();
    afx_msg long OLE_FindReplace (long searchFrom, const VARIANT& findText, const VARIANT& replaceText, BOOL wrapSearch, BOOL wholeWordSearch, BOOL caseSensativeSearch);

    // Printing support
public:
    afx_msg void OLE_PrinterSetupDialog ();
    afx_msg void OLE_PrintDialog ();
    afx_msg void OLE_PrintOnce ();

    // GetHeight
public:
    afx_msg long OLE_GetHeight (long from, long to);

public:
    afx_msg BOOL OLE_GetDirty ();
    afx_msg void OLE_SetDirty (BOOL dirty);

private:
    bool fDataDirty; // keep separate flag - instead of using IsModfied/SetModifiedFlag () - cuz those set for other properties than the text (like control size etc). This is only for DOCUMENT data being dirty.

    //LedItViewController callbacks
public:
    virtual void OnBrowseHelpCommand () override;
    virtual void OnAboutBoxCommand () override;
    virtual void ForceUIActive () override;
    virtual void FireOLEEvent (DISPID eventID) override;
    virtual void FireOLEEvent (DISPID dispid, BYTE* pbParams, ...) override;
    virtual void FireUpdateUserCommand (const wstring& internalCmdName, VARIANT_BOOL* enabled, VARIANT_BOOL* checked, wstring* name) override;
    virtual void FireUserCommand (const wstring& internalCmdName) override;
#if qFunnyDisplayInDesignMode
    virtual bool IsInDesignMode () const override;
    virtual bool DrawExtraDesignModeBorder () const override;
#endif
    virtual HMENU GenerateContextMenu () override;

protected:
    DECLARE_DISPATCH_MAP ()
    DECLARE_MESSAGE_MAP ()
    DECLARE_EVENT_MAP ()

private:
    nonvirtual Led_FileFormat GuessFormatFromName (LPCTSTR name);
    nonvirtual void           DoReadFile (LPCTSTR filename, Memory::SmallStackBuffer<char>* buffer, size_t* size);
    nonvirtual void           WriteBytesToFile (LPCTSTR filename, const void* buffer, size_t size);

    // Dispatch and event IDs
public:
    LedItView              fEditor;
    StyledTextIO::HTMLInfo fHTMLInfo;
};

DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//  class   ActiveLedItControl::OnCreateExtras
inline ActiveLedItControl::OnCreateExtras::OnCreateExtras ()
    : fReadOnly (false)
    , fEnabled (true)
{
}

#endif /*__ActiveLedItControl_h__*/
