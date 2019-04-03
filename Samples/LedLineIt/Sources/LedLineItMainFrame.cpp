/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <stdio.h>

#include "Stroika/Foundation/Characters/CString/Utilities.h"

#include "FontMenu.h"
#include "LedLineItView.h"
#include "Resource.h"

#include "LedLineItMainFrame.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

inline LedLineItView* GetActiveLedItView ()
{
    CMDIFrameWnd* mainFrame = dynamic_cast<CMDIFrameWnd*> (::AfxGetMainWnd ());
    if (mainFrame != NULL) {
        CMDIChildWnd* childFrame = mainFrame->MDIGetActive ();
        if (childFrame != NULL) {
            return dynamic_cast<LedLineItView*> (childFrame->GetActiveView ());
        }
    }
    return NULL;
}

/*
 ********************************************************************************
 ***************** LedLineItMainFrame::StatusBar::GotoEdit **********************
 ********************************************************************************
 */
using StatusBar = LedLineItMainFrame::StatusBar;
using GotoEdit  = StatusBar::GotoEdit;
BEGIN_MESSAGE_MAP (GotoEdit, CEdit)
ON_WM_GETDLGCODE ()
ON_WM_KEYDOWN ()
END_MESSAGE_MAP ()

UINT GotoEdit::OnGetDlgCode ()
{
    return CEdit::OnGetDlgCode () | DLGC_WANTALLKEYS;
}

void GotoEdit::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_TAB or nChar == VK_RETURN) {
        LedLineItView* v = GetActiveLedItView ();
        if (v != NULL) {
            v->SetFocus ();
        }
    }
    else {
        CEdit::OnKeyDown (nChar, nRepCnt, nFlags);
    }
}

/*
 ********************************************************************************
 ************************ LedLineItMainFrame::StatusBar *************************
 ********************************************************************************
 */
const int kMagicGotoWidgetID = 9991;
using StatusBar              = LedLineItMainFrame::StatusBar;
BEGIN_MESSAGE_MAP (StatusBar, CStatusBar)
ON_EN_CHANGE (kMagicGotoWidgetID, OnMagicEdited)
ON_EN_KILLFOCUS (kMagicGotoWidgetID, OnMagicLoseFocus)
ON_WM_LBUTTONDOWN ()
END_MESSAGE_MAP ()
void StatusBar::OnLButtonDown (UINT nFlags, CPoint oPoint)
{
    CRect r;
    GetItemRect (1, &r);
    if (r.PtInRect (oPoint)) {
        TrackInGotoLineField ();
    }
    else {
        inherited::OnLButtonDown (nFlags, oPoint);
    }
}

void StatusBar::TrackInGotoLineField ()
{
    CRect r;
    GetItemRect (1, &r);
    // make sure plenty wide..
    r.right = max (r.right, r.left + 50);
    if (fGotoEdit.m_hWnd == 0) {
        Verify (fGotoEdit.Create (WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_AUTOHSCROLL | ES_RIGHT, r, this, kMagicGotoWidgetID));
    }
    fGotoEdit.SetFont (GetFont ());
    fGotoEdit.SetWindowText (GetPaneText (1));
    fGotoEdit.SetSel (0, -1);
    fGotoEdit.SetFocus ();
}

void StatusBar::OnMagicEdited ()
{
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    CString wt;
    fGotoEdit.GetWindowText (wt);
    int lines = 0;
    if (::_stscanf (wt, _T ("%d"), &lines) == 1) {
        LedLineItView* v = GetActiveLedItView ();
        if (v != NULL) {
            v->SetCurUserLine (lines);
        }
    }
    else {
        Led_BeepNotify ();
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996)
}

void StatusBar::OnMagicLoseFocus ()
{
    Verify (fGotoEdit.DestroyWindow ());
}

/*
 ********************************************************************************
 ******************************** LedLineItMainFrame ****************************
 ********************************************************************************
 */
IMPLEMENT_DYNCREATE (LedLineItMainFrame, CMDIFrameWnd)
BEGIN_MESSAGE_MAP (LedLineItMainFrame, LedLineItMainFrame::inherited)
ON_WM_CREATE ()
ON_WM_INITMENUPOPUP ()
ON_COMMAND (ID_HELP_FINDER, OnHelpFinder)
ON_COMMAND (ID_HELP, OnHelp)
ON_COMMAND (ID_CONTEXT_HELP, OnContextHelp)
ON_COMMAND (ID_DEFAULT_HELP, OnHelpFinder)
ON_UPDATE_COMMAND_UI (ID_LINENUMBER, OnUpdateLineIndicator)
ON_COMMAND (kCloseWindowCmdID, OnCloseWindowCommand)
ON_COMMAND (kCloseAllWindowsCmdID, OnCloseAllWindowsCommand)
END_MESSAGE_MAP ()

bool LedLineItMainFrame::GetStatusBarShown () const
{
    return !!fStatusBar.IsWindowVisible ();
}

void LedLineItMainFrame::SetStatusBarShown (bool shown)
{
    fStatusBar.ShowWindow (shown ? SW_SHOWNORMAL : SW_HIDE);
}

void LedLineItMainFrame::TrackInGotoLineField ()
{
    Require (GetStatusBarShown ());
    fStatusBar.TrackInGotoLineField ();
}

int LedLineItMainFrame::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
    RequireNotNull (lpCreateStruct);

    if (inherited::OnCreate (lpCreateStruct) == -1) {
        return -1;
    }

    Led_Size desiredSize = Led_Size (
        Led_CvtScreenPixelsFromTWIPSV (Led_TWIPS (1440 * 11)),
        Led_CvtScreenPixelsFromTWIPSH (Led_TWIPS (static_cast<long> (1440 * 8.5))));
    Led_Rect newBounds = Led_Rect (lpCreateStruct->y, lpCreateStruct->x, desiredSize.v, desiredSize.h);
    newBounds          = EnsureRectOnScreen (newBounds);
    MoveWindow (CRect (AsRECT (newBounds)));

    CMenu* menuBar = GetMenu ();
    AssertNotNull (menuBar);
#if qSupportSyntaxColoring
    FixupFontMenu (menuBar->GetSubMenu (2)->GetSubMenu (7));
#else
    FixupFontMenu (menuBar->GetSubMenu (2)->GetSubMenu (6));
#endif

    if (!fToolBar.Create (this) || !fToolBar.LoadToolBar (IDR_MAINFRAME)) {
        TRACE0 ("Failed to create toolbar\n");
        return -1; // fail to create
    }

    static UINT indicators[] = {
        ID_SEPARATOR, // status line indicator
        ID_LINENUMBER,
    };
    if (!fStatusBar.Create (this) || !fStatusBar.SetIndicators (indicators, sizeof (indicators) / sizeof (UINT))) {
        TRACE0 ("Failed to create status bar\n");
        return -1; // fail to create
    }

    // TODO: Remove this if you don't want tool tips or a resizeable toolbar
    fToolBar.SetBarStyle (fToolBar.GetBarStyle () | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

    // TODO: Delete these three lines if you don't want the toolbar to
    //  be dockable
    fToolBar.EnableDocking (CBRS_ALIGN_ANY);
    EnableDocking (CBRS_ALIGN_ANY);
    DockControlBar (&fToolBar);

    return 0;
}

void LedLineItMainFrame::OnUpdateLineIndicator (CCmdUI* pCmdUI)
{
    RequireNotNull (pCmdUI);

    TCHAR buf[1024];
    {
        LedLineItView* v = GetActiveLedItView ();
        if (v == NULL) {
            // can happen, for example, during PrintPreview
            Characters::CString::Copy (buf, NEltsOf (buf), _T ("N/A"));
        }
        else {
            DISABLE_COMPILER_MSC_WARNING_START (4996)
            (void)::_stprintf (buf, _T ("%d"), v->GetCurUserLine ());
            DISABLE_COMPILER_MSC_WARNING_END (4996)
        }
        pCmdUI->SetText (buf);
    }
    pCmdUI->Enable ();

    // dynamicly size the indicator
    HFONT     hFont = (HFONT)fStatusBar.SendMessage (WM_GETFONT);
    CClientDC dcScreen (NULL);
    HGDIOBJ   oldFont = NULL;
    if (hFont != NULL) {
        oldFont = dcScreen.SelectObject (hFont);
    }
    UINT nID     = 0;
    UINT nStyle  = 0;
    int  cxWidth = 0;
    fStatusBar.GetPaneInfo (1, nID, nStyle, cxWidth);
    cxWidth = max (dcScreen.GetTextExtent (buf).cx, 20l);
    fStatusBar.SetPaneInfo (1, nID, nStyle, cxWidth);
    if (oldFont != NULL) {
        (void)dcScreen.SelectObject (oldFont);
    }
}

static bool IsPopupInOwningMenu (HMENU popup, HMENU potentialOwner)
{
    RequireNotNull (popup);
    if (potentialOwner != NULL) {
        int nIndexMax = ::GetMenuItemCount (potentialOwner);
        for (int nIndex = 0; nIndex < nIndexMax; nIndex++) {
            HMENU itsSubMenu = ::GetSubMenu (potentialOwner, nIndex);
            if (itsSubMenu == popup) {
                return true;
            }
            else if (itsSubMenu != NULL) {
                // See if its inside sub menu.
                if (IsPopupInOwningMenu (popup, itsSubMenu)) {
                    return true;
                }
            }
        }
    }
    return false;
}
void LedLineItMainFrame::OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    // Disable inappropriate commands (done by inherited version)
    inherited::OnInitMenuPopup (pPopupMenu, nIndex, bSysMenu);

    // Check if this is our context menu, or the systemmenu, or one of the
    // applications main window menus.
    //  We only do our remove-disabled hack for the context menu...
    if (bSysMenu or IsPopupInOwningMenu (pPopupMenu->m_hMenu, ::GetMenu (m_hWnd))) {
        return;
    }

    // Remove disabled menu items from the popup (to save space).
    // Remove consecutive (or leading) separators as useless.
    CCmdUI state;
    state.m_pSubMenu = NULL;
    state.m_pMenu    = pPopupMenu;
    Assert (state.m_pOther == NULL);
    Assert (state.m_pParentMenu == NULL);

    state.m_nIndexMax = pPopupMenu->GetMenuItemCount ();

    // Remove disabled items (and unneeded separators)
    {
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
}

void LedLineItMainFrame::OnCloseWindowCommand ()
{
    CMDIChildWnd* childFrame = MDIGetActive ();
    if (childFrame != NULL) {
        childFrame->SendMessage (WM_SYSCOMMAND, SC_CLOSE);
    }
}

void LedLineItMainFrame::OnCloseAllWindowsCommand ()
{
    CMDIChildWnd* childFrame = NULL;
    while ((childFrame = MDIGetActive ()) != NULL) {
        CDocument* pDocument = childFrame->GetActiveDocument ();
        if (pDocument != NULL) {
            if (pDocument->SaveModified ()) {
                pDocument->OnCloseDocument ();
                continue;
            }
            else {
                return;
            }
        }
        childFrame->SendMessage (WM_SYSCOMMAND, SC_CLOSE);
    }
}
