/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 * Description:
 *      Implementation of the ActiveLedItControl ActiveX Control class.
 *
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <climits>

#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

#include <afxctl.h>

#include <atlbase.h>

#include "Stroika/Foundation/Characters/LineEndings.h"

#include "Stroika/Frameworks/Led/StdDialogs.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_LedNative.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_PlainText.h"

#include "ActiveLedIt.h"
#include "ActiveLedItPpg.h"
#include "DispIDs.h"
#include "FontMenu.h"
#include "Toolbar.h"
#include "UserConfigCommands.h"

#include "ActiveLedItControl.h"

#include "ActiveLedIt_h.h"

// Not sure about this - experiment... See spr#0521
#define qDisableEditorWhenNotActive 0

#define CATCH_AND_HANDLE_EXCEPTIONS()                   \
    catch (HRESULT hr)                                  \
    {                                                   \
        AfxThrowOleException (hr);                      \
    }                                                   \
    catch (Win32ErrorException & we)                    \
    {                                                   \
        AfxThrowOleException (HRESULT_FROM_WIN32 (we)); \
    }                                                   \
    catch (HRESULTErrorException & h)                   \
    {                                                   \
        AfxThrowOleException (h);                       \
    }                                                   \
    catch (...)                                         \
    {                                                   \
        AfxThrowMemoryException ();                     \
    }

const int kEditorWindowID = 100;

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::StyledTextIO;

/*
     ********************************************************************************
     ************************** COMBased_SpellCheckEngine ***************************
     ********************************************************************************
     */

COMBased_SpellCheckEngine::COMBased_SpellCheckEngine (IDispatch* engine)
    : inherited ()
    , fEngine (engine)
{
    // NB: We use IDispatch interface rather than the vtable interface to avoid having to directly include the spellchecker IDL here,
    // and having to compile it ourselves in this project
}

bool COMBased_SpellCheckEngine::ScanForUndefinedWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
                                                      const Led_tChar** wordStartResult, const Led_tChar** wordEndResult)
{
    RequireNotNull (startBuf);
    Require (startBuf <= endBuf);

    if (fEngine != NULL) {
        size_t textBufLen = endBuf - startBuf;
        size_t cursorIdx  = *cursor == NULL ? 0 : (*cursor - startBuf);

        BSTR        bstrVal = ::SysAllocStringLen (startBuf, textBufLen);
        CComVariant textToScan (bstrVal);
        ::SysFreeString (bstrVal);
        CComVariant result;
        CComVariant cursorIdxAsCCV = cursorIdx;
        Led_ThrowIfErrorHRESULT (fEngine.Invoke2 (CA2W ("CreateScanContext"), &textToScan, &cursorIdxAsCCV, &result));
        Led_ThrowIfErrorHRESULT (result.ChangeType (VT_DISPATCH));
        if (result.vt == VT_DISPATCH) {
            CComPtr<IDispatch> scanContext = result.pdispVal;
            CComVariant        comCursor;
            if (SUCCEEDED (scanContext.GetPropertyByName (CA2W ("Cursor"), &comCursor))) {
                size_t sbCursor = comCursor.uintVal;
                sbCursor        = max (sbCursor, cursorIdx + 1);
                sbCursor        = min (sbCursor, textBufLen);
                *cursor         = startBuf + sbCursor;
            }
            CComVariant comWordFound;
            if (SUCCEEDED (scanContext.GetPropertyByName (CA2W ("WordFound"), &comWordFound)) and comWordFound.boolVal) {
                CComVariant comWordStart;
                CComVariant comWordEnd;
                if (SUCCEEDED (scanContext.GetPropertyByName (CA2W ("WordStart"), &comWordStart)) and
                    SUCCEEDED (scanContext.GetPropertyByName (CA2W ("WordEnd"), &comWordEnd)) and
                    SUCCEEDED (comWordStart.ChangeType (VT_UI4)) and
                    SUCCEEDED (comWordEnd.ChangeType (VT_UI4))) {
                    size_t newWS     = comWordStart.uintVal;
                    size_t newWE     = comWordEnd.uintVal;
                    newWS            = min (newWS, textBufLen);
                    newWE            = min (newWE, textBufLen);
                    newWE            = max (newWS, newWE);
                    *wordStartResult = startBuf + newWS;
                    *wordEndResult   = startBuf + newWE;
                    return true;
                }
            }
        }
    }
    return false;
}

bool COMBased_SpellCheckEngine::LookupWord_ (const Led_tString& checkWord, Led_tString* matchedWordResult)
{
    CComVariant comMissingWord;
    CComVariant result;
    CComVariant checkWordCCV = CComVariant (checkWord.c_str ());
    Led_ThrowIfErrorHRESULT (fEngine.Invoke2 (CA2W ("LookupWord"), &checkWordCCV, matchedWordResult == NULL ? NULL : &comMissingWord, &result));
    if (SUCCEEDED (result.ChangeType (VT_BOOL)) and
        result.boolVal) {
        if (matchedWordResult != NULL) {
            if (SUCCEEDED (comMissingWord.ChangeType (VT_BSTR))) {
                *matchedWordResult = Led_tString (comMissingWord.bstrVal);
            }
            else {
                // even if MW found - if call requested matchedWord and COM didn't provide - treat that as failure...
                return false;
            }
        }
        return true;
    }
    return false;
}

vector<Led_tString> COMBased_SpellCheckEngine::GenerateSuggestions (const Led_tString& misspelledWord)
{
    CComVariant comResult;
    CComVariant misspelledWordAsCCV = misspelledWord.c_str ();
    Led_ThrowIfErrorHRESULT (fEngine.Invoke1 (CA2W ("GenerateSuggestions"), &misspelledWordAsCCV, &comResult));
    return UnpackVectorOfStringsFromVariantArray (comResult);
}

TextBreaks* COMBased_SpellCheckEngine::PeekAtTextBreaksUsed ()
{
    return this;
}

void COMBased_SpellCheckEngine::FindWordBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
                                                size_t* wordStartResult, size_t* wordEndResult, bool* wordReal) const
{
    Led_tString        text = Led_tString (startOfText, startOfText + lengthOfText);
    CComVariant        wordInfoResult;
    CComPtr<IDispatch> engine                               = fEngine;
    CComVariant        textOffsetToStartLookingForWordAsCCV = textOffsetToStartLookingForWord;
    CComVariant        textAsCCV                            = text.c_str ();
    Led_ThrowIfErrorHRESULT (engine.Invoke2 (CA2W ("FindWordBreaks"), &textAsCCV, &textOffsetToStartLookingForWordAsCCV, &wordInfoResult));
    Led_ThrowIfErrorHRESULT (wordInfoResult.ChangeType (VT_DISPATCH));
    if (wordInfoResult.vt == VT_DISPATCH) {
        CComPtr<IDispatch> wordInfo = wordInfoResult.pdispVal;
        // Validate COM results. They COULD be bogus - and that shouldn't crash/assert-out LED
        {
            CComVariant val;
            Led_ThrowIfErrorHRESULT (wordInfo.GetPropertyByName (CA2W ("WordStart"), &val));
            Led_ThrowIfErrorHRESULT (val.ChangeType (VT_UI4));
            *wordStartResult = val.uintVal;
            *wordStartResult = min (*wordStartResult, lengthOfText);
        }
        {
            CComVariant val;
            Led_ThrowIfErrorHRESULT (wordInfo.GetPropertyByName (CA2W ("WordEnd"), &val));
            Led_ThrowIfErrorHRESULT (val.ChangeType (VT_UI4));
            *wordEndResult = val.uintVal;
            *wordEndResult = min (*wordEndResult, lengthOfText);
        }
        {
            CComVariant val;
            Led_ThrowIfErrorHRESULT (wordInfo.GetPropertyByName (CA2W ("WordReal"), &val));
            Led_ThrowIfErrorHRESULT (val.ChangeType (VT_BOOL));
            *wordReal = !!val.boolVal;
        }
    }
}

void COMBased_SpellCheckEngine::FindLineBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
                                                size_t* wordEndResult, bool* wordReal) const
{
    // This really shouldn't be called - since the line-breaks code for a spellchecker isn't necessarily very reasonable.
    // Anyhow - if it ever does get called - this should produce a vaguley reasonable result... LGP 2003-12-16
    size_t wordStartIgnored = 0;
    FindWordBreaks (startOfText, lengthOfText, textOffsetToStartLookingForWord, &wordStartIgnored, wordEndResult, wordReal);
}

COMBased_SpellCheckEngine::UDInterface* COMBased_SpellCheckEngine::GetUDInterface ()
{
    return this;
}

bool COMBased_SpellCheckEngine::AddWordToUserDictionarySupported () const
{
    CComVariant        comResult;
    CComPtr<IDispatch> engine = fEngine;
    Led_ThrowIfErrorHRESULT (engine.GetPropertyByName (CA2W ("AddWordToUserDictionarySupported"), &comResult));
    return SUCCEEDED (comResult.ChangeType (VT_BOOL)) and comResult.boolVal;
}

void COMBased_SpellCheckEngine::AddWordToUserDictionary (const Led_tString& word)
{
    CComVariant wordAsCCV = word.c_str ();
    Led_ThrowIfErrorHRESULT (fEngine.Invoke1 (CA2W ("AddWordToUserDictionary"), &wordAsCCV));
}

/*
     ********************************************************************************
     ************* ActiveLedItControl::ActiveLedItControlFactory ********************
     ********************************************************************************
     */

// Control type information
static const DWORD BASED_CODE _dwActiveLedItOleMisc =
    OLEMISC_SIMPLEFRAME |
    OLEMISC_ACTIVATEWHENVISIBLE | /* See SPR#1522 & qDontUIActivateOnOpen about this */
    OLEMISC_SETCLIENTSITEFIRST |
    OLEMISC_INSIDEOUT |
    OLEMISC_CANTLINKINSIDE |
    OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE (ActiveLedItControl, IDS_ACTIVELEDIT, _dwActiveLedItOleMisc)

// ActiveLedItControl::ActiveLedItControlFactory::UpdateRegistry -
// Adds or removes system registry entries for ActiveLedItControl
BOOL ActiveLedItControl::ActiveLedItControlFactory::UpdateRegistry (BOOL bRegister)
{
    // TODO: Verify that your control follows apartment-model threading rules.
    // Refer to MFC TechNote 64 for more information.
    // If your control does not conform to the apartment-model rules, then
    // you must modify the code below, changing the 6th parameter from
    // afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.
    if (bRegister) {
        return AfxOleRegisterControlClass (
            AfxGetInstanceHandle (),
            m_clsid,
            m_lpszProgID,
            IDS_ACTIVELEDIT,
            IDB_ACTIVELEDIT,
            afxRegInsertable | afxRegApartmentThreading,
            _dwActiveLedItOleMisc,
            _tlid,
            _wVerMajor,
            _wVerMinor);
    }
    else {
        return AfxOleUnregisterClass (m_clsid, m_lpszProgID);
    }
}

/*
     ********************************************************************************
     ******************************** ActiveLedItControl ****************************
     ********************************************************************************
     */
using ALIC = ActiveLedItControl;
IMPLEMENT_DYNCREATE (ActiveLedItControl, COleControl)

BEGIN_MESSAGE_MAP (ActiveLedItControl, COleControl)
ON_WM_CREATE ()
ON_WM_WINDOWPOSCHANGED ()
ON_WM_SETFOCUS ()
ON_WM_ERASEBKGND ()
ON_COMMAND (ID_APP_ABOUT, AboutBox)
ON_OLEVERB (AFX_IDS_VERB_EDIT, OnEdit)
ON_OLEVERB (AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP ()

BEGIN_DISPATCH_MAP (ActiveLedItControl, COleControl)

//  Properties
DISP_STOCKPROP_APPEARANCE ()
DISP_STOCKPROP_BACKCOLOR ()
DISP_STOCKPROP_BORDERSTYLE ()
DISP_STOCKPROP_HWND ()

DISP_FUNCTION_ID (ALIC, "InvalidateLayout", DISPID_InvalidateLayout, OLE_InvalidateLayout, VT_EMPTY, VTS_NONE)

DISP_FUNCTION_ID (ALIC, "VersionNumber", DISPID_VersionNumber, OLE_VersionNumber, VT_I4, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "ShortVersionString", DISPID_ShortVersionString, OLE_ShortVersionString, VT_BSTR, VTS_NONE)
DISP_PROPERTY_EX_ID (ALIC, "ReadOnly", DISPID_ReadOnly, OLE_GetReadOnly, OLE_SetReadOnly, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "Enabled", DISPID_ENABLED, OLE_GetEnabled, OLE_SetEnabled, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "EnableAutoChangesBackgroundColor",
                     DISPID_EnableAutoChangesBackgroundColor,
                     OLE_GetEnableAutoChangesBackgroundColor, OLE_SetEnableAutoChangesBackgroundColor, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "WindowMarginTop", DISPID_WindowMarginTop, OLE_GetWindowMarginTop, OLE_SetWindowMarginTop, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "WindowMarginLeft", DISPID_WindowMarginLeft, OLE_GetWindowMarginLeft, OLE_SetWindowMarginLeft, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "WindowMarginBottom", DISPID_WindowMarginBottom, OLE_GetWindowMarginBottom, OLE_SetWindowMarginBottom, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "WindowMarginRight", DISPID_WindowMarginRight, OLE_GetWindowMarginRight, OLE_SetWindowMarginRight, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "PrintMarginTop", DISPID_PrintMarginTop, OLE_GetPrintMarginTop, OLE_SetPrintMarginTop, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "PrintMarginLeft", DISPID_PrintMarginLeft, OLE_GetPrintMarginLeft, OLE_SetPrintMarginLeft, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "PrintMarginBottom", DISPID_PrintMarginBottom, OLE_GetPrintMarginBottom, OLE_SetPrintMarginBottom, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "PrintMarginRight", DISPID_PrintMarginRight, OLE_GetPrintMarginRight, OLE_SetPrintMarginRight, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "HasVerticalScrollBar", DISPID_HasVerticalScrollBar, GetHasVerticalScrollBar, SetHasVerticalScrollBar, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "HasHorizontalScrollBar", DISPID_HasHorizontalScrollBar, GetHasHorizontalScrollBar, SetHasHorizontalScrollBar, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "Text", DISPID_Text, GetBufferText, SetBufferText, VT_BSTR)
DISP_PROPERTY_EX_ID (ALIC, "TextCRLF", DISPID_TextCRLF, GetBufferTextCRLF, SetBufferTextCRLF, VT_BSTR)
DISP_PROPERTY_EX_ID (ALIC, "TextRTF", DISPID_TextRTF, GetBufferTextAsRTF, SetBufferTextAsRTF, VT_BSTR)
DISP_PROPERTY_EX_ID (ALIC, "TextHTML", DISPID_TextHTML, GetBufferTextAsHTML, SetBufferTextAsHTML, VT_BSTR)
DISP_FUNCTION_ID (ALIC, "TextBitmap", DISPID_TextBitmap, GetBufferTextAsDIB, VT_VARIANT, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "Length", DISPID_Length, GetBufferLength, VT_I4, VTS_NONE)
DISP_PROPERTY_EX_ID (ALIC, "MaxLength", DISPID_MaxLength, GetMaxLength, SetMaxLength, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "SupportContextMenu", DISPID_SupportContextMenu, GetSupportContextMenu, SetSupportContextMenu, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "ContextMenu", DISPID_ContextMenu, OLE_GetContextMenu, OLE_SetContextMenu, VT_VARIANT)
DISP_PROPERTY_EX_ID (ALIC, "Toolbars", DISPID_ToolbarList, OLE_GetToolbarList, OLE_SetToolbarList, VT_VARIANT)
DISP_PROPERTY_EX_ID (ALIC, "HideDisabledContextMenuItems",
                     DISPID_HideDisabledContextMenuItems,
                     OLE_GetHideDisabledContextMenuItems, OLE_SetHideDisabledContextMenuItems, VT_BOOL)
DISP_FUNCTION_ID (ALIC, "BuiltinCommands", DISPID_BuiltinCommands, OLE_GetBuiltinCommands, VT_VARIANT, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "PredefinedMenus", DISPID_PredefinedMenus, OLE_GetPredefinedMenus, VT_VARIANT, VTS_NONE)
DISP_PROPERTY_EX_ID (ALIC, "AcceleratorTable", DISPID_AcceleratorTable, OLE_GetAcceleratorTable, OLE_SetAcceleratorTable, VT_VARIANT)
DISP_FUNCTION_ID (ALIC, "GetDefaultContextMenu", DISPID_GetDefaultContextMenu, OLE_GetDefaultContextMenu, VT_DISPATCH, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "GetDefaultAcceleratorTable", DISPID_GetDefaultAcceleratorTable, OLE_GetDefaultAcceleratorTable, VT_DISPATCH, VTS_NONE)

DISP_FUNCTION_ID (ALIC, "MakeNewPopupMenuItem", DISPID_MakeNewPopupMenuItem, OLE_MakeNewPopupMenuItem, VT_DISPATCH, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "MakeNewUserMenuItem", DISPID_MakeNewUserMenuItem, OLE_MakeNewUserMenuItem, VT_DISPATCH, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "MakeNewAcceleratorElement", DISPID_MakeNewAcceleratorElement, OLE_MakeNewAcceleratorElement, VT_DISPATCH, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "InvokeCommand", DISPID_InvokeCommand, OLE_InvokeCommand, VT_EMPTY, VTS_VARIANT)
DISP_FUNCTION_ID (ALIC, "CommandEnabled", DISPID_CommandEnabled, OLE_CommandEnabled, VT_BOOL, VTS_VARIANT)
DISP_FUNCTION_ID (ALIC, "CommandChecked", DISPID_CommandChecked, OLE_CommandChecked, VT_BOOL, VTS_VARIANT)
DISP_FUNCTION_ID (ALIC, "MakeNewToolbarList", DISPID_MakeNewToolbarList, OLE_MakeNewToolbarList, VT_DISPATCH, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "MakeNewToolbar", DISPID_MakeNewToolbar, OLE_MakeNewToolbar, VT_DISPATCH, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "MakeIconButtonToolbarItem", DISPID_MakeIconButtonToolbarItem, OLE_MakeIconButtonToolbarItem, VT_DISPATCH, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "MakeSeparatorToolbarItem", DISPID_MakeSeparatorToolbarItem, OLE_MakeSeparatorToolbarItem, VT_DISPATCH, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "MakeBuiltinToolbar", DISPID_MakeBuiltinToolbar, OLE_MakeBuiltinToolbar, VT_DISPATCH, VTS_WBSTR)
DISP_FUNCTION_ID (ALIC, "MakeBuiltinToolbarItem", DISPID_MakeBuiltinToolbarItem, OLE_MakeBuiltinToolbarItem, VT_DISPATCH, VTS_WBSTR)
DISP_FUNCTION_ID (ALIC, "CurrentEventArguments", DISPID_CurrentEventArguments, OLE_GetCurrentEventArguments, VT_VARIANT, VTS_NONE)
DISP_PROPERTY_EX_ID (ALIC, "SmartCutAndPaste", DISPID_SmartCutAndPaste, GetSmartCutAndPaste, SetSmartCutAndPaste, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "SmartQuoteMode", DISPID_SmartQuoteMode, OLE_GetSmartQuoteMode, OLE_SetSmartQuoteMode, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "WrapToWindow", DISPID_WrapToWindow, GetWrapToWindow, SetWrapToWindow, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "ShowParagraphGlyphs", DISPID_ShowParagraphGlyphs, GetShowParagraphGlyphs, SetShowParagraphGlyphs, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "ShowTabGlyphs", DISPID_ShowTabGlyphs, GetShowTabGlyphs, SetShowTabGlyphs, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "ShowSpaceGlyphs", DISPID_ShowSpaceGlyphs, GetShowSpaceGlyphs, SetShowSpaceGlyphs, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "UseSelectEOLBOLRowHilightStyle",
                     DISPID_UseSelectEOLBOLRowHilightStyle,
                     OLE_GetUseSelectEOLBOLRowHilightStyle, OLE_SetUseSelectEOLBOLRowHilightStyle, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "ShowSecondaryHilight", DISPID_ShowSecondaryHilight, OLE_GetShowSecondaryHilight, OLE_SetShowSecondaryHilight, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "ShowHidableText", DISPID_ShowHidableText, OLE_GetShowHidableText, OLE_SetShowHidableText, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "HidableTextColor", DISPID_HidableTextColor, OLE_GetHidableTextColor, OLE_SetHidableTextColor, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "HidableTextColored", DISPID_HidableTextColored, OLE_GetHidableTextColored, OLE_SetHidableTextColored, VT_BOOL)
DISP_PROPERTY_EX_ID (ALIC, "SpellChecker", DISPID_SpellChecker, OLE_GetSpellChecker, OLE_SetSpellChecker, VT_VARIANT)
DISP_PROPERTY_EX_ID (ALIC, "SelStart", DISPID_SELSTART, GetSelStart, SetSelStart, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "SelLength", DISPID_SELLENGTH, GetSelLength, SetSelLength, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "SelText", DISPID_SELTEXT, GetSelText, SetSelText, VT_BSTR)
DISP_PROPERTY_EX_ID (ALIC, "SelTextRTF", DISPID_SelTextRTF, GetSelTextAsRTF, SetSelTextAsRTF, VT_BSTR)
DISP_PROPERTY_EX_ID (ALIC, "SelTextHTML", DISPID_SelTextHTML, GetSelTextAsHTML, SetSelTextAsHTML, VT_BSTR)
DISP_PROPERTY_EX_ID (ALIC, "SelColor", DISPID_SelColor, GetSelColor, SetSelColor, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "SelFontFace", DISPID_SelFontFace, GetSelFontFace, SetSelFontFace, VT_BSTR)
DISP_PROPERTY_EX_ID (ALIC, "SelFontSize", DISPID_SelFontSize, GetSelFontSize, SetSelFontSize, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "SelBold", DISPID_SelBold, GetSelBold, SetSelBold, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "SelItalic", DISPID_SelItalic, GetSelItalic, SetSelItalic, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "SelStrikeThru", DISPID_SelStrikeThru, GetSelStrikeThru, SetSelStrikeThru, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "SelUnderline", DISPID_SelUnderline, GetSelUnderline, SetSelUnderline, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "SelJustification", DISPID_SelJustification, OLE_GetSelJustification, OLE_SetSelJustification, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "SelListStyle", DISPID_SelListStyle, OLE_GetSelListStyle, OLE_SetSelListStyle, VT_I4)
DISP_PROPERTY_EX_ID (ALIC, "SelHidable", DISPID_SelHidable, OLE_GetSelHidable, OLE_SetSelHidable, VT_I4)

//  Functions
DISP_FUNCTION_ID (ALIC, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "LoadFile", DISPID_LoadFile, LoadFile, VT_EMPTY, VTS_BSTR)
DISP_FUNCTION_ID (ALIC, "SaveFile", DISPID_SaveFile, SaveFile, VT_EMPTY, VTS_BSTR)
DISP_FUNCTION_ID (ALIC, "SaveFileCRLF", DISPID_SaveFileCRLF, SaveFileCRLF, VT_EMPTY, VTS_BSTR)
DISP_FUNCTION_ID (ALIC, "SaveFileRTF", DISPID_SaveFileRTF, SaveFileRTF, VT_EMPTY, VTS_BSTR)
DISP_FUNCTION_ID (ALIC, "SaveFileHTML", DISPID_SaveFileHTML, SaveFileHTML, VT_EMPTY, VTS_BSTR)
DISP_FUNCTION_ID (ALIC, "Refresh", DISPID_REFRESH, Refresh, VT_EMPTY, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "ScrollToSelection", DISPID_ScrollToSelection, ScrollToSelection, VT_EMPTY, VTS_NONE)

//UNDO support
DISP_PROPERTY_EX_ID (ALIC, "MaxUndoLevel", DISPID_MaxUndoLevel, OLE_GetMaxUndoLevel, OLE_SetMaxUndoLevel, VT_I4)
DISP_FUNCTION_ID (ALIC, "CanUndo", DISPID_CanUndo, OLE_GetCanUndo, VT_BOOL, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "CanRedo", DISPID_CanRedo, OLE_GetCanRedo, VT_BOOL, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "Undo", DISPID_Undo, OLE_Undo, VT_EMPTY, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "Redo", DISPID_Redo, OLE_Redo, VT_EMPTY, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "CommitUndo", DISPID_CommitUndo, OLE_CommitUndo, VT_EMPTY, VTS_NONE)

// Font/Para DLG
DISP_FUNCTION_ID (ALIC, "LaunchFontSettingsDialog", DISPID_LaunchFontSettingsDialog, OLE_LaunchFontSettingsDialog, VT_EMPTY, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "LaunchParagraphSettingsDialog", DISPID_LaunchParagraphSettingsDialog, OLE_LaunchParagraphSettingsDialog, VT_EMPTY, VTS_NONE)

// Find support
DISP_FUNCTION_ID (ALIC, "LaunchFindDialog", DISPID_LaunchFindDialog, OLE_LaunchFindDialog, VT_EMPTY, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "Find", DISPID_Find, OLE_Find, VT_I4, VTS_I4 VTS_VARIANT VTS_BOOL VTS_BOOL VTS_BOOL)

// Replace support
DISP_FUNCTION_ID (ALIC, "LaunchReplaceDialog", DISPID_LaunchReplaceDialog, OLE_LaunchReplaceDialog, VT_EMPTY, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "FindReplace", DISPID_FindReplace, OLE_FindReplace, VT_I4, VTS_I4 VTS_VARIANT VTS_VARIANT VTS_BOOL VTS_BOOL VTS_BOOL)

// Print support
DISP_FUNCTION_ID (ALIC, "PrinterSetupDialog", DISPID_PrinterSetupDialog, OLE_PrinterSetupDialog, VT_EMPTY, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "PrintDialog", DISPID_PrintDialog, OLE_PrintDialog, VT_EMPTY, VTS_NONE)
DISP_FUNCTION_ID (ALIC, "PrintOnce", DISPID_PrintOnce, OLE_PrintOnce, VT_EMPTY, VTS_NONE)

// GetHeight
DISP_FUNCTION_ID (ALIC, "GetHeight", DISPID_GetHeight, OLE_GetHeight, VT_I4, VTS_I4 VTS_I4)

//DIRTY support
DISP_PROPERTY_EX_ID (ALIC, "Dirty", DISPID_Dirty, OLE_GetDirty, OLE_SetDirty, VT_BOOL)

END_DISPATCH_MAP ()

BEGIN_EVENT_MAP (ActiveLedItControl, COleControl)
EVENT_CUSTOM_ID ("Change", DISPID_Change, _IGNORED_, VTS_NONE)
EVENT_STOCK_CLICK ()
EVENT_STOCK_DBLCLICK ()
EVENT_STOCK_KEYDOWN ()
EVENT_STOCK_KEYUP ()
EVENT_STOCK_KEYPRESS ()
EVENT_STOCK_MOUSEDOWN ()
EVENT_STOCK_MOUSEMOVE ()
EVENT_STOCK_MOUSEUP ()
EVENT_STOCK_KEYPRESS ()
EVENT_CUSTOM_ID ("SelChange", DISPID_SelChange, _IGNORED_, VTS_NONE)
EVENT_CUSTOM_ID ("UpdateUserCommand", DISPID_UpdateUserCommand, _IGNORED_, VTS_NONE)
EVENT_CUSTOM_ID ("UserCommand", DISPID_UserCommand, _IGNORED_, VTS_NONE)
END_EVENT_MAP ()

#if qKeepListOfALInstancesForSPR_1599BWA
set<ActiveLedItControl*> ActiveLedItControl::sAll;

const set<ActiveLedItControl*>& ActiveLedItControl::GetAll ()
{
    return sAll;
}
#endif

BEGIN_PROPPAGEIDS (ActiveLedItControl, 1)
PROPPAGEID (ActiveLedItPropPage::guid)
END_PROPPAGEIDS (ActiveLedItControl)

// Initialize class factory and guid
IMPLEMENT_OLECREATE_EX (ActiveLedItControl, "ActiveLedIt.ActiveLedItCtrl.1",
                        0x9a013a76, 0xad34, 0x11d0, 0x8d, 0x9b, 0, 0xa0, 0xc9, 0x8, 0xc, 0x73)

// Type library ID and version
IMPLEMENT_OLETYPELIB (ActiveLedItControl, _tlid, _wVerMajor, _wVerMinor)

ActiveLedItControl::ActiveLedItControl ()
    : COleControl ()
    , MarkerOwner ()
    , LedItViewController ()
    , fSpellChecker (NULL)
    , fLedSpellCheckWrapper (NULL)
    , fConextMenu ()
    , fToolbarList ()
    , fBuiltinCommands ()
    , fPredefinedMenus ()
    , fAcceleratorTable ()
    , fWin32AccelTable (NULL)
    , fLastAccelTableUpdateAt (0)
    , fCurrentEventArguments ()
    , fEditor ()
    , fDataDirty (false)
{
    fConextMenu.Attach (OLE_GetDefaultContextMenu ());
    fToolbarList.Attach (OLE_MakeNewToolbarList ());
    fAcceleratorTable.Attach (OLE_GetDefaultAcceleratorTable ());

    InitializeIIDs (&DIID__DActiveLedIt, &DIID__DActiveLedItEvents);

    SetInitialSize (300, 150); // Specify default control size... Really should be based on GetSystemMetrics, or screen resolution or something...

    EnableSimpleFrame ();

    fTextStore.AddMarkerOwner (this);
    fEditor.SetController (this);
#if qKeepListOfALInstancesForSPR_1599BWA
    sAll.insert (this);
#endif
}

ActiveLedItControl::~ActiveLedItControl ()
{
#if qKeepListOfALInstancesForSPR_1599BWA
    sAll.erase (this);
#endif
    if (fSpellChecker != NULL) {
        fSpellChecker->Release ();
        fSpellChecker = NULL;
        fEditor.SetSpellCheckEngine (NULL);
    }
    fEditor.SetController (NULL);
    fTextStore.RemoveMarkerOwner (this);
    if (fWin32AccelTable != NULL) {
        ::DestroyAcceleratorTable (fWin32AccelTable);
    }
}

void ActiveLedItControl::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    if (updateInfo.fRealContentUpdate) {
        SetModifiedFlag ();
        fDataDirty = true;
        FireOLEEvent (DISPID_Change);
    }
}

TextStore* ActiveLedItControl::PeekAtTextStore () const
{
    return &const_cast<ActiveLedItControl*> (this)->fTextStore;
}

void ActiveLedItControl::OnDraw (CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
    RequireNotNull (pdc);
    TRACE ("ActiveLedItControl::OnDraw (rcBounds= (%d, %d, %d, %d), rcInvalid= (%d, %d, %d, %d))\n",
           rcBounds.top, rcBounds.left, rcBounds.bottom, rcBounds.right,
           rcInvalid.top, rcInvalid.left, rcInvalid.bottom, rcInvalid.right);
    Led_Rect invalRect = AsLedRect (rcInvalid);
    if (invalRect.IsEmpty ()) {
        return;
    }

    Led_Rect oldWinRect = fEditor.GetWindowRect ();
    try {
        Led_Rect newWinRect = oldWinRect;
        if (m_hWnd == NULL) {
            /*
                 *  This can occur occasionally, when called from places like MSWord XP, and wordpad.
                 *  This MAYBE because these apps don't respect OLEMISC_SETCLIENTSITEFIRST?
                 */
            fEditor.SetDefaultTextColor (TextImager::eDefaultBackgroundColor, Led_Color (TranslateColor (GetBackColor ())));

            Led_Size controlSize = Led_Size (0, 0);
            {
                int cx;
                int cy;
                GetControlSize (&cx, &cy);
                controlSize = Led_Size (cy, cx);
            }
            newWinRect = Led_Rect (AsLedRect (rcBounds).GetOrigin (), controlSize);
        }
        else {
            /*
                 *  Queer logic, and I'm not really sure what is right to do here. If the given rcBounds
                 *  are the same as the last size handled to our last OnWindowPosChanged () - then this uses
                 *  the same WindowRect for Led. But otherwise, it offsets it. Without this logic, AL instances
                 *  embedded in WordPad or LedIt mess up badly when not leftmost in the embedding editors
                 *  window. -- LGP 2003-05-06
                 */
            RECT editorWndRct;
            Verify (::GetWindowRect (m_hWnd, &editorWndRct));
            ScreenToClient (&editorWndRct);
            newWinRect += AsLedRect (rcBounds).GetOrigin () - AsLedRect (editorWndRct).GetOrigin ();
        }
        fEditor.SetWindowRect (newWinRect);
        Led_MFC_TabletFromCDC           useTablet = Led_MFC_TabletFromCDC (pdc);
        LedItView::TemporarilyUseTablet tmpUseTablet (fEditor, useTablet);
        fEditor.WindowDrawHelper (useTablet, invalRect, false);
        fEditor.SetWindowRect (oldWinRect);
    }
    catch (...) {
        // Ignore exceptions on display. Would be had. Assert error if ever happens, but in release control,
        // not much we can usefully do...
        Assert (false);
        fEditor.SetWindowRect (oldWinRect);
    }
}

void ActiveLedItControl::OnDrawMetafile (CDC* pDC, const CRect& rcBounds)
{
    /*
         *  Override COleControl::OnDrawMetafile - to properly handle drawing to a metafile.
         *  See SPR#1447.
         */
    RequireNotNull (pDC);
    TRACE ("ActiveLedItControl::OnDrawMetafile (rcBounds= (%d, %d, %d, %d))\n",
           rcBounds.top, rcBounds.left, rcBounds.bottom, rcBounds.right);

    {
        int cx;
        int cy;
        GetControlSize (&cx, &cy);
        CRect rc (0, 0, cx, cy);
        pDC->SetViewportOrg (0, 0);
        pDC->SetViewportExt (cx, cy);
    }

    Led_MFC_TabletFromCDC useTablet                         = Led_MFC_TabletFromCDC (pDC);
    bool                  oldImageUsingOffscreenBitmapsFlag = fEditor.GetImageUsingOffscreenBitmaps ();
    bool                  oldInDrawMetaFileMode             = fEditor.fInDrawMetaFileMode;
    Led_Rect              oldWinRect                        = fEditor.GetWindowRect ();
    try {
        Led_Rect newWinRect = AsLedRect (rcBounds);

        {
            Led_Rect wmr = useTablet->CvtFromTWIPS (fEditor.GetDefaultWindowMargins ());
            newWinRect.top += wmr.GetTop ();
            newWinRect.left += wmr.GetLeft ();
            newWinRect.bottom -= wmr.GetBottom ();
            newWinRect.right -= wmr.GetRight ();
        }

        fEditor.SetImageUsingOffscreenBitmaps (false);
        fEditor.fInDrawMetaFileMode = true;
        if (not newWinRect.IsEmpty ()) {
            LedItView::TemporarilyUseTablet tmpUseTablet (fEditor, useTablet);
            fEditor.SetWindowRect (newWinRect);
            fEditor.WindowDrawHelper (useTablet, AsLedRect (rcBounds), false);
            //  fEditor.WindowDrawHelper (useTablet, newWinRect, true);
            fEditor.SetWindowRect (oldWinRect);
        }
        fEditor.fInDrawMetaFileMode = oldInDrawMetaFileMode;
        fEditor.SetWindowRect (oldWinRect);
        fEditor.SetImageUsingOffscreenBitmaps (oldImageUsingOffscreenBitmapsFlag);
    }
    catch (...) {
        // Ignore exceptions on display. Would be had. Assert error if ever happens, but in release control,
        // not much we can usefully do...
        Assert (false);
        fEditor.SetImageUsingOffscreenBitmaps (oldImageUsingOffscreenBitmapsFlag);
        fEditor.fInDrawMetaFileMode = oldInDrawMetaFileMode;
        fEditor.SetWindowRect (oldWinRect);
    }
}

BOOL ActiveLedItControl::OnEraseBkgnd ([[maybe_unused]] CDC* pDC)
{
    RequireNotNull (pDC);
    return true; // cuz we erase our own background...
}

void ActiveLedItControl::DoPropExchange (CPropExchange* pPX)
{
    RequireNotNull (pPX);
    ExchangeVersion (pPX, MAKELONG (_wVerMinor, _wVerMajor));
    COleControl::DoPropExchange (pPX);
    ExchangeTextAsRTFBlob (pPX);

    // For SPR#0835- added the ability to load these selected properties. May want to add more in the future.
    if (pPX->IsLoading ()) {
        try {
            // default font face for the control
            CString aProp;
            PX_String (pPX, _T("DefaultFontFace"), aProp, _T(""));
            if (not aProp.IsEmpty ()) {
                Led_IncrementalFontSpecification applyFontSpec;
                applyFontSpec.SetFontName ((const TCHAR*)aProp);
                fEditor.SetDefaultFont (applyFontSpec);
            }
        }
        catch (...) {
        }

        try {
            // default font size for the control
            CString aProp;
            PX_String (pPX, _T("DefaultFontSize"), aProp, _T(""));
            if (not aProp.IsEmpty ()) {
                Led_IncrementalFontSpecification applyFontSpec;
                applyFontSpec.SetPointSize (static_cast<Led_FontSpecification::FontSize> (_ttoi ((const TCHAR*)aProp)));
                fEditor.SetDefaultFont (applyFontSpec);
            }
        }
        catch (...) {
        }

        try {
            //
            BOOL aProp = false;
            if (PX_Bool (pPX, _T("ReadOnly"), aProp)) {
                OLE_SetReadOnly (aProp);
            }
        }
        catch (...) {
        }

        try {
            // Plain "Text" property
            CString aProp;
            PX_String (pPX, _T("Text"), aProp, _T(""));
            if ((fTextStore.GetLength () == 0) and (not aProp.IsEmpty ())) {
                SetBufferTextCRLF (aProp);
                fDataDirty = false;
            }
        }
        catch (...) {
        }

        try {
            // "TextRTF" property
            CString aProp;
            PX_String (pPX, _T("TextRTF"), aProp, _T(""));
            if ((fTextStore.GetLength () == 0) and (not aProp.IsEmpty ())) {
                SetBufferTextAsRTF (aProp);
                fDataDirty = false;
            }
        }
        catch (...) {
        }

        try {
            // "TextHTML" property
            CString aProp;
            PX_String (pPX, _T("TextHTML"), aProp, _T(""));
            if ((fTextStore.GetLength () == 0) and (not aProp.IsEmpty ())) {
                SetBufferTextAsHTML (aProp);
                fDataDirty = false;
            }
        }
        catch (...) {
        }
    }
}

// ActiveLedItControl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
// For information on using these flags, please see MFC technical note
// #nnn, "Optimizing an ActiveX Control".
DWORD ActiveLedItControl::GetControlFlags ()
{
    DWORD dwFlags = COleControl::GetControlFlags ();
#if 0
        // The control will not be redrawn when making the transition
        // between the active and inactivate state.
        dwFlags |= noFlickerActivate;

//DISABLED CUZ WE DON'T SUPPORT THIS IN OUR ONDRAW() method...
        // The control can optimize its OnDraw method, by not restoring
        // the original GDI objects in the device context.
        dwFlags |= canOptimizeDraw;
#endif
    return dwFlags;
}

void ActiveLedItControl::OnGetControlInfo (LPCONTROLINFO pControlInfo)
{
    RequireNotNull (pControlInfo);
    ::memset (pControlInfo, 0, sizeof (*pControlInfo));
    pControlInfo->cb = sizeof (*pControlInfo);

    // Unfortunately, the Win32 SDK documeantion is vague about the lifetime of the array returns
    // as part of the CONTROLINFO structure. Hopefully this will cause no problems. Empirically - it
    // doesn't seem to get called very often. And - the value should rarely change (but under OLE AUT control,
    // so it COULD) -- LGP 2004-01-24
    HACCEL hAccel = GetCurrentWin32AccelTable ();

    //pControlInfo->hAccel = ::LoadAccelerators (AfxGetResourceHandle (),MAKEINTRESOURCE (kAcceleratorTableID));
    pControlInfo->hAccel = hAccel;
    // NB: queer - but CopyAcceleratorTable also counts # of entries...
    pControlInfo->cAccel  = pControlInfo->hAccel == NULL ? 0 : static_cast<USHORT> (::CopyAcceleratorTable (pControlInfo->hAccel, NULL, 0));
    pControlInfo->dwFlags = CTRLINFO_EATS_RETURN;
}

BOOL ActiveLedItControl::PreTranslateMessage (MSG* pMsg)
{
    RequireNotNull (pMsg);
    switch (pMsg->message) {
        case WM_KEYDOWN:
        case WM_KEYUP:
            switch (pMsg->wParam) {
                case VK_TAB: {
                    // Quirks here are for SPR#0829
                    if (pMsg->message == WM_KEYDOWN) {
                        fEditor.SendMessage (WM_CHAR, VK_TAB, pMsg->lParam);
                    }
                    return TRUE;
                } break;
                case VK_UP:
                case VK_DOWN:
                case VK_LEFT:
                case VK_RIGHT:
                case VK_HOME:
                case VK_END:
                case VK_PRIOR:
                case VK_NEXT:
                    fEditor.SendMessage (pMsg->message, pMsg->wParam, pMsg->lParam);
                    return TRUE;
            }
            break;
    }

    HACCEL hAccel = GetCurrentWin32AccelTable ();
    if (hAccel != NULL) {
        return ::TranslateAccelerator (pMsg->hwnd, hAccel, pMsg);
    }
    return false; // so command dispatched normally
}

void ActiveLedItControl::AddFrameLevelUI ()
{
    // Kindof a hack - seems this is called when we get activated, and RemoveFrameLevelUI () is called when we are
    // deactivated. This is about the behavior we want for those cases. Sadly, I've found no docs to see the
    // "right" way to handle this...
    COleControl::AddFrameLevelUI ();
#if qDisableEditorWhenNotActive
    fEditor.ModifyStyle (WS_DISABLED, 0);
    fEditor.SetFocus ();
#endif
}

void ActiveLedItControl::RemoveFrameLevelUI ()
{
#if qDisableEditorWhenNotActive
    fEditor.ModifyStyle (0, WS_DISABLED);
    if (CWnd::GetFocus () == &fEditor) {
        // Not sure who to set the focus to, but just anybody but ME...
        CWnd::GetDesktopWindow ()->SetFocus ();
    }
#endif
    COleControl::RemoveFrameLevelUI ();
}

BOOL ActiveLedItControl::OnSetObjectRects (LPCRECT lprcPosRect, LPCRECT lprcClipRect)
{
#if qDefaultTracingOn
    if (lprcClipRect == NULL) {
        DbgTrace (Led_SDK_TCHAROF ("ActiveLedItControl::OnSetObjectRects (m_bUIActive=%d, m_bInPlaceSiteWndless=%d, lprcPosRect=(%d, %d, %d, %d), NULL)\n"),
                  m_bUIActive, m_bInPlaceSiteWndless,
                  lprcPosRect->top, lprcPosRect->left, lprcPosRect->bottom, lprcPosRect->right);
    }
    else {
        DbgTrace (Led_SDK_TCHAROF ("ActiveLedItControl::OnSetObjectRects (m_bUIActive=%d, m_bInPlaceSiteWndless=%d, lprcPosRect=(%d, %d, %d, %d), lprcClipRect=(%d, %d, %d, %d))\n"),
                  m_bUIActive, m_bInPlaceSiteWndless,
                  lprcPosRect->top, lprcPosRect->left, lprcPosRect->bottom, lprcPosRect->right,
                  lprcClipRect->top, lprcClipRect->left, lprcClipRect->bottom, lprcClipRect->right);
    }
#endif
#if 0
        // keep debugging hack for SPR#1168
        if (lprcClipRect != NULL) {
            if (AsLedRect (*lprcPosRect) != AsLedRect (*lprcClipRect)) {
                int breakHEre = 1;
            }
        }
#endif
    BOOL r = COleControl::OnSetObjectRects (lprcPosRect, lprcClipRect);
    return r;
}

BOOL ActiveLedItControl::OnGetNaturalExtent (DWORD /* dwAspect */, LONG /* lindex */,
                                             DVTARGETDEVICE* /* ptd */, HDC /* hicTargetDev */,
                                             DVEXTENTINFO* /*pExtentInfo*/, LPSIZEL /*psizel*/
)
{
    // Tried overriding this to address teh funny size of the AL widget when inserted into
    // LedIt 3.1a6x. Its a very small issue. BUt - for whatever reason - this method never
    // seems to get called - LGP 2003-04-29.
    return FALSE;
}

void ActiveLedItControl::OnResetState ()
{
    COleControl::OnResetState (); // Resets defaults found in DoPropExchange
}

void ActiveLedItControl::ExchangeTextAsRTFBlob (CPropExchange* pPX)
{
    const LPCTSTR kTextAsRTFBlob = _T("TextAsRTFBlob");
    RequireNotNull (pPX);
    if (pPX->IsLoading ()) {
        HGLOBAL hglobal = NULL;
        PX_Blob (pPX, kTextAsRTFBlob, hglobal);
        if (hglobal != NULL) {
            try {
                Led_StackBasedHandleLocker hdl (hglobal);
                const byte*                data = reinterpret_cast<byte*> (hdl.GetPointer ());
                if (data != NULL) {
                    size_t size = *(size_t*)data;
                    string s    = string (((const char*)data) + sizeof (size_t), size);
                    SetBufferTextAsRTF (Led_ANSI2SDKString (s).c_str ());
                }
                ::GlobalFree (hglobal);
            }
            catch (...) {
                if (hglobal != NULL) {
                    ::GlobalFree (hglobal);
                }
                throw;
            }
        }
    }
    else {
        string  s       = GetBufferTextAsRTF_ ();
        size_t  len     = s.length ();
        HGLOBAL hglobal = ::GlobalAlloc (GMEM_MOVEABLE, len + sizeof (size_t));
        if (hglobal != NULL) {
            {
                Led_StackBasedHandleLocker hdl (hglobal);
                void*                      pvBlob = hdl.GetPointer ();
                AssertNotNull (pvBlob);
                *(long*)pvBlob = len;
                ::memcpy (reinterpret_cast<char*> (pvBlob) + sizeof (size_t), s.c_str (), len);
            }
            try {
                PX_Blob (pPX, kTextAsRTFBlob, hglobal);
                ::GlobalFree (hglobal);
            }
            catch (...) {
                ::GlobalFree (hglobal);
                throw;
            }
        }
    }
}

const DWORD dwSupportedBits =
    INTERFACESAFE_FOR_UNTRUSTED_CALLER |
    INTERFACESAFE_FOR_UNTRUSTED_DATA;
const DWORD dwNotSupportedBits = ~dwSupportedBits;

BEGIN_INTERFACE_MAP (ActiveLedItControl, COleControl)
INTERFACE_PART (ActiveLedItControl, IID_IObjectSafety, ObjSafe)
END_INTERFACE_MAP ()

ULONG FAR EXPORT ActiveLedItControl::XObjSafe::AddRef ()
{
    METHOD_PROLOGUE (ActiveLedItControl, ObjSafe)
    return pThis->ExternalAddRef ();
}

ULONG FAR EXPORT ActiveLedItControl::XObjSafe::Release ()
{
    METHOD_PROLOGUE (ActiveLedItControl, ObjSafe)
    return pThis->ExternalRelease ();
}

HRESULT FAR EXPORT ActiveLedItControl::XObjSafe::QueryInterface (
    REFIID iid, void FAR* FAR* ppvObj)
{
    METHOD_PROLOGUE (ActiveLedItControl, ObjSafe)
    return (HRESULT)pThis->ExternalQueryInterface (&iid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
// CStopLiteCtrl::XObjSafe::GetInterfaceSafetyOptions
// Allows container to query what interfaces are safe for what. We're
// optimizing significantly by ignoring which interface the caller is
// asking for.
HRESULT STDMETHODCALLTYPE
ActiveLedItControl::XObjSafe::GetInterfaceSafetyOptions (
    /* [in] */ REFIID riid,
    /* [out] */ DWORD __RPC_FAR* pdwSupportedOptions,
    /* [out] */ DWORD __RPC_FAR* pdwEnabledOptions)
{
    METHOD_PROLOGUE (ActiveLedItControl, ObjSafe)

    HRESULT retval = ResultFromScode (S_OK);

    // does interface exist?
    IUnknown FAR* punkInterface;
    retval = pThis->ExternalQueryInterface (&riid,
                                            (void**)&punkInterface);
    if (retval != E_NOINTERFACE) { // interface exists
        punkInterface->Release (); // release it--just checking!
    }

    // we support both kinds of safety and have always both set,
    // regardless of interface
    *pdwSupportedOptions = *pdwEnabledOptions = dwSupportedBits;

    return retval; // E_NOINTERFACE if QI failed
}

/////////////////////////////////////////////////////////////////////////////
// CStopLiteCtrl::XObjSafe::SetInterfaceSafetyOptions
// Since we're always safe, this is a no-brainer--but we do check to make
// sure the interface requested exists and that the options we're asked to
// set exist and are set on (we don't support unsafe mode).
HRESULT STDMETHODCALLTYPE
ActiveLedItControl::XObjSafe::SetInterfaceSafetyOptions (
    /* [in] */ REFIID riid,
    /* [in] */ DWORD  dwOptionSetMask,
    /* [in] */ DWORD  dwEnabledOptions)
{
    METHOD_PROLOGUE (ActiveLedItControl, ObjSafe)

    // does interface exist?
    IUnknown FAR* punkInterface;
    pThis->ExternalQueryInterface (&riid, (void**)&punkInterface);
    if (punkInterface) {           // interface exists
        punkInterface->Release (); // release it--just checking!
    }
    else { // interface doesn't exist
        return ResultFromScode (E_NOINTERFACE);
    }

    // can't set bits we don't support
    if (dwOptionSetMask & dwNotSupportedBits) {
        return ResultFromScode (E_FAIL);
    }

    // can't set bits we do support to zero
    dwEnabledOptions &= dwSupportedBits;
    // (we already know there are no extra bits in mask )
    if ((dwOptionSetMask & dwEnabledOptions) !=
        dwOptionSetMask) {
        return ResultFromScode (E_FAIL);
    }

    // don't need to change anything since we're always safe
    return ResultFromScode (S_OK);
}

void ActiveLedItControl::AboutBox ()
{
    OnAboutBoxCommand ();
}

Led_FileFormat ActiveLedItControl::GuessFormatFromName (LPCTSTR name)
{
    Led_FileFormat format   = eUnknownFormat;
    Led_SDK_String pathName = name;
    Led_SDK_String fileName = pathName.substr (pathName.rfind (_T ("\\")) + 1);
    Led_SDK_String suffix   = (fileName.rfind (_T (".")) == Led_SDK_String::npos) ? _T ("") : fileName.substr (fileName.rfind (_T (".")) + 1);
    if (::_tcsicmp (suffix.c_str (), _T ("txt")) == 0) {
        format = eTextFormat;
    }
    else if (::_tcsicmp (suffix.c_str (), _T ("led")) == 0) {
        format = eLedPrivateFormat;
    }
    else if (::_tcsicmp (suffix.c_str (), _T ("rtf")) == 0) {
        format = eRTFFormat;
    }
    else if (::_tcsicmp (suffix.c_str (), _T ("htm")) == 0 or ::_tcsicmp (suffix.c_str (), _T ("html")) == 0) {
        format = eHTMLFormat;
    }
    return format;
}

void ActiveLedItControl::DoReadFile (LPCTSTR filename, Memory::SmallStackBuffer<char>* buffer, size_t* size)
{
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    int fd = ::_topen (filename, O_RDONLY | O_BINARY, _S_IREAD);
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    try {
        if (fd < 0) {
            AfxThrowFileException (CFileException::fileNotFound, -1, filename);
        }
        long eof = ::_lseek (fd, 0, SEEK_END);
        if (eof < 0) {
            AfxThrowFileException (CFileException::badSeek, -1, filename);
        }
        buffer->GrowToSize (eof);
        if (size != NULL) {
            *size = eof;
        }
        (void)::_lseek (fd, 0, SEEK_SET);
        int nBytes = ::_read (fd, *buffer, eof);
        if (nBytes != eof) {
            AfxThrowFileException (CFileException::genericException, -1, filename);
        }
        (void)::_close (fd);
    }
    catch (...) {
        if (fd >= 0) {
            (void)::_close (fd);
        }
        throw;
    }
}

void ActiveLedItControl::WriteBytesToFile (LPCTSTR filename, const void* buffer, size_t size)
{
    (void)::_tunlink (filename);
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    int fd = ::_topen (filename, O_CREAT | O_RDWR | O_TRUNC | O_BINARY, _S_IREAD | _S_IWRITE);
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    try {
        if (fd < 0) {
            AfxThrowFileException (CFileException::fileNotFound, -1, filename);
        }
        int nBytes = ::_write (fd, buffer, size);
        if (nBytes != static_cast<int> (size)) {
            AfxThrowFileException (CFileException::genericException, -1, filename);
        }
        (void)::_close (fd);
    }
    catch (...) {
        if (fd >= 0) {
            (void)::_close (fd);
        }
        throw;
    }
}

void ActiveLedItControl::LoadFile (LPCTSTR filename)
{
    fCommandHandler.Commit ();
    fEditor.Replace (0, fEditor.GetEnd (), LED_TCHAR_OF (""), 0);

    Memory::SmallStackBuffer<char> buffer (0);
    size_t                         size = 0;
    DoReadFile (filename, &buffer, &size);

    StyledTextIOSrcStream_Memory                 source (buffer, size);
    WordProcessor::WordProcessorTextIOSinkStream sink (&fEditor);

    Led_FileFormat format = GuessFormatFromName (filename);

ReRead:
    switch (format) {
        case eTextFormat: {
            StyledTextIOReader_PlainText textReader (&source, &sink);
            textReader.Read ();
        } break;

        case eLedPrivateFormat: {
            StyledTextIOReader_LedNativeFileFormat textReader (&source, &sink);
            textReader.Read ();
        } break;

        case eRTFFormat: {
            StyledTextIOReader_RTF textReader (&source, &sink);
            textReader.Read ();
        } break;

        case eHTMLFormat: {
            StyledTextIOReader_HTML textReader (&source, &sink, &fHTMLInfo);
            textReader.Read ();
        } break;

        case eUnknownFormat: {
            /*
                     *  Should enhance this unknown/format reading code to take into account file suffix in our guess.
                     */

            // Try RTF
            try {
                StyledTextIOReader_RTF reader (&source, &sink);
                if (reader.QuickLookAppearsToBeRightFormat ()) {
                    format = eRTFFormat;
                    goto ReRead;
                }
            }
            catch (...) {
                // ignore any errors, and proceed to next file type
            }

            // Try LedNativeFileFormat
            try {
                StyledTextIOReader_LedNativeFileFormat reader (&source, &sink);
                if (reader.QuickLookAppearsToBeRightFormat ()) {
                    format = eLedPrivateFormat;
                    goto ReRead;
                }
            }
            catch (...) {
                // ignore any errors, and proceed to next file type
            }

            // Try HTML
            try {
                StyledTextIOReader_HTML reader (&source, &sink);
                if (reader.QuickLookAppearsToBeRightFormat ()) {
                    format = eHTMLFormat;
                    goto ReRead;
                }
            }
            catch (...) {
                // ignore any errors, and proceed to next file type
            }

            // Nothing left todo but to read the text file as plain text, as best we can...
            format = eTextFormat;
            goto ReRead;
        } break;

        default: {
            Assert (false); // don't support reading that format (yet?)!
        } break;
    }
    fEditor.SetEmptySelectionStyle ();
}

void ActiveLedItControl::SaveFile (LPCTSTR filename)
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    WordProcessor::WordProcessorTextIOSrcStream source (&fEditor);
    StyledTextIOWriterSinkStream_Memory         sink;
    switch (GuessFormatFromName (filename)) {
        case eTextFormat: {
            StyledTextIOWriter_PlainText textWriter (&source, &sink);
            textWriter.Write ();
        } break;

        case eRTFFormat: {
            StyledTextIOWriter_RTF textWriter (&source, &sink);
            textWriter.Write ();
        } break;

        case eHTMLFormat: {
            StyledTextIOWriter_HTML textWriter (&source, &sink, &fHTMLInfo);
            textWriter.Write ();
        } break;

        case eLedPrivateFormat: {
            StyledTextIOWriter_LedNativeFileFormat textWriter (&source, &sink);
            textWriter.Write ();
        } break;

        default: {
            StyledTextIOWriter_RTF textWriter (&source, &sink);
            textWriter.Write ();
        } break;
    }
    WriteBytesToFile (filename, sink.PeekAtData (), sink.GetLength ());
}

void ActiveLedItControl::SaveFileCRLF (LPCTSTR filename)
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    WordProcessor::WordProcessorTextIOSrcStream source (&fEditor);
    StyledTextIOWriterSinkStream_Memory         sink;
    StyledTextIOWriter_PlainText                textWriter (&source, &sink);
    textWriter.Write ();
    WriteBytesToFile (filename, sink.PeekAtData (), sink.GetLength ());
}

void ActiveLedItControl::SaveFileRTF (LPCTSTR filename)
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    WordProcessor::WordProcessorTextIOSrcStream source (&fEditor);
    StyledTextIOWriterSinkStream_Memory         sink;
    StyledTextIOWriter_RTF                      textWriter (&source, &sink);
    textWriter.Write ();
    WriteBytesToFile (filename, sink.PeekAtData (), sink.GetLength ());
}

void ActiveLedItControl::SaveFileHTML (LPCTSTR filename)
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    WordProcessor::WordProcessorTextIOSrcStream source (&fEditor);
    StyledTextIOWriterSinkStream_Memory         sink;
    StyledTextIOWriter_HTML                     textWriter (&source, &sink);
    textWriter.Write ();
    WriteBytesToFile (filename, sink.PeekAtData (), sink.GetLength ());
}

void ActiveLedItControl::Refresh ()
{
    fEditor.Refresh ();
}

void ActiveLedItControl::ScrollToSelection ()
{
    fEditor.ScrollToSelection ();
}

long ActiveLedItControl::OLE_GetMaxUndoLevel ()
{
    return fCommandHandler.GetMaxUnDoLevels ();
}

void ActiveLedItControl::OLE_SetMaxUndoLevel (long maxUndoLevel)
{
    // sanity check arguments
    maxUndoLevel = max (0L, maxUndoLevel);
    maxUndoLevel = min (maxUndoLevel, 10L);
    fCommandHandler.SetMaxUnDoLevels (maxUndoLevel);
}

BOOL ActiveLedItControl::OLE_GetCanUndo ()
{
    return fCommandHandler.CanUndo () or (fCommandHandler.GetMaxUnDoLevels () == 1 and fCommandHandler.CanRedo ());
}

BOOL ActiveLedItControl::OLE_GetCanRedo ()
{
    return fCommandHandler.CanRedo ();
}

void ActiveLedItControl::OLE_Undo ()
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    if (fCommandHandler.CanUndo ()) {
        fCommandHandler.DoUndo (fEditor);
    }
    else if (fCommandHandler.GetMaxUnDoLevels () == 1 and fCommandHandler.CanRedo ()) {
        fCommandHandler.DoRedo (fEditor);
    }
    else {
        // ignore bad undo request - no ability to return errors here til we redo this in ATL
    }
}

void ActiveLedItControl::OLE_Redo ()
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    if (fCommandHandler.CanRedo ()) {
        fCommandHandler.DoRedo (fEditor);
    }
    else {
        // ignore bad undo request - no ability to return errors here til we redo this in ATL
    }
}

void ActiveLedItControl::OLE_CommitUndo ()
{
    fCommandHandler.Commit ();
}

void ActiveLedItControl::OLE_LaunchFontSettingsDialog ()
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    fEditor.OnChooseFontCommand ();
}

void ActiveLedItControl::OLE_LaunchParagraphSettingsDialog ()
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    fEditor.OnParagraphSpacingChangeCommand ();
}

void ActiveLedItControl::OLE_LaunchFindDialog ()
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    fEditor.OnFindCommand ();
}

long ActiveLedItControl::OLE_Find (long searchFrom, const VARIANT& findText, BOOL wrapSearch, BOOL wholeWordSearch, BOOL caseSensativeSearch)
{
    // don't have any better error checking technology here - should return E_INVALIDARG!!!
    if (searchFrom < 0) {
        return -1;
    }
    if (static_cast<size_t> (searchFrom) > fTextStore.GetEnd ()) {
        return -1;
    }
    if (findText.vt != VT_BSTR) {
        return -1;
    }

    TextStore::SearchParameters parameters;

#if qWideCharacters
    parameters.fMatchString = findText.bstrVal;
#else
    USES_CONVERSION;
    parameters.fMatchString = OLE2A (findText.bstrVal);
#endif
    parameters.fWrapSearch          = !!wrapSearch;
    parameters.fWholeWordSearch     = !!wholeWordSearch;
    parameters.fCaseSensativeSearch = !!caseSensativeSearch;

    size_t whereTo = fTextStore.Find (parameters, searchFrom);

    return whereTo;
}

void ActiveLedItControl::OLE_LaunchReplaceDialog ()
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    fEditor.OnReplaceCommand ();
}

long ActiveLedItControl::OLE_FindReplace (long searchFrom, const VARIANT& findText, const VARIANT& replaceText, BOOL wrapSearch, BOOL wholeWordSearch, BOOL caseSensativeSearch)
{
    // don't have any better error checking technology here - should return E_INVALIDARG!!!
    if (searchFrom < 0) {
        return -1;
    }
    if (static_cast<size_t> (searchFrom) > fTextStore.GetEnd ()) {
        return -1;
    }
    if (findText.vt != VT_BSTR) {
        return -1;
    }
    if (replaceText.vt != VT_BSTR) {
        return -1;
    }

    TextStore::SearchParameters parameters;

#if qWideCharacters
    Led_tString findStr    = findText.bstrVal;
    Led_tString replaceStr = replaceText.bstrVal;
#else
    USES_CONVERSION;
    Led_tString findStr    = OLE2A (findText.bstrVal);
    Led_tString replaceStr = OLE2A (replaceText.bstrVal);
#endif
    return fEditor.OLE_FindReplace (searchFrom, findStr, replaceStr, wrapSearch, wholeWordSearch, caseSensativeSearch);
}

void ActiveLedItControl::OLE_PrinterSetupDialog ()
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    fEditor.OnFilePrintSetup ();
}

void ActiveLedItControl::OLE_PrintDialog ()
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    fEditor.OnFilePrint ();
}

void ActiveLedItControl::OLE_PrintOnce ()
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    fEditor.OnFilePrintOnce ();
}

long ActiveLedItControl::OLE_GetHeight (long from, long to)
{
    using RowReference = MultiRowTextImager::RowReference;
    if (from < 0) {
        from = 0;
    }
    if (to < 0) {
        to = fTextStore.GetEnd ();
    }
    if (from > to) {
        // throw invalid input
        return -1;
    }
    if (static_cast<size_t> (to) > fTextStore.GetEnd ()) {
        // throw invalid input
        return -1;
    }
    RowReference startingRow = fEditor.GetRowReferenceContainingPosition (from);
    RowReference endingRow   = fEditor.GetRowReferenceContainingPosition (to);
    /*
         *  Always take one more row than they asked for, since they will expect if you start and end on a given row - you'll get
         *  the height of that row.
         */
    return fEditor.GetHeightOfRows (startingRow, fEditor.CountRowDifference (startingRow, endingRow) + 1);
}

BOOL ActiveLedItControl::OLE_GetDirty ()
{
    return fDataDirty;
}

void ActiveLedItControl::OLE_SetDirty (BOOL dirty)
{
    fDataDirty = !!dirty;
}

void ActiveLedItControl::OnBrowseHelpCommand ()
{
    Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/ActiveLedIt/UserDocs.asp", kAppName, kURLDemoFlag));
}

void ActiveLedItControl::OnAboutBoxCommand ()
{
    class MyAboutBox : public Led_StdDialogHelper_AboutBox {
    private:
        using inherited = Led_StdDialogHelper_AboutBox;

    public:
        MyAboutBox (HINSTANCE hInstance, HWND parentWnd)
            : inherited (hInstance, parentWnd)
        {
        }

    public:
        virtual BOOL OnInitDialog () override
        {
            BOOL result = inherited::OnInitDialog ();

            // Cuz of fact that dlog sizes specified in dlog units, and that doesn't work well for bitmaps
            // we must resize our dlog on the fly based on pict resource size...
            const int kPictWidth = 437; // must agree with ACTUAL bitmap size
            const int kButHSluff = 17;
#if qDemoMode
            const int kButVSluff  = 136;
            const int kPictHeight = 393;
#else
            const int kButVSluff  = 61;
            const int kPictHeight = 318;
#endif
            {
                RECT windowRect;
                ::GetWindowRect (GetHWND (), &windowRect);
                // figure size of non-client area...
                int ncWidth  = 0;
                int ncHeight = 0;
                {
                    RECT clientRect;
                    ::GetClientRect (GetHWND (), &clientRect);
                    ncWidth  = AsLedRect (windowRect).GetWidth () - AsLedRect (clientRect).GetWidth ();
                    ncHeight = AsLedRect (windowRect).GetHeight () - AsLedRect (clientRect).GetHeight ();
                }
                ::MoveWindow (GetHWND (), windowRect.left, windowRect.top, kPictWidth + ncWidth, kPictHeight + ncHeight, false);
            }

            // Place and fill in version information
            {
                HWND w = ::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_VersionFieldID);
                AssertNotNull (w);
                const int kVERWidth = 230;
#if qDemoMode
                ::MoveWindow (w, kPictWidth / 2 - kVERWidth / 2, 35, 230, 2 * 14, false);
#else
                ::MoveWindow (w, kPictWidth / 2 - kVERWidth / 2, 35, 230, 14, false);
#endif
#if _UNICODE
#define kUNICODE_NAME_ADORNER L" [UNICODE]"
#elif qWideCharacters
#define kUNICODE_NAME_ADORNER " [Internal UNICODE]"
#else
#define kUNICODE_NAME_ADORNER
#endif
#if qDemoMode
                TCHAR nDaysString[1024];
                (void)::_stprintf (nDaysString, _T("%d"), DemoPrefs ().GetDemoDaysLeft ());
#endif
                ::SetWindowText (w,
                                 (
                                     Led_SDK_String (_T (qLed_ShortVersionString) kUNICODE_NAME_ADORNER _T (" (") _T (__DATE__) _T (")"))
#if qDemoMode
                                     + Led_SDK_String (_T("\r\nDemo expires in ")) + nDaysString + _T(" days.")
#endif
                                     )
                                     .c_str ());
            }

            // Place hidden buttons which map to URLs
            {
                HWND w = ::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_InfoLedFieldID);
                AssertNotNull (w);
                ::MoveWindow (w, 15, 159, 142, 17, false);
                w = ::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_LedWebPageFieldID);
                AssertNotNull (w);
                ::MoveWindow (w, 227, 159, 179, 17, false);
            }

            // Place OK button
            {
                HWND w = ::GetDlgItem (GetHWND (), IDOK);
                AssertNotNull (w);
                RECT tmp;
                ::GetWindowRect (w, &tmp);
                ::MoveWindow (w, kButHSluff, kPictHeight - AsLedRect (tmp).GetHeight () - kButVSluff, AsLedRect (tmp).GetWidth (), AsLedRect (tmp).GetHeight (), false); // width/height we should presevere
            }

            ::SetWindowText (GetHWND (), _T ("About ActiveLedIt!"));

            return (result);
        }

    public:
        virtual void OnClickInInfoField () override
        {
            try {
                Led_URLManager::Get ().Open ("mailto:info-led@sophists.com");
            }
            catch (...) {
                // ignore for now - since errors here prent dialog from dismissing (on MacOSX)
            }
            inherited::OnClickInInfoField ();
        }

        virtual void OnClickInLedWebPageField () override
        {
            try {
                Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/ActiveLedIt/", kAppName, kURLDemoFlag));
            }
            catch (...) {
                // ignore for now - since errors here prent dialog from dismissing (on MacOSX)
            }
            inherited::OnClickInLedWebPageField ();
        }
    };
    MyAboutBox dlg (::AfxGetResourceHandle (), fEditor.GetHWND ());
    dlg.DoModal ();
}

void ActiveLedItControl::ForceUIActive ()
{
    OnActivateInPlace (TRUE, NULL); // == UI-Activate the control
}

void ActiveLedItControl::FireOLEEvent (DISPID eventID)
{
    FireOLEEvent (eventID, EVENT_PARAM (VTS_NONE));
}

void ActiveLedItControl::FireOLEEvent (DISPID dispid, BYTE* pbParams, ...)
{
    // Clone of COleControl::FireEvent, but cannot call directly cuz of the va_list stuff.
    va_list argList;
    va_start (argList, pbParams);
    FireEventV (dispid, pbParams, argList);
    va_end (argList);
}

void ActiveLedItControl::FireUpdateUserCommand (const wstring& internalCmdName, VARIANT_BOOL* enabled, VARIANT_BOOL* checked, wstring* name)
{
    try {
        CComObject<ActiveLedIt_CurrentEventArguments>* o = NULL;
        Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_CurrentEventArguments>::CreateInstance (&o));
        o->fInternalName       = internalCmdName;
        o->fName               = *name;
        o->fEnabled            = !!*enabled;
        o->fChecked            = !!*checked;
        fCurrentEventArguments = o;
        FireOLEEvent (DISPID_UpdateUserCommand);
        *name    = o->fName;
        *enabled = o->fEnabled;
        *checked = o->fChecked;
    }
    catch (...) {
        fCurrentEventArguments.Release ();
        throw;
    }
    fCurrentEventArguments.Release ();
}

void ActiveLedItControl::FireUserCommand (const wstring& internalCmdName)
{
    try {
        CComObject<ActiveLedIt_CurrentEventArguments>* o = NULL;
        Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_CurrentEventArguments>::CreateInstance (&o));
        o->fInternalName       = internalCmdName;
        fCurrentEventArguments = o;
        FireOLEEvent (DISPID_UserCommand);
    }
    catch (...) {
        fCurrentEventArguments.Release ();
        throw;
    }
    fCurrentEventArguments.Release ();
}

#if qFunnyDisplayInDesignMode
bool ActiveLedItControl::IsInDesignMode () const
{
    return not const_cast<ActiveLedItControl*> (this)->AmbientUserMode ();
}

bool ActiveLedItControl::DrawExtraDesignModeBorder () const
{
    return const_cast<ActiveLedItControl*> (this)->GetAppearance () == 0 and
           const_cast<ActiveLedItControl*> (this)->GetBorderStyle () == 0;
}
#endif

HMENU ActiveLedItControl::GenerateContextMenu ()
{
    if (fConextMenu != NULL) {
        CComQIPtr<IALCommandList> cm = fConextMenu;
        if (cm != NULL) {
            HMENU menu = NULL;
            if (SUCCEEDED (cm->GeneratePopupMenu (fAcceleratorTable, &menu))) {
                return menu;
            }
        }
    }
    return NULL;
}

int ActiveLedItControl::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
    if (COleControl::OnCreate (lpCreateStruct) == -1) {
        return -1;
    }

    fEditor.SetDefaultTextColor (TextImager::eDefaultBackgroundColor, Led_Color (TranslateColor (GetBackColor ())));

    CRect clientRect;
    GetClientRect (&clientRect);
    if (fEditor.Create (WS_CHILD | WS_VISIBLE, clientRect, this, kEditorWindowID) == 0) {
        return -1;
    }

    CComQIPtr<IALToolbarList> tbl = fToolbarList;
    if (tbl.p != NULL) {
        tbl->NotifyOfOwningActiveLedIt (CComQIPtr<IDispatch> (GetControllingUnknown ()), m_hWnd);
    }

    if (fOnCreateExtras.get () != NULL) {
        OLE_SetReadOnly (fOnCreateExtras.get ()->fReadOnly);
        OLE_SetEnabled (fOnCreateExtras.get ()->fEnabled);
        fOnCreateExtras.release ();
    }

    return 0;
}

void ActiveLedItControl::OnWindowPosChanged (WINDOWPOS* lpwndpos)
{
    RequireNotNull (lpwndpos);
    DbgTrace (Led_SDK_TCHAROF ("ActiveLedItControl::OnWindowPosChanged (cx=%d, cy=%d, flags=0x%x)\n"), lpwndpos->cx, lpwndpos->cy, lpwndpos->flags);

    IdleManager::NonIdleContext nonIdleContext;

    /*
         *  Used to adjust bounds of fEditor in the OnSize () method. Not entire sure why that wasn't good
         *  enough. But - doing it here, and redundantly like below - seems to make the display glitch in SPR#1168
         *  go away.    -- LGP 2003-05-06
         */
    COleControl::OnWindowPosChanged (lpwndpos);

    Layout ();
}

void ActiveLedItControl::OnSetFocus (CWnd* pOldWnd)
{
    IdleManager::NonIdleContext nonIdleContext;
    COleControl::OnSetFocus (pOldWnd);
    fEditor.SetFocus ();
}

void ActiveLedItControl::Layout ()
{
    IdleManager::NonIdleContext nonIdleContext;
    if (m_hWnd != NULL) {
        Led_Distance toolbarHeightUsed = 0;

        CRect cr;
        GetClientRect (&cr);

        try {
            CComQIPtr<IALToolbarList> tbl = fToolbarList;
            if (tbl.p != NULL) {
                UINT preferredHeight = 0;
                Led_ThrowIfErrorHRESULT (tbl->get_PreferredHeight (&preferredHeight));
                toolbarHeightUsed = preferredHeight;
                CRect tblRect     = cr;
                tblRect.bottom    = tblRect.top + preferredHeight;
                Led_ThrowIfErrorHRESULT (tbl->SetRectangle (tblRect.left, tblRect.top, tblRect.Width (), tblRect.Height ()));
            }
        }
        catch (...) {
        }

        CRect editRect = cr;
        editRect.top += toolbarHeightUsed;
        fEditor.MoveWindow (editRect);
    }
}

void ActiveLedItControl::OLE_InvalidateLayout ()
{
    Layout ();
}

#if qDontUIActivateOnOpen
HRESULT ActiveLedItControl::OnOpen (BOOL /*bTryInPlace*/, LPMSG pMsg)
{
    return OnActivateInPlace (false, pMsg);
}
#endif

void ActiveLedItControl::OnBackColorChanged ()
{
    COleControl::OnBackColorChanged ();

    Led_Color c = Led_Color (TranslateColor (GetBackColor ()));
    if (fEditor.GetDefaultTextColor (TextImager::eDefaultBackgroundColor) == NULL or
        *fEditor.GetDefaultTextColor (TextImager::eDefaultBackgroundColor) != c) {
        fEditor.SetDefaultTextColor (TextImager::eDefaultBackgroundColor, c);
        fEditor.Refresh ();
    }
}

#if qFunnyDisplayInDesignMode
void ActiveLedItControl::OnAmbientPropertyChange (DISPID dispid)
{
    COleControl::OnAmbientPropertyChange (dispid);
    if (dispid == DISPID_AMBIENT_USERMODE) {
        InvalidateControl ();
        ::InvalidateRect (fEditor.GetHWND (), NULL, true); // so we get the border too
    }
}
#endif

UINT ActiveLedItControl::OLE_VersionNumber ()
{
    return kActiveLedItDWORDVersion;
}

BSTR ActiveLedItControl::OLE_ShortVersionString ()
{
    try {
        string result = qLed_ShortVersionString + string (kDemoString);
        return CString (result.c_str ()).AllocSysString ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false); /*NOTREACHED*/
    return NULL;
}

BOOL ActiveLedItControl::OLE_GetReadOnly ()
{
    if (fEditor.m_hWnd == NULL) {
        // Avoid barfing if no window...
        if (fOnCreateExtras.get () == NULL) {
            return false;
        }
        else {
            return fOnCreateExtras.get ()->fReadOnly;
        }
    }
    try {
        bool result = !!(fEditor.GetStyle () & ES_READONLY);
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    return false;
}

void ActiveLedItControl::OLE_SetReadOnly (BOOL bNewValue)
{
    try {
        if (fEditor.m_hWnd == NULL) {
            if (fOnCreateExtras.get () == NULL) {
                fOnCreateExtras = unique_ptr<OnCreateExtras> (new OnCreateExtras ());
            }
            fOnCreateExtras.get ()->fReadOnly = !!bNewValue;
        }
        else {
            fEditor.SetReadOnly (bNewValue);
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

BOOL ActiveLedItControl::OLE_GetEnabled ()
{
    if (fEditor.m_hWnd == NULL) {
        // Avoid barfing if no window...
        if (fOnCreateExtras.get () == NULL) {
            return true;
        }
        else {
            return fOnCreateExtras.get ()->fEnabled;
        }
    }
    try {
        return not(fEditor.GetStyle () & WS_DISABLED);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    return false;
}

void ActiveLedItControl::OLE_SetEnabled (BOOL bNewValue)
{
    try {
        if (fEditor.m_hWnd == NULL) {
            if (fOnCreateExtras.get () == NULL) {
                fOnCreateExtras = unique_ptr<OnCreateExtras> (new OnCreateExtras ());
            }
            fOnCreateExtras.get ()->fEnabled = !!bNewValue;
        }
        else {
            if (bNewValue) {
                fEditor.ModifyStyle (WS_DISABLED, 0);
            }
            else {
                fEditor.ModifyStyle (0, WS_DISABLED);
            }
            fEditor.Refresh ();
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

BOOL ActiveLedItControl::OLE_GetEnableAutoChangesBackgroundColor ()
{
    return fEditor.fEnableAutoChangesBackgroundColor;
}

void ActiveLedItControl::OLE_SetEnableAutoChangesBackgroundColor (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        IdleManager::NonIdleContext nonIdleContext;
        fEditor.fEnableAutoChangesBackgroundColor = !!bNewValue;
        fEditor.Refresh ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

int ActiveLedItControl::OLE_GetWindowMarginTop ()
{
    return fEditor.GetDefaultWindowMargins ().GetTop ();
}

void ActiveLedItControl::OLE_SetWindowMarginTop (int windowMarginTop)
{
    try {
        Led_TWIPS_Rect margRect = fEditor.GetDefaultWindowMargins ();
        margRect.top            = Led_TWIPS (windowMarginTop);
        fEditor.SetDefaultWindowMargins (margRect);
        Invalidate ();
        fEditor.Invalidate ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

int ActiveLedItControl::OLE_GetWindowMarginLeft ()
{
    return fEditor.GetDefaultWindowMargins ().GetLeft ();
}

void ActiveLedItControl::OLE_SetWindowMarginLeft (int windowMarginLeft)
{
    try {
        Led_TWIPS_Rect margRect = fEditor.GetDefaultWindowMargins ();
        margRect.left           = Led_TWIPS (windowMarginLeft);
        fEditor.SetDefaultWindowMargins (margRect);
        Invalidate ();
        fEditor.Invalidate ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

int ActiveLedItControl::OLE_GetWindowMarginBottom ()
{
    return fEditor.GetDefaultWindowMargins ().GetBottom ();
}

void ActiveLedItControl::OLE_SetWindowMarginBottom (int windowMarginBottom)
{
    try {
        Led_TWIPS_Rect margRect = fEditor.GetDefaultWindowMargins ();
        margRect.SetBottom (Led_TWIPS (windowMarginBottom));
        fEditor.SetDefaultWindowMargins (margRect);
        Invalidate ();
        fEditor.Invalidate ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

int ActiveLedItControl::OLE_GetWindowMarginRight ()
{
    return fEditor.GetDefaultWindowMargins ().GetRight ();
}

void ActiveLedItControl::OLE_SetWindowMarginRight (int windowMarginRight)
{
    try {
        Led_TWIPS_Rect margRect = fEditor.GetDefaultWindowMargins ();
        margRect.SetRight (Led_TWIPS (windowMarginRight));
        fEditor.SetDefaultWindowMargins (margRect);
        Invalidate ();
        fEditor.Invalidate ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

int ActiveLedItControl::OLE_GetPrintMarginTop ()
{
    return fEditor.GetPrintMargins ().GetTop ();
}

void ActiveLedItControl::OLE_SetPrintMarginTop (int printMarginTop)
{
    try {
        Led_TWIPS_Rect margRect = fEditor.GetPrintMargins ();
        margRect.SetTop (Led_TWIPS (printMarginTop));
        fEditor.SetPrintMargins (margRect);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

int ActiveLedItControl::OLE_GetPrintMarginLeft ()
{
    return fEditor.GetPrintMargins ().GetLeft ();
}

void ActiveLedItControl::OLE_SetPrintMarginLeft (int printMarginLeft)
{
    try {
        Led_TWIPS_Rect margRect = fEditor.GetPrintMargins ();
        margRect.SetLeft (Led_TWIPS (printMarginLeft));
        fEditor.SetPrintMargins (margRect);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

int ActiveLedItControl::OLE_GetPrintMarginBottom ()
{
    return fEditor.GetPrintMargins ().GetBottom ();
}

void ActiveLedItControl::OLE_SetPrintMarginBottom (int printMarginBottom)
{
    try {
        Led_TWIPS_Rect margRect = fEditor.GetPrintMargins ();
        margRect.SetBottom (Led_TWIPS (printMarginBottom));
        fEditor.SetPrintMargins (margRect);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

int ActiveLedItControl::OLE_GetPrintMarginRight ()
{
    return fEditor.GetPrintMargins ().GetRight ();
}

void ActiveLedItControl::OLE_SetPrintMarginRight (int printMarginRight)
{
    try {
        Led_TWIPS_Rect margRect = fEditor.GetPrintMargins ();
        margRect.SetRight (Led_TWIPS (printMarginRight));
        fEditor.SetPrintMargins (margRect);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

UINT ActiveLedItControl::GetHasVerticalScrollBar ()
{
    switch (fEditor.GetScrollBarType (LedItView::v)) {
        case LedItView::eScrollBarNever:
            return eNoScrollBar;
        case LedItView::eScrollBarAsNeeded:
            return eShowScrollbarIfNeeded;
        case LedItView::eScrollBarAlways:
            return eShowScrollBar;
    }
    return eNoScrollBar;
}

void ActiveLedItControl::SetHasVerticalScrollBar (UINT bNewValue)
{
    if (bNewValue != GetHasVerticalScrollBar ()) {
        IdleManager::NonIdleContext nonIdleContext;
        SetModifiedFlag ();
        switch (bNewValue) {
            case eNoScrollBar:
                fEditor.SetScrollBarType (LedItView::v, LedItView::eScrollBarNever);
                break;
            case eShowScrollBar:
                fEditor.SetScrollBarType (LedItView::v, LedItView::eScrollBarAlways);
                break;
            case eShowScrollbarIfNeeded:
                fEditor.SetScrollBarType (LedItView::v, LedItView::eScrollBarAsNeeded);
                break;
        }
    }
}

UINT ActiveLedItControl::GetHasHorizontalScrollBar ()
{
    switch (fEditor.GetScrollBarType (LedItView::h)) {
        case LedItView::eScrollBarNever:
            return eNoScrollBar;
        case LedItView::eScrollBarAsNeeded:
            return eShowScrollbarIfNeeded;
        case LedItView::eScrollBarAlways:
            return eShowScrollBar;
    }
    return eNoScrollBar;
}

void ActiveLedItControl::SetHasHorizontalScrollBar (UINT bNewValue)
{
    if (bNewValue != GetHasHorizontalScrollBar ()) {
        IdleManager::NonIdleContext nonIdleContext;
        SetModifiedFlag ();
        switch (bNewValue) {
            case eNoScrollBar:
                fEditor.SetScrollBarType (LedItView::h, LedItView::eScrollBarNever);
                break;
            case eShowScrollBar:
                fEditor.SetScrollBarType (LedItView::h, LedItView::eScrollBarAlways);
                break;
            case eShowScrollbarIfNeeded:
                fEditor.SetScrollBarType (LedItView::h, LedItView::eScrollBarAsNeeded);
                break;
        }
    }
}

BSTR ActiveLedItControl::GetBufferText ()
{
    size_t                              len = fEditor.GetLength ();
    Memory::SmallStackBuffer<Led_tChar> buf (len + 1);
    fEditor.CopyOut (0, len, buf);
    buf[len] = '\0';
    return CString (buf).AllocSysString ();
}

void ActiveLedItControl::SetBufferText (LPCTSTR text)
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    try {
        IdleManager::NonIdleContext nonIdleContext;
        fCommandHandler.Commit ();
#if _UNICODE
        size_t                            len = text == NULL ? 0 : ::_tcslen (text);
        Memory::SmallStackBuffer<wchar_t> buf (len + 1);
        buf[0] = 0xfeff;
        memcpy (&buf[1], text, len * sizeof (wchar_t));
        StyledTextIOSrcStream_Memory source (buf, (len + 1) * sizeof (wchar_t));
#else
        StyledTextIOSrcStream_Memory source (text, text == NULL ? 0 : ::_tcslen (text));
#endif
        WordProcessor::WordProcessorTextIOSinkStream sink (&fEditor);
        StyledTextIOReader_PlainText                 textReader (&source, &sink);
        fEditor.Replace (0, fEditor.GetEnd (), LED_TCHAR_OF (""), 0); // clear out current text
        textReader.Read ();
        fEditor.SetEmptySelectionStyle ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

BSTR ActiveLedItControl::GetBufferTextCRLF ()
{
    try {
        size_t                              len = fEditor.GetLength ();
        Memory::SmallStackBuffer<Led_tChar> buf (len + 1);
        fEditor.CopyOut (0, len, buf);
        buf[len] = '\0';
        Memory::SmallStackBuffer<Led_tChar> buf2 (2 * len + 1);
        len       = Characters::NLToNative<Led_tChar> (buf, len, buf2, 2 * len + 1);
        buf2[len] = '\0';
        return CString (buf2).AllocSysString ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false); /*NOTREACHED*/
    return NULL;
}

void ActiveLedItControl::SetBufferTextCRLF (LPCTSTR text)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    SetBufferText (text);
}

BSTR ActiveLedItControl::GetBufferTextAsRTF ()
{
    try {
        return CString (GetBufferTextAsRTF_ ().c_str ()).AllocSysString ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false); /*NOTREACHED*/
    return NULL;
}

string ActiveLedItControl::GetBufferTextAsRTF_ ()
{
    WordProcessor::WordProcessorTextIOSrcStream source (&fEditor);
    StyledTextIOWriterSinkStream_Memory         sink;
    StyledTextIOWriter_RTF                      textWriter (&source, &sink);
    textWriter.Write ();
    size_t                         len = sink.GetLength ();
    Memory::SmallStackBuffer<char> buf (len + 1);
    memcpy (buf, sink.PeekAtData (), len);
    buf[len] = '\0';
    return string (static_cast<char*> (buf));
}

void ActiveLedItControl::SetBufferTextAsRTF (LPCTSTR text)
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    try {
        IdleManager::NonIdleContext              nonIdleContext;
        TextInteractor::TemporarilySetUpdateMode tsum (fEditor, m_hWnd == NULL ? TextInteractor::eNoUpdate : TextInteractor::eDefaultUpdate);
        fCommandHandler.Commit ();

        string                                       s = Led_SDKString2ANSI (text);
        StyledTextIOSrcStream_Memory                 source (s.c_str (), s.length ());
        WordProcessor::WordProcessorTextIOSinkStream sink (&fEditor);
        StyledTextIOReader_RTF                       textReader (&source, &sink);
        fEditor.Replace (0, fEditor.GetEnd (), LED_TCHAR_OF (""), 0); // clear out current text
        textReader.Read ();
        fEditor.SetEmptySelectionStyle ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

BSTR ActiveLedItControl::GetBufferTextAsHTML ()
{
    try {
        WordProcessor::WordProcessorTextIOSrcStream source (&fEditor);
        StyledTextIOWriterSinkStream_Memory         sink;
        StyledTextIOWriter_HTML                     textWriter (&source, &sink);
        textWriter.Write ();
        size_t                         len = sink.GetLength ();
        Memory::SmallStackBuffer<char> buf (len + 1);
        memcpy (buf, sink.PeekAtData (), len);
        buf[len] = '\0';
        return CString (buf).AllocSysString ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false); /*NOTREACHED*/
    return NULL;
}

void ActiveLedItControl::SetBufferTextAsHTML (LPCTSTR text)
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    try {
        IdleManager::NonIdleContext nonIdleContext;
        fCommandHandler.Commit ();
        string                                       s = Led_SDKString2ANSI (text);
        StyledTextIOSrcStream_Memory                 source (s.c_str (), s.length ());
        WordProcessor::WordProcessorTextIOSinkStream sink (&fEditor);
        StyledTextIOReader_HTML                      textReader (&source, &sink);
        fEditor.Replace (0, fEditor.GetEnd (), LED_TCHAR_OF (""), 0); // clear out current text
        textReader.Read ();
        fEditor.SetEmptySelectionStyle ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

inline void PackBytesIntoVariantSAFEARRAY (const void* bytes, size_t nBytes, VARIANT* into)
{
    // Note: we must use SafeArrayCreate/SafeArrayAllocData instead of SafeArrayCreateVector
    // due to bugs with the OLE32.DLL marshalling code (apparantly manually deletes both pointers and
    // doesnt call SafeArrayDelete...
    SAFEARRAYBOUND rgsaBounds[1];
    rgsaBounds[0].lLbound   = 0;
    rgsaBounds[0].cElements = nBytes;
    SAFEARRAY* ar           = ::SafeArrayCreate (VT_I1, 1, rgsaBounds);
    Led_ThrowIfNull (ar);
    Led_ThrowIfErrorHRESULT (::SafeArrayAllocData (ar));
    void* ptr = NULL;
    Led_ThrowIfErrorHRESULT (::SafeArrayAccessData (ar, &ptr));
    (void)::memcpy (ptr, bytes, nBytes);
    ::SafeArrayUnaccessData (ar);
    ::VariantInit (into);
    into->vt     = VT_ARRAY | VT_I1;
    into->parray = ar;
}

VARIANT ActiveLedItControl::GetBufferTextAsDIB ()
{
    if (fEditor.m_hWnd == NULL) {
        VARIANT v;
        ::VariantInit (&v);
        return v;
    }

    CWindowDC             windowDC (&fEditor);
    Led_MFC_TabletFromCDC tabFromDC (&windowDC);
    Led_Tablet            tablet = tabFromDC;

    Led_Distance rhsMargin = 0;
    {
        const int kRTF_SPEC_DefaultInches = 6; // HACK - see comments in SinkStreamDestination::SetRightMargin ()
        Led_TWIPS rhsTWIPS                = Led_TWIPS (kRTF_SPEC_DefaultInches * 1440);
        rhsMargin                         = Led_CvtScreenPixelsFromTWIPSH (rhsTWIPS);
    }

    size_t   savedScrollPos  = fEditor.GetMarkerPositionOfStartOfWindow ();
    Led_Rect savedWindowRect = fEditor.GetWindowRect ();
    Led_Rect offscreenRect (0, 0, 1000, rhsMargin); // height doesn't matter as long as more than one row - adjused below...

    Led_Bitmap memoryBitmap;
    try {
        {
        Again:
            unsigned nTimes = 0;
            fEditor.SetWindowRect (offscreenRect);
            offscreenRect.bottom = fEditor.GetHeightOfRows (0, fEditor.GetRowCount ()); // make sure extends far enuf down - and then reset the WindowRect accordingly...
            fEditor.SetWindowRect (offscreenRect);

            Led_Distance farthestRHSMargin = fEditor.CalculateFarthestRightMarginInWindow ();
            if (farthestRHSMargin != static_cast<Led_Distance> (offscreenRect.GetRight ()) and farthestRHSMargin > 10) {
                offscreenRect.right = farthestRHSMargin;
                if (++nTimes > 5) {
                    // don't think this can ever happen - but in case...
                    Assert (false);
                }
                else {
                    goto Again;
                }
            }
        }

        Led_Tablet_ memDC;
        Verify (memDC.CreateCompatibleDC (tablet));
        Verify (memoryBitmap.CreateCompatibleBitmap (tablet->m_hDC, offscreenRect.GetWidth (), offscreenRect.GetHeight ()));
        Verify (memDC.SelectObject (memoryBitmap));
        (void)memDC.SetWindowOrg (offscreenRect.left, offscreenRect.top);

        LedItView::TemporarilyUseTablet tmpUseTablet (fEditor, &memDC, LedItView::TemporarilyUseTablet::eDontDoTextMetricsChangedCall);
        fEditor.Draw (offscreenRect, false);

        fEditor.SetWindowRect (savedWindowRect);
        fEditor.SetTopRowInWindowByMarkerPosition (savedScrollPos);
    }
    catch (...) {
        fEditor.SetWindowRect (savedWindowRect);
        fEditor.SetTopRowInWindowByMarkerPosition (savedScrollPos);
        throw;
    }

#define qCopyDIBToClipToTest 0
//  #define qCopyDIBToClipToTest    1
#if qCopyDIBToClipToTest
    {
        Verify (::OpenClipboard (fEditor.m_hWnd));
        Verify (::EmptyClipboard ());
        HGLOBAL dataHandle = ::GlobalAlloc (GMEM_DDESHARE, dibRAMSize);
        Verify (dataHandle);
        ::memcpy (Led_StackBasedHandleLocker (dataHandle).GetPointer (), tmpDIB, dibRAMSize);
        Verify (::SetClipboardData (CF_DIB, dataHandle));
        Verify (::CloseClipboard ());
        ::GlobalFree (dataHandle); // MAYTBE must do a ::GlobalFree (dataHandle) here?? DOCS on SetClipboardData() are ambiguous... - but robert complained of mem-leak and this could be it! - LGP 2000-06-28
    }
#endif

    VARIANT v;
    ::VariantInit (&v);

    try {
        PICTDESC pictDesc;
        ::memset (&pictDesc, 0, sizeof (pictDesc));
        pictDesc.cbSizeofstruct = sizeof (pictDesc);
        pictDesc.picType        = PICTYPE_BITMAP;
        pictDesc.bmp.hbitmap    = memoryBitmap;
        pictDesc.bmp.hpal       = NULL; // NOT SURE THIS IS RIGHT???
        CComQIPtr<IDispatch> result;
        Led_ThrowIfErrorHRESULT (::OleCreatePictureIndirect (&pictDesc, IID_IDispatch, true, (void**)&result));
        v.vt       = VT_DISPATCH;
        v.pdispVal = result.Detach ();
    }
    catch (...) {
    }
    return v;
}

long ActiveLedItControl::GetBufferLength ()
{
    return fEditor.GetLength ();
}

long ActiveLedItControl::GetMaxLength ()
{
    return fEditor.GetMaxLength ();
}

void ActiveLedItControl::SetMaxLength (long maxLength)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    fEditor.SetMaxLength (maxLength < 0 ? -1 : maxLength);
}

BOOL ActiveLedItControl::GetSupportContextMenu ()
{
    return fEditor.GetSupportContextMenu ();
}

void ActiveLedItControl::SetSupportContextMenu (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    if (bNewValue != GetSupportContextMenu ()) {
        fEditor.SetSupportContextMenu (!!bNewValue);
    }
}

BOOL ActiveLedItControl::OLE_GetHideDisabledContextMenuItems ()
{
    return fEditor.GetHideDisabledContextMenuItems ();
}

void ActiveLedItControl::OLE_SetHideDisabledContextMenuItems (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    if (bNewValue != OLE_GetHideDisabledContextMenuItems ()) {
        fEditor.SetHideDisabledContextMenuItems (!!bNewValue);
    }
}

BOOL ActiveLedItControl::GetSmartCutAndPaste ()
{
    return fEditor.GetSmartCutAndPasteMode ();
}

void ActiveLedItControl::SetSmartCutAndPaste (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    if (bNewValue != GetSmartCutAndPaste ()) {
        fEditor.SetSmartCutAndPasteMode (!!bNewValue);
    }
}

BOOL ActiveLedItControl::OLE_GetSmartQuoteMode ()
{
#if qWideCharacters
    return fEditor.GetSmartQuoteMode ();
#else
    return false;
#endif
}

void ActiveLedItControl::OLE_SetSmartQuoteMode (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
#if qWideCharacters
    if (bNewValue != OLE_GetSmartQuoteMode ()) {
        fEditor.SetSmartQuoteMode (!!bNewValue);
    }
#endif
}

BOOL ActiveLedItControl::GetWrapToWindow ()
{
    return fEditor.GetWrapToWindow ();
}

void ActiveLedItControl::SetWrapToWindow (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    if (bNewValue != GetWrapToWindow ()) {
        fEditor.SetWrapToWindow (!!bNewValue);
    }
}

BOOL ActiveLedItControl::GetShowParagraphGlyphs ()
{
    return fEditor.GetShowParagraphGlyphs ();
}

void ActiveLedItControl::SetShowParagraphGlyphs (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    if (bNewValue != GetShowParagraphGlyphs ()) {
        fEditor.SetShowParagraphGlyphs (!!bNewValue);
    }
}

BOOL ActiveLedItControl::GetShowTabGlyphs ()
{
    return fEditor.GetShowTabGlyphs ();
}

void ActiveLedItControl::SetShowTabGlyphs (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    if (bNewValue != GetShowTabGlyphs ()) {
        fEditor.SetShowTabGlyphs (!!bNewValue);
    }
}

BOOL ActiveLedItControl::GetShowSpaceGlyphs ()
{
    return fEditor.GetShowSpaceGlyphs ();
}

void ActiveLedItControl::SetShowSpaceGlyphs (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    if (bNewValue != GetShowSpaceGlyphs ()) {
        fEditor.SetShowSpaceGlyphs (!!bNewValue);
    }
}

BOOL ActiveLedItControl::OLE_GetUseSelectEOLBOLRowHilightStyle ()
{
    return fEditor.GetUseSelectEOLBOLRowHilightStyle ();
}

void ActiveLedItControl::OLE_SetUseSelectEOLBOLRowHilightStyle (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    if (bNewValue != OLE_GetUseSelectEOLBOLRowHilightStyle ()) {
        fEditor.SetUseSelectEOLBOLRowHilightStyle (!!bNewValue);
        fEditor.Refresh ();
    }
}

BOOL ActiveLedItControl::OLE_GetShowSecondaryHilight ()
{
    return fEditor.GetUseSecondaryHilight ();
}

void ActiveLedItControl::OLE_SetShowSecondaryHilight (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    if (bNewValue != OLE_GetShowSecondaryHilight ()) {
        fEditor.SetUseSecondaryHilight (!!bNewValue);
        fEditor.Refresh ();
    }
}

BOOL ActiveLedItControl::OLE_GetShowHidableText ()
{
    ColoredUniformHidableTextMarkerOwner* uhtmo = dynamic_cast<ColoredUniformHidableTextMarkerOwner*> (static_cast<HidableTextMarkerOwner*> (fEditor.GetHidableTextDatabase ().get ()));
    AssertNotNull (uhtmo);
    return !uhtmo->IsHidden ();
}

void ActiveLedItControl::OLE_SetShowHidableText (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    IdleManager::NonIdleContext nonIdleContext;
    if (bNewValue) {
        fEditor.GetHidableTextDatabase ()->ShowAll ();
    }
    else {
        fEditor.GetHidableTextDatabase ()->HideAll ();
    }
}

OLE_COLOR ActiveLedItControl::OLE_GetHidableTextColor ()
{
    try {
        ColoredUniformHidableTextMarkerOwner* uhtmo = dynamic_cast<ColoredUniformHidableTextMarkerOwner*> (static_cast<HidableTextMarkerOwner*> (fEditor.GetHidableTextDatabase ().get ()));
        AssertNotNull (uhtmo);
        return uhtmo->GetColor ().GetOSRep ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false);
    return 0; // NOTREACHED
}

void ActiveLedItControl::OLE_SetHidableTextColor (OLE_COLOR color)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        ColoredUniformHidableTextMarkerOwner* uhtmo = dynamic_cast<ColoredUniformHidableTextMarkerOwner*> (static_cast<HidableTextMarkerOwner*> (fEditor.GetHidableTextDatabase ().get ()));
        AssertNotNull (uhtmo);
        uhtmo->SetColor (Led_Color (TranslateColor (color)));
        fEditor.Refresh ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

BOOL ActiveLedItControl::OLE_GetHidableTextColored ()
{
    try {
        ColoredUniformHidableTextMarkerOwner* uhtmo = dynamic_cast<ColoredUniformHidableTextMarkerOwner*> (static_cast<HidableTextMarkerOwner*> (fEditor.GetHidableTextDatabase ().get ()));
        AssertNotNull (uhtmo);
        return uhtmo->GetColored ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false);
    return 0; // NOTREACHED
}

void ActiveLedItControl::OLE_SetHidableTextColored (BOOL bNewValue)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        ColoredUniformHidableTextMarkerOwner* uhtmo = dynamic_cast<ColoredUniformHidableTextMarkerOwner*> (static_cast<HidableTextMarkerOwner*> (fEditor.GetHidableTextDatabase ().get ()));
        AssertNotNull (uhtmo);
        uhtmo->SetColored (!!bNewValue);
        fEditor.Refresh ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

VARIANT ActiveLedItControl::OLE_GetSpellChecker ()
{
    try {
        VARIANT result;
        ::VariantInit (&result);
        if (fSpellChecker != NULL) {
            result.vt       = VT_DISPATCH;
            result.pdispVal = fSpellChecker;
            result.pdispVal->AddRef ();
        }
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

void ActiveLedItControl::OLE_SetSpellChecker (VARIANT& newValue)
{
    // Note: the MSVC Class wizard for OLE / MFC in MSVC.Net 2003 creates the property setter as taking a 'VARIANT' argument. However
    // empirically - thats NOT what gets passed! This reference crap (or pointer) is necessary to get the right value assigned to
    // us - LGP 2003-06-11
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        IdleManager::NonIdleContext nonIdleContext;
        if (fSpellChecker != NULL) {
            fSpellChecker->Release ();
            fSpellChecker = NULL;
            ChangedSpellCheckerCOMObject ();
        }

        Assert (fSpellChecker == NULL);
        VARIANT tmpV;
        ::VariantInit (&tmpV);
        if (::VariantChangeType (&tmpV, &newValue, 0, VT_DISPATCH) == S_OK) {
            fSpellChecker = tmpV.pdispVal;
            if (fSpellChecker != NULL) {
                fSpellChecker->AddRef ();
            }
            ::VariantClear (&tmpV);
            ChangedSpellCheckerCOMObject ();
        }
        else if (::VariantChangeType (&tmpV, &newValue, 0, VT_BSTR) == S_OK) {
            // must be a CLSID or a PROGID. Either way - try to create the object
            HRESULT hr = S_OK;
            if (::SysStringLen (tmpV.bstrVal) != 0) {
                CLSID theCLSID;
                if (SUCCEEDED (hr = ::CLSIDFromProgID (tmpV.bstrVal, &theCLSID)) or
                    SUCCEEDED (hr = ::CLSIDFromString (tmpV.bstrVal, &theCLSID))) {
                    hr = ::CoCreateInstance (theCLSID, NULL, CLSCTX_ALL, IID_IDispatch, reinterpret_cast<LPVOID*> (&fSpellChecker));
                    Assert (SUCCEEDED (hr) == (fSpellChecker != NULL));
                }
            }
            ::VariantClear (&tmpV);
            ChangedSpellCheckerCOMObject ();
            Led_ThrowIfErrorHRESULT (hr);
        }
        else {
            Led_ThrowIfErrorHRESULT (DISP_E_TYPEMISMATCH);
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

void ActiveLedItControl::ChangedSpellCheckerCOMObject ()
{
    fEditor.SetSpellCheckEngine (NULL);
    fLedSpellCheckWrapper = COMBased_SpellCheckEngine (fSpellChecker);
    if (fSpellChecker != NULL) {
        fEditor.SetSpellCheckEngine (&fLedSpellCheckWrapper);
    }
}

VARIANT ActiveLedItControl::OLE_GetContextMenu ()
{
    try {
        VARIANT result;
        ::VariantInit (&result);
        if (fConextMenu.p != NULL) {
            result.vt       = VT_DISPATCH;
            result.pdispVal = fConextMenu;
            result.pdispVal->AddRef ();
        }
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

void ActiveLedItControl::OLE_SetContextMenu (VARIANT& newValue)
{
    // Note: the MSVC Class wizard for OLE / MFC in MSVC.Net 2003 creates the property setter as taking a 'VARIANT' argument. However
    // empirically - thats NOT what gets passed! This reference crap (or pointer) is necessary to get the right value assigned to
    // us - LGP 2003-06-11 (originally - but now copied for SetContextMenu - LGP 2004-01-14)
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        fConextMenu.Release ();
        VARIANT tmpV;
        ::VariantInit (&tmpV);
        if (::VariantChangeType (&tmpV, &newValue, 0, VT_DISPATCH) == S_OK) {
            fConextMenu = tmpV.pdispVal;
            ::VariantClear (&tmpV);
        }
        else {
            Led_ThrowIfErrorHRESULT (DISP_E_TYPEMISMATCH);
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

VARIANT ActiveLedItControl::OLE_GetToolbarList ()
{
    try {
        VARIANT result;
        ::VariantInit (&result);
        if (fToolbarList.p != NULL) {
            result.vt       = VT_DISPATCH;
            result.pdispVal = fToolbarList;
            result.pdispVal->AddRef ();
        }
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

void ActiveLedItControl::OLE_SetToolbarList (VARIANT& newValue)
{
    // Note: the MSVC Class wizard for OLE / MFC in MSVC.Net 2003 creates the property setter as taking a 'VARIANT' argument. However
    // empirically - thats NOT what gets passed! This reference crap (or pointer) is necessary to get the right value assigned to
    // us - LGP 2003-06-11 (originally - but now copied for SetContextMenu - LGP 2004-01-14)
    try {
        IdleManager::NonIdleContext nonIdleContext;
        {
            CComQIPtr<IALToolbarList> tbl = fToolbarList;
            if (tbl.p != NULL) {
                tbl->NotifyOfOwningActiveLedIt (NULL, NULL);
            }
        }
        fToolbarList.Release ();
        VARIANT tmpV;
        ::VariantInit (&tmpV);
        if (::VariantChangeType (&tmpV, &newValue, 0, VT_DISPATCH) == S_OK) {
            fToolbarList = tmpV.pdispVal;
            ::VariantClear (&tmpV);
        }
        else {
            Led_ThrowIfErrorHRESULT (DISP_E_TYPEMISMATCH);
        }
        {
            CComQIPtr<IALToolbarList> tbl = fToolbarList;
            if (tbl.p != NULL) {
                tbl->NotifyOfOwningActiveLedIt (CComQIPtr<IDispatch> (GetControllingUnknown ()), m_hWnd);
            }
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

VARIANT ActiveLedItControl::OLE_GetBuiltinCommands ()
{
    try {
        if (fBuiltinCommands.p == NULL) {
            fBuiltinCommands = GenerateBuiltinCommandsObject ();
        }
        VARIANT result;
        ::VariantInit (&result);
        {
            result.vt       = VT_DISPATCH;
            result.pdispVal = fBuiltinCommands;
            result.pdispVal->AddRef ();
        }
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

namespace {
    CComPtr<IDispatch> mkMenu_Select ()
    {
        try {
            CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
            Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
            CComQIPtr<IDispatch> result = o->GetUnknown ();

            o->put_Name (CComBSTR (L"Select"));
            o->put_InternalName (CComBSTR (kName_SelectMenu));

            o->AppendBuiltinCmd (kCmd_SelectWord);
            o->AppendBuiltinCmd (kCmd_SelectTextRow);
            o->AppendBuiltinCmd (kCmd_SelectParagraph);

            o->AppendBuiltinCmd (kCmd_Separator);

            o->AppendBuiltinCmd (kCmd_SelectTableIntraCellAll);
            o->AppendBuiltinCmd (kCmd_SelectTableCell);
            o->AppendBuiltinCmd (kCmd_SelectTableRow);
            o->AppendBuiltinCmd (kCmd_SelectTableColumn);
            o->AppendBuiltinCmd (kCmd_SelectTable);

            o->AppendBuiltinCmd (kCmd_Separator);

            o->AppendBuiltinCmd (kCmd_SelectAll);

            o->AppendBuiltinCmd (kCmd_Separator);

            o->AppendBuiltinCmd (kCmd_Find);
            o->AppendBuiltinCmd (kCmd_FindAgain);
            o->AppendBuiltinCmd (kCmd_EnterFindString);
            o->AppendBuiltinCmd (kCmd_Replace);
            o->AppendBuiltinCmd (kCmd_ReplaceAgain);

            o->AppendBuiltinCmd (kCmd_Separator);

            o->AppendBuiltinCmd (kCmd_CheckSpelling);
            return result;
        }
        CATCH_AND_HANDLE_EXCEPTIONS ();
    }

    CComPtr<IDispatch> mkMenu_Insert ()
    {
        try {
            CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
            Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
            CComQIPtr<IDispatch> result = o->GetUnknown ();

            o->put_Name (CComBSTR (L"Insert"));
            o->put_InternalName (CComBSTR (kName_InsertMenu));

            o->AppendBuiltinCmd (kCmd_InsertTable);
            o->AppendBuiltinCmd (kCmd_InsertTableRowAbove);
            o->AppendBuiltinCmd (kCmd_InsertTableRowBelow);
            o->AppendBuiltinCmd (kCmd_InsertTableColBefore);
            o->AppendBuiltinCmd (kCmd_InsertTableColAfter);

            o->AppendBuiltinCmd (kCmd_Separator);

            o->AppendBuiltinCmd (kCmd_InsertURL);
            o->AppendBuiltinCmd (kCmd_InsertSymbol);
            return result;
        }
        CATCH_AND_HANDLE_EXCEPTIONS ();
    }

    CComPtr<IDispatch> mkMenu_Font ()
    {
        try {
            CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
            Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
            CComQIPtr<IDispatch> result = o->GetUnknown ();

            o->put_Name (CComBSTR (L"Font"));
            o->put_InternalName (CComBSTR (kName_FontNameMenu));

            const vector<Led_SDK_String>& fontNames = GetUsableFontNames ();
            Assert (fontNames.size () <= kLastFontNameCmd - kBaseFontNameCmd + 1);
            for (size_t i = 0; i < fontNames.size (); i++) {
                WORD cmdNum = static_cast<WORD> (kBaseFontNameCmd + i);
                if (cmdNum > kLastFontNameCmd) {
                    break; // asserted out before above - now just ignore extra font names...
                }
                ActiveLedIt_BuiltinCommand* c = ActiveLedIt_BuiltinCommand::mk (BuiltinCmdSpec (cmdNum, mkFontNameCMDName (fontNames[i]).c_str ()));
                c->SetName (Led_SDKString2Wide (fontNames[i]));
                o->Insert (c);
            }

            return result;
        }
        CATCH_AND_HANDLE_EXCEPTIONS ();
    }

    CComPtr<IDispatch> mkMenu_FontStyle ()
    {
        try {
            CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
            Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
            CComQIPtr<IDispatch> result = o->GetUnknown ();

            o->put_Name (CComBSTR (L"Font Style"));
            o->put_InternalName (CComBSTR (kName_FontStyleMenu));

            o->AppendBuiltinCmd (kCmd_FontStylePlain);

            o->AppendBuiltinCmd (kCmd_Separator);

            o->AppendBuiltinCmd (kCmd_FontStyleBold);
            o->AppendBuiltinCmd (kCmd_FontStyleItalic);
            o->AppendBuiltinCmd (kCmd_FontStyleUnderline);
            o->AppendBuiltinCmd (kCmd_FontStyleStrikeout);
            o->AppendBuiltinCmd (kCmd_SubScript);
            o->AppendBuiltinCmd (kCmd_SuperScript);

            return result;
        }
        CATCH_AND_HANDLE_EXCEPTIONS ();
    }

    CComPtr<IDispatch> mkMenu_FontSize (bool forComboBox)
    {
        try {
            CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
            Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
            CComQIPtr<IDispatch> result = o->GetUnknown ();

            o->put_Name (CComBSTR (L"Font Size"));
            o->put_InternalName (CComBSTR (kName_FontSizeMenu));

            o->AppendBuiltinCmd (kCmd_FontSize9);
            o->AppendBuiltinCmd (kCmd_FontSize10);
            o->AppendBuiltinCmd (kCmd_FontSize12);
            o->AppendBuiltinCmd (kCmd_FontSize14);
            o->AppendBuiltinCmd (kCmd_FontSize18);
            o->AppendBuiltinCmd (kCmd_FontSize24);
            o->AppendBuiltinCmd (kCmd_FontSize36);
            o->AppendBuiltinCmd (kCmd_FontSize48);
            o->AppendBuiltinCmd (kCmd_FontSize72);

            if (not forComboBox) {
                o->AppendBuiltinCmd (kCmd_Separator);

                o->AppendBuiltinCmd (kCmd_FontSizeSmaller);
                o->AppendBuiltinCmd (kCmd_FontSizeLarger);

                o->AppendBuiltinCmd (kCmd_Separator);
            }

            o->AppendBuiltinCmd (kCmd_FontSizeOther);

            return result;
        }
        CATCH_AND_HANDLE_EXCEPTIONS ();
    }

    CComPtr<IDispatch> mkMenu_FontColor ()
    {
        try {
            CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
            Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
            CComQIPtr<IDispatch> result = o->GetUnknown ();

            o->put_Name (CComBSTR (L"Font Color"));
            o->put_InternalName (CComBSTR (kName_FontColorMenu));

            o->AppendBuiltinCmd (kCmd_BlackColor);
            o->AppendBuiltinCmd (kCmd_MaroonColor);
            o->AppendBuiltinCmd (kCmd_GreenColor);
            o->AppendBuiltinCmd (kCmd_OliveColor);
            o->AppendBuiltinCmd (kCmd_NavyColor);
            o->AppendBuiltinCmd (kCmd_PurpleColor);
            o->AppendBuiltinCmd (kCmd_TealColor);
            o->AppendBuiltinCmd (kCmd_GrayColor);
            o->AppendBuiltinCmd (kCmd_SilverColor);
            o->AppendBuiltinCmd (kCmd_RedColor);
            o->AppendBuiltinCmd (kCmd_LimeColor);
            o->AppendBuiltinCmd (kCmd_YellowColor);
            o->AppendBuiltinCmd (kCmd_BlueColor);
            o->AppendBuiltinCmd (kCmd_FuchsiaColor);
            o->AppendBuiltinCmd (kCmd_AquaColor);
            o->AppendBuiltinCmd (kCmd_WhiteColor);

            o->AppendBuiltinCmd (kCmd_Separator);

            o->AppendBuiltinCmd (kCmd_OtherColor);

            return result;
        }
        CATCH_AND_HANDLE_EXCEPTIONS ();
    }

    CComPtr<IDispatch> mkMenu_ParagraphJustification ()
    {
        try {
            CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
            Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
            CComQIPtr<IDispatch> result = o->GetUnknown ();

            o->put_Name (CComBSTR (L"Paragraph Justification"));
            o->put_InternalName (CComBSTR (kName_ParagraphJustification));

            o->AppendBuiltinCmd (kCmd_JustifyLeft);
            o->AppendBuiltinCmd (kCmd_JustifyCenter);
            o->AppendBuiltinCmd (kCmd_JustifyRight);
            o->AppendBuiltinCmd (kCmd_JustifyFull);

            return result;
        }
        CATCH_AND_HANDLE_EXCEPTIONS ();
    }

    CComPtr<IDispatch> mkMenu_ListStyle ()
    {
        try {
            CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
            Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
            CComQIPtr<IDispatch> result = o->GetUnknown ();

            o->put_Name (CComBSTR (L"List Style"));
            o->put_InternalName (CComBSTR (kName_ListStyle));

            o->AppendBuiltinCmd (kCmd_ListStyle_None);
            o->AppendBuiltinCmd (kCmd_ListStyle_Bullet);

            return result;
        }
        CATCH_AND_HANDLE_EXCEPTIONS ();
    }
}

VARIANT ActiveLedItControl::OLE_GetPredefinedMenus ()
{
    try {
        if (fPredefinedMenus.p == NULL) {
            CComObject<ActiveLedIt_StaticCommandList>* o = NULL;
            Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_StaticCommandList>::CreateInstance (&o));
            CComPtr<IDispatch> saved = o; // so auto-deleted on exception

            o->Append (mkMenu_Select ());
            o->Append (mkMenu_Insert ());
            o->Append (mkMenu_Font ());
            o->Append (mkMenu_FontStyle ());
            o->Append (mkMenu_FontSize (false));
            o->Append (mkMenu_FontColor ());
            o->Append (mkMenu_ParagraphJustification ());
            o->Append (mkMenu_ListStyle ());

            fPredefinedMenus = o;
        }
        VARIANT result;
        ::VariantInit (&result);
        {
            result.vt       = VT_DISPATCH;
            result.pdispVal = fPredefinedMenus;
            result.pdispVal->AddRef ();
        }
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

VARIANT ActiveLedItControl::OLE_GetAcceleratorTable ()
{
    try {
        VARIANT result;
        ::VariantInit (&result);
        if (fAcceleratorTable.p != NULL) {
            result.vt       = VT_DISPATCH;
            result.pdispVal = fAcceleratorTable;
            result.pdispVal->AddRef ();
        }
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

void ActiveLedItControl::OLE_SetAcceleratorTable (VARIANT& newValue)
{
    // Note: the MSVC Class wizard for OLE / MFC in MSVC.Net 2003 creates the property setter as taking a 'VARIANT' argument. However
    // empirically - thats NOT what gets passed! This reference crap (or pointer) is necessary to get the right value assigned to
    // us - LGP 2003-06-11 (originally - but now copied for SetContextMenu - LGP 2004-01-14)
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        fAcceleratorTable.Release ();
        VARIANT tmpV;
        ::VariantInit (&tmpV);
        if (::VariantChangeType (&tmpV, &newValue, 0, VT_DISPATCH) == S_OK) {
            fAcceleratorTable = tmpV.pdispVal;
            ::VariantClear (&tmpV);
        }
        else {
            Led_ThrowIfErrorHRESULT (DISP_E_TYPEMISMATCH);
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

HACCEL ActiveLedItControl::GetCurrentWin32AccelTable ()
{
    const float kTimeBetweenRecomputes = 10.0f;
    if (fWin32AccelTable == NULL or Time::GetTickCount () - fLastAccelTableUpdateAt > kTimeBetweenRecomputes) {
        {
            CComQIPtr<IALAcceleratorTable> accelTable = fAcceleratorTable;
            if (accelTable.p != NULL) {
                HACCEL maybeNewAccelTable = NULL;
                accelTable->GenerateWin32AcceleratorTable (&maybeNewAccelTable);
                fLastAccelTableUpdateAt = Time::GetTickCount ();

                // Now - to avoid causing problems with callers of GetCurrentWin32AccelTable () that might want to hang onto a
                // pointer (and wouldn't want it to get stale ;-)) - like OnGetControlInfo - try to avoid changing the handle
                // if not actually needed. See if the old and new handles are identical - and if so - delete the NEW one rather
                // than the old...
                bool keepOld = false;
                if (fWin32AccelTable != NULL) {
                    size_t accelTableSize = static_cast<size_t> (::CopyAcceleratorTable (fWin32AccelTable, NULL, 0));
                    if (accelTableSize == static_cast<size_t> (::CopyAcceleratorTable (maybeNewAccelTable, NULL, 0))) {
                        Memory::SmallStackBuffer<ACCEL> oldOne (accelTableSize);
                        Verify (::CopyAcceleratorTable (fWin32AccelTable, oldOne, accelTableSize) == static_cast<int> (accelTableSize));
                        Memory::SmallStackBuffer<ACCEL> newOne (accelTableSize);
                        Verify (::CopyAcceleratorTable (maybeNewAccelTable, newOne, accelTableSize) == static_cast<int> (accelTableSize));
                        if (::memcmp (oldOne, newOne, accelTableSize * sizeof (ACCEL)) == 0) {
                            keepOld = true;
                        }
                    }
                }
                if (keepOld) {
                    ::DestroyAcceleratorTable (maybeNewAccelTable);
                }
                else {
                    if (fWin32AccelTable != NULL) {
                        ::DestroyAcceleratorTable (fWin32AccelTable);
                        fWin32AccelTable = NULL;
                    }
                    fWin32AccelTable = maybeNewAccelTable;
                }
            }
        }
    }
    return fWin32AccelTable;
}

IDispatch* ActiveLedItControl::OLE_GetDefaultContextMenu ()
{
    try {
        CComObject<ActiveLedIt_UserCommandList>* o = NULL;
        Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_UserCommandList>::CreateInstance (&o));
        CComQIPtr<IDispatch> saved = o->GetUnknown (); // so deleted on exceptions during the subsequent add code

        o->AppendBuiltinCmd (kCmd_Undo);
        o->AppendBuiltinCmd (kCmd_Redo);
        o->AppendBuiltinCmd (kCmd_Separator);
        o->AppendBuiltinCmd (kCmd_Cut);
        o->AppendBuiltinCmd (kCmd_Copy);
        o->AppendBuiltinCmd (kCmd_Paste);
        o->AppendBuiltinCmd (kCmd_Clear);
        o->AppendBuiltinCmd (kCmd_Separator);

        o->Insert (mkMenu_Select ());
        o->Insert (mkMenu_Insert ());

        o->AppendBuiltinCmd (kCmd_Separator);
        o->AppendBuiltinCmd (kCmd_RemoveTableRows);
        o->AppendBuiltinCmd (kCmd_RemoveTableColumns);
        o->AppendBuiltinCmd (kCmd_Separator);

        o->Insert (mkMenu_Font ());
        o->Insert (mkMenu_FontStyle ());
        o->Insert (mkMenu_FontSize (false));
        o->Insert (mkMenu_FontColor ());

        o->AppendBuiltinCmd (kCmd_ChooseFontDialog);
        o->AppendBuiltinCmd (kCmd_Separator);

        o->Insert (mkMenu_ParagraphJustification ());

        o->AppendBuiltinCmd (kCmd_ParagraphIndents);
        o->AppendBuiltinCmd (kCmd_ParagraphSpacing);
        o->AppendBuiltinCmd (kCmd_Separator);

        o->Insert (mkMenu_ListStyle ());

        o->AppendBuiltinCmd (kCmd_IncreaseIndent);
        o->AppendBuiltinCmd (kCmd_DescreaseIndent);
        o->AppendBuiltinCmd (kCmd_Separator);
        o->AppendBuiltinCmd (kCmd_PropertiesForSelection);
        o->AppendBuiltinCmd (kCmd_OpenEmbedding);
        o->AppendBuiltinCmd (kCmd_Separator);

        o->AppendBuiltinCmd (kCmd_Print);
        o->AppendBuiltinCmd (kCmd_PrintSetup);

        o->AppendBuiltinCmd (kCmd_Separator);

        o->AppendBuiltinCmd (kCmd_BrowseOnlineHelp);
        o->AppendBuiltinCmd (kCmd_CheckForUpdatesOnWeb);

        o->AppendBuiltinCmd (kCmd_Separator);

        o->AppendBuiltinCmd (kCmd_About);

        o->AddRef (); // cuz returning a pointer
        return saved;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

IDispatch* ActiveLedItControl::OLE_GetDefaultAcceleratorTable ()
{
    try {
        CComObject<ActiveLedIt_AcceleratorTable>* o = NULL;
        Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_AcceleratorTable>::CreateInstance (&o));
        CComQIPtr<IDispatch> saved = o->GetUnknown (); // so deleted on exceptions during the subsequent add code

        o->AppendACCEL ("Undo", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'Z');
        o->AppendACCEL ("Undo", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eALT), VK_BACK);
        o->AppendACCEL ("Redo", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'Y');
        o->AppendACCEL ("Cut", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'X');
        o->AppendACCEL ("Cut", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eSHIFT), VK_DELETE);
        o->AppendACCEL ("Copy", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'C');
        o->AppendACCEL ("Copy", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), VK_INSERT);
        o->AppendACCEL ("Paste", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'V');
        o->AppendACCEL ("Paste", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eSHIFT), VK_INSERT);
        o->AppendACCEL ("SelectAll", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'A');
        o->AppendACCEL ("ChooseFontDialog", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'D');
        o->AppendACCEL ("Find", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'F');
        o->AppendACCEL ("FindAgain", static_cast<AcceleratorModifierFlag> (eVIRTKEY), VK_F3);
        o->AppendACCEL ("EnterFindString", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'E');
        o->AppendACCEL ("Replace", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'H');
        o->AppendACCEL ("ReplaceAgain", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'R');
        o->AppendACCEL ("CheckSpelling", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'L');
        o->AppendACCEL ("Plain", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'T');
        o->AppendACCEL ("Bold", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'B');
        o->AppendACCEL ("Print", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'P');
        o->AppendACCEL ("Italic", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'I');
        o->AppendACCEL ("Underline", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'U');
        o->AppendACCEL ("Smaller", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), VK_SUBTRACT);
        o->AppendACCEL ("Larger", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), VK_ADD);
        o->AppendACCEL ("Properties", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eALT), VK_RETURN);
        o->AppendACCEL ("IncreaseListIndent", static_cast<AcceleratorModifierFlag> (eVIRTKEY), VK_TAB);
        o->AppendACCEL ("DecreaseListIndent", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eSHIFT), VK_TAB);
        o->AppendACCEL ("BrowseOnlineHelp", static_cast<AcceleratorModifierFlag> (eVIRTKEY), VK_F1);
#if 0
            o->AppendACCEL ("Help", static_cast<AcceleratorModifierFlag> (eVIRTKEY), VK_F1);
            o->AppendACCEL ("ContextHelp", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eSHIFT), VK_F1);
#endif
        o->AddRef (); // cuz returning a pointer
        return saved;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

IDispatch* ActiveLedItControl::OLE_MakeNewPopupMenuItem ()
{
    try {
        CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
        Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
        o->AddRef (); // cuz returning a pointer
        CComQIPtr<IDispatch> result = o->GetUnknown ();
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

IDispatch* ActiveLedItControl::OLE_MakeNewUserMenuItem ()
{
    try {
        CComObject<ActiveLedIt_UserCommand>* o = NULL;
        Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_UserCommand>::CreateInstance (&o));
        o->AddRef (); // cuz returning a pointer
        CComQIPtr<IDispatch> result = o->GetUnknown ();
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

IDispatch* ActiveLedItControl::OLE_MakeNewAcceleratorElement ()
{
    try {
        CComObject<ActiveLedIt_AcceleratorElement>* o = NULL;
        Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_AcceleratorElement>::CreateInstance (&o));
        o->AddRef (); // cuz returning a pointer
        CComQIPtr<IDispatch> result = o->GetUnknown ();
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

void ActiveLedItControl::OLE_InvokeCommand (const VARIANT& command)
{
    try {
        IdleManager::NonIdleContext nonIdleContext;
        UINT32                      cmdNum = CmdObjOrName2Num (command);
        if (cmdNum != 0) {
            (void)::SendMessage (fEditor.GetHWND (), WM_COMMAND, cmdNum, 0);
            return;
        }
        throw HRESULTErrorException (TYPE_E_ELEMENTNOTFOUND);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

BOOL ActiveLedItControl::OLE_CommandEnabled (const VARIANT& command)
{
    try {
        UINT32 cmdNum = CmdObjOrName2Num (command);

        struct MyCmdUI : CCmdUI {
            MyCmdUI ()
                : CCmdUI ()
                , fEnabled (false)
            {
            }
            virtual void Enable (BOOL bOn) override
            {
                fEnabled         = !!bOn;
                m_bEnableChanged = TRUE;
            }
            virtual void SetCheck (int /*nCheck*/) override
            {
            }
            virtual void SetText (LPCTSTR /*lpszText*/) override
            {
            }

            bool fEnabled;
        };
        MyCmdUI state;
        state.m_nID = cmdNum;
        state.DoUpdate (&fEditor, true);
        return state.fEnabled;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

BOOL ActiveLedItControl::OLE_CommandChecked (const VARIANT& command)
{
    try {
        UINT32 cmdNum = CmdObjOrName2Num (command);

        struct MyCmdUI : CCmdUI {
            MyCmdUI ()
                : CCmdUI ()
                , fChecked (false)
            {
            }
            virtual void Enable ([[maybe_unused]] BOOL bOn) override
            {
                m_bEnableChanged = TRUE;
            }
            virtual void SetCheck (int nCheck) override
            {
                fChecked = nCheck != 0;
            }
            virtual void SetText (LPCTSTR /*lpszText*/) override
            {
            }

            bool fChecked;
        };
        MyCmdUI state;
        state.m_nID = cmdNum;
        state.DoUpdate (&fEditor, true);
        return state.fChecked;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

IDispatch* ActiveLedItControl::OLE_MakeNewToolbarList ()
{
    try {
        CComObject<ActiveLedIt_ToolbarList>* o = NULL;
        Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_ToolbarList>::CreateInstance (&o));
        o->AddRef (); // cuz returning a pointer
        CComQIPtr<IDispatch> result = o->GetUnknown ();
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

CComPtr<IDispatch> ActiveLedItControl::MakeNewToolbar ()
{
    CComPtr<IDispatch> tmp;
    tmp.Attach (OLE_MakeNewToolbar ());
    return tmp;
}

IDispatch* ActiveLedItControl::OLE_MakeNewToolbar ()
{
    try {
        CComObject<ActiveLedIt_Toolbar>* o = NULL;
        Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_Toolbar>::CreateInstance (&o));
        o->AddRef (); // cuz returning a pointer
        CComQIPtr<IDispatch> result = o->GetUnknown ();
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

IDispatch* ActiveLedItControl::OLE_MakeIconButtonToolbarItem ()
{
    try {
        CComObject<ActiveLedIt_IconButtonToolbarElement>* o = NULL;
        Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_IconButtonToolbarElement>::CreateInstance (&o));
        o->AddRef (); // cuz returning a pointer
        CComQIPtr<IDispatch> result = o->GetUnknown ();
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

CComPtr<IDispatch> ActiveLedItControl::MakeSeparatorToolbarItem ()
{
    CComPtr<IDispatch> tmp;
    tmp.Attach (OLE_MakeSeparatorToolbarItem ());
    return tmp;
}

IDispatch* ActiveLedItControl::OLE_MakeSeparatorToolbarItem ()
{
    try {
        CComObject<ActiveLedIt_SeparatorToolbarElement>* o = NULL;
        Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_SeparatorToolbarElement>::CreateInstance (&o));
        o->AddRef (); // cuz returning a pointer
        CComQIPtr<IDispatch> result = o->GetUnknown ();
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

CComPtr<IDispatch> ActiveLedItControl::MakeBuiltinToolbar (LPCOLESTR builtinToolbarName)
{
    if (wstring (builtinToolbarName) == L"Standard") {
        CComPtr<IDispatch>    newTB = MakeNewToolbar ();
        CComQIPtr<IALToolbar> tb    = newTB;
        Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"EditBar")));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"FormatBar")));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Print"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("ActiveLedIt"))));
        return newTB;
    }

    if (wstring (builtinToolbarName) == L"StandardToolsOnly") { // not including format bar - assumes will be added separately
        CComPtr<IDispatch>    newTB = MakeNewToolbar ();
        CComQIPtr<IALToolbar> tb    = newTB;
        Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"EditBar")));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"SelectBar")));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"InsertBar")));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Print"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("ActiveLedIt"))));
        return newTB;
    }

    if (wstring (builtinToolbarName) == L"FormatBar") {
        CComPtr<IDispatch>    newTB = MakeNewToolbar ();
        CComQIPtr<IALToolbar> tb    = newTB;
        Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"CharacterFormatBar")));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"ParagraphFormatBar")));
        return newTB;
    }

    if (wstring (builtinToolbarName) == L"EditBar") {
        CComPtr<IDispatch>    newTB = MakeNewToolbar ();
        CComQIPtr<IALToolbar> tb    = newTB;
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Undo"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Redo"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Cut"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Copy"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Paste"))));
        return newTB;
    }

    if (wstring (builtinToolbarName) == L"SelectBar") {
        CComPtr<IDispatch>    newTB = MakeNewToolbar ();
        CComQIPtr<IALToolbar> tb    = newTB;
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Find"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Replace"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("CheckSpelling"))));
        return newTB;
    }

    if (wstring (builtinToolbarName) == L"InsertBar") {
        CComPtr<IDispatch>    newTB = MakeNewToolbar ();
        CComQIPtr<IALToolbar> tb    = newTB;
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("InsertTable"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("InsertURL"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("InsertSymbol"))));
        return newTB;
    }

    if (wstring (builtinToolbarName) == L"CharacterFormatBar") {
        CComPtr<IDispatch>    newTB = MakeNewToolbar ();
        CComQIPtr<IALToolbar> tb    = newTB;
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("FontNameComboBox"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("FontSizeComboBox"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Bold"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Italic"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Underline"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("FontColor"))));
        return newTB;
    }

    if (wstring (builtinToolbarName) == L"ParagraphFormatBar") {
        CComPtr<IDispatch>    newTB = MakeNewToolbar ();
        CComQIPtr<IALToolbar> tb    = newTB;
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("JustifyLeft"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("JustifyCenter"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("JustifyRight"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("JustifyFull"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("ParagraphIndents"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("ParagraphSpacing"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("ListStyle_None"))));
        Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("ListStyle_Bullet"))));
        return newTB;
    }

    // if name doesn't match - return NULL
    return CComPtr<IDispatch> ();
}

IDispatch* ActiveLedItControl::OLE_MakeBuiltinToolbar (LPCOLESTR builtinToolbarName)
{
    try {
        return MakeBuiltinToolbar (builtinToolbarName).Detach ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

CComPtr<IDispatch> ActiveLedItControl::MakeBuiltinToolbarItem (LPCOLESTR builtinToolbarItemName)
{
    CComPtr<IDispatch> tmp;
    tmp.Attach (OLE_MakeBuiltinToolbarItem (builtinToolbarItemName));
    return tmp;
}

IDispatch* ActiveLedItControl::OLE_MakeBuiltinToolbarItem (LPCOLESTR builtinToolbarItemName)
{
    const ActiveLedItControl::ToolBarIconSpec kToolBarIconSpecs[] = {
        {_T("Undo"), kToolbarButton_Undo_ResID, _T("Undo"), eIconButton_PushButton},
        {_T("Redo"), kToolbarButton_Redo_ResID, _T("Redo"), eIconButton_PushButton},
        {_T("Cut"), kToolbarButton_Cut_ResID, _T("Cut"), eIconButton_PushButton},
        {_T("Copy"), kToolbarButton_Copy_ResID, _T("Copy"), eIconButton_PushButton},
        {_T("Paste"), kToolbarButton_Paste_ResID, _T("Paste"), eIconButton_PushButton},
        {_T("Print"), kToolbarButton_Print_ResID, _T("Print"), eIconButton_PushButton},
        {_T("Bold"), kToolbarButton_Bold_ResID, _T("Bold"), eIconButton_Toggle},
        {_T("Italic"), kToolbarButton_Italics_ResID, _T("Italic"), eIconButton_Toggle},
        {_T("Underline"), kToolbarButton_Underline_ResID, _T("Underline"), eIconButton_Toggle},
        {_T("JustifyLeft"), kToolbarButton_JustifyLeft_ResID, _T("JustifyLeft"), eIconButton_Sticky},
        {_T("JustifyCenter"), kToolbarButton_JustifyCenter_ResID, _T("JustifyCenter"), eIconButton_Sticky},
        {_T("JustifyRight"), kToolbarButton_JustifyRight_ResID, _T("JustifyRight"), eIconButton_Sticky},
        {_T("JustifyFull"), kToolbarButton_JustifyFull_ResID, _T("JustifyFull"), eIconButton_Sticky},
        {_T("ParagraphIndents"), kToolbarButton_ParagraphIndents_ResID, _T("ParagraphIndents"), eIconButton_PushButton},
        {_T("ParagraphSpacing"), kToolbarButton_ParagraphSpacing_ResID, _T("ParagraphSpacing"), eIconButton_PushButton},
        {_T("ListStyle_None"), kToolbarButton_ListStyle_None_ResID, _T("ListStyle_None"), eIconButton_Sticky},
        {_T("ListStyle_Bullet"), kToolbarButton_ListStyle_Bullet_ResID, _T("ListStyle_Bullet"), eIconButton_Sticky},
        {_T("Find"), kToolbarButton_Find_ResID, _T("Find"), eIconButton_PushButton},
        {_T("Replace"), kToolbarButton_Replace_ResID, _T("Replace"), eIconButton_PushButton},
        {_T("CheckSpelling"), kToolbarButton_CheckSpelling_ResID, _T("CheckSpelling"), eIconButton_PushButton},
        {_T("InsertTable"), kToolbarButton_InsertTable_ResID, _T("InsertTable"), eIconButton_PushButton},
        {_T("InsertURL"), kToolbarButton_InsertURL_ResID, _T("InsertURL"), eIconButton_PushButton},
        {_T("InsertSymbol"), kToolbarButton_InsertSymbol_ResID, _T("InsertSymbol"), eIconButton_PushButton},
    };

    try {
        for (size_t i = 0; i < NEltsOf (kToolBarIconSpecs); ++i) {
            if (CComBSTR (builtinToolbarItemName) == CComBSTR (kToolBarIconSpecs[i].fIconName)) {
                return mkIconElement (kToolBarIconSpecs[i]).Detach ();
            }
        }

        if (wstring (builtinToolbarItemName) == wstring (L"FontNameComboBox")) {
            return MakeBuiltinComboBoxToolbarItem (mkMenu_Font ()).Detach ();
        }
        else if (wstring (builtinToolbarItemName) == wstring (L"FontSizeComboBox")) {
            return MakeBuiltinComboBoxToolbarItem (mkMenu_FontSize (true)).Detach ();
        }
        else if (wstring (builtinToolbarItemName) == wstring (L"FontColor")) {
            return mkIconElement (kToolbarButton_FontColor_ResID, mkMenu_FontColor ()).Detach ();
        }
        else if (wstring (builtinToolbarItemName) == wstring (L"ActiveLedIt")) {
            CComPtr<IDispatch> tmp;
            tmp.Attach (OLE_GetDefaultContextMenu ());
            return mkIconElement (kToolbarButton_ActiveLedIt_ResID, tmp).Detach ();
        }
        return NULL;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

CComPtr<IDispatch> ActiveLedItControl::mkIconElement (int iconResID)
{
    CComPtr<IDispatch> item;
    item.Attach (OLE_MakeIconButtonToolbarItem ());
    CComQIPtr<IALIconButtonToolbarElement> newIconButton = item;

    CComPtr<IDispatch> picture;
    {
        PICTDESC pictDesc;
        ::memset (&pictDesc, 0, sizeof (pictDesc));
        pictDesc.cbSizeofstruct = sizeof (pictDesc);
        pictDesc.picType        = PICTYPE_ICON;
        pictDesc.icon.hicon     = reinterpret_cast<HICON> (::LoadImage (AfxGetInstanceHandle (), MAKEINTRESOURCE (iconResID), IMAGE_ICON, 0, 0, 0));
        Led_ThrowIfErrorHRESULT (::OleCreatePictureIndirect (&pictDesc, IID_IDispatch, true, (void**)&picture));
    }

    Led_ThrowIfErrorHRESULT (newIconButton->put_ButtonImage (picture));
    return item;
}

CComPtr<IDispatch> ActiveLedItControl::mkIconElement (const ToolBarIconSpec& s)
{
    CComPtr<IDispatch>                     item       = mkIconElement (s.fIconResId);
    CComQIPtr<IALIconButtonToolbarElement> iconButton = item;

    Led_ThrowIfErrorHRESULT (iconButton->put_Command (CComVariant (s.fCmdName)));
    Led_ThrowIfErrorHRESULT (iconButton->put_ButtonStyle (s.fButtonStyle));
    return item;
}

CComPtr<IDispatch> ActiveLedItControl::mkIconElement (int iconResID, CComPtr<IDispatch> cmdList)
{
    CComPtr<IDispatch>                     item       = mkIconElement (iconResID);
    CComQIPtr<IALIconButtonToolbarElement> iconButton = item;

    Led_ThrowIfErrorHRESULT (iconButton->put_Command (CComVariant (cmdList)));
    return item;
}

CComPtr<IDispatch> ActiveLedItControl::MakeBuiltinComboBoxToolbarItem (CComPtr<IDispatch> cmdList)
{
    try {
        CComObject<ActiveLedIt_ComboBoxToolbarElement>* o = NULL;
        Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_ComboBoxToolbarElement>::CreateInstance (&o));
        CComQIPtr<IDispatch>                 result     = o->GetUnknown ();
        CComQIPtr<IALComboBoxToolbarElement> alcomboBox = result;
        Led_ThrowIfErrorHRESULT (alcomboBox->put_CommandList (cmdList));
        return result;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

VARIANT ActiveLedItControl::OLE_GetCurrentEventArguments ()
{
    VARIANT result;
    ::VariantInit (&result);
    {
        result.vt       = VT_DISPATCH;
        result.pdispVal = fCurrentEventArguments;
        if (result.pdispVal != NULL) {
            result.pdispVal->AddRef ();
        }
    }
    return result;
}

long ActiveLedItControl::GetSelStart ()
{
    return fEditor.GetSelectionStart ();
}

void ActiveLedItControl::SetSelStart (long start)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    size_t s;
    size_t e;
    fEditor.GetSelection (&s, &e);
    size_t l        = e - s;
    size_t bufLen   = fEditor.GetLength ();
    size_t newStart = min (static_cast<size_t> (start), bufLen);
    size_t newEnd   = min (newStart + l, bufLen);
    fEditor.SetSelection (newStart, newEnd);
}

long ActiveLedItControl::GetSelLength ()
{
    size_t s;
    size_t e;
    fEditor.GetSelection (&s, &e);
    return e - s;
}

void ActiveLedItControl::SetSelLength (long length)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    if (length == -1) {
        length = INT_MAX;
    }
    if (length < 0) {
        length = 0;
    }
    size_t                  s;
    [[maybe_unused]] size_t e;
    fEditor.GetSelection (&s, &e);
    size_t bufLen = fEditor.GetLength ();
    size_t newEnd = min (s + static_cast<size_t> (length), bufLen);
    fEditor.SetSelection (s, newEnd);
}

BSTR ActiveLedItControl::GetSelText ()
{
    try {
        size_t s;
        size_t e;
        fEditor.GetSelection (&s, &e);
        size_t                              len = e - s;
        Memory::SmallStackBuffer<Led_tChar> buf (len + 1);
        fEditor.CopyOut (s, len, buf);
        buf[len] = '\0';
        return CString (buf).AllocSysString ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false); /*NOTREACHED*/
    return NULL;
}

void ActiveLedItControl::SetSelText (LPCTSTR text)
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    try {
        size_t                              len = ::_tcslen (text);
        Memory::SmallStackBuffer<Led_tChar> buf (len + 1);
        len = Characters::NativeToNL<Led_tChar> (Led_SDKString2tString (text).c_str (), len, buf, len + 1);
        size_t s;
        size_t e;
        fEditor.GetSelection (&s, &e);
        fEditor.Replace (s, e, buf, len);
        if (s != e) {
            fEditor.SetSelection (s, s + len);
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

BSTR ActiveLedItControl::GetSelTextAsRTF ()
{
    try {
        WordProcessor::WordProcessorTextIOSrcStream source (&fEditor, fEditor.GetSelectionStart (), fEditor.GetSelectionEnd ());
        StyledTextIOWriterSinkStream_Memory         sink;
        StyledTextIOWriter_RTF                      textWriter (&source, &sink);
        textWriter.Write ();
        size_t                         len = sink.GetLength ();
        Memory::SmallStackBuffer<char> buf (len + 1);
        ::memcpy (buf, sink.PeekAtData (), len);
        buf[len] = '\0';
        return CString (buf).AllocSysString ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false); /*NOTREACHED*/
    return NULL;
}

void ActiveLedItControl::SetSelTextAsRTF (LPCTSTR text)
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    try {
        IdleManager::NonIdleContext nonIdleContext;
        fCommandHandler.Commit ();
        StyledTextIOSrcStream_Memory                 source (text, text == NULL ? 0 : ::_tcslen (text));
        WordProcessor::WordProcessorTextIOSinkStream sink (&fEditor, fEditor.GetSelectionStart ());
        StyledTextIOReader_RTF                       textReader (&source, &sink);
        fEditor.Replace (fEditor.GetSelectionStart (), fEditor.GetSelectionEnd (), LED_TCHAR_OF (""), 0);
        textReader.Read ();
        fEditor.SetEmptySelectionStyle ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();

    // SHOULD FIX THIS CODE TO ASSURE OUTPUTTED TEXT REMAINS SELECTED!!!
}

BSTR ActiveLedItControl::GetSelTextAsHTML ()
{
    try {
        WordProcessor::WordProcessorTextIOSrcStream source (&fEditor, fEditor.GetSelectionStart (), fEditor.GetSelectionEnd ());
        StyledTextIOWriterSinkStream_Memory         sink;
        StyledTextIOWriter_HTML                     textWriter (&source, &sink);
        textWriter.Write ();
        size_t                         len = sink.GetLength ();
        Memory::SmallStackBuffer<char> buf (len + 1);
        ::memcpy (buf, sink.PeekAtData (), len);
        buf[len] = '\0';
        return CString (buf).AllocSysString ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false);
    return 0; // NOTREACHED
}

void ActiveLedItControl::SetSelTextAsHTML (LPCTSTR text)
{
    CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (fEditor.GetHWND ());
    try {
        IdleManager::NonIdleContext nonIdleContext;
        fCommandHandler.Commit ();
        StyledTextIOSrcStream_Memory                 source (text, text == NULL ? 0 : ::_tcslen (text));
        WordProcessor::WordProcessorTextIOSinkStream sink (&fEditor, fEditor.GetSelectionStart ());
        StyledTextIOReader_HTML                      textReader (&source, &sink);
        fEditor.Replace (fEditor.GetSelectionStart (), fEditor.GetSelectionEnd (), LED_TCHAR_OF (""), 0);
        textReader.Read ();
        fEditor.SetEmptySelectionStyle ();
        // SHOULD FIX THIS CODE TO ASSURE OUTPUTTED TEXT REMAINS SELECTED!!!
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

OLE_COLOR ActiveLedItControl::GetSelColor ()
{
    try {
        Assert (fEditor.GetSelectionEnd () >= fEditor.GetSelectionStart ());
        size_t                           selectionLength = fEditor.GetSelectionEnd () - fEditor.GetSelectionStart ();
        Led_IncrementalFontSpecification fsp             = fEditor.GetContinuousStyleInfo (fEditor.GetSelectionStart (), selectionLength);
        //HOW DO YOU SAY RETURNS ERROR??? LIKE YOU CAN WITH ATL???
        if (not fsp.GetTextColor_Valid ()) {
            //HACK!!!
            fsp = fEditor.GetContinuousStyleInfo (fEditor.GetSelectionStart (), 0);
            Assert (fsp.GetTextColor_Valid ());
        }
        return fsp.GetTextColor ().GetOSRep ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false);
    return 0; // NOTREACHED
}

void ActiveLedItControl::SetSelColor (OLE_COLOR color)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        IdleManager::NonIdleContext nonIdleContext;
        fCommandHandler.Commit ();
        Led_IncrementalFontSpecification applyFontSpec;
        applyFontSpec.SetTextColor (Led_Color (TranslateColor (color)));
        fEditor.InteractiveSetFont (applyFontSpec);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

BSTR ActiveLedItControl::GetSelFontFace ()
{
    try {
        Assert (fEditor.GetSelectionEnd () >= fEditor.GetSelectionStart ());
        size_t                           selectionLength = fEditor.GetSelectionEnd () - fEditor.GetSelectionStart ();
        Led_IncrementalFontSpecification fsp             = fEditor.GetContinuousStyleInfo (fEditor.GetSelectionStart (), selectionLength);
        if (not fsp.GetFontNameSpecifier_Valid ()) {
            return CString ().AllocSysString ();
        }
        return CString (fsp.GetFontName ().c_str ()).AllocSysString ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false);
    return 0; // NOTREACHED
}

void ActiveLedItControl::SetSelFontFace (LPCTSTR fontFace)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        IdleManager::NonIdleContext nonIdleContext;
        fCommandHandler.Commit ();
        Led_IncrementalFontSpecification applyFontSpec;
        applyFontSpec.SetFontName (fontFace);
        fEditor.InteractiveSetFont (applyFontSpec);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

long ActiveLedItControl::GetSelFontSize ()
{
    try {
        Assert (fEditor.GetSelectionEnd () >= fEditor.GetSelectionStart ());
        size_t                           selectionLength = fEditor.GetSelectionEnd () - fEditor.GetSelectionStart ();
        Led_IncrementalFontSpecification fsp             = fEditor.GetContinuousStyleInfo (fEditor.GetSelectionStart (), selectionLength);
        if (not fsp.GetPointSize_Valid ()) {
            return 0;
        }
        return fsp.GetPointSize ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false);
    return 0; // NOTREACHED
}

void ActiveLedItControl::SetSelFontSize (long size)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        IdleManager::NonIdleContext nonIdleContext;
        fCommandHandler.Commit ();
        Led_IncrementalFontSpecification applyFontSpec;
        if (size <= 0) {
            size = 2; // minsize?
        }
        if (size >= 128) {
            size = 128;
        }
        applyFontSpec.SetPointSize (static_cast<unsigned short> (size));
        fEditor.InteractiveSetFont (applyFontSpec);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

long ActiveLedItControl::GetSelBold ()
{
    try {
        size_t selStart;
        size_t selEnd;
        fEditor.GetSelection (&selStart, &selEnd);
        Assert (selStart <= selEnd);
        size_t                           selectionLength = selEnd - selStart;
        Led_IncrementalFontSpecification fsp             = fEditor.GetContinuousStyleInfo (selStart, selectionLength);
        if (not fsp.GetStyle_Bold_Valid ()) {
            return 2;
        }
        return fsp.GetStyle_Bold () ? 1 : 0;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false);
    return 0; // NOTREACHED
}

void ActiveLedItControl::SetSelBold (long bold)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        IdleManager::NonIdleContext nonIdleContext;
        fCommandHandler.Commit ();
        Led_IncrementalFontSpecification applyFontSpec;
        applyFontSpec.SetStyle_Bold (!!bold);
        fEditor.InteractiveSetFont (applyFontSpec);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

long ActiveLedItControl::GetSelItalic ()
{
    try {
        size_t selStart;
        size_t selEnd;
        fEditor.GetSelection (&selStart, &selEnd);
        Assert (selStart <= selEnd);
        size_t                           selectionLength = selEnd - selStart;
        Led_IncrementalFontSpecification fsp             = fEditor.GetContinuousStyleInfo (selStart, selectionLength);
        if (not fsp.GetStyle_Italic_Valid ()) {
            return 2;
        }
        return fsp.GetStyle_Italic () ? 1 : 0;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false);
    return 0; // NOTREACHED
}

void ActiveLedItControl::SetSelItalic (long italic)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        IdleManager::NonIdleContext nonIdleContext;
        fCommandHandler.Commit ();
        Led_IncrementalFontSpecification applyFontSpec;
        applyFontSpec.SetStyle_Italic (!!italic);
        fEditor.InteractiveSetFont (applyFontSpec);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

long ActiveLedItControl::GetSelStrikeThru ()
{
    try {
        size_t selStart;
        size_t selEnd;
        fEditor.GetSelection (&selStart, &selEnd);
        Assert (selStart <= selEnd);
        size_t                           selectionLength = selEnd - selStart;
        Led_IncrementalFontSpecification fsp             = fEditor.GetContinuousStyleInfo (selStart, selectionLength);
        if (not fsp.GetStyle_Strikeout_Valid ()) {
            return 2;
        }
        return fsp.GetStyle_Strikeout () ? 1 : 0;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false);
    return 0; // NOTREACHED
}

void ActiveLedItControl::SetSelStrikeThru (long strikeThru)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        IdleManager::NonIdleContext nonIdleContext;
        fCommandHandler.Commit ();
        Led_IncrementalFontSpecification applyFontSpec;
        applyFontSpec.SetStyle_Strikeout (!!strikeThru);
        fEditor.InteractiveSetFont (applyFontSpec);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

long ActiveLedItControl::GetSelUnderline ()
{
    try {
        size_t selStart;
        size_t selEnd;
        fEditor.GetSelection (&selStart, &selEnd);
        Assert (selStart <= selEnd);
        size_t                           selectionLength = selEnd - selStart;
        Led_IncrementalFontSpecification fsp             = fEditor.GetContinuousStyleInfo (selStart, selectionLength);
        if (not fsp.GetStyle_Underline_Valid ()) {
            return 2;
        }
        return fsp.GetStyle_Underline () ? 1 : 0;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false);
    return 0; // NOTREACHED
}

void ActiveLedItControl::SetSelUnderline (long underline)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        IdleManager::NonIdleContext nonIdleContext;
        fCommandHandler.Commit ();
        Led_IncrementalFontSpecification applyFontSpec;
        applyFontSpec.SetStyle_Underline (!!underline);
        fEditor.InteractiveSetFont (applyFontSpec);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

UINT ActiveLedItControl::OLE_GetSelJustification ()
{
    try {
        Led_Justification justification = eLeftJustify;
        size_t            selStart;
        size_t            selEnd;
        fEditor.GetSelection (&selStart, &selEnd);
        Assert (selStart <= selEnd);
        if (fEditor.GetJustification (selStart, selEnd, &justification)) {
            switch (justification) {
                case eLeftJustify:
                    return eLeftJustification;
                case eRightJustify:
                    return eRightJustification;
                case eCenterJustify:
                    return eCenterJustification;
                case eFullyJustify:
                    return eFullJustification;
                default:
                    return eLeftJustification; // what should we return here?
            }
        }
        else {
            return eNoCommonJustification;
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false);
    return 0; // NOTREACHED
}

void ActiveLedItControl::OLE_SetSelJustification (UINT justification)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        IdleManager::NonIdleContext nonIdleContext;
        Led_Justification           lh = eLeftJustify;
        switch (justification) {
            case eLeftJustification:
                lh = eLeftJustify;
                break;
            case eCenterJustification:
                lh = eCenterJustify;
                break;
            case eRightJustification:
                lh = eRightJustify;
                break;
            case eFullJustification:
                lh = eFullyJustify;
                break;
            default:
                return; // should be an error - but no error reporting supported here- should return E_INVALIDARG;
        }
        fEditor.InteractiveSetJustification (lh);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

UINT ActiveLedItControl::OLE_GetSelListStyle ()
{
    try {
        Led::ListStyle listStyle = Led::eListStyle_None;
        size_t         selStart;
        size_t         selEnd;
        fEditor.GetSelection (&selStart, &selEnd);
        Assert (selStart <= selEnd);
        if (fEditor.GetListStyle (selStart, selEnd, &listStyle)) {
            // take advantage of (assumption) that OLE list style enum is the same as the Led
            // one... NB: cast REALLY not necessary as these are all cast to 'int' - but done
            // for clarity that we are really intionally mixing enum types here - LGP 2003-06-03
            return static_cast<::ListStyle> (listStyle);
        }
        else {
            return static_cast<UINT> (eNoCommonListStyle);
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    Assert (false);
    return 0; // NOTREACHED
}

void ActiveLedItControl::OLE_SetSelListStyle (UINT listStyle)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        IdleManager::NonIdleContext nonIdleContext;
        ::ListStyle                 ls1 = static_cast<::ListStyle> (listStyle);
        Led::ListStyle              ls2 = static_cast<Led::ListStyle> (ls1); // take advantage of assumption that Led list style
        // enum is idential to one used in OLE API
        fEditor.InteractiveSetListStyle (ls2);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}

UINT ActiveLedItControl::OLE_GetSelHidable ()
{
    try {
        size_t selStart;
        size_t selEnd;
        fEditor.GetSelection (&selStart, &selEnd);
        Assert (selStart <= selEnd);
        if (fEditor.GetHidableTextDatabase ()->GetHidableRegionsContiguous (selStart, selEnd, true)) {
            return true;
        }
        else if (fEditor.GetHidableTextDatabase ()->GetHidableRegionsContiguous (selStart, selEnd, false)) {
            return false;
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
    return 2;
}

void ActiveLedItControl::OLE_SetSelHidable (UINT hidable)
{
    CHECK_DEMO_AND_BEEP_AND_RETURN ();
    try {
        IdleManager::NonIdleContext nonIdleContext;
        size_t                      selStart;
        size_t                      selEnd;
        fEditor.GetSelection (&selStart, &selEnd);
        Assert (selStart <= selEnd);
        if (hidable) {
            fEditor.GetHidableTextDatabase ()->MakeRegionHidable (selStart, selEnd);
        }
        else {
            fEditor.GetHidableTextDatabase ()->MakeRegionUnHidable (selStart, selEnd);
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ();
}
