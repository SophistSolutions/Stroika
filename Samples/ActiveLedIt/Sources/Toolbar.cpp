/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#pragma warning(push)
#pragma warning(disable : 5054)
#include <afxctl.h>
#pragma warning(pop)

#include <atlbase.h>

#include "DispIDs.h"

#include "Toolbar.h"

#define CATCH_AND_HANDLE_EXCEPTIONS()     \
    catch (HRESULT hr)                    \
    {                                     \
        return hr;                        \
    }                                     \
    catch (Win32ErrorException & we)      \
    {                                     \
        return (HRESULT_FROM_WIN32 (we)); \
    }                                     \
    catch (HRESULTErrorException & h)     \
    {                                     \
        return static_cast<HRESULT> (h);  \
    }                                     \
    catch (...)                           \
    {                                     \
        return DISP_E_EXCEPTION;          \
    }

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
                Led_ThrowIfErrorHRESULT (DISP_E_MEMBERNOTFOUND);
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
        for (UINT i = 0; i < itemCount; i++) {
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
                Led_ThrowIfErrorHRESULT (al.Invoke1 (DISPID_CommandEnabled, &cmdNameCCV, &result));
                Led_ThrowIfErrorHRESULT (result.ChangeType (VT_BOOL));
                ::EnableMenuItem (menu, i, MF_BYPOSITION | (result.boolVal ? MF_ENABLED : MF_GRAYED));

                cmdNameCCV = cmdName.c_str ();
                Led_ThrowIfErrorHRESULT (al.Invoke1 (DISPID_CommandChecked, &cmdNameCCV, &result));
                Led_ThrowIfErrorHRESULT (result.ChangeType (VT_BOOL));
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

LRESULT ActiveLedIt_IconButtonToolbarElement::OnLButtonDown ([[maybe_unused]] UINT uMsg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam, BOOL& bHandled)
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
                    Led_ThrowIfErrorHRESULT (oal.GetProperty (DISPID_AcceleratorTable, &accel));
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
                                int cmdNum = ::TrackPopupMenu (mH, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, wr.left, wr.bottom, 0, m_hWnd, NULL);
                                if (cmdNum != 0) {
                                    wstring cmdName = CmdNum2Name (cmdNum);
                                    if (cmdName.length () != 0) {
                                        CComVariant cmdNameCCV = cmdName.c_str ();
                                        Led_ThrowIfErrorHRESULT (oal.Invoke1 (DISPID_InvokeCommand, &cmdNameCCV));
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

LRESULT ActiveLedIt_IconButtonToolbarElement::OnLButtonUp ([[maybe_unused]] UINT uMsg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam, BOOL& bHandled)
{
    /*
     *  Pushing on a button causes it to grab the focus (cannot figure out how to stop this - but its not SO bad).
     *  Instead - just reset the focus back to the OCX when the click is finihsed.
     */
    LRESULT lr = DefWindowProc ();
    try {
        CComPtr<IDispatch> oal = fOwningActiveLedIt;
        CComVariant        res;
        Led_ThrowIfErrorHRESULT (oal.GetProperty (DISPID_HWND, &res));
        Led_ThrowIfErrorHRESULT (res.ChangeType (VT_UINT));
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
    const Led_Distance kBorderSize = 3;
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        if (fButtonImage.p == NULL) {
            *pVal = kBorderSize * 2;
            return S_OK;
        }
        OLE_XSIZE_HIMETRIC height = 0;
        Led_ThrowIfErrorHRESULT (fButtonImage->get_Height (&height));
        SIZEL hmSize;
        hmSize.cx = 0;
        hmSize.cy = height;
        SIZEL pixSize;
        ::memset (&pixSize, 0, sizeof (pixSize));
        AtlHiMetricToPixel (&hmSize, &pixSize);
        *pVal = pixSize.cy + 2 * kBorderSize;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::get_PreferredWidth (UINT* pVal)
{
    const Led_Distance kBorderSize = 3;
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        if (fButtonImage.p == NULL) {
            *pVal = kBorderSize * 2;
            return S_OK;
        }
        OLE_XSIZE_HIMETRIC width = 0;
        Led_ThrowIfErrorHRESULT (fButtonImage->get_Width (&width));
        SIZEL hmSize;
        hmSize.cx = width;
        hmSize.cy = 0;
        SIZEL pixSize;
        ::memset (&pixSize, 0, sizeof (pixSize));
        AtlHiMetricToPixel (&hmSize, &pixSize);
        *pVal = pixSize.cx + kBorderSize * 2;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
                    Led_ThrowIfErrorHRESULT (otb->get_hWnd (&parentHWND));
                }

                int itemID = 0;
                Create (parentHWND, &r, NULL, WS_CHILD | WS_VISIBLE, 0, itemID);
                UpdateButtonObj ();
            }
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
                    Led_ThrowIfErrorHRESULT (al.Invoke1 (DISPID_CommandEnabled, &cmdCCV, &result));
                    Led_ThrowIfErrorHRESULT (result.ChangeType (VT_BOOL));
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
                    Led_ThrowIfErrorHRESULT (al.Invoke1 (DISPID_CommandChecked, &commandCCV, &result));
                    Led_ThrowIfErrorHRESULT (result.ChangeType (VT_BOOL));
                    checked = !!result.boolVal;
                }
                SendMessage (BM_SETSTATE, checked ? BST_PUSHED : 0);
            }
        }
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::put_ButtonImage (IDispatch* val)
{
    try {
        fButtonImage = val;
#if qDebug
        if (fButtonImage.p == NULL and val != NULL) {
            DumpSupportedInterfaces (val, "SUP INTERFACES FOR JSCRIPT IMAGE");
        }
#endif
        UpdateButtonObj ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::get_Command (VARIANT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        CComVariant (fCommand).Detach (pVal);
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::put_Command (VARIANT val)
{
    try {
        fCommand = CComVariant (val);
        CheckFixButtonStyle ();
        UpdateButtonObj ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_IconButtonToolbarElement::put_ButtonStyle (IconButtonStyle val)
{
    try {
        fIconButtonStyle = val;
        CheckFixButtonStyle ();
        UpdateButtonObj ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
            Led_ThrowIfErrorHRESULT (fButtonImage->get_Type (&pictType));
            OLE_HANDLE pictHandle = NULL;
            Led_ThrowIfErrorHRESULT (fButtonImage->get_Handle (&pictHandle));
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

LRESULT ActiveLedIt_ComboBoxToolbarElement::OnCBDropDown ([[maybe_unused]] USHORT uMsg, [[maybe_unused]] USHORT wParam, [[maybe_unused]] HWND ctlHandle, BOOL& bHandled)
{
    fDropDownActive = true;
    LRESULT lr      = DefWindowProc ();
    bHandled        = true;
    return lr;
}

LRESULT ActiveLedIt_ComboBoxToolbarElement::OnCBCloseUp ([[maybe_unused]] USHORT uMsg, [[maybe_unused]] USHORT wParam, [[maybe_unused]] HWND ctlHandle, BOOL& bHandled)
{
    fDropDownActive = false;
    /*
     *  Using the combobox grabs the focus. Reset the focus back to the OCX when the click is finihsed.
     */
    LRESULT lr = DefWindowProc ();
    try {
        CComPtr<IDispatch> oal = fOwningActiveLedIt;
        CComVariant        res;
        Led_ThrowIfErrorHRESULT (oal.GetProperty (DISPID_HWND, &res));
        Led_ThrowIfErrorHRESULT (res.ChangeType (VT_UINT));
        HWND controlHWND = reinterpret_cast<HWND> (res.iVal);
        ::SetFocus (controlHWND);
    }
    catch (...) {
        // ignore any errors here.
    }
    bHandled = true;
    return lr;
}

LRESULT ActiveLedIt_ComboBoxToolbarElement::OnCBSelChange ([[maybe_unused]] USHORT uMsg, [[maybe_unused]] USHORT wParam, [[maybe_unused]] HWND ctlHandle, BOOL& bHandled)
{
    bHandled   = true;
    LRESULT lr = DefWindowProc ();
    int     r  = static_cast<int> (fComboBox.SendMessage (CB_GETCURSEL, 0, 0));
    try {
        if (r >= 0 and static_cast<size_t> (r) < fCommandListCache.size ()) {
            CComQIPtr<IALCommand> alc = fCommandListCache[r];
            CComBSTR              internalName;
            Led_ThrowIfErrorHRESULT (alc->get_InternalName (&internalName));
            CComPtr<IDispatch> al              = fOwningActiveLedIt;
            CComVariant        internalNameCCV = internalName;
            Led_ThrowIfErrorHRESULT (al.Invoke1 (DISPID_InvokeCommand, &internalNameCCV));
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
        const Led_Distance kWhiteSluff = 4;
        *pVal                          = ::GetSystemMetrics (SM_CYVSCROLL) + kWhiteSluff;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
                    Led_ThrowIfErrorHRESULT (otb->get_hWnd (&parentHWND));
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
                        CComVariant alcmdCCV = *i;
                        Led_ThrowIfErrorHRESULT (al.Invoke1 (DISPID_CommandEnabled, &alcmdCCV, &result));
                        Led_ThrowIfErrorHRESULT (result.ChangeType (VT_BOOL));
                        if (result.boolVal) {
                            enabled = true;
                        }
                    }
                    {
                        CComVariant result;
                        CComVariant alcmdCCV = *i;
                        Led_ThrowIfErrorHRESULT (al.Invoke1 (DISPID_CommandChecked, &alcmdCCV, &result));
                        Led_ThrowIfErrorHRESULT (result.ChangeType (VT_BOOL));
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_ComboBoxToolbarElement::put_CommandList (IDispatch* val)
{
    try {
        fCommandList = val;
        UpdatePopupObj ();
        CallInvalidateLayout ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

void ActiveLedIt_ComboBoxToolbarElement::UpdatePopupObj ()
{
    if (m_hWnd != NULL) {
        //  resetcoennt in combobox, and then walk list of commands and add items...
        //  Need to keep SIDE-vector of CComPtr<> guys for each elt - since the IALcommandList chould get
        //  changed behind our backs... (need some way to keep them in sync?)
        (void)fComboBox.SendMessage (CB_RESETCONTENT, 0, 0);
        fCommandListCache.clear ();
        CComQIPtr<IALCommandList> cmdList = fCommandList;
        if (cmdList.p != NULL) {
            long cmdCount = 0;
            Led_ThrowIfErrorHRESULT (cmdList->get_Count (&cmdCount));
            HDC     hdc     = GetWindowDC ();
            HGDIOBJ oldFont = ::SelectObject (hdc, ::GetStockObject (DEFAULT_GUI_FONT));
            try {
                Led_Distance maxItemWidth = 0;
                for (size_t i = 0; i < static_cast<size_t> (cmdCount); ++i) {
                    CComPtr<IDispatch> e;
                    Led_ThrowIfErrorHRESULT (cmdList->get_Item (i, &e));
                    CComQIPtr<IALCommand> alc = e;
                    CComBSTR              name;
                    Led_ThrowIfErrorHRESULT (alc->get_Name (&name));
                    fCommandListCache.push_back (alc);
                    Led_SDK_String itemPrintName = Led_Wide2SDKString (wstring (name));
                    Verify (fComboBox.SendMessage (CB_ADDSTRING, 0, reinterpret_cast<LPARAM> (itemPrintName.c_str ())) != CB_ERR);
                    SIZE sz;
                    memset (&sz, 0, sizeof (sz));
                    ::GetTextExtentPoint32 (hdc, itemPrintName.c_str (), itemPrintName.length (), &sz);
                    maxItemWidth = max (maxItemWidth, static_cast<Led_Distance> (sz.cx));
                }
                ::SelectObject (hdc, oldFont);
                ReleaseDC (hdc);
                const Led_Distance kMaxMax     = 200; // don't make any wider than this - even if really long text...
                maxItemWidth                   = min (maxItemWidth, kMaxMax);
                const Led_Distance kWhiteSluff = 6;
                fPreferredWidth                = 2 * ::GetSystemMetrics (SM_CXEDGE) + ::GetSystemMetrics (SM_CXVSCROLL) + maxItemWidth + kWhiteSluff;
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
        Led_ThrowIfErrorHRESULT (oal.Invoke0 (DISPID_InvalidateLayout));
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_SeparatorToolbarElement::SetRectangle (int X, int Y, UINT width, UINT height)
{
    try {
        fBounds = Led_Rect (Y, X, height, width);
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_SeparatorToolbarElement::NotifyOfOwningToolbar (IDispatch* /*owningToolbar*/, IDispatch* /*owningActiveLedIt*/)
{
    try {
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_SeparatorToolbarElement::UpdateEnableState ()
{
    try {
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
                Led_Tablet_             tablet (hdc, Led_Tablet_::eDoesntOwnDC);
                Led_Pen                 usePen (PS_SOLID, 1, Led_Color::kBlack.GetOSRep ());
                Led_GDI_Obj_Selector    penSelector (&tablet, usePen);
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
        Led_ThrowIfErrorHRESULT (oal.Invoke0 (DISPID_InvalidateLayout));
    }
}

void ActiveLedIt_Toolbar::DoLayout ()
{
#if 1
    Led_Rect clientBounds = Led_Rect (0, 0, fBounds.GetHeight (), fBounds.GetWidth ());
#else
    Led_Rect clientBounds = Led_Rect (0, 0,
                                      max (static_cast<Led_Coordinate> (fBounds.GetHeight ()) - 2 * ::GetSystemMetrics (SM_CYEDGE), 0),
                                      max (static_cast<Led_Coordinate> (fBounds.GetHeight ()) - 2 * ::GetSystemMetrics (SM_CXEDGE), 0));
#endif

    // MAYBE NOT NEEDED ANYMORE???
    if (m_hWnd != NULL) { // probbaly not really needed anymore - now that I subtract out the SM_CXYEDGE - but what the heck... Just in case...
        RECT cr;
        ::GetClientRect (m_hWnd, &cr);
        clientBounds.bottom = max (clientBounds.bottom, cr.bottom);
    }

    Led_Rect itemBoundsCursor = Led_Rect (clientBounds.GetTop (), clientBounds.GetLeft () + kHTBEdge, clientBounds.GetHeight (), 0);

    for (vector<CComPtr<IDispatch>>::iterator i = fToolbarItems.begin (); i != fToolbarItems.end (); ++i) {
        CComQIPtr<IALToolbarElement> tbi            = *i;
        UINT                         preferredWidth = 0;
        Led_ThrowIfErrorHRESULT (tbi->get_PreferredWidth (&preferredWidth));
        UINT preferredHeight = 0;
        Led_ThrowIfErrorHRESULT (tbi->get_PreferredHeight (&preferredHeight));

        Led_Distance useHeight  = min (Led_Distance (preferredHeight), itemBoundsCursor.GetHeight ());
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
            CComQIPtr<IALToolbarElement> tbi = *i;
            Led_ThrowIfErrorHRESULT (tbi->UpdateEnableState ());
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_Toolbar::get_Count (long* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        *pVal = fToolbarItems.size ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
            Led_ThrowIfErrorHRESULT (tbe->NotifyOfOwningToolbar (this, fOwningActiveLedIt));
        }
        CallInvalidateLayout ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
        Led_ThrowIfErrorHRESULT (alt->get_Count (&nElts2Add));
        for (size_t i = 0; i < static_cast<size_t> (nElts2Add); ++i) {
            CComPtr<IDispatch> e;
            Led_ThrowIfErrorHRESULT (alt->get_Item (i, &e));
            fToolbarItems.insert (fToolbarItems.begin () + idx, e);
            if (fOwningActiveLedIt != NULL) {
                CComQIPtr<IALToolbarElement> tbe = e;
                tbe->NotifyOfOwningToolbar (this, fOwningActiveLedIt);
            }
            ++idx;
        }
        CallInvalidateLayout ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
            CComQIPtr<IALToolbarElement> tbe = fToolbarItems[idx];
            Led_ThrowIfErrorHRESULT (tbe->NotifyOfOwningToolbar (NULL, NULL));
        }
        fToolbarItems.erase (fToolbarItems.begin () + idx, fToolbarItems.begin () + idx + 1);
        CallInvalidateLayout ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_Toolbar::Clear ()
{
    try {
        if (fOwningActiveLedIt != NULL) {
            for (vector<CComPtr<IDispatch>>::iterator i = fToolbarItems.begin (); i != fToolbarItems.end (); ++i) {
                CComQIPtr<IALToolbarElement> tbe = *i;
                Led_ThrowIfErrorHRESULT (tbe->NotifyOfOwningToolbar (NULL, NULL));
            }
        }
        fToolbarItems.clear ();
        CallInvalidateLayout ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_Toolbar::get_hWnd (HWND* pVal)
{
    try {
        *pVal = m_hWnd;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
            CComQIPtr<IALToolbarElement> tbi             = *i;
            UINT                         preferredHeight = 0;
            Led_ThrowIfErrorHRESULT (tbi->get_PreferredHeight (&preferredHeight));
            maxHeight = max (maxHeight, preferredHeight);
        }

        // Use MAX preferred height of all buttons, plus room for bottom line, plus room for edges of toolbar itself
        //      *pVal = maxHeight + 2*kVInset + kRoomForBotLine + 2 * ::GetSystemMetrics (SM_CYEDGE);
        // Use MAX preferred height of all buttons, plus room for above/below vinset
        *pVal = maxHeight + 2 * kVInset;

        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_Toolbar::get_PreferredWidth (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        UINT totalPrefWidth = 0;
        for (vector<CComPtr<IDispatch>>::iterator i = fToolbarItems.begin (); i != fToolbarItems.end (); ++i) {
            CComQIPtr<IALToolbarElement> tbi            = *i;
            UINT                         preferredWidth = 0;
            Led_ThrowIfErrorHRESULT (tbi->get_PreferredWidth (&preferredWidth));
            totalPrefWidth += preferredWidth;
        }

        // Use total preferred width of all buttons, plus room for bottom line, plus room for edges of toolbar itself
        *pVal = totalPrefWidth + 2 * kHTBEdge + kHSluff * (fToolbarItems.size ()) + 2 * ::GetSystemMetrics (SM_CYEDGE);

        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
                    Led_ThrowIfErrorHRESULT (altbl->get_hWnd (&parentHWND));
                }
                RECT r = AsRECT (fBounds);
                Create (parentHWND, &r);
            }

            for (vector<CComPtr<IDispatch>>::iterator i = fToolbarItems.begin (); i != fToolbarItems.end (); ++i) {
                CComQIPtr<IALToolbarElement> tbi = *i;
                tbi->NotifyOfOwningToolbar (owningActiveLedIt == NULL ? NULL : this, owningActiveLedIt);
            }

            CallInvalidateLayout ();
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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

LRESULT ActiveLedIt_ToolbarList::OnPaint ([[maybe_unused]] UINT uMsg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam, [[maybe_unused]] BOOL& bHandled)
{
    PAINTSTRUCT ps;
    HDC         hdc = ::BeginPaint (m_hWnd, &ps);
    if (hdc != NULL) {
        try {
            Led_Tablet_          tablet (hdc, Led_Tablet_::eDoesntOwnDC);
            Led_Pen              usePen (PS_SOLID, 1, Led_Color::kBlack.GetOSRep ());
            Led_GDI_Obj_Selector penSelector (&tablet, usePen);
            Led_Rect             drawRect = Led_Rect (0, 0, fBounds.GetHeight (), fBounds.GetWidth ());
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_ToolbarList::get_Count (UINT* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = fToolbars.size ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_ToolbarList::Add (IDispatch* newElt, UINT atIndex)
{
    if (newElt == NULL) {
        return E_INVALIDARG;
    }
    try {
        CComQIPtr<IALToolbar> tb = newElt;
        Led_ThrowIfErrorHRESULT (tb->NotifyOfOwningActiveLedIt (fOwningActiveLedIt, fOwningActiveLedIt == NULL ? NULL : this));
        size_t idx = min (static_cast<size_t> (atIndex), fToolbars.size ());
        fToolbars.insert (fToolbars.begin () + idx, newElt);
        CallInvalidateLayout ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_ToolbarList::Remove (VARIANT eltIntNameOrIndex)
{
    try {
        size_t idx = DoFindIndex (&fToolbars, eltIntNameOrIndex);
        if (idx != kBadIndex) {
            return E_INVALIDARG;
        }
        CComQIPtr<IALToolbar> tb = fToolbars[idx];
        Led_ThrowIfErrorHRESULT (tb->NotifyOfOwningActiveLedIt (NULL, NULL));
        fToolbars.erase (fToolbars.begin () + idx, fToolbars.begin () + idx + 1);
        CallInvalidateLayout ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_ToolbarList::Clear ()
{
    try {
        for (vector<CComPtr<IDispatch>>::iterator i = fToolbars.begin (); i != fToolbars.end (); ++i) {
            CComQIPtr<IALToolbar> tb = *i;
            Led_ThrowIfErrorHRESULT (tb->NotifyOfOwningActiveLedIt (NULL, NULL));
        }
        fToolbars.clear ();
        CallInvalidateLayout ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

void ActiveLedIt_ToolbarList::CallInvalidateLayout ()
{
    if (fOwningActiveLedIt != NULL) {
        CComPtr<IDispatch> oal = fOwningActiveLedIt;
        Led_ThrowIfErrorHRESULT (oal.Invoke0 (DISPID_InvalidateLayout));
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
                CComQIPtr<IALToolbar> tb = *i;
                Led_ThrowIfErrorHRESULT (tb->NotifyOfOwningActiveLedIt (owningActiveLedIt, owningActiveLedIt == NULL ? NULL : this));
            }
            CallInvalidateLayout ();
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_ToolbarList::get_hWnd (HWND* pVal)
{
    try {
        *pVal = m_hWnd;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
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
            CComQIPtr<IALToolbar> tb              = *i;
            UINT                  preferredHeight = 0;
            Led_ThrowIfErrorHRESULT (tb->get_PreferredHeight (&preferredHeight));
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
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_ToolbarList::get_PreferredWidth (UINT* pVal)
{
    try {
        if (pVal == NULL) {
            return E_INVALIDARG;
        }
        UINT maxWidth = 0;
        for (vector<CComPtr<IDispatch>>::iterator i = fToolbars.begin (); i != fToolbars.end (); ++i) {
            CComQIPtr<IALToolbar> tb             = *i;
            UINT                  preferredWidth = 0;
            Led_ThrowIfErrorHRESULT (tb->get_PreferredWidth (&preferredWidth));
            maxWidth = max (maxWidth, preferredWidth);
        }

        // Use total preferred width of all buttons, plus room for bottom line, plus room for edges of toolbar itself
        *pVal = maxWidth + 2 * ::GetSystemMetrics (SM_CYEDGE);

        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_ToolbarList::SetRectangle (int X, int Y, UINT width, UINT height)
{
    try {
        fBounds = Led_Rect (Y, X, height, width);
        MoveWindow (X, Y, width, height);
        DoLayout ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

void ActiveLedIt_ToolbarList::DoLayout ()
{
    Led_Rect clientBounds = Led_Rect (0, 0,
                                      max (static_cast<Led_Coordinate> (fBounds.GetHeight ()) - 2 * ::GetSystemMetrics (SM_CYEDGE), Led_Coordinate (0)),
                                      max (static_cast<Led_Coordinate> (fBounds.GetWidth ()) - 2 * ::GetSystemMetrics (SM_CXEDGE), Led_Coordinate (0)));
    if (m_hWnd != NULL) { // probbaly not really needed anymore - now that I subtract out the SM_CXYEDGE - but what the heck... Just in case...
        RECT cr;
        ::GetClientRect (m_hWnd, &cr);
        clientBounds.bottom = max (clientBounds.bottom, cr.bottom);
    }

    Led_Rect itemBoundsCursor = clientBounds;
    for (vector<CComPtr<IDispatch>>::iterator i = fToolbars.begin (); i != fToolbars.end (); ++i) {
        CComQIPtr<IALToolbar> tb = *i;
        if (tb.p != NULL) {
            UINT height = 0;
            Led_ThrowIfErrorHRESULT (tb->get_PreferredHeight (&height));
            itemBoundsCursor.bottom = itemBoundsCursor.top + height;
            Led_ThrowIfErrorHRESULT (tb->SetRectangle (itemBoundsCursor.left, itemBoundsCursor.top, itemBoundsCursor.GetWidth (), itemBoundsCursor.GetHeight ()));
            itemBoundsCursor.top = itemBoundsCursor.bottom;
        }
    }
}
