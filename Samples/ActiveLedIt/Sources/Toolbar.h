/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef __Toolbar_h__
#define __Toolbar_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxctl.h>
#include <atlwin.h>

#include "Stroika/Frameworks/Led/IdleManager.h"
#include "Stroika/Frameworks/Led/Platform/ATL.h"
#include "Stroika/Frameworks/Led/TextInteractor.h"

#include "ActiveLedItConfig.h"
#include "Resource.h"
#include "UserConfigCommands.h"

#include "ActiveLedIt_h.h"

class ATL_NO_VTABLE ActiveLedIt_IconButtonToolbarElement : public CComObjectRootEx<CComSingleThreadModel>,
                                                           public CComCoClass<ActiveLedIt_IconButtonToolbarElement>,
                                                           public IDispatchImpl<IALToolbarElement, &IID_IALToolbarElement, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
                                                           public IDispatchImpl<IALIconButtonToolbarElement, &IID_IALIconButtonToolbarElement, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
                                                           public CWindowImpl<ActiveLedIt_IconButtonToolbarElement> {
public:
    ActiveLedIt_IconButtonToolbarElement ();
    virtual ~ActiveLedIt_IconButtonToolbarElement ();

public:
    DECLARE_NO_REGISTRY ()

public:
    DECLARE_WND_SUPERCLASS (NULL, _T("Button"))
    BEGIN_MSG_MAP (ActiveLedIt_IconButtonToolbarElement)
    MESSAGE_HANDLER (WM_LBUTTONDOWN, OnLButtonDown)
    MESSAGE_HANDLER (WM_LBUTTONUP, OnLButtonUp)
    MESSAGE_HANDLER (WM_MOUSEMOVE, OnMouseMove)
    END_MSG_MAP ()
    nonvirtual LRESULT OnCreate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    nonvirtual LRESULT OnSetFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    nonvirtual LRESULT OnLButtonDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    nonvirtual LRESULT OnLButtonUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    nonvirtual LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
    BEGIN_COM_MAP (ActiveLedIt_IconButtonToolbarElement)
    COM_INTERFACE_ENTRY (IALIconButtonToolbarElement)
    COM_INTERFACE_ENTRY2 (IDispatch, IALIconButtonToolbarElement)
    COM_INTERFACE_ENTRY2 (IALToolbarElement, IALIconButtonToolbarElement)
    END_COM_MAP ()

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

    //  IALToolbarElement
public:
    STDMETHOD (get_PreferredHeight)
    (UINT* pVal);
    STDMETHOD (get_PreferredWidth)
    (UINT* pVal);
    STDMETHOD (get_X)
    (INT* pVal);
    STDMETHOD (get_Y)
    (INT* pVal);
    STDMETHOD (get_Width)
    (UINT* pVal);
    STDMETHOD (get_Height)
    (UINT* pVal);
    STDMETHOD (SetRectangle)
    (int X, int Y, UINT width, UINT height);
    STDMETHOD (NotifyOfOwningToolbar)
    (IDispatch* owningToolbar, IDispatch* owningActiveLedIt);
    STDMETHOD (UpdateEnableState)
    ();

    //  IALIconButtonToolbarElement
public:
    STDMETHOD (get_ButtonImage)
    (IDispatch** pVal);
    STDMETHOD (put_ButtonImage)
    (IDispatch* val);
    STDMETHOD (get_Command)
    (VARIANT* pVal);
    STDMETHOD (put_Command)
    (VARIANT val);
    STDMETHOD (get_ButtonStyle)
    (IconButtonStyle* pVal);
    STDMETHOD (put_ButtonStyle)
    (IconButtonStyle val);

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();

private:
    nonvirtual void UpdateButtonObj ();
    nonvirtual void CheckFixButtonStyle ();

private:
    HWND                fHWnd;
    bool                fPressedOnClick;
    CComQIPtr<IPicture> fButtonImage;
    CComVariant         fCommand;
    IconButtonStyle     fIconButtonStyle;
    IDispatch*          fOwningToolbar;
    IDispatch*          fOwningActiveLedIt;
    Led_Rect            fBounds;
};

class ATL_NO_VTABLE ActiveLedIt_ComboBoxToolbarElement : public CComObjectRootEx<CComSingleThreadModel>,
                                                         public CComCoClass<ActiveLedIt_ComboBoxToolbarElement>,
                                                         public IDispatchImpl<IALToolbarElement, &IID_IALToolbarElement, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
                                                         public IDispatchImpl<IALComboBoxToolbarElement, &IID_IALComboBoxToolbarElement, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
                                                         public CWindowImpl<ActiveLedIt_ComboBoxToolbarElement> {
public:
    ActiveLedIt_ComboBoxToolbarElement ();
    virtual ~ActiveLedIt_ComboBoxToolbarElement ();

public:
    DECLARE_NO_REGISTRY ()

public:
    DECLARE_WND_CLASS (NULL)
    BEGIN_MSG_MAP (ActiveLedIt_ComboBoxToolbarElement)
    COMMAND_HANDLER (0, CBN_DROPDOWN, OnCBDropDown)
    COMMAND_HANDLER (0, CBN_CLOSEUP, OnCBCloseUp)
    COMMAND_HANDLER (0, CBN_SELCHANGE, OnCBSelChange)
    END_MSG_MAP ()
    nonvirtual LRESULT OnCBSelChange (USHORT uMsg, USHORT wParam, HWND ctlHandle, BOOL& bHandled);
    nonvirtual LRESULT OnCBDropDown (USHORT uMsg, USHORT wParam, HWND ctlHandle, BOOL& bHandled);
    nonvirtual LRESULT OnCBCloseUp (USHORT uMsg, USHORT wParam, HWND ctlHandle, BOOL& bHandled);

public:
    BEGIN_COM_MAP (ActiveLedIt_ComboBoxToolbarElement)
    COM_INTERFACE_ENTRY2 (IDispatch, IALComboBoxToolbarElement)
    COM_INTERFACE_ENTRY2 (IALToolbarElement, IALComboBoxToolbarElement)
    COM_INTERFACE_ENTRY (IALComboBoxToolbarElement)
    END_COM_MAP ()

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

    //  IALToolbarElement
public:
    STDMETHOD (get_PreferredHeight)
    (UINT* pVal);
    STDMETHOD (get_PreferredWidth)
    (UINT* pVal);
    STDMETHOD (get_X)
    (INT* pVal);
    STDMETHOD (get_Y)
    (INT* pVal);
    STDMETHOD (get_Width)
    (UINT* pVal);
    STDMETHOD (get_Height)
    (UINT* pVal);
    STDMETHOD (SetRectangle)
    (int X, int Y, UINT width, UINT height);
    STDMETHOD (NotifyOfOwningToolbar)
    (IDispatch* owningToolbar, IDispatch* owningActiveLedIt);
    STDMETHOD (UpdateEnableState)
    ();

    //  IALComboBoxToolbarElement
public:
    STDMETHOD (get_CommandList)
    (IDispatch** pVal);
    STDMETHOD (put_CommandList)
    (IDispatch* val);

    // Specify elements
private:
    Led_Distance fPreferredWidth;

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();

private:
    nonvirtual void UpdatePopupObj ();
    nonvirtual void CallInvalidateLayout ();

private:
    bool                        fDropDownActive;
    CComPtr<IDispatch>          fCommandList;
    vector<CComPtr<IALCommand>> fCommandListCache;
    HWND                        fHWnd;
    CWindow                     fComboBox; // in sub-window because of how combobox sends notifications (to parent - not self)
    bool                        fPressedOnClick;
    IDispatch*                  fOwningToolbar;
    IDispatch*                  fOwningActiveLedIt;
    Led_Rect                    fBounds;
};

class ATL_NO_VTABLE ActiveLedIt_SeparatorToolbarElement : public CComObjectRootEx<CComSingleThreadModel>,
                                                          public CComCoClass<ActiveLedIt_SeparatorToolbarElement>,
                                                          public IDispatchImpl<IALToolbarElement, &IID_IALToolbarElement, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion> {
public:
    ActiveLedIt_SeparatorToolbarElement ();
    virtual ~ActiveLedIt_SeparatorToolbarElement ();

public:
    DECLARE_NO_REGISTRY ()

public:
    BEGIN_COM_MAP (ActiveLedIt_SeparatorToolbarElement)
    COM_INTERFACE_ENTRY (IALToolbarElement)
    COM_INTERFACE_ENTRY (IDispatch)
    END_COM_MAP ()

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

    //  IALToolbarElement
public:
    STDMETHOD (get_PreferredHeight)
    (UINT* pVal);
    STDMETHOD (get_PreferredWidth)
    (UINT* pVal);
    STDMETHOD (get_X)
    (INT* pVal);
    STDMETHOD (get_Y)
    (INT* pVal);
    STDMETHOD (get_Width)
    (UINT* pVal);
    STDMETHOD (get_Height)
    (UINT* pVal);
    STDMETHOD (SetRectangle)
    (int X, int Y, UINT width, UINT height);
    STDMETHOD (NotifyOfOwningToolbar)
    (IDispatch* owningToolbar, IDispatch* owningActiveLedIt);
    STDMETHOD (UpdateEnableState)
    ();

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();

private:
    Led_Rect fBounds;
};

class ATL_NO_VTABLE ActiveLedIt_Toolbar : public CComObjectRootEx<CComSingleThreadModel>,
                                          public CComCoClass<ActiveLedIt_Toolbar>,
                                          public IDispatchImpl<IALToolbar, &IID_IALToolbar, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
                                          public CWindowImpl<ActiveLedIt_Toolbar>,
                                          public EnterIdler {
public:
    ActiveLedIt_Toolbar ();
    virtual ~ActiveLedIt_Toolbar ();

public:
    DECLARE_NO_REGISTRY ()

public:
    DECLARE_WND_CLASS_EX (NULL, CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, COLOR_BTNFACE);
    BEGIN_MSG_MAP (ActiveLedIt_Toolbar)
    MESSAGE_HANDLER (WM_COMMAND, OnCommand)
    END_MSG_MAP ()
protected:
    nonvirtual LRESULT OnCommand (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
    enum { kHSluff = 1 };
    enum { kVInset = 3 };
    enum { kHTBEdge = 3 };

public:
    BEGIN_COM_MAP (ActiveLedIt_Toolbar)
    COM_INTERFACE_ENTRY (IALToolbar)
    COM_INTERFACE_ENTRY (IDispatch)
    END_COM_MAP ()

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

protected:
    virtual void OnEnterIdle () override;

public:
    STDMETHOD (get__NewEnum)
    (IUnknown** ppUnk);
    STDMETHOD (get_Item)
    (long Index, IDispatch** pVal);
    STDMETHOD (get_Count)
    (long* pVal);
    STDMETHOD (Add)
    (IDispatch* newElt, UINT atIndex = 0xffffffff);
    STDMETHOD (MergeAdd)
    (IDispatch* newElts, UINT afterElt = static_cast<UINT> (-1));
    STDMETHOD (Remove)
    (VARIANT eltIntNameOrIndex);
    STDMETHOD (Clear)
    ();
    STDMETHOD (get_hWnd)
    (HWND* pVal);
    STDMETHOD (get_PreferredHeight)
    (UINT* pVal);
    STDMETHOD (get_PreferredWidth)
    (UINT* pVal);
    STDMETHOD (NotifyOfOwningActiveLedIt)
    (IDispatch* owningActiveLedIt, IDispatch* owningALToolbar);
    STDMETHOD (SetRectangle)
    (int X, int Y, UINT width, UINT height);

private:
    nonvirtual void CallInvalidateLayout ();
    nonvirtual void DoLayout ();

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();

private:
    vector<CComPtr<IDispatch>> fToolbarItems;
    IDispatch*                 fOwningActiveLedIt; // don't use CComPtr<> cuz don't want to addref and create circular reference
    IDispatch*                 fOwningALToolbar;   //  ''
    Led_Rect                   fBounds;
};

class ATL_NO_VTABLE ActiveLedIt_ToolbarList : public CComObjectRootEx<CComSingleThreadModel>,
                                              public CComCoClass<ActiveLedIt_ToolbarList>,
                                              public IDispatchImpl<IALToolbarList, &IID_IALToolbarList, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
                                              public CWindowImpl<ActiveLedIt_ToolbarList> {
public:
    ActiveLedIt_ToolbarList ();
    virtual ~ActiveLedIt_ToolbarList ();

public:
    DECLARE_NO_REGISTRY ()

public:
    enum { kRoomForBotLine = 1 };

public:
    DECLARE_WND_CLASS_EX (NULL, CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, COLOR_BTNFACE);
    BEGIN_MSG_MAP (ActiveLedIt_ToolbarList)
    MESSAGE_HANDLER (WM_PAINT, OnPaint)
    END_MSG_MAP ()
protected:
    nonvirtual LRESULT OnPaint (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
    BEGIN_COM_MAP (ActiveLedIt_ToolbarList)
    COM_INTERFACE_ENTRY (IALToolbarList)
    COM_INTERFACE_ENTRY (IDispatch)
    END_COM_MAP ()

public:
    DECLARE_PROTECT_FINAL_CONSTRUCT ()

public:
    STDMETHOD (get__NewEnum)
    (IUnknown** ppUnk);
    STDMETHOD (get_Item)
    (long Index, IDispatch** pVal);
    STDMETHOD (get_Count)
    (UINT* pVal);
    STDMETHOD (Add)
    (IDispatch* newElt, UINT atIndex = 0xffffffff);
    STDMETHOD (Remove)
    (VARIANT eltIntNameOrIndex);
    STDMETHOD (Clear)
    ();
    STDMETHOD (get_hWnd)
    (HWND* pVal);
    STDMETHOD (get_PreferredHeight)
    (UINT* pVal);
    STDMETHOD (get_PreferredWidth)
    (UINT* pVal);
    STDMETHOD (NotifyOfOwningActiveLedIt)
    (IDispatch* owningActiveLedIt, HWND owningHWND);
    STDMETHOD (SetRectangle)
    (int X, int Y, UINT width, UINT height);

private:
    nonvirtual void CallInvalidateLayout ();
    nonvirtual void DoLayout ();

public:
    HRESULT FinalConstruct ();
    void    FinalRelease ();

private:
    vector<CComPtr<IDispatch>> fToolbars;
    IDispatch*                 fOwningActiveLedIt; // don't use CComPtr<> cuz don't want to addref and create circular reference
    Led_Rect                   fBounds;
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*__Toolbar_h__*/
