/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef __LedItView_h__
#define __LedItView_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

DISABLE_COMPILER_MSC_WARNING_START (5054)
#include <afxctl.h>
DISABLE_COMPILER_MSC_WARNING_END (5054)

#include <atlbase.h>

#include "Stroika/Frameworks/Led/ChunkedArrayTextStore.h"
#include "Stroika/Frameworks/Led/Platform/MFC_WordProcessor.h"
#include "Stroika/Frameworks/Led/WordProcessor.h"

#include "ActiveLedItConfig.h"
#include "Resource.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;

class LedItViewController {
public:
    LedItViewController ();
    ~LedItViewController ();

public:
    virtual void OnBrowseHelpCommand ()                            = 0;
    virtual void OnAboutBoxCommand ()                              = 0;
    virtual void ForceUIActive ()                                  = 0;
    virtual void FireOLEEvent (DISPID eventID)                     = 0;
    virtual void FireOLEEvent (DISPID dispid, BYTE* pbParams, ...) = 0;
#if qFunnyDisplayInDesignMode
    virtual bool IsInDesignMode () const            = 0;
    virtual bool DrawExtraDesignModeBorder () const = 0;
#endif
    virtual HMENU GenerateContextMenu () = 0;

public:
    nonvirtual void FireKeyDown (USHORT* pnChar, short nShiftState);
    nonvirtual void FireKeyUp (USHORT* pnChar, short nShiftState);
    nonvirtual void FireKeyPress (USHORT* pnChar);
    nonvirtual void FireMouseDown (short nButton, short nShiftState, OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y);
    nonvirtual void FireMouseUp (short nButton, short nShiftState, OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y);
    nonvirtual void FireMouseMove (short nButton, short nShiftState, OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y);
    nonvirtual void FireClick ();
    nonvirtual void FireDblClick ();
    virtual void    FireUpdateUserCommand (const wstring& internalCmdName, VARIANT_BOOL* enabled, VARIANT_BOOL* checked, wstring* name) = 0;
    virtual void    FireUserCommand (const wstring& internalCmdName)                                                                    = 0;

public:
    ChunkedArrayTextStore                 fTextStore;
    MultiLevelUndoCommandHandler          fCommandHandler;
    WordProcessor::HidableTextDatabasePtr fHidableTextDatabase;
};

using LedItViewAlmostBASE = Led_MFC_ExceptionHandlerHelper<Led_MFC_X<WordProcessor>>;

DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning

class LedItView : public WordProcessorCommonCommandHelper_MFC<LedItViewAlmostBASE> {
private:
    using inherited = WordProcessorCommonCommandHelper_MFC<LedItViewAlmostBASE>;

public:
    LedItView ();

public:
    nonvirtual void SetController (LedItViewController* controller);

private:
    LedItViewController* fController;

public:
    virtual ~LedItView ();

public:
    nonvirtual bool GetSupportContextMenu () const;
    nonvirtual void SetSupportContextMenu (bool allowContextMenu);

private:
    bool fSupportContextMenu;

public:
    nonvirtual bool GetHideDisabledContextMenuItems () const;
    nonvirtual void SetHideDisabledContextMenuItems (bool hideDisabledContextMenuItems);

private:
    bool fHideDisabledContextMenuItems;

public:
    nonvirtual bool GetWrapToWindow () const;
    nonvirtual void SetWrapToWindow (bool wrapToWindow);

private:
    bool fWrapToWindow;

public:
    nonvirtual long GetMaxLength () const;
    nonvirtual void SetMaxLength (long maxLength);

private:
    long fMaxLength;

public:
    virtual void GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const override;
    virtual void SetWindowRect (const Led_Rect& windowRect) override;

public:
    nonvirtual Led_Distance CalculateFarthestRightMarginInWindow () const;

protected:
    afx_msg int OnMouseActivate (CWnd* pDesktopWnd, UINT nHitTest, UINT message);

public:
    virtual void PostNcDestroy () override;
    afx_msg void OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
    afx_msg void OnContextMenu (CWnd* /*pWnd*/, CPoint /*point*/);

    // Reflect events (call FireEvent) for these events. Must be done here cuz COleControl code for same doesn't
    // work cuz our focus (LedItView) isn't the same as the COleControl).
public:
    afx_msg void OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSysKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual void OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed) override;
    afx_msg void OnLButtonDown (UINT nFlags, CPoint oPoint);
    afx_msg void OnLButtonUp (UINT nFlags, CPoint oPoint);
    afx_msg void OnLButtonDblClk (UINT nFlags, CPoint oPoint);
    afx_msg void OnRButtonDown (UINT nFlags, CPoint oPoint);
    afx_msg void OnRButtonUp (UINT nFlags, CPoint oPoint);
    afx_msg void OnMouseMove (UINT nFlags, CPoint oPoint);

public:
    afx_msg void OnPasteAsTextCommand ();
    afx_msg void OnUpdatePasteAsTextCommand (CCmdUI* pCmdUI);

public:
    afx_msg void OnOLEUserCommand (UINT nID);
    afx_msg void OnUpdateOLEUserCommand (CCmdUI* pCmdUI);

protected:
    virtual SearchParameters GetSearchParameters () const override;
    virtual void             SetSearchParameters (const SearchParameters& sp) override;

public:
    virtual void SetSelection (size_t start, size_t end) override;
    using TextInteractor::SetSelection;

protected:
    virtual void AboutToUpdateText (const UpdateInfo& updateInfo) override;

public:
    virtual void EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing) override;

public:
    bool fEnableAutoChangesBackgroundColor;

public:
    nonvirtual long OLE_FindReplace (long searchFrom, const Led_tString& findText, const Led_tString& replaceText, BOOL wrapSearch, BOOL wholeWordSearch, BOOL caseSensativeSearch);

public:
    bool fInDrawMetaFileMode;

public:
    nonvirtual void UpdateScrollBars ();

protected:
    afx_msg void OnBrowseHelpCommand ();
    afx_msg void OnCheckForUpdatesWebPageCommand ();
    afx_msg void OnAboutBoxCommand ();

public:
    afx_msg void OnFilePrintOnce ();
    afx_msg void OnFilePrint ();
    afx_msg void OnFilePrintSetup ();

private:
    nonvirtual void DoPrintHelper (bool showPrintDlg);

    DECLARE_MESSAGE_MAP ()

#ifdef _DEBUG
public:
    virtual void AssertValid () const override;
    virtual void Dump (CDumpContext& dc) const override;
#endif
};
DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

//  class   LedItViewController
inline void LedItViewController::FireKeyDown (USHORT* pnChar, short nShiftState)
{
    FireOLEEvent (DISPID_KEYDOWN, EVENT_PARAM (VTS_PI2 VTS_I2), pnChar, nShiftState);
}
inline void LedItViewController::FireKeyUp (USHORT* pnChar, short nShiftState)
{
    FireOLEEvent (DISPID_KEYUP, EVENT_PARAM (VTS_PI2 VTS_I2), pnChar, nShiftState);
}
inline void LedItViewController::FireKeyPress (USHORT* pnChar)
{
    FireOLEEvent (DISPID_KEYPRESS, EVENT_PARAM (VTS_PI2), pnChar);
}
inline void LedItViewController::FireMouseDown (short nButton, short nShiftState, OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y)
{
    FireOLEEvent (DISPID_MOUSEDOWN, EVENT_PARAM (VTS_I2 VTS_I2 VTS_XPOS_PIXELS VTS_YPOS_PIXELS), nButton, nShiftState, x, y);
}
inline void LedItViewController::FireMouseUp (short nButton, short nShiftState, OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y)
{
    FireOLEEvent (DISPID_MOUSEUP, EVENT_PARAM (VTS_I2 VTS_I2 VTS_XPOS_PIXELS VTS_YPOS_PIXELS), nButton, nShiftState, x, y);
}
inline void LedItViewController::FireMouseMove (short nButton, short nShiftState, OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y)
{
    FireOLEEvent (DISPID_MOUSEMOVE, EVENT_PARAM (VTS_I2 VTS_I2 VTS_XPOS_PIXELS VTS_YPOS_PIXELS), nButton, nShiftState, x, y);
}
inline void LedItViewController::FireClick ()
{
    FireOLEEvent (DISPID_CLICK, EVENT_PARAM (VTS_NONE));
}
inline void LedItViewController::FireDblClick ()
{
    FireOLEEvent (DISPID_DBLCLICK, EVENT_PARAM (VTS_NONE));
}

//  class   LedItView
inline bool LedItView::GetWrapToWindow () const
{
    return fWrapToWindow;
}
inline bool LedItView::GetSupportContextMenu () const
{
    return fSupportContextMenu;
}
inline bool LedItView::GetHideDisabledContextMenuItems () const
{
    return fHideDisabledContextMenuItems;
}
inline long LedItView::GetMaxLength () const
{
    return fMaxLength;
}

#endif /*__LedItView_h__*/
