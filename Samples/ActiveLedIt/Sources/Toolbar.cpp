/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

DISABLE_COMPILER_MSC_WARNING_START (5054)
#include <afxctl.h>
DISABLE_COMPILER_MSC_WARNING_END (5054)

#include <atlbase.h>

#include "DispIDs.h"

#include "Toolbar.h"

using Stroika::Foundation::Characters::String;

namespace {
    template <class EnumType, class CollType>
    HRESULT CreateSTLEnumerator (IUnknown** ppUnk, IUnknown* pUnkForRelease, CollType& collection)
    {
        if (ppUnk == NULL)
            return E_POINTER;
        *ppUnk = NULL;

        CComObject<EnumType>* pEnum = NULL;
        HRESULT               hr    = CComObject<EnumType>::CreateInstance (&pEnum);

        if (FAILED (hr))
            return hr;

        hr = pEnum->Init (pUnkForRelease, collection);

        if (SUCCEEDED (hr))
            hr = pEnum->QueryInterface (ppUnk);

        if (FAILED (hr))
            delete pEnum;

        return hr;
    }
}

namespace {
    size_t DoFindIndex (vector<CComPtr<IDispatch>>* list, VARIANT eltIntNameOrObj)
    {
        CComVariant e2r (eltIntNameOrObj);
        if (SUCCEEDED (e2r.ChangeType (VT_UI4))) {
            size_t idx = e2r.ulVal;
            if (idx >= list->size ()) {
                ThrowIfErrorHRESULT (DISP_E_MEMBERNOTFOUND);
            }
            return idx;
        }
        else if (SUCCEEDED (e2r.ChangeType (VT_DISPATCH))) {
            for (vector<CComPtr<IDispatch>>::iterator i = list->begin (); i != list->end (); ++i) {
                if ((*i).p == e2r.pdispVal) {
                    return i - list->begin ();
                }
            }
        }
        return kBadIndex;
    }
}

namespace {
    // Used as an implemenation detail for CComEnumOnSTL<> invocation
    struct STL_ATL_COPY_VARIANT_IDISPATCH {
        static HRESULT copy (VARIANT* p1, const ATL::CComPtr<IDispatch>* p2)
        {
            if (p2 != NULL and *p2 != NULL) {
                p1->vt         = VT_DISPATCH;
                p1->pdispVal   = *p2;
                IDispatch* pp2 = *p2;
                pp2->AddRef ();
                return S_OK;
            }
            else {
                p1->vt = VT_EMPTY;
                return E_FAIL;
            }
        }
        static void init (VARIANT* p)
        {
            p->vt = VT_EMPTY;
        }
        static void destroy (VARIANT* p)
        {
            ::VariantClear (p);
        }
    };
}

namespace {
    void DoEnableDisableChecksEtcsOnMenu (CComPtr<IDispatch> al, HMENU menu)
    {
        UINT itemCount = ::GetMenuItemCount (menu);
        for (UINT i = 0; i < itemCount; ++i) {
            UINT itemID = ::GetMenuItemID (menu, i);
            if (itemID == -1) {
                // see if a popup menu - and recurse
                MENUITEMINFO menuItemInfo;
                memset (&menuItemInfo, 0, sizeof (menuItemInfo));
                menuItemInfo.cbSize = sizeof (menuItemInfo);
                Verify (::GetMenuItemInfo (menu, i, true, &menuItemInfo));
                if (menuItemInfo.hSubMenu != NULL) {
                    DoEnableDisableChecksEtcsOnMenu (al, menuItemInfo.hSubMenu);
                }
            }
            else {
                wstring     cmdName = CmdNum2Name (itemID);
                CComVariant result;
                CComVariant cmdNameCCV = cmdName.c_str ();
                ThrowIfErrorHRESULT (al.Invoke1 (DISPID_CommandEnabled, &cmdNameCCV, &result));
                ThrowIfErrorHRESULT (result.ChangeType (VT_BOOL));
                ::EnableMenuItem (menu, i, MF_BYPOSITION | (result.boolVal ? MF_ENABLED : MF_GRAYED));

                cmdNameCCV = cmdName.c_str ();
                ThrowIfErrorHRESULT (al.Invoke1 (DISPID_CommandChecked, &cmdNameCCV, &result));
                ThrowIfErrorHRESULT (result.ChangeType (VT_BOOL));
                ::CheckMenuItem (menu, i, MF_BYPOSITION | (result.boolVal ? MF_CHECKED : MF_UNCHECKED));
            }
        }
    }
}

/*
 ********************************************************************************
 ******************* ActiveLedIt_IconButtonToolbarElement ***********************
 ********************************************************************************
 */
ActiveLedIt_IconButtonToolbarElement::ActiveLedIt_IconButtonToolbarElement ()
    : fHWnd (NULL)
    , fButtonImage ()
    , fPressedOnClick (false)
    , fCommand ()
    , fIconButtonStyle (eIconButton_PushButton)
    , fOwningToolbar (NULL)
    , fOwningActiveLedIt (NULL)
    , fBounds ()
{
}

ActiveLedIt_IconButtonToolbarElement::~ActiveLedIt_IconButtonToolbarElement ()
{
}

HRESULT ActiveLedIt_IconButtonToolbarElement::FinalConstruct ()
{
    return S_OK;
}

void ActiveLedIt_IconButtonToolbarElement::FinalRelease ()
{
}

LRESULT ActiveLedIt_IconButtonToolbarElement::OnLButtonDown ([[maybe_unused]] UINT uMsg, [[maybe_unused]] WPARAM wParam,
                                                             [[maybe_unused]] LPARAM lParam, BOOL& bHandled)
{
    bHandled = true;
    try {
        switch (fIconButtonStyle) {
            case eIconButton_Sticky:
            case eIconButton_Toggle: {
                if (IsWindowEnabled ()) {
                    bool wasPushed  = !!(SendMessage (BM_GETSTATE) & BST_PUSHED);
                    fPressedOnClick = wasPushed;
                    if (fIconButtonStyle == eIconButton_Sticky) {
                        SendMessage (BM_SETSTATE, BST_PUSHED);
                    }
                    else {
                        SendMessage (BM_SETSTATE, wasPushed ? 0 : BST_PUSHED);
                    }
                    SendMessage (GetParent (), WM_COMMAND, MAKEWPARAM (GetWindowLong (GWL_ID), BN_CLICKED), reinterpret_cast<LPARAM> (m_hWnd));
                }
                return true;
            }

            case eIconButton_Popup: {
                CComPtr<IDispatch>           oal = fOwningActiveLedIt;
                CComPtr<IALAcceleratorTable> accelerators;
                {
                    CComVariant accel;
                    ThrowIfErrorHRESULT (oal.GetProperty (DISPID_AcceleratorTable, &accel));
                    if (SUCCEEDED (accel.ChangeType (VT_DISPATCH))) {
                        accelerators = CComQIPtr<IALAcceleratorTable> (accel.pdispVal);
                    }
                }
                if (SUCCEEDED (fCommand.ChangeType (VT_DISPATCH))) {
                    CComQIPtr<IALCommandList> cm = fCommand.pdispVal;
                    if (cm != NULL) {
                        HMENU mH = NULL;
                        if (SUCCEEDED (cm->GeneratePopupMenu (accelerators, &mH))) {
                            try {
                                RECT wr;
                                GetWindowRect (&wr);
                                DoEnableDisableChecksEtcsOnMenu (oal, mH);
                                int cmdNum =
                                    ::TrackPopupMenu (mH, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, wr.left, wr.bottom, 0, m_hWnd, NULL);
                                if (cmdNum != 0) {
                                    wstring cmdName = CmdNum2Name (cmdNum);
                                    if (cmdName.length () != 0) {
                                        CComVariant cmdNameCCV = cmdName.c_str ();
                                        ThrowIfErrorHRESULT (oal.Invoke1 (DISPID_InvokeCommand, &cmdNameCCV));
                                    }
                                }
                                ::DestroyMenu (mH);
                            }
                            catch (...) {
                                ::DestroyMenu (mH);
                                throw;
                            }
                        }
                    }
                }
                return true;
            }

            default: {
                return DefWindowProc ();
            }
        }
    }
    catch (...) {
        // ignore exceptions doing mousedown processing
        return true;
    }
}

LRESULT ActiveLedIt_IconButtonToolbarElement::OnLButtonUp ([[maybe_unused]] UINT uMsg, [[maybe_unused]] WPARAM wParam,
                                                           [[maybe_unused]] LPARAM lParam, BOOL& bHandled)
{
    /*
     *  Pushing on a button causes it to grab the focus (cannot figure out how to stop this - but its not SO bad).
     *  Instead - just reset the focus back to the OCX when the click is finihsed.
     */
    LRESULT lr = DefWindowProc ();
    try {
        CComPtr<IDispatch> oal = fOwningActiveLedIt;
        CComVariant        res;
        ThrowIfErrorHRESULT (oal.GetProperty (DISPID_HWND, &res));
        ThrowIfErrorHRESULT (res.ChangeType (VT_UINT));
        HWND controlHWND = reinterpret_cast<HWND> (res.iVal);
        ::SetFocus (controlHWND);
    }
    catch (...) {
        // ignore any errors here.
    }
    bHandled = true;
    return lr;
}

LRESULT ActiveLedIt_IconButtonToolbarElement::OnMouseMove ([[maybe_unused]] UINT uMsg, [[maybe_unused]] WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (GetCapture () == m_hWnd and fIconButtonStyle == eIconButton_PushButton) {
#define MY_GET_X_LPARAM(lp) ((int)(short)LOWORD (lp))
#define MY_GET_Y_LPARAM(lp) ((int)(short)HIWORD (lp))
        RECT cr;
        GetClientRect (&cr);
        bool clickedInside = (AsLedRect (cr).Contains (Led_Point (MY_GET_Y_LPARAM (lParam), MY_GET_X_LPARAM (lParam))));
        bool newPressed    = clickedInside ? not fPressedOnClick : fPressedOnClick;
        SendMessage (BM_SETSTATE, newPressed ? BST_PUSHED : 0);
        bHandled = true;
        return DefWindowProc ();
#undef MY_GET_X_LPARAM
#undef MY_GET_Y_LPARAM
    }
    return 0;
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::get_PreferredHeight (UINT* pVal)
{
    const DistanceType kBorderSize = 3;
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        if (fButtonImage.p == NULL) {
            *pVal = kBorderSize * 2;
            return S_OK;
        }
        OLE_XSIZE_HIMETRIC height = 0;
        ThrowIfErrorHRESULT (fButtonImage->get_Height (&height));
        SIZEL hmSize;
        hmSize.cx = 0;
        hmSize.cy = height;
        SIZEL pixSize;
        ::memset (&pixSize, 0, sizeof (pixSize));
        AtlHiMetricToPixel (&hmSize, &pixSize);
        *pVal = pixSize.cy + 2 * kBorderSize;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::get_PreferredWidth (UINT* pVal)
{
    const DistanceType kBorderSize = 3;
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        if (fButtonImage.p == NULL) {
            *pVal = kBorderSize * 2;
            return S_OK;
        }
        OLE_XSIZE_HIMETRIC width = 0;
        ThrowIfErrorHRESULT (fButtonImage->get_Width (&width));
        SIZEL hmSize;
        hmSize.cx = width;
        hmSize.cy = 0;
        SIZEL pixSize;
        ::memset (&pixSize, 0, sizeof (pixSize));
        AtlHiMetricToPixel (&hmSize, &pixSize);
        *pVal = pixSize.cx + kBorderSize * 2;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::get_X (INT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fBounds.left;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::get_Y (INT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fBounds.top;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::get_Width (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fBounds.GetWidth ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::get_Height (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fBounds.GetHeight ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::SetRectangle (int X, int Y, UINT width, UINT height)
{
    try {
        if (m_hWnd != NULL) {
            fBounds = Led_Rect (Y, X, height, width);
            MoveWindow (X, Y, width, height);
        }
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::NotifyOfOwningToolbar (IDispatch* owningToolbar, IDispatch* owningActiveLedIt)
{
    try {
        if (fOwningToolbar != owningToolbar or fOwningActiveLedIt != owningActiveLedIt) {
            if (m_hWnd != NULL) { // maybe already destroyed by parent window destruction
                DestroyWindow ();
            }

            fOwningToolbar     = owningToolbar;
            fOwningActiveLedIt = owningActiveLedIt;

            if (fOwningToolbar != NULL) {
                RECT r          = AsRECT (fBounds);
                HWND parentHWND = 0;
                {
                    CComQIPtr<IALToolbar> otb = fOwningToolbar;
                    ThrowIfErrorHRESULT (otb->get_hWnd (&parentHWND));
                }

                int itemID = 0;
                Create (parentHWND, &r, NULL, WS_CHILD | WS_VISIBLE, 0, itemID);
                UpdateButtonObj ();
            }
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::UpdateEnableState ()
{
    try {
        if (m_hWnd != NULL) {
            bool enabled = false;
            if (fOwningActiveLedIt != NULL) {
                CComPtr<IDispatch> al = fOwningActiveLedIt;
                if (fIconButtonStyle == eIconButton_Popup) {
                    enabled = true; // even if no items enabled - still allow show of menus (kind of like in a menu bar)
                }
                else {
                    CComVariant result;
                    CComVariant cmdCCV = fCommand;
                    ThrowIfErrorHRESULT (al.Invoke1 (DISPID_CommandEnabled, &cmdCCV, &result));
                    ThrowIfErrorHRESULT (result.ChangeType (VT_BOOL));
                    enabled = !!result.boolVal;
                }
            }
            EnableWindow (enabled);
            if (fIconButtonStyle == eIconButton_Toggle or fIconButtonStyle == eIconButton_Sticky) {
                bool checked = false;
                if (fOwningActiveLedIt != NULL) {
                    CComPtr<IDispatch> al = fOwningActiveLedIt;
                    CComVariant        result;
                    CComVariant        commandCCV = fCommand;
                    ThrowIfErrorHRESULT (al.Invoke1 (DISPID_CommandChecked, &commandCCV, &result));
                    ThrowIfErrorHRESULT (result.ChangeType (VT_BOOL));
                    checked = !!result.boolVal;
                }
                SendMessage (BM_SETSTATE, checked ? BST_PUSHED : 0);
            }
        }
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::get_ButtonImage (IDispatch** pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = CComQIPtr<IDispatch> (fButtonImage);
        if (*pVal != NULL) {
            (*pVal)->AddRef ();
        }
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::put_ButtonImage (IDispatch* val)
{
    try {
        fButtonImage = val;
        if constexpr (qDebug) {
            if (fButtonImage.p == NULL and val != NULL) {
                DumpSupportedInterfaces (val, "SUP INTERFACES FOR JSCRIPT IMAGE");
            }
        }
        UpdateButtonObj ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::get_Command (VARIANT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        CComVariant{fCommand}.Detach (pVal);
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::put_Command (VARIANT val)
{
    try {
        fCommand = CComVariant{val};
        CheckFixButtonStyle ();
        UpdateButtonObj ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::get_ButtonStyle (IconButtonStyle* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fIconButtonStyle;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::put_ButtonStyle (IconButtonStyle val)
{
    try {
        fIconButtonStyle = val;
        CheckFixButtonStyle ();
        UpdateButtonObj ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

void ActiveLedIt_IconButtonToolbarElement::UpdateButtonObj ()
{
    if (m_hWnd != NULL) {
        if (fButtonImage.p == NULL) {
            (void)SendMessage (BM_SETIMAGE, IMAGE_ICON, NULL);
            (void)SendMessage (BM_SETIMAGE, IMAGE_BITMAP, NULL);
        }
        else {
            short pictType = 0;
            ThrowIfErrorHRESULT (fButtonImage->get_Type (&pictType));
            OLE_HANDLE pictHandle = NULL;
            ThrowIfErrorHRESULT (fButtonImage->get_Handle (&pictHandle));
            switch (pictType) {
                case PICTYPE_BITMAP: {
                    SetWindowLong (GWL_STYLE, (GetWindowLong (GWL_STYLE) & ~BS_ICON) | BS_BITMAP);
                    (void)SendMessage (BM_SETIMAGE, IMAGE_BITMAP, pictHandle);
                } break;
                case PICTYPE_ICON: {
                    SetWindowLong (GWL_STYLE, (GetWindowLong (GWL_STYLE) & ~BS_BITMAP) | BS_ICON);
                    (void)SendMessage (BM_SETIMAGE, IMAGE_ICON, pictHandle);
                } break;
                default: {
                    (void)SendMessage (BM_SETIMAGE, IMAGE_ICON, NULL);
                } break;
            }
        }

        SetWindowLong (GWL_ID, CmdObjOrName2Num (fCommand));
    }
}

void ActiveLedIt_IconButtonToolbarElement::CheckFixButtonStyle ()
{
    CComVariant c = fCommand;
    if (SUCCEEDED (c.ChangeType (VT_DISPATCH))) {
        CComQIPtr<IALCommandList> alc = c.pdispVal;
        if (alc.p == NULL) {
            if (fIconButtonStyle == eIconButton_Popup) {
                fIconButtonStyle = eIconButton_PushButton;
            }
        }
        else {
            fIconButtonStyle = eIconButton_Popup;
        }
    }
    else {
        if (fIconButtonStyle == eIconButton_Popup) {
            fIconButtonStyle = eIconButton_PushButton;
        }
    }
}

/*
     ********************************************************************************
     ********************* ActiveLedIt_ComboBoxToolbarElement ***********************
     ********************************************************************************
     */
ActiveLedIt_ComboBoxToolbarElement::ActiveLedIt_ComboBoxToolbarElement ()
    : fDropDownActive (false)
    , fHWnd (NULL)
    , fComboBox ()
    , fPreferredWidth (100)
    , fPressedOnClick (false)
    , fCommandList ()
    , fCommandListCache ()
    , fOwningToolbar (NULL)
    , fOwningActiveLedIt (NULL)
    , fBounds ()
{
}

ActiveLedIt_ComboBoxToolbarElement::~ActiveLedIt_ComboBoxToolbarElement ()
{
}

HRESULT ActiveLedIt_ComboBoxToolbarElement::FinalConstruct ()
{
    return S_OK;
}

void ActiveLedIt_ComboBoxToolbarElement::FinalRelease ()
{
}

LRESULT ActiveLedIt_ComboBoxToolbarElement::OnCBDropDown ([[maybe_unused]] USHORT uMsg, [[maybe_unused]] USHORT wParam,
                                                          [[maybe_unused]] HWND ctlHandle, BOOL& bHandled)
{
    fDropDownActive = true;
    LRESULT lr      = DefWindowProc ();
    bHandled        = true;
    return lr;
}

LRESULT ActiveLedIt_ComboBoxToolbarElement::OnCBCloseUp ([[maybe_unused]] USHORT uMsg, [[maybe_unused]] USHORT wParam,
                                                         [[maybe_unused]] HWND ctlHandle, BOOL& bHandled)
{
    fDropDownActive = false;
    /*
     *  Using the combobox grabs the focus. Reset the focus back to the OCX when the click is finihsed.
     */
    LRESULT lr = DefWindowProc ();
    try {
        CComPtr<IDispatch> oal = fOwningActiveLedIt;
        CComVariant        res;
        ThrowIfErrorHRESULT (oal.GetProperty (DISPID_HWND, &res));
        ThrowIfErrorHRESULT (res.ChangeType (VT_UINT));
        HWND controlHWND = reinterpret_cast<HWND> (res.iVal);
        ::SetFocus (controlHWND);
    }
    catch (...) {
        // ignore any errors here.
    }
    bHandled = true;
    return lr;
}

LRESULT ActiveLedIt_ComboBoxToolbarElement::OnCBSelChange ([[maybe_unused]] USHORT uMsg, [[maybe_unused]] USHORT wParam,
                                                           [[maybe_unused]] HWND ctlHandle, BOOL& bHandled)
{
    bHandled   = true;
    LRESULT lr = DefWindowProc ();
    int     r  = static_cast<int> (fComboBox.SendMessage (CB_GETCURSEL, 0, 0));
    try {
        if (r >= 0 and static_cast<size_t> (r) < fCommandListCache.size ()) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
            CComQIPtr<IALCommand> alc = (IDispatch*)fCommandListCache[r];
#else
            CComQIPtr<IALCommand> alc = fCommandListCache[r];
#endif
            CComBSTR internalName;
            ThrowIfErrorHRESULT (alc->get_InternalName (&internalName));
            CComPtr<IDispatch> al              = fOwningActiveLedIt;
            CComVariant        internalNameCCV = internalName;
            ThrowIfErrorHRESULT (al.Invoke1 (DISPID_InvokeCommand, &internalNameCCV));
        }
    }
    catch (...) {
    }
    return lr;
}

STDMETHODIMP ActiveLedIt_ComboBoxToolbarElement::get_PreferredHeight (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        const DistanceType kWhiteSluff = 4;
        *pVal                          = ::GetSystemMetrics (SM_CYVSCROLL) + kWhiteSluff;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ComboBoxToolbarElement::get_PreferredWidth (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fPreferredWidth;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ComboBoxToolbarElement::get_X (INT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fBounds.left;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ComboBoxToolbarElement::get_Y (INT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fBounds.top;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ComboBoxToolbarElement::get_Width (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fBounds.GetWidth ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ComboBoxToolbarElement::get_Height (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fBounds.GetHeight ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ComboBoxToolbarElement::SetRectangle (int X, int Y, UINT width, UINT height)
{
    try {
        if (m_hWnd != NULL) {
            fBounds = Led_Rect (Y, X, height, width);
            MoveWindow (X, Y, width, height);
            RECT cr;
            GetClientRect (&cr);
            cr.bottom += 200; // fix SPR#1659- comboboxes don't popup on Win2k
            fComboBox.MoveWindow (&cr);
        }
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ComboBoxToolbarElement::NotifyOfOwningToolbar (IDispatch* owningToolbar, IDispatch* owningActiveLedIt)
{
    try {
        if (fOwningToolbar != owningToolbar or fOwningActiveLedIt != owningActiveLedIt) {
            if (m_hWnd != NULL) { // maybe already destroyed by parent window destruction
                DestroyWindow ();
            }

            fOwningToolbar     = owningToolbar;
            fOwningActiveLedIt = owningActiveLedIt;

            if (fOwningToolbar != NULL) {
                RECT r          = AsRECT (fBounds);
                HWND parentHWND = 0;
                {
                    CComQIPtr<IALToolbar> otb = fOwningToolbar;
                    ThrowIfErrorHRESULT (otb->get_hWnd (&parentHWND));
                }

                Create (parentHWND, &r, NULL, WS_CHILD | WS_VISIBLE);
                fComboBox.Create (_T("ComboBox"), m_hWnd, &r, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST);
                {
                    HFONT f = reinterpret_cast<HFONT> (::GetStockObject (DEFAULT_GUI_FONT));
                    fComboBox.SendMessage (WM_SETFONT, reinterpret_cast<LPARAM> (f), true);
                }

                UpdatePopupObj ();
            }
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_ComboBoxToolbarElement::UpdateEnableState ()
{
    try {
        if (m_hWnd != NULL and not fDropDownActive) {
            // Walk list of all subcommands - and if ANY enabled - then we are, and otherwise  we are disabled
            bool   enabled     = false;
            size_t idxSelected = kBadIndex;
            if (fOwningActiveLedIt != NULL) {
                CComPtr<IDispatch> al = fOwningActiveLedIt;
                for (vector<CComPtr<IALCommand>>::iterator i = fCommandListCache.begin (); i != fCommandListCache.end (); ++i) {
                    {
                        CComVariant result;
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
                        CComVariant alcmdCCV = (IDispatch*)*i;
#else
                        CComVariant alcmdCCV = *i;
#endif
                        ThrowIfErrorHRESULT (al.Invoke1 (DISPID_CommandEnabled, &alcmdCCV, &result));
                        ThrowIfErrorHRESULT (result.ChangeType (VT_BOOL));
                        if (result.boolVal) {
                            enabled = true;
                        }
                    }
                    {
                        CComVariant result;
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
                        CComVariant alcmdCCV = (IDispatch*)*i;
#else
                        CComVariant alcmdCCV = *i;
#endif
                        ThrowIfErrorHRESULT (al.Invoke1 (DISPID_CommandChecked, &alcmdCCV, &result));
                        ThrowIfErrorHRESULT (result.ChangeType (VT_BOOL));
                        if (result.boolVal) {
                            idxSelected = i - fCommandListCache.begin ();
                        }
                    }
                }
            }
            fComboBox.EnableWindow (enabled);
            if (idxSelected == kBadIndex) {
                (void)fComboBox.SendMessage (CB_SETCURSEL, static_cast<WPARAM> (-1), 0);
            }
            else {
                Verify (fComboBox.SendMessage (CB_SETCURSEL, static_cast<int> (idxSelected), 0) != CB_ERR);
            }
        }
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ComboBoxToolbarElement::get_CommandList (IDispatch** pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fCommandList;
        if (*pVal != NULL) {
            (*pVal)->AddRef ();
        }
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ComboBoxToolbarElement::put_CommandList (IDispatch* val)
{
    try {
        fCommandList = val;
        UpdatePopupObj ();
        CallInvalidateLayout ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

void ActiveLedIt_ComboBoxToolbarElement::UpdatePopupObj ()
{
    if (m_hWnd != NULL) {
        //  resetcoennt in combobox, and then walk list of commands and add items...
        //  Need to keep SIDE-vector of CComPtr<> guys for each elt - since the IALcommandList chould get
        //  changed behind our backs... (need some way to keep them in sync?)
        (void)fComboBox.SendMessage (CB_RESETCONTENT, 0, 0);
        fCommandListCache.clear ();
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
        CComQIPtr<IALCommandList> cmdList = (IDispatch*)fCommandList;
#else
        CComQIPtr<IALCommandList> cmdList = fCommandList;
#endif
        if (cmdList.p != NULL) {
            long cmdCount = 0;
            ThrowIfErrorHRESULT (cmdList->get_Count (&cmdCount));
            HDC     hdc     = GetWindowDC ();
            HGDIOBJ oldFont = ::SelectObject (hdc, ::GetStockObject (DEFAULT_GUI_FONT));
            try {
                DistanceType maxItemWidth = 0;
                for (long i = 0; i < cmdCount; ++i) {
                    CComPtr<IDispatch> e;
                    ThrowIfErrorHRESULT (cmdList->get_Item (i, &e));
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
                    CComQIPtr<IALCommand> alc = (IDispatch*)e;
#else
                    CComQIPtr<IALCommand> alc = e;
#endif
                    CComBSTR name;
                    ThrowIfErrorHRESULT (alc->get_Name (&name));
                    fCommandListCache.push_back (alc);
                    SDKString itemPrintName = String{wstring{name}}.AsSDKString ();
                    Verify (fComboBox.SendMessage (CB_ADDSTRING, 0, reinterpret_cast<LPARAM> (itemPrintName.c_str ())) != CB_ERR);
                    SIZE sz;
                    memset (&sz, 0, sizeof (sz));
                    ::GetTextExtentPoint32 (hdc, itemPrintName.c_str (), static_cast<int> (itemPrintName.length ()), &sz);
                    maxItemWidth = max (maxItemWidth, static_cast<DistanceType> (sz.cx));
                }
                ::SelectObject (hdc, oldFont);
                ReleaseDC (hdc);
                const DistanceType kMaxMax     = 200; // don't make any wider than this - even if really long text...
                maxItemWidth                   = min (maxItemWidth, kMaxMax);
                const DistanceType kWhiteSluff = 6;
                fPreferredWidth = 2 * ::GetSystemMetrics (SM_CXEDGE) + ::GetSystemMetrics (SM_CXVSCROLL) + maxItemWidth + kWhiteSluff;
            }
            catch (...) {
                ::SelectObject (hdc, oldFont);
                ReleaseDC (hdc);
                throw;
            }
        }
    }
}

void ActiveLedIt_ComboBoxToolbarElement::CallInvalidateLayout ()
{
    if (fOwningActiveLedIt != NULL) {
        CComPtr<IDispatch> oal = fOwningActiveLedIt;
        ThrowIfErrorHRESULT (oal.Invoke0 (DISPID_InvalidateLayout));
    }
}

/*
     ********************************************************************************
     ******************* ActiveLedIt_SeparatorToolbarElement ************************
     ********************************************************************************
     */
ActiveLedIt_SeparatorToolbarElement::ActiveLedIt_SeparatorToolbarElement ()
    : fBounds ()
{
}

ActiveLedIt_SeparatorToolbarElement::~ActiveLedIt_SeparatorToolbarElement ()
{
}

HRESULT ActiveLedIt_SeparatorToolbarElement::FinalConstruct ()
{
    return S_OK;
}

void ActiveLedIt_SeparatorToolbarElement::FinalRelease ()
{
}

STDMETHODIMP ActiveLedIt_SeparatorToolbarElement::get_PreferredHeight (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = 1;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_SeparatorToolbarElement::get_PreferredWidth (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = 6;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_SeparatorToolbarElement::get_X (INT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fBounds.left;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_SeparatorToolbarElement::get_Y (INT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fBounds.top;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_SeparatorToolbarElement::get_Width (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fBounds.GetWidth ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_SeparatorToolbarElement::get_Height (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fBounds.GetHeight ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_SeparatorToolbarElement::SetRectangle (int X, int Y, UINT width, UINT height)
{
    try {
        fBounds = Led_Rect (Y, X, height, width);
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_SeparatorToolbarElement::NotifyOfOwningToolbar (IDispatch* /*owningToolbar*/, IDispatch* /*owningActiveLedIt*/)
{
    try {
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_SeparatorToolbarElement::UpdateEnableState ()
{
    try {
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

/*
     ********************************************************************************
     ****************************** ActiveLedIt_Toolbar *****************************
     ********************************************************************************
     */
ActiveLedIt_Toolbar::ActiveLedIt_Toolbar ()
    : fToolbarItems ()
    , fOwningActiveLedIt (NULL)
    , fOwningALToolbar (NULL)
    , fBounds ()
{
}

ActiveLedIt_Toolbar::~ActiveLedIt_Toolbar ()
{
}

LRESULT ActiveLedIt_Toolbar::OnCommand ([[maybe_unused]] UINT uMsg, WPARAM wParam, [[maybe_unused]] LPARAM lParam, BOOL& bHandled)
{
    if (!HIWORD (wParam)) {
        if (fOwningActiveLedIt != NULL) {
            CComPtr<IDispatch> oal        = fOwningActiveLedIt;
            wstring            cmdName    = CmdNum2Name (LOWORD (wParam));
            CComVariant        cmdNameCCV = cmdName.c_str ();
            oal.Invoke1 (DISPID_InvokeCommand, &cmdNameCCV);
            bHandled = true;
        }
    }
    return -1;
}
#if 0
    LRESULT ActiveLedIt_Toolbar::OnPaint (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        PAINTSTRUCT ps;
        HDC         hdc     =   ::BeginPaint (m_hWnd, &ps);
        if (hdc != NULL) {
            try {
                Tablet             tablet (hdc, Tablet::eDoesntOwnDC);
                Pen                 usePen (PS_SOLID, 1, Color::kBlack.GetOSRep ());
                GDI_Obj_Selector    penSelector (&tablet, usePen);
                Led_Rect                drawRect    =   Led_Rect  (0, 0, fBounds.GetHeight (), fBounds.GetWidth ());
                if (m_hWnd != NULL) {       // probbaly not really needed anymore - now that I subtract out the SM_CXYEDGE - but what the heck... Just in case...
                    RECT    cr;
                    ::GetClientRect (m_hWnd, &cr);
                    drawRect = AsLedRect (cr);
                }
                tablet.MoveTo (Led_Point (drawRect.GetBottom () - 1, drawRect.GetLeft ()));
                tablet.LineTo (Led_Point (drawRect.GetBottom () - 1, drawRect.GetRight ()));
            }
            catch (...) {
                ::EndPaint (m_hWnd, &ps);
                throw;
            }
            ::EndPaint (m_hWnd, &ps);
        }
        return 0;
    }
#endif
void ActiveLedIt_Toolbar::CallInvalidateLayout ()
{
    if (fOwningActiveLedIt != NULL) {
        CComPtr<IDispatch> oal = fOwningActiveLedIt;
        ThrowIfErrorHRESULT (oal.Invoke0 (DISPID_InvalidateLayout));
    }
}

void ActiveLedIt_Toolbar::DoLayout ()
{
#if 1
    Led_Rect clientBounds = Led_Rect (0, 0, fBounds.GetHeight (), fBounds.GetWidth ());
#else
    Led_Rect clientBounds = Led_Rect (0, 0, max (static_cast<CoordinateType> (fBounds.GetHeight ()) - 2 * ::GetSystemMetrics (SM_CYEDGE), 0),
                                      max (static_cast<CoordinateType> (fBounds.GetHeight ()) - 2 * ::GetSystemMetrics (SM_CXEDGE), 0));
#endif

    // MAYBE NOT NEEDED ANYMORE???
    if (m_hWnd != NULL) { // probbaly not really needed anymore - now that I subtract out the SM_CXYEDGE - but what the heck... Just in case...
        RECT cr;
        ::GetClientRect (m_hWnd, &cr);
        clientBounds.bottom = max (clientBounds.bottom, cr.bottom);
    }

    Led_Rect itemBoundsCursor = Led_Rect (clientBounds.GetTop (), clientBounds.GetLeft () + kHTBEdge, clientBounds.GetHeight (), 0);

    for (vector<CComPtr<IDispatch>>::iterator i = fToolbarItems.begin (); i != fToolbarItems.end (); ++i) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
        CComQIPtr<IALToolbarElement> tbi = (IDispatch*)*i;
#else
        CComQIPtr<IALToolbarElement> tbi = *i;
#endif
        UINT preferredWidth = 0;
        ThrowIfErrorHRESULT (tbi->get_PreferredWidth (&preferredWidth));
        UINT preferredHeight = 0;
        ThrowIfErrorHRESULT (tbi->get_PreferredHeight (&preferredHeight));

        DistanceType useHeight  = min (DistanceType (preferredHeight), itemBoundsCursor.GetHeight ());
        Led_Rect     itemBounds = itemBoundsCursor;
        itemBounds.right        = itemBounds.left + preferredWidth;
        itemBoundsCursor.right  = itemBounds.right;
        itemBounds.bottom       = itemBounds.top + useHeight;
        itemBounds              = CenterRectInRect (itemBounds, itemBoundsCursor);
        tbi->SetRectangle (itemBounds.GetLeft (), itemBounds.GetTop (), itemBounds.GetWidth (), itemBounds.GetHeight ());

        itemBoundsCursor.left  = itemBoundsCursor.right + kHSluff;
        itemBoundsCursor.right = itemBoundsCursor.left;
    }
}

HRESULT ActiveLedIt_Toolbar::FinalConstruct ()
{
    IdleManager::Get ().AddEnterIdler (this);
    return S_OK;
}

void ActiveLedIt_Toolbar::FinalRelease ()
{
    IdleManager::Get ().RemoveEnterIdler (this);
}

void ActiveLedIt_Toolbar::OnEnterIdle ()
{
    try {
        for (vector<CComPtr<IDispatch>>::iterator i = fToolbarItems.begin (); i != fToolbarItems.end (); ++i) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
            CComQIPtr<IALToolbarElement> tbi = (IDispatch*)*i;
#else
            CComQIPtr<IALToolbarElement> tbi = *i;
#endif
            ThrowIfErrorHRESULT (tbi->UpdateEnableState ());
        }
    }
    catch (...) {
    }
}

STDMETHODIMP ActiveLedIt_Toolbar::get__NewEnum (IUnknown** ppUnk)
{
    using VarVarEnum = CComEnumOnSTL<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, STL_ATL_COPY_VARIANT_IDISPATCH, std::vector<CComPtr<IDispatch>>>;
    try {
        if (ppUnk == NULL) {
            return E_INVALIDARG;
        }
        return CreateSTLEnumerator<VarVarEnum> (ppUnk, this, fToolbarItems);
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_Toolbar::get_Item (long Index, IDispatch** pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        if (Index < 0 or static_cast<size_t> (Index) >= fToolbarItems.size ()) {
            return E_INVALIDARG;
        }
        *pVal = fToolbarItems[Index];
        (*pVal)->AddRef ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_Toolbar::get_Count (long* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = static_cast<long> (fToolbarItems.size ());
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_Toolbar::Add (IDispatch* newElt, UINT atIndex)
{
    try {
        if (newElt == NULL) {
            return E_INVALIDARG;
        }
        size_t idx = min (static_cast<size_t> (atIndex), fToolbarItems.size ());
        fToolbarItems.insert (fToolbarItems.begin () + idx, newElt);
        if (fOwningActiveLedIt != NULL) {
            CComQIPtr<IALToolbarElement> tbe = newElt;
            ThrowIfErrorHRESULT (tbe->NotifyOfOwningToolbar (this, fOwningActiveLedIt));
        }
        CallInvalidateLayout ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_Toolbar::MergeAdd (IDispatch* newElts, UINT afterElt)
{
    try {
        if (newElts == NULL) {
            return E_INVALIDARG;
        }
        size_t                idx       = min (static_cast<size_t> (afterElt), fToolbarItems.size ());
        CComQIPtr<IALToolbar> alt       = newElts;
        long                  nElts2Add = 0;
        ThrowIfErrorHRESULT (alt->get_Count (&nElts2Add));
        for (long i = 0; i < nElts2Add; ++i) {
            CComPtr<IDispatch> e;
            ThrowIfErrorHRESULT (alt->get_Item (i, &e));
            fToolbarItems.insert (fToolbarItems.begin () + idx, e);
            if (fOwningActiveLedIt != NULL) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
                CComQIPtr<IALToolbarElement> tbe = (IDispatch*)e;
#else
                CComQIPtr<IALToolbarElement> tbe = e;
#endif
                tbe->NotifyOfOwningToolbar (this, fOwningActiveLedIt);
            }
            ++idx;
        }
        CallInvalidateLayout ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_Toolbar::Remove (VARIANT eltIntNameOrIndex)
{
    try {
        size_t idx = DoFindIndex (&fToolbarItems, eltIntNameOrIndex);
        if (idx != kBadIndex) {
            return E_INVALIDARG;
        }
        if (fOwningActiveLedIt != NULL) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
            CComQIPtr<IALToolbarElement> tbe = (IDispatch*)fToolbarItems[idx];
#else
            CComQIPtr<IALToolbarElement> tbe = fToolbarItems[idx];
#endif
            ThrowIfErrorHRESULT (tbe->NotifyOfOwningToolbar (NULL, NULL));
        }
        fToolbarItems.erase (fToolbarItems.begin () + idx, fToolbarItems.begin () + idx + 1);
        CallInvalidateLayout ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_Toolbar::Clear ()
{
    try {
        if (fOwningActiveLedIt != NULL) {
            for (vector<CComPtr<IDispatch>>::iterator i = fToolbarItems.begin (); i != fToolbarItems.end (); ++i) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
                CComQIPtr<IALToolbarElement> tbe = (IDispatch*)*i;
#else
                CComQIPtr<IALToolbarElement> tbe = *i;
#endif
                ThrowIfErrorHRESULT (tbe->NotifyOfOwningToolbar (NULL, NULL));
            }
        }
        fToolbarItems.clear ();
        CallInvalidateLayout ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_Toolbar::get_hWnd (HWND* pVal)
{
    try {
        *pVal = m_hWnd;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_Toolbar::get_PreferredHeight (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }

        UINT maxHeight = 0;
        for (vector<CComPtr<IDispatch>>::iterator i = fToolbarItems.begin (); i != fToolbarItems.end (); ++i) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
            CComQIPtr<IALToolbarElement> tbi = (IDispatch*)*i;
#else
            CComQIPtr<IALToolbarElement> tbi = *i;
#endif
            UINT preferredHeight = 0;
            ThrowIfErrorHRESULT (tbi->get_PreferredHeight (&preferredHeight));
            maxHeight = max (maxHeight, preferredHeight);
        }

        // Use MAX preferred height of all buttons, plus room for bottom line, plus room for edges of toolbar itself
        //      *pVal = maxHeight + 2*kVInset + kRoomForBotLine + 2 * ::GetSystemMetrics (SM_CYEDGE);
        // Use MAX preferred height of all buttons, plus room for above/below vinset
        *pVal = maxHeight + 2 * kVInset;

        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_Toolbar::get_PreferredWidth (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        UINT totalPrefWidth = 0;
        for (vector<CComPtr<IDispatch>>::iterator i = fToolbarItems.begin (); i != fToolbarItems.end (); ++i) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
            CComQIPtr<IALToolbarElement> tbi = (IDispatch*)*i;
#else
            CComQIPtr<IALToolbarElement> tbi = *i;
#endif
            UINT preferredWidth = 0;
            ThrowIfErrorHRESULT (tbi->get_PreferredWidth (&preferredWidth));
            totalPrefWidth += preferredWidth;
        }

        // Use total preferred width of all buttons, plus room for bottom line, plus room for edges of toolbar itself
        *pVal = static_cast<UINT> (totalPrefWidth + 2 * kHTBEdge + kHSluff * (fToolbarItems.size ()) + 2 * ::GetSystemMetrics (SM_CYEDGE));

        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_Toolbar::NotifyOfOwningActiveLedIt (IDispatch* owningActiveLedIt, IDispatch* owningALToolbar)
{
    try {
        if (fOwningActiveLedIt != owningActiveLedIt or fOwningALToolbar != owningALToolbar) {
            if (m_hWnd != NULL) {
                DestroyWindow ();
            }

            fOwningActiveLedIt = owningActiveLedIt;
            fOwningALToolbar   = owningALToolbar;

            if (fOwningActiveLedIt != NULL) {
                HWND                      parentHWND = 0;
                CComQIPtr<IALToolbarList> altbl      = fOwningALToolbar;
                if (altbl.p != NULL) {
                    ThrowIfErrorHRESULT (altbl->get_hWnd (&parentHWND));
                }
                RECT r = AsRECT (fBounds);
                Create (parentHWND, &r);
            }

            for (vector<CComPtr<IDispatch>>::iterator i = fToolbarItems.begin (); i != fToolbarItems.end (); ++i) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
                CComQIPtr<IALToolbarElement> tbi = (IDispatch*)*i;
#else
                CComQIPtr<IALToolbarElement> tbi = *i;
#endif
                tbi->NotifyOfOwningToolbar (owningActiveLedIt == NULL ? NULL : this, owningActiveLedIt);
            }

            CallInvalidateLayout ();
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_Toolbar::SetRectangle (int X, int Y, UINT width, UINT height)
{
    try {
        fBounds = Led_Rect (Y, X, height, width);
        MoveWindow (X, Y, width, height);
        DoLayout ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

/*
     ********************************************************************************
     **************************** ActiveLedIt_ToolbarList ***************************
     ********************************************************************************
     */
ActiveLedIt_ToolbarList::ActiveLedIt_ToolbarList ()
    : fToolbars ()
    , fOwningActiveLedIt (NULL)
    , fBounds ()
{
}

ActiveLedIt_ToolbarList::~ActiveLedIt_ToolbarList ()
{
    if (m_hWnd != NULL) {
        DestroyWindow ();
    }
}

HRESULT ActiveLedIt_ToolbarList::FinalConstruct ()
{
    return S_OK;
}

void ActiveLedIt_ToolbarList::FinalRelease ()
{
}

LRESULT ActiveLedIt_ToolbarList::OnPaint ([[maybe_unused]] UINT uMsg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam,
                                          [[maybe_unused]] BOOL& bHandled)
{
    PAINTSTRUCT ps;
    HDC         hdc = ::BeginPaint (m_hWnd, &ps);
    if (hdc != NULL) {
        try {
            Tablet           tablet (hdc, Tablet::eDoesntOwnDC);
            Pen              usePen (PS_SOLID, 1, Color::kBlack.GetOSRep ());
            GDI_Obj_Selector penSelector (&tablet, usePen);
            Led_Rect         drawRect = Led_Rect (0, 0, fBounds.GetHeight (), fBounds.GetWidth ());
            if (m_hWnd != NULL) { // probbaly not really needed anymore - now that I subtract out the SM_CXYEDGE - but what the heck... Just in case...
                RECT cr;
                ::GetClientRect (m_hWnd, &cr);
                drawRect = AsLedRect (cr);
            }
            tablet.MoveTo (Led_Point (drawRect.GetBottom () - 1, drawRect.GetLeft ()));
            tablet.LineTo (Led_Point (drawRect.GetBottom () - 1, drawRect.GetRight ()));
        }
        catch (...) {
            ::EndPaint (m_hWnd, &ps);
            throw;
        }
        ::EndPaint (m_hWnd, &ps);
    }
    return 0;
}

STDMETHODIMP ActiveLedIt_ToolbarList::get__NewEnum (IUnknown** ppUnk)
{
    using VarVarEnum = CComEnumOnSTL<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, STL_ATL_COPY_VARIANT_IDISPATCH, std::vector<CComPtr<IDispatch>>>;
    if (ppUnk == NULL) {
        return E_INVALIDARG;
    }
    try {
        return CreateSTLEnumerator<VarVarEnum> (ppUnk, this, fToolbars);
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ToolbarList::get_Item (long Index, IDispatch** pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    if (Index < 0 or static_cast<size_t> (Index) >= fToolbars.size ()) {
        return E_INVALIDARG;
    }
    try {
        *pVal = fToolbars[Index];
        (*pVal)->AddRef ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_ToolbarList::get_Count (UINT* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = static_cast<UINT> (fToolbars.size ());
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_ToolbarList::Add (IDispatch* newElt, UINT atIndex)
{
    if (newElt == NULL) {
        return E_INVALIDARG;
    }
    try {
        CComQIPtr<IALToolbar> tb = newElt;
        ThrowIfErrorHRESULT (tb->NotifyOfOwningActiveLedIt (fOwningActiveLedIt, fOwningActiveLedIt == NULL ? NULL : this));
        size_t idx = min (static_cast<size_t> (atIndex), fToolbars.size ());
        fToolbars.insert (fToolbars.begin () + idx, newElt);
        CallInvalidateLayout ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_ToolbarList::Remove (VARIANT eltIntNameOrIndex)
{
    try {
        size_t idx = DoFindIndex (&fToolbars, eltIntNameOrIndex);
        if (idx != kBadIndex) {
            return E_INVALIDARG;
        }
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
        CComQIPtr<IALToolbar> tb = (IDispatch*)fToolbars[idx];
#else
        CComQIPtr<IALToolbar> tb = fToolbars[idx];
#endif
        ThrowIfErrorHRESULT (tb->NotifyOfOwningActiveLedIt (NULL, NULL));
        fToolbars.erase (fToolbars.begin () + idx, fToolbars.begin () + idx + 1);
        CallInvalidateLayout ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ToolbarList::Clear ()
{
    try {
        for (vector<CComPtr<IDispatch>>::iterator i = fToolbars.begin (); i != fToolbars.end (); ++i) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
            CComQIPtr<IALToolbar> tb = (IDispatch*)*i;
#else
            CComQIPtr<IALToolbar> tb = *i;
#endif
            ThrowIfErrorHRESULT (tb->NotifyOfOwningActiveLedIt (NULL, NULL));
        }
        fToolbars.clear ();
        CallInvalidateLayout ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

void ActiveLedIt_ToolbarList::CallInvalidateLayout ()
{
    if (fOwningActiveLedIt != NULL) {
        CComPtr<IDispatch> oal = fOwningActiveLedIt;
        ThrowIfErrorHRESULT (oal.Invoke0 (DISPID_InvalidateLayout));
    }
}

STDMETHODIMP ActiveLedIt_ToolbarList::NotifyOfOwningActiveLedIt (IDispatch* owningActiveLedIt, HWND owningHWND)
{
    try {
        if (fOwningActiveLedIt != owningActiveLedIt) {
            if (m_hWnd != NULL) {
                DestroyWindow ();
            }

            fOwningActiveLedIt = owningActiveLedIt;

            if (fOwningActiveLedIt != NULL) {
                RECT r = AsRECT (fBounds);
                Create (owningHWND, &r, NULL, 0, WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE);
            }

            for (vector<CComPtr<IDispatch>>::iterator i = fToolbars.begin (); i != fToolbars.end (); ++i) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
                CComQIPtr<IALToolbar> tb = (IDispatch*)*i;
#else
                CComQIPtr<IALToolbar> tb = *i;
#endif
                ThrowIfErrorHRESULT (tb->NotifyOfOwningActiveLedIt (owningActiveLedIt, owningActiveLedIt == NULL ? NULL : this));
            }
            CallInvalidateLayout ();
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_ToolbarList::get_hWnd (HWND* pVal)
{
    try {
        *pVal = m_hWnd;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_ToolbarList::get_PreferredHeight (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }

        UINT totalHeight = 0;
        for (vector<CComPtr<IDispatch>>::iterator i = fToolbars.begin (); i != fToolbars.end (); ++i) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
            CComQIPtr<IALToolbar> tb = (IDispatch*)*i;
#else
            CComQIPtr<IALToolbar> tb = *i;
#endif
            UINT preferredHeight = 0;
            ThrowIfErrorHRESULT (tb->get_PreferredHeight (&preferredHeight));
            totalHeight += preferredHeight;
        }

        // Use total preferred height of all toolbars, plus room for bottom line, plus room for edges of toolbar itself
        if (fToolbars.size () > 0) {
            *pVal = totalHeight + kRoomForBotLine + 2 * ::GetSystemMetrics (SM_CYEDGE);
        }
        else {
            // if no toolbars - no edge/bottom line either...
            *pVal = 0;
        }

        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ToolbarList::get_PreferredWidth (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        UINT maxWidth = 0;
        for (vector<CComPtr<IDispatch>>::iterator i = fToolbars.begin (); i != fToolbars.end (); ++i) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
            CComQIPtr<IALToolbar> tb = (IDispatch*)*i;
#else
            CComQIPtr<IALToolbar> tb = *i;
#endif
            UINT preferredWidth = 0;
            ThrowIfErrorHRESULT (tb->get_PreferredWidth (&preferredWidth));
            maxWidth = max (maxWidth, preferredWidth);
        }

        // Use total preferred width of all buttons, plus room for bottom line, plus room for edges of toolbar itself
        *pVal = maxWidth + 2 * ::GetSystemMetrics (SM_CYEDGE);

        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

STDMETHODIMP ActiveLedIt_ToolbarList::SetRectangle (int X, int Y, UINT width, UINT height)
{
    try {
        fBounds = Led_Rect (Y, X, height, width);
        MoveWindow (X, Y, width, height);
        DoLayout ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION ()
}

void ActiveLedIt_ToolbarList::DoLayout ()
{
    Led_Rect clientBounds =
        Led_Rect (0, 0, max (static_cast<CoordinateType> (fBounds.GetHeight ()) - 2 * ::GetSystemMetrics (SM_CYEDGE), CoordinateType (0)),
                  max (static_cast<CoordinateType> (fBounds.GetWidth ()) - 2 * ::GetSystemMetrics (SM_CXEDGE), CoordinateType (0)));
    if (m_hWnd != NULL) { // probbaly not really needed anymore - now that I subtract out the SM_CXYEDGE - but what the heck... Just in case...
        RECT cr;
        ::GetClientRect (m_hWnd, &cr);
        clientBounds.bottom = max (clientBounds.bottom, cr.bottom);
    }

    Led_Rect itemBoundsCursor = clientBounds;
    for (vector<CComPtr<IDispatch>>::iterator i = fToolbars.begin (); i != fToolbars.end (); ++i) {
#if qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
        CComQIPtr<IALToolbar> tb = (IDispatch*)*i;
#else
        CComQIPtr<IALToolbar> tb = *i;
#endif
        if (tb.p != NULL) {
            UINT height = 0;
            ThrowIfErrorHRESULT (tb->get_PreferredHeight (&height));
            itemBoundsCursor.bottom = itemBoundsCursor.top + height;
            ThrowIfErrorHRESULT (
                tb->SetRectangle (itemBoundsCursor.left, itemBoundsCursor.top, itemBoundsCursor.GetWidth (), itemBoundsCursor.GetHeight ()));
            itemBoundsCursor.top = itemBoundsCursor.bottom;
        }
    }
}
